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
from helpers.bake import Bake
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
    
    bake = Bake()
    bake.bake_material_textures()
    bake.bake_animations()
    
    
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