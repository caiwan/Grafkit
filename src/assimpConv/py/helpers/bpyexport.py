import json 
import bpy

"""
Object serializer helpers 
"""

# Actually, we don't really need JSONEncoder here;
class BpyObject(json.JSONEncoder):
    """ Common stuff
    """
    def __init__(self):
        json.JSONEncoder.__init__(self)
        pass
        
        
    def default(self, obj):
        return BpyObject.newobject(obj)
        
    def newobject(self, object):
        ret = {}
        if isinstance(object, bpy.types.ID):
            ret["name"] = object.name.replace(".", "_")
            ret["keys"] = self.getkeys(object)
            ret["class"] = object.__class__.__name__
            if isinstance(object, bpy.types.Object): 
                ret["type"] = object.type
                ret["parent"] = None
                if object.parent:
                    ret["parent"] = object.parent.name.replace(".", "_")
        return ret
    
    def getkeys(self, object, res={}):
        if isinstance(object, bpy.types.bpy_struct):
            for k, v in object.items():
                if not k.startswith("_"):
                    items[k] = v
            return res
        return None
         
    def worldmatrix(self, object, res={}):
        val = {}
        loc, rot, scale = object.matrix_world.decompose()
        val["loc"] = (loc.x , loc.y, loc.z)
        val["rot"] = (rot.w, rot.x, rot.y, rot.z) 
        val["scale"] = (scale.x, scale.y, scale.z)
        res["worldmatrix"] = val
        return res
      
    def localmatrix(self, object, res={}):
        val = {}
        loc, rot, scale = (object.location, object.rotation_quaternion, object.scale)
        val["loc"] = (loc.x , loc.y, loc.z)
        val["rot"] = (rot.w, rot.x, rot.y, rot.z) 
        val["scale"] = (scale.x, scale.y, scale.z)
        res["localmatrix"] = val
        return res

    def eval_animations(self, object, frame):
        k = None
        # https://docs.blender.org/api/blender_python_api_current/bpy.types.AnimData.html#bpy.types.AnimData
        # 
        if object.animation_data:
            k = {}
            anim = object.animation_data
            if anim.action and anim.action.fcurves:
                for track in anim.action.fcurves:
                    p = track.data_path
                    if p not in k:
                        k[p] = [None, None, None, None] 
                    i = track.array_index
                    v = track.evaluate(frame)
                    k[p][i] = v
        return k
        
    pass

    
class Scene (BpyObject):
    def __init__(self):
        BpyObject.__init__(self)
        pass
        
    def default(self, object):
        res = self.newobject(object)
        res["frame_start"] = object.frame_start
        res["frame_end"] = object.frame_end
        res["frame_step"] = object.frame_step
        res["fps"] = object.render.fps
        res["fps_base"] = object.render.fps_base

        return res
    pass
    

class Camera(BpyObject):
    """ Camera exporter class
    """
    
    def __init__(self):
        BpyObject.__init__(self)
        pass 
    
    def default(self, object):
        res = self.newobject(object)
        return res
        

class CameraFrame(BpyObject):
    def __init__(self, object):
        BpyObject.__init__(self)
        pass 
        
    def default(self, object):
        res = self.worldmatrix(object)
        res["angle"] = object.data.angle
        res["dof_distance"] = object.dof_distance
        return res
        
        
class Material(BpyObject):
    def __init__(self):
        BpyObject.__init__(self)
        pass 
    
    def default(self, object):
        res = self.newobject(object)
        res["diffuse_intensity"] = object.diffuse_intensity
        res["roughness"] = object.roughness
        res["specular_intensity"] = object.specular_intensity
        res["specular_hardness"] = object.specular_hardness
        # res["specular_slope"] = object.specular_slope
        res["emit"] = object.emit
        res["use_shadeless"] = object.use_shadeless
        res["ambient_level"] = object.ambient
        
        return res

    pass
        
