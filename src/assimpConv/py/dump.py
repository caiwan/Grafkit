import bpy
import tempfile
import os

class Bake:
    """
    Helps to bake the animation data before export
    """
    pass


    
class Dump:

    def __init__(self):
        self._obj_set = set()
        pass
        
    #
    _skip_data_types = ["bl_rna", "rna_type", "translation_context", "use_autopack", "is_dirty", "is_saved", "is_updated", "window_managers", "cache_files"]
    _base_types = ["bool", "int", "str", "float"]
    _iter_types = ["tuple", "list", "bpy_prop_collection"]
    _map_types = ["dict"]
        

    #    
    def dump(self, object):
        return self._walk_object(object)
        pass
        
    #  
    
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

            
            # ez vicceskedik itten
            # ki kellene szurni a fabol a rekurziot
            try:
                if id(node) in self._obj_set:
                    print("skipping", name, clazz, node)
                    continue
                else:
                    self._obj_set.add(id(node))
            except TypeError:
                pass
            
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
                            stack.append(a)
                            name_stack.append(d)
                    except AttributeError as e :
                        pass
                        
                    pass #if
                pass #for
                
            pass #while

        return result["data"]


class Collada:
    """
    Export scene to a tempfile as collada, then push the data to the server
    """

    _cmd_dae = "collada"

    def __init__(self, connection):
        self.c = connection

        self.triangulate = True
        self.use_texture_copies = False
        self.include_material_textures = True
        self.include_uv_textures = True

        pass

    def _save_collada(self):
        # https://docs.blender.org/api/blender_python_api_current/bpy.ops.wm.html
        bpy.ops.wm.collada_export(
            filepath=self.tmp.name, \
            #apply_modifier=True, \
            triangulate=self.triangulate, \
            use_texture_copies=self.use_texture_copies, \
            include_material_textures=self.include_material_textures, \
            include_uv_textures=self.include_uv_textures \
        )
        pass

    def __enter__(self):
        self.tmp = tempfile.NamedTemporaryFile(delete=False)
        self.tmp.close()
        os.unlink(self.tmp.name)

        self._save_collada()

        return self

    def __exit__(self, type, value, traceback):
        daefile = self.tmp.name + ".dae"
        if os.path.exists(daefile):
            with open(daefile) as f:
                print("Reading dae")
                self.c.send(self._cmd_dae, f.read())
            os.unlink(daefile)
