import bpy

"""
Object serializer helpers 
"""

# Actually, we don't really need JSONEncoder here;
class BpyObject():
    """ Common stuff
    """
    def __init__(self, object):
        self.o = object;
        pass
        
    def reprJSON(self, obj):
        return BpyObject.newobject(obj)
        
    def newobject(self):
        ret = {}
        if isinstance(self.o, bpy.types.ID):
            ret["name"] = self.o.name.replace(".", "_")
            ret["keys"] = self.getkeys()
            ret["class"] = self.o.__class__.__name__
            if isinstance(self.o, bpy.types.Object): 
                ret["type"] = self.o.type
                ret["parent"] = None
                if self.o.parent:
                    ret["parent"] = self.o.parent.name.replace(".", "_")
        return ret
    
    def getkeys(self):
        res = {}
        if isinstance(self.o, bpy.types.bpy_struct):
            for k, v in self.o.items():
                if not k.startswith("_"):
                    res[k] = v
            return res
        return None
         
    def worldmatrix(self, res={}):
        val = {}
        loc, rot, scale = self.o.matrix_world.decompose()
        val["loc"] = (loc.x , loc.y, loc.z)
        val["rot"] = (rot.w, rot.x, rot.y, rot.z) 
        val["scale"] = (scale.x, scale.y, scale.z)
        res["worldmatrix"] = val
        return res
      
    def localmatrix(self, res={}):
        val = {}
        loc, rot, scale = (self.o.location, self.o.rotation_quaternion, self.o.scale)
        val["loc"] = (loc.x , loc.y, loc.z)
        val["rot"] = (rot.w, rot.x, rot.y, rot.z) 
        val["scale"] = (scale.x, scale.y, scale.z)
        res["localmatrix"] = val
        return res

    def eval_animations(self, frame):
        k = None
        # https://docs.blender.org/api/blender_python_api_current/bpy.types.AnimData.html#bpy.types.AnimData
        # 
        if self.o.animation_data:
            k = {}
            anim = self.o.animation_data
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
    def __init__(self, scene):
        BpyObject.__init__(self, scene)
        pass
        
    def reprJSON(self):
        res = self.newobject()
        res["frame_start"] = self.o.frame_start
        res["frame_end"] = self.o.frame_end
        res["frame_step"] = self.o.frame_step
        res["fps"] = self.o.render.fps
        res["fps_base"] = self.o.render.fps_base

        return res
    pass
    

class Camera(BpyObject):
    """ Camera exporter class
    """
    
    def __init__(self, camera):
        BpyObject.__init__(self)
        self.c = camera
        pass 
    
    def reprJSON(self):
        object = self.c
        res = self.newobject(object)
        return res
        

class CameraFrame(BpyObject):
    def __init__(self, object):
        BpyObject.__init__(self, object)
        pass 
        
    def reprJSON(self):
        res = self.worldmatrix()
        res["angle"] = self.o.data.angle
        if hasattr(self.o, "dof_distance"):
            res["dof_distance"] = self.o.dof_distance
        return res
        
        
class Material(BpyObject):
    def __init__(self, object):
        BpyObject.__init__(self, object)
        pass 
    
    def reprJSON(self):
        res = self.newobject()
        res["diffuse_intensity"] = self.o.diffuse_intensity
        res["roughness"] = self.o.roughness
        res["specular_intensity"] = self.o.specular_intensity
        res["specular_hardness"] = self.o.specular_hardness
        # res["specular_slope"] = self.o.specular_slope
        res["emit"] = self.o.emit
        res["use_shadeless"] = self.o.use_shadeless
        res["ambient_level"] = self.o.ambient
        
        return res

    pass
        
