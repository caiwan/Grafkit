import os
import sys
import inspect
import argparse
import bpy
from subprocess import call

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

# there is no chance to make __init__.py weork under blender properly, fuck it, damn crap shit    

from helpers import client
from helpers.collada import Collada
from helpers import bpyexport

def get_args():
    parser = argparse.ArgumentParser()

    # get all script args
    _, all_arguments = parser.parse_known_args()
    double_dash_index = all_arguments.index('--')
    script_args = all_arguments[double_dash_index + 1:]

    # add parser rules
    parser.add_argument('-i', '--input', help="input blender file")
    parser.add_argument('-t', '--test', help="dumps out stuff to json")
    parser.add_argument('-p', '--host', help="host:port we connect to dump data to")
    parsed_script_args, _ = parser.parse_known_args(script_args)

    return parsed_script_args

    
def dump_shitz_per_frmae(conn, scene):
    """ This crappy fuckshit seems does not work at all
    """
    camera_keys = []
    
    objects = [obj for obj in scene.objects if obj.type in ["MESH", "EMPTY", "LIGHT"]]

    object_keys = [bpyexport.BpyObject(obj).newobject() for obj in objects]
    for i in range(len(objects)):
        object_keys[i].update(bpyexport.BpyObject(objects[i]).localmatrix()) #bazdmeg
    
    for i in range(scene.frame_start - 1, scene.frame_end, scene.frame_step):
        t = i * (scene.render.fps_base / scene.render.fps)
        
        print("Baking frame {}".format(i))
        
        camera = scene.camera
        scene.frame_set(i)
        
        bpy.ops.object.paths_calculate()
        scene.update()
        
        v = bpyexport.CameraFrame(camera).reprJSON()
        key = {"v":v, "t":t}
        camera_keys.append(key)
        
        for i in range(len(objects)):
            object = objects[i]
            v = {}
            v["actions"] = bpyexport.BpyObject(object).eval_animations(i) 
            v.update(bpyexport.BpyObject(object).localmatrix())
            if v:
                key = {"v":v, "t":t}
               
                if not "frames" in object_keys[i]:
                    object_keys[i]["frames"] = []
                
                object_keys[i]["frames"].append(key)
        
    conn.send("bpydump", {"ObjectAnimations": object_keys})
    conn.send("bpydump", {"MainCameraMovement" : camera_keys})
    
    
def do_dump(conn):
    scene = bpy.context.scene
    
    conn.send("collada", Collada())
    conn.send("bpydump", {"Scene": bpyexport.Scene(scene)})
    
    materials = [bpyexport.Material(material) for material in bpy.data.materials]
    conn.send("bpydump", {"Materials": materials})
    
    # dump_shitz_per_frmae(conn, scene)
    pass #shit


def do_bake():
    """ This thing tries to bake all the object animations
        and camera movement into a single track
        
        Drawback: it eleminates scenegraph hierarchy and flattens everzthing 
        
        This suposed to be it, but fingers crossed:
        https://wiki.blender.org/index.php/Dev:2.4/Source/Animation/AnimationBaking
        https://docs.blender.org/api/blender_python_api_2_72_1/bpy.ops.nla.html#bpy.ops.nla.bake
    """ 
    scene = bpy.context.scene
    
    # needs to be iterated 
    # due it missses the other children in scenegraph
    # otherwise it bakes only a random child
    for obj in scene.objects:
        scene.objects.active = obj
        scene.update()
        
        # this would be nice too 
        # for modifier in obj.modifiers:
        #   modifier.apply()
    
        print("Baking object {}".format(obj.name))
    
        # visual keying = bakes constraints
        # aside that, parenting and constraints should be cleared
        bpy.ops.nla.bake(\
            frame_start=scene.frame_start,\
            frame_end=scene.frame_end,\
            step=scene.frame_step,\
            only_selected=True,\
            visual_keying=True,\
            clear_constraints=True,\
            clear_parents=True,\
            bake_types={'OBJECT'}\
        )
    
    
# Main
if __name__ == "__main__":
    args = get_args()
    infile = args.input

    # open
    bpy.ops.wm.open_mainfile(filepath=infile)
    
    # bake
    do_bake()
    
    # hope 
    outfile = args.test
    if outfile:
        print("--- DUMPS TO FILE ---")
        print("Output: {}".format(outfile))
        with client.Filedump(outfile) as conn:
            do_dump(conn)
    else:
        hostaddr = str(args.host)
        with client.Connection(hostaddr) as conn:
            do_dump(conn)