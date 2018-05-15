import os
import sys
import inspect

import bpy
import bmesh

cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

from . import camel2snake

scene = bpy.context.scene
    
def add_material_names_as_pbr_textures():
    # quick add pbr texture properties
    for material in bpy.data.materials:
        for key in ["BaseColor", "Metallic", "Normal", "Roughness", "Emission"]:
            if key == "Emission" and material.emit < 0.0001:
                continue
            material["mat_{0}".format(camel2snake(key))] = "pbr_{0}_{1}.jpg".format(material.name, key)    


def bake_animations():
    pass
    
    
def triangulate_object(obj):
    me = obj.data

    bm = bmesh.new()
    bm.from_mesh(me)

    bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)

    bm.to_mesh(me)
    bm.free()
    
def bake_triangulate():
    for obj in scene.objects:
        scene.objects.active = obj
        scene.update()
        
        triangulate_object(obj)

            
def bake_flat_animations():
    # needs to be iterated 
    # due it missses the other children in scenegraph
    # otherwise it bakes only a random child
    for obj in scene.objects:
        scene.objects.active = obj
        scene.update()
        
        # this would be nice too 
        # for modifier in obj.modifiers:
        #   modifier.apply()
    
        print("Baking object {}".format(obj.name))
        sys.stdout.flush()
    
        # visual keying = bakes constraints
        # aside that, parenting and constraints should be cleared
        bpy.ops.nla.bake(\
            frame_start=scene.frame_start,\
            frame_end=scene.frame_end,\
            step=scene.frame_step,\
            only_selected=True,\
            visual_keying=True,\
            clear_constraints=True,\
            clear_parents=True,\
            bake_types={'OBJECT'}\
    )
    
    
def do_bake():
    """ This thing tries to bake all the object animations
        and camera movement into a single track
        
        Drawback: it eleminates scenegraph hierarchy and flattens everzthing 
        
        This suposed to be it, but fingers crossed:
        https://wiki.blender.org/index.php/Dev:2.4/Source/Animation/AnimationBaking
        https://docs.blender.org/api/blender_python_api_2_72_1/bpy.ops.nla.html#bpy.ops.nla.bake
    """ 
    add_material_names_as_pbr_textures()
    # bake_triangulate()
    # bake_animations()
    bake_flat_animations()
    
