"""
Blender exporter client
Caiwan / IR
"""

import re
import socket
import json

class Connection:
  _cmd_initconn = "hello"
  _cmd_closeconn = "goodbye"
  _cmd_data = "data"
  
  class Package:
    def __init__(self, cmd, data):
      self.cmd = cmd;
      self.data = data;

  def __init__(self, addr):
    r = re.search("([^\\:]+):([0-9]{5})", addr)
    if r:
      self.host = (r.group(1), int(r.group(2)))
    else:
      raise RuntimeError("Invlaid address")
    
    self.s = None
    pass
    
  
  def __enter__(self):
    self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.s.connect(self.host)
    self._send(Connection.Package(self._cmd_initconn, ""))
    pass
  
  
  def __exit__(self, type, value, traceback):
    self._send(Connection.Package(self._cmd_closeconn, ""))
    self.s.close()
    pass
    

  def _send(self, obj):
    j = json.dumps(obj.__dict__)
    self.s.send(j.encode("utf-8"))
    pass
    
  def data(self, obj):
    self._send(Connection.Package(self._cmd_data, obj))
    pass
    