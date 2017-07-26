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

from helpers import Dump
from helpers.client import Connection
from helpers.collada import Collada

def get_args():
    parser = argparse.ArgumentParser()

    # get all script args
    _, all_arguments = parser.parse_known_args()
    double_dash_index = all_arguments.index('--')
    script_args = all_arguments[double_dash_index + 1:]

    # add parser rules
    parser.add_argument('-i', '--input', help="input blender file")
    parser.add_argument(
        '-p', '--host', help="host:port we connect to dump data to")
    parsed_script_args, _ = parser.parse_known_args(script_args)

    return parsed_script_args


if __name__ == "__main__":
    args = get_args()
    infile = str(args.input)
    hostaddr = str(args.host)

    with Connection(hostaddr) as conn:
        # open blendfile
        bpy.ops.wm.open_mainfile(filepath=infile)
        
        # bake scene        
        # with Bake as bake:
            # pass
        
        # dump data from scene
        with Dump(conn) as dump:
            dump.add(Collada())

    
        pass
        
