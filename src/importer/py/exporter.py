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

from helpers import do_bake, do_dump
from helpers import client

try:
    from tests import build_test_scene
except ImportError:
    pass

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

    
# Main
if __name__ == "__main__":
    args = get_args()
    
    if args.input:
        print("=== READING FILE {}".format(infile))
        sys.stdout.flush()
        infile = args.input
        bpy.ops.wm.open_mainfile(filepath=infile)
    else:
        print("=== BUILD TEST SCENE ===")
        build_test_scene()
        pass
    
    do_bake()
    
    hostaddr = str(args.host)
    with client.Connection(hostaddr) as conn:
        do_dump(conn)
