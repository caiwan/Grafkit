import bpy
import unittest


class Dump:

    def __init__(self):
        pass
        
    #
    _skip_data_types = ["bl_rna", "rna_type", "translation_context", "use_autopack", "is_dirty", "is_saved", "is_updated", "window_managers", "cache_files"]
    _base_types = ["bool", "int", "str", "tuple", "list", "dict", "float"]
        
    #
    def __enter__(self):        
        return self

    def __exit__(self, type, value, traceback):
        pass

    #    
    def dump(self, object):
        return self._walk_object(object)
        pass

        
    #    
    def _walk_vector(self, object):
        pass
        
    def _walk_bpy_collection(self, object):
        pass
        
    def _walk_object(self, object):
        stack = []
        name_stack = []
    
        stack.append(object)
        name_stack.append("data")
        
        result = {}
        outnode = result

        while stack:
            node = stack.pop()
            name = name_stack.pop()
            
            #
            if name in self._skip_data_types:
                continue

            clazz = node.__class__.__name__
            
            print(name, clazz, node)

            #
            if clazz in self._base_types:
                if isinstance(outnode, dict):
                    outnode[name] = node
                elif isinstance(outnode, list):
                    outnode.append(node)
                continue
                
            elif clazz in ["Vector"]:
                continue
                
            elif clazz in ["bpy_prop_collection"] and False:
                newnode = []
                if isinstance(outnode, dict):
                    outnode[name] = newnode
                elif isinstance(outnode, list):
                    outnode.append(newnode)
                outnode = newnode

                for k in node:
                    stack.append(k)
                    name_stack.append(None)

                pass #elif
                
            else:
                newnode = {}
                if isinstance(outnode, dict):
                    outnode[name] = newnode
                elif isinstance(outnode, list):
                    outnode.append(newnode)
                outnode = newnode
                
                pass #else

            for d in dir(node):
                if not d.startswith("_"):
                    try:
                        a = getattr(node, d)
                        if not callable(a):
                            stack.append(a)
                            name_stack.append(d)
                    except AttributeError as e :
                        pass
                        
                    pass #if
                pass #for
                
            pass #while

        # print(result)
        return result


if __name__ == "__main__":
    with Dump() as d:
        data = d.dump(bpy.data)
        print(data)
    input()
