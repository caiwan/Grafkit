import os
import sys
import inspect
import json


# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

class Dumpable:
    def get_cmd():
        raise NotImplementedError


class DumpJSON (json.JSONEncoder):

    def default(self, obj):
        if hasattr(obj,'reprJSON'):
            return obj.reprJSON()
        elif isinstance(obj, (dict)):
            return {k: self.default(v) for k,v  in dict}
        else:
            try:
                return [v for v in iter(obj)]
            except TypeError:
                return json.JSONEncoder.default(self, obj)
            pass
