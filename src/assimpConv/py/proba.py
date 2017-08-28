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

# there is no chance to make __init__.py weork under blender properly, fuck it, damn crap shit    

import tqdm

from helpers.material import Material
from helpers.scene import Scene
from helpers.client import Filedump
from helpers.bpyobjects import BpyObject
    
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
    
    with Filedump("out.json") as d:

        # camera_keys = []
        
        scene = bpy.context.scene

        objects = [obj for obj in scene.objects]
        object_keys = {}
        
        print("num of obj:", len(objects))
        
        for obj in objects:
            res = obj.newobject()
            res["frames"] = []
            object_keys[res["nme"]] = res
        
        bpyobj = BpyObject()
        
        for i in tqdm.trange(scene.frame_start, scene.frame_end, scene.frame_step):
            t = i * (scene.render.fps_base / scene.render.fps)
            
            for object in objects:
                
                # fk https://docs.blender.org/api/2.73a/bpy.types.Object.html#bpy.types.Object
                
                key = {}
                key ["t"] = t
                
                # obj_camera = bpy.context.scene.camera
                
                key ["v"] = object.localmatrix(object)
                # camera_keys.append(key)
                
                object_keys[object.name]["frames"].append(key)
            
        # d.send("CameraMain", camera_keys)
        d.send("ObjectAnimations", object_keys)
		
    pass # main
