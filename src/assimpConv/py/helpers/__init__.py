import os
import sys
import inspect

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)


class Dumpable:
    """ Dumpable base class
    """
    
    def __init__(self):
        pass
        
    def dump(self):
        """Returns the serialized object data"""
        return ""
        pass
        
    def get_cmd(self):
        """ Retruns the command key of the object"""
        return ""
        pass
        
    pass

class Dump:
    """ Network dump framework class
    """
    
    def __init__(self, conn):
        self._dumps = []
        self._conn = conn;
        pass #ctor
        
    def __enter__(self):
        return self
        pass
        
    def add(self, obj):
        if isinstance(obj, Dumpable):
            self._dumps.append(obj)
        else:
            raise TypeError("Object is not Dumpable")
        
        pass # dump
        
    def __exit__(self, type, value, traceback):
        for obj in self._dumps:
            o = obj.dump()
            c = obj.get_cmd()
            self._conn.send(c, o)
        pass
    


class DumapbleObject (Dumpable):
    """ Recursive Object walker framework for bpy objects
    """

    def __init__(self, obj, cmd="object"):
        self._obj = obj
        self._cmd = cmd
        self._obj_set = set()
        pass
        
        
    def dump(self):
        return self._walk_object(self._obj)
        
        
    def get_cmd():
        return self._cmd
        
    #
    _skip_data_types = ["bl_rna", "rna_type", "translation_context", "use_autopack", "is_dirty", "is_saved", "is_updated", "window_managers", "cache_files"]
    _base_types = ["bool", "int", "str", "float"]
    _iter_types = ["tuple", "list", "bpy_prop_collection"]
    _map_types = ["dict"]
        
    
    def _walk_vector(self, object):
        return { 'x':0, 'y':0, 'z':0, 'w':0 }

       
    def _walk_collection(self, object):
        res = []
        for k in object:
            res.append(self._walk_object(k))
        return res
        
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
            print(name, clazz, node, id(node))

            #
            if clazz in self._base_types:
                outnode[name] = node
                continue
                
            elif clazz in ["Vector"]:
                nnode = self._walk_vector(node)
                outnode[name] = nnode
                continue
                
            elif clazz in self._iter_types :
                nnode = self._walk_collection(node)
                outnode[name] = nnode
                continue
                
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
                            # ez vicceskedik itten
                            # ki kellene szurni a fabol a rekurziot
                            
                            try:
                                if id(a) in self._obj_set:
                                    continue
                                else:
                                    self._obj_set.add(id(a))
                            except TypeError:
                                pass
            
                            stack.append(a)
                            name_stack.append(d)
                            
                    except AttributeError as e :
                        pass
                        
                    pass #if
                pass #for
                
            pass #while

        return result["data"]


# ---
class Bakeable:
	pass
	
	
class Bake:
	pass
	
	
# ---
class ContextWalker:
	pass
