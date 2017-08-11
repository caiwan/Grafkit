
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
            return object.items()
                
        return None
        
    pass


