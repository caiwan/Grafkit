import os
import sys
import inspect

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

class Dumpable:
    def get_cmd():
        raise NotImplementedError

class DumpJSON (json.JSONEncoder):
    def default(self):
        if hasattr(obj,'reprJSON'):
            return obj.reprJSON()
        else:
            return json.JSONEncoder.default(self, obj)
        pass
