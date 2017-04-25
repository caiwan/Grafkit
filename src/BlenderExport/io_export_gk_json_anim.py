"""
TBD

Track id (index) and binding
---
Path            Index   Bindng
location        0       location.x
location        1       location.y
location        2       location.z
rotation_euler  0       rotation_euler.x   # if XYZ euler was selected -> todo: determine which one
rotation_euler  1       rotation_euler.y
rotation_euler  2       rotation_euler.z
scale           0       scale.x
scale           1       scale.y
scale           2       scale.z

"""

from json import JSONEncoder

import bpy
from bpy.props import BoolProperty, IntProperty, EnumProperty
from bpy_extras.io_utils import ExportHelper

__author__ = "Peter Sari (Caiwan)"

_DEBUG_MODE = True

bl_info = {
    "name": "Export Animation data for TimeMachine engine (.json)",
    "author": __author__,
    "version": (0, 1),
    "blender": (2, 71, 0),
    "location": "File > Export > Constraints for TimeMachine (.json)",
    "description": "Export constraint and path animation data to import them into TimeMachine(.json)",
    "warning": "Work in progress",
    "wiki_url": "TBD"
                "Scripts/Import-Export/JSON_TimeMachine_anim_export",
    "category": "Import-Export"}


class Entity(object):
    def serialize(self):
        raise NotImplementedError


class MyEncoder(JSONEncoder):
    def default(self, o):
        if isinstance(o, Entity):
            return o.serialize()
        return super(MyEncoder, self).default(o)


class ControlCurve(Entity):
    path = ""

    def __init__(self, key, fcurve):
        assert isinstance(fcurve, bpy.types.FCurve)
        self.index = fcurve.array_index
        self.path = fcurve.data_path
        self.key = key

        self.track = []
        for cp in fcurve.keyframe_points:
            self.track.append({
                "t": cp.co[0],  # frame#
                "v": cp.co[1]  # value
            })

            # self.value = fcurve.get(0)

    def serialize(self):
        return self.__dict__


class AnimationEntity(Entity):
    name = ""
    curves = []

    def __init__(self, name, anim_data):
        assert isinstance(anim_data, bpy.types.AnimData)

        self.parent = name
        self.name = anim_data.action.name
        self.range = [anim_data.action.frame_range[0], anim_data.action.frame_range[1]]

        for key, elem in anim_data.action.fcurves.items():
            assert isinstance(elem, bpy.types.FCurve)

            keyframe = ControlCurve(key, elem)
            self.curves.append(keyframe)

    def serialize(self):
        res = self.__dict__
        res.update({
            "tracks": self.curves
        })
        return res


class ConstraintEntity(Entity):
    def __init__(self, name, constraint):
        self.target_name = name
        self.name = constraint.name
        self.track_axis = constraint.track_axis

    def serialize(self):
        return self.__dict__

# ==================================================================================================================


class CurveEntity(AnimationEntity):
    def __init__(self, name, curve):
        AnimationEntity.__init__(self, name, curve.animation_data)

        assert isinstance(curve, bpy.types.Curve)

        self.duration = curve.path_duration

    def serialize(self):
        return self.__dict__


class SplineEntity(Entity):
    def __init__(self, spline):
        pass

    def serialize(self):
        return self.__dict__


# ==================================================================================================================


class Export_gkjson(bpy.types.Operator, ExportHelper):
    """Export animation data from the entire scene
    """

    bl_idname = "export_anim.gjson"
    bl_label = "Export Grafkit Animation"

    bl_options = {'PRESET'}

    filename_ext = ".json"

    use_pretty_print = BoolProperty(
            name="Pretty print json",
            description="Export json with pretty print",
            default=False,
        )

    is_constraints_only = BoolProperty(
            name="Export constraints only",
            description="Export only animation channels with are constraint of an other object",
            default=False,
        )

    is_export_rest_camera = BoolProperty(
            name="Export rest of cameras",
            description="Export rest of cameras without animation data",
            default=False,
        )

    @classmethod
    def poll(cls, context):
        return context.object is not None

    def execute(self, context):
        filepath = self.filepath
        filepath = bpy.path.ensure_ext(filepath, self.filename_ext)

        self.do_export(filepath, self.properties)

        return {'FINISHED'}

    def invoke(self, context, event):
        wm = context.window_manager

        if True:
            # File selector
            wm.fileselect_add(self)  # will run self.execute()
            return {'RUNNING_MODAL'}
        elif True:
            # search the enum
            wm.invoke_search_popup(self)
            return {'RUNNING_MODAL'}
        elif False:
            # Redo popup
            return wm.invoke_props_popup(self, event)
        elif False:
            return self.execute(context)

# ==================================================================================================================
    @staticmethod
    def do_export(file, properties):
        constraints = []
        anim_tracks = []

        # todo search for camera parameters:
        # >>> bpy.data.cameras[0].animation_data.action.fcurves[0].data_path
        # 'lens'

        # -- search for constraints
        for obj in bpy.data.objects:
            if obj.constraints:
                for elem in obj.constraints:
                    constraints.append(ConstraintEntity(obj.name, elem))

            # todo properties.is_constraints_only
            if obj.animation_data:
                # for elem in obj.animation_data:
                anim_tracks.append(AnimationEntity(obj.name, obj.animation_data))

        # --- search for paths and enumerate them
        curve_tracks = []
        for curve in bpy.data.curves:
            entity = CurveEntity(curve.name, curve)
            curve_tracks.append(entity)
            # for spline in curve.splines:
            #     print(spline)

        all_the_data = {
            "curves": curve_tracks,
            "animations": anim_tracks,
            "constraints": constraints
        }

        if _DEBUG_MODE:
            print("Writing " + file)

        fp = open(file, "w")

        str_ = MyEncoder(
                indent=2 if properties.use_pretty_print else 0      # todo do not even define tag, if no indent used
            ).encode(all_the_data)

        print(str_)

        fp.write(str_)

        fp.flush()
        fp.close()


# ==================================================================================================================


def menu_func(self, context):
    self.layout.operator(Export_gkjson.bl_idname, text="Grafkit TimeMachine animation (.json)")
    # print(context)


def register():
    # print("hello")
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)


def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func)


if __name__ == "__main__":
    register()

    if _DEBUG_MODE:

        class Dummy:
            use_pretty_print = True

        Export_gkjson.do_export("c:\\test.json", Dummy())

"""
FYI, RTFM
0) you are here
http://www.blender.org/api/blender_python_api_2_76_release/bpy.types.BlendData.html#bpy.types.BlendData
http://www.blender.org/api/blender_python_api_2_76_release/bpy.types.bpy_struct.html
http://www.blender.org/api/blender_python_api_2_76_release/bpy.types.Object.html#bpy.types.Object

1) animation data and their keyframes
http://www.blender.org/api/blender_python_api_2_62_release/bpy.types.AnimData.html
http://www.blender.org/api/blender_python_api_2_62_release/bpy.types.Action.html#bpy.types.Action
http://www.blender.org/api/blender_python_api_2_62_release/bpy.types.ActionFCurves.html#bpy.types.ActionFCurves
http://www.blender.org/api/blender_python_api_2_62_release/bpy.types.FCurve.html#bpy.types.FCurve
http://www.blender.org/api/blender_python_api_2_62_release/bpy.types.Keyframe.html#bpy.types.Keyframe

2) paths and curves
http://www.blender.org/api/blender_python_api_2_76_release/bpy.types.Curve.html#bpy.types.Curve
http://www.blender.org/api/blender_python_api_2_76_release/bpy.types.Spline.html#bpy.types.Spline

3) constraints
http://www.blender.org/api/blender_python_api_2_76_release/bpy.types.Constraint.html#bpy.types.Constraint
http://www.blender.org/api/blender_python_api_2_76_release/bpy.types.ObjectConstraints.html#bpy.types.ObjectConstraints
"""
