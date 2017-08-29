import os
import sys
import inspect

# fix import paths for internal imports
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
if cmd_folder not in sys.path:
    sys.path.insert(0, cmd_folder)


class Dumpable:
    """ Dumpable base class
    """
    
    def __init__(self):
        pass
        
    def dump(self):
        """Returns the serialized object data"""
        return ""
        pass
        
    def get_cmd(self):
        """ Retruns the command key of the object"""
        return ""
        pass
        
    pass

    
class Dump (json.JSONEncoder):
    """ Network dump framework class
    """
    
    def default(self):
        pass
    

    