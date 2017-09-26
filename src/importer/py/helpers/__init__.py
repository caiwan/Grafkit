import os
import sys
import inspect
import json
import re

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)

# https://stackoverflow.com/questions/1175208/elegant-python-function-to-convert-camelcase-to-snake-case
first_cap_re = re.compile('(.)([A-Z][a-z]+)')
all_cap_re = re.compile('([a-z0-9])([A-Z])')
def camel2snake(name):
    s1 = first_cap_re.sub(r'\1_\2', name)
    return all_cap_re.sub(r'\1_\2', s1).lower()


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
