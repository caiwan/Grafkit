"""
blender -b -P $(FULL_CURRENT_PATH) -- --input x
"""

import argparse
import sys, os, inspect
import json

import bpy

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

from helpers import client
from helpers import bpyexport
    
def get_args():
    parser = argparse.ArgumentParser()

    # get all script args
    _, all_arguments = parser.parse_known_args()
    double_dash_index = all_arguments.index('--')
    script_args = all_arguments[double_dash_index + 1:]

    # add parser rules
    parser.add_argument('-i', '--input', help="input blender file")
    parsed_script_args, _ = parser.parse_known_args(script_args)

    return parsed_script_args


if __name__ == "__main__":
    args = get_args()
    infile = str(args.input)
    
    bpy.ops.wm.open_mainfile(filepath=infile)
    
    with client.Filedump("out.json") as d:

        # camera_keys = []
        
        scene = bpy.context.scene

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
		
    pass # main
    