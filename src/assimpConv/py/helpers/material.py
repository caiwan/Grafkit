import bpy

"""
Material.diffuse_intensity
Material.roughness				- Oren-Nayar  hasznalja csak

Material.specular_intensity
Material.specular_hardness
Material.specular_slope

Material.emit
Material.use_shadeless 			- flat gyk.

Material.ambient 				- global ambient merteke

Kesobbiekben a colorramppel is lehet kezdeni valamit specular es diffuse oldalon:
Material.use_diffuse_ramp
Material.use_specular_ramp

"""


from . import Dumpable


class Material:

   def __init__(self, object):
        self.m.object
        pass

    def __enter__(self):
        res = {}

        res["diffuse_intensity"] = self.m.diffuse_intensity
        res["roughness"] = self.m.roughness
        res["specular_intensity"] = self.m.specular_intensity
        res["specular_hardness"] = self.m.specular_hardness
        res["specular_slope"] = self.m.specular_slope
        res["emit"] = self.m.emit
        res["use_shadeless"] = self.m.use_shadeless
        res["ambient_level"] = self.m.ambient
        
        return res
    
    def __exit__(self):
        pass
        
    
    pass