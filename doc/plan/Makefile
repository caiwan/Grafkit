api:
	make api
	
editor:
	make editor

all: api editor
	cat *.md | pandoc -sS --toc plan.doc.html
