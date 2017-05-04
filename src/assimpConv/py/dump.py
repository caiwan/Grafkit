import bpy

def hello(scene):
  print("hello blender: " + scene.name)

  l = dir(scene)
  for k in l:
    print(k)