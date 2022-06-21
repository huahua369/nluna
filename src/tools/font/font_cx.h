#ifndef __font_h__
#define __font_h__
#define hz_font_h
#include <unordered_map>
#include <map>
#include <atomic>
#include <algorithm>
#include <codecvt>
#include <string>
#include <ntype.h>
#include <base/Singleton.h>
#include "font/tags.h"
#include <view/image.h>
#include <view/image_packer.h>
#include <data/json_helper.h>
#include <view/draw_info.h>
#include <view/mapView.h>
#include <view/file.h>
//#define 0_STD_STR_
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

	// 简易stb_image加载
	class stbimage_cx
	{
	public:
		int width = 0;
		int height = 0;
		int comp = 4;
		int rcomp = 4;
		uint8_t* pxdata = nullptr;
	public:
		stbimage_cx();
		stbimage_cx(const char* fn);

		~stbimage_cx();
		bool load(const char* fn);

		bool load_mem(const char* d, size_t s);

	};

	/*---------------------------------------------------------------------------------------------------------------------------------------------------
	TODO	字体管理

	----------------------------------------------------------------------------------------------------------------------------------------------------*/
	class glyph_shape;
	class font_impl_info;
	class Fonts;
	class ft_item;
	// class ftg_item;
	class Glyph;


	class ftg_item
	{
	public:
		unsigned int _glyph_index = 0;
		Image* _image = nullptr;
		// 缓存位置xy, 字符图像zw(width,height)
		glm::ivec4 _rect;
		// 渲染偏移坐标
		glm::ivec2 _dwpos;
		// 原始的advance
		int advance = 0;
	};

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

#if 1
	typedef uint8_t byte;
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
		unsigned char   lcd_mode;
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
			//unsigned char is_bitmap;
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

#endif

	struct meta_tag
	{
		std::string tag;
		std::string v;
	};
	struct bitmap_ttinfo;
	class tt_info;
	struct familys_t {
		tt_info** familys = 0;
		size_t count = 0;
	};
	// todo				 tt_info
	class tt_info
	{
	public:
		friend class SBitDecoder;
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
				const char* name, int len);

			std::string get_name(bool isu8 = true);
			void print();

			//static std::wstring to_wstr(std::wstring str, bool is_swap = false);
		};

	public:
		// todo			tt_info数据区
		Fonts* ctx = 0;
		//k=language_id			2052 简体中文	1033 英语
		std::map<int, std::vector<info_one*>> _detail;
		std::vector<meta_tag>	_meta;
		font_impl_info* _font = 0;
		std::string _name, _aname;
		std::string _style;
		// 支持的语言
		std::string _slng;
		unsigned int  _index = 0;
		int isFixedPitch = 0;
		int postFixedPitch, charFixedPitch;
		bool is_copy = true;
		std::vector<unsigned char> _vdata;
		unsigned char* data = 0;
		int dataSize = 0;
		unsigned char freeData = 0;
		head_table head;
		hheaTbl hhea;
		post_header post;
		bool first_bitmap = false;
		float ascender = .0;
		float descender = .0;
		float lineh = .0;
		float fh = .0;
		Glyph* glyphs_ = 0;
		int cglyphs = 0;
		int nglyphs = 0;
		//int lut[256];
		//std::vector<tt_info*> fallbacks;

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
		std::unordered_map<uint64_t, ft_item*> _cache_table, _cache_table1;
		// 数据表
		//std::vector<ft_item> _ft_item_data;
		std::list<ft_item> _ft_item_data, _ft_item_data1;
		// 索引缓存 unsigned int glyph_index;uint16_t height;
		std::unordered_map<uint64_t, ftg_item*> _cache_glyphidx;
		auto_destroy_cx _cdc;
	private:
		bitmap_ttinfo* bitinfo = 0;

		// 自动化析构
		auto_destroy_cx dc;
	public:
		tt_info();

		~tt_info();
		friend class Fonts;
		static tt_info* new_info()
		{
			return new tt_info();
		}
		static void free_info(tt_info* p)
		{
			delete p;
		}
		const char* init(const char* d, size_t s, bool iscp);
		void add_info(int platform,
			int encoding,
			int language,
			int nameid,
			const char* name, int length);
		std::vector<info_one*>* get_info(int language);
		// 获取中文信息
		std::string get_info_str(int language = 2052, int idx = 1);

		size_t get_info_strv(int language, int idx, std::vector<uint16_t>& buf);

		bool is_script(const char* s);
		void print_info(int language = -1);

		Glyph* alloc_glyph();
		Glyph* find_glyph(int codepoint);
		void inser_glyph(Glyph* p);
		void set_first_bitmap(bool is);
	public:
		//void push_fallbacks(tt_info* fp)
		//{
		//	LOCK_W(_lock);
		//	fallbacks.push_back(fp);
		//}
		/*
		todo				缓存操作

		字符utf16(int32)、字号{unsigned short 1-1638}、模糊大小(unsigned short)
		// 缓存位置xy, 字符图像zw(width,height)
		// 基线偏移
		*/
		ft_item* push_cache1(ft_key_s* key, Image* img, const glm::ivec4& rect, const glm::ivec2& baseline, double advance, double blf);
		// 批量插入
		ft_item* push_cache(ft_key_s* key, ft_item* ps, size_t n, std::vector<ft_item*>* out);
		void clear_cache();
		// 查询返回指针
		ft_item* find_item(ft_key_s* key);
		// todo 获取字符大小
		glm::ivec3 get_char_extent(char32_t ch, unsigned char font_size, unsigned short font_dpi, std::vector<tt_info*>* fallbacks);
		void clear_char_lut();

		// 字形索引缓存
		ftg_item* push_gcache(unsigned int glyph_index, uint16_t height, Image* img, const glm::ivec4& rect, const glm::ivec2& pos);
		ftg_item* get_gcache(unsigned int glyph_index, uint16_t height);
		void clear_gcache();

		ftg_item mk_glyph(unsigned int glyph_index, unsigned int unicode_codepoint, int fns);
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
		// 获取字体最大box
		glm::ivec4 get_bounding_box(double scale, bool is_align0);

		const char* get_glyph_index_u8(const char* u8str, int* oidx, tt_info** renderFont, std::vector<tt_info*>* fallbacks);
		const char* get_glyph_index_u8(const char* u8str, int* oidx, tt_info** renderFont, familys_t* fallbacks);
		//int get_glyph_index_u8(const char* u8str, tt_info** renderFont, std::vector<tt_info*>* fallbacks);
		int get_glyph_index(unsigned int codepoint, tt_info** renderFont, std::vector<tt_info*>* fallbacks);
	public:
		void init_post_table();
		// 初始化位图信息
		int init_sbit();
		bool is_fixed_pitch()
		{
			return isFixedPitch;
		}
	public:
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
		int get_glyph_image(int gidx, double height, glm::ivec4* ot, Bitmap* bitmap, std::vector<char>* out, int lcd_type, uint32_t unicode_codepoint);
		/*
		获取量
		*/
		double get_scale_height(int height);
		int get_line_height(double height, bool islinegap = true);
		// 获取最大行高
		glm::ivec3 get_rbl_height(double height, glm::ivec3* out, bool* is);
		double get_base_line(double height);
		int get_xmax_extent(double height, int* line_gap);
		void get_VMetrics(int* ascent, int* descent, int* lineGap);
		void get_HMetrics(int codepoint, int* advance, int* lsb);
		void get_HMetrics_idx(int glyphindex, int* advance, int* lsb);
	private:
		int get_glyph_bitmap(int gidx, int height, glm::ivec4* ot, Bitmap* out_bitmap, std::vector<char>* out);
		int get_custom_decoder_bitmap(uint32_t unicode_codepoint, int height, glm::ivec4* ot, Bitmap* out_bitmap, std::vector<char>* out);
	};
	// !tt_info
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

		// 横向对齐
		enum class text_align :int
		{
			null, left, center, right, justify, inherit
		};
		// 纵向对齐
		enum class vertical_align :int
		{
			null, top, middle, bottom, baseline
		};
	public:
		Fonts* ft = 0;
		unsigned int color = -1;
		unsigned int color_blur = 0xff000000;
		unsigned int color_selection = 0x80ff9033;
		unsigned int outline_color = 0;
		// 装饰线
		unsigned int text_decoration = 0;
		double underline_position = 0.0;
		double underline_thickness = 1.0;
		double line_thickness = 1.0;

		double row_height = -1;
		double row_base_y = 0;
		// 最大行高
		int row_y = 0;
		// tab宽度
		uint8_t _tabs = 2;
		// 字间隔
		int _zpace = 0;
		// 行间隔
		int _lineSpacing = 0;
		css_text* _row = 0;
		struct
		{
			int x = 0, y = 0;
		} blur_pos;
		double  _fzpace = 0.0, _flineSpacing = 0.0;
		text_align _text_align = text_align::null;
		vertical_align _vertical_align = vertical_align::null;
		std::vector<glm::ivec3> colors;
		// lcd模式{1,1},{3,1},{1,3}
		int lcd_type = 0;
	private:
		//tt_info* font = nullptr;
		std::vector<tt_info*> font_family;
		double font_size = 12;
		double dpi = 96;
		unsigned char blur_size = 0;
		std::atomic_int* _upinc = nullptr;
		int _curinc = -1;
		// 字高
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
				blur_pos = c.blur_pos;
				_fzpace = c._fzpace;
				_flineSpacing = c._flineSpacing;
				font_family = c.font_family;
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
		static void destroy(css_text* p)
		{
			if (p)delete p;
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
			return font_family[0];
		}
		auto& get_fonts()
		{
			return font_family;
		}
		auto get_font_family()
		{
			return &font_family;
		}
		void set_font(tt_info* p)
		{
			size_t n = 1;
			if (font_family.size() < n)
			{
				font_family.resize(n);
			}
			for (size_t i = 0; i < n; i++, p++)
			{
				font_family[i] = p;
			}
		}
		void set_font2(tt_info* en, tt_info* cn)
		{
			size_t n = cn ? 2 : 1;
			if (font_family.size() < n)
			{
				font_family.resize(n);
			}
			font_family[0] = en;
			if (cn)
				font_family[1] = cn;
		}
		void set_fonts(tt_info** p, size_t n = 1)
		{
			if (font_family.size() < n)
			{
				font_family.resize(n);
			}
			for (size_t i = 0; i < n; i++, p++)
			{
				font_family[i] = *p;
			}
		}
		void set_font_size(double size)
		{
			font_size = size;
			update();
			mk_fns();
		}
		void set_font_dpi(double dpi_)
		{
			if (dpi_ > 0.0)
				dpi = dpi_;
			update();
			mk_fns();
		}
		void set_blur_size(unsigned char bs)
		{
			blur_size = bs;
		}
		void set_blur(glm::ivec2 offset, unsigned int col, unsigned char bsize)
		{
			blur_pos.x = offset.x;
			blur_pos.y = offset.y;
			color_blur = col;
			blur_size = bsize;
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
		double get_line_height()
		{
			return row_height;
		}
	private:
		void mk_fns();
	};
	// todo 字符宽高结构
	class text_extent
	{
	public:
		struct ystrr_t {
			size_t n = 0;		// 字符数
			size_t size = 0;	// 字节数
			glm::ivec2 autobr;	// 自动换行前面的字符数，偏移
			const char* s = 0;	// utf8字符串
			std::string str;
		};
		struct item_t
		{
			int weight = 0;	// 宽/高度
			int n = 0;		// 数量
			bool rtl = false;		// 右-左 光标移动用
			bool rtl_range = false;	// 右-左 范围判断
		};
		// 每行宽度信息
		std::vector<std::vector<item_t>> _line;
		// 垂直方向，高度
		std::vector<item_t> _y;
		// 每行字符串信息
		std::vector<ystrr_t> _ystr;
		// x数量，y索引号	真实行数
		std::vector<glm::ivec2> _ycpos;
		// 每行宽度
		std::vector<int> _y_width;
		glm::ivec3 _size;
		// 容器宽度，自动换行
		unsigned int _view_width = -1;
		// 行距
		unsigned int _row_span = 0;
		// 大于0.52则选中下一个
		float _offset = 0.52f;
		const char* _endstr = 0;
		// 光标位置
		glm::ivec2 _cpos;
		//
		std::vector<glm::ivec2> _cposv;
		// 渲染光标信息
		glm::ivec4 _draw_c;
		glm::ivec2 _text_pos;
		// 事件处理的大小
		glm::ivec2 _ev_size;
		t_unordered_map<int, std::vector<glm::ivec3>> _mav;
	private:
		item_t* _last = 0;
		item_t* _lasty = 0;
		std::vector<item_t>* _lastys = 0;
		int mx = 0;
		int curry = 0;
	public:
		text_extent()
		{
		}

		~text_extent()
		{
		}
		void clear();
		bool empty();
		// 设置自动换行宽度，-1则不会自动换行
		void set_view_width(int w, int row_span);
		// 设置光标位置
		void set_cpos(glm::ivec2 cp, int idx = 0);
		// 获取光标
		glm::ivec2 get_cpos();
		// 添加光标
		void set_cpos2(int idx, glm::ivec2 cp);
		// 多光标
		glm::ivec2 get_cpos2(int idx);
		void clear2();
		glm::ivec2 cpos2te(glm::ivec2 cp);
		// text_extent对象坐标转 从1开始的坐标
		glm::ivec2 te2cpos2(const glm::ivec2& cp);
		bool is_end(const glm::ivec2& cp);
		// 加减光标位置
		void set_cpos_inc_x(int x);
		void set_cpos_inc_y(int y);
		glm::ivec2 get_cpos_inc_x(int x, int idx = 0);
		glm::ivec2 get_cpos_inc_y(int y, int idx = 0);
		// home键=true, end=false
		void set_cpos_home_end(bool ish);
		glm::ivec4 get_cu_idx();
		glm::ivec4 get_px_idx(int idx);
		// 获取坐标在文本的行列位置return{字符位置xy，光标坐标zw}当前行高height
		glm::ivec4 get_pos_idx(glm::ivec2 pos, bool* is_range, glm::ivec2* cp);
		glm::ivec4 get_pos_idx(glm::ivec2 pos);

		// 获取索引在文本的行列位置return{字符位置xy，光标坐标zw}
		glm::ivec4 get_cursor_idx(glm::ivec2 idx);

		// 获取坐标下文本rlen返回字符数量，rsize返回字节
		const char* get_substr(glm::uvec2 first, glm::uvec2 second, size_t* rlen, size_t* rsize);
		// char*pstr=0; auto n=get_substr（first,second, out, &pstr）
		size_t get_substr(glm::uvec2 first, glm::uvec2 second, std::string& out, const char** pstr);
		// 获取选区		glm::ivec3{x=第一个位置，z=宽度位置，y}
		void get_pos2_rects(glm::ivec2 first, glm::ivec2 second, t_vector<glm::ivec4>& outvd, int height);

		size_t get_line_count();
		size_t get_showline_count();
	public:
		void pushx(int x, bool rtl);
		void pushy(int y, int cy, const char* t, const glm::ivec2& autobr, int height);
		size_t getlast_y();
		void mk_last_line(const char* bt);
		glm::ivec2 size() {
			auto ret = glm::ivec2(_size.x, _size.z);
			return ret;
		};
		glm::ivec3 size3() {
			return _size;
		};
		int& x1() { return _size.x; }
		int& y1() { return _size.y; }
		int& z1() { return _size.z; }
	private:
		glm::ivec2 get_idx(std::vector<item_t>& v, int x, float inv, int* o);
		//glm::ivec3 get_idx2(std::vector<item_t>& v, int idx);
		// make 位置表
		std::vector<glm::ivec3>& get_av(int y);
	};
	// 字符串渲染结构
	class css_text_info
	{
	public:
		double fns = 0;
		double outline = 0;

		tt_info* font = nullptr;
		double scale = 0;
		double line_height = 0;
		double oline_height = 0;
		int line_gap = 0;
		double xe = 0;
		double base_line = 0;
		double bs = 0;
		double maxbs = 0;
		double py = 0;
		glm::ivec3 rbl;
	public:
		css_text_info()
		{
		}

		~css_text_info()
		{
		}
		void set_css(css_text* csst, tt_info* ft)
		{
			outline = csst->row_y;
			fns = csst->get_fheight();

			font = ft;
			scale = font->get_scale_height(fns);
			line_height = font->get_line_height(fns);
			oline_height = line_height;
			line_gap = 0;
			xe = font->get_xmax_extent(fns, &line_gap);
			base_line = font->get_base_line(fns);
			auto& ftts = csst->get_fonts();
			for (auto it : ftts)
			{
				base_line = std::max(base_line, (double)it->get_base_line(fns));
			}
			bs = 0;
			rbl = font->get_rbl_height(fns, nullptr, nullptr);
			if (csst->row_height > rbl.x)
			{
				py = csst->row_height - line_height;
				line_height = csst->row_height;
				bs = csst->row_base_y - base_line;
			}
			else
			{
				if (csst->row_y != 0)
					outline = font->get_line_height(fns, false);
			}
			maxbs = -line_height;
		}
	private:

	};

	class image_text_info
	{
	public:
		Image* dst = nullptr;
		tt_info* font = nullptr;
		double font_size = 0;
		unsigned int color = -1;
		glm::ivec2 pos;
		unsigned char blur_size = 0;
		glm::ivec2 blur_pos;
		unsigned int color_blur = -1;
		double font_dpi = 96.0;
		bool first_bitmap = false;
		int lcd_type = 0;
		std::vector<tt_info*>* font_family = nullptr;
	public:
		image_text_info()
		{
		}

		~image_text_info()
		{
		}

	private:

	};


	// fd_info
	class fd_info
	{
	private:
		mfile_t* _fp = nullptr;
		char* _data = nullptr;
		int64_t _size = 0;
	public:
		fd_info()
		{
		}
		fd_info(mfile_t* p)
		{
			init(p);
		}

		~fd_info()
		{
		}
		void free_mv()
		{
			if (_fp)
			{
				delete (_fp);
			}
		}
		void init(mfile_t* p)
		{
			_fp = p;
			if (_fp)
			{
				_data = (char*)_fp->data();
				_size = _fp->size();
			}
		}
		char* data()
		{
			return _data;
		}
		int64_t size()
		{
			return _size;
		}
	private:

	};


	class Fonts
	{
	public:
		/* FIR filter used by the default and light filters */
		static void lcd_filter_fir(Bitmap* bitmap, int  mode, int weights_type);


	private:
		// TODO		数据区
		std::set<std::string> dv;
		std::set<std::string> _styles;
		std::vector<std::vector<char>*> fn_data;
		std::vector<fd_info> fd_data;
		std::vector<tt_info*> _fonts;
		// 根据字体名查找
		std::unordered_map<std::string, std::vector<tt_info*>> _tbs;
		// 读写锁
		LockS _lk;
		int def_language_id = 2052;
		// 图像缓存
		ImagePacker<Image> _packer = {};
		css_text _decss;

	public:
		void* uptr = 0;
		std::function<void(const char* name, tt_info*, void* uptr)> on_add_cb;
	public:

		Fonts();

		~Fonts();
	public:
		void set_langid(int langid);
		tt_info* get_font(int idx);
		// 文件名，是否复制数据
		int add_font_file(const std::string& fn);
		int add_font_mem(const char* data, size_t len, bool iscp);

		njson get_font_info(int language_id = 2052);

		tt_info* get_font(const std::string& name, const std::string& st = "Regular");

		void set_defontcss(tt_info* f1, tt_info* f2);
		css_text* create_css_text();
	public:

		bool is_ttc(const char* data, std::vector<uint32_t>* outv);
		//对应int32大小的成员 的转换 范例
		int32_t swapInt32(int32_t value);
		static char* tt_ushort(char* p);
		union intWithFloat
		{
			int32_t m_i32;
			float m_f32;
		};

		float swapFloat32(float value);
	public:

		// Based on Exponential blur, Jani Huhtanen, 2006

#define APREC 16
#define ZPREC 7

		static void blurCols(unsigned char* dst, int w, int h, int dstStride, int alpha);

		static void blurRows(unsigned char* dst, int w, int h, int dstStride, int alpha);
		static void Blur(unsigned char* dst, int w, int h, int dstStride, float blur, int n, int mode = 0x01 | 0x02);

		/* A possible bicubic interpolation consists in performing a convolution
		   with the following kernel :
									[ 0  2  0  0][s0]
									[-1  0  1  0][s1]
			p(t) = 0.5 [1 t t^2 t^3][ 2 -5  4 -1][s2]
									[-1  3 -3  1][s3]
			Where s0..s3 are the samples, and t is the range from 0.0 to 1.0 */
		static inline float bicubic(float s0, float s1, float s2, float s3, float t);


		/* sYX is a matrix with the samples (row-major)
		   xt and yt are the interpolation fractions in the two directions ranging from
		   0.0 to 1.0 */
		static inline float bicubic2D(
			float s00, float s01, float s02, float s03,
			float s10, float s11, float s12, float s13,
			float s20, float s21, float s22, float s23,
			float s30, float s31, float s32, float s33, float xt, float yt);
		//
		/*
		lcd =a
		a->b
		*/
		static void lcd2rgba(byte* b, byte* a, int W, int H);

		static void copy_bitmap(Bitmap* dst, Bitmap* src, int x, int y, std::vector<char>* out);
		static void init_bitmap_bitdepth(Bitmap* bitmap, int bit_depth);
		/* Enlarge `bitmap' horizontally and vertically by `xpixels' */
	/* and `ypixels', respectively.                              */

		static int ft_bitmap_assure_buffer(Bitmap* bitmap, unsigned int xpixels, unsigned int ypixels, std::vector<char>* nbuf);
		/*
		加粗bitmap，支持位图PX_MONO、PX_GRAY
		newbuf和不能和buffer一样
		*/
		static void bitmap_embolden(Bitmap* bitmap, int xstr, int ystr, std::vector<char>* newbuf = nullptr);

		static void copy_bitmap_mask(Bitmap* dst, Bitmap* src, glm::ivec2 dstpos, std::function<void(unsigned char* dp, unsigned char* sp)> func);

		static void get_blur(Bitmap* dst, Bitmap* src, float blur, int n, std::vector<char>* bitdata, int mode = 0);

		glyph_shape* get_glyph_shape(tt_info* font, const char* str);
		// 获取字形
		Glyph* get_glyph_test0(tt_info* font, const char* codepoint, short isize, short iblur, int bitmapOption = FONS_GLYPH_BITMAP_REQUIRED);
		char* get_glyph_test1(tt_info* font, unsigned int codepoint, float height, glm::ivec4* ot, std::vector<char>* out);
		Glyph* get_glyph_test(tt_info* font, unsigned int codepoint, short isize, short iblur, int bitmapOption = FONS_GLYPH_BITMAP_REQUIRED);


		static void print_test(const void* d, int w, int h);

		static int mini(int a, int b)
		{
			return a < b ? a : b;
		}

		static int maxi(int a, int b)
		{
			return a > b ? a : b;
		}

		// 获取utf8字符
		static const char* get_u8_last(const char* str, unsigned int* codepoint);
		static const char* get_u8_index(const char* str, int idx);
	public:
		// TODO:push_cache_bitmap
		Image* push_cache_bitmap(Bitmap* bitmap, glm::ivec2* pos, int linegap = 0, unsigned int col = -1);
	public:
		std::vector<Image*> get_all_cacheimage();
		void save_cache(std::string fdn);
		glm::vec3 make_outline_y(css_text* ct, size_t n);
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
		std::vector<ft_item*> make_cache(tt_info* font, ft_key_s* key, double fns, std::vector<tt_info*>* fallbacks, int lcd_type);
		static int get_kern_advance(tt_info* font, int g1, int g2);
		static int get_kern_advance_ch(tt_info* font, int ch1, int ch2);

#define _NO_CACHE_0
#if 1
		// 获取单个u8字符宽高
		glm::ivec3 get_extent_ch(css_text* csst, const char* str);
		// 获取单个unicode字符宽高
		glm::ivec3 get_extent_cp(css_text* csst, unsigned int codepoint);
		// 获取n个字符宽高
		void get_extent_str(css_text* csst, const char* str, size_t count, size_t first, text_extent* oet);
		// todo		渲染到image
		void build_text(image_text_info* info, const std::string& str, size_t first_idx, size_t count);
		/*
		void build_text(Image* dst, tt_info* font, const std::string& str, size_t count, size_t first_idx, double font_size, unsigned int color = -1
			, glm::ivec2 pos = { 0,0 }, unsigned char blur_size = 0, glm::ivec2 blur_pos = { 0,0 }, unsigned int color_blur = -1
			, double dpi = 96, bool first_bitmap = true, int lcd_type = 0);*/
			// todo		生成到draw_font_info
		glm::ivec2 build_info(css_text* csst, const std::string& str, size_t count, size_t first_idx, glm::ivec2 pos, draw_font_info* out);
		void draw_image2(Image* dst, draw_image_info* info);
		glm::ivec2 build_to_image(css_text* csst, const std::string& str, size_t count, size_t first_idx, glm::ivec2 pos, Image* dst);
		css_text* create_ct(uint32_t n);
		void free_ct(css_text* cts);
#endif
	public:
#ifndef _FONT_NO_BITMAP

#endif // !_FONT_NO_BITMAP

	public:
		static glm::ivec3 mk_fontbitmap(tt_info* font, unsigned int glyph_index, unsigned int unicode_codepoint
			, int fns, glm::ivec2 pos, unsigned int col, Image* outimg);
	private:


	};
	//！Fonts

	/*
		todo			路径填充光栅化-function
		输入路径path，缩放比例scale、bpm_size{宽高xy,每行步长z}，
		xy_off偏移默认0
		输出灰度图
		invert 1倒置, 0正常

		//#define stbtt_vertex_type short
		//	typedef struct
		//	{
		//		stbtt_vertex_type x, y, cx, cy, cx1, cy1;
		//		unsigned char type, padding;
		//	} stbtt_vertex;

		STBTT_vmove 移动，即当前不绘制，将当前的点移动到指定的位置。所以我们只要保持当前点即可，同时要结束上一次轮廓线的指定mTess->endContour();，同时开始下一次的轮廓线指定。
		STBTT_vline 即绘制直线，即将上一次的顶点插入到轮廓线insertVertex，将当前的顶点保持。
		STBTT_vcurve 即当前是绘制二次贝塞尔曲线。首先插入上次的顶点，接着取得当前贝塞尔曲线的控制点和顶点，
					此时有三个顶点，一个是上一次的顶点p0，一个是控制点p1，一个是当前的顶点p2。则通过二次贝塞尔曲线方程，则可计算曲线上的点。
					y = (1-t)*(1-t)p0 + 2*t*(1-t)p1 + t*t p2;

		对于精度要求比较高的，可以循环取得曲线上的点，我们这边则直接取得中点（t=0.5）。最后将取得的点插入到轮廓线中。
	*/
	unsigned char* get_glyph_bitmap_subpixel(stbtt_vertex* vertices, int num_verts, glm::vec2 scale, glm::vec2 shift,
		glm::vec2 xy_off, std::vector<unsigned char>* out, glm::ivec3 bpm_size, int invert = 0);

	void get_path_bitmap(stbtt_vertex* vertices, int num_verts, image_gray* bmp, glm::vec2 scale = { 1, 1 }, glm::vec2 xy_off = { 0.0f, 0.0f });
	// 生成模糊

	void build_blur(image_gray* grayblur, float blur, unsigned int fill, int blurcount, Image* dst, glm::ivec2 pos, bool iscp);


	sfnt_header* get_tag(font_impl_info* font_i, std::string tag);

	unsigned int fons_decutf8(unsigned int* state, unsigned int* codep, unsigned int byte);
	std::wstring to_wstr(const wchar_t* str, int len, bool is_swap);
	std::wstring to_wstr(uint16_t* str, int len, bool is_swap);
}; //!hz
#endif // !__font_h__

#if 0
		// todo		文本渲染
glm::ivec2 draw_text(Fonts::css_text* csst, glm::ivec2 pos, const std::string& str, size_t count = -1, size_t first_idx = 0, draw_font_info* out = nullptr)
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
