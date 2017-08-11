"""
Blender exporter client
Caiwan / IR
"""

import re
import socket
import json
from struct import pack

class Connection:
    _cmd_initconn = "hello"
    _cmd_closeconn = "goodbye"
    
    class Package:
        def __init__(self, cmd, data):
            self.cmd = cmd;
            self.data = data;

        
    def __init__(self, addr):
        r = re.search("([^\\:]+):([0-9]{4,5})", addr)
        if r:
            self.host = (r.group(1), int(r.group(2)))
        else:
            raise RuntimeError("Invlaid address: " + addr)
        
        self.s = None
        pass
        
    
    def __enter__(self):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect(self.host)
        self._send_network(Connection.Package(self._cmd_initconn, ""))
        return self 
    
    
    def __exit__(self, type, value, traceback):
        ret = None
        if value:
            err = {"type":str(type), "value":str(value), "traceback":str(traceback)}
            ret = {"error":err}
            
        self._send_network(Connection.Package(self._cmd_closeconn, ret))
        self.s.close()
        pass
        
        
    def _send_network(self, obj):
        j = json.dumps(obj.__dict__).encode("utf-8")
        h = pack("I", len(j))
        print("sending {} bytes of data".format(len(j)))
        self.s.send(h)
        self.s.send(j)
        #self.s.send("\x00")
        pass
        
    def send(self, cmd, obj):
        self._send_network(Connection.Package(cmd, obj))
        pass
        
    def senderror(self, type, value, traceback):
        ret = None
        if value:
            err = {"type":str(type), "value":str(value), "traceback":str(traceback)}
            ret = {"error":err}
            
            _send_network(Connection.Package(self._cmd_closeconn, ret))
        
        
class Filedump:
    def __init__(self, fn):
        self.fn = fn
        pass
        
        
    def __enter__(self):
        self._fp = open(self.fn, "w")
        return self
    
    
    def __exit__(self, type, value, traceback):
        ret = None
        if value:
            err = {"type":str(type), "value":str(value), "traceback":str(traceback)}
            ret = {"error":err}
            
        self._fp.close()


    def _sendfile(self, obj):
        json.dump(obj.__dict__, self._fp, indent=4, sort_keys=True)
        pass

    def send(self, cmd, obj):
        self._sendfile(Connection.Package(cmd, obj))
        pass
        
        
    def senderror(self, type, value, traceback):
        ret = None
        if value:
            err = {"type":str(type), "value":str(value), "traceback":str(traceback)}
            ret = {"error":err}
            
            _send_network(Connection.Package(self._cmd_closeconn, ret))
