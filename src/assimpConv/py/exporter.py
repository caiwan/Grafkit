import os, sys, inspect
import argparse
import bpy
from subprocess import call

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile( inspect.currentframe() ))[0]))
if cmd_folder not in sys.path:
     sys.path.insert(0, cmd_folder)

import client

def get_args():
  parser = argparse.ArgumentParser()
 
  # get all script args
  _, all_arguments = parser.parse_known_args()
  double_dash_index = all_arguments.index('--')
  script_args = all_arguments[double_dash_index + 1: ]
 
  # add parser rules
  parser.add_argument('-i', '--input', help="input blender file")
  parser.add_argument('-p', '--host', help="host:port we connect to dump data to")
  parsed_script_args, _ = parser.parse_known_args(script_args)
 
  return parsed_script_args
 
if __name__ == "__main__": 
  args = get_args()
  infile = str(args.input)
  hostaddr = str(args.host)

  with client.Connection(hostaddr) as conn:
    # open blendfile
    bpy.ops.wm.open_mainfile(filepath=infile)
    
    # https://docs.blender.org/api/blender_python_api_current/bpy.types.Scene.html#bpy.types.Scene
    scene = bpy.data.scenes['Scene']
    
    # bake scene
    # https://docs.blender.org/api/blender_python_api_2_75_0/bpy.ops.nla.html
    bpy.ops.nla.bake(\
      frame_start=scene.frame_start, \
      frame_end=scene.frame_end, \
      step=scene.frame_step, \
      only_selected=False,\
      visual_keying=False,\
      clear_constraints=True,\
      clear_parents=False,\
      use_current_action=False, 
      bake_types={'POSE', 'OBJECT'}\
    )
    
    # export scene to tempfile
    # https://docs.blender.org/api/blender_python_api_current/bpy.ops.wm.html
    bpy.ops.wm.collada_export(\
      filepath=args.output, \
      #apply_modifier=True, \
      triangulate=True, \
      use_texture_copies=True, \
      include_material_textures=True, \
      include_uv_textures=True \
     )
  
    # export external stuff to tempfile
    # ... 
    dump.hello(scene)
     
    # call converter tool
    # call(["echo", "hello tool"])
