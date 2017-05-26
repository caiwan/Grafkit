import sys, os, inspect
import json

import bpy

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

import dump

"""
blender -b -P $(FULL_CURRENT_PATH)
"""

if __name__ == "__main__":
    d = dump.Dump()
    with open("hello.json", "w") as fp:
        data = d.dump(bpy.data.scenes)
        json.dump(data, fp, indent=4, sort_keys=True)
    pass # main
