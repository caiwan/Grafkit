import bpy
from .bpyobjects import BpyObject

class Camera(BpyObject):

    def __init__(self, object):
        BpyObject.__init__(self)
        self.c = object
        pass 
    
    def _dumpobject(self):
        res = self.newobject(self.c)
        return res
        
    def dumpframe(self):
        res = {}
        res["angle"] = self.c.data.angle
        
        loc, rot, scale = self.c.matrix_world.decompose()
        res["loc"] = (loc.x , loc.y, loc.z)
        res["rot"] = (rot.w, rot.x, rot.y, rot.z) 
        res["scale"] = (scale.x, scale.y, scale.z)
        return res
        
        
