#ifndef __font_h__
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

Apple官方TrueType字体标准https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html#Overview

微软官方TrueType字体标准https://www.microsoft.com/en-us/Typography/SpecificationsOverview.aspx
---------------------
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
namespace hz
{

	static char* w2a(wchar_t* data_from, size_t size, char* out, size_t outsize)
	{
		static std::locale sys_locale("");
		const wchar_t* data_from_end = data_from + size;
		const wchar_t* data_from_next = 0;
		char* data_to = (char*)out;
		char* data_to_end = data_to + outsize;
		char* data_to_next = 0;
		typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
		mbstate_t out_state = { 0 };
		auto result = std::use_facet<convert_facet>(sys_locale).out(
			out_state, data_from, data_from_end, data_from_next,
			data_to, data_to_end, data_to_next);
		if (result == convert_facet::ok)
		{
			return data_to;
		}
		return data_to;
	}
#if 0
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
#endif
	/*---------------------------------------------------------------------------------------------------------------------------------------------------
	TODO	字体管理

	----------------------------------------------------------------------------------------------------------------------------------------------------*/
	class Fonts :public Singleton<Fonts>
	{
	public:

		typedef int32_t Fixed;
#define FWord int16_t
#define uFWord uint16_t
#define Card8 uint8_t
#define Card16 uint16_t
#define Card32 uint32_t

#define head_VERSION VERSION(1, 0)

#define DATE_TIME_SIZE 8
		typedef Card8 longDateTime[DATE_TIME_SIZE];

		/* TrueType Collection Header */
		typedef struct
		{
			Card32 TTCTag;
			Fixed Version;
			Card32 DirectoryCount;
			Card32* TableDirectory; /* [DirectoryCount] */
			Card32 DSIGTag;
			Card32 DSIGLength;
			Card32 DSIGOffset;
		} ttcfTbl;
		typedef struct
		{
			Fixed	version;//	0x00010000 if (version 1.0)
			Fixed	fontRevision;//	set by font manufacturer
			uint32_t	checkSumAdjustment;//	To compute : set it to 0, calculate the checksum for the 'head' table and put it in the table directory, sum the entire font as a uint32_t, then store 0xB1B0AFBA - sum. (The checksum for the 'head' table will be wrong as a result.That is OK; do not reset it.)
			uint32_t	magicNumber;//	set to 0x5F0F3CF5
			uint16_t	flags;/*	bit 0 - y value of 0 specifies baseline
			bit 1 - x position of left most black bit is LSB
			bit 2 - scaled point size and actual point size will differ(i.e. 24 point glyph differs from 12 point glyph scaled by factor of 2)
			bit 3 - use integer scaling instead of fractional
			bit 4 - (used by the Microsoft implementation of the TrueType scaler)
			bit 5 - This bit should be set in fonts that are intended to e laid out vertically, and in which the glyphs have been drawn such that an x - coordinate of 0 corresponds to the desired vertical baseline.
			bit 6 - This bit must be set to zero.
			bit 7 - This bit should be set if the font requires layout for correct linguistic rendering(e.g.Arabic fonts).
			bit 8 - This bit should be set for an AAT font which has one or more metamorphosis effects designated as happening by default.
			bit 9 - This bit should be set if the font contains any strong right - to - left glyphs.
			bit 10 - This bit should be set if the font contains Indic - style rearrangement effects.
			bits 11 - 13 - Defined by Adobe.
			bit 14 - This bit should be set if the glyphs in the font are simply generic symbols for code point ranges, such as for a last resort font.
			*/
			uint16_t	unitsPerEm;//	range from 64 to 16384
			longDateTime	created;//	international date
			longDateTime	modified;//	international date
			FWord	xMin;//	for all glyph bounding boxes
			FWord	yMin;//	for all glyph bounding boxes
			FWord	xMax;//	for all glyph bounding boxes
			FWord	yMax;//	for all glyph bounding boxes
			uint16_t	macStyle;/*	bit 0 bold
			bit 1 italic
			bit 2 underline
			bit 3 outline
			bit 4 shadow
			bit 5 condensed(narrow)
			bit 6 extended*/
			uint16_t	lowestRecPPEM;//	smallest readable size in pixels
			int16_t	fontDirectionHint;/*	0 Mixed directional glyphs
			1 Only strongly left to right glyphs
			2 Like 1 but also contains neutrals
			- 1 Only strongly right to left glyphs
			- 2 Like - 1 but also contains neutrals*/
			int16_t	indexToLocFormat;//	0 for short offsets, 1 for long
			int16_t	glyphDataFormat;//	0 for current format

		}head_table;
		// Entry
		typedef struct sfnt_header_
		{
			unsigned int tag = 0;
			unsigned int checksum;
			unsigned int offset; //From beginning of header.
			unsigned int logicalLength;
		}sfnt_header;
		typedef struct
		{
			Fixed version;
			Card16 numTables;
			Card16 searchRange;
			Card16 entrySelector;
			Card16 rangeShift;
			sfnt_header* directory; /* [numTables] */
		} sfntTbl;
		typedef struct post_header_
		{
			int32_t	format;//	Format of this table
			int32_t	italicAngle;	//Italic angle in degrees
			int16_t	underlinePosition;	//Underline position
			int16_t	underlineThickness;	//Underline thickness
			uint32_t	isFixedPitch;	//Font is monospaced; set to 1 if the font is monospaced and 0 otherwise(N.B., to maintain compatibility with older versions of the TrueType spec, accept any non - zero value as meaning that the font is monospaced)
			uint32_t	minMemType42;	//Minimum memory usage when a TrueType font is downloaded as a Type 42 font
			uint32_t	maxMemType42;	//Maximum memory usage when a TrueType font is downloaded as a Type 42 font
			uint32_t	minMemType1;	//Minimum memory usage when a TrueType font is downloaded as a Type 1 font
			uint32_t	maxMemType1;
		}post_header;
		typedef struct
		{
			Fixed version;
			FWord ascender;
			FWord descender;
			FWord lineGap;
			uFWord advanceWidthMax;
			FWord minLeftSideBearing;
			FWord minRightSideBearing;
			FWord xMaxExtent;
			int16_t caretSlopeRise;
			int16_t caretSlopeRun;
			int16_t caretOffset;
			int16_t reserved[4];
			int16_t metricDataFormat;
			uint16_t numberOfLongHorMetrics;
		} hheaTbl;


		typedef struct {
			uint16_t format;
			uint16_t length;
			uint16_t language;
			unsigned char glyphId[256];
		} Format0;
#define FORMAT0_SIZE (uint16 * 3 + uint8 * 256)
		/*
		Format 2: High - byte mapping through table
		*/
		typedef struct {
			unsigned short firstCode;
			unsigned short entryCount;
			short idDelta;
			unsigned short idRangeOffset;
		} Segment2;
#define SEGMENT2_SIZE (uint16 * 3 + int16 * 1)

		typedef struct {
			unsigned short format;
			unsigned short length;
			unsigned short language;
			unsigned short segmentKeys[256];
			Segment2* segment;
			unsigned short* glyphId;
		} Format2;
		// ------------------------------------------------------------------------------------------------
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
			void* data = 0;
			unsigned char   palette_mode;
			void* palette;
		} Bitmap;

		union ft_key_s
		{
			uint64_t u = 0;
			struct
			{
				char32_t unicode_codepoint;
				unsigned short font_dpi;
				// 字号支持 1-255
				unsigned char font_size;
				// 模糊大小支持 0-255
				unsigned char blur_size;
			}v;
		};
		union ft_char_s
		{
			uint64_t u = 0;
			struct
			{
				char32_t unicode_codepoint;
				unsigned short font_dpi;
				// 字号支持 1-255
				unsigned char font_size;
			}v;
		};
		class ft_item
		{
		public:
			Image* _image = nullptr;
			char32_t _unicode_codepoint = 0;
			// 缓存位置xy, 字符图像zw(width,height)
			glm::ivec4 _rect;
			// 基线偏移
			glm::ivec2 _baseline;
			double _baseline_f = 0.0;
			// 步进，等宽字体一般会英文相同、中文相同
			double _advance = 0.0;
			unsigned char _blur_size = 0;
			int count = 0;
		public:
			ft_item()
			{
			}
			~ft_item()
			{
			}
			void set_it(char32_t codepoint, Image* img, const glm::ivec4& rect, const glm::ivec2& baseline, double advance, unsigned char blur_size, double baselinef)
			{
				_unicode_codepoint = codepoint;
				_image = img;
				_rect = rect;
				_baseline = baseline;
				_baseline_f = baselinef;
				_advance = advance;
				_blur_size = blur_size;
			}
			void set_it(const ft_item& fit)
			{
				*this = fit;
			}
		private:
		};
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


#define DL_long(v) v=ttLONG(data);data+=4
#define DL_short(v) v=ttSHORT(data);data+=2
#define DL_ulong(v) v=ttULONG(data);data+=4
#define DL_ushort(v) v=ttUSHORT(data);data+=2

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

		/* FIR filter used by the default and light filters */
		static void lcd_filter_fir(Fonts::Bitmap* bitmap, int  mode, int weights_type)
		{
			static const uint8_t  default_filter[5] = { 0x08, 0x4d, 0x56, 0x4d, 0x08 };
			static const uint8_t  light_filter[5] = { 0x00, 0x55, 0x56, 0x55, 0x00 };

			const uint8_t* weights = weights_type ? default_filter : light_filter;
			uint32_t   width = (uint32_t)bitmap->width;
			uint32_t   height = (uint32_t)bitmap->rows;


			/* horizontal in-place FIR filter */
			if (mode == 0 && width >= 4)
			{
				unsigned char* line = bitmap->buffer;


				/* take care of bitmap flow */
				if (bitmap->pitch < 0)
					line -= bitmap->pitch * (int64_t)(bitmap->rows - 1);

				/* `fir' and `pix' must be at least 32 bit wide, since the sum of */
				/* the values in `weights' can exceed 0xFF                        */

				for (; height > 0; height--, line += bitmap->pitch)
				{
					uint32_t  fir[4];        /* below, `pix' is used as the 5th element */
					uint32_t  val1, xx;


					val1 = line[0];
					if (val1)
					{
						val1 = val1;
					}
					fir[0] = weights[2] * val1;
					fir[1] = weights[3] * val1;
					fir[2] = weights[4] * val1;
					fir[3] = 0;

					val1 = line[1];
					fir[0] += weights[1] * val1;
					fir[1] += weights[2] * val1;
					fir[2] += weights[3] * val1;
					fir[3] += weights[4] * val1;

					for (xx = 2; xx < width; xx++)
					{
						uint32_t  val, pix;


						val = line[xx];
						pix = fir[0] + weights[0] * val;
						fir[0] = fir[1] + weights[1] * val;
						fir[1] = fir[2] + weights[2] * val;
						fir[2] = fir[3] + weights[3] * val;
						fir[3] = weights[4] * val;

						pix >>= 8;
						pix |= (uint32_t)-(int)(pix >> 8);
						line[xx - 2] = (unsigned char)pix;
					}

					{
						uint32_t  pix;


						pix = fir[0] >> 8;
						pix |= (uint32_t)-(int)(pix >> 8);
						line[xx - 2] = (unsigned char)pix;

						pix = fir[1] >> 8;
						pix |= (uint32_t)-(int)(pix >> 8);
						line[xx - 1] = (unsigned char)pix;
					}
				}
			}

			/* vertical in-place FIR filter */
			else if (mode == 1 && height >= 4)
			{
				unsigned char* column = bitmap->buffer;
				int    pitch = bitmap->pitch;


				/* take care of bitmap flow */
				if (bitmap->pitch < 0)
					column -= (int64_t)bitmap->pitch * (int64_t)(bitmap->rows - 1);

				for (; width > 0; width--, column++)
				{
					unsigned char* col = column;
					uint32_t   fir[4];       /* below, `pix' is used as the 5th element */
					uint32_t   val1, yy;


					val1 = col[0];
					fir[0] = weights[2] * val1;
					fir[1] = weights[3] * val1;
					fir[2] = weights[4] * val1;
					fir[3] = 0;
					col += pitch;

					val1 = col[0];
					fir[0] += weights[1] * val1;
					fir[1] += weights[2] * val1;
					fir[2] += weights[3] * val1;
					fir[3] += weights[4] * val1;
					col += pitch;

					for (yy = 2; yy < height; yy++)
					{
						uint32_t  val, pix;


						val = col[0];
						pix = fir[0] + weights[0] * val;
						fir[0] = fir[1] + weights[1] * val;
						fir[1] = fir[2] + weights[2] * val;
						fir[2] = fir[3] + weights[3] * val;
						fir[3] = weights[4] * val;

						pix >>= 8;
						pix |= (uint32_t)-(int)(pix >> 8);
						col[-2 * pitch] = (unsigned char)pix;
						col += pitch;
					}

					{
						uint32_t  pix;


						pix = fir[0] >> 8;
						pix |= (uint32_t)-(int)(pix >> 8);
						col[-2 * pitch] = (unsigned char)pix;

						pix = fir[1] >> 8;
						pix |= (uint32_t)-(int)(pix >> 8);
						col[-pitch] = (unsigned char)pix;
					}
				}
			}
		}

		class Glyph
		{
		public:
			unsigned int codepoint = 0;
			int index = 0;
			Glyph* next = 0;
			short size = 0, blur = 0;
			short x0 = 0, y0 = 0, x1 = 0, y1 = 0;
			short xadv = 0, xoff = 0, yoff = 0;
			unsigned int _width = 0, _height = 0;
			std::vector<unsigned char> bitmap;
		public:
			Glyph()
			{
			}

			~Glyph()
			{
			}
			unsigned char* resize_bitmap(int64_t w, int64_t h)
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

				ftError = FT_Set_Pixel_Sizes(font->font, 0, (unsigned int)(size * (float)font->font->units_per_EM / (float)(font->font->ascender - font->font->descender)));
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
			static void getFontHMetrics(font_impl* font, int glyph, int* advance, int* lsb)
			{
				stbtt_GetGlyphHMetrics(&font->font, glyph, advance, lsb);
				return;
			}

			static float getPixelHeightScale(font_impl* font, float size)
			{
				return stbtt_ScaleForPixelHeight(&font->font, size);
			}
			static void getPixelGLScale(font_impl* font, glm::ivec4* ot)
			{
				int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
				stbtt_GetCodepointBox(&font->font, L'g', &x0, &y0, &x1, &y1);
				ot->x = x0;
				ot->y = y0;
				ot->z = x1;
				ot->w = y1;
				ot++;
				stbtt_GetCodepointBox(&font->font, L'l', &x0, &y0, &x1, &y1);
				ot->x = x0;
				ot->y = y0;
				ot->z = x1;
				ot->w = y1;
				ot++;
				stbtt_GetCodepointBox(&font->font, L'中', &x0, &y0, &x1, &y1);
				ot->x = x0;
				ot->y = y0;
				ot->z = x1;
				ot->w = y1;
				return;
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


#define BYTE_( p, i )  ( ((const unsigned char*)(p))[(i)] )
#define BYTE_U16( p, i, s1 )  ( (uint16_t)( BYTE_( p, i ) ) << (s1) )
#define BYTE_U32( p, i, s1 )  ( (uint32_t)( BYTE_( p, i ) ) << (s1) )
#define PEEK_USHORT( p )  uint16_t( BYTE_U16( p, 0, 8 ) | BYTE_U16( p, 1, 0 ) )
#define NEXT_SHORT( b ) ( (short)( b += 2, PEEK_USHORT( b - 2 ) ) )
#define NEXT_USHORT( b ) ( (unsigned short)( b += 2, PEEK_USHORT( b - 2 ) ) )
			static unsigned char* tt_cmap2_get_subheader(unsigned char* table, unsigned int char_code)
			{
				unsigned char* result = NULL;
				if (char_code < 0x10000UL)
				{
					unsigned int   char_lo = (unsigned int)(char_code & 0xFF);
					unsigned int   char_hi = (unsigned int)(char_code >> 8);
					unsigned char* p = table + 6;    /* keys table */
					unsigned char* subs = table + 518;  /* subheaders table */
					unsigned char* sub;
					if (char_hi == 0)
					{
						/* an 8-bit character code -- we use subHeader 0 in this case */
						/* to test whether the character code is in the charmap       */
						/*                                                            */
						sub = subs;
						p += char_lo * 2;
						if (PEEK_USHORT(p) != 0)sub = 0;
					}
					else
					{
						p += char_hi * 2;
						sub = subs + (uint64_t)PEEK_USHORT(p);
					}
					result = sub;
				}
				return result;
			}

			static int tt_cmap2_char_index(unsigned char* table, unsigned int char_code)
			{
				unsigned int   result = 0;
				unsigned char* subheader;
				subheader = tt_cmap2_get_subheader(table, char_code);
				if (subheader)
				{
					unsigned char* p = subheader;
					unsigned int   idx = ((unsigned int)(char_code) & 0xFF);
					unsigned int   start, count;
					int    delta;
					unsigned int   offset;
					start = NEXT_USHORT(p);
					count = NEXT_USHORT(p);
					delta = NEXT_SHORT(p);
					offset = PEEK_USHORT(p);

					idx -= start;
					if (idx < count && offset != 0)
					{
						p += offset + 2 * idx;
						idx = PEEK_USHORT(p);
						if (idx != 0)
							result = (unsigned int)((int)idx + delta) & 0xFFFFU;
					}
				}
				return result;
			}
			static int get_ext_glyph_index(const stbtt_fontinfo* info, unsigned int codepoint)
			{
				int ret = 0;
				uint8_t* data = info->data;
				uint32_t index_map = info->index_map;
				wchar_t wt[] = { codepoint, 0 };
				uint16_t format = ttUSHORT(data + index_map + 0);
				/*
					0*
					2
					4*
					6*
					8
					10
					12*
					13*
					14
				*/
				if (format == 2) {
					//STBTT_assert(0); // @TODO: high-byte mapping for japanese/chinese/korean
					if (codepoint > 127)
					{
						char st[4] = { 0 };
						auto astr = w2a(wt, 1, st, 4);
						char* t = (char*)& codepoint;
						t[0] = st[1]; t[1] = st[0];
						//std::swap(t[0], t[1]);
					}
					ret = tt_cmap2_char_index(data + index_map, codepoint);
				}
				return ret;
			}
			static int get_glyph_index2(font_impl* font, const char* t)
			{
				const stbtt_fontinfo* info = &font->font;
				int ret = 0;
				uint8_t* data = info->data;
				uint32_t index_map = info->index_map;
				uint16_t format = ttUSHORT(data + index_map + 0);
				unsigned int codepoint = 0;
				if (format == 2) {
					// @TODO: high-byte mapping for japanese/chinese/korean
					codepoint = (unsigned int)t[0] & 0xff;
					if (codepoint > 127)
					{
						codepoint <<= 8;
						codepoint |= (unsigned int)t[1] & 0xff;
					}
					ret = tt_cmap2_char_index(data + index_map, codepoint);
				}
				return ret;
			}

			static int getGlyphIndex(font_impl* font, int codepoint)
			{
				int idx = stbtt_FindGlyphIndex(&font->font, codepoint);
				return idx ? idx : get_ext_glyph_index(&font->font, codepoint);
			}

			static int buildGlyphBitmap(font_impl* font, int glyph, float scale,
				int* advance, int* lsb, int* x0, int* y0, int* x1, int* y1)
			{
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
			static int getKernAdvanceCH(font_impl* font, int ch1, int ch2)
			{
				return stbtt_GetCodepointKernAdvance(&font->font, ch1, ch2);
			}
			static void get_head(font_impl* font, head_table* p)
			{
				if (font && p)
				{
					auto info = &font->font;
					auto data = info->data + info->head;
					DL_long(p->version);
					DL_long(p->fontRevision);
					DL_long(p->checkSumAdjustment);
					DL_long(p->magicNumber);
					DL_ushort(p->flags);
					DL_ushort(p->unitsPerEm);
					int sldt = sizeof(longDateTime);
					memcpy(p->created, data, sldt); data += sldt;
					memcpy(p->modified, data, sldt); data += sldt;
					auto pxy = info->data + info->head + 36;
					DL_ushort(p->xMin);
					DL_ushort(p->yMin);
					DL_ushort(p->xMax);
					DL_ushort(p->yMax);
					DL_ushort(p->macStyle);
					DL_ushort(p->lowestRecPPEM);
					DL_short(p->fontDirectionHint);
					DL_short(p->indexToLocFormat);
					DL_short(p->glyphDataFormat);
				}
			}
			static void get_hhea(font_impl* font, hheaTbl* hhea)
			{
				if (font && hhea)
				{
					auto info = &font->font;
					auto data = info->data + info->hhea;

					DL_long(hhea->version);
					DL_short(hhea->ascender);
					DL_short(hhea->descender);
					DL_short(hhea->lineGap);
					DL_ushort(hhea->advanceWidthMax);
					DL_short(hhea->minLeftSideBearing);
					DL_short(hhea->minRightSideBearing);
					DL_short(hhea->xMaxExtent);
					DL_short(hhea->caretSlopeRise);
					DL_short(hhea->caretSlopeRun);
					DL_short(hhea->caretOffset);
					DL_short(hhea->reserved[0]);
					DL_short(hhea->reserved[1]);
					DL_short(hhea->reserved[2]);
					DL_short(hhea->reserved[3]);
					DL_short(hhea->metricDataFormat);
					DL_ushort(hhea->numberOfLongHorMetrics);
				}
			}
			static glm::ivec4 get_boundin_box(font_impl* font)
			{
				int x0 = 0, y0 = 0, x1, y1; // =0 suppresses compiler warning
				stbtt_GetFontBoundingBox(&font->font, &x0, &y0, &x1, &y1);
				return glm::ivec4(x0, y0, x1, y1);
			}
			static glm::ivec2 get_codepoint_hmetrics(font_impl* font, int ch)
			{
				int advance = 0, lsb = 0;
				stbtt_GetCodepointHMetrics(&font->font, ch, &advance, &lsb);
				return glm::ivec2(advance, lsb);
			}
			static char* get_glyph_bitmap(font_impl* font, int gidx, double scale, glm::ivec4* ot, std::vector<char>* out, double* advance, glm::vec2 lcd = { 1,1 })
			{
				//int i, j, baseline, ch = 0;
				int baseline;
				int ascent;
				int descent;
				int linegap;
				//float xpos = 2; // leave a little padding in case the character extends left
				//float scale = stbtt_ScaleForPixelHeight(&font->font, height);
				//float scale1 = stbtt_ScaleForMappingEmToPixels(&font->font, height);
				int x0 = 0, y0 = 0, x1, y1; // =0 suppresses compiler warning
				//if (!stbtt_GetGlyphBox(&font->font, gidx, &x0, &y0, &x1, &y1))
				//{
				//}
				auto fbb = get_boundin_box(font);
				stbtt_GetFontVMetrics(&font->font, &ascent, &descent, &linegap);
				//double pscale1 = fbb.w + abs(fbb.y);
				//double pscale = ascent;
				//auto scale2 = height / pscale;
				//scale = scale2;
				baseline = (int)(ascent * scale);
				int advancei, lsb;//, x0, y0, x1, y1;
				float shift_x = 0.0f;
				float shift_y = 0.0f;
				auto ghl = get_codepoint_hmetrics(font, L'g');
				auto Lhl = get_codepoint_hmetrics(font, L'L');
				stbtt_GetGlyphHMetrics(&font->font, gidx, &advancei, &lsb);
				double adv = (double)advancei * scale;
				//stbtt_GetCodepointBitmapBoxSubpixel(&font->font, ch, scale, scale, shiftX, shiftY, &x0, &y0, &x1, &y1);
				stbtt_GetGlyphBitmapBoxSubpixel(&font->font, gidx, scale * lcd.x, scale * lcd.y, shift_x, shift_y, &x0, &y0, &x1, &y1);

				*advance = adv;
				ot->x = x0;
				ot->y = y0;
				ot->z = x1 - x0;
				ot->w = y1 - y0;
				size_t pcs = (int64_t)ot->z * ot->w;
				if (pcs == 117)
				{
					pcs = 117;
				}
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
					scale * lcd.x, scale * lcd.y,
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
				uint16_t majorVersion = 0, minorVersion = 0;
				uint32_t numSizes = 0;
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
		// todo				 tt_info
		class tt_info
		{
		public:

			class info_one
			{
			public:
#ifdef _STD_STR_
				std::wstring name_;
				std::string name_a;
				std::string ndata;
#else
				w_string name_;
				s_string name_a;
				s_string ndata;
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
					ndata.assign(temp, length_);
					name_.assign((wchar_t*)name, length_ / 2);
				}

				std::string get_name(bool isu8 = true)
				{
					std::string n;
					if (encoding_id)
					{
#ifdef __json_helper_h__
#ifdef _STD_STR_
						n = isu8 ? jsonT::wstring_to_utf8(to_wstr(name_, true)) : jsonT::wstring_to_ansi(to_wstr(name_, true));
#else
						n = isu8 ? jsonT::wstring_to_utf8(name_.to_wstr(true)) : jsonT::wstring_to_ansi(name_.to_wstr(true));
#endif
#else
#ifdef _STD_STR_
						n = isu8 ? wstring_to_utf8(to_wstr(name_, true)) : wstring_to_ansi(to_wstr(name_, true));
#else
						n = isu8 ? wstring_to_utf8(name_.to_wstr(true)) : wstring_to_ansi(name_.to_wstr(true));
#endif
#endif // __json_helper_h__

						switch (platform_id)
						{
						case 0:
							// unicode
							break;
						case 1:
							// Macintosh
							break;
						case 2:
							// (reserved; do not use)
							break;
						case 3:
							// Microsoft
							if (encoding_id == 3 && language_id == 2052)
							{
								n = "";
								char* tw = (char*)ndata.data();
								tw++;
								int len = length_ / 2;
								for (size_t i = 0; i < len; i++, tw += 2)
								{
									auto it = *tw;
									n.push_back(it);
								}
								n = jsonT::ansi_to_utf8(n);
							}
							break;
						default:
							break;
						}
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

				static std::wstring to_wstr(std::wstring str, bool is_swap = false)
				{
					std::wstring ret = str;
					if (is_swap)
					{
						char* temp = (char*)ret.data();
						for (int i = 0; i < ret.size(); ++i)
						{
							temp = tt_ushort(temp);
						}
					}
					return ret;
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
					static IndexSubTableArray a;
					a.firstGlyphIndex = 0;
					a.lastGlyphIndex = 98;
					a.additionalOffsetToIndexSubtable = 0;
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
			// todo			tt_info数据区
			//k=language_id			2052 简体中文	1033 英语
			std::map<int, std::vector<info_one*>> _detail;

			font_impl_info font;
			std::string _name, _aname;
			int isFixedPitch;
			int postFixedPitch, charFixedPitch;
			bool is_copy = true;
			std::vector<unsigned char*> _vdata;
			unsigned char* data = 0;
			int dataSize = 0;
			unsigned char freeData = 0;
			head_table head;
			hheaTbl hhea;

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
			// 缩放缓存表
			std::unordered_map<int, double> _scale_lut;
			// 宽度缓存表
			std::unordered_map<unsigned int, glm::ivec3> _char_lut;

			LockS _lock;
		private:
			/*
				图像缓存表
				字体		key: uint64_t：字符utf16(int32)、字高font_height（字号*dpi/72）{unsigned short 1-1638}、模糊大小(unsigned char)
			*/

			// 查询表
			std::unordered_map<uint64_t, ft_item*> _cache_table;
			// 数据表
			//std::vector<ft_item> _ft_item_data;
			std::list<ft_item> _ft_item_data;
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
			void push_fallbacks(tt_info* fp)
			{
				LOCK_W(_lock);
				fallbacks.push_back(fp);
			}
			/*
			todo				缓存操作

			字符utf16(int32)、字号{unsigned short 1-1638}、模糊大小(unsigned short)
			// 缓存位置xy, 字符图像zw(width,height)
			// 基线偏移
			*/
			ft_item* push_cache(ft_key_s* key, Image* img, const glm::ivec4& rect, const glm::ivec2& baseline, double advance, double blf)
			{
				if (!key || !key->u || !key->v.unicode_codepoint || !key->v.font_size)
				{
					return nullptr;
				}
				LOCK_W(_lock);
				int fkss = sizeof(ft_key_s);
				ft_item* ret = 0;
				auto it = _cache_table.find(key->u);
				if (it == _cache_table.end())
				{
					//ret = fit_reserve(1);
					ft_item fm;
					fm.set_it(key->v.unicode_codepoint, img, rect, baseline, advance, key->v.blur_size, blf);
					_ft_item_data.emplace_back(fm);
					auto lt = --_ft_item_data.end();
					ret = &(*lt);
					_cache_table[key->u] = &(*lt);
				}
				else {
					throw (-1);
				}
				return ret;
				//_ft_item_data[ret].set_it(key->v.unicode_codepoint, img, rect, baseline, advance, key->v.blur_size);
				//return &_ft_item_data[ret];
			}
			// 批量插入
			ft_item* push_cache(ft_key_s* key, ft_item* ps, size_t n, std::vector<ft_item*>* out)
			{
				if (!key || !key->u || !key->v.font_size || !ps || !n)
				{
					return nullptr;
				}
				LOCK_W(_lock);
				//auto ret = fit_reserve(n);
				ft_key_s key2;
				key2.u = key->u;
				for (size_t i = 0; i < n; i++)
				{
					auto& it = ps[i];
					key2.v.unicode_codepoint = it._unicode_codepoint;
					key2.v.blur_size = it._blur_size;
					it.count = n;
					ft_item fm;
					fm.set_it(it);
					_ft_item_data.emplace_back(fm);
					auto lt = --_ft_item_data.end();
					_cache_table[key2.u] = &(*lt);
					out->push_back(&(*lt));
					//_ft_item_data[ret + i].set_it(it);
					//_cache_table[key2.u] = ret + i;
					//out->push_back(&_ft_item_data[ret + i]);
				}
				return 0;// _ft_item_data.data() + ret;
			}
			void clear_cache()
			{
				LOCK_W(_lock);
				_ft_item_data.clear();
				_cache_table.clear();
			}
			// 查询返回指针
			ft_item* find_item(ft_key_s* key)
			{
				ft_item* p = nullptr;
				if (key && key->u)
				{
					LOCK_R(_lock);
					ft_key_s key2;
					key2.u = key->u;
					//key2.v.blur_size = 0;
					auto it = _cache_table.find(key2.u);
					if (it != _cache_table.end())
					{
						//p = _ft_item_data.data() + it->second;
						p = it->second;
					}
				}
				return p;
			}
			// todo 获取字符大小
			glm::ivec3 get_char_extent(char32_t ch, unsigned char font_size, unsigned short font_dpi)
			{
				ft_char_s cs;
				cs.v.font_dpi = font_dpi;
				cs.v.font_size = font_size;
				cs.v.unicode_codepoint = ch;
				{
					LOCK_R(_lock);
					auto it = _char_lut.find(cs.u);
					if (it != _char_lut.end())
					{
						return it->second;
					}
				}
				glm::ivec3 ret;
				tt_info* rfont = nullptr;
				auto g = get_glyph_index(ch, &rfont);
				if (g)
				{
					double fns = round((double)font_size * font_dpi / 72.0);
					double scale = rfont->get_scale_height(fns);
					int x0 = 0, y0 = 0, x1 = 0, y1 = 0, advance, lsb;
					font_dev::buildGlyphBitmap(&rfont->font, g, scale, &advance, &lsb, &x0, &y0, &x1, &y1);
					double adv = scale * advance;
					ret = { x1 - x0, y1 - y0 , adv };
					LOCK_W(_lock);
					_char_lut[cs.u] = ret;
				}
				return ret;
			}
			void clear_char_lut()
			{
				LOCK_W(_lock);
				_char_lut.clear();
			}
		private:
			//// 增加n个缓存
			//size_t fit_reserve(int n)
			//{
			//	ft_item* p = 0;
			//	size_t idx = _ft_item_data.size();
			//	_ft_item_data.resize(idx + n);
			//	p = _ft_item_data.data() + idx;
			//	return idx;
			//}

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
			void init_post_table()
			{
				font_impl_info* font_i = &font;
				const stbtt_fontinfo* ft = &font_i->font;
				sfnt_header* post_table = get_tag(font_i, TAG_POST);
				if (!post_table)
				{
					post_table = get_tag(font_i, TAG_post);
				}
				uint8_t* fc = ft->data;
				if (post_table)
				{
					uint8_t* p = fc + post_table->offset;
					post_header post = {};

					post.format = stb_font::ttSHORT(p); p += 4;
					post.italicAngle = stb_font::ttSHORT(p); p += 4;
					post.underlinePosition = stb_font::ttSHORT(p); p += 2;
					post.underlineThickness = stb_font::ttSHORT(p); p += 2;
					post.isFixedPitch = stb_font::ttULONG(p); p += 4;
					post.minMemType42 = stb_font::ttULONG(p); p += 4;
					post.maxMemType42 = stb_font::ttULONG(p); p += 4;
					post.minMemType1 = stb_font::ttULONG(p); p += 4;
					post.maxMemType1 = stb_font::ttULONG(p); p += 4;
					postFixedPitch = post.isFixedPitch;
				}
				{
					int adv[3], lsb[1];
#if 0
					int tecp = 0;
					auto tf = "a";
					tecp = *((int*)tf);
					int glyph = font_dev::getGlyphIndex(&font, tecp),
						glyph1 = font_dev::get_glyph_index2(&font, "翡"),
						glyph2 = font_dev::get_glyph_index2(&font, "a");
					//8304
#endif
					get_HMetrics('a', adv, lsb);
					get_HMetrics('i', &adv[1], lsb);
					get_HMetrics('.', &adv[2], lsb);
					bool isfixed = (adv[0] != adv[1] || adv[0] != adv[2] || adv[1] != adv[2]);
					charFixedPitch = isfixed ? 0 : 1;
				}
				isFixedPitch = postFixedPitch || charFixedPitch;
				font_dev::get_head(font_i, &head);
				font_dev::get_hhea(font_i, &hhea);

			}
			bool is_fixed_pitch()
			{
				return isFixedPitch;
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
				return;
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
				line += (uint64_t)y_pos * pitch + (x_pos >> 3);
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
					//return 0;
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
					p += (uint64_t)4 * (glyph_index - start);
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
					p += (uint64_t)2 * (glyph_index - start);
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
					//TODO		解析SBIX
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
				printf("<%s>包含位图大小\n", _aname.c_str());
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
			int get_glyph_bitmap(int gidx, int height, glm::ivec4* ot, Bitmap* out_bitmap, std::vector<char>* out)
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
					out->resize((uint64_t)bitmap->rows * bitmap->pitch);
					memcpy(out->data(), bitmap->buffer, out->size());
					ot->x = metrics->horiBearingX;
					ot->y = -metrics->horiBearingY;
					auto ha = metrics->horiAdvance;
					bitmap->advance = std::max(metrics->horiAdvance, metrics->vertAdvance);
					ot->z = bitmap->width;
					ot->w = bitmap->rows;
					if (out_bitmap)
					{
						*out_bitmap = *bitmap;
						//out_bitmap->data = out;
						out_bitmap->buffer = (unsigned char*)out->data();
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
			int get_glyph_image(int gidx, double height, glm::ivec4* ot, Bitmap* bitmap, std::vector<char>* out, bool first_bitmap, int lcd_type = 0)
			{
				int ret = 0;
				if (gidx > 0)
				{
#ifndef _FONT_NO_BITMAP
					if (first_bitmap)
					{
						// 解析位图
						ret = get_glyph_bitmap(gidx, height, ot, bitmap, out);
					}
#endif
					if (!ret)
					{
						// 解析轮廓并光栅化
						double advance = height;
						glm::vec2 lcds[] = { {1,1},{3,1},{1,3} };
						double scale = get_scale_height(height);
						auto fbb = font_dev::get_boundin_box(&font);
						font_dev::get_glyph_bitmap(&font, gidx, scale, ot, out, &advance, lcds[lcd_type]);
						if (bitmap)
						{
							auto hh = hhea;
							auto he = hhea.ascender + hhea.descender + hhea.lineGap;
							auto hef = hhea.ascender - hhea.descender + hhea.lineGap;

							double hed = scale * he;
							double hedf = scale * hef;
							double lg = scale * hhea.lineGap;
							bitmap->buffer = (unsigned char*)out->data();
							bitmap->width = bitmap->pitch = ot->z;
							bitmap->rows = ot->w;
							bitmap->advance = advance;
							bitmap->pixel_mode = PX_GRAY;	//255灰度图
							init_bitmap_bitdepth(bitmap, 8);
							ret = 1;
						}
					}
				}
				return ret;
			}
			/*
			获取量
			*/
			double get_scale_height(int height)
			{
				double scale = 0.0;
				{
					//LOCK_W(_lock);
					scale = _scale_lut[height];
					if (!(scale > 0))
					{
						scale = font_dev::getPixelHeightScale(&font, height);
						//glm::ivec4 glrc[3];
						//font_dev::getPixelGLScale(&font, glrc);
						//auto adl = hhea.ascender + abs(hhea.descender);
						//double ng = abs(glrc[0].y) + abs(glrc[1].w);
						//double s1 = height - 1;
						//auto n = s1 / ng;
						_scale_lut[height] = scale;// = n;
					}
				}
				return scale;
			}
			int get_line_height(double height, bool islinegap = true)
			{
				double scale = get_scale_height(height);
				auto adl = hhea.ascender + abs(hhea.descender) + (islinegap ? hhea.lineGap : 0);
				double f = scale * adl;
				int ret = ceil(f);	// 向上取整
				if (ret & 1)
				{
					ret++;
				}
				return ret;
			}
			// 获取最大行高
			glm::ivec3 get_rbl_height(double height, glm::ivec3* out, bool* is)
			{
#if 0
				double scale = get_scale_height(height);
				double f = hhea.ascender;
				double ba = ceil(f * scale);
				f += abs(hhea.descender);
				int ret = ceil(f * scale);	// 向上取整
				if (ret & 1)
				{
					ret++;
				}
				if (ret > out->y)
				{
					out->z = std::max(out->z, (int)ba);
					out->y = ret;
					f += hhea.lineGap;
					ret = ceil(f * scale);
					if (ret & 1)
					{
						ret++;
					}
					out->x = ret;
				}
				return ret;
#else
				glm::ivec3 ret;
				double scale = get_scale_height(height);
				double f = hhea.ascender;
				double ba = ceil(f * scale);
				f += abs(hhea.descender);
				ret.z = ba;
				ret.y = ceil(f * scale);	// 向上取整
				if (ret.y & 1)
				{
					ret.y++;
				}
				f += hhea.lineGap;
				ret.x = ceil(f * scale);
				if (ret.x & 1)
				{
					ret.x++;
				}
				if (out)
				{
					if (ret.x >= out->x || (ret.x == out->x && out->z < ret.x))
					{
						*out = ret;
						if (is)
							* is = true;
					}
				}
				return ret;
#endif
			}
			int get_base_line(double height)
			{
				float scale = get_scale_height(height);
				double f = hhea.ascender;
				return ceil(f * scale);
				//return floor(f * scale); // 向下取整
			}
			int get_xmax_extent(double height, int* line_gap)
			{
				float scale = get_scale_height(height);
				double f = hhea.xMaxExtent, lig = hhea.lineGap;
				if (line_gap)
				{
					*line_gap = ceil(lig * scale);
				}
				return ceil(f * scale);
			}
			void get_VMetrics(int* ascent, int* descent, int* lineGap)
			{
				font_dev::getFontVMetrics(&font, ascent, descent, lineGap);
			}
			void get_HMetrics(int codepoint, int* advance, int* lsb)
			{
				int glyph = font_dev::getGlyphIndex(&font, codepoint);
				font_dev::getFontHMetrics(&font, glyph, advance, lsb);
				return;
			}
		private:

		};
		// !tt_info

	public:
		// todo 样式
		//----------------------------------------------------------------------------------------------------------------------------------------------------------
		class css_text
		{
		public:
			enum e_text_decoration
			{
				none,//: 无装饰
				blink = 1,//: 闪烁
				underline = 2,//: 下划线
				line_through = 4,//: 贯穿线
				overline = 8,//: 上划线
				all_line = underline | line_through | overline,
			};
		public:
			unsigned int color = -1;
			unsigned int color_blur = 0xff000000;
			unsigned int outline_color = 0;
			// 装饰线
			unsigned int text_decoration = 0;
			double line_thickness = 1.0;
			double row_height = -1;
			double row_base_y = 0;
			// 最大行高
			int row_y = 0;
			// tab宽度
			uint8_t _tabs = 4;
			// 字间隔
			int _zpace = 0;
			// 行间隔
			int _lineSpacing = 0;
			css_text* _row = 0;
			bool first_bitmap = false;
			struct
			{
				int x = 0, y = 0;
			} blur_pos;
			double  _fzpace = 0.0, _flineSpacing = 0.0;
		private:
			tt_info* font = nullptr;
			double font_size = 12;
			double dpi = 96;
			unsigned char blur_size = 0;
			std::atomic_int* _upinc = nullptr;
			int _curinc = 0;
			double fns = 0;
		public:
			css_text()
			{
				_upinc = new std::atomic_int(0);
			}
			css_text(const css_text& c)
			{
				*this = c;
			}
			~css_text()
			{
				if (_upinc)
				{
					delete _upinc;
				}
				_upinc = nullptr;
			}
			css_text& operator=(const css_text& c)
			{
				if (this != &c)
				{
					color = c.color;
					color_blur = c.color_blur;
					outline_color = c.outline_color;
					text_decoration = c.text_decoration;
					line_thickness = c.line_thickness;
					row_height = c.row_height;
					row_base_y = c.row_base_y;
					row_y = c.row_y;
					_tabs = c._tabs;
					_zpace = c._zpace;
					_lineSpacing = c._lineSpacing;
					_row = c._row;
					first_bitmap = c.first_bitmap;
					blur_pos = c.blur_pos;;
					_fzpace = c._fzpace;
					_flineSpacing = c._flineSpacing;
					font = c.font;
					font_size = c.font_size;
					dpi = c.dpi;
					blur_size = c.blur_size;
					if (!_upinc)
						_upinc = new std::atomic_int(0);
				}
				return *this;
			}
			static css_text* create(const css_text* c)
			{
				css_text* p = new css_text();
				if (c)
				{
					*p = *c;
				}
				return p;
			}
			void update()
			{
				if (_upinc)
				{
					(*_upinc)++;
				}
			}
		public:
			tt_info* get_font()
			{
				return font;
			}
			void set_font(tt_info* p)
			{
				font = p;
			}
			void set_font_size(double size, double dpi_ = 96)
			{
				font_size = size;
				if (dpi_ > 0.0)
					dpi = dpi_;
				update();
				mk_fns();
			}
			void set_blur_size(unsigned char bs)
			{
				blur_size = bs;
			}
			double get_font_size()
			{
				return font_size;
			}
			double get_font_dpi()
			{
				return dpi;
			}
			unsigned char get_blur_size()
			{
				return blur_size;
			}
			double get_fheight()
			{
				if (_curinc != *_upinc)
				{
					_curinc = *_upinc;
					mk_fns();
				}
				return fns;
			}
			double get_row_lt(unsigned int etd)
			{
				return _row && _row->text_decoration & etd ? _row->line_thickness : line_thickness;
			}
		private:
			void mk_fns()
			{
				fns = font_size * dpi / 72.0;
				_fzpace = round((double)_zpace * dpi / 72.0);
				_flineSpacing = round((double)_lineSpacing * dpi / 72.0);
				fns = round(fns);
			}
		};
		// todo 字符宽高结构
		class text_extent
		{
		public:
			glm::ivec2 _size;
			const char* next_char = nullptr;
		public:
			text_extent()
			{
			}

			~text_extent()
			{
			}

		private:

		};

	private:
		// TODO		数据区
		std::set<std::string> dv;
		std::vector<std::vector<char>*> fn_data;
		std::vector<tt_info*> _fonts;
		// 根据字体名查找
		std::unordered_map<std::string, std::vector<tt_info*>> _tbs;
		// 读写锁
		LockS _lk;
		int def_language_id = 2052;
		// 图像缓存
		ImagePacker<Image> _packer;
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
			for (auto it : _fonts)
			{
				tt_info::free_info(it);
			}
		}
	public:
#ifndef __FILE__h__
		static std::string getAP()
		{
			std::string ret;
#ifdef _WIN32
			char szAppPath[1024] = { 0 };
			GetModuleFileNameA(GetModuleHandleA(""), (char*)szAppPath, 1024);
			(strrchr((char*)szAppPath, '\\'))[1] = 0;
			ret = szAppPath;
#else
			// linux获取本进程目录
			size_t len = PATH_MAX;
			char processdir[PATH_MAX];
			char processname[1024];
			char* path_end;
			std::vector<std::string> vs;
			if (readlink("/proc/self/exe", processdir, len) <= 0)
				return ret;
			path_end = strrchr(processdir, '/');
			if (path_end == NULL)
				return ret;
			++path_end;
			strcpy(processname, path_end);
			*path_end = '\0';
			size_t l = (size_t)(path_end - processdir);
			vs.push_back(processdir);
			vs.push_back(processname);
			ret = processdir;
			//ret = getenv("HOME");
			//ret += "/";
#endif
			return ret;
		}
		static std::string getAP(std::string name, std::string pat = "")
		{
			std::string rap = getAP();
			std::string stp = rap.find('/') != -1 ? "/" : "\\";
			if (pat != "")
			{
				rap += pat + stp;
			}
			rap += name;
			return rap;
		}
		//验证
		static std::string getFn(std::string fnc, bool isutf8 = false)
		{
			std::string tem = fnc;
#ifndef __ANDROID__
			if (0 != access(fnc.c_str(), 0))
			{
				std::set<std::string> fv;
				fv.insert(getAP(fnc));
				for (auto it : s()->dv)
					fv.insert(getAP(fnc, it));
				fnc = "";
				for (auto& it : fv)
				{
					if (0 == access(it.c_str(), 0))
					{
						fnc = it;
						break;
					}
				}
			}
#endif
			if (fnc == "")
			{
				fnc = tem;
			}
#ifdef strutil
			return isutf8 ? hz::strutil::AnsiToUtf8(fnc.c_str()) : fnc;
#else
			return fnc;
#endif
		}
		static bool read_binary_file(std::string filename, std::vector<char>& result, bool ismv = false)
		{
			int64_t size = 0;
			uint64_t  retval;
			void* buff = 0;
			result.clear();
			//LOGW(("read_binary_file0:" + filename).c_str());
#if (defined(VK_USE_PLATFORM_IOS_MVK) || defined(VK_USE_PLATFORM_MACOS_MVK))
			filename = [[[NSBundle mainBundle]resourcePath] stringByAppendingPathComponent:@(filename)] .UTF8String;
#endif
			filename = s()->getFn(filename);
#ifdef _WIN32
			if (filename.find("/"))
			{
				filename = replace(filename, "/", "\\");
			}
#endif
			const char* fn = filename.c_str();
			FILE* fp = ismv ? 0 : fopen(filename.c_str(), "rb");
			if (!fp)
			{
#ifdef __ANDROID__

				LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);
				LOGI("a_mgr%p,%s", a_mgr, filename.c_str());
				AAsset* asset = AAssetManager_open((AAssetManager*)a_mgr, filename.c_str(), AASSET_MODE_STREAMING);

				assert(asset);
				if (!asset)return false;
				size_t size = AAsset_getLength(asset);
				assert(size > 0);
				LOGI("%p", size);
				result.resize(size);
				void* textureData = result.data();
				AAsset_read(asset, textureData, size);
				AAsset_close(asset);
				LOGW("read_binary_file1: %d", (int)size);
				return true;
#else
				return false;
#endif
			}
#ifdef _WIN32
			_fseeki64(fp, 0L, SEEK_END);
			size = _ftelli64(fp);
			_fseeki64(fp, 0L, SEEK_SET);
#else			
			fseeko64(fp, 0L, SEEK_END);
			size = ftello64(fp);
			fseeko64(fp, 0L, SEEK_SET);
#endif // _WIN32
			result.resize(size);
			buff = &result[0];
			retval = fread(buff, size, 1, fp);
			assert(retval == 1);
			fclose(fp);
			return true;
		}
		static bool save_binary_file(std::string filename, const char* data, uint64_t size, uint64_t pos = 0, bool is_plus = false)
		{
			uint64_t  retval;
			if (!data || !size)
			{
				return false;
			}
			//LOGW(("read_binary_file0:" + filename).c_str());
#if (defined(VK_USE_PLATFORM_IOS_MVK) || defined(VK_USE_PLATFORM_MACOS_MVK))
			filename = [[[NSBundle mainBundle]resourcePath] stringByAppendingPathComponent:@(filename)] .UTF8String;
#endif
			//filename = s()->getFn(filename);
#ifdef _WIN32
			if (filename[1] != ':')
				filename = getAP(filename);
#endif // _WIN32
			//check_make_path(getPath(filename.c_str(), pathdrive | pathdir));
			const char* fn = filename.c_str();
			FILE* fp = fopen(filename.c_str(), is_plus ? "ab" : "wb");

			if (!fp && !is_plus)
			{
#ifdef MAPVIEW 
				hz::MapView mv;
				if (mv.openfile(fn) || mv.createfile(fn))			//打开文件					
				{
					mv.creatmap(size);
					char* buf = (char*)mv.mapview();	//获取映射内容
					if (buf)
					{
						memcpy(buf, data, size);
						mv.FlushView();
						return true;
					}
				}
#endif
				return false;// "fail to open file: " + filename;
			}
#ifdef _WIN32
			if (is_plus)
				_fseeki64(fp, pos, SEEK_SET);
#else			
			if (is_plus)
				fseeko64(fp, pos, SEEK_SET);
#endif // _WIN32
			retval = fwrite(data, size, 1, fp);
			assert(retval == 1);
			fclose(fp);
			return true;
		}

#endif
	public:
		void set_langid(int langid)
		{
			def_language_id = langid;
		}
		tt_info* get_font(int idx)
		{
			return (idx < 0 || idx > _fonts.size()) ? nullptr : _fonts[idx];
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
			read_binary_file(fn, data);
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
					font->_name = font->get_info_str(def_language_id);
					font->_aname = jsonT::utf8_to_ansi(font->_name);
					font->init_post_table();
#ifndef _FONT_NO_BITMAP
					font->init_sbit();
#endif // !_FONT_NO_BITMAP
					LOCK_W(_lk);
					_tbs[font->_name].push_back(font);
					_fonts.push_back(font);

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
					ret[n]["is_fixed"] = it->isFixedPitch;
					//ret[n]["post_fixed"] = it->postFixedPitch;
					//ret[n]["char_fixed"] = it->charFixedPitch;
					ret[n]["style"].push_back(ns);
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
				ret = it->second.size() ? it->second[0] : nullptr;
				for (auto& ft : it->second)
				{
					auto fst = ft->get_info_str(2052, 2);
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
		static void Blur(unsigned char* dst, int w, int h, int dstStride, float blur, int n, int mode = 0x01 | 0x02)
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
				if (mode & 0x01)
					blurRows(dst, w, h, dstStride, alpha);
				if (mode & 0x02)
					blurCols(dst, w, h, dstStride, alpha);
			}
		}

		/* A possible bicubic interpolation consists in performing a convolution
		   with the following kernel :
									[ 0  2  0  0][s0]
									[-1  0  1  0][s1]
			p(t) = 0.5 [1 t t^2 t^3][ 2 -5  4 -1][s2]
									[-1  3 -3  1][s3]
			Where s0..s3 are the samples, and t is the range from 0.0 to 1.0 */
		static inline float bicubic(float s0, float s1, float s2, float s3, float t) {
			float r0 = 0.5f * (2.0f * s1);
			float r1 = 0.5f * (-s0 + s2);
			float r2 = 0.5f * (2.0 * s0 - 5.0f * s1 + 4.0f * s2 - s3);
			float r3 = 0.5f * (-s0 + 3.0f * s1 - 3.0f * s2 + s3);
			return r3 * t * t * t + r2 * t * t + r1 * t + r0;
		}


		/* sYX is a matrix with the samples (row-major)
		   xt and yt are the interpolation fractions in the two directions ranging from
		   0.0 to 1.0 */
		static inline float bicubic2D(
			float s00, float s01, float s02, float s03,
			float s10, float s11, float s12, float s13,
			float s20, float s21, float s22, float s23,
			float s30, float s31, float s32, float s33, float xt, float yt) {
			// The bicubic convolution consists in passing the bicubic kernel in x
			// and then in y (or vice-versa, really)
			float r0 = bicubic(s00, s01, s02, s03, xt);
			float r1 = bicubic(s10, s11, s12, s13, xt);
			float r2 = bicubic(s20, s21, s22, s23, xt);
			float r3 = bicubic(s30, s31, s32, s33, xt);
			return bicubic(r0, r1, r2, r3, yt);
		}
		//
		/*
		lcd =a
		a->b
		*/
		static void lcd2rgba(byte* b, byte* a, int W, int H)
		{
			byte* end = b + (W * H * 4);
			int x = 0;
			int z = W & 3;
			while (1) {
				if (b >= end) break;
				b[0] = a[2]; // reverse RGB
				b[1] = a[1];
				b[2] = a[0];
				b[3] = 0;
				a += 3;
				b += 4;
				x++;
				if (x == W) {
					x = 0;
					a += z;
				}
			}
		}

		static void copy_bitmap(Bitmap* dst, Bitmap* src, int x, int y, std::vector<char>* out)
		{
			Fonts::init_bitmap_bitdepth(dst, 8);
			size_t size = dst->rows * (uint32_t)dst->pitch;
			if (size > out->size())
			{
				out->resize(size);
			}
			dst->buffer = (unsigned char*)out->data();
			memset(dst->buffer, 0, size);
			int pixel_mode = src->pixel_mode;
			unsigned char* db = dst->buffer;
			unsigned char* sb = src->buffer;

			if (pixel_mode == PX_GRAY)				/*256灰度图*/
			{
				for (size_t j = 0; j < src->rows && j + y < dst->rows; j++)
				{
					auto pj = src->pitch * j;
					unsigned char* pixel = sb + pj;
					auto jp = j + y;
					int64_t psy = (jp * dst->pitch);
					if (psy < 0 || jp >= dst->rows)
					{
						continue;
					}
					unsigned char* dc = db + psy;
					for (int i = 0; (i < src->width) && ((i + x) < dst->width); i++)
					{
						auto dt = &dc[i + x];
						int ds = dt - db;
						if (ds > size)
						{
							dt = dt;
						}
						dc[i + x] = pixel[i];
					}
				}
			}
			else if (pixel_mode == PX_MONO)			/*单色位图1位*/
			{
				for (size_t j = 0; j < src->rows && j + y < dst->rows; j++)
				{
					auto pj = src->pitch * j;
					unsigned char* pixel = sb + pj;
					auto jp = j + y;
					int64_t psy = (jp * dst->pitch);
					if (psy < 0 || jp >= dst->rows)
					{
						continue;
					}
					unsigned char* dc = db + psy;
					for (int i = 0; (i < src->width) && ((i + x) < dst->width); i++)
					{
						unsigned char c = (pixel[i / 8] & (0x80 >> (i & 7))) ? 255 : 0;
						auto dt = &dc[i + x];
						int ds = dt - db;
						if (ds > size)
						{
							dt = dt;
						}
						dc[i + x] = c ? 255 : 0;
					}
				}
			}
		}
		static void init_bitmap_bitdepth(Bitmap* bitmap, int bit_depth)
		{
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
				return;
			}
		}
		/* Enlarge `bitmap' horizontally and vertically by `xpixels' */
	/* and `ypixels', respectively.                              */

		static int ft_bitmap_assure_buffer(Fonts::Bitmap* bitmap, unsigned int xpixels, unsigned int ypixels, std::vector<char>* nbuf)
		{
			int				error;
			int             pitch;
			int             new_pitch;
			unsigned int         bpp;
			unsigned int         i, width, height;
			unsigned char* buffer = NULL;


			width = bitmap->width;
			height = bitmap->rows;
			pitch = bitmap->pitch;
			if (pitch < 0)
				pitch = -pitch;

			switch (bitmap->pixel_mode)
			{
			case PX_MONO:
				bpp = 1;
				new_pitch = (int)((width + xpixels + 7) >> 3);
				break;
			case PX_GRAY2:
				bpp = 2;
				new_pitch = (int)((width + xpixels + 3) >> 2);
				break;
			case PX_GRAY4:
				bpp = 4;
				new_pitch = (int)((width + xpixels + 1) >> 1);
				break;
			case PX_GRAY:
			case PX_LCD:
			case PX_LCD_V:
				bpp = 8;
				new_pitch = (int)(width + xpixels);
				break;
			default:
				return 0;
			}

			/* if no need to allocate memory */
			if (ypixels == 0 && new_pitch <= pitch)
			{
				/* zero the padding */
				unsigned int  bit_width = (unsigned int)pitch * 8;
				unsigned int  bit_last = (width + xpixels) * bpp;


				if (bit_last < bit_width)
				{
					unsigned char* line = bitmap->buffer + (bit_last >> 3);
					unsigned char* end = bitmap->buffer + pitch;
					unsigned int   shift = bit_last & 7;
					unsigned int   mask = 0xFF00U >> shift;
					unsigned int   count = height;


					for (; count > 0; count--, line += pitch, end += pitch)
					{
						unsigned char* write = line;


						if (shift > 0)
						{
							write[0] = (unsigned char)(write[0] & mask);
							write++;
						}
						if (write < end)
							memset(write, 0, end - write);
					}
				}

				return 0;
			}

			/* otherwise allocate new buffer */
			auto ry = bitmap->rows + ypixels;
			nbuf->resize(ry * new_pitch);
			buffer = (unsigned char*)nbuf->data();
			///if (FT_QALLOC_MULT(buffer, new_pitch, bitmap->rows + ypixels))
			//	return error;

			/* new rows get added at the top of the bitmap, */
			/* thus take care of the flow direction         */
			if (bitmap->pitch > 0)
			{
				unsigned int  len = (width * bpp + 7) >> 3;
				char* dst;
				char* dst_b = (char*)buffer;

				for (i = 0; i < bitmap->rows; i++)
				{
					dst = (char*)buffer + (unsigned int)new_pitch * (ypixels + i);
					char* src = (char*)bitmap->buffer + (unsigned int)pitch * i;
					auto dec = dst - dst_b;
					uint64_t ls = dec + len;
					if (ls > nbuf->size())
					{
						ls = ls;
					}
					memcpy(dst, src, len);
				}
			}
			else
			{
				unsigned int  len = (width * bpp + 7) >> 3;


				for (i = 0; i < bitmap->rows; i++)
					memcpy(buffer + (unsigned int)new_pitch * i,
						bitmap->buffer + (unsigned int)pitch * i,
						len);
			}

			//free(bitmap->buffer);
			bitmap->buffer = buffer;

			if (bitmap->pitch < 0)
				new_pitch = -new_pitch;

			/* set pitch only, width and height are left untouched */
			bitmap->pitch = new_pitch;

			return 0;
		}
		/*
		加粗bitmap，支持位图PX_MONO、PX_GRAY
		newbuf和不能和buffer一样
		*/
		static void bitmap_embolden(Fonts::Bitmap* bitmap, int xstr, int ystr, std::vector<char>* newbuf = nullptr)
		{
			if (!newbuf)
			{
				return;
			}
			ft_bitmap_assure_buffer(bitmap, xstr, ystr, newbuf);

			/* take care of bitmap flow */
			auto pitch = bitmap->pitch;
			int x, y, i;
			unsigned char* p;
			if (pitch > 0)
				p = bitmap->buffer + pitch * ystr;
			else
			{
				pitch = -pitch;
				p = bitmap->buffer + (unsigned int)pitch * (bitmap->rows - 1);
			}

			/* for each row */
			for (y = 0; y < bitmap->rows; y++)
			{
				/*
				 * Horizontally:
				 *
				 * From the last pixel on, make each pixel or'ed with the
				 * `xstr' pixels before it.
				 */
				for (x = pitch - 1; x >= 0; x--)
				{
					unsigned char  tmp;


					tmp = p[x];
					for (i = 1; i <= xstr; i++)
					{
						if (bitmap->pixel_mode == PX_MONO)
						{
							p[x] |= tmp >> i;

							/* the maximum value of 8 for `xstr' comes from here */
							if (x > 0)
								p[x] |= p[x - 1] << (8 - i);

#if 0
							if (p[x] == 0xFF)
								break;
#endif
						}
						else
						{
							if (x - i >= 0)
							{
								if (p[x] + p[x - i] > bitmap->num_grays - 1)
								{
									p[x] = (unsigned char)(bitmap->num_grays - 1);
									break;
								}
								else
								{
									p[x] = (unsigned char)(p[x] + p[x - i]);
									if (p[x] == bitmap->num_grays - 1)
										break;
								}
							}
							else
								break;
						}
					}
				}

				/*
				 * Vertically:
				 *
				 * Make the above `ystr' rows or'ed with it.
				 */
				for (x = 1; x <= ystr; x++)
				{
					unsigned char* q;


					q = p - bitmap->pitch * x;
					for (i = 0; i < pitch; i++)
						q[i] |= p[i];
				}

				p += bitmap->pitch;
			}

			bitmap->width += (unsigned int)xstr;
			bitmap->rows += (unsigned int)ystr;
		}

		static void copy_bitmap_mask(Bitmap* dst, Bitmap* src, glm::ivec2 dstpos, std::function<void(unsigned char* dp, unsigned char* sp)> func)
		{
			int x = dstpos.x, y = dstpos.y;
			int pixel_mode = src->pixel_mode;
			unsigned char* db = dst->buffer;
			unsigned char* sb = src->buffer;
			dst->pixel_mode = PX_GRAY;
			if (pixel_mode == PX_GRAY)				/*256灰度图*/
			{
				for (size_t j = 0; j < src->rows && j + y < dst->rows; j++)
				{
					auto pj = src->pitch * j;
					unsigned char* pixel = sb + pj;
					auto jp = j + y;
					int64_t psy = (jp * dst->pitch);
					if (psy < 0 || jp >= dst->rows)
					{
						continue;
					}
					unsigned char* dc = db + psy;
					for (int i = 0; (i < src->width) && ((i + x) < dst->width); i++)
					{
						func(dc + i + x, pixel + i);
					}
				}
			}
			else if (pixel_mode == PX_MONO)			/*单色位图1位*/
			{
				for (size_t j = 0; j < src->rows && j + y < dst->rows; j++)
				{
					auto pj = src->pitch * j;
					unsigned char* pixel = sb + pj;
					auto jp = j + y;
					int64_t psy = (jp * dst->pitch);
					if (psy < 0 || jp >= dst->rows)
					{
						continue;
					}
					unsigned char* dc = db + psy;
					for (int i = 0; (i < src->width) && ((i + x) < dst->width); i++)
					{
						unsigned char c = (pixel[i / 8] & (0x80 >> (i & 7))) ? 255 : 0;
						func(dc + i + x, &c);
					}
				}
			}
		}

		static void get_blur(Bitmap* dst, Bitmap* src, float blur, int n, std::vector<char>* bitdata, int mode = 0)
		{
			*dst = *src;
			dst->width += blur * 2.0;
			dst->rows += blur * 2.0;
			dst->pitch = dst->width;
			dst->advance += blur * 2.0;
			uint32_t size = dst->rows * (uint32_t)dst->pitch;
			if (size == 0)
				return;
			bitdata->resize(size);
			copy_bitmap(dst, src, blur, blur, bitdata);
			if (mode < 1)
			{
				mode = 0x01 | 0x02;
			}
			Blur((unsigned char*)dst->buffer, dst->width, dst->rows, dst->pitch, blur, n, mode);
		}

		glyph_shape* get_glyph_shape(tt_info* font, const char* str)
		{
			if (!font && _fonts.size())
			{
				font = _fonts[0];
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
		Glyph* get_glyph_test0(tt_info* font, const char* codepoint, short isize, short iblur, int bitmapOption = FONS_GLYPH_BITMAP_REQUIRED)
		{
			return get_glyph_test(font, font_dev::get_u8_to_u16(codepoint), isize, iblur, bitmapOption);
		}
		char* get_glyph_test1(tt_info* font, unsigned int codepoint, float height, glm::ivec4* ot, std::vector<char>* out)
		{
			int i, g, advance, lsb, x0, y0, x1, y1, gw, gh, gx = 0, gy = 0, x, y;
			float scale;
			Glyph* glyph = NULL;
			unsigned int h;
			int pad, added;
			unsigned char* bdst;
			unsigned char* dst;
			tt_info* renderFont = font;
			if (!font && _fonts.size())
			{
				renderFont = _fonts[0];
			}
			if (!renderFont)
			{
				return nullptr;
			}
			g = font->get_glyph_index(codepoint, &renderFont);
			double adv;
			return font_dev::get_glyph_bitmap(&renderFont->font, g, height, ot, out, &adv);
		}
		Glyph* get_glyph_test(tt_info* font, unsigned int codepoint, short isize, short iblur, int bitmapOption = FONS_GLYPH_BITMAP_REQUIRED)
		{
			int i, g, advance, lsb, x0, y0, x1, y1, gw, gh, gx = 0, gy = 0, x, y;
			float scale;
			Glyph* glyph = NULL;
			unsigned int h;
			float size = isize; // 10.0f;
			int pad, added;
			unsigned char* bdst;
			unsigned char* dst;
			if (!font && _fonts.size())
			{
				font = _fonts[0];
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
			font_dev::buildGlyphBitmap(&renderFont->font, g, scale, &advance, &lsb, &x0, &y0, &x1, &y1);
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
				Blur(bdst, gw, gh, params_width, iblur, 2);
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
		static const char* get_u8_index(const char* str, int idx)
		{
			unsigned int codepoint[1];
			unsigned int utf8state = 0;
			for (; str && *str && idx > 0; ) {
				if (fons_decutf8(&utf8state, codepoint, *(const unsigned char*)str++))
				{
					idx--;
					continue;
				}
				break;
			}
			return str;
		}
	private:
		Image* push_cache_bitmap(Bitmap* bitmap, glm::ivec2* pos, int linegap = 0, unsigned int col = -1)
		{
			int width = bitmap->width + linegap, height = bitmap->rows + linegap;
			glm::ivec4 rc4 = { 0, 0, bitmap->width, bitmap->rows };
			auto ret = _packer.push_rect({ width, height }, pos);
			if (ret)
			{
				rc4.x = pos->x;
				rc4.y = pos->y;
				//ret->draw_rect(rc4, 0, col);
				ret->copy_to_image(bitmap->buffer, bitmap->pitch, rc4, col, bitmap->pixel_mode, 0.0);
				ret->set_update();
			}
			return ret;
		}
	public:
		void save_cache(std::string fdn)
		{
			int i = 0;
			_packer.maps([=, &i](Image* img) {
				std::string fn = fdn + std::to_string(i);
#ifdef __FILE__h__
				img->saveImage(fn + ".png");
#else
				auto d = img->png_data();
				save_binary_file(fn + ".png", d.data(), d.size());
#endif
				});
		}
		glm::vec3 make_outline_y(css_text* ct, size_t n)
		{
			glm::ivec3 ret = { 0,0,0 };
			std::vector<glm::ivec3> os;
			css_text* mt = 0;
			for (size_t i = 0; i < n; i++)
			{
				auto it = &ct[i];
				bool ism = false;
				auto font = it->get_font();
				if (font)
				{
					auto itr = font->get_rbl_height(it->get_fheight(), &ret, &ism);
					os.push_back(itr);
					it->line_thickness = std::max(1.0, floor(itr.y / 24.0));
					if (ism)
					{
						mt = it;
					}
				}

			}
			for (size_t i = 0; i < n; i++)
			{
				auto it = &ct[i];
				it->row_height = ret.x;
				it->row_y = ret.y;
				it->row_base_y = ret.z;
				it->_row = mt;
			}

			return ret;
		}
	public:
		/*
			查询是否有缓存，没有则生成
				ft_key_s key {
				char32_t unicode_codepoint;
				unsigned short font_dpi;
				// 字号支持 1-255
				unsigned char font_size;
				// 模糊大小支持 0-255
				unsigned char blur_size;
				};
		*/
		std::vector<ft_item*> make_cache(tt_info* font, ft_key_s* key, double fns, bool first_bitmap)
		{
			std::vector<ft_item*> ret;
			ft_key_s k2;
			k2.u = key->u;
			k2.v.blur_size = 0;
			ret.resize(1);
			ret[0] = font->find_item(&k2);
			bool isblur = false;
			if (key->v.blur_size)
			{
				ret.push_back(font->find_item(key));
			}
			if (!ret[0] || (ret.size() > 1 && !ret[1]))
			{
				tt_info* rfont = nullptr;
				Bitmap bitmap[1] = {}, blur[1] = {};
				std::vector<char> bitbuf[2];
				glm::ivec4 rc;
				std::vector<ft_item>  ps;
				auto kt = key->v;
				double base_line = 0;
				int gidx = font->get_glyph_index(kt.unicode_codepoint, &rfont);
				if (rfont && gidx)
				{
					auto bit = rfont->get_glyph_image(gidx, fns, &rc, bitmap, &bitbuf[0], first_bitmap);
					glm::ivec2 pos;
					if (font != rfont)
					{
						base_line = rfont->get_base_line(fns);
					}
					if (!ret[0])
					{
						ps.resize(1);
						auto fit = &ps[0];
						auto frc = rc;
						auto bimg = push_cache_bitmap(bitmap, &pos, 0, -1);
						frc.x = pos.x;
						frc.y = pos.y;
						fit->set_it(kt.unicode_codepoint, bimg, frc, { rc.x, rc.y }, bitmap->advance, 0, base_line);
					}
					if (kt.blur_size > 0)
					{
						ft_item tt;
						ps.push_back(tt);
						auto fit = &ps[ps.size() - 1];
						Fonts::get_blur(blur, bitmap, kt.blur_size, 1, &bitbuf[1]);
						auto buimg = push_cache_bitmap(blur, &pos, 0, -1);
						auto frc = glm::ivec4(pos.x, pos.y, blur->width, blur->rows);
						fit->set_it(kt.unicode_codepoint, buimg, frc, { rc.x, rc.y }, bitmap->advance, kt.blur_size, base_line);
					}
					if (ps.size())
					{
						std::vector<ft_item*> outit;
						font->push_cache(key, ps.data(), ps.size(), &outit);
						if (!ret[0] && outit.size() > 1)
						{
							ret[0] = outit[0];
							ret[1] = outit[1];
						}
						else
						{
							int idx = 0;
							if (kt.blur_size > 0)idx = 1;
							ret[idx] = outit[0];
						}
					}
				}
				else
				{
					ret.clear();
				}
			}
			return ret;
		}
		static int get_kern_advance(tt_info* font, int g1, int g2)
		{
			return font ? font_dev::getGlyphKernAdvance(&font->font, g1, g2) : 0;
		}
		static int get_kern_advance_ch(tt_info* font, int ch1, int ch2)
		{
			return font ? font_dev::getKernAdvanceCH(&font->font, ch1, ch2) : 0;
		}

#define _NO_CACHE_0
#if 1
		// 获取单个u8字符宽高
		glm::ivec3 get_extent_ch(css_text * csst, const char* str)
		{
			glm::ivec3 ret;
			tt_info* font = csst->get_font();
			unsigned int codepoint;
			auto t = Fonts::get_u8_last(str, &codepoint);
			if (font)
			{
				auto cs = font->get_char_extent(codepoint, csst->get_font_size(), csst->get_font_dpi());
				int c = t - str;
				ret = { cs.x, cs.y, c };
			}
			return ret;
		}
		// 获取单个unicode字符宽高
		glm::ivec3 get_extent_cp(css_text* csst, unsigned int codepoint)
		{
			glm::ivec3 ret;
			tt_info* font = csst->get_font();
			if (font)
			{
				ret = font->get_char_extent(codepoint, csst->get_font_size(), csst->get_font_dpi());
			}
			return ret;
		}
		// 获取n个字符宽高
		text_extent get_extent_str(css_text* csst, const char* str, size_t count, size_t first)
		{
			text_extent ret;
			tt_info* font = csst->get_font();
			tt_info* rfont = nullptr;
			unsigned int codepoint;
			auto t = utf8_char_pos(str, first);
			size_t i = 0;
			int maxh = 0;
			for (; *t && i < count; i++)
			{
				if (*t == '\n')
				{
					break;
				}
				if (*t == '\t')
				{
					auto spc = get_extent_cp(csst, L' ');
					ret._size.x += spc.z * csst->_tabs + csst->_fzpace;
					t++;
					continue;;
				}
				t = Fonts::get_u8_last(t, &codepoint);
				auto k = get_extent_cp(csst, codepoint);
				ret._size.x += k.z + csst->_fzpace;
				ret._size.y = std::max(ret._size.y, k.y);
			}
			ret.next_char = t;
			return ret;
		}
		// todo		渲染到image
		void build_text(Image* dst, Fonts::tt_info* font, const std::string& str, size_t count, size_t first_idx, double font_size, unsigned int color = -1
			, glm::ivec2 pos = { 0,0 }, unsigned char blur_size = 0, glm::ivec2 blur_pos = { 0,0 }, unsigned int color_blur = -1
			, double dpi = 96, bool first_bitmap = true)
		{
			Fonts::tt_info* rfont = nullptr;
			double fns = font_size * dpi / 72.0;
			const char* t = str.c_str();// +first_idx;
			t = utf8_char_pos(t, first_idx, str.size());
			unsigned int unicode_codepoint = 0;
			char32_t ch = 0;
			double scale = font->get_scale_height(fns);
			int px = pos.x, py = pos.y, tpx = 0, tpy = 0;
			int img_height = dst->width;
			double line_height = font->get_line_height(fns);
			double base_line = font->get_base_line(fns);
			double adv = 0; int kern = 0;
#ifdef _NO_CACHE_
			// 增强亮度
			float brightness = 0.0;
			glm::ivec4 rc;
			Fonts::Bitmap bitmap[1] = {};
			Fonts::Bitmap blur[1] = {};
			std::vector<char> bit_buf[2];
			// 分配缓存区
			bit_buf[0].resize(line_height * line_height);
			bit_buf[1].resize(line_height * line_height);
#else
			Fonts::ft_key_s fks[1] = {};
			auto& fk = fks->v;
			std::vector<Fonts::ft_item*> ftits;
#endif // _NO_CACHE_
			auto ft = this;// get_ft(0);
			for (int i = 0; i < count && t && *t; i++)
			{
				const char* t1 = t, * t2;
				t = Fonts::get_u8_last(t, &unicode_codepoint);
				t2 = t;
				if (ch)
					kern = Fonts::get_kern_advance_ch(font, ch, unicode_codepoint);
				if (kern != 0)
				{
					auto kernf = scale * kern;
					kern = kernf;
				}
				ch = unicode_codepoint;
				glm::ivec4 fs;
#ifndef _NO_CACHE_
				fk.unicode_codepoint = unicode_codepoint;
				fk.font_size = font_size;
				fk.font_dpi = dpi;
				fk.blur_size = blur_size;
				ftits = ft->make_cache(font, fks, fns, false);
				if (ftits.size())
				{
					auto ftit = ftits[0];
					if (tpx + ftit->_advance > img_height)
					{
						tpx = px = 0;
						py += line_height;
					}
					double dbl = ftit->_baseline_f > 0 ? ftit->_baseline_f : base_line;
					// 缓存图画到目标图像
					if (blur_size > 0 && ftits.size() > 1)
					{
						auto bft = ftits[1];
						glm::ivec2 dps = { kern + tpx + bft->_baseline.x + blur_pos.x - blur_size,
							py + dbl + bft->_baseline.y + blur_pos.y - blur_size };
						dst->draw_image2(bft->_image, bft->_rect, dps, color_blur);
					}
					glm::ivec2 dps = { kern + tpx + ftit->_baseline.x,py + dbl + ftit->_baseline.y };
					dst->draw_image2(ftit->_image, ftit->_rect, dps, color);
					adv = ftit->_advance;
				}
				tpx += adv;
#else
				int gidx = font->get_glyph_index(unicode_codepoint, &rfont);
				if (rfont)
				{
					auto bit = rfont->get_glyph_image(gidx, fns, &rc, bitmap, &bit_buf[0], false);
					if (blur_size > 0)
					{
						Fonts::get_blur(blur, bitmap, blur_size, 1, &bit_buf[1]);
					}
					auto cpbit = blur;

					if (tpx + bitmap->advance > img_height)
					{
						tpx = px = 0;
						py += line_height;
					}

					if (bit)
					{
						double dbl = ftit->_baseline_f > 0 ? ftit->_baseline_f : base_line;
						// 灰度图转RGBA
						if (blur_size > 0)
						{
							glm::ivec4 brc = { kern + tpx + rc.x + blur_pos.x - blur_size, py + dbl + rc.y + blur_pos.y - blur_size,
								cpbit->width, cpbit->rows };
							dst->copy_to_image(cpbit->buffer, cpbit->pitch, brc, color_blur, cpbit->pixel_mode, 0, true);
						}
						glm::ivec4 frc = { kern + tpx + rc.x,py + dbl + rc.y, bitmap->width, bitmap->rows };
						dst->copy_to_image(bitmap->buffer, bitmap->pitch, frc, color, bitmap->pixel_mode, brightness, true);
					}
				}
				tpx += bitmap->advance;
#endif
			}

#ifndef _NO_CACHE_
			//if (ftit && ftit->_image)
			//	ftit->_image->saveImage("temp/test_cache.png");
#endif
			return;
		}
		// todo		生成到draw_font_info
		glm::ivec2 build_info(Fonts::css_text* csst, const std::string& str, size_t count, size_t first_idx, glm::ivec2 pos, draw_font_info* out)
		{
			glm::ivec2 ret;
			auto font = csst->get_font();
			if (!csst || !font || count < 1)
			{
				return ret;
			}
			Fonts::tt_info* rfont = nullptr;
			double fns = csst->get_fheight();
			const char* t = str.c_str();
			t = utf8_char_pos(t, first_idx, str.size());
			unsigned int unicode_codepoint = 0;
			char32_t ch = 0;
			int px = pos.x, py = pos.y, tpx = px, tpy = py, mx = 0;
			int img_height = INT_MAX;
			double scale = font->get_scale_height(fns);
			double line_height = font->get_line_height(fns);
			double oline_height = line_height;
			int line_gap = 0;
			double xe = font->get_xmax_extent(fns, &line_gap);
			double base_line = font->get_base_line(fns);
			double adv = 0;
			double outline = csst->row_y;
			double bs = 0;
			auto rbl = font->get_rbl_height(fns, nullptr, nullptr);
			if (csst->row_height > rbl.x)
			{
				py += csst->row_height - line_height;
				line_height = csst->row_height;
				bs = csst->row_base_y - base_line;
			}
			else
			{
				if (csst->row_y != 0)
					outline = font->get_line_height(fns, false);
			}
			Fonts::ft_key_s fks[1] = {};
			auto& fk = fks->v;
			unsigned int oc = csst->outline_color ? csst->outline_color : csst->color;
			std::vector<Fonts::ft_item*> ftits;
			int kern = 0;
			std::vector<draw_image_info> vitems, * vitem = &vitems;
			std::vector<draw_image_info> vblurs;
			std::vector<glm::ivec2> vposi, * vpos = &vposi;
			if (out)
			{
				vitem = &out->vitem;
				vpos = &out->vpos;
			}

			vitem->reserve(std::min(count, str.size()));
			std::wstring twstr;
			double maxbs = 0.0;
			for (int i = 0; i < count && t && *t; i++)
			{
				const char* t1 = t, * t2;
				t = Fonts::get_u8_last(t, &unicode_codepoint);
				if (unicode_codepoint == '\n')
				{
					mx = std::max(tpx, mx);
					tpx = px;
					py += line_height;
					tpy += line_height;
					ch = 0;
					continue;
				}
				t2 = t;
				if (ch)
					kern = Fonts::get_kern_advance_ch(font, ch, unicode_codepoint);
				if (kern != 0)
				{
					auto kernf = scale * kern;
					kern = kernf;
				}
				twstr.push_back(unicode_codepoint);
				ch = unicode_codepoint;
				fk.unicode_codepoint = unicode_codepoint;
				fk.font_size = csst->get_font_size();
				fk.font_dpi = csst->get_font_dpi();
				fk.blur_size = csst->get_blur_size();
				ftits = make_cache(font, fks, fns, csst->first_bitmap);
				if (ftits.size())
				{
					auto ftit = ftits[0];

					if (tpx + ftit->_advance > img_height)
					{
						mx = std::max(tpx, mx);
						tpx = px;
						py += line_height;
						tpy += line_height;
					}
					double dbl = /*ftit->_baseline_f > 0 ? ftit->_baseline_f :*/ base_line;
					// 缓存图画到目标图像
					if (fk.blur_size > 0 && ftits.size() > 1)
					{
						auto bft = ftits[1];
						glm::ivec2 dps = { tpx + bft->_baseline.x + csst->blur_pos.x - fk.blur_size,
							py + dbl + bft->_baseline.y + csst->blur_pos.y - fk.blur_size };
						draw_image_info dii;
						dii.user_image = bft->_image;
						dii.a = { dps.x + kern, dps.y, bft->_rect.z, bft->_rect.w };
						dii.rect = bft->_rect;
						dii.col = csst->color_blur;
						dii.unser_data = (void*)unicode_codepoint;
						vblurs.push_back(dii);
					}
					glm::ivec2 dps = { tpx + ftit->_baseline.x,py + dbl + ftit->_baseline.y };
					draw_image_info dii;
					dii.user_image = ftit->_image;
					dii.a = { dps.x + kern, dps.y, ftit->_rect.z, ftit->_rect.w };
					dii.rect = ftit->_rect;
					dii.col = csst->color;
					dii.adv = adv;
					dii.unser_data = (void*)unicode_codepoint;
					adv = ftit->_advance + kern;
					double iy0 = (double)dii.a.y + dii.a.w - pos.y;
					iy0 -= fns;
					maxbs = std::max(maxbs, iy0);
					vitem->push_back(dii);
				}
				vpos->push_back({ adv, py });
				tpx += adv + csst->_fzpace;
			}
			njson posn, dif;
			maxbs -= bs;
			mx = std::max(tpx, mx);
			ret.x = mx;
			ret.y = pos.y;
			if (out)
			{
				out->base_line = base_line;
				out->diffbs = bs;
				out->awidth = mx;
			}
			float maxh = pos.y + outline, miny = pos.y;
			double cury = 0;
			int i = 0;
			for (auto& it : *vitem)
			{
				posn.push_back(v4to(it.a));
				double iy = (double)it.a.y + it.a.w - pos.y;
				iy -= fns;
				dif.push_back(iy);
				auto t = (unsigned int)it.unser_data;
				it.a.y -= maxbs;
				if (i < vblurs.size())
					vblurs[i].a.y -= maxbs;
				if (t > 255)
				{
					it.a.y = std::max(it.a.y, miny);
					if (it.a.y + it.a.w > maxh)
					{
						double diff = it.a.y + it.a.w - maxh;
						it.a.y -= diff;
						if (i < vblurs.size())
							vblurs[i].a.y -= diff;
					}
				}
				i++;
			}
			if (vblurs.size())
				vitem->insert(vitem->begin(), vblurs.begin(), vblurs.end());
			return ret;
		}
#endif
	public:
#ifndef _FONT_NO_BITMAP

#endif // !_FONT_NO_BITMAP

	public:
		/*
		todo			路径填充光栅化-function
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


	// css_text
}; //!hz
#endif // !__font_h__

#if 0
		// todo		文本渲染
glm::ivec2 draw_text(Fonts::css_text * csst, glm::ivec2 pos, const std::string & str, size_t count = -1, size_t first_idx = 0, draw_font_info * out = nullptr)
{
	auto ft = Fonts::s();
	draw_font_info dfi;
	if (!out)
	{
		out = &dfi;
	}
	glm::ivec2 ret = ft->build_info(csst, str, count, first_idx, pos, out);
	double oly = csst->row_base_y + 1;
	double tns = csst->line_thickness;
	auto fns = csst->get_fheight();
	double base_line = csst->font->get_base_line(fns);
	double bs = csst->row_base_y - base_line;
	if (csst->text_decoration & hz::Fonts::css_text::underline)
	{
		//下划线
		oly += tns;
		auto col = csst->color;
		draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns);
	}
	if (csst->text_decoration & hz::Fonts::css_text::overline)
	{
		//上划线
		auto col = csst->color;
		draw_line({ pos.x, pos.y }, { out->awidth, pos.y }, col, tns);
	}
	for (auto& it : out->vitem)
	{
		draw_image(&it);
	}
	if (csst->text_decoration & hz::Fonts::css_text::line_through)
	{
		//贯穿线
		tns = csst->line_thickness1;
		oly = bs + base_line * 0.68;
		auto col = csst->color;
		draw_line({ pos.x, pos.y + oly }, { out->awidth, pos.y + oly }, col, tns);
	}
	return ret;
}
#endif