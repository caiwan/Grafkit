import bpy

import os
import sys
import inspect
import json
import re

scene = bpy.context.scene
ops = bpy.ops

# http://danielhnyk.cz/creating-animation-blender-using-python/

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)
    

def _cleanup_scene():
    for object in scene.objects:
        scene.objects.unlink(object)
    
    
def _add_camera(name, pos=(0,0,0), euler=(0,0,0), lens=10):
    cam_data = bpy.data.cameras.new(name=name + "_obj")  
    cam_ob = bpy.data.objects.new(name=name, object_data=cam_data)  
    scene.objects.link(cam_ob)
    cam_ob.location = pos  
    cam_ob.rotation_euler = euler  
    cam = bpy.data.cameras[cam_data.name]  
    cam.lens = lens
    return cam_ob
    
    
def _add_light(name):
    lamp_data = bpy.data.lamps.new(name=name + "_obj", type='POINT')  
    lamp_object = bpy.data.objects.new(name=name, object_data=lamp_data)  
    scene.objects.link(lamp_object)  
    lamp_object.location = (-3, 0, 12)
    return lamp_object
    
    
def _add_cube(name, x=0, y=0, z=0):
    bpy.ops.mesh.primitive_cube_add(location=(x,y,z))  
    obj =  bpy.context.object
    obj.name = name
    return obj
    
    

    