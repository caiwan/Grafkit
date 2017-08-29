import bpy
import tempfile

import os
import sys
import inspect

from . import Dumpable

class Collada(Dumpable):
    """
    Export scene to a tempfile as collada, then push the data to the server
    """
    _cmd = "collada"

    def __init__(self):
        Dumpable.__init__(self)
        
        self.triangulate = True
        self.use_texture_copies = False
        self.include_material_textures = True
        self.include_uv_textures = True

        pass #ctor

    def reprJSON(self):
        self.tmp = tempfile.NamedTemporaryFile(delete=False)
        self.tmp.close()
        os.unlink(self.tmp.name)

        self._save_collada()

        daefile = self.tmp.name + ".dae"
        
        dae = ""
        
        if os.path.exists(daefile):
            with open(daefile) as f:
                print("Reading dae")
                dae = f.read()
            os.unlink(daefile) 

        return dae
        pass # dump dae
        
    def get_cmd(self):
        return self._cmd
        
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
