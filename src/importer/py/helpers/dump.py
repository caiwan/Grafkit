import os
import sys
import inspect

import bpy

cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

from . import camel2snake
from . import collada

from . import bpyexport

scene = bpy.context.scene

def do_dump_camera_frames(conn):
    """ Helps us to get which camera is active on the right scene"""
    camera_keys = []
    
    lastkey = ""
    
    for i in range(scene.frame_start - 1, scene.frame_end, scene.frame_step):
        t = i * (scene.render.fps_base / scene.render.fps)
        scene.frame_set(i)
        scene.update()
        camera = scene.camera
        
        if camera.name != lastkey:
            lastkey = camera.name
            camera_keys.append({"v":{"key":camera.name.replace(".", "_")}, "t":t})
        
    conn.send("camera", {"CameraKeys": camera_keys})
    
    pass
    
    
def do_dump_models(conn):
    for obj in scene.objects:
        scene.objects.active = obj
        scene.update()
        
        obj.calc_normals();
        
        for material in obj.materials:
            pass
            
        for polygons in obj.polygons:
            pass
        
        for uv in obj.uv_textures:
            pass
        
        for color in obj.vertex_colors:
            pass
        
    pass
    
    
def do_dump(conn):
    """ Dumps all the data from the context """
    scene = bpy.context.scene
    
    conn.send("collada", collada.InternalExporter())
    conn.send("scene", {"Scene": bpyexport.Scene(scene)})
    
    materials = [bpyexport.Material(material) for material in bpy.data.materials]
    conn.send("material", {"Materials": materials})
    
    do_dump_camera_frames(conn)
    
    pass #shit