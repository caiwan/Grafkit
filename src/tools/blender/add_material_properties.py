# quick add pbr texture properties

for material in bpy.data.materials:
    for key in ["metalness", "normal", "roughness", "emission"]:
        if key == "emission" and material.emit < 0.0001:
            continue
        material["mat_{0}".format(key)] = "pbr_{0}_{1}.jpg".format(material.name, key)