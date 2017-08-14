"""
QnD Scene exporter stuff
"""

import bpy
from .bpyobjects import BpyObject

class Scene (BpyObject):
    """
    helps to bake the animation data before export
	
	bpy.context.scene.frame_start
	bpy.context.scene.frame_end
	bpy.context.scene.frame_step
	bpy.context.scene.render.fps = 60
	bpy.context.scene.render.fps_base = 1
	
    """

    def __init__(self, object):
        BpyObject.__init__(self)
        self.s = object
        pass
        
    def _dumpobject(self):
        res = {}
        res["frame_start"] = self.s.frame_start
        res["frame_end"] = self.s.frame_end
        res["frame_step"] = self.s.frame_step
        res["fps"] = self.s.render.fps
        res["fps_base"] = self.s.render.fps_base

        return res
    pass

    
    