# md5er
# calculates md5 hashes 

import os
import sys
from hashlib import md5

# ---------------------------------------
#delete False
if len(sys.argv) != 3:
	print 'lol, leiras goez here '
	print 'file2md5.py root outfile'
	print 'root    - relative to py script path'
	print 'outfile - c header file'
	sys.exit(0)

# ---------------------------------------	
rroot = sys.argv[1]
outfile = sys.argv[2]

with file('{0}'.format(outfile), 'w') as of:
	print >> of, '//md5 hashes'
	print >> of,'const struct {'
	print >> of,'	const char *filename;'
	print >> of,'	const char *md5hash;'
	print >> of,'} __file_hashes[] = {'
		
	for root, dirs, files in os.walk(rroot):
		for file in files:
			hasher = md5()
			path = os.path.join(root, file)

			with open(path, 'rb') as f:
				while True:
					data = f.read(4096)
					hasher.update(data)
					if not data:
						break

			#quick-and-dirty megoldas mostan
			ppath = path[len(rroot):].replace('../','').replace('./','').replace('\\','/')
			print >> of,'	{{"{0}", "{1}"}},'.format(ppath, hasher.hexdigest())
			print ppath
			
	print >> of,'};'		