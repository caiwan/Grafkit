import bpy
import unittest


def _walk(self, object):
    stack = []
    stack.append(object)
    nameStack = []
    nameStack.append("data")

    result = {}
    outnode = result

    while stack:
        node = stack.pop()
        name = nameStack.pop()

        if name in ["bl_rna", "rna_type", "translation_context", "is_dirty", "is_saved", "is_updated", "window_managers"]:
            continue

        clazz = node.__class__.__name__

        if clazz in ["bool", "int", "str", "tuple", "list", "dict", "float"]:
            if isinstance(outnode, dict):
                outnode[name] = node
            elif isinstance(outnode, list):
                outnode.append(node)
            continue
        elif clazz in ["Vector"]:
            continue
        elif clazz in ["bpy_prop_collection"]:
            newnode = []
            if isinstance(outnode, dict):
                outnode[name] = newnode
            elif isinstance(outnode, list):
                outnode.append(newnode)
            outnode = newnode

            for k in node:
                stack.append(k)
                nameStack.append(None)

            pass
        else:
            newnode = {}
            if isinstance(outnode, dict):
                outnode[name] = newnode
            elif isinstance(outnode, list):
                outnode.append(newnode)
            outnode = newnode
            pass

        for d in dir(node):
            if not d.startswith("_"):
                try:
                    a = getattr(node, d)
                    if not callable(a):
                        stack.append(a)
                        nameStack.append(d)
                except AttributeError as e :
                    pass

    # print(result)
    return result


if __name__ == "__main__":
    _walk(None, bpy.data)
