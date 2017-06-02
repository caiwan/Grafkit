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

import dump
    
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
    
    d = dump.Dump()
    with open("hello.json", "w") as fp:
        data = d.dump(bpy.data.actions)
        json.dump(data, fp, indent=4, sort_keys=True)
    pass # main
