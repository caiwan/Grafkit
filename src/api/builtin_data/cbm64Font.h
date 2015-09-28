/**
\file 
	
	\brief C64 chargen rom dump
	
	Contains the whole both CBM charset.
*/

#ifndef _CBM64_FONT_H_
#define _CBM64_FONT_H_

/** 

\brief Pointer to the font data

offset       | description
-------------|----------------------------------------
0 (0x0000)   | Uppercase font and both grpahics chars
2048 (0x1000)| Lowecase font and upper graphic chars

	@todo include that converter class, which has been taken off for a while
*/
extern const unsigned char *cbm64Font;

/// Bit per pixel
#define cbm64FontBPP 1
/// Height in pixel
#define cbm64FontHeight	8
/// Width in pixel
#define cbm64FontWidth 8

#endif //_CBM64_FONT_H_