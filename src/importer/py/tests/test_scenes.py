from . import *

def build_test_scene_1():
    cleanup_scene()
    
    _add_cube("testcube")
    _add_camera("testcamera")
    _add_light("testlight")
    