#pyGNURocket
#Converts .track files into char arrays
#slapec^ir 2012

import os
import sys

if len(sys.argv) != 3:
  print 'File to C source file converter tool'
  print 'This tool creates a .c source and a .h header file from the contents of a directory.'
  print '.c and .h extensions will attached automatically.'
  print 'usage: file2_bin.py <input_dir> <output_file>[.c|.h]'
  sys.exit(0)


input = sys.argv[1]
output = sys.argv[2]

def parseFile(arg):
  with file(input+"/"+arg,'rb') as f:
    data = f.read()
    return ', '.join(hex(ord(r)) for r in data)

o = open(output+".c",'w')
#with file(output+".c",'w') as o:
#with sys.stdout as o:
for f in os.listdir(input):
    print >> o, '//{0} {1}'.format(f.replace('.','_'), f)

for f in os.listdir(input):
    print 'File: {0}'.format(f)
    print >> o, 'unsigned char _{0}[] = {{{1}}};'.format(f.replace('.','_'),parseFile(f))
    print >> o, 'unsigned char *{0} = _{0};'.format(f.replace('.','_'))

o.close();

o = open(output+".h",'w')
for f in os.listdir(input):
    print >> o, '//{0} {1}'.format(f.replace('.','_'), f)
    print >> o, '#define SIZEOF_{0} {1}'.format(f.replace('.','_'), os.stat(input+"/"+f).st_size)
    print >> o, 'extern unsigned char *{0};'.format(f.replace('.','_'))
    print >> o, ''
o.close()	
	
print 'File(s) written'


