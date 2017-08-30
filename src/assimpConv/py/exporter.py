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

from helpers.client import Connection
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
    parser.add_argument('-p', '--host', help="host:port we connect to dump data to")
    parsed_script_args, _ = parser.parse_known_args(script_args)

    return parsed_script_args


if __name__ == "__main__":
    args = get_args()
    infile = str(args.input)
    hostaddr = str(args.host)

    # open blendfile
    bpy.ops.wm.open_mainfile(filepath=infile)
    
    # bake stuff 
    # ... 
    
    with Connection(hostaddr) as conn:

        scene = bpy.context.scene
    
        conn.send("collada", Collada())
        conn.send("bpydump", {"Scene": bpyexport.Scene(scene)})
        
        materials = [bpyexport.Material(material) for material in bpy.data.materials]
        conn.send("bpydump", {"Materials": materials})
    
        camera_keys = []
        
        objects = [obj for obj in scene.objects if obj.type in ["MESH", "EMPTY", "LIGHT"]]
        
        # object_keys = {name: bpyexport.BpyObject(obj).newobject() for name, obj in objects}
        object_keys = [bpyexport.BpyObject(obj).newobject() for obj in objects]
        for i in range(len(objects)):
            object_keys[i].update(bpyexport.BpyObject(objects[i]).localmatrix()) #bazdmeg
        
        for i in range(scene.frame_start - 1, scene.frame_end, scene.frame_step):
            t = i * (scene.render.fps_base / scene.render.fps)
            
            print("Baking frame {}".format(i))
            
            camera = scene.camera
            scene.frame_set(i)
            
            v = bpyexport.CameraFrame(camera).reprJSON()
            key = {"v":v, "t":t}
            camera_keys.append(key)
            
            for i in range(len(objects)):
                object = objects[i]
                v =  bpyexport.BpyObject(object).eval_animations(i) 
                if v:
                    key = {"v":v, "t":t}
                   
                    if not "frames" in object_keys[i]:
                        object_keys[i]["frames"] = []
                    
                    object_keys[i]["frames"].append(key)
            
        conn.send("bpydump", {"ObjectAnimations": object_keys})
        conn.send("bpydump", {"MainCameraMovement" : camera_keys})
        
        pass
        
