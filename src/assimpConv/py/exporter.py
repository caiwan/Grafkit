import os
import sys
import inspect
import argparse
import bpy
import tqdm
from subprocess import call

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

# there is no chance to make __init__.py weork under blender properly, fuck it, damn crap shit    

from helpers import Dump
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
        
        # dump data from scene
        # should be rewritten later
        with dump(conn) as d:
            d.add(collada())
            
        conn.send("bpydump", {"Scene", bpyexport.Scene(scene)})
        
        materials = [bpzexport().Scene().default()]
        for material in bpy.data.materials:
            # with Material(material) as m:
                # materials.append(m)
            # pass
        # conn.send("bpydump", {"Materials": materials})
        # pass
        
        # camera_keys = []
        
        

        # List of objects to be baked
        # Cameras will be done in a different way
        objects = {(obj.name.replace(".", "_"), obj) for obj in scene.objects if obj.type in ["MESH", "EMPTY", "LIGHT"]}
        
        object_keys = {name: bpyexport.BpyObject().newobject(obj) for name, obj in objects}
        
        print("Objects to be baked ({}) : [{}]".format(len(objects), ", ".join([n for n, _ in objects])))
        
        for i in range(scene.frame_start - 1, scene.frame_end, scene.frame_step):
            t = i * (scene.render.fps_base / scene.render.fps)
            
            print("Baking frame {}".format(i))
            
            for name, object in objects:
                v =  bpyexport.BpyObject().eval_animations(object, i) 
                if v:
                    key = {"v":v, "t":t}
                   
                    if not "frames" in object_keys[name]:
                        object_keys[name]["frames"] = []
                    
                    object_keys[name]["frames"].append(key)
            
        d.send("ObjectAnimations", object_keys)
        
        
        
        # for i in trange(scene["frame_start"], scene["frame_end"], scene["frame_step"]):
            # t = i * (scene["fps_base"] / scene["fps"])
            # bpy.context.scene.frame_set(i)
            # c = Camera(bpy.context.scene.camera)
            
            # print("Rendering camera at frame", i)
            
            # key = {}
            # key ["t"] = t
            # key ["v"] = c.dumpframe()
            # camera_keys.append(key)
            
            # # export everz single objects position
            
            
        # conn.send("bpydump", {"MainCameraMovement" : camera_keys})
            
    
        pass
        
