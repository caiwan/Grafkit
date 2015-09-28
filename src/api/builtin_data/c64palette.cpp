#include "c64palette.h"

const static unsigned char _c64Pal[48] = {
	0x00,0x00,0x00,
	0xFF,0xFF,0xFF,
	0x68,0x37,0x2B,
	0x70,0xA4,0xB2,
	0x6F,0x3D,0x86,
	0x58,0x8D,0x43,
	0x35,0x28,0x79,
	0xB8,0xC7,0x6F,
	0x6F,0x4F,0x25,
	0x43,0x39,0x00,
	0x9A,0x67,0x59,
	0x44,0x44,0x44,
	0x6C,0x6C,0x6C,
	0x9A,0xD2,0x84,
	0x6C,0x5E,0xB5,
	0x95,0x95,0x95
};

const unsigned char *c64Pal = _c64Pal;

/**
\var c64Pal

 R | G | B | index | name
---|---|---|-------|---------
00 |00 |00 | 0     | black
FF |FF |FF | 1     | white 
68 |37 |2B | 2     | red
70 |A4 |B2 | 3     | cyan
6F |3D |86 | 4     | purple
58 |8D |43 | 5     | green
35 |28 |79 | 6     | blue
B8 |C7 |6F | 7     | yellow
6F |4F |25 | 8     | orange
43 |39 |00 | 9     | brown
9A |67 |59 | A     | light red 
44 |44 |44 | B     | dark grey
6C |6C |6C | C     | grey 
9A |D2 |84 | D     | light green
6C |5E |B5 | E     | light blue 
95 |95 |95 | F     | light grey

*/