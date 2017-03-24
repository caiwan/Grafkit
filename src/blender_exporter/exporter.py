import argparse
import bpy
from subprocess import call


def get_args():
  parser = argparse.ArgumentParser()
 
  # get all script args
  _, all_arguments = parser.parse_known_args()
  double_dash_index = all_arguments.index('--')
  script_args = all_arguments[double_dash_index + 1: ]
 
  # add parser rules
  parser.add_argument('-i', '--input', help="input file")
  parser.add_argument('-o', '--output', help="output file")
  parsed_script_args, _ = parser.parse_known_args(script_args)
  return parsed_script_args
 
if __name__ == "__main__": 
  args = get_args()
  infile = str(args.input)
  outfile= str(args.output)
  
  # open blendfile
  bpy.ops.wm.open_mainfile(filepath=infile)
  
  # if number_of_cubes > 1:
  #   for x in range(0, number_of_cubes):
  #     bpy.ops.mesh.primitive_cube_add(location=(x, 0, 0))

  # bake scene
  # https://docs.blender.org/api/blender_python_api_2_75_0/bpy.ops.nla.html
  
  # export scene to tempfile
  # https://docs.blender.org/api/blender_python_api_current/bpy.ops.wm.html
  bpy.ops.wm.collada_export(\
    filepath=args.save, \
    #apply_modifier=True, \
    triangulate=True, \
    use_texture_copies=True, \
    include_material_textures=True, \
    include_uv_textures=True \
   )

  # export external stuff to tempfile
  # ... 
   
  # call converter tool
  call(["echo", "hello tool"])
  