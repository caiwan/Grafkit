import sys, os, inspect

import bpy

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

import proba


if __name__ == "__main__":
    args = proba.get_args()
    
    infile = args.input

    bpy.ops.wm.open_mainfile(filepath=infile)
    
    for material in bpy.data.materials:
        keys = material.items()
        for key, value in keys:
            print(value.to_dict())
            for d in dir(value):
                if not d.startswith("_"):
                    print(d)
            for k, v in value.items():
                print("key, value:", k, v.to_dict())
                # for kk, vv in v.items():
                    # print ("kk, vv:", kk, vv)
                    # print(d)
            break
    
    pass