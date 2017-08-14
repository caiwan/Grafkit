import bpy
from .bpyobjects import BpyObject

class Material(BpyObject):

    def __init__(self, object):
        BpyObject.__init__(self)
        self.m = object
        pass 
    
    def _dumpobject(self):
        res = self.newobject(self.m)
        
        res["diffuse_intensity"] = self.m.diffuse_intensity
        res["roughness"] = self.m.roughness
        res["specular_intensity"] = self.m.specular_intensity
        res["specular_hardness"] = self.m.specular_hardness
        # res["specular_slope"] = self.m.specular_slope
        res["emit"] = self.m.emit
        res["use_shadeless"] = self.m.use_shadeless
        res["ambient_level"] = self.m.ambient
        
        return res
        
    
    pass