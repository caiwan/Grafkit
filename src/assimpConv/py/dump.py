import bpy
import tempfile
import os


class Bake:
    """
    Helps to bake the animation data before export
    """
    pass


class Dump:
    """
    Export everyhting out of blender
    """
    _cmd_dump = "dump"

    def __init__(self, connection):
        pass

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        pass

    def dump(self, object):
        self._walk(object)
        pass

    def _walk(self, object):
        stack = []
        stack.append(object)
        nameStack = []
        nameStack.append("")

        result = {}
        outnode = result

        while stack:
            node = stack.pop()
            name = nameStack.pop()

            print (node, name)

            c = False
            for d in dir(node):
                if not d.startswith("_"):
                    a = getattr(node, d)
                    if not callable(a):
                        stack.append(a)
                        nameStack.append(d)
                        c = True

            if not c:
                outnode[name] = node
                pass
            else:
                newnode = {}
                outnode [name] = newnode
                pass

        print(result)
        return result


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
