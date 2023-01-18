#pragma once

#include "../utils/reference.h"

#include "texture.h"

#include "renderer.h"
#include "particle.h"

namespace Grafkit {
	
	class FontMap;

	typedef Ref<FontMap> FontMapRef;

	class IFontmapBuilder {
	public:
		IFontmapBuilder() {}
		virtual ~IFontmapBuilder() {}

		virtual FontMapRef operator() (FontMapRef in) = 0;
	};
	
	/**
	A spritesheet for font maps 
	*/
	class FontMap {
	public:
		char face[256];		///< name of font
		int fontsize;		///< font size in pt

							// mindig unicodenak kellene lennie:
		int charset;		///< The name of the OEM charset used (when not unicode). 0=undefined 1=UTF-8 2=ANSI
		int isUnicode;		///< Set to 1 if it is the unicode charset.

		float strechH;		///< The font height stretch in percentage [0..1]. 1 means no stretch.

							///@todo ez nem kell pl. <- parserbol ki kell venni
		int isSmooth;		///< Set to 1 if smoothing was turned on.
		int MSAAlevel;		///< The supersampling level used. 1 means no supersampling was used.

		float4 padding;	///< The padding for each character (up, right, down, left).
		float2 spacing;	///< The spacing for each character (horizontal, vertical).

							// common info

		float lineHeight;	///< This is the distance in pixels between each line of text.
		float base;			///< The number of pixels from the absolute top of the line to the base of the characters.
		float2 scale;	///< The width&height of the texture, normally used to scale the x pos of the character image.

		int maxPages;		///< Number of pages

		std::vector<Grafkit::TextureRef> pages; ///< Pages (Containing textures)

		std::vector<int> charIDMap;	///< Id map to resolve chars

									/// This tag describes on character in the font. There is one for each included character in the font.
		struct char_fmt {
			wchar_t id;			///< The character id.
			float2 pos;		///< The top-left position of the character image in the texture.
			float2 size;		///< The width/height of the character image in the texture.
			float2 offset;	///< How much the current position should be offset when copying the image from the texture to the screen.
			float advanceX;		///< How much the current position should be advanced after drawing the character.
			int pageId;			///< The texture page where the character image is found.
			int channel;		///< The texture channel where the character image is found (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels).

			char_fmt() {
				id = 0;
				advanceX = 0.f;
				pageId = -1;
				channel = -1;
			}
		};

		std::vector<struct char_fmt> chars;	///< Map of chars

		int isUseKerning;

		///@todo esetlegesen ezzel kiegesziteni a dolgot: https://www.sgi.com/tech/stl/hash_map.html
		/// `[first<<16 | second] => index`
		std::map<int, int> kerning_cache;

		/// The kerning information is used to adjust the distance between certain characters, e.g. some characters should be placed closer to each other than others.
		struct kerning_fmt {
			wchar_t first;		///< The first character id.
			wchar_t second;		///< The second character id.
			float amount;		///< How much the x position should be adjusted when drawing the second character immediately following the first.

			kerning_fmt() {
				first = 0;
				second = 0;
				amount = 0.f;
			}
		};

		/// Array of kernings
		std::vector<struct kerning_fmt> kernings;

	public:
		/// Generating font map using a custom builder
		FontMap();
		~FontMap();

		inline int getPageCount() { return this->pages.size(); }
		inline Grafkit::TextureRef getPage(int n) { return this->pages[n]; }


	};

};
