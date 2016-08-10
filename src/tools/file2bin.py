#pyFile2bin
#Converts files into char arrays
#slapec^ir 2012

import os
import sys

if len(sys.argv) != 3:
  print 'File to header file converter tool'
  print 'This tool creates a .h header file from the contents of a directory.'
  print '.h extension will attached automatically.'
  print 'usage: file2_bin.py <input_dir> <output_file>.h'
  sys.exit(0)

input = sys.argv[1]
output = sys.argv[2]

def parseFile(arg):
  with file(input+"/"+arg,'rb') as f:
    data = f.read()
    return ', '.join(hex(ord(r)) for r in data)

with file(output+".h",'w') as o:
  for f in os.listdir(input):
    print >> o, '//{0} {1}'.format(f.replace('.','_'), f)
	
  for f in os.listdir(input):
    print 'File: {0}'.format(f)
    print >> o, 'unsigned char {0}[{2}] = {{{1}}};'.format(f.replace('.','_'),parseFile(f),os.stat(input+"/"+f).st_size)
	
print 'File(s) written'


