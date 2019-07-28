﻿#ifndef __font_h__
#define __font_h__
#include <unordered_map>
#include <map>
#include <atomic>
#include <algorithm>
#include <codecvt>
#include <string>
#include "font/tags.h"

/*
 如果不需要位图字体则定义_FONT_NO_BITMAP

*/

#ifndef NJSON_H
//#define JSON_NOEXCEPTION
#include "json.hpp"
#if defined( NLOHMANN_JSON_HPP) || defined(INCLUDE_NLOHMANN_JSON_HPP_)
using njson = nlohmann::json;
#define NJSON_H
#endif
#endif

#ifdef FONS_USE_FREETYPE
#ifndef FT_CONFIG_OPTION_SUBPIXEL_RENDERING
#define FT_CONFIG_OPTION_SUBPIXEL_RENDERING
#endif
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H
#include FT_SFNT_NAMES_H
#include FT_LCD_FILTER_H
//#include <ft2build.h>
//#include FT_FREETYPE_H
#include FT_ADVANCES_H
#else
#include "stb/lib_src.h"
#endif // !FONS_USE_FREETYPE
/*
ttf说明
head	字体头 字体的全局信息
cmap	字符代码到图元的映射 把字符代码映射为图元索引
glyf	图元数据 图元轮廓定义以及网格调整指令
maxp	最大需求表 字体中所需内存分配情况的汇总数据
mmtx	水平规格 图元水平规格
loca	位置表索引 把元索引转换为图元的位置
name	命名表 版权说明、字体名、字体族名、风格名等等
hmtx	水平布局 字体水平布局星系：上高、下高、行间距、最大前进宽度、最小左支撑、最小右支撑
kerm	字距调整表 字距调整对的数组
post
PostScript信息 所有图元的PostScript FontInfo目录项和PostScript名

PCLT	PCL 5数据 HP PCL 5Printer Language 的字体信息：字体数、宽度、x高度、风格、记号集等等
OS/2	OS/2和Windows特有的规格 TrueType字体所需的规格集


*/

class font_hs
{
public:
	std::wstring font_name;
	int size = 0;
	int dpi = 96;
	//int lh = 0;
	font_hs() {}
};
namespace std {
	template<>
	class hash<font_hs> {
	public:
		size_t operator()(const font_hs& s) const
		{
			size_t h1 = std::hash<std::wstring>()(s.font_name);
			size_t h2 = std::hash<int>()(s.size);
			size_t h3 = std::hash<int>()(s.dpi);
			//size_t h4 = std::hash<int>()(s.lh);
			return h1 ^ h2 ^ h3/* ^ h4*/;
		}
	};
}
namespace hz {
	class font_att :public Res
	{
	public:

	public:
		int						fontsize = 10;	//字号
		int						dpi = 96;
		int						step = 0;		//字间隔

		unsigned int			color = 0xff000000;
		unsigned int			back_color = 0x80FF9033;
		double wrap_width = 0;
		int line_height = 0;
		int char_height = 0;
	private:
		std::map<int, int> fixed_width;
		// 字宽
		int char_width[256] = { 0 };
		std::atomic<bool> is_fixed_width = false;

		std::vector<glm::ivec2> fw2;
		LockS _lock;
	private:
		unsigned int _hash = 0;
	public:
		font_att()
		{
			set_size(-1, -1);
		}
		font_att(int fs, int s) : fontsize(fs), step(s)
		{
			set_size(-1, -1);
		}
		font_att(const font_att& c)
		{
			copy(c);
		}
	public:

		~font_att()
		{
		}
		void copy(const font_att& c)
		{
			if (*this != c)
			{
				clear_char_width();
				LOCK_W(_lock);
				step = c.step;// || c.color != color);				
				wrap_width = c.wrap_width;
				fw2.clear();
				set_size(c.fontsize, c.dpi);
			}
		}
		bool is_fixed()
		{
			return is_fixed_width;
		}
		unsigned int getFhash(const std::wstring& fname, int fontsize, int dpi/*, int lh*/)
		{
			font_hs fs;
			fs.font_name = fname;
			fs.size = fontsize;
			fs.dpi = dpi;
			//fs.lh = lh;
			unsigned int ret = 0;
			{
				//PTIMER;
				static std::hash<font_hs> hfn;
				ret = hfn(fs);
			}
			return ret;
		}
	public:
		bool operator != (const font_att& c) {
			return (c.fontsize != fontsize || c.step != step || c.dpi != dpi);
		}

		static font_att* create()
		{
			return new font_att();
		}
		void set_size(int fsize, int fdpi, bool isset = true)
		{
			unsigned int nh = 0;
			{
				LOCK_W(_lock);
				if (fsize > 0)
					fontsize = fsize;
				if (fdpi > 0)
					dpi = fdpi;
				char_height = dpi * fontsize / 72;
				line_height = char_height + 4;
				if (line_height & 1)
				{
					line_height++;
				}
				//nh = getFhash(wfontname, fontsize, dpi);
			}
			if (nh != _hash && isset)
			{
				_hash = nh;
			}
		}

		unsigned int get_hash()
		{
			if (!_hash)
			{
				LOCK_W(_lock);
				//_hash = getFhash(wfontname, fontsize, dpi);
			}
			LOCK_R(_lock);
			return _hash;
		}
		int gcw(char c)
		{
			LOCK_R(_lock);
			return char_width[c];
		}
		void set_cw(char c, int n)
		{
			if (c > 0)
			{
				LOCK_W(_lock);
				char_width[c] = n;
			}
		}
		int get_char_width(wchar_t c)
		{
			int ret = 0;
			if (c < 256)
			{
				switch (c)
				{
				case L'\r':
					break;
				default:
				{
					if (is_fixed_width)
					{
						ret = gcw(0);
					}
					else
					{
						ret = gcw(c);
						/*					if (!(ret > 0) && get_char_width_func)
											{
												ret = get_char_width_func(this, c);
												scw(c, ret);
											}*/
					}
				}
				break;
				}
			}
			else
			{
				ret = char_height;
			}
			return ret;
		}
		void clear_char_width()
		{
			LOCK_W(_lock);
			for (int i = 1; i < 256; i++)
			{
				char_width[i] = 0;
			}
		}

	public:

	private:

	};

	class Fonts
	{
	public:
		typedef struct sfnt_header_
		{
			unsigned int tag = 0;
			unsigned int checksum;
			unsigned int offset; //From beginning of header.
			unsigned int logicalLength;
		}sfnt_header;
		typedef struct  Bitmap_
		{
			unsigned int    rows;
			unsigned int    width;
			int             pitch;
			double			advance;
			int             bit_depth;
			unsigned char* buffer;
			unsigned int	capacity = 0;
			unsigned short  num_grays;
			unsigned char   pixel_mode;
			unsigned char   palette_mode;
			void* palette;
		} Bitmap;
		typedef enum  _Pixel_Mode_
		{
			PX_NONE = 0,
			PX_MONO,
			PX_GRAY,
			PX_GRAY2,
			PX_GRAY4,
			PX_LCD,
			PX_LCD_V,
			PX_BGRA,

			PX_MAX      /* do not remove */

		} Pixel_Mode;

		typedef enum STT_
		{
			TYPE_NONE = 0,
			TYPE_EBLC, /* `EBLC' (Microsoft), */
			 /* `bloc' (Apple)      */
			 TYPE_CBLC, /* `CBLC' (Google)     */
			 TYPE_SBIX, /* `sbix' (Apple)      */
			 /* do not remove */
			 TYPE_MAX
		} sbit_table_type;
#ifndef _FONT_NO_BITMAP
#ifdef NO_CPU_LENDIAN
#define UINT8_BITFIELD_BENDIAN
#else
#define UINT8_BITFIELD_LENDIAN
#endif
#ifdef UINT8_BITFIELD_LENDIAN
#define UINT8_BITFIELD(f0, f1, f2, f3, f4, f5, f6, f7) \
        uint8_t f0 : 1; \
        uint8_t f1 : 1; \
        uint8_t f2 : 1; \
        uint8_t f3 : 1; \
        uint8_t f4 : 1; \
        uint8_t f5 : 1; \
        uint8_t f6 : 1; \
        uint8_t f7 : 1;
#else
#define UINT8_BITFIELD(f0, f1, f2, f3, f4, f5, f6, f7) \
        uint8_t f7 : 1; \
        uint8_t f6 : 1; \
        uint8_t f5 : 1; \
        uint8_t f4 : 1; \
        uint8_t f3 : 1; \
        uint8_t f2 : 1; \
        uint8_t f1 : 1; \
        uint8_t f0 : 1;
#endif
#define BYTE_BITFIELD UINT8_BITFIELD
		// EBLC头用到的结构
		struct SbitLineMetrics {
			char ascender;
			char descender;
			uint8_t widthMax;
			char caretSlopeNumerator;
			char caretSlopeDenominator;
			char caretOffset;
			char minOriginSB;
			char minAdvanceSB;
			char maxBeforeBL;
			char minAfterBL;
			char pad1;
			char pad2;
		};

		struct BigGlyphMetrics {
			uint8_t height;
			uint8_t width;
			char horiBearingX;
			char horiBearingY;
			uint8_t horiAdvance;
			char vertBearingX;
			char vertBearingY;
			uint8_t vertAdvance;
		};

		struct SmallGlyphMetrics {
			uint8_t height;
			uint8_t width;
			char bearingX;
			char bearingY;
			uint8_t advance;
		};
		struct BitmapSizeTable {
			uint32_t indexSubTableArrayOffset; //offset to indexSubtableArray from beginning of EBLC.
			uint32_t indexTablesSize; //number of bytes in corresponding index subtables and array
			uint32_t numberOfIndexSubTables; //an index subtable for each range or format change
			uint32_t colorRef; //not used; set to 0.
			SbitLineMetrics hori; //line metrics for text rendered horizontally
			SbitLineMetrics vert; //line metrics for text rendered vertically
			unsigned short startGlyphIndex; //lowest glyph index for this size
			unsigned short endGlyphIndex; //highest glyph index for this size
			uint8_t ppemX; //horizontal pixels per Em
			uint8_t ppemY; //vertical pixels per Em
			struct BitDepth {
				enum Value : uint8_t {
					BW = 1,
					Gray4 = 2,
					Gray16 = 4,
					Gray256 = 8,
				};
				uint8_t value;
			} bitDepth; //the Microsoft rasterizer v.1.7 or greater supports
			union Flags {
				struct Field {
					//0-7
					BYTE_BITFIELD(
						Horizontal, // Horizontal small glyph metrics
						Vertical,  // Vertical small glyph metrics
						Reserved02,
						Reserved03,
						Reserved04,
						Reserved05,
						Reserved06,
						Reserved07)
				} field;
				struct Raw {
					static const char Horizontal = 1u << 0;
					static const char Vertical = 1u << 1;
					char value;
				} raw;
			} flags;
		}; //bitmapSizeTable[numSizes];

		struct IndexSubTableArray {
			unsigned short firstGlyphIndex; //first glyph code of this range
			unsigned short lastGlyphIndex; //last glyph code of this range (inclusive)
			uint32_t additionalOffsetToIndexSubtable; //add to BitmapSizeTable::indexSubTableArrayOffset to get offset from beginning of 'EBLC'
		}; //indexSubTableArray[BitmapSizeTable::numberOfIndexSubTables];

