#pyGNURocket
#Converts any binary file into c source and header
#slapec^ir 2012

import os
import sys

# ---------------------------------------
#delete False
if False and len(sys.argv) != 3:
	print 'File to C source file converter tool'
	print 'This tool creates a .c source and a .h header file from the contents of a directory.'
	print '.c and .h extensions will attached automatically.'
	print 'usage: file2_bin.py <input_dir> <output_file>[.c|.h]'
	sys.exit(0)


input = sys.argv[1]
output = sys.argv[2]

#delete these lines
#input = './exmp'
#output = 'zsiraf'

# ---------------------------------------
def parseFile(arg):
  with file(input+"/"+arg,'rb') as f:
    out = ''
    count = 0

    for r in f.read():
      out += '{0}, '.format(hex(ord(r)))
      count += 1
      if count == 20:
        out = out + '\r\n'
        count = 0
  #szomoru tudom
  return out[:-2]
# ---------------------------------------

with file('{0}.c'.format(output), 'w') as f:
  for fname in os.listdir(input):
    if os.path.isfile(os.path.join(input, fname)):
      formatName = fname.replace('.', '_')
      print 'File: {0}'.format(fname)
      print >> f, '//{0} {1}'.format(formatName, fname)
      print >> f, 'unsigned char _{0}[] = {{{1}}};'.format(formatName,parseFile(fname))
      print >> f, 'unsigned char *{0} = _{0};'.format(formatName)

#python 3.1 ismeri mar azt a with utasitast ami tobb objekttel dolgozik
#ez most ilyen marad
with file('{0}.h'.format(output),'w') as f:
  for fname in os.listdir(input):
    if os.path.isfile(os.path.join(input, fname)):
      formatName = fname.replace('.', '_')
      print >> f, '//{0} {1}'.format(formatName, fname)
      print >> f, '#define SIZEOF_{0} {1}'.format(formatName, os.stat(input+"/"+fname).st_size)
      print >> f, 'extern unsigned char *{0};'.format(formatName)
      print >> f, ''
	
print 'File(s) written'