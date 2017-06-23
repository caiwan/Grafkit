import bpy
import tempfile
import os

class Bake:
    """
    Helps to bake the animation data before export
    """

    def __init__(self):
        pass
        
    def bake(self):
        # bake scene
        # https://docs.blender.org/api/blender_python_api_2_75_0/bpy.ops.nla.html
        bpy.ops.nla.bake(
            frame_start=scene.frame_start,
            frame_end=scene.frame_end,
            step=scene.frame_step,
            only_selected=False,
            visual_keying=False,
            clear_constraints=True,
            clear_parents=False,
            use_current_action=False,
            bake_types={'POSE', 'OBJECT'}
        )
        
        pass
    
    pass