		struct IndexSubHeader {
			unsigned short indexFormat; //format of this indexSubTable
			unsigned short imageFormat; //format of 'EBDT' image data
			uint32_t imageDataOffset; //offset to image data in 'EBDT' table
		};

		// Variable metrics glyphs with 4 byte offsets
		struct IndexSubTable1 {
			IndexSubHeader header;
			//uint32_t offsetArray[lastGlyphIndex - firstGlyphIndex + 1 + 1]; //last element points to one past end of last glyph
			//glyphData = offsetArray[glyphIndex - firstGlyphIndex] + imageDataOffset
		};

		// All Glyphs have identical metrics
		struct IndexSubTable2 {
			IndexSubHeader header;
			uint32_t imageSize; // all glyphs are of the same size
			BigGlyphMetrics bigMetrics; // all glyphs have the same metrics; glyph data may be compressed, byte-aligned, or bit-aligned
		};

		// Variable metrics glyphs with 2 byte offsets
		struct IndexSubTable3 {
			IndexSubHeader header;
			//unsigned short offsetArray[lastGlyphIndex - firstGlyphIndex + 1 + 1]; //last element points to one past end of last glyph, may have extra element to force even number of elements
			//glyphData = offsetArray[glyphIndex - firstGlyphIndex] + imageDataOffset
		};

		// Variable metrics glyphs with sparse glyph codes
		struct IndexSubTable4 {
			IndexSubHeader header;
			uint32_t numGlyphs;
			struct CodeOffsetPair {
				unsigned short glyphCode;
				unsigned short offset; //location in EBDT
			}; //glyphArray[numGlyphs+1]
		};

		// Constant metrics glyphs with sparse glyph codes
		struct IndexSubTable5 {
			IndexSubHeader header;
			uint32_t imageSize; //all glyphs have the same data size
			BigGlyphMetrics bigMetrics; //all glyphs have the same metrics
			uint32_t numGlyphs;
			//unsigned short glyphCodeArray[numGlyphs] //must have even number of entries (set pad to 0)
		};

		union IndexSubTable {
			IndexSubHeader header;
			IndexSubTable1 format1;
			IndexSubTable2 format2;
			IndexSubTable3 format3;
			IndexSubTable4 format4;
			IndexSubTable5 format5;
		};

		union GlyphMetrics
		{
			struct BigGlyphMetrics _big;
			struct SmallGlyphMetrics _small;
		};


#endif // !_FONT_NO_BITMAP



#define FONS_INVALID -1

		enum FONSflags {
			FONS_ZERO_TOPLEFT = 1,
			FONS_ZERO_BOTTOMLEFT = 2,
		};

		enum FONSalign {
			// Horizontal align
			FONS_ALIGN_LEFT = 1 << 0,	// Default
			FONS_ALIGN_CENTER = 1 << 1,
			FONS_ALIGN_RIGHT = 1 << 2,
			// Vertical align
			FONS_ALIGN_TOP = 1 << 3,
			FONS_ALIGN_MIDDLE = 1 << 4,
			FONS_ALIGN_BOTTOM = 1 << 5,
			FONS_ALIGN_BASELINE = 1 << 6, // Default
		};

		enum FONSglyphBitmap {
			FONS_GLYPH_BITMAP_OPTIONAL = 1,
			FONS_GLYPH_BITMAP_REQUIRED = 2,
		};

		enum FONSerrorCode {
			// Font atlas is full.
			FONS_ATLAS_FULL = 1,
			// Scratch memory used to render glyphs is full, requested size reported in 'val', you may need to bump up FONS_SCRATCH_BUF_SIZE.
			FONS_SCRATCH_FULL = 2,
			// Calls to fonsPushState has created too large stack, if you need deep state stack bump up FONS_MAX_STATES.
			FONS_STATES_OVERFLOW = 3,
			// Trying to pop too many states fonsPopState().
			FONS_STATES_UNDERFLOW = 4,
		};

		// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
		// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define FONS_UTF8_ACCEPT 0
#define FONS_UTF8_REJECT 12

		static unsigned int fons_decutf8(unsigned int* state, unsigned int* codep, unsigned int byte)
		{
			static const unsigned char utf8d[] = {
				// The first part of the table maps bytes to character classes that
				// to reduce the size of the transition table and create bitmasks.
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
				10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

				// The second part is a transition table that maps a combination
				// of a state of the automaton and a character class to a state.
				0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
				12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
				12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
				12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
				12,36,12,12,12,12,12,12,12,12,12,12,
			};

			unsigned int type = utf8d[byte];

			*codep = (*state != FONS_UTF8_ACCEPT) ?
				(byte & 0x3fu) | (*codep << 6) :
				(0xff >> type) & (byte);

			*state = utf8d[256 + *state + type];
			return *state;
		}

		class Glyph
		{
		public:
			unsigned int codepoint = 0;
			int index;
			Glyph* next = 0;
			short size, blur;
			short x0, y0, x1, y1;
			short xadv, xoff, yoff;
			unsigned int _width, _height;
			std::vector<unsigned char> bitmap;
		public:
			Glyph()
			{
			}

			~Glyph()
			{
			}
			unsigned char* resize_bitmap(int w, int h)
			{
				size_t s = w * h;
				if (s != bitmap.size())
				{
					_width = w; _height = h;
					bitmap.resize(s);
				}
				return (unsigned char*)bitmap.data();
			}
		private:

		};



#define FONS_NOTUSED(v)  (void)sizeof(v)
#ifdef FONS_USE_FREETYPE
		class ft_font
		{
		public:
			ft_font()
			{
			}

			~ft_font()
			{
			}

		private:

			typedef struct {
				FT_Face font;
			}font_impl;

			FT_Library ftLibrary;

			int init()
			{
				FT_Error ftError;
				ftError = FT_Init_FreeType(&ftLibrary);
				return ftError == 0;
			}

			int done()
			{
				FT_Error ftError;
				ftError = FT_Done_FreeType(ftLibrary);
				return ftError == 0;
			}

			int loadFont(font_impl* font, unsigned char* data, int dataSize)
			{
				FT_Error ftError;
				//font->font.userdata = stash;
				ftError = FT_New_Memory_Face(ftLibrary, (const uint8_t*)data, dataSize, 0, &font->font);
				return ftError == 0;
			}

			void getFontVMetrics(font_impl* font, int* ascent, int* descent, int* lineGap)
			{
				*ascent = font->font->ascender;
				*descent = font->font->descender;
				*lineGap = font->font->height - (*ascent - *descent);
			}

			float getPixelHeightScale(font_impl* font, float size)
			{
				return size / (font->font->ascender - font->font->descender);
			}

			int getGlyphIndex(font_impl* font, int codepoint)
			{
				return FT_Get_Char_Index(font->font, codepoint);
			}

			int buildGlyphBitmap(font_impl* font, int glyph, float size, float scale,
				int* advance, int* lsb, int* x0, int* y0, int* x1, int* y1)
			{
				FT_Error ftError;
				FT_GlyphSlot ftGlyph;
				FT_Fixed advFixed;
				FONS_NOTUSED(scale);

				ftError = FT_Set_Pixel_Sizes(font->font, 0, (FT_UInt)(size * (float)font->font->units_per_EM / (float)(font->font->ascender - font->font->descender)));
				if (ftError) return 0;
				ftError = FT_Load_Glyph(font->font, glyph, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
				if (ftError) return 0;
				ftError = FT_Get_Advance(font->font, glyph, FT_LOAD_NO_SCALE, &advFixed);
				if (ftError) return 0;
				ftGlyph = font->font->glyph;
				*advance = (int)advFixed;
				*lsb = (int)ftGlyph->metrics->horiBearingX;
				*x0 = ftGlyph->bitmap_left;
				*x1 = *x0 + ftGlyph->bitmap.width;
				*y0 = -ftGlyph->bitmap_top;
				*y1 = *y0 + ftGlyph->bitmap.rows;
				return 1;
			}

			void renderGlyphBitmap(font_impl* font, unsigned char* output, int outWidth, int outHeight, int outStride,
				float scaleX, float scaleY, int glyph)
			{
				FT_GlyphSlot ftGlyph = font->font->glyph;
				int ftGlyphOffset = 0;
				int x, y;
				FONS_NOTUSED(outWidth);
				FONS_NOTUSED(outHeight);
				FONS_NOTUSED(scaleX);
				FONS_NOTUSED(scaleY);
				FONS_NOTUSED(glyph);	// glyph has already been loaded by buildGlyphBitmap

				for (y = 0; y < ftGlyph->bitmap.rows; y++) {
					for (x = 0; x < ftGlyph->bitmap.width; x++) {
						output[(y * outStride) + x] = ftGlyph->bitmap.buffer[ftGlyphOffset++];
					}
				}
			}

			int getGlyphKernAdvance(font_impl* font, int glyph1, int glyph2)
			{
				FT_Vector ftKerning;
				FT_Get_Kerning(font->font, glyph1, glyph2, FT_KERNING_DEFAULT, &ftKerning);
				return (int)((ftKerning.x + 32) >> 6);  // Round up and convert to integer
			}

		};
#define font_dev ft_font
#else
		class stb_font
		{
		public:
			/*

			*/
			struct font_impl {
				stbtt_fontinfo font;
				// EBLC	Embedded bitmap location data	嵌入式位图位置数据
				int eblc;
				uint32_t sbit_table_size;
				int      sbit_table_type;
				uint32_t sbit_num_strikes;
				// EBDT	Embedded bitmap data	嵌入式位图数据 either `CBDT', `EBDT', or `bdat' 
				uint32_t ebdt_start;
				uint32_t ebdt_size;
				// EBSC	Embedded bitmap scaling data	嵌入式位图缩放数据
				uint32_t ebsc;
				std::map<std::string, sfnt_header> _tb;
			};
			typedef struct font_impl font_impl;
		public:
			stb_font()
			{
			}

			~stb_font()
			{
			}
			static font_impl* new_fontinfo()
			{
				return new font_impl();
			}
			static void free_fontinfo(font_impl* p)
			{
				font_impl* pf = (font_impl*)p;
				delete pf;
			}
		public:

			static int init(void*)
			{
				return 1;
			}

			static int done(void*)
			{
				return 1;
			}

			static int loadFont(font_impl* font, const void* data, int idx, void* ud)
			{
				int stbError;
				FONS_NOTUSED(idx);

				font->font.userdata = ud;
				int fso = get_offset(data, idx);
				stbError = stbtt_InitFont(&font->font, (unsigned char*)data, fso);
				stbError = init_table(font, (unsigned char*)data, fso);
				return stbError;
			}
			static int get_numbers(const void* data)
			{
				return stbtt_GetNumberOfFonts((unsigned char*)data);
			}
			static int get_offset(const void* data, int idx)
			{
				return stbtt_GetFontOffsetForIndex((unsigned char*)data, idx);
			}
			static void getFontVMetrics(font_impl* font, int* ascent, int* descent, int* lineGap)
			{
				stbtt_GetFontVMetrics(&font->font, ascent, descent, lineGap);
			}

			static float getPixelHeightScale(font_impl* font, float size)
			{
				return stbtt_ScaleForPixelHeight(&font->font, size);
			}
			// 获取utf8字符
			static const char* get_glyph_index_last(font_impl* font, const char* str, int* index)
			{
				unsigned int codepoint = 0;
				unsigned int utf8state = 0;
				*index = -1;
				for (; str && *str && *index == -1; ++str) {
					if (fons_decutf8(&utf8state, &codepoint, *(const unsigned char*)str))
						continue;
					*index = stbtt_FindGlyphIndex(&font->font, codepoint);
				}
				return str;
			}
			static unsigned int get_u8_to_u16(const char* str)
			{
				unsigned int codepoint = 0;
				unsigned int utf8state = 0;
				for (; str && *str; ++str) {
					if (fons_decutf8(&utf8state, &codepoint, *(const unsigned char*)str))
						continue;
					break;
				}
				return codepoint;
			}
			static int get_glyph_index(font_impl* font, const char* str)
			{
				unsigned int codepoint = 0;
				unsigned int utf8state = 0;
				int index = -1;
				for (; str && *str && index == -1; ++str) {
					if (fons_decutf8(&utf8state, &codepoint, *(const unsigned char*)str))
						continue;
					index = stbtt_FindGlyphIndex(&font->font, codepoint);
				}
				return index;
			}

			static int getGlyphIndex(font_impl* font, int codepoint)
			{
				return stbtt_FindGlyphIndex(&font->font, codepoint);
			}

			static int buildGlyphBitmap(font_impl* font, int glyph, float size, float scale,
				int* advance, int* lsb, int* x0, int* y0, int* x1, int* y1)
			{
				FONS_NOTUSED(size);
				stbtt_GetGlyphHMetrics(&font->font, glyph, advance, lsb);
				stbtt_GetGlyphBitmapBox(&font->font, glyph, scale, scale, x0, y0, x1, y1);
				return 1;
			}

			static void renderGlyphBitmap(font_impl* font, unsigned char* output, int outWidth, int outHeight, int outStride,
				float scaleX, float scaleY, int glyph)
			{
				stbtt_MakeGlyphBitmap(&font->font, output, outWidth, outHeight, outStride, scaleX, scaleY, glyph);
			}

			static int getGlyphKernAdvance(font_impl* font, int glyph1, int glyph2)
			{
				return stbtt_GetGlyphKernAdvance(&font->font, glyph1, glyph2);
			}

			static char* get_glyph_bitmap(font_impl* font, int gidx, double height, glm::ivec4* ot, std::string* out, double* advance)
			{
				//int i, j, baseline, ch = 0;
				int baseline;
				int ascent;
				int descent;
				int linegap;
				//float xpos = 2; // leave a little padding in case the character extends left
				float scale = stbtt_ScaleForPixelHeight(&font->font, height);
				stbtt_GetFontVMetrics(&font->font, &ascent, &descent, &linegap);
				baseline = (int)(ascent * scale);
				int advancei, lsb, x0, y0, x1, y1;
				float shift_x = 0.0f;
				float shift_y = 0.0f;
				//stbtt_GetCodepointHMetrics(&font->font, ch, &advance, &lsb);
				stbtt_GetGlyphHMetrics(&font->font, gidx, &advancei, &lsb);
				//stbtt_GetCodepointBitmapBoxSubpixel(&font->font, ch, scale, scale, shiftX, shiftY, &x0, &y0, &x1, &y1);
				stbtt_GetGlyphBitmapBoxSubpixel(&font->font, gidx, scale, scale, shift_x, shift_y, &x0, &y0, &x1, &y1);
				double adv = advancei * scale;
				*advance = adv;
				ot->x = x0;
				ot->y = -y0;
				ot->z = x1 - x0;
				ot->w = y1 - y0;
				size_t pcs = (int64_t)ot->z * ot->w;
				if (out->size() < pcs)
				{
					out->resize(pcs);
				}
				char* pxs = out->data();
				memset(pxs, 0, out->size());
				stbtt_MakeGlyphBitmapSubpixel(&font->font,
					(unsigned char*)pxs,
					x1 - x0,
					y1 - y0,
					x1 - x0, // screen width ( stride )
					scale, scale,
					shift_x, shift_y, // shift x, shift y 
					gidx);

				return pxs;
			}
		public:
			static std::string get_font_name(font_impl* font)
			{
				int len = 0;
				auto str = getFontNameString(&font->font);
				return str ? str : "";
			}
#if 1

			static uint16_t ttUSHORT(uint8_t * p) { return p[0] * 256 + p[1]; }
			static uint16_t ttSHORT(uint8_t* p) { return p[0] * 256 + p[1]; }
			static uint32_t ttULONG(uint8_t* p) { return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]; }
			static int ttLONG(uint8_t* p) { return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]; }
#ifndef stbtt_tag4
#define stbtt_tag4(p,c0,c1,c2,c3) ((p)[0] == (c0) && (p)[1] == (c1) && (p)[2] == (c2) && (p)[3] == (c3))
#define stbtt_tag(p,str)           stbtt_tag4(p,str[0],str[1],str[2],str[3])
#endif // !stbtt_tag4

			// @OPTIMIZE: binary search
			static uint32_t find_table(uint8_t* data, uint32_t fontstart, const char* tag)
			{
				int num_tables = ttUSHORT(data + fontstart + 4);
				uint32_t tabledir = fontstart + 12;
				int i;
				char* t = 0;
				for (i = 0; i < num_tables; ++i) {
					uint32_t loc = tabledir + 16 * i;
					t = (char*)data + loc + 0;
					if (stbtt_tag(data + loc + 0, tag))
						return ttULONG(data + loc + 8);
				}
				return 0;
			}
#endif // !1
			static void enum_table(uint8_t* data, uint32_t fontstart, std::map<std::string, sfnt_header>& out)
			{
				char buf[128];
				memcpy(buf, data + fontstart, 128);
				int num_tables = ttUSHORT(data + fontstart + 4);
				memcpy(buf, data + fontstart + 4, 4);
				uint32_t tabledir = fontstart + 12;
				int i;
				char* t = 0;
				std::string n;
				sfnt_header sh;
				if (num_tables > 0)
				{
					for (i = 0; i < num_tables; ++i) {
						uint32_t loc = tabledir + 16 * i;
						t = (char*)data + loc + 0;
						memcpy(buf, t, 128);
						n.assign(t, 4);
						sh.tag = ttULONG(data + loc + 0);
						sh.checksum = ttULONG(data + loc + 4);
						sh.offset = ttULONG(data + loc + 8);
						sh.logicalLength = ttULONG(data + loc + 12);
						out[n] = sh; //{sh.tag, sh.checksum, sh.offset, sh.logicalLength};
					}
				}
			}

			// 获取字体信息
			static const char* getFontNameString(const stbtt_fontinfo* font)
			{
				int i, count, stringOffset;
				uint8_t* fc = font->data;
				uint32_t offset = font->fontstart;
				uint32_t nm = find_table(fc, offset, "name");
				if (!nm) return 0;
				tt_info* ttp = (tt_info*)font->userdata;
				count = ttUSHORT(fc + nm + 2);
				stringOffset = nm + ttUSHORT(fc + nm + 4);
				for (i = 0; i < count; ++i) {
					uint32_t loc = nm + 6 + 12 * i;
					int platform = ttUSHORT(fc + loc + 0);
					int encoding = ttUSHORT(fc + loc + 2);
					int language = ttUSHORT(fc + loc + 4);
					int nameid = ttUSHORT(fc + loc + 6);
					int length = ttUSHORT(fc + loc + 8);
					const char* name = (const char*)(fc + stringOffset + ttUSHORT(fc + loc + 10));
					ttp->add_info(platform, encoding, language, nameid, name, length);
				}
				return NULL;
			}
			class eblc_h
			{
			public:
				uint16_t majorVersion, minorVersion;
				uint32_t numSizes;
			public:
				eblc_h()
				{
				}

				~eblc_h()
				{
				}

			private:

			};
			//// 获取ebdt信息
			//static const char* get_ebdt(font_impl* font_i)
			//{
			//	const stbtt_fontinfo* font = &font_i->font;
			//	int i, count, stringOffset;
			//	uint8_t* fc = font->data;
			//	uint32_t offset = font->fontstart;
			//	uint32_t nm = get_tag(font_i, "EBDT");
			//	uint32_t eblc = get_tag(font_i, "EBLC");
			//	uint32_t ebsc = get_tag(font_i, "EBSC");
			//	if (!nm) return 0;
			//	tt_info* ttp = (tt_info*)font->userdata;
			//	int majorVersion = ttUSHORT(fc + nm + 0);
			//	int minorVersion = ttUSHORT(fc + nm + 2);
			//	//eblc
			//	uint32_t numSizes = ttULONG(fc + eblc + 4);
			//	char* b = (char*)fc + eblc;
			//	eblc_h* eblcp = (eblc_h*)b;

			//	return NULL;
			//}
		public:
			// 获取字体轮廓
			static stbtt_vertex* get_char_shape(font_impl* font, const char* str, int& verCount)
			{
				stbtt_vertex* stbVertex = NULL;
				verCount = 0;
				auto idx = get_glyph_index(font, str);
				if (!(idx < 0))
					verCount = stbtt_GetGlyphShape(&font->font, idx, &stbVertex);
				return stbVertex;
			}
			static void free_shape(stbtt_fontinfo* font, stbtt_vertex* v)
			{
				stbtt_FreeShape(font, v);
			}
			static int init_table(font_impl* font, unsigned char* data, int fontstart)
			{
				enum_table(data, fontstart, font->_tb);
				return 1;
			}
		private:
		};//！stb_font
		typedef stb_font font_dev;
#endif

		typedef font_dev::font_impl font_impl_info;

		class ttc_info
		{
		public:
			char tag[4];
			unsigned int version;
			unsigned int count;
			unsigned int offset[8];
		public:
			ttc_info()
			{
			}

			~ttc_info()
			{
			}

		private:

		};

		class glyph_shape
		{
		public:
			stbtt_vertex* stbVertex = NULL;
			std::vector<stbtt_vertex> v;
			int verCount = 0;
			stbtt_fontinfo* font = 0;
			std::string fmt;
			std::vector<glm::vec2> _path;
		public:
			glyph_shape()
			{
			}

			~glyph_shape()
			{
				font_dev::free_shape(font, stbVertex);
			}
			void set()
			{
				convert(stbVertex, verCount);
			}
			void convert(stbtt_vertex* stbVertex, int verCount)
			{
				glm::vec2 current;
				glm::vec2 previous;
				_path.clear();
				fmt.clear();
				for (int nIdx = 0; nIdx < verCount; nIdx++)
				{
					stbtt_vertex* stbVer = ((stbtt_vertex*)stbVertex + nIdx);
					switch (stbVer->type)
					{
					case STBTT_vmove:
						current.x = stbVer->x;
						current.y = stbVer->y;
						if (fmt.size())
							fmt.push_back('z');
						fmt.push_back('M');
						_path.push_back(current);
						break;

					case STBTT_vline:
						//mTess->insertVertex(current);
						current.x = stbVer->x;
						current.y = stbVer->y;
						fmt.push_back('L');
						_path.push_back(current);
						break;

					case STBTT_vcurve:
					{
						//mTess->insertVertex(current);
						glm::vec2 contrl;
						contrl.x = stbVer->cx;
						contrl.y = stbVer->cy;

						previous = current;

						current.x = stbVer->x;
						current.y = stbVer->y;

						fmt.push_back('C');
						_path.push_back(contrl);
						_path.push_back(contrl);
						_path.push_back(current);
						//insert the middle point
						//mTess->insertVertex(_getBezierPoint(previous, contrl, current, 0.5));
					}
					break;
					case STBTT_vcubic:
					{

					}break;
					default:break;
					}
				}
				fmt.push_back('z');
			}
		private:

		};

		class tt_info
		{
		public:

			class info_one
			{
			public:
#ifdef _STD_STR_
				std::wstring name_;
				std::string name_a;
#else
				w_string name_;
				s_string name_a;
#endif
				int platform_id = 0;
				int encoding_id = 0;
				int language_id = 0;
				int name_id = 0;
				int length_ = 0;
				info_one(int platform,
					int encoding,
					int language,
					int nameid,
					const char* name, int len)
				{
					platform_id = platform;
					encoding_id = encoding;
					language_id = language;
					name_id = nameid;
					length_ = len;
					std::wstring w;
					//std::vector<unsigned short> vs,vs1;
					size_t wlen = length_ / 2;
					char* temp = (char*)name;
					if (*temp)
					{
						name_a.assign(temp, length_);
					}
					name_.assign((wchar_t*)name, length_ / 2);
				}

				std::string get_name(bool isu8 = true)
				{
					std::string n;
					if (encoding_id)
					{
#ifdef __json_helper_h__
						n = isu8 ? jsonT::wstring_to_utf8(name_.to_wstr(true)) : jsonT::wstring_to_ansi(name_.to_wstr(true));
#endif // __json_helper_h__
					}
					else {
						n = name_a.c_str();
					}
					return n;
				}
				void print()
				{
					std::string n = get_name(false);
					printf("name：%s\tname_id：%d\tplatform_id：%d\tlanguage_id：%d\tencoding_id：%d\n", n.c_str(), name_id, platform_id, language_id, encoding_id);
				}
			};

#ifndef _FONT_NO_BITMAP
			class SBitDecoder
			{
			public:
				Bitmap bitmap[1] = {};
				std::vector<char> bitdata;
				BigGlyphMetrics metrics[1] = {};
				bool          metrics_loaded;
				bool          bitmap_allocated;
				uint8_t          bit_depth;

				BitmapSizeTable* _bst;
				std::vector<IndexSubTableArray>* _ist;

				uint8_t* ebdt_base;
				uint8_t* eblc_base;
				uint8_t* eblc_limit;
				uint8_t* p, * p_limit;

				uint32_t _strike_index = 0;
				tt_info* _face = 0;

				// 是否在回收状态
				int _recycle = 0;
			public:
				SBitDecoder()
				{

				}

				~SBitDecoder()
				{
				}

				int init(tt_info* face, uint32_t strike_index)
				{
					int ret = 0;
					SBitDecoder* decoder = this;
					if (!face->_ebdt_table || !face->_sbit_table
						|| strike_index >= face->_bsts.size() || strike_index >= face->_index_sub_table.size())
						return 0;
					if (_face == face && _strike_index == strike_index)
					{
						return 1;
					}
					_face = face;
					_strike_index = strike_index;
					font_impl_info* font_i = &face->font;
					const stbtt_fontinfo* font = &font_i->font;
					decoder->_bst = &face->_bsts[strike_index];
					decoder->_ist = &face->_index_sub_table[strike_index];
					uint8_t* fc = font->data;
					decoder->eblc_base = fc + face->_sbit_table->offset;
					decoder->ebdt_base = fc + face->_ebdt_table->offset;
					decoder->metrics_loaded = 0;
					decoder->bitmap_allocated = 0;
					decoder->bit_depth = decoder->_bst->bitDepth.value;
					decoder->p = decoder->eblc_base + decoder->_bst->indexSubTableArrayOffset;
					decoder->p_limit = decoder->p + decoder->_bst->indexTablesSize;
					ret = 1;
					return ret;
				}
			public:
				IndexSubTableArray* get_image_offset(unsigned int glyph_index)
				{
					auto& ist = *_ist;
					for (size_t i = 0; i < ist.size(); i++)
					{
						auto& it = ist[i];
						if (glyph_index >= it.firstGlyphIndex && glyph_index <= it.lastGlyphIndex)
						{
							return &ist[i];
						}
					}
					return nullptr;
				}
				int resize_bitmap(uint32_t size)
				{
					if (size > bitdata.size())
					{
						bitmap->capacity = size;
						bitdata.resize(size);
						bitmap->buffer = (unsigned char*)bitdata.data();
					}
					memset(bitmap->buffer, 0, size);
					return 0;
				}
			private:

			};
#endif //!_FONT_NO_BITMAP
		public:
			//k=language_id			2052 简体中文	1033 英语
			std::map<int, std::vector<info_one*>> _detail;

			font_impl_info font;
			std::string name;
			bool is_copy = true;
			std::vector<unsigned char*> _vdata;
			unsigned char* data = 0;
			int dataSize = 0;
			unsigned char freeData = 0;
			float ascender;
			float descender;
			float lineh;
			float fh;
			Glyph* glyphs_ = 0;
			int cglyphs = 0;
			int nglyphs = 0;
			//int lut[256];
			std::vector<tt_info*> fallbacks;

			std::unordered_map<int, Glyph*> _lut;
			LockS _lock;

		private:
#ifndef _FONT_NO_BITMAP

			sbit_table_type _sttype = sbit_table_type::TYPE_NONE;
			// _sbit_table可能是CBLC、EBLC、bloc、sbix
			sfnt_header* _sbit_table = 0;
			sfnt_header* _ebdt_table = 0;
			sfnt_header* _ebsc_table = 0;
			std::vector<BitmapSizeTable> _bsts;
			std::vector<std::vector<IndexSubTableArray>> _index_sub_table;
			std::unordered_map<uint8_t, uint32_t> _msidx_table;
			std::set<SBitDecoder*> _dec_table;
			std::queue<SBitDecoder*> _free_dec;
			LockS _sbit_lock;
#endif // !_FONT_NO_BITMAP

		public:
			tt_info()
			{
			}

			~tt_info()
			{
				destroy_all_dec();
			}
			static tt_info* new_info()
			{
				return new tt_info();
			}
			static void free_info(tt_info* p)
			{
				delete p;
			}
			const char* init(const char* d, size_t s, bool iscp)
			{
				LOCK_W(_lock);
				is_copy = iscp;
				if (is_copy)
				{
					_vdata.resize(s);
					data = (unsigned char*)_vdata.data();
				}
				else
				{
					data = (unsigned char*)d;
				}
				dataSize = s;
				return (char*)data;
			}
			void add_info(int platform,
				int encoding,
				int language,
				int nameid,
				const char* name, int length)
			{
				info_one* e = new info_one(platform, encoding, language, nameid, name, length);
				_detail[language].push_back(e);
			}
			std::vector<info_one*>* get_info(int language)
			{
				LOCK_R(_lock);
				std::vector<info_one*>* ret = nullptr;
				auto it = _detail.find(language);
				if (it != _detail.end())
				{
					ret = &it->second;
				}
				return ret;
			}
			// 获取中文信息
			std::string get_info_str(int language = 2052, int idx = 1)
			{
				LOCK_R(_lock);
				std::string ret;
				int ls[] = { language, 1033 };

				for (int i = 0; i < 2 && ret.empty(); i++)
				{
					auto it = _detail.find(ls[i]);
					if (it != _detail.end())
					{
						auto& p = it->second;
						for (size_t j = 0; j < p.size(); j++)
						{
							if (p[j]->name_id == idx)
							{
								ret = (p[j]->get_name());
							}
						}
					}
				}
				return ret;
			}
			void print_info(int language = -1)
			{
				LOCK_R(_lock);
				std::vector<info_one*>* ret = nullptr;
				auto it = _detail.find(language);
				if (it != _detail.end())
				{
					ret = &it->second;
					for (auto it : *ret)
					{
						it->print();
					}
				}
				else {
					for (auto& [k, v] : _detail)
					{
						for (auto it : v)
						{
							it->print();
						}
						printf("\n");
					}
				}
			}

			Glyph* alloc_glyph()
			{
				Glyph* p = new Glyph();
				return p;
			}
			Glyph* find_glyph(int codepoint)
			{
				LOCK_R(_lock);
				Glyph* p = nullptr;
				auto it = _lut.find(codepoint);
				if (it != _lut.end())
				{
					p = it->second;
				}
				return p;
			}
			void inser_glyph(Glyph* p)
			{
				LOCK_W(_lock);
				auto it = _lut.find(p->codepoint);
				if (it != _lut.end())
				{
					p->next = it->second;
					it->second = p;
				}
				else
				{
					_lut[p->codepoint] = p;
				}
			}
		public:

			int get_glyph_index_u8(const char* u8str, tt_info** renderFont)
			{
				// Create a new glyph or rasterize bitmap data for a cached glyph.
				int g = font_dev::get_glyph_index(&font, u8str);
				// Try to find the glyph in fallback fonts.
				if (g == 0) {
					for (auto it : fallbacks)
					{
						int fallbackIndex = font_dev::get_glyph_index(&it->font, u8str);
						if (fallbackIndex != 0) {
							g = fallbackIndex;
							*renderFont = it;
							break;
						}
					}
					// It is possible that we did not find a fallback glyph.
					// In that case the glyph index 'g' is 0, and we'll proceed below and cache empty glyph.
				}
				else
				{
					*renderFont = this;
				}
				return g;
			}
			int get_glyph_index(unsigned int codepoint, tt_info** renderFont)
			{
				// Create a new glyph or rasterize bitmap data for a cached glyph.
				int g = font_dev::getGlyphIndex(&font, codepoint);
				// Try to find the glyph in fallback fonts.
				if (g == 0) {
					for (auto it : fallbacks)
					{
						int fallbackIndex = font_dev::getGlyphIndex(&it->font, codepoint);
						if (fallbackIndex != 0) {
							g = fallbackIndex;
							*renderFont = it;
							break;
						}
					}
					// It is possible that we did not find a fallback glyph.
					// In that case the glyph index 'g' is 0, and we'll proceed below and cache empty glyph.
				}
				else
				{
					*renderFont = this;
				}
				return g;
			}
		public:
#ifndef _FONT_NO_BITMAP
			// 获取子表
			static void get_index_sub_table(uint8_t* d, unsigned int n, std::vector<IndexSubTableArray>& out)
			{
				out.resize(n);
				for (int i = 0; i < n; i++)
				{
					out[i].firstGlyphIndex = stb_font::ttUSHORT(d);
					out[i].lastGlyphIndex = stb_font::ttUSHORT(d + 2);
					out[i].additionalOffsetToIndexSubtable = stb_font::ttULONG(d + 4);
					d += 8;
				}
			}
			// 获取BitmapSizeTable
			static njson get_bitmap_size_table(uint8_t* blc, unsigned int count,
				std::vector<BitmapSizeTable>& bsts,
				std::vector<std::vector<IndexSubTableArray>>& index_sub_table, std::unordered_map<uint8_t, uint32_t>& ms)
			{
				auto b = blc + 8;
				bsts.resize(count);
				index_sub_table.resize(count);
				njson ns, n;
				for (size_t i = 0; i < count; i++)
				{
					auto& it = bsts[i];
					it.indexSubTableArrayOffset = stb_font::ttULONG(b); //offset to indexSubtableArray from beginning of EBLC.
					b += 4;
					it.indexTablesSize = stb_font::ttULONG(b); //number of bytes in corresponding index subtables and array
					b += 4;
					it.numberOfIndexSubTables = stb_font::ttULONG(b); //an index subtable for each range or format change
					b += 4;
					it.colorRef = stb_font::ttULONG(b); //not used; set to 0.
					b += 4;
					memcpy(&it.hori, b, sizeof(SbitLineMetrics));
					b += sizeof(SbitLineMetrics);
					memcpy(&it.vert, b, sizeof(SbitLineMetrics));
					b += sizeof(SbitLineMetrics);
					it.startGlyphIndex = stb_font::ttUSHORT(b); //lowest glyph index for this size
					b += 2;
					it.endGlyphIndex = stb_font::ttUSHORT(b); //highest glyph index for this size
					b += 2;
					it.ppemX = *b; //horizontal pixels per Em
					b += 1;
					it.ppemY = *b; //vertical pixels per Em
					b += 1;
					it.bitDepth.value = *b;
					b += 1;
					it.flags.raw.value = *b;
					b += 1;
					get_index_sub_table(blc + it.indexSubTableArrayOffset, it.numberOfIndexSubTables, index_sub_table[i]);
					n["ppem"] = { it.ppemX, it.ppemY };
					ms[it.ppemY] = i;
					n["id"] = i;
					ns.push_back(n);
				}
				return ns;
			}
			static int load_metrics(
				uint8_t** pp,
				uint8_t* limit,
				int         big, BigGlyphMetrics* metrics)
			{
				uint8_t* p = *pp;
				if (p + 5 > limit)
					return 0;
				metrics->height = p[0];
				metrics->width = p[1];
				metrics->horiBearingX = (char)p[2];
				metrics->horiBearingY = (char)p[3];
				metrics->horiAdvance = p[4];
				p += 5;
				if (big)
				{
					if (p + 3 > limit)
						return 0;
					metrics->vertBearingX = (char)p[0];
					metrics->vertBearingY = (char)p[1];
					metrics->vertAdvance = p[2];
					p += 3;
				}
				else
				{
					/* avoid uninitialized data in case there is no vertical info -- */
					metrics->vertBearingX = 0;
					metrics->vertBearingY = 0;
					metrics->vertAdvance = 0;
				}
				*pp = p;
				return 1;
			}
			typedef int(*SBitDecoder_LoadFunc)(SBitDecoder* decoder, uint8_t* p, uint8_t* plimit, int x_pos, int y_pos);
			// 复制位图到bitmap
			static int load_byte_aligned(SBitDecoder* decoder, uint8_t* p, uint8_t* limit, int x_pos, int y_pos)
			{
				Bitmap* bitmap = decoder->bitmap;
				uint8_t* line;
				int      pitch, width, height, line_bits, h;
				unsigned int     bit_height, bit_width;
				bit_width = bitmap->width;
				bit_height = bitmap->rows;
				pitch = bitmap->pitch;
				line = bitmap->buffer;

				width = bitmap->width;
				height = bitmap->rows;

				line_bits = width * bitmap->bit_depth;

				if (x_pos < 0 || (unsigned int)(x_pos + width) > bit_width ||
					y_pos < 0 || (unsigned int)(y_pos + height) > bit_height)
				{
					printf("tt_sbit_decoder_load_byte_aligned:"
						" invalid bitmap dimensions\n"); return 0;
				}
				if (p + ((line_bits + 7) >> 3) * height > limit)
				{
					printf("tt_sbit_decoder_load_byte_aligned: broken bitmap\n");
					return 0;
				}
				/* now do the blit */
				line += y_pos * pitch + (x_pos >> 3);
				x_pos &= 7;

				if (x_pos == 0)
				{
					for (h = height; h > 0; h--, line += pitch)
					{
						uint8_t* pwrite = line;
						int    w;
						for (w = line_bits; w >= 8; w -= 8)
						{
							pwrite[0] = (uint8_t)(pwrite[0] | *p++);
							pwrite += 1;
						}
						if (w > 0)
							pwrite[0] = (uint8_t)(pwrite[0] | (*p++ & (0xFF00U >> w)));
					}
				}
				else  /* x_pos > 0 */
				{
					for (h = height; h > 0; h--, line += pitch)
					{
						uint8_t* pwrite = line;
						int    w;
						uint32_t   wval = 0;
						for (w = line_bits; w >= 8; w -= 8)
						{
							wval = (uint32_t)(wval | *p++);
							pwrite[0] = (uint8_t)(pwrite[0] | (wval >> x_pos));
							pwrite += 1;
							wval <<= 8;
						}
						if (w > 0)
							wval = (uint32_t)(wval | (*p++ & (0xFF00U >> w)));
						/* 读取所有位，并有'x_pos+w'位要写入 */
						pwrite[0] = (uint8_t)(pwrite[0] | (wval >> x_pos));

						if (x_pos + w > 8)
						{
							pwrite++;
							wval <<= 8;
							pwrite[0] = (uint8_t)(pwrite[0] | (wval >> x_pos));
						}
					}
				}

				return 1;
			}
			// 按像素位复制
			static int load_bit_aligned(SBitDecoder* decoder, uint8_t* p, uint8_t* limit, int x_pos, int y_pos)
			{
				Bitmap* bitmap = decoder->bitmap;
				int ret = 1;
				uint8_t* line;
				int      pitch, width, height, line_bits, h, nbits;
				unsigned int     bit_height, bit_width;
				unsigned short   rval;

				bit_width = bitmap->width;
				bit_height = bitmap->rows;
				pitch = bitmap->pitch;
				line = bitmap->buffer;

				width = bit_width;
				height = bit_height;

				line_bits = width * bitmap->bit_depth;

				if (x_pos < 0 || (unsigned int)(x_pos + width) > bit_width ||
					y_pos < 0 || (unsigned int)(y_pos + height) > bit_height)
				{
					printf("tt_sbit_decoder_load_bit_aligned:"
						" invalid bitmap dimensions\n");
					return 0;
				}

				if (p + ((line_bits * height + 7) >> 3) > limit)
				{
					printf("tt_sbit_decoder_load_bit_aligned: broken bitmap\n");
					return 0;
				}

				if (!line_bits || !height)
				{
					/* nothing to do */
					return 0;
				}

				/* now do the blit */

				/* adjust `line' to point to the first byte of the bitmap */
				line += y_pos * pitch + (x_pos >> 3);
				x_pos &= 7;

				/* the higher byte of `rval' is used as a buffer */
				rval = 0;
				nbits = 0;

				for (h = height; h > 0; h--, line += pitch)
				{
					uint8_t* pwrite = line;
					int    w = line_bits;
					/* handle initial byte (in target bitmap) specially if necessary */
					if (x_pos)
					{
						w = (line_bits < 8 - x_pos) ? line_bits : 8 - x_pos;
						if (h == height)
						{
							rval = *p++;
							nbits = x_pos;
						}
						else if (nbits < w)
						{
							if (p < limit)
								rval |= *p++;
							nbits += 8 - w;
						}
						else
						{
							rval >>= 8;
							nbits -= w;
						}

						*pwrite++ |= ((rval >> nbits) & 0xFF) &
							(~(0xFFU << w) << (8 - w - x_pos));
						rval <<= 8;
						w = line_bits - w;
					}

					/* handle medial bytes */
					for (; w >= 8; w -= 8)
					{
						rval |= *p++;
						*pwrite++ |= (rval >> nbits) & 0xFF;
						rval <<= 8;
					}

					/* handle final byte if necessary */
					if (w > 0)
					{
						if (nbits < w)
						{
							if (p < limit)
								rval |= *p++;
							*pwrite |= ((rval >> nbits) & 0xFF) & (0xFF00U >> w);
							nbits += 8 - w;

							rval <<= 8;
						}
						else
						{
							*pwrite |= ((rval >> nbits) & 0xFF) & (0xFF00U >> w);
							nbits -= w;
						}
					}
				}
				return ret;
			}

			// 解码复制
			static int load_compound(SBitDecoder* decoder, uint8_t* p, uint8_t* limit, int x_pos, int y_pos)
			{
				Bitmap* bitmap = decoder->bitmap;
				int   num_components, error = 0;
				num_components = stb_font::ttUSHORT(p); p += 2;
				for (int i = 0; i < num_components; i++)
				{
					uint32_t  gindex = stb_font::ttUSHORT(p); p += 2;
					char  dx = *p; p++;
					char  dy = *p; p++;
					/* NB: a recursive call */
					error = get_index(decoder, gindex, x_pos + dx, y_pos + dy);
					if (error)
						break;
				}
				return 0;
			}
			// 分配位置空间
			static int alloc_bitmap(SBitDecoder* decoder)
			{
				uint32_t     width, height;
				uint32_t    size;
				Bitmap* bitmap = decoder->bitmap;
				int bit_depth = decoder->bit_depth;
				BigGlyphMetrics* metrics = decoder->metrics;
				width = metrics->width;
				height = metrics->height;

				bitmap->width = width;
				bitmap->rows = height;
				bitmap->bit_depth = bit_depth;

				switch (bit_depth)
				{
				case 1:
					bitmap->pixel_mode = Pixel_Mode::PX_MONO;
					bitmap->pitch = (int)((bitmap->width + 7) >> 3);
					bitmap->num_grays = 2;
					break;

				case 2:
					bitmap->pixel_mode = Pixel_Mode::PX_GRAY2;
					bitmap->pitch = (int)((bitmap->width + 3) >> 2);
					bitmap->num_grays = 4;
					break;

				case 4:
					bitmap->pixel_mode = Pixel_Mode::PX_GRAY4;
					bitmap->pitch = (int)((bitmap->width + 1) >> 1);
					bitmap->num_grays = 16;
					break;

				case 8:
					bitmap->pixel_mode = Pixel_Mode::PX_GRAY;
					bitmap->pitch = (int)(bitmap->width);
					bitmap->num_grays = 256;
					break;

				case 32:
					bitmap->pixel_mode = Pixel_Mode::PX_BGRA;
					bitmap->pitch = (int)(bitmap->width * 4);
					bitmap->num_grays = 256;
					break;

				default:
					return 0;
				}

				size = bitmap->rows * (uint32_t)bitmap->pitch;

				/* check that there is no empty image */
				if (size == 0)
					return 0;     /* exit successfully! */
				decoder->resize_bitmap(size);
				return 1;
			}
			// 解析加载位图
			static int load_bitmap(
				SBitDecoder* decoder,
				uint32_t glyph_format,
				uint32_t glyph_start,
				uint32_t glyph_size,
				int x_pos, int y_pos)
			{
				uint8_t* p, * p_limit, * data;
				data = decoder->ebdt_base + glyph_start;
				p = data;
				p_limit = p + glyph_size;
				//uint8_t* ebdt_base,  Bitmap* bitmap,
				// 根据字形格式glyph_format读取数据int bit_depth, 
				BigGlyphMetrics* metrics = decoder->metrics;
				switch (glyph_format)
				{
				case 1:
				case 2:
				case 8:
				case 17:
					load_metrics(&p, p_limit, 0, metrics);
					break;
				case 6:
				case 7:
				case 9:
				case 18:
					load_metrics(&p, p_limit, 1, metrics);
					break;
				default:
					return 0;
				}
				SBitDecoder_LoadFunc  loader = 0;
				int ret = 0;
				{
					switch (glyph_format)
					{
					case 1:
					case 6:
						loader = load_byte_aligned;
						break;

					case 2:
					case 7:
					{
						/* Don't trust `glyph_format'.  For example, Apple's main Korean */
						/* system font, `AppleMyungJo.ttf' (version 7.0d2e6), uses glyph */
						/* format 7, but the data is format 6.  We check whether we have */
						/* an excessive number of bytes in the image: If it is equal to  */
						/* the value for a byte-aligned glyph, use the other loading     */
						/* routine.                                                      */
						/*                                                               */
						/* Note that for some (width,height) combinations, where the     */
						/* width is not a multiple of 8, the sizes for bit- and          */
						/* byte-aligned data are equal, for example (7,7) or (15,6).  We */
						/* then prefer what `glyph_format' specifies.                    */

						int  width = metrics->width;
						int  height = metrics->height;

						int  bit_size = (width * height + 7) >> 3;
						int  byte_size = height * ((width + 7) >> 3);


						if (bit_size < byte_size &&
							byte_size == (int)(p_limit - p))
							loader = load_byte_aligned;
						else
							loader = load_bit_aligned;
					}
					break;

					case 5:
						loader = load_bit_aligned;
						break;

					case 8:
						if (p + 1 > p_limit)
							return 0;

						p += 1;  /* skip padding */
						/* fall-through */

					case 9:
						loader = load_compound;
						break;

					case 17: /* small metrics, PNG image data   */
					case 18: /* big metrics, PNG image data     */
					case 19: /* metrics in EBLC, PNG image data */
#ifdef FT_CONFIG_OPTION_USE_PNG
						loader = load_png;
						break;
#else
						return 0;
#endif /* FT_CONFIG_OPTION_USE_PNG */

					default:
						return 0;
					}
					ret = alloc_bitmap(decoder);
					if (!ret)
						return ret;
				}

				ret = loader(decoder, p, p_limit, x_pos, y_pos);

				return ret;
			}

			// 获取一个字体索引的位图
			static int get_index(SBitDecoder* decoder, unsigned int glyph_index, int x_pos, int y_pos)
			{
				uint32_t image_start = 0, image_end = 0, image_offset = 0;
				uint32_t   start, end, index_format, image_format;
				uint8_t* p = decoder->p, * p_limit = decoder->p_limit;
				auto it = decoder->get_image_offset(glyph_index);
				if (!it)
				{
					return 0;
				}
				start = it->firstGlyphIndex;
				end = it->lastGlyphIndex;
				image_offset = it->additionalOffsetToIndexSubtable;
				if (!image_offset)
				{
					return 0;
				}
				p += image_offset;
				//p_limit
				index_format = stb_font::ttUSHORT(p);
				image_format = stb_font::ttUSHORT(p + 2);
				image_offset = stb_font::ttULONG(p + 4);
				p += 8;
				switch (index_format)
				{
				case 1: /* 4-byte offsets relative to `image_offset' */
					p += 4 * (glyph_index - start);
					if (p + 8 > p_limit)
						return 0;
					image_start = stb_font::ttULONG(p);
					p += 4;
					image_end = stb_font::ttULONG(p);
					p += 4;
					if (image_start == image_end)  /* missing glyph */
						return 0;
					break;
				case 2: /* big metrics, constant image size */
				{
					uint32_t  image_size;
					if (p + 12 > p_limit)
						return 0;
					image_size = stb_font::ttULONG(p);
					p += 4;
					if (!load_metrics(&p, p_limit, 1, decoder->metrics))return 0;
					image_start = image_size * (glyph_index - start);
					image_end = image_start + image_size;
				}
				break;
				case 3: /* 2-byte offsets relative to 'image_offset' */
					p += 2 * (glyph_index - start);
					if (p + 4 > p_limit)
						return 0;
					image_start = stb_font::ttUSHORT(p);
					p += 2;
					image_end = stb_font::ttUSHORT(p);
					p += 2;
					if (image_start == image_end)  /* missing glyph */
						return 0;
					break;
				case 4: /* sparse glyph array with (glyph,offset) pairs */
				{
					unsigned int  mm, num_glyphs;
					if (p + 4 > p_limit)
						return 0;
					num_glyphs = stb_font::ttULONG(p);
					p += 4;
					/* overflow check for p + ( num_glyphs + 1 ) * 4 */
					if (p + 4 > p_limit ||
						num_glyphs > (unsigned int)(((p_limit - p) >> 2) - 1))
						return 0;
					for (mm = 0; mm < num_glyphs; mm++)
					{
						unsigned int  gindex = stb_font::ttUSHORT(p);
						p += 2;
						if (gindex == glyph_index)
						{
							image_start = stb_font::ttUSHORT(p);
							p += 4;
							image_end = stb_font::ttUSHORT(p);
							break;
						}
						p += 2;
					}
					if (mm >= num_glyphs)
						return 0;
				}
				break;
				case 5: /* constant metrics with sparse glyph codes */
				case 19:
				{
					uint32_t  image_size, mm, num_glyphs;
					if (p + 16 > p_limit)
						return 0;
					image_size = stb_font::ttULONG(p);
					p += 4;
					if (!load_metrics(&p, p_limit, 1, decoder->metrics))return 0;
					num_glyphs = stb_font::ttULONG(p);
					p += 4;
					/* overflow check for p + 2 * num_glyphs */
					if (num_glyphs > (uint32_t)((p_limit - p) >> 1))
						return 0;

					for (mm = 0; mm < num_glyphs; mm++)
					{
						uint32_t  gindex = stb_font::ttUSHORT(p);
						p += 2;
						if (gindex == glyph_index)
							break;
					}
					if (mm >= num_glyphs)
						return 0;
					image_start = image_size * mm;
					image_end = image_start + image_size;
				}
				break;

				default:
					return 0;
				}
				if (image_start > image_end)
				{
					return 0;
				}
				image_end -= image_start;
				image_start = image_offset + image_start;
				return load_bitmap(decoder, image_format, image_start, image_end, x_pos, y_pos);
			}
			// 初始化位图信息
			int init_sbit()
			{
				font_impl_info* font_i = &font;
				const stbtt_fontinfo* font = &font_i->font;
				int i, count, stringOffset;
				uint8_t* fc = font->data;
				uint32_t offset = font->fontstart, table_size = 0, sbit_num_strikes = 0;
				uint32_t ebdt_start = 0, ebdt_size = 0;
				sfnt_header* ebdt_table = 0;
				auto sbit_table = get_tag(font_i, TAG_CBLC);
				sbit_table_type stt = sbit_table_type::TYPE_NONE;
				do
				{
					if (sbit_table)
					{
						stt = sbit_table_type::TYPE_CBLC;
						break;
					}
					sbit_table = get_tag(font_i, TAG_EBLC);
					if (sbit_table)
					{
						stt = sbit_table_type::TYPE_EBLC;
						break;
					}
					sbit_table = get_tag(font_i, TAG_bloc);
					if (sbit_table)
					{
						stt = sbit_table_type::TYPE_EBLC;
						break;
					}
					sbit_table = get_tag(font_i, TAG_sbix);
					if (sbit_table)
					{
						stt = sbit_table_type::TYPE_SBIX;
						break;
					}
					else
					{
						// error
						return 0;
					}
				} while (0);
				table_size = sbit_table->logicalLength;
				switch (stt)
				{
				case sbit_table_type::TYPE_EBLC:
				case sbit_table_type::TYPE_CBLC:
				{
					uint8_t* p = fc + sbit_table->offset;
					uint32_t   count;
					int majorVersion = stb_font::ttUSHORT(p + 0);
					int minorVersion = stb_font::ttUSHORT(p + 2);
					uint32_t num_strikes = stb_font::ttULONG(p + 4);
					if (num_strikes >= 0x10000UL)
					{
						return 0;
					}

					/*
					 *  Count the number of strikes available in the table.  We are a bit
					 *  paranoid there and don't trust the data.
					 */
					count = num_strikes;
					if (8 + 48UL * count > table_size)
						count = (uint32_t)((table_size - 8) / 48);
					sbit_num_strikes = count;
				}
				break;
				case sbit_table_type::TYPE_SBIX:
				{
					//TODO:解析SBIX
				}
				break;
				default:
					break;
				}
				do
				{
					ebdt_table = get_tag(font_i, TAG_CBDT);
					if (ebdt_table) break;
					ebdt_table = get_tag(font_i, TAG_EBDT);
					if (ebdt_table) break;
					ebdt_table = get_tag(font_i, TAG_bdat);
					if (!ebdt_table) return 0;
				} while (0);

				auto ebsc_table = get_tag(font_i, TAG_EBSC);
				tt_info* ttp = (tt_info*)font->userdata;
				uint8_t* b = fc + sbit_table->offset;
				count = stb_font::ttULONG(b + 4);
				if (!count)
				{
					return 0;
				}
				// 位图表eblc
				//std::vector<BitmapSizeTable> bsts;
				//std::vector<std::vector<IndexSubTableArray>> index_sub_table;
				njson ns = get_bitmap_size_table(b, count, _bsts, _index_sub_table, _msidx_table);
				for (auto& it : ns)
					printf("%s\n", it.dump().c_str());
				// 位图数据表ebdt
				b = fc + ebdt_table->offset;
				glm::ivec2 version = { stb_font::ttUSHORT(b + 0), stb_font::ttUSHORT(b + 2) };
				_sbit_table = sbit_table;
				_ebdt_table = ebdt_table;
				_ebsc_table = ebsc_table;
				return ns.size();
			}
			int get_sidx(int height)
			{
				auto it = _msidx_table.find(height);
				return it != _msidx_table.end() ? it->second : -1;
			}
			std::unordered_map<uint8_t, uint32_t>* get_msidx_table()
			{
				return &_msidx_table;
			}

			// 创建sbit解码器
			SBitDecoder* new_SBitDecoder()
			{
				SBitDecoder* p = nullptr;
				LOCK_W(_sbit_lock);
#ifndef NO_SBIT_RECYCLE
				if (_free_dec.size())
				{
					p = _free_dec.front();
					_free_dec.pop();
					p->_recycle = 0;
				}
				else
#endif // !NO_SBIT_RECYCLE
				{
					p = new SBitDecoder();
					_dec_table.insert(p);
				}
				return p;
			}
			// 回收
			void recycle(SBitDecoder* p)
			{
				if (p)
				{
					LOCK_W(_sbit_lock);
					// 不是自己的不回收
					auto it = _dec_table.find(p);
					if (it != _dec_table.end() && p->_recycle == 0)
					{
#ifndef NO_SBIT_RECYCLE
						p->_recycle = 1;
						_free_dec.push(p);
#else
						_dec_table.erase(p);
						delete p;
#endif
					}

				}
			}
			void destroy_all_dec()
			{
				LOCK_W(_sbit_lock);
				for (auto it : _dec_table)
					if (it)delete it;
				_dec_table.clear();
			}
			// 传空白的解码指针
			int get_glyph_bitmap_test(std::string str, int height, SBitDecoder* decoder)
			{
				font_impl_info* font_i = &font;
				Bitmap* bitmap = 0;
				int x_pos = 0, y_pos = 0;
				Image img;
				img.resize(512, 512);
				img.clear_color(0);
				const char* t = (char*)str.c_str(); unsigned int wcp = 0;
				int gidx = 0;
				int sidx = get_sidx(height);
				if (sidx < 0)
				{
					return 0;
				}
				int x = 10, y = 10;
				decoder->init(this, sidx);
				bitmap = decoder->bitmap;
				for (int i = 0; t && *t; i++)
				{
					char32_t ch;
					const char* t1 = t, * t2;
					//t = get_u8_last(t, &wcp);
					t = font_dev::get_glyph_index_last(font_i, t, &gidx);
					BigGlyphMetrics* metrics = decoder->metrics;
					if (gidx)
					{
						if (get_index(decoder, gidx, x_pos, y_pos))
						{
							// 灰度图转RGBA
							img.copy_to_image((unsigned char*)bitmap->buffer, bitmap->pitch, { x + metrics->horiBearingX, y + metrics->horiAdvance - metrics->horiBearingY, bitmap->width, bitmap->rows }, 0xff0080ff, 1, 0);
						}
					}
					x += bitmap->width + 2 + metrics->horiBearingX;
				}
				img.saveImage("test_bitmap.png");
				return 1;
			}
			// 传空白的解码指针
			int get_glyph_bitmap(int gidx, int height, glm::ivec4* ot, std::string* out, Bitmap* out_bitmap)
			{
				Bitmap* bitmap = 0;
				int x_pos = 0, y_pos = 0, ret = 0;
				int sidx = get_sidx(height);
				if (sidx < 0)
				{
					return 0;
				}
				int x = 10, y = 10;
				SBitDecoder* decoder = new_SBitDecoder();
				decoder->init(this, sidx);
				bitmap = decoder->bitmap;
				BigGlyphMetrics* metrics = decoder->metrics;

				if (get_index(decoder, gidx, x_pos, y_pos))
				{
					out->resize(bitmap->rows * (uint32_t)bitmap->pitch);
					memcpy(out->data(), bitmap->buffer, out->size());
					ot->x = metrics->horiBearingX;
					ot->y = metrics->horiBearingY;
					auto ha = metrics->horiAdvance;
					bitmap->advance = std::max(metrics->horiAdvance, metrics->vertAdvance);
					ot->z = bitmap->width;
					ot->w = bitmap->rows;
					if (out_bitmap)
					{
						*out_bitmap = *bitmap;
					}
					ret = 1;
					// 灰度图转RGBA
					//img.copy_to_image((unsigned char*)bitmap->buffer, bitmap->pitch,
					//	{ x + metrics->horiBearingX, y + metrics->horiAdvance - metrics->horiBearingY, bitmap->width, bitmap->rows },
					//	0xff0080ff, 1, 0);
				}
				else
				{
					ret = 0;
				}
				recycle(decoder);
				return ret;
			}
#endif  // !_FONT_NO_BITMAP
			/*
			输入
			int gidx			字符索引号
			double height		期望高度
			bool first_bitmap	是否优先查找位图字体
			输出
			glm::ivec4* ot		x,y,z=width,w=height
			std::string* out	输出缓存区
			Bitmap* bitmap		输出位图信息
			返回1成功
			*/
			int get_glyph_image(int gidx, double height, glm::ivec4* ot, std::string* out, Bitmap* bitmap, bool first_bitmap)
			{
				int ret = 0;
				if (gidx > 0)
				{
#ifndef _FONT_NO_BITMAP
					if (first_bitmap)
					{
						ret = get_glyph_bitmap(gidx, height, ot, out, bitmap);
					}
#endif
					if (!ret)
					{
						double advance = height;
						font_dev::get_glyph_bitmap(&font, gidx, height, ot, out, &advance);
						if (bitmap)
						{
							bitmap->buffer = (unsigned char*)out->data();
							bitmap->width = bitmap->pitch = ot->z;
							bitmap->rows = ot->w;
							bitmap->advance = advance;
							bitmap->pixel_mode = PX_GRAY;
							ret = 1;
						}
					}
				}
				return ret;
			}
		private:

		};
		// !tt_info


	private:
		// 数据区
		std::vector<std::vector<char>*> fn_data;
		std::vector<tt_info*> fonts;
		// 根据字体名查找
		std::unordered_map<std::string, std::vector<tt_info*>> _tbs;
		LockS _lk;
		int def_language_id = 2052;
	public:
		Fonts()
		{
		}

		~Fonts()
		{
			for (auto it : fn_data)
			{
				delete it;
			}
			for (auto it : fonts)
			{
				tt_info::free_info(it);
			}
		}
	public:
		void set_langid(int langid)
		{
			def_language_id = langid;
		}
		// 文件名，是否复制数据
		int add_font_file(const std::string& fn)
		{
			int ret = 0;
			std::vector<char>* nd = new std::vector<char>();
			auto& data = *nd;
#ifdef __FILE__h__
			hz::File::read_binary_file(fn, data);
#else

#endif // __FILE__h__
			if (data.size())
			{
				fn_data.push_back(nd);
				nd->swap(data);
				ret = add_font_mem(nd->data(), nd->size(), false);
			}
			return ret;
		}
		int add_font_mem(const char* data, size_t len, bool iscp)
		{
			int ret = 0;
			std::vector<stb_font::font_impl*> fp;
			stb_font ft;
			int nums = ft.get_numbers(data);
			int hr = 0;
			njson ftn;
			for (size_t i = 0; i < nums; i++)
			{
				auto font = tt_info::new_info();
				data = font->init(data, len, iscp && i == 0);
				stb_font::font_impl* fi = &font->font;
				hr = ft.loadFont(fi, data, i, font);
				if (hr)
				{
					int ascent = 0, descent = 0, lineGap = 0;
					ft.getFontVMetrics(fi, &ascent, &descent, &lineGap);
					auto fh = ascent - descent;
					njson fot;
					fot["ascender"] = (float)ascent / (float)fh;
					fot["descender"] = (float)descent / (float)fh;
					fot["lineh"] = (float)(fh + lineGap) / (float)fh;
					fot["_ascent"] = ascent;
					fot["_descent"] = descent;
					fot["_lineGap"] = lineGap;
					auto name = ft.get_font_name(fi);
					ftn.push_back(fot);
					fp.push_back(fi);
					font->fh = fh;
					font->ascender = (float)ascent / (float)fh;
					font->descender = (float)descent / (float)fh;
					font->lineh = (float)(fh + lineGap) / (float)fh;
					font->name = font->get_info_str(def_language_id);

#ifndef _FONT_NO_BITMAP
					font->init_sbit();
#endif // !_FONT_NO_BITMAP
					LOCK_W(_lk);
					_tbs[font->name].push_back(font);
					fonts.push_back(font);

					//font->print_info();
				}
				else
				{
					tt_info::free_info(font);
				}
			}
			return fp.size();
		}

		njson get_font_info(int language_id = 2052)
		{
			LOCK_R(_lk);
			njson ret;;
			for (auto& [k, v] : _tbs)
			{
				for (auto it : v)
				{
					auto n = it->get_info_str(language_id, 1);
					auto ns = it->get_info_str(1033, 2);
					if (ns.empty())
					{
						ns = it->get_info_str(language_id, 2);
					}
					ret[n].push_back(ns);
				}
			}
			return ret;
		}

		tt_info* get_font(const std::string& name, const std::string& st = "Regular")
		{
			LOCK_R(_lk);
			tt_info* ret = nullptr;
			auto it = _tbs.find(name);
			if (it != _tbs.end() && it->second.size())
			{
				for (auto& ft : it->second)
				{
					auto fst = ft->get_info_str(1033, 2);
					if (fst == st)
					{
						ret = ft;
					}
				}
			}
			return ret;
		}
		static sfnt_header* get_tag(font_impl_info* font_i, std::string tag)
		{
			auto it = font_i->_tb.find(tag);
			return it != font_i->_tb.end() ? &it->second : nullptr;
		}
	public:

		bool is_ttc(const char* data, std::vector<uint32_t>* outv)
		{
			uint32_t num_fonts = 0;
			uint32_t offset = 0;
			bool ret = false;
			ttc_info* tip = (ttc_info*)data;

			if (memcmp(data, "ttcf", 4) != 0)
				return ret;
			num_fonts = swapInt32(tip->count);
			//tip->version = swapInt32(tip->version);
			if (tip->count)
			{
				for (size_t i = 0; i < num_fonts; i++)
				{
					offset = swapInt32(tip->offset[i]);
					if (outv)
					{
						outv->push_back(offset);
					}
				}
				ret = true;
			}
			return ret;
		}
		//对应int32大小的成员 的转换 范例 
		int32_t swapInt32(int32_t value)
		{
			return ((value & 0x000000FF) << 24) |
				((value & 0x0000FF00) << 8) |
				((value & 0x00FF0000) >> 8) |
				((value & 0xFF000000) >> 24);
		}
		static char* tt_ushort(char* p)
		{
			std::swap(p[0], p[1]);
			return p + 2;
		}
		union intWithFloat
		{
			int32_t m_i32;
			float m_f32;
		};

		float swapFloat32(float value)
		{
			intWithFloat i;
			i.m_i32 = value;
			i.m_f32 = swapInt32(i.m_i32);
			return i.m_f32;
		}
	public:

		// Based on Exponential blur, Jani Huhtanen, 2006

#define APREC 16
#define ZPREC 7

		static void blurCols(unsigned char* dst, int w, int h, int dstStride, int alpha)
		{
			int x, y;
			for (y = 0; y < h; y++) {
				int z = 0; // force zero border
				for (x = 1; x < w; x++) {
					z += (alpha * (((int)(dst[x]) << ZPREC) - z)) >> APREC;
					dst[x] = (unsigned char)(z >> ZPREC);
				}
				dst[w - 1] = 0; // force zero border
				z = 0;
				for (x = w - 2; x >= 0; x--) {
					z += (alpha * (((int)(dst[x]) << ZPREC) - z)) >> APREC;
					dst[x] = (unsigned char)(z >> ZPREC);
				}
				dst[0] = 0; // force zero border
				dst += dstStride;
			}
		}

		static void blurRows(unsigned char* dst, int w, int h, int dstStride, int alpha)
		{
			int x, y;
			for (x = 0; x < w; x++) {
				int z = 0; // force zero border
				for (y = dstStride; y < h * dstStride; y += dstStride) {
					z += (alpha * (((int)(dst[y]) << ZPREC) - z)) >> APREC;
					dst[y] = (unsigned char)(z >> ZPREC);
				}
				dst[(h - 1) * dstStride] = 0; // force zero border
				z = 0;
				for (y = (h - 2) * dstStride; y >= 0; y -= dstStride) {
					z += (alpha * (((int)(dst[y]) << ZPREC) - z)) >> APREC;
					dst[y] = (unsigned char)(z >> ZPREC);
				}
				dst[0] = 0; // force zero border
				dst++;
			}
		}


		static void blur(unsigned char* dst, int w, int h, int dstStride, float blur, int n)
		{
			int alpha;
			float sigma;
			if (blur < 1)
				return;
			// Calculate the alpha such that 90% of the kernel is within the radius. (Kernel extends to infinity)
			sigma = (float)blur * 0.57735f; // 1 / sqrt(3)
			alpha = (int)((1 << APREC) * (1.0f - expf(-2.3f / (sigma + 1.0f))));
			for (int i = 0; i < n; i++)
			{
				blurRows(dst, w, h, dstStride, alpha);
				blurCols(dst, w, h, dstStride, alpha);
			}
		}
		glyph_shape* get_glyph_shape(tt_info* font, const char* str)
		{
			if (!font && fonts.size())
			{
				font = fonts[0];
			}
			else
			{
				return nullptr;
			}
			int vc = 0;
			auto gs = font_dev::get_char_shape(&font->font, str, vc);
			glyph_shape* ret = nullptr;
			if (vc)
			{
				ret = new glyph_shape();
				ret->stbVertex = gs;
				ret->verCount = vc;
				ret->font = &font->font.font;
				ret->v.assign(gs, gs + vc);
				ret->set();
			}
			return ret;
		}
		// 获取字形
		Glyph* get_glyph(tt_info* font, const char* codepoint, short isize, short iblur, int bitmapOption = FONS_GLYPH_BITMAP_REQUIRED)
		{
			return get_glyph(font, font_dev::get_u8_to_u16(codepoint), isize, iblur, bitmapOption);
		}
		char* get_glyph(tt_info* font, unsigned int codepoint, float height, glm::ivec4* ot, std::string* out)
		{
			int i, g, advance, lsb, x0, y0, x1, y1, gw, gh, gx = 0, gy = 0, x, y;
			float scale;
			Glyph* glyph = NULL;
			unsigned int h;
			int pad, added;
			unsigned char* bdst;
			unsigned char* dst;
			tt_info* renderFont = font;
			if (!font && fonts.size())
			{
				renderFont = fonts[0];
			}
			if (!renderFont)
			{
				return nullptr;
			}
			g = font->get_glyph_index(codepoint, &renderFont);
			double adv;
			return font_dev::get_glyph_bitmap(&renderFont->font, g, height, ot, out, &adv);
		}
		Glyph* get_glyph(tt_info* font, unsigned int codepoint, short isize, short iblur, int bitmapOption = FONS_GLYPH_BITMAP_REQUIRED)
		{
			int i, g, advance, lsb, x0, y0, x1, y1, gw, gh, gx = 0, gy = 0, x, y;
			float scale;
			Glyph* glyph = NULL;
			unsigned int h;
			float size = isize; // 10.0f;
			int pad, added;
			unsigned char* bdst;
			unsigned char* dst;
			if (!font && fonts.size())
			{
				font = fonts[0];
			}
			else
			{
				//return glyph;
			}

			tt_info* renderFont = font;
			if (isize < 2) return NULL;
			if (iblur > 20) iblur = 20;
			pad = iblur + 2;

			// Reset allocator.
			//stash->nscratch = 0;

			// Find code point and size.
			// 查找是否有用过
			auto tg = font->find_glyph(codepoint);
			for (; tg;)
			{
				if (tg->codepoint == codepoint && tg->size == isize && tg->blur == iblur) {
					glyph = tg;
					if (bitmapOption == FONS_GLYPH_BITMAP_OPTIONAL || (glyph->x0 >= 0 && glyph->y0 >= 0)) {
						return glyph;
					}
					// At this point, glyph exists but the bitmap data is not yet created.
					break;
				}
				tg = tg->next;
			}
			// Create a new glyph or rasterize bitmap data for a cached glyph.
			g = font->get_glyph_index(codepoint, &renderFont);

			scale = font_dev::getPixelHeightScale(&renderFont->font, size);
			font_dev::buildGlyphBitmap(&renderFont->font, g, size, scale, &advance, &lsb, &x0, &y0, &x1, &y1);
			gw = x1 - x0 + pad * 2;
			gh = y1 - y0 + pad * 2;

			// Determines the spot to draw glyph in the atlas.
			//if (bitmapOption == FONS_GLYPH_BITMAP_REQUIRED) {
			//	// Find free spot for the rect in the atlas
			//	added = atlasAddRect(stash->atlas, gw, gh, &gx, &gy);
			//	if (added == 0 && stash->handleError != NULL) {
			//		// Atlas is full, let the user to resize the atlas (or not), and try again.
			//		stash->handleError(stash->errorUptr, FONS_ATLAS_FULL, 0);
			//		added = atlasAddRect(stash->atlas, gw, gh, &gx, &gy);
			//	}
			//	if (added == 0) return NULL;
			//}
			//else {
			//	// Negative coordinate indicates there is no bitmap data created.
			//	gx = -1;
			//	gy = -1;
			//}

			// Init glyph.
			if (glyph == NULL) {
				glyph = font->alloc_glyph();
				glyph->codepoint = codepoint;
				glyph->size = isize;
				glyph->blur = iblur;
				glyph->next = 0;

				// Insert char to hash lookup.
				font->inser_glyph(glyph);
				//glyph->next = font->lut[h];
				//font->lut[h] = font->nglyphs - 1;
			}
			glyph->index = g;
			glyph->x0 = (short)gx;
			glyph->y0 = (short)gy;
			glyph->x1 = (short)(glyph->x0 + gw);
			glyph->y1 = (short)(glyph->y0 + gh);
			glyph->xadv = (short)(scale * advance * 10.0f);
			glyph->xoff = (short)(x0 - pad);
			glyph->yoff = (short)(y0 - pad);

			if (bitmapOption == FONS_GLYPH_BITMAP_OPTIONAL) {
				return glyph;
			}
			// Rasterize
			//dst = &stash->texData[(glyph->x0 + pad) + (glyph->y0 + pad) * stash->params.width];
			int gwh[] = { gw - pad * 2, gh - pad * 2 };
			int params_width = gwh[0];
			int params_height = gwh[1];
			dst = glyph->resize_bitmap(params_width, params_height);
			font_dev::renderGlyphBitmap(&renderFont->font, dst, gwh[0], gwh[1], params_width, scale, scale, g);
			{
				int w, h, i, j, c = 'a';
				auto bitmap = stbtt_GetCodepointBitmap(&renderFont->font.font, 0, stbtt_ScaleForPixelHeight(&renderFont->font.font, 20), c, &w, &h, 0, 0);
				//print_test(bitmap, w, h);
			}
			//print_test(dst, params_width, params_height);
			// Make sure there is one pixel empty border.
			auto mdst = &dst[glyph->x0 + glyph->y0 * params_width];
			for (y = 0; y < gh; y++) {
				mdst[y * params_width] = 0;
				mdst[gw - 1 + y * params_width] = 0;
			}
			for (x = 0; x < gw; x++) {
				mdst[x] = 0;
				mdst[x + (gh - 1) * params_width] = 0;
			}
			// Debug code to color the glyph background
		/*	unsigned char* fdst = &stash->texData[glyph->x0 + glyph->y0 * params_width];
			for (y = 0; y < gh; y++) {
				for (x = 0; x < gw; x++) {
					int a = (int)fdst[x+y*params_width] + 20;
					if (a > 255) a = 255;
					fdst[x+y*params_width] = a;
				}
			}*/

			// Blur
			if (iblur > 0) {
				//stash->nscratch = 0;
				bdst = &dst[glyph->x0 + glyph->y0 * params_width];
				blur(bdst, gw, gh, params_width, iblur, 2);
			}
			//print_test(dst, params_width, params_height);

			int dirtyRect[4] = {};
			dirtyRect[0] = mini(dirtyRect[0], glyph->x0);
			dirtyRect[1] = mini(dirtyRect[1], glyph->y0);
			dirtyRect[2] = maxi(dirtyRect[2], glyph->x1);
			dirtyRect[3] = maxi(dirtyRect[3], glyph->y1);

			return glyph;
		}


		static void print_test(const void* d, int w, int h)
		{
			unsigned char* bitmap = (unsigned char*)d;
			printf("\n");
			for (int j = 0; j < h; ++j)
			{
				printf("\t");
				for (int i = 0; i < w; ++i)
				{
					printf("%c ", " .:ioVM@"[bitmap[j * w + i] >> 5]);
				}
				printf("\n");
			}
			printf("\n");
		}

		static int mini(int a, int b)
		{
			return a < b ? a : b;
		}

		static int maxi(int a, int b)
		{
			return a > b ? a : b;
		}

		// 获取utf8字符
		static const char* get_u8_last(const char* str, unsigned int* codepoint)
		{
			unsigned int utf8state = 0;
			for (; str && *str; ) {
				if (fons_decutf8(&utf8state, codepoint, *(const unsigned char*)str++))
					continue;
				break;
			}
			return str;
		}
	public:
#ifndef _FONT_NO_BITMAP

#endif // !_FONT_NO_BITMAP

	public:
		/*
		输入路径path，缩放比例scale、bpm_size{宽高xy,每行步长z}，
		xy_off偏移默认0
		输出灰度图
		*/
		static void get_path_bitmap(stbtt_vertex* vertices, int num_verts, std::vector<char>* bmp, glm::ivec3 bpm_size,
			glm::vec2 scale = { 1, 1 }, glm::vec2 xy_off = { 0.0f,0.0f })
		{
			get_glyph_bitmap_subpixel(vertices, num_verts, scale, { 0, 0 }, xy_off, bmp, bpm_size);
		}
		//invert 1倒置, 0正常
		static unsigned char* get_glyph_bitmap_subpixel(stbtt_vertex* vertices, int num_verts, glm::vec2 scale, glm::vec2 shift,
			glm::vec2 xy_off, std::vector<char>* out, glm::ivec3 bpm_size, int invert = 0)
		{
			stbtt__bitmap gbm;
			if ((int)scale.x == 0 || (int)scale.y == 0)
			{
				scale.x = scale.y = std::max(scale.x, scale.y);
			}
			// now we get the size
			gbm.w = bpm_size.x;
			gbm.h = bpm_size.y;
			gbm.pixels = NULL; // in case we error
			int bms = gbm.w * gbm.h;
			if (bms > 0)
			{
				if (out->empty())
					out->resize(bms);
				gbm.pixels = (unsigned char*)out->data();
				if (gbm.pixels)
				{
					gbm.stride = bpm_size.z;
					stbtt_Rasterize(&gbm, 0.35f, vertices, num_verts, scale.x, scale.y, shift.x, shift.y, xy_off.x, xy_off.y, invert, 0);
				}
			}
			return gbm.pixels;
		}

	private:


	}; //！Fonts

}; //!hz
#endif // !__font_h__