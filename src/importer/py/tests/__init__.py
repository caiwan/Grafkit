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
    

def cleanup_scene():
    for object in scene.objects:
        scene.objects.unlink(object)
    
    
def add_camera():
    cam_data = bpy.data.cameras.new(name="Camera")  
    cam_ob = bpy.data.objects.new(name="Camera", object_data=cam_data)  
    scene.objects.link(cam_ob)  
    cam_ob.location = (-3, 0, 5)  
    cam_ob.rotation_euler = (3.14/6,0,-0.3)  
    cam = bpy.data.cameras[cam_data.name]  
    cam.lens = 10
    return cam_ob
    
    
def add_light():
    lamp_data = bpy.data.lamps.new(name="lampa", type='POINT')  
    lamp_object = bpy.data.objects.new(name="Lampicka", object_data=lamp_data)  
    scene.objects.link(lamp_object)  
    lamp_object.location = (-3, 0, 12)
    return lamp_object
    
    
def add_cube(x=0, y=0, z=0):
    bpy.ops.mesh.primitive_cube_add(location = (x,y,z))  
    return bpy.context.object
    
    
def build_test_scene():
    cleanup_scene()
    
    cube = add_cube()
    
    # add animation
    