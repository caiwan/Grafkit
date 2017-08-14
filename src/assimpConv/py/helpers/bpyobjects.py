
import bpy

"""
Object helper
"""

class BpyObject:

    def __init__(self):
        pass
        
    def newobject(self, object):
        ret = {}
        if isinstance(object, bpy.types.ID):
            ret["name"] = object.name
            ret["keys"] = self.getkeys(object)
        return ret
    
    
    # https://docs.blender.org/api/blender_python_api_2_67_1/bpy.props.html
    def getkeys(self, object):
        if isinstance(object, bpy.types.bpy_struct):
            items = {}
            for k, v in object.items():
                if not k.startswith("_"):
                    items[k] = v
            return items
        return None
        
        
    def __enter__(self):    
        return self._dumpobject()
        # return self
        
        
    def __exit__(self, a, b, c):
        if b:
            raise b 
        pass
        
        
    pass


