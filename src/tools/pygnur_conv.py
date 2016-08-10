#pyGNURocket
#Converts .track files into char arrays
#slapec^ir 2012

import os
import sys

print 'usage: pygnur_conv.py <input_dir> <output_file>'
if len(sys.argv) != 3:
  sys.exit(0)

input = sys.argv[1]
output = sys.argv[2]

def parseTrack(arg):
  with file(input+"/"+arg,'rb') as f:
    data = f.read()
    return ', '.join(hex(ord(r)) for r in data)

with file(output,'w') as o:
#with sys.stdout as o:
  for f in os.listdir(input):
    if f[-6:] == '.track':
      print >> o, '//{0} {1}'.format(f.replace('.','_'), f)
  for f in os.listdir(input):
    if f[-6:] == '.track':
      print >> o, 'static unsigned char {0}[] = {{{1}}};'.format(f.replace('.','_'),parseTrack(f))

print 'File written'