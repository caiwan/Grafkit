import os
import sys
import inspect

import bpy

cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

from . import camel2snake
    
class Bake:
    """
    Helps to bake the animation data before export
    """

    def __init__(self):
        pass
        
    def bake_material_textures(self):
        # quick add pbr texture properties
        for material in bpy.data.materials:
            for key in ["BaseColor", "Metalness", "Normal", "Roughness", "Emission"]:
                if key == "Emission" and material.emit < 0.0001:
                    continue
                material["mat_{0}".format(camel2snake(key))] = "pbr_{0}_{1}.jpg".format(material.name, key)    

                
    def bake_animations(self):
        scene = bpy.context.scene
    
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