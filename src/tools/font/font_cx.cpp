
/*


2022-07-07: build_blur高斯模糊函数，get_path_bitmap路径光栅化函数

*/

#include <data/json_helper.h>

#include "font_c.h"
#include "font_cx.h"
#include "view/mapView.h"
#include <view/file.h>


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
#include "stb_src/lib_src.h"
#endif // !FONS_USE_FREETYPE

namespace hz
{
	stbimage_cx::stbimage_cx()
	{
	}

	stbimage_cx::stbimage_cx(const char* fn)
	{
		load(fn);
	}

	stbimage_cx::~stbimage_cx()
	{
		stbi_image_free(pxdata);
	}

	bool stbimage_cx::load(const char* fn)
	{
		std::vector<char> data;
		File::read_binary_file(fn, data);
		if (data.empty())
		{
			return false;
		}
		pxdata = stbi_load_from_memory((stbi_uc*)data.data(), data.size(), &width, &height, &rcomp, comp);
		//float* pxdata1 = stbi_loadf_from_memory((stbi_uc*)data.data(), data.size(), &width, &height, &comp, 4);
		//float pxf[128] = {};
		//memcpy(pxf, pxdata1, sizeof(float) * 100);
		return (pxdata ? true : false);
	}
	bool stbimage_cx::load_mem(const char* d, size_t s)
	{
		pxdata = stbi_load_from_memory((stbi_uc*)d, s, &width, &height, &rcomp, comp);
		return (pxdata ? true : false);
	}



	struct SBitDecoder;
	struct BigGlyphMetrics;
	// 获取一个字体索引的位图
	static int get_index(SBitDecoder* decoder, unsigned int glyph_index, int x_pos, int y_pos);
	int load_metrics(uint8_t** pp, uint8_t* limit, int big, BigGlyphMetrics* metrics);

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
	MapView* load_file(std::string fn)
	{
		MapView* mv = MapView::create();
#ifdef _WIN32
		if (fn[1] != ':')
			fn = File::getAP(fn);
#endif // _WIN32
		if (mv->openfile(fn.c_str(), true))			//打开文件
		{
			auto size = mv->getFileSize();
			char* buf = (char*)mv->mapview();	//获取映射内容
			if (buf)
			{
				return mv;
			}
		}
		if (mv)
		{
			MapView::destroy(mv);
		}
		return nullptr;
	}

	// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
	// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define FONS_UTF8_ACCEPT 0
#define FONS_UTF8_REJECT 12

	unsigned int fons_decutf8(unsigned int* state, unsigned int* codep, unsigned int byte)
	{
		static const unsigned char utf8d[] = {
			// The first part of the table maps bytes to character classes that
			// to reduce the size of the transition table and create bitmasks.
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 11, 6, 6, 6, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,

			// The second part is a transition table that maps a combination
			// of a state of the automaton and a character class to a state.
			0, 12, 24, 36, 60, 96, 84, 12, 12, 12, 48, 72, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
			12, 0, 12, 12, 12, 12, 12, 0, 12, 0, 12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 24, 12, 12,
			12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12,
			12, 12, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12,
			12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
		};

		unsigned int type = utf8d[byte];

		*codep = (*state != FONS_UTF8_ACCEPT) ?
			(byte & 0x3fu) | (*codep << 6) :
			(0xff >> type) & (byte);

		*state = utf8d[256 + *state + type];
		return *state;
	}
	// 文件名，是否复制数据


	/* FIR filter used by the default and light filters */

	void Fonts::lcd_filter_fir(Bitmap* bitmap, int mode, int weights_type)
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

	// todo 字体


#if 1


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
			ftError = FT_New_Memory_Face(ftLibrary, (const uint8_t*)data, dataSize, 0, font->_font);
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
			std::map<std::string, sfnt_header> _tb;
			// EBLC	Embedded bitmap location data	嵌入式位图位置数据
			int eblc = 0;
			uint32_t sbit_table_size = 0;
			int      sbit_table_type = 0;
			uint32_t sbit_num_strikes = 0;
			// EBDT	Embedded bitmap data	嵌入式位图数据 either `CBDT', `EBDT', or `bdat'
			uint32_t ebdt_start = 0;
			uint32_t ebdt_size = 0;
			// EBSC	Embedded bitmap scaling data	嵌入式位图缩放数据
			uint32_t ebsc = 0;
			int format = 0;
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
			// 字体格式
			font->format = ttUSHORT(font->font.data + font->font.index_map + 0);
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
			return size < 0 ? stbtt_ScaleForPixelHeight(&font->font, -size) : stbtt_ScaleForMappingEmToPixels(&font->font, size);
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
		static const char* get_glyph_index(font_impl* font, const char* str, int* idx)
		{
			unsigned int codepoint = 0;
			unsigned int utf8state = 0;
			int index = -1;
			for (; str && *str && index == -1; ++str) {
				if (fons_decutf8(&utf8state, &codepoint, *(const unsigned char*)str))
					continue;
				index = stbtt_FindGlyphIndex(&font->font, codepoint);
			}
			*idx = index;
			return str;
		}


#define BYTE_( p, i )  ( ((const unsigned char*)(p))[(i)] )
#define BYTE_U16( p, i, s1 )  ( (uint16_t)( BYTE_( p, i ) ) << (s1) )
#define BYTE_U32( p, i, s1 )  ( (uint32_t)( BYTE_( p, i ) ) << (s1) )
#define PEEK_USHORT( p )  uint16_t( BYTE_U16( p, 0, 8 ) | BYTE_U16( p, 1, 0 ) )
#define PEEK_LONG( p )  int32_t( BYTE_U32( p, 0, 24 ) | BYTE_U32( p, 1, 16 ) | BYTE_U32( p, 2,  8 ) | BYTE_U32( p, 3,  0 ) )
#define PEEK_ULONG( p )  uint32_t(BYTE_U32( p, 0, 24 ) | BYTE_U32( p, 1, 16 ) | BYTE_U32( p, 2,  8 ) | BYTE_U32( p, 3,  0 ) )

#define NEXT_CHAR( buffer ) ( (signed char)*buffer++ )

#define NEXT_BYTE( buffer ) ( (unsigned char)*buffer++ )
#define NEXT_SHORT( b ) ( (short)( b += 2, PEEK_USHORT( b - 2 ) ) )
#define NEXT_USHORT( b ) ( (unsigned short)( b += 2, PEEK_USHORT( b - 2 ) ) )
#define NEXT_LONG( buffer ) ( (long)( buffer += 4, PEEK_LONG( buffer - 4 ) ) )
#define NEXT_ULONG( buffer ) ( (unsigned long)( buffer += 4, PEEK_ULONG( buffer - 4 ) ) )



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
			wchar_t wt[] = { (wchar_t)codepoint, 0 };
			uint16_t format = ttUSHORT(data + index_map + 0);
			// 0*, 2, 4*, 6*, 8, 10, 12*, 13*, 14
			if (format == 2) {
				//STBTT_assert(0); // @TODO: high-byte mapping for japanese/chinese/korean
				if (codepoint > 127)
				{
					char st[4] = { 0 };
					auto astr = w2a(wt, 1, st, 4);
					char* t = (char*)&codepoint;
					t[0] = st[1]; t[1] = st[0];
				}
				ret = tt_cmap2_char_index(data + index_map, codepoint);
			}
			return ret;
		}
		// GBK字符串
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
			return font->format == 2 ? get_ext_glyph_index(&font->font, codepoint) : stbtt_FindGlyphIndex(&font->font, codepoint);
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
		static glm::ivec4 get_bounding_box(font_impl* font)
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
		static char* get_glyph_bitmap(font_impl* font, int gidx, double scale, glm::ivec4* ot, std::vector<char>* out, double* advance, glm::vec2 lcd = { 1, 1 })
		{
			//int i, j, baseline, ch = 0;
			//int baseline;
			int ascent;
			int descent;
			int linegap;
			//float xpos = 2; // leave a little padding in case the character extends left
			//float scale = stbtt_ScaleForPixelHeight(&font->font, height);
			//float scale1 = stbtt_ScaleForMappingEmToPixels(&font->font, height);
			int x0 = 0, y0 = 0, x1, y1;

			stbtt_GetFontVMetrics(&font->font, &ascent, &descent, &linegap);
			//double pscale1 = fbb.w + abs(fbb.y);
			//double pscale = ascent;
			//auto scale2 = height / pscale;
			//scale = scale2;
			//baseline = (int)(ascent * scale);
			int advancei, lsb;//, x0, y0, x1, y1;
			float shift_x = 0.0f;
			float shift_y = 0.0f;
			//auto ghl = get_codepoint_hmetrics(font, L'g');
			//auto Lhl = get_codepoint_hmetrics(font, L'L');
			stbtt_GetGlyphHMetrics(&font->font, gidx, &advancei, &lsb);
			double adv = ceil((double)advancei * scale);
			//stbtt_GetCodepointBitmapBoxSubpixel(&font->font, ch, scale, scale, shiftX, shiftY, &x0, &y0, &x1, &y1);
			stbtt_GetGlyphBitmapBoxSubpixel(&font->font, gidx, scale * lcd.x, scale * lcd.y, shift_x, shift_y, &x0, &y0, &x1, &y1);
			glm::ivec4 ot0 = {};
			if (!ot)ot = &ot0;
			*advance = adv;
			ot->x = x0;
			ot->y = y0;
			ot->z = x1 - x0;
			ot->w = y1 - y0;
			size_t pcs = (int64_t)ot->z * ot->w;
			char* pxs = 0;
			if (out)
			{
				if (out->size() < pcs)
				{
					out->resize(pcs);
				}
				pxs = out->data();
				memset(pxs, 0, out->size());
				stbtt_MakeGlyphBitmapSubpixel(&font->font,
					(unsigned char*)pxs,
					x1 - x0,
					y1 - y0,
					x1 - x0, // screen width ( stride )
					scale * lcd.x, scale * lcd.y,
					shift_x, shift_y, // shift x, shift y
					gidx);
			}

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

		static uint16_t ttUSHORT(uint8_t* p) { return p[0] * 256 + p[1]; }
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
		struct metainfo_t
		{
			uint32_t	version;//	The version of the table format, currently 1
			uint32_t	flags;//	Flags, currently unusedand set to 0
			uint32_t	dataOffset;//	Offset from the beginning of the table to the data
			uint32_t	numDataMaps;//	The number of data maps in the table
		};
		struct DataMaps_t
		{
			char tag[4];
			uint32_t	dataOffset;//	Offset from the beginning of the table to the data for this tag
			uint32_t	dataLength;//	Length of the data.The data is not required to be padded to any byte boundary.
		};
		struct metainfo_tw
		{
			uint32_t	version;//	Version number of the metadata table — set to 1.
			uint32_t	flags;//	Flags — currently unused; set to 0.
			uint32_t	reserved;//	Not used; should be set to 0.
			uint32_t	dataMapsCount;//	The number of data maps in the table.
			DataMaps_t dataMaps[1];//[dataMapsCount]	Array of data map records.
		};
		struct meta_tag_t
		{
			char tag[5];
			std::string v;
		};
		// 获取字体meta信息
		static int get_meta_string(const stbtt_fontinfo* font, std::vector<meta_tag>& mtv)
		{
			int i, count, stringOffset;
			uint8_t* fc = font->data;
			uint32_t offset = font->fontstart;
			uint32_t nm = find_table(fc, offset, "meta");
			if (!nm) return 0;
			tt_info* ttp = (tt_info*)font->userdata;
			uint8_t* tp = fc + nm;
			metainfo_tw meta = {};
			uint32_t* ti = (uint32_t*)&meta;

			for (int i = 0; i < 4; i++)
				ti[i] = ttULONG(tp + i * 4);
			if (meta.dataMapsCount > 0)
			{
				auto dm = (tp + 16);
				auto dm1 = (tp);
				mtv.resize(meta.dataMapsCount);
				for (size_t i = 0; i < meta.dataMapsCount; i++)
				{
					auto& it = mtv[i];
					it.tag.assign((char*)dm, 4);
					auto offset = ttULONG(dm + 4);
					auto length = ttULONG(dm + 8);
					if (length > 0)
						it.v.assign((char*)dm1 + offset, length);
					dm += 12;
				}
			}
			return 0;
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
			int idx = 0;
			get_glyph_index(font, str, &idx);
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
#define  font_dev stb_font
#endif
	class font_impl_info :public  font_dev::font_impl {
	public:
	};

#endif // 1

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

		int init(tt_info* ttp, uint32_t strike_index);
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

	class bitmap_ttinfo
	{
	public:

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
		tt_info* _t = 0;
		// 自定义位图
		Image* _buf = 0;
		// 支持的大小
		std::vector<glm::ivec2> _chsize;
		// 范围
		std::vector<glm::ivec2> _unicode_rang;
	public:
		bitmap_ttinfo() {}
		~bitmap_ttinfo() {
			Image::destroy(_buf);
			destroy_all_dec();
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

	};
	int SBitDecoder::init(tt_info* ttp, uint32_t strike_index)
	{
		int ret = 0;
		SBitDecoder* decoder = this;
		auto face = ttp->bitinfo;
		if (!face->_ebdt_table || !face->_sbit_table
			|| strike_index >= face->_bsts.size() || strike_index >= face->_index_sub_table.size())
			return 0;
		if (_face == ttp && _strike_index == strike_index)
		{
			return 1;
		}
		_face = ttp;
		_strike_index = strike_index;
		font_impl_info* font_i = ttp->_font;
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





#endif // !_FONT_NO_BITMAP
	// 传空白的解码指针
#if 0
	int get_glyph_bitmap_test(tt_info* _t, std::string str, int height, SBitDecoder* decoder)
	{
		font_impl_info* font_i = _t->_font;
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
		decoder->init(_t, sidx);
		bitmap = decoder->bitmap;
		for (int i = 0; t && *t; i++)
		{
			char32_t ch;
			const char* t1 = t, * t2;
			//t = get_u8_last(t, &wcp);
			t = get_glyph_index_last(font_i, t, &gidx);
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
#endif

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
		std::vector<unsigned char>* bitmap = 0;
	public:
		Glyph()
		{
		}

		~Glyph()
		{
			if (bitmap)
				delete bitmap;
			bitmap = 0;
		}
		unsigned char* resize_bitmap(int64_t w, int64_t h)
		{
			size_t s = w * h;
			if (!bitmap)
				bitmap = new std::vector<unsigned char>();
			if (s != bitmap->size())
			{
				_width = w; _height = h;
				bitmap->resize(s);
			}
			return (unsigned char*)bitmap->data();
		}
	private:

	};



	font_cx::font_cx()
	{
	}

	font_cx::~font_cx()
	{
	}
	Fonts::Fonts()
	{
		_packer.set_defmax({ 1024, 1024 });
	}

	Fonts::~Fonts()
	{
		for (auto it : fn_data)
		{
			delete it;
		}
		for (auto it : fd_data)
		{
			it.free_mv();
		}
		for (auto it : _fonts)
		{
			tt_info::free_info(it);
		}
	}
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
		void convert(stbtt_vertex* stbVertex, int verCount);
	private:

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
		void* renderfont = nullptr;
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
	int get_index(SBitDecoder* decoder, unsigned int glyph_index, int x_pos, int y_pos)
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
	njson get_bitmap_size_table(uint8_t* blc, unsigned int count,
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
	int load_metrics(uint8_t** pp, uint8_t* limit, int big, BigGlyphMetrics* metrics)
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




	void Fonts::set_langid(int langid)
	{
		def_language_id = langid;
	}
	tt_info* Fonts::get_font(int idx)
	{
		return (idx < 0 || idx > _fonts.size()) ? nullptr : _fonts[idx];
	}
	int Fonts::add_font_file(const std::string& fn, njson* pname)
	{
		int ret = 0;
#if 0
		std::vector<char>* fd;
		{
			fd = new std::vector<char>();
#ifdef __FILE__h__
			hz::File::read_binary_file(fn, *fd);
#else
			read_binary_file(fn, fd);
#endif // __FILE__h__
		}
		if (fd->size())
		{
			print_time pti("add_font_file afm");
			fn_data.push_back(fd);
			//nd->swap(data);
			ret = add_font_mem(fd->data(), fd->size(), false);
		}
#else
		//MapView* mvfp;
		//{
		//	//print_time pti("add_font_file");
		//	mvfp = load_file(fn);
		//}
		auto mv = new mfile_t();
		auto md = mv->open_d(fn, true);
#endif // 0
		if (!md)
		{
			delete mv;
			return ret;
		}
		if (mv && mv->get_file_size())
		{
			//print_time pti("add_font_file afm");
			auto& fdi = fd_data.emplace_back(fd_info{ mv });
			ret = add_font_mem(fdi.data(), fdi.size(), false, pname);
		}
		return ret;
	}

	// todo 初始化位图信息
	void nsimsun_ascii(bitmap_ttinfo* obt);

	int Fonts::add_font_mem(const char* data, size_t len, bool iscp, njson* pname)
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
			font->ctx = this;
			stb_font::font_impl* fi = font->_font;
			hr = ft.loadFont(fi, data, i, font);
			if (hr)
			{
				font->num_glyphs = fi->font.numGlyphs;
				int ascent = 0, descent = 0, lineGap = 0;
				ft.getFontVMetrics(fi, &ascent, &descent, &lineGap);
				auto fh = ascent - descent;
				//njson fot;num_glyphs
				//fot["ascender"] = (float)ascent / (float)fh;
				//fot["descender"] = (float)descent / (float)fh;
				//fot["lineh"] = (float)(fh + lineGap) / (float)fh;
				//fot["_ascent"] = ascent;
				//fot["_descent"] = descent;
				//fot["_lineGap"] = lineGap;
				auto name = ft.get_font_name(fi);
				//ftn.push_back(fot);
				fp.push_back(fi);
				font->fh = fh;
				font->_index = i;
				font->ascender = (float)ascent / (float)fh;
				font->descender = (float)descent / (float)fh;
				font->lineh = (float)(fh + lineGap) / (float)fh;
				font->_name = font->get_info_str(def_language_id);
				if (pname)
				{
					if (!pname->is_array())
						*pname = njson::array();
					pname->push_back(font->_name);
				}
				auto cn_name = font->get_info_str(1033);
				font->_aname = u8_gbk(font->_name);
				auto a_style = font->get_info_str(2052, 2);
				auto a_style1 = font->get_info_str(1033, 2);
				auto str6 = font->get_info_str(2052, 6);
				auto str61 = font->get_info_str(1033, 6);
				font->_style = a_style1;
				//if (font->_aname == "新宋体")
				ft.get_meta_string(&fi->font, font->_meta);
				for (auto& it : font->_meta)
				{
					if (it.tag == "slng")
					{
						font->_slng = it.v;
					}
				}
				_styles.insert(font->_style);
				if (1)
				{
					// print_time pti("add_font_mem");
					font->init_post_table();
					font->init_color();
#ifndef _FONT_NO_BITMAP
					font->init_sbit();
					if (cn_name == "NSimSun")
					{
						nsimsun_ascii(font->bitinfo);
					}
#endif // !_FONT_NO_BITMAP
					//LOCK_W(_lk);
					_tbs[font->_name].push_back(font);
					_fonts.push_back(font);
				}
				if (on_add_cb)
					on_add_cb(font->_name.c_str(), font, uptr);
				//printf("%s\n", font->_aname.c_str());
				//font->print_info();
			}
			else
			{
				tt_info::free_info(font);
			}
		}
		return fp.size();
	}

	njson Fonts::get_font_info(int language_id)
	{
		//LOCK_R(_lk);
		njson ret;
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

	tt_info* Fonts::get_font(const std::string& name, const std::string& st)
	{
		//LOCK_R(_lk);
		tt_info* ret = nullptr;
		tt_info* ret1 = nullptr;
		auto it = _tbs.find(name);
		if (it != _tbs.end() && it->second.size())
		{
			ret = it->second.size() ? it->second[0] : nullptr;
			for (auto& ft : it->second)
			{
				auto fst = ft->_style;
				if (!ret1)ret1 = ft;
				if (fst == st)
				{
					ret = ft;
				}
			}
		}
		return ret ? ret : ret1;
	}

	sfnt_header* get_tag(font_impl_info* font_i, std::string tag)
	{
		auto it = font_i->_tb.find(tag);
		return it != font_i->_tb.end() ? &it->second : nullptr;
	}

	void Fonts::set_defontcss(tt_info* f1, tt_info* f2)
	{
		//LOCK_W(_lk);
		_decss.set_font2(f1, f2);
	}

	css_text* Fonts::create_css_text()
	{
		//LOCK_R(_lk);
		css_text* p = css_text::create(&_decss);
		return p;
	}

	bool Fonts::is_ttc(const char* data, std::vector<uint32_t>* outv)
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


	// 获取字形


	/*
	加粗bitmap，支持位图PX_MONO、PX_GRAY
	newbuf和不能和buffer一样
	*/


	//
	/*
	lcd =a
	a->b
	*/


	/* sYX is a matrix with the samples (row-major)
	xt and yt are the interpolation fractions in the two directions ranging from
	0.0 to 1.0 */


	/* A possible bicubic interpolation consists in performing a convolution
	with the following kernel :
	[ 0  2  0  0][s0]
	[-1  0  1  0][s1]
	p(t) = 0.5 [1 t t^2 t^3][ 2 -5  4 -1][s2]
	[-1  3 -3  1][s3]
	Where s0..s3 are the samples, and t is the range from 0.0 to 1.0 */


	//对应int32大小的成员 的转换 范例

	int32_t Fonts::swapInt32(int32_t value)
	{
		return ((value & 0x000000FF) << 24) |
			((value & 0x0000FF00) << 8) |
			((value & 0x00FF0000) >> 8) |
			((value & 0xFF000000) >> 24);
	}

	char* Fonts::tt_ushort(char* p)
	{
		std::swap(p[0], p[1]);
		return p + 2;
	}

	float Fonts::swapFloat32(float value)
	{
		intWithFloat i;
		i.m_i32 = value;
		i.m_f32 = swapInt32(i.m_i32);
		return i.m_f32;
	}

	void Fonts::blurCols(unsigned char* dst, int w, int h, int dstStride, int alpha)
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

	void Fonts::blurRows(unsigned char* dst, int w, int h, int dstStride, int alpha)
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

	void Fonts::Blur(unsigned char* dst, int w, int h, int dstStride, float blur, int n, int mode)
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

	float Fonts::bicubic(float s0, float s1, float s2, float s3, float t) {
		float r0 = 0.5f * (2.0f * s1);
		float r1 = 0.5f * (-s0 + s2);
		float r2 = 0.5f * (2.0 * s0 - 5.0f * s1 + 4.0f * s2 - s3);
		float r3 = 0.5f * (-s0 + 3.0f * s1 - 3.0f * s2 + s3);
		return r3 * t * t * t + r2 * t * t + r1 * t + r0;
	}

	float Fonts::bicubic2D(float s00, float s01, float s02, float s03, float s10, float s11, float s12, float s13, float s20, float s21, float s22, float s23, float s30, float s31, float s32, float s33, float xt, float yt) {
		// The bicubic convolution consists in passing the bicubic kernel in x
		// and then in y (or vice-versa, really)
		float r0 = bicubic(s00, s01, s02, s03, xt);
		float r1 = bicubic(s10, s11, s12, s13, xt);
		float r2 = bicubic(s20, s21, s22, s23, xt);
		float r3 = bicubic(s30, s31, s32, s33, xt);
		return bicubic(r0, r1, r2, r3, yt);
	}

	void Fonts::lcd2rgba(byte* b, byte* a, int W, int H)
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

	void Fonts::copy_bitmap(Bitmap* dst, Bitmap* src, int x, int y, std::vector<char>* out)
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

	void Fonts::init_bitmap_bitdepth(Bitmap* bitmap, int bit_depth)
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

	int Fonts::ft_bitmap_assure_buffer(Bitmap* bitmap, unsigned int xpixels, unsigned int ypixels, std::vector<char>* nbuf)
	{
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

	void Fonts::bitmap_embolden(Bitmap* bitmap, int xstr, int ystr, std::vector<char>* newbuf)
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

	void Fonts::copy_bitmap_mask(Bitmap* dst, Bitmap* src, glm::ivec2 dstpos, std::function<void(unsigned char* dp, unsigned char* sp)> func)
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

	void Fonts::get_blur(Bitmap* dst, Bitmap* src, float blur, int n, std::vector<char>* bitdata, int mode)
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

	glyph_shape* Fonts::get_glyph_shape(tt_info* font, const char* str)
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
		auto gs = font_dev::get_char_shape(font->_font, str, vc);
		glyph_shape* ret = nullptr;
		if (vc)
		{
			ret = new glyph_shape();
			ret->stbVertex = gs;
			ret->verCount = vc;
			ret->font = &font->_font->font;
			ret->v.assign(gs, gs + vc);
			ret->set();
		}
		return ret;
	}

	Glyph* Fonts::get_glyph_test0(tt_info* font, const char* codepoint, short isize, short iblur, int bitmapOption)
	{
		return get_glyph_test(font, font_dev::get_u8_to_u16(codepoint), isize, iblur, bitmapOption);
	}

	char* Fonts::get_glyph_test1(tt_info* font, unsigned int codepoint, float height, glm::ivec4* ot, std::vector<char>* out)
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
		g = font->get_glyph_index(codepoint, &renderFont, nullptr);
		double adv;
		return font_dev::get_glyph_bitmap(renderFont->_font, g, height, ot, out, &adv);
	}

	Glyph* Fonts::get_glyph_test(tt_info* font, unsigned int codepoint, short isize, short iblur, int bitmapOption)
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
		g = font->get_glyph_index(codepoint, &renderFont, nullptr);

		scale = font_dev::getPixelHeightScale(renderFont->_font, size);
		font_dev::buildGlyphBitmap(renderFont->_font, g, scale, &advance, &lsb, &x0, &y0, &x1, &y1);
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
		font_dev::renderGlyphBitmap(renderFont->_font, dst, gwh[0], gwh[1], params_width, scale, scale, g);
		{
			int w, h, i, j, c = 'a';
			auto bitmap = stbtt_GetCodepointBitmap(&renderFont->_font->font, 0, stbtt_ScaleForPixelHeight(&renderFont->_font->font, 20), c, &w, &h, 0, 0);
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

	void Fonts::print_test(const void* d, int w, int h)
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


	// TODO:push_cache_bitmap


	// 获取utf8字符

	const char* Fonts::get_u8_last(const char* str, unsigned int* codepoint)
	{
		unsigned int utf8state = 0;
		for (; str && *str; ) {
			if (fons_decutf8(&utf8state, codepoint, *(const unsigned char*)str++))
				continue;
			break;
		}
		return str;
	}

	const char* Fonts::get_u8_index(const char* str, int idx)
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

	Image* Fonts::push_cache_bitmap(Bitmap* bitmap, glm::ivec2* pos, int linegap, unsigned int col)
	{
		int width = bitmap->width + linegap, height = bitmap->rows + linegap;
		glm::ivec4 rc4 = { 0, 0, bitmap->width, bitmap->rows };
		auto ret = _packer.push_rect({ width, height }, pos);
		if (ret)
		{
			rc4.x = pos->x;
			rc4.y = pos->y;
			//ret->draw_rect(rc4, 0, col);
			if (bitmap->lcd_mode)
			{
				lcd_filter_fir(bitmap, 0, 0);
				lcd_filter_fir(bitmap, 0, 0);
			}
			ret->copy_to_image(bitmap->buffer, bitmap->pitch, rc4, col, bitmap->pixel_mode, bitmap->lcd_mode);
			ret->set_update();
		}
		return ret;
	}
	Image* Fonts::push_cache_bitmap(Bitmap* bitmap, glm::ivec2* pos, unsigned int col, Image* ret, int linegap)
	{
		int width = bitmap->width + linegap, height = bitmap->rows + linegap;
		glm::ivec4 rc4 = { 0, 0, bitmap->width, bitmap->rows };
		if (!ret)
			ret = _packer.push_rect({ width, height }, pos);
		if (ret)
		{
			rc4.x += pos->x;
			rc4.y += pos->y;
			//ret->draw_rect(rc4, 0, col);
			if (bitmap->lcd_mode)
			{
				lcd_filter_fir(bitmap, 0, 0);
				lcd_filter_fir(bitmap, 0, 0);
			}
			if (col)
				ret->copy_to_image(bitmap->buffer, bitmap->pitch, rc4, col, bitmap->pixel_mode, bitmap->lcd_mode);
		}
		return ret;
	}

	std::vector<Image*> Fonts::get_all_cacheimage()
	{
		std::vector<Image*>  ret;
		auto a = _packer.getall();
		ret.insert(ret.end(), a.begin(), a.end());
		return ret;
	}

	void Fonts::save_cache(std::string fdn)
	{
		int i = 0;
		_packer.maps([=, &i](Image* img)
			{
				std::string fn = fdn + std::to_string(i);
#ifdef __FILE__h__
		img->saveImage(fn + ".png");
#else
		auto d = img->png_data();
		save_binary_file(fn + ".png", d.data(), d.size());
#endif
			});
	}

	glm::vec3 Fonts::make_outline_y(css_text* ct, size_t n)
	{
		glm::ivec3 ret = { 0, 0, 0 };
		std::vector<glm::ivec3> os;
		css_text* mt = 0;
		for (size_t i = 0; i < n; i++)
		{
			auto it = &ct[i];
			bool ism = false;
			auto& ft = it->get_fonts();
			double lthis = 1.0;
			if (!it->ft)
			{
				it->ft = this;
			}
			for (auto font : ft)
			{
				if (font)
				{
					auto itr = font->get_rbl_height(it->get_fheight(), &ret, &ism);
					os.push_back(itr);
					lthis = std::max(lthis, floor(itr.y / 24.0));
					if (ism)
					{
						mt = it;
					}
				}
			}
			it->line_thickness = lthis;
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

	std::vector<ft_item*> Fonts::make_cache(tt_info* font, ft_key_s* key, double fns, std::vector<tt_info*>* fallbacks, int lcd_type)
	{
		std::vector<ft_item*> ret;
		ft_key_s k2;
		k2.u = key->u;
		k2.v.blur_size = 0;
		ret.resize(1);
		if (!font && fallbacks)
		{
			font = fallbacks->at(0);
		}
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
			int gidx = font->get_glyph_index(kt.unicode_codepoint, &rfont, fallbacks);
			if (rfont && gidx)
			{
				auto bit = rfont->get_glyph_image(gidx, fns, &rc, bitmap, &bitbuf[0], lcd_type, kt.unicode_codepoint);
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
					fit->renderfont = rfont;
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
					fit->renderfont = rfont;
				}
				if (ps.size())
				{
					std::vector<ft_item*> outit;
					font->push_cache(key, ps.data(), ps.size(), &outit);//, bit == 2
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
	int Fonts::get_kern_advance(tt_info* font, int g1, int g2)
	{
		return font ? font_dev::getGlyphKernAdvance(font->_font, g1, g2) : 0;
	}
	int Fonts::get_kern_advance_ch(tt_info* font, int ch1, int ch2)
	{
		return font ? font_dev::getKernAdvanceCH(font->_font, ch1, ch2) : 0;
	}
#define _NO_CACHE_

	// todo		渲染到image
	void Fonts::build_text(image_text_info* info, const std::string& str, size_t first_idx, size_t count)
	{
		tt_info* rfont = nullptr;
		double fns = info->font_size * info->font_dpi / 72.0;
		const char* t = str.c_str();// +first_idx;
		t = utf8_char_pos(t, first_idx, str.size());
		unsigned int unicode_codepoint = 0;
		char32_t ch = 0;
		double scale = info->font->get_scale_height(fns);
		int px = info->pos.x, py = info->pos.y, tpx = 0, tpy = 0;
		int img_height = info->dst->width;
		double line_height = info->font->get_line_height(fns);
		double base_line = info->font->get_base_line(fns);
		double adv = 0; int kern = 0;
#ifdef _NO_CACHE_
		// 增强亮度
		float brightness = 0.0;
		glm::ivec4 rc;
		Bitmap bitmap[1] = {};
		Bitmap blur[1] = {};
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
				kern = Fonts::get_kern_advance_ch(info->font, ch, unicode_codepoint);
			if (kern != 0)
			{
				auto kernf = scale * kern;
				kern = kernf;
			}
			ch = unicode_codepoint;
			glm::ivec4 fs;
#ifndef _NO_CACHE_
			fk.unicode_codepoint = unicode_codepoint;
			fk.font_size = info->font_size;
			fk.font_dpi = info->dpi;
			fk.blur_size = info->blur_size;
			ftits = ft->make_cache(info->font, fks, fns, nullptr, info->lcd_type);
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
				if (info->blur_size > 0 && ftits.size() > 1)
				{
					auto bft = ftits[1];
					glm::ivec2 dps = { kern + tpx + bft->_baseline.x + info->blur_pos.x - info->blur_size,
						py + dbl + bft->_baseline.y + info->blur_pos.y - info->blur_size
					};
					info->dst->draw_image2(bft->_image, bft->_rect, dps, info->color_blur);
				}
				glm::ivec2 dps = { kern + tpx + ftit->_baseline.x, py + dbl + ftit->_baseline.y };
				info->dst->draw_image2(ftit->_image, ftit->_rect, dps, info->color);
				adv = ftit->_advance;
			}
			tpx += adv;
#else
			int gidx = info->font->get_glyph_index(unicode_codepoint, &rfont, info->font_family);
			if (rfont)
			{
				auto bit = rfont->get_glyph_image(gidx, fns, &rc, bitmap, &bit_buf[0], info->lcd_type, unicode_codepoint);
				if (info->blur_size > 0)
				{
					Fonts::get_blur(blur, bitmap, info->blur_size, 1, &bit_buf[1]);
				}
				auto cpbit = blur;

				if (tpx + bitmap->advance > img_height)
				{
					tpx = px = 0;
					py += line_height;
				}

				if (bit)
				{
					double dbl = base_line;
					// 灰度图转RGBA
					if (info->blur_size > 0)
					{
						glm::ivec4 brc = { kern + tpx + rc.x + info->blur_pos.x - info->blur_size, py + dbl + rc.y + info->blur_pos.y - info->blur_size,
							cpbit->width, cpbit->rows };
						info->dst->copy_to_image(cpbit->buffer, cpbit->pitch, brc, info->color_blur, cpbit->pixel_mode, 0, true);
					}
					glm::ivec4 frc = { kern + tpx + rc.x, py + dbl + rc.y, bitmap->width, bitmap->rows };
					if (info->lcd_type)
					{
						lcd_filter_fir(bitmap, 0, 0);
						lcd_filter_fir(bitmap, 0, 0);
						info->dst->copy_lcd_to_image(bitmap->buffer, bitmap->pitch, frc, info->color, false);
					}
					else
					{
						info->dst->copy_to_image(bitmap->buffer, bitmap->pitch, frc, info->color, bitmap->pixel_mode, brightness, true);
					}
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
#undef _NO_CACHE_


	static void
		_print_codepoint(unsigned int codepoint) {
		if (codepoint <= 0xFFFF) {
			printf("U+%04X" "\n", codepoint);
		}
		else if (codepoint <= 0xFFFFF) {
			printf("U+%05X" "\n", codepoint);
		}
		else if (codepoint <= 0x10FFFF) {
			printf("U+%06X" "\n", codepoint);
		}
	}
	unsigned int u8tocodepoint(unsigned char* t, unsigned char** ot)
	{
		unsigned char byte;
		unsigned int  remaining = 0;
		unsigned int  codepoint = 0;
		size_t        offset = 0;
		while (*t)
		{
			byte = *t; t++;
			if (remaining == 0) {
				if (byte <= 0x7F) {
					_print_codepoint(byte);

					codepoint = 0;
					remaining = 0;
				}
				else if (((byte >> 5) & 0b111) == 0b110) {
					codepoint = byte & 0b11111;
					remaining = 1;
				}
				else if (((byte >> 4) & 0b1111) == 0b1110) {
					codepoint = byte & 0b1111;
					remaining = 2;
				}
				else if (((byte >> 3) & 0b11111) == 0b11110) {
					codepoint = byte & 0b111;
					remaining = 3;
				}
				else {
					fprintf(stderr, "ERROR at offset 0x%zX: invalid start byte: 0x%02X" "\n", offset, byte);
					codepoint = 0;
					remaining = 0;
				}
			}
			else {
				if (((byte >> 6) & 0b11) == 0b10) {
					codepoint = (codepoint << 6) | (byte & 0b111111);
					remaining--;

					if (remaining == 0) {
						_print_codepoint(codepoint);
					}
				}
				else {
					fprintf(stderr, "ERROR at offset 0x%zX: invalid continuation byte: 0x%02X" "\n", offset, byte);
					codepoint = 0;
					remaining = 0;
				}
			}

			offset++;
		}
		return codepoint;
	}
	// todo		生成到draw_font_info

	glm::ivec2 Fonts::build_info(css_text* csst, const std::string& str, size_t count, size_t first_idx, glm::ivec2 pos, draw_font_info* out)
	{
		glm::ivec2 ret;
		if (!csst || count < 1)
		{
			return ret;
		}
		tt_info* rfont = nullptr;
		const char* t = str.c_str();
		t = utf8_char_pos(t, first_idx, str.size());
		unsigned int unicode_codepoint = 0;
		char32_t ch = 0;
		int px = pos.x, py = pos.y, tpx = px, tpy = py, mx = 0;
		int img_height = INT_MAX;
		double line_height = 0;
#if 0
		auto font = csst->get_font();
		double fns = csst->get_fheight();
		double scale = font->get_scale_height(fns);
		double line_height = font->get_line_height(fns);
		double oline_height = line_height;
		int line_gap = 0;
		double xe = font->get_xmax_extent(fns, &line_gap);
		double base_line = font->get_base_line(fns);
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
#else
		std::unordered_map<tt_info*, css_text_info> cti;
		int ccs = 1;
		if (!csst->get_font())
		{
			ccs = 0;
		}
		if (ccs < 1)
		{
			return ret;
		}
		auto ct = csst;
		auto ctfs = ct->get_fonts();
		for (auto it : ctfs)
		{
			cti[it].set_css(ct, it);
			line_height = std::max(line_height, cti[it].line_height);
		}
#endif
		double adv = 0, fns = csst->get_fheight();
		ft_key_s fks[1] = {};
		auto& fk = fks->v;
		std::vector<ft_item*> ftits;
		int kern = 0;
		std::vector<draw_image_info> vitems, * vitem = &vitems;
		std::vector<draw_image_info> vblurss, * blurs = &vblurss;
		std::vector<glm::ivec2> vposi, * vpos = &vposi;
		if (out)
		{
			vitem = &out->vitem;
			vpos = &out->vpos;
			blurs = &out->blurs;
		}
		std::vector<draw_image_info>& vblurs = *blurs;
		vitem->reserve(std::min(count, str.size()));
		std::wstring twstr;
		css_text_info* ctip = nullptr;
		int spaces = csst->_fzpace;
		if (csst->_text_align == css_text::text_align::justify)
		{
			//spaces = ;
		}
		auto familys = csst->get_font_family();
		for (int i = 0; i < count && t && *t; i++)
		{
			const char* t1 = t, * t2;
			t = Fonts::get_u8_last(t, &unicode_codepoint);
			//auto u = u8tocodepoint((unsigned char*)t, 0);
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
			if (ch && rfont)
				kern = Fonts::get_kern_advance_ch(rfont, ch, unicode_codepoint);
			if (kern != 0 && ctip)
			{
				auto kernf = ctip->scale * kern;
				kern = kernf;
			}
			twstr.push_back(unicode_codepoint);
			ch = unicode_codepoint;
			fk.unicode_codepoint = unicode_codepoint;
			fk.font_size = csst->get_font_size();
			fk.font_dpi = csst->get_font_dpi();
			fk.blur_size = csst->get_blur_size();
			ftits = make_cache(nullptr, fks, fns, familys, csst->lcd_type);

			if (ch > 128)
			{
				ch = ch;
			}
			if (ftits.size() && ftits[0])
			{
				auto ftit = ftits[0];
				rfont = (tt_info*)ftit->renderfont;
				ctip = &cti[rfont];
				if (tpx + ftit->_advance > img_height)
				{
					mx = std::max(tpx, mx);
					tpx = px;
					py += line_height;
					tpy += line_height;
				}
				double dbl = ctip->base_line;
				// 缓存图画到目标图像
				adv = ftit->_advance + kern;
				if (fk.blur_size > 0 && ftits.size() > 1)
				{
					auto bft = ftits[1];
					if (bft)
					{
						glm::ivec2 dps = { tpx + bft->_baseline.x + csst->blur_pos.x - fk.blur_size,
							py + dbl + bft->_baseline.y + csst->blur_pos.y - fk.blur_size };
						draw_image_info dii;
						dii.user_image = bft->_image;
						dii.a = { dps.x + kern, dps.y, bft->_rect.z, bft->_rect.w };
						dii.rect = bft->_rect;
						dii.col = csst->color_blur;
						dii.unser_data = unicode_codepoint;
						dii.adv = adv;
						dii.unser_data1 = ctip;
						vblurs.push_back(dii);
					}
				}
				glm::ivec2 dps = { tpx + ftit->_baseline.x, py + dbl + ftit->_baseline.y };
				draw_image_info dii;
				dii.user_image = ftit->_image;
				dii.a = { dps.x + kern, dps.y, ftit->_rect.z, ftit->_rect.w };
				dii.rect = ftit->_rect;
				dii.col = csst->color;
				dii.unser_data = unicode_codepoint;
				dii.adv = adv;
				dii.unser_data1 = ctip;
				double iy0 = (double)dii.a.y + dii.a.w - pos.y;
				iy0 -= fns;
				ctip->maxbs = std::max(ctip->maxbs, iy0);
				vitem->push_back(dii);
			}
			else {
				auto spc = get_extent_cp(csst, L' ');
				auto spc1 = get_extent_cp(csst, L'\t');
				if (ch == U'\t')
				{
					adv = ((int64_t)spc.z * csst->_tabs);
				}
				if (ch == U' ')
				{
					adv = ((int64_t)spc.z);
				}
			}
			vpos->push_back({ adv, py });
			tpx += adv + spaces;
		}
		//njson posn, dif;
		mx = std::max(tpx, mx);
		ret.x = mx;
		ret.y = pos.y;
		if (out && ctip)
		{
			out->base_line = ctip->base_line;
			out->diffbs = ctip->bs;
			out->awidth = mx;
		}
		float miny = pos.y;
		double cury = 0;
		int i = 0;
		double incy = 0;
		for (auto& it : *vitem)
		{
			//posn.push_back(v4to(it.a));
			double iy = (double)it.a.y + it.a.w - pos.y;
			iy -= fns;
			//dif.push_back(iy);
			auto t = (uint64_t)it.unser_data;
			ctip = (css_text_info*)it.unser_data1;
			float maxh = pos.y + ctip->outline;
			double maxbs = ctip->maxbs - ctip->bs;
			if (maxbs > 0)
			{
				maxbs = maxbs;
			}
			it.a.y += ctip->bs;
			if (i < vblurs.size())
				vblurs[i].a.y += ctip->bs;
			if (t > 255)
			{
				auto cy = vpos->at(i);
				it.a.y = std::max(it.a.y, miny);
				double tcy = it.a.y + it.a.w - cy.y;
				if (tcy > maxh)
				{
					double diff = (double)tcy - maxh;
					it.a.y -= diff;
					if (i < vblurs.size())
					{
						vblurs[i].a.y -= diff;
						if (vblurs[i].a.y < 490)
						{
							i = i;
						}
					}
				}
			}
			i++;
		}
		return ret;
	}

	void Fonts::draw_image2(Image* dst, draw_image_info* info)
	{
		if (!dst || !info || !info->user_image)
		{
			return;
		}
		glm::vec2 texsize = { info->user_image->width, info->user_image->height };
		auto rect = info->rect;
		auto a = info->a;
		auto sliced = info->sliced;
		if (rect.z < 1)
		{
			rect.x = rect.y = 0;
			rect.z = a.z;
		}
		if (rect.w < 1)
		{
			rect.w = a.w;
		}
		glm::ivec2 dpos = { a.x, a.y };
		dst->draw_image2(info->user_image, rect, dpos, info->col);

	}
	glm::ivec2 Fonts::build_to_image(css_text* csst, const std::string& str, size_t count, size_t first_idx, glm::ivec2 pos, Image* dst)
	{
		draw_font_info tem;
		glm::ivec2 ret = build_info(csst, str, count, first_idx, pos, &tem);
		// 显示文本
		for (auto& it : tem.vitem)
		{
			draw_image2(dst, &it);
		}
		//tem.vitem[0].user_image->saveImage("fontchach.png");
		return ret;
	}

	css_text* Fonts::create_ct(uint32_t n)
	{
		css_text* ret = new css_text[n];
		return ret;
	}

	void Fonts::free_ct(css_text* cts)
	{
		if (cts)delete[]cts;
	}
	// 计算字体宽高
	void css_text::mk_fns()
	{
		fns = round((double)font_size * dpi / 72.0);
		_fzpace = round((double)_zpace * dpi / 72.0);
		_flineSpacing = round((double)_lineSpacing * dpi / 72.0);

		for (auto it : font_family)
		{
			underline_position = it->post.underlinePosition;
			underline_thickness = it->post.underlineThickness;
		}
	}

#if 1

	// 获取n个字符宽高


	// 获取单个unicode字符宽高


	// 获取单个u8字符宽高

	glm::ivec3 Fonts::get_extent_ch(css_text* csst, const char* str)
	{
		glm::ivec3 ret;
		tt_info* font = csst->get_font();
		unsigned int codepoint;
		auto t = Fonts::get_u8_last(str, &codepoint);
		if (font)
		{
			auto cs = font->get_char_extent(codepoint, csst->get_font_size(), csst->get_font_dpi(), csst->get_font_family());
			int c = t - str;
			ret = { cs.x, cs.y, c };
		}
		return ret;
	}

	glm::ivec3 Fonts::get_extent_cp(css_text* csst, unsigned int codepoint)
	{
		glm::ivec3 ret;
		tt_info* font = csst->get_font();
		if (font)
		{
			ret = font->get_char_extent(codepoint, csst->get_font_size(), csst->get_font_dpi(), csst->get_font_family());
		}
		return ret;
	}

	void Fonts::get_extent_str(css_text* csst, const char* str, size_t count, size_t first, text_extent* oet)
	{
		assert(oet);
		text_extent& ret = *oet;

		tt_info* font = csst->get_font();
		tt_info* rfont = nullptr;
		unsigned int codepoint;
		auto t = utf8_char_pos(str, first);
		size_t i = 0;
		int my = 0, cx = 0, w = 0;
		int rh = csst->get_line_height();
		ret._size.z = rh;
		glm::ivec2 brc;
		ret.mk_last_line(t);
		ret._ycpos.push_back({ 0, ret._ystr.size() });
		auto ycposit = ret._ycpos.rbegin();
		ret.pushy(rh, my, t, brc, rh);
		for (; *t && i < count; i++)
		{
			if (*t == '\n')
			{
				t++;
				brc.x = 0; ret.mk_last_line(t);
				ret._ycpos.push_back({ 0, ret._ystr.size() });
				ycposit = ret._ycpos.rbegin();
				ret.pushy(rh, my, t, brc, rh); my = 0;
				continue;
			}
			if (*t == '\t')
			{
				auto spc = get_extent_cp(csst, L' ');
				auto spc1 = get_extent_cp(csst, L'\t');
				w = (int64_t)spc.z * csst->_tabs + csst->_fzpace;
				cx += w;
				// todo rtl
				ret.pushx(w, false);
				t++;
				continue;
			}
			t = Fonts::get_u8_last(t, &codepoint);
			glm::ivec3 k = get_extent_cp(csst, codepoint);
			w = k.z + csst->_fzpace;
			// 计算自动换行
			if (cx > 0 && ((unsigned int)(cx + w) >= ret._view_width))
			{
				ycposit->x++;
				ret.mk_last_line(t);
				brc.x += ret.getlast_y();
				brc.y--;
				ret.pushy(rh, my, t, brc, rh);
				cx = my = 0;
			}
			cx += w;
			ret.pushx(w, false);
			my = std::max(my, k.y);
		}
		ret.mk_last_line(t);
		ret.pushy(-1, -1, t, brc, rh);
		ret._endstr = t;
		return;
	}

	ftg_item tt_info::mk_glyph(unsigned int glyph_index, unsigned int unicode_codepoint, int fns)
	{
		unsigned int col = -1;
		int lcd_type = 0;
		int linegap = 0;
		tt_info* rfont = this;
		Bitmap bitmap[1] = {};
		std::vector<char> bitbuf[1];
		glm::ivec4 rc;
		std::vector<ft_item>  ps;
		glm::ivec3 rets;
		ftg_item ret = {};
		if (-1 == glyph_index)
			glyph_index = get_glyph_index(unicode_codepoint, &rfont, 0);
		if (rfont && glyph_index)
		{
			auto rp = rfont->get_gcache(glyph_index, fns);
			do {
				if (rp)break;

				auto bit = rfont->get_glyph_image(glyph_index, fns, &rc, bitmap, 0, lcd_type, unicode_codepoint);
				if (colorinfo && bit)
				{
					glm::ivec4 bs = { rc.x, rc.y, bitmap->width, bitmap->rows };
					std::vector<uint32_t> ag;
					std::vector<uint32_t> cols;
					if (get_gcolor(glyph_index, ag, cols))
					{
						glm::ivec2 pos;
						Image* img = 0;
						img = ctx->push_cache_bitmap(bitmap, &pos, 0, img);
						for (size_t i = 0; i < ag.size(); i++)
						{
							bitbuf->clear();
							auto bit = rfont->get_glyph_image(ag[i], fns, &rc, bitmap, bitbuf, lcd_type, unicode_codepoint);
							if (bit)
							{
								auto ps1 = pos;
								ps1.x += bitmap->x - bs.x, ps1.y += bitmap->y + abs(bs.y);
								img = ctx->push_cache_bitmap(bitmap, &ps1, cols[i], img);
							}
						}
						glm::ivec4 rc4 = { pos.x, pos.y, bs.z,bs.w };
						if (img)
						{
							rp = rfont->push_gcache(glyph_index, fns, img, rc4, { bs.x,bs.y });
							if (rp)
							{
								rp->color = -1;
								rp->advance = bitmap->advance;
							}
						}
						break;
					}
				}
				bit = rfont->get_glyph_image(glyph_index, fns, &rc, bitmap, bitbuf, lcd_type, unicode_codepoint);
				if (bit)
				{
					glm::ivec2 pos;
					auto img = ctx->push_cache_bitmap(bitmap, &pos, linegap, col);
					glm::ivec4 rc4 = { pos.x, pos.y, bitmap->width, bitmap->rows };
					if (img)
					{
						rp = rfont->push_gcache(glyph_index, fns, img, rc4, { rc.x, rc.y });
						rp->color = 0;
						if (rp)rp->advance = bitmap->advance;
					}
				}
			} while (0);
			if (rp)
				ret = *rp;
		}
		return ret;
	}
	glm::ivec3 Fonts::mk_fontbitmap(tt_info* font, unsigned int glyph_index, unsigned int unicode_codepoint
		, int fns, glm::ivec2 pos, unsigned int col, Image* outimg)
	{
		int lcd_type = 0;
		int linegap = 0;
		tt_info* rfont = font;
		Bitmap bitmap[1] = {};
		std::vector<char> bitbuf[2];
		glm::ivec4 rc;
		std::vector<ft_item>  ps;
		glm::ivec3 rets;
		if (-1 == glyph_index)
			glyph_index = font->get_glyph_index(unicode_codepoint, &rfont, 0);
		if (rfont && glyph_index)
		{
			auto bit = rfont->get_glyph_image(glyph_index, fns, &rc, bitmap, &bitbuf[0], lcd_type, unicode_codepoint);
			if (bit && outimg)
			{
				int width = bitmap->width + linegap, height = bitmap->rows + linegap;
				glm::ivec4 rc4 = { 0, 0, bitmap->width, bitmap->rows };
				auto ret = outimg;
				if (ret)
				{
					rets.x = width;
					rets.y = height;
					rets.z = bitmap->advance;
					rc4.x = pos.x + rc.x;
					rc4.y = pos.y + rc.y;
					//ret->draw_rect(rc4, 0, col);
					ret->copy_to_image(bitmap->buffer, bitmap->pitch, rc4, col, bitmap->pixel_mode, bitmap->lcd_mode);
					//ret->set_update();
				}
			}
		}
		return rets;
	}
	// ！fonts
	void text_extent::clear()
	{
		_last = 0;
		_lasty = 0;
		_lastys = 0;
		_size = {};
		curry = 0;
		_line.clear();
		_y.clear();
		_y_width.clear();
		_ystr.clear();
		_ycpos.clear();
		_cposv.clear();
		_cposv.resize(2);
		_mav.clear();
	}
	void text_extent::pushx(int x, bool rtl) {
		mx += x;
		if (!_last || _last->weight != x || _last->rtl != rtl)
		{
			_lastys->push_back({ x, 0, rtl });
			_last = _lastys->data() + _lastys->size() - 1;
		}
		_last->n++;
		_y_width[curry] += x;
	}
	// todo 设置最后一行
	void text_extent::mk_last_line(const char* bt)
	{
		if (_ystr.size())
		{
			// 修改上一行的信息
			auto it = _ystr.rbegin();
			it->size = bt - it->s;
			it->n = get_utf8_count(it->s, it->size);
			it->str = u8_gbk(it->s, it->size);
		}

	}
	void text_extent::pushy(int y, int cy, const char* bt, const glm::ivec2& autobr, int height)
	{
		_size.x = std::max(mx, _size.x);
		_size.y += height;// _size.z;
		if (y < 0)return;
		_ystr.push_back({ 0, 0, autobr, bt });
		if (!_lasty || _lasty->weight != y) {
			_y.push_back({ y, 0 });
			_lasty = &_y[_y.size() - 1];
		}
		mx = 0;
		_line.push_back({});
		_lastys = &_line[_line.size() - 1];
		_last = 0;
		_lasty->n++;
		if (_y_width.size())
			curry++;
		_y_width.push_back(0);
	}
	size_t text_extent::get_line_count()
	{
		return _ycpos.size();
	}
	size_t text_extent::get_showline_count()
	{
		return _line.size();
	}
	/*
		struct ystrr_t {
			const char* s = 0;	// utf8字符串开头位置
			size_t n = 0;		// 字符数
			size_t size = 0;	// 字节数
		};
		std::vector<ystrr_t> _ystr;	所有行字符串信息
		get_substr函数获取两个2维坐标之间的字符串
	*/
	const char* text_extent::get_substr(glm::uvec2 first, glm::uvec2 second, size_t* rlen, size_t* rsize)
	{
		const char* ret = 0;
		const char* ret1 = 0;
		size_t outn = 0;
		size_t ys = _ystr.size();
		if (first > second)
			std::swap(first, second);
		if (first.y < ys)
		{
			auto& it = _ystr[first.y];
			ret = utf8_char_pos(it.s, first.x, it.size);
			outn = it.n - first.x;
		}
		if (second.y >= ys)
		{
			second.y = ys - 1;
		}
		if (second.y < ys)
		{
			auto& it = _ystr[second.y];
			if (second.x > it.n)
			{
				second.x = it.n;
			}
			ret1 = utf8_char_pos(it.s, second.x, it.size);
			if (first.y != second.y)
				outn += second.x;
			else
				outn -= it.n - second.x;
		}
		// 获取中间行字符数量
		size_t fy = first.y;
		size_t sey = second.y;
		for (size_t i = fy + 1; i < ys && i < sey; i++)
		{
			auto& it = _ystr[i];
			outn += it.n;
		}
		if (rlen)
			*rlen = outn;
		if (rsize && ret && ret1)
			*rsize = ret1 - ret;
		return ret;
	}
	size_t text_extent::get_substr(glm::uvec2 first, glm::uvec2 second, std::string& out, const char** pstr)
	{
		size_t rlen = 0, rsize = 0;
		auto str = get_substr(first, second, &rlen, &rsize);
		if (rsize > 0)
		{
			if (str)
				out.append(str, str + rsize);
			if (pstr)
				*pstr = str;
		}
		return rsize;
	}
	bool text_extent::empty() {
		return _y.empty();
	}
	void text_extent::set_view_width(int w, int row_span)
	{
		_view_width = w;
		_row_span = row_span;
	}
	void text_extent::set_cpos(glm::ivec2 cp, int idx)
	{
		if (idx >= _cposv.size())
		{
			_cposv.resize(idx + 1);
		}
		//if (cp.x == -1)cp.x = ;
		//if (cp.y == -1)cp.y = ;
		_cposv[idx] = cp;
		if (idx == 0)
			_cpos = _cposv[0];
	}

	// 添加光标
	void text_extent::set_cpos2(int idx, glm::ivec2 cp) {
		if (idx >= _cposv.size())
		{
			_cposv.resize(idx + 1);
		}
		_cposv[idx] = cpos2te(cp);
		if (idx == 0)
			_cpos = _cposv[0];
	}
	// 多光标
	glm::ivec2 text_extent::get_cpos2(int idx) {
		glm::ivec2 ret = { -1, -1 };
		do
		{
			if (idx < 0) { idx = 0; }
			if (idx >= _cposv.size())
			{
				idx >= _cposv.size() - 1;
			}
			if (idx < 0)break;
			ret = te2cpos2(_cposv[idx]);
		} while (0);
		return ret;
	}

	void text_extent::clear2() {
		_cposv.clear();
	}
	glm::ivec2 text_extent::cpos2te(glm::ivec2 cp)
	{
		cp -= 1;
		if (cp.x < 0)
		{
			cp.x = 0;
		}
		if (cp.y > _ycpos.size())
		{
			cp.y = _ycpos.size();
			cp.y--;
			//assert(0);
			//return { -1, -1 };
		}
		if (cp.y < 0)
		{
			cp.y = 0;
		}
		if (_ycpos.empty() || cp.y >= _ycpos.size())
		{
			return { 0, 0 };
		}
		auto ycp = _ycpos[cp.y];
		int xx = cp.x;
		int yy = ycp.y;
		for (int i = ycp.y; i < ycp.x; i++)
		{
			auto& yp = _ystr[i];
			if (xx > yp.n)
			{
				xx -= yp.n;
				yy++;
			}
			else {
				break;
			}
		}
		//printf("\t%d,%d转换%d,%d\n", cp.x, cp.y, xx, yy);
		return glm::ivec2{ xx, yy };
	}
	glm::ivec2 text_extent::get_cpos()
	{
		return _cposv[0];
	}

	glm::ivec2 text_extent::te2cpos2(const glm::ivec2& cp)
	{
		glm::ivec2 ret = { 1, 1 };
		if (_ystr.empty())
		{
			return ret;
		}
		unsigned int cy = cp.y;
		unsigned int cx = cp.x;
		//assert((cy < _ystr.size()));
		if (cy > _ystr.size())
		{
			cy = _ystr.size() - 1;
		}
		auto& yp = _ystr[cy];
		if (cx > yp.n)
		{
			cx = yp.n;
		}
		ret.y = cy + 1;
		ret.x = cx + 1;
		ret += yp.autobr;
		return ret;
	}
	bool text_extent::is_end(const glm::ivec2& cp)
	{
		bool ret = true;
		auto cp2 = cpos2te(cp);
		if (cp2.y >= _ystr.size() || _ystr.empty())return ret;
		auto yp = _ystr.rbegin();
		if (cp2.x < yp->n)
		{
			ret = false;
		}
		return ret;
	}
	void text_extent::set_cpos_inc_x(int x)
	{
		auto cp = get_cpos_inc_x(x);
		set_cpos(cp, 0);
		set_cpos(cp, 1);
	}
	void text_extent::set_cpos_inc_y(int y)
	{
		auto cp = get_cpos_inc_y(y);
		set_cpos(cp, 0);
		set_cpos(cp, 1);
	}

	glm::ivec2 text_extent::get_cpos_inc_x(int x, int idx)
	{
		auto ret = _cposv[idx];
		do {
			if (ret.y >= _ystr.size() || _ystr.empty())break;
			auto yp = _ystr[ret.y];
			if (ret.x > yp.n)
			{
				ret.x = yp.n;
			}
			int ki = ret.x + x;
			if (ki < 0)
			{
				ret.y--;
				if (ret.y < 0)
				{
					ret.y = 0;
				}
				else
				{
					int incx = 0;
					if (yp.autobr.y != 0)incx = -1;
					yp = _ystr[ret.y];
					ret.x = yp.n;
				}
				break;
			}
			else if (ki > yp.n)
			{
				ret.y++;
				ret.x = yp.n;
				if (ret.y >= _ystr.size())
				{
					ret.y = _ystr.size() - 1;
				}
				else {
					ret.x = 0;
				}
				break;
			}
			ret.x += x;
		} while (0);
		return ret;
	}
	glm::ivec2 text_extent::get_cpos_inc_y(int y, int idx)
	{
		auto ret = _cposv[idx];
		ret.y += y;
		if (ret.y < 0)
		{
			ret.y = 0;
		}
		if (ret.y >= _ystr.size())
		{
			ret.y = _ystr.size() - 1;
		}
		return ret;
	}
	void text_extent::set_cpos_home_end(bool ish)
	{
		if (ish)
		{
			_cpos.x = 0;
		}
		else
		{
			auto yp = _ystr[_cpos.y];
			_cpos.x = yp.n;
		}
		set_cpos(_cpos, 0);
		set_cpos(_cpos, 1);
	}
	glm::ivec4 text_extent::get_px_idx(int idx)
	{
		return get_cursor_idx(_cposv[idx]);
	}
	glm::ivec4 text_extent::get_cu_idx()
	{
		return get_cursor_idx(_cposv[0]);
	}
	//struct item_t
	//{
	//	int weight = 0;		// 宽/高度
	//	int n = 0;		// 数量
	//};
	// 获取坐标在一行/列哪个位置
	glm::ivec2 get_idx_rtl(std::vector<text_extent::item_t>& v, int pos, float inv, int* o)
	{
		int tc = 0, c = pos, px = 0, last = 0, ic = 0;
		bool rtl = false;
		bool st = false;
		auto nv = v.size();
		for (auto it : v)
		{
			int k = it.weight * it.n; last = it.weight;
			rtl = it.rtl;
			if (c <= k)
			{
				int n = c / it.weight;
				if (n < it.n)
				{
					int n1 = (c % it.weight);
					n1 = (n1 > it.weight * inv) ? 1 : 0;
					px += (n + n1) * it.weight;
					tc += n + n1;
					if (o)*o = it.weight;
				}
				st = true;
				break;
			}
			ic++;
			px += k;
			tc += it.n;
			c -= k;
		}
		if (o && *o < 1 && v.size())
			*o = v.rbegin()->weight;
		for (; rtl;)
		{
			int ac = 0;
			int dc = tc;
			int idx = 0;
			int nc = tc;
			for (size_t i = 0; i < nv; i++)
			{
				auto& it = v[i];
				ac += it.n;
				if (dc <= it.n && dc > 0)
				{
					idx = i;
					nc = it.n - dc;
					dc = 0;
				}
				dc -= it.n;
			}
			if (idx > ic - 1)
			{
				break;
			}
			int mtc = nc;	// 修正rtl光标坐标
			for (size_t i = ic; i < nv; i++)
			{
				auto& it = v[i];
				if (it.rtl)
				{
					mtc += it.n;
				}
				else {
					break;
				}
			}
#if 1
			// 本rtl块范围
			glm::ivec2 r = { nv, 0 };
			// 获取 ic前
			for (int i = ic - 1; i >= 0; i--)
			{
				auto& it = v[i];
				if (it.rtl)
				{
					if (i < r.x)
					{
						r.x = i;
					}
					if (i > r.y)
					{
						r.y = i;
					}
				}
				else {
					it.n;
				}
			}
			// 获取 ic后
			for (size_t i = ic; i < nv; i++)
			{
				auto& it = v[i];
				if (it.rtl)
				{
					if (i < r.x)
					{
						r.x = i;
					}
					if (i > r.y)
					{
						r.y = i;
					}
				}
			}
			// 获取rtl之前的字符数量
			for (size_t i = 0; i < r.x; i++)
			{
				auto& it = v[i];
				mtc += it.n;
			}
			tc = mtc;
#ifdef DEBUG
			printf("rtl: %d\t%d\n", r.x, r.y);
#endif // DEBUG
#endif
			break;
		}
		if (tc < 1)
		{
#ifdef DEBUG
			printf("X:\t%d\t%d\n", tc, px);
#endif // DEBUG
			tc = 0; px = 0;
		}
		return glm::ivec2{ tc, px };
	}
	glm::ivec2 text_extent::get_idx(std::vector<item_t>& v, int pos, float inv, int* o)
	{
		int tc = 0, c = pos, px = 0, last = 0, ic = 1;
		bool rtl = false;
		for (auto it : v)
		{
			int k = it.weight * it.n; last = it.weight;
			if (c <= k)
			{
				int n = c / it.weight;
				if (n < it.n)
				{
					int n1 = (c % it.weight);
					n1 = (n1 > it.weight * inv) ? 1 : 0;
					px += (n + n1) * it.weight;
					tc += n + n1;
					ic = 0;
					if (o)*o = it.weight;
				}
				break;
			}
			rtl = it.rtl;
			px += k;
			tc += it.n;
			c -= k;
		}
		if (o && *o < 1 && v.size())
			*o = v.rbegin()->weight;
		if (rtl)
		{
			rtl = rtl;
		}
		return glm::ivec2{ tc, px };
	}
	glm::ivec3 get_idx2(std::vector<text_extent::item_t>& v, int idx)
	{
		if (idx < 0)
		{
			idx = 0;
		}
		size_t n = idx;
		int tc = 0, c = n, px = 0, last = 0, ic = 1;
		if (v.size())
		{
			last = v[0].weight;
		}
		for (auto it : v)
		{
			last = it.weight;
			if (c <= it.n)
			{
				px += it.weight * c;
				tc += c;
				break;
			}
			px += it.weight * it.n;
			tc += it.n;
			c -= it.n;
		}
		return glm::ivec3{ tc, px, last };
	}
	template<class T1>
	bool it_cb(T1& it, glm::ivec4& ot, bool is)
	{
		ot.z = it.weight;
		if (ot.w <= it.n)
		{
			if (is)
				ot.y += it.weight * ot.w;
			ot.x += ot.w;
			ot.w = 0;
			return true;
		}
		if (is)
		{
			ot.y += it.weight * it.n;
		}
		ot.x += it.n;
		ot.w -= it.n;
		return false;
	}
	template<class T1>
	bool foridx(T1& v, int64_t first, int64_t second, glm::ivec4& ot)
	{
		bool ret = false;
		int64_t x = first;
		int64_t otw = ot.w;
		int64_t ac = 0;
		for (size_t i = second; i <= first; i++)
		{
			ac += v[i].n;
		}
		if (otw <= ac)
		{
			second--;
			for (; x != second; x--)
			{
				if (it_cb(v[x], ot, false))
				{
					ret = true;
					break;
				}
			}
			second++;
		}
		else {
			ot.w -= ac;
		}
		{
			if (x < 0)x = 0;
			auto dc = ac - otw;
			for (auto i = second; i <= x; i++)
			{
				auto& it = v[i];
				auto n = it.n;
				if (dc <= it.n && dc >= 0)
				{
					ot.y += it.weight * dc;
					break;
				}
				dc -= n;
				ot.y += it.weight * n;
			}
			ot.z = v[x].weight;
		}
		return ret;
	}
	template<class T>
	glm::ivec3 get_idx2av(T& v, int idx)
	{
		if (idx < 0)
		{
			idx = 0;
		}
		if (idx >= v.size())
		{
			idx = v.size() - 1;
		}
		glm::ivec3 ret;
		if (idx >= 0) { ret = v[idx]; }
#if 0


		glm::ivec4 ret;
		size_t n = v.size(), i = 0;
		int tc = 0, c = idx, px = 0, last = 0, ic = 1;
		if (v.size())
		{
			last = v[0].weight;
		}
		int icn = 1;
		size_t lx = 0;
		ret.w = c;
		for (size_t i = 0; i < n; i++)
		{
			auto& it = v[i];
			if (it.rtl)
			{
				size_t x = i + 1;
				for (; x < n && v[x].rtl; x++);
				lx = x;
				x--;
				if (x - i > 1)
				{
					if (foridx(v, x, i, ret))
						break;
					i = x;
				}
			}
			else if (it_cb(it, ret, true))
				break;
		}
#endif // 0
		return ret;
	}

	size_t text_extent::getlast_y()
	{
		size_t ret = 0;
		if (_ystr.size())
		{
			auto yp = _ystr.rbegin();
			ret = yp->n;
		}
		return ret;
	}
	// 获取坐标在文本的行列位置
	glm::ivec4 text_extent::get_pos_idx(glm::ivec2 pos)
	{
		return get_pos_idx(pos, 0, 0);
	}
	template<class T, class Ty>
	bool is_max2(T& vt, int px, double offset, Ty& ot)
	{
		bool ret = false;
		int d = vt.y - vt.x;
		if (px > vt.x)
		{
			ret = true;
			ot.y = vt.x;
			if (offset * d < px - vt.x)
			{
				ot.x++;
				ot.y = vt.y;
			}
		}
		return ret;
	}
	template<class T, class Ty>
	bool is_min2(T& vt, int px, double offset, Ty& ot)
	{
		bool ret = false;
		int d = vt.y - vt.x;
		if (px < vt.y)
		{
			ret = true;
			ot.y = (offset * d > px - vt.x) ? vt.x : vt.y;
		}
		return ret;
	}
	// 获取坐标在文本的行列位置return{字符位置xy，光标坐标zw}当前行高height
	glm::ivec4 text_extent::get_pos_idx(glm::ivec2 pos, bool* is_range_, glm::ivec2* cp)
	{
		glm::ivec4 ret = { -1, -1, -1, -1 };
		int height = 0;
		bool ir = true;
		glm::uvec2 py = get_idx(_y, pos.y, 0.0f, &height);	// 判断在哪行
		py.x--;
		if (py.x >= _line.size())
		{
			if (_line.empty())
				ret.z = ret.w = 0;
			return ret;
		}
		if (cp)
		{
			cp->y = py.x;
		}
		ret.y = py.x; ret.w = py.y - height;
		for (; ret.y >= 0;)
		{
			auto& vx = _line[ret.y];
			int lw = _y_width[ret.y];
			int w = 0;
			auto& av = get_av(ret.y);
			auto px = get_idx_rtl(vx, pos.x, _offset, &w);	// 判断在哪列
			glm::ivec2 pxo;
			if (av.empty())
			{
				ret.x = px.x; ret.z = px.y;
				if (cp)
				{
					cp->x = px.x;
				}
				break;
			}
			auto rit = (*av.rbegin());
			auto bit = (*av.begin());
			if (rit.z == 0 && pos.x >= rit.y)
			{
				pxo.x = av.size();
				pxo.y = rit.y;
			}
			else if (bit.z == 0 && pos.x < bit.y * _offset)
			{
				pxo.x = 0;
				pxo.y = 0;
			}
			else
			{
				// todo rtl未处理
				for (size_t i = 0; i < av.size(); i++)
				{
					auto it = av[i];
					if (is_range(it, pos.x))
					{
						pxo.x = i;
						if (it.z)
						{
							std::swap(it.x, it.y);
						}
						pxo.y = it.x;
						int d1 = abs(pos.x - it.x);
						int d1o = abs(_offset * (it.y - it.x));
						if (d1o < d1)
						{
							pxo.x++;
							pxo.y = it.y;
						}
						break;
					}
				}
			}
			px = pxo;
			if (lw < pos.x || pos.x < 0)
			{
				ir = false;
			}
			if (px.x < 0)
			{
				ir = false;
				px.x = 0;
			}
			if (px.y < 0)
			{
				ir = false;
				px.y = 0;
			}
			if (is_range_)
			{
				*is_range_ = ir;
			}
			ret.x = px.x; ret.z = px.y;
			if (cp)
			{
				cp->x = px.x;
			}
			break;
		}
		//if (o) *o = height;
		return ret;
	}
	// 返回光标坐标
	glm::ivec4 text_extent::get_cursor_idx(glm::ivec2 idx)
	{
		glm::ivec4 ret = { -1, -1, -1, -1 };
		int height = 0;
		bool ir = true;
		glm::uvec3 py = {};
		do
		{
			if (idx.y == -1)
			{
				idx.y = _y.size() - 1;
			}
			py = get_idx2(_y, idx.y);	// 判断在哪行

			if (py.x >= _line.size())
			{
				if (_line.empty())
					ret.z = ret.w = 0;
				return ret;
			}
		} while (0);
		//height = py.z;
		ret.y = py.x; ret.w = py.y - height;
		if (ret.y >= 0)
		{
			auto& vx = get_av(ret.y);
			auto& vx1 = _line[ret.y];
			int lw = _y_width[ret.y];
			int w = 0;
			if (idx.x == -1)
			{
				idx.x = _ystr[ret.y].n;
			}
			auto px12 = get_idx2(vx1, idx.x);
			auto px = get_idx2av(vx, idx.x);	// 判断在哪列

			ret.x = idx.x; ret.z = px.x;
			if (idx.x >= vx.size())
			{
				ret.z = px.y;
			}
		}
		//ret. = height;
		return ret;
	}
	std::vector<glm::ivec3>& text_extent::get_av(int y)
	{
		auto& av = _mav[y];
		do
		{
			if (av.size() == _ystr[y].n)
			{
				break;
			}
			auto& v = _line[y];
			int oty = 0;
			int f1 = -1;
			auto n = v.size();
			av.clear();
			if (n == 0)
			{
				break;
			}
			av.reserve(n);
			int lps = 0;
			for (size_t i = 0; i < n; i++)
			{
				auto& it = v[i];
				if (!it.rtl)
				{
					if (f1 > -1)
					{
						std::reverse(av.begin() + f1, av.end());
						f1 = -1;
					}
				}
				else {
					if (f1 < 0)
						f1 = av.size();
				}
				for (size_t k = 0; k < it.n; k++)
				{
					oty += it.weight;
					av.push_back({ lps, oty, it.rtl ? 1 : 0 });
					lps = oty;
				}
			}
			if (f1 > -1)
			{
				std::reverse(av.begin() + f1, av.end());
			}
		} while (0);
		return av;
	}
	// size.x=高度，size.y=坐标
	template<class Tv, class Ty>
	void vx_for(Tv& av, size_t x1, size_t x2, const glm::ivec2& size, Ty& ot)
	{
		size_t n = av.size();
		for (size_t i = x1; i < x2 && i < n; i++)
		{
			auto [x, z, rtl] = av[i];
			if (x > z)std::swap(x, z);
			ot.push_back({ x, size.y, z - x, size.x });
		}
		if (ot.size() > 3)
		{
			n = n;
		}
		return;

	}

	void text_extent::get_pos2_rects(glm::ivec2 first, glm::ivec2 second, t_vector<glm::ivec4>& outvd, int height)
	{
		if (first == second)
		{
			return;
		}
		if (first > second)std::swap(first, second);
		glm::ivec3 py = get_idx2(_y, first.y);		// 判断开始行
		glm::ivec3 py1 = get_idx2(_y, second.y);	// 判断结束行

		size_t y = first.y;
		for (size_t i = py.x; i <= py1.x; i++)
		{
			size_t x1 = i == py.x ? first.x : 0;
			size_t x2 = i == py1.x ? second.x : -1;
			auto& vx = get_av(i);
			glm::ivec3 ry = get_idx2(_y, y++);
			int h = height > 0 ? height : ry.z;
			vx_for(vx, x1, x2, { h, ry.y }, outvd);
		}
	}


#endif // 1

	// todo tt_info

	tt_info::tt_info()
	{
		bitinfo = new bitmap_ttinfo();
		_font = new font_impl_info();
		bitinfo->_t = this;
	}

	tt_info::~tt_info()
	{
		for (auto& [k, v] : _detail)
		{
			for (auto it : v)
			{
				delete it;
			}
		}
		delop(bitinfo);
		delop(_font);
		bitinfo = 0;
		_font = 0;
	}

	const char* tt_info::init(const char* d, size_t s, bool iscp)
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

	void tt_info::add_info(int platform, int encoding, int language, int nameid, const char* name, int length)
	{
		//info_one* e = dc.ac<info_one>(platform, encoding, language, nameid, name, length);
		info_one* e = new info_one(platform, encoding, language, nameid, name, length);
		_detail[language].push_back(e);
	}

	std::vector<tt_info::info_one*>* tt_info::get_info(int language)
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

	std::string tt_info::get_info_str(int language, int idx)
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
	size_t tt_info::get_info_strv(int language, int idx, std::vector<uint16_t>& buf)
	{
		//LOCK_R(_lock);
		size_t ret = 0;
		int ls[] = { language, 1033 };

		for (int i = 0; i < 2; i++)
		{
			auto it = _detail.find(ls[i]);
			if (it != _detail.end())
			{
				auto& p = it->second;
				for (size_t j = 0; j < p.size(); j++)
				{
					if (p[j]->name_id == idx)
					{
						auto pn = (p[j]->name_);
						ret = pn.size();
						buf.resize(ret);
						memcpy(buf.data(), pn.data(), ret * 2);
					}
				}
			}
		}
		return ret;
	}

	// 传空白的解码指针
	int tt_info::get_glyph_bitmap(int gidx, int height, glm::ivec4* ot, Bitmap* out_bitmap, std::vector<char>* out)
	{
		Bitmap* bitmap = 0;
		int x_pos = 0, y_pos = 0, ret = 0;
		int sidx = bitinfo->get_sidx(height);
		if (sidx < 0)
		{
			return 0;
		}
		int x = 10, y = 10;
		SBitDecoder* decoder = bitinfo->new_SBitDecoder();
		decoder->init(this, sidx);
		bitmap = decoder->bitmap;
		BigGlyphMetrics* metrics = decoder->metrics;

		if (get_index(decoder, gidx, x_pos, y_pos))
		{
			if (out) {
				out->resize((uint64_t)bitmap->rows * bitmap->pitch);
				memcpy(out->data(), bitmap->buffer, out->size());
			}
			if (ot) {
				ot->x = metrics->horiBearingX;
				ot->y = -metrics->horiBearingY;
				ot->z = bitmap->width;
				ot->w = bitmap->rows;
			}
			auto ha = metrics->horiAdvance;
			bitmap->advance = std::max(metrics->horiAdvance, metrics->vertAdvance);
			if (out_bitmap)
			{
				*out_bitmap = *bitmap;
				//out_bitmap->data = out;
				if (out)
					out_bitmap->buffer = (unsigned char*)out->data();
			}
			ret = 2;
			// 灰度图转RGBA
			//img.copy_to_image((unsigned char*)bitmap->buffer, bitmap->pitch,
			//	{ x + metrics->horiBearingX, y + metrics->horiAdvance - metrics->horiBearingY, bitmap->width, bitmap->rows },
			//	0xff0080ff, 1, 0);
		}
		else
		{
			ret = 0;
		}
		bitinfo->recycle(decoder);
		return ret;
	}

	int tt_info::get_custom_decoder_bitmap(uint32_t unicode_codepoint, int height, glm::ivec4* ot, Bitmap* out_bitmap, std::vector<char>* out)
	{
		auto& bch = bitinfo->_chsize;
		auto& bur = bitinfo->_unicode_rang;
		auto& img = bitinfo->_buf;
		int ret = 0;
		if (img)
		{
			// 910=12，11=14，12/13=16f
			glm::ivec2 uc, bc; size_t idx = 0;
			int inc = 2;
			if (height & 1)
			{
				height--;
				inc--;
			}
			for (size_t i = 0; i < bch.size(); i++)
			{
				auto& it = bch[i];
				if (it.x == height)
				{
					uc = bur[i];
					bc = it;
					break;
				}
				idx += it.x;
			}
			if (!uc.x || !uc.y || !unicode_codepoint)
			{
				return ret;
			}
			if (unicode_codepoint >= uc.x && unicode_codepoint <= uc.y)
			{
				int px = (unicode_codepoint - uc.x) * bc.y;
				int py = idx;
				out_bitmap->rows = bc.x;
				out_bitmap->width = bc.y;
				Fonts::init_bitmap_bitdepth(out_bitmap, 32);
				// RGBA
				out_bitmap->pitch = bc.y * 4;
				out_bitmap->advance = bc.y;
				if (ot) {
					ot->x = 0;
					ot->y = -bc.x + inc;
					ot->z = bc.y;
					ot->w = bc.x;
				}

				size_t size = out_bitmap->rows * (uint64_t)out_bitmap->pitch;
				if (out) {
					if (size > out->size())
					{
						out->resize(size);
					}
					out_bitmap->buffer = (unsigned char*)out->data();
					Image tem[1] = {};
					img->getBox({ px, py, bc.y, bc.x }, tem);
					auto td = tem->data();
					auto ti = tem->dsize();
					for (size_t i = 0; i < ti; i++)
					{
						if (*td == 0xff000000)
						{
							*td = 0;
						}
						td++;
					}
					//tem->saveImage("temadfd.png");
					memcpy(out_bitmap->buffer, tem->data(), size);
				}
				ret = 1;
			}
		}
		return ret;
	}

	void tt_info::print_info(int language)
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

	Glyph* tt_info::alloc_glyph()
	{
		Glyph* p = uac.new_mem<Glyph>(1);
		return p;
	}

	Glyph* tt_info::find_glyph(int codepoint)
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

	void tt_info::inser_glyph(Glyph* p)
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

	void tt_info::set_first_bitmap(bool is)
	{
		first_bitmap = is;
	}

	bool tt_info::is_script(const char* s)
	{
		return s && _slng.find(s) != std::string::npos;
	}

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

	ft_item* tt_info::push_cache1(ft_key_s* key, Image* img, const glm::ivec4& rect, const glm::ivec2& baseline, double advance, double blf)
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

	ft_item* tt_info::push_cache(ft_key_s* key, ft_item* ps, size_t n, std::vector<ft_item*>* out)
	{
		if (!key || !key->u || !key->v.font_size || !ps || !n)
		{
			return nullptr;
		}
		LOCK_W(_lock);
		//auto ret = fit_reserve(n);
		ft_key_s key2;
		key2.u = key->u;
		//key2.v.is_bitmap = bits;
		auto& itd = _ft_item_data;// (bits ? _ft_item_data1 : _ft_item_data);
		auto& ct = _cache_table;// (bits ? _cache_table1 : _cache_table);
		for (size_t i = 0; i < n; i++)
		{
			auto& it = ps[i];
			key2.v.unicode_codepoint = it._unicode_codepoint;
			key2.v.blur_size = it._blur_size;
			it.count = n;
			ft_item fm;
			fm.set_it(it);
			itd.emplace_back(fm);
			auto lt = --itd.end();
			ct[key2.u] = &(*lt);
			out->push_back(&(*lt));
		}
		return 0;
	}

	void tt_info::clear_cache()
	{
		LOCK_W(_lock);
		_ft_item_data.clear();
		_cache_table.clear();
		_ft_item_data1.clear();
		_cache_table1.clear();
	}

	// 查询返回指针

	ft_item* tt_info::find_item(ft_key_s* key)
	{
		ft_item* p = nullptr;
		if (key && key->u)
		{
			LOCK_R(_lock);
			ft_key_s key2;
			key2.u = key->u;
			//key2.v.blur_size = 0;
			std::unordered_map<uint64_t, ft_item*>* ct[2] = { &_cache_table, &_cache_table1 };
			do {
				//	key2.v.is_bitmap = bits;
				auto it1 = ct[0]->find(key2.u);
				if (it1 != ct[0]->end())
				{
					p = it1->second;
					break;
				}/*
				 if (bits && key2.v.unicode_codepoint > 128)
				 {
				 auto it = ct[1]->find(key2.u);
				 if (it != ct[1]->end())
				 {
				 p = it->second;
				 }
				 break;
				 }*/
			} while (0);
		}
		return p;
	}

	// todo 获取字符大小

	glm::ivec3 tt_info::get_char_extent(char32_t ch, unsigned char font_size, unsigned short font_dpi, std::vector<tt_info*>* fallbacks)
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
		auto g = get_glyph_index(ch, &rfont, fallbacks);
		if (g)
		{
			double fns = round((double)font_size * font_dpi / 72.0);
			double scale = rfont->get_scale_height(fns);
			int x0 = 0, y0 = 0, x1 = 0, y1 = 0, advance, lsb;
			font_dev::buildGlyphBitmap(rfont->_font, g, scale, &advance, &lsb, &x0, &y0, &x1, &y1);
			double adv = scale * advance;
			ret = { x1 - x0, y1 - y0, adv };
			LOCK_W(_lock);
			_char_lut[cs.u] = ret;
		}
		return ret;
	}

	void tt_info::clear_char_lut()
	{
		LOCK_W(_lock);
		_char_lut.clear();
	}

	const char* tt_info::get_glyph_index_u8(const char* u8str, int* oidx, tt_info** renderFont, std::vector<tt_info*>* fallbacks)
	{
		// Create a new glyph or rasterize bitmap data for a cached glyph.
		int g = 0;
		auto str = font_dev::get_glyph_index(_font, u8str, &g);
		// Try to find the glyph in fallback fonts.
		if (g < 1) {
			if (fallbacks)
			{
				for (auto it : *fallbacks)
				{
					str = font_dev::get_glyph_index(it->_font, u8str, &g);
					if (g > 0) {
						*renderFont = it;
						break;
					}
				}
			}
			// It is possible that we did not find a fallback glyph.
			// In that case the glyph index 'g' is 0, and we'll proceed below and cache empty glyph.
		}
		else
		{
			*renderFont = this;
		}
		*oidx = g;
		return str;
	}
	const char* tt_info::get_glyph_index_u8(const char* u8str, int* oidx, tt_info** renderFont, familys_t* fbs)
	{
		// Create a new glyph or rasterize bitmap data for a cached glyph.
		int g = 0;
		auto str = font_dev::get_glyph_index(_font, u8str, &g);
		// Try to find the glyph in fallback fonts.
		if (g < 1) {
			if (fbs)
			{
				for (size_t i = 0; i < fbs->count; i++)
				{
					str = font_dev::get_glyph_index(fbs->familys[i]->_font, u8str, &g);
					if (g > 0) {
						*renderFont = fbs->familys[i];
						break;
					}
				}
			}
			// It is possible that we did not find a fallback glyph.
			// In that case the glyph index 'g' is 0, and we'll proceed below and cache empty glyph.
		}
		else
		{
			*renderFont = this;
		}
		*oidx = g;
		return str;
	}

	union gcache_key
	{
		uint64_t u;
		struct {
			unsigned int glyph_index;
			uint16_t height;
		}v;
	};
	ftg_item* tt_info::push_gcache(unsigned int glyph_index, uint16_t height, Image* img, const glm::ivec4& rect, const glm::ivec2& pos)
	{
		gcache_key k = {}; k.v.glyph_index = glyph_index; k.v.height = height;
		auto& pt = _cache_glyphidx[k.u];
		if (!pt)
		{
			pt = _cdc.new_mem<ftg_item>(1);
		}
		pt->_glyph_index = glyph_index;
		pt->_image = img;
		pt->_dwpos = pos;
		pt->_rect = rect;
		return pt;
	}
	ftg_item* tt_info::get_gcache(unsigned int glyph_index, uint16_t height)
	{
		gcache_key k = {}; k.v.glyph_index = glyph_index; k.v.height = height;
		ftg_item* ret = 0;
		auto it = _cache_glyphidx.find(k.u);
		if (it != _cache_glyphidx.end())
		{
			ret = it->second;
		}
		return ret;
	}
	void tt_info::clear_gcache()
	{
		for (auto& [k, v] : _cache_glyphidx)
		{
			_cdc.free_mem(v, 1);
		}
		_cache_glyphidx.clear();
	}

	int tt_info::get_glyph_index(unsigned int codepoint, tt_info** renderFont, std::vector<tt_info*>* fallbacks)
	{
		// Create a new glyph or rasterize bitmap data for a cached glyph.
		int g = font_dev::getGlyphIndex(_font, codepoint);
		// Try to find the glyph in fallback fonts.
		if (g == 0) {
			if (fallbacks)
			{
				for (auto it : *fallbacks)
				{
					if (_font == it->_font)continue;
					int fallbackIndex = font_dev::getGlyphIndex(it->_font, codepoint);
					if (fallbackIndex != 0) {
						g = fallbackIndex;
						*renderFont = it;
						break;
					}
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

	void tt_info::init_post_table()
	{
		font_impl_info* font_i = _font;
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

	int tt_info::init_sbit()
	{
		font_impl_info* font_i = _font;
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
		njson ns = get_bitmap_size_table(b, count, bitinfo->_bsts, bitinfo->_index_sub_table, bitinfo->_msidx_table);
#if 0
		//ndef nnDEBUG
		printf("<%s>包含位图大小\n", _aname.c_str());
		for (auto& it : ns)
			printf("%s\n", it.dump().c_str());
#endif // !nnDEBUG
		// 位图数据表ebdt
		b = fc + ebdt_table->offset;
		glm::ivec2 version = { stb_font::ttUSHORT(b + 0), stb_font::ttUSHORT(b + 2) };
		bitinfo->_sbit_table = sbit_table;
		bitinfo->_ebdt_table = ebdt_table;
		bitinfo->_ebsc_table = ebsc_table;
		if (ns.size()) {
			first_bitmap = true;
		}
		return ns.size();
	}

	typedef struct  LayerIterator_
	{
		uint32_t   num_layers;
		uint32_t   layer;
		uint8_t* p;

	} LayerIterator;
	typedef struct  Palette_Data_ {
		uint16_t         num_palettes;
		const uint16_t* palette_name_ids;
		const uint16_t* palette_flags;

		uint16_t         num_palette_entries;
		const uint16_t* palette_entry_name_ids;

	} Palette_Data;

	union Color_2
	{
		uint32_t c;
		struct {
			uint8_t r, g, b, a;
		};
		struct {
			uint8_t red, green, blue, alpha;
		};
	};
	struct Cpal;
	struct Colr;
	struct gcolors_t
	{
		Cpal* cpal;
		Colr* colr;
		/* glyph colors */
		Palette_Data palette_data;         /* since 2.10 */
		uint16_t palette_index;
		Color_2* palette;
		Color_2 foreground_color;
		bool have_foreground_color;
	};
	struct GlyphSlot;
	int tt_face_load_colr(tt_info* face, uint8_t* b, sfnt_header* sp);

	void tt_face_free_colr(tt_info*);

	bool tt_face_get_colr_layer(tt_info* face,
		uint32_t            base_glyph,
		uint32_t* aglyph_index,
		uint32_t* acolor_index,
		LayerIterator* iterator);
	// 获取颜色
	Color_2 get_c2(tt_info* face1, uint32_t color_index);

	int tt_face_colr_blend_layer(tt_info* face,
		uint32_t       color_index,
		GlyphSlot* dstSlot,
		GlyphSlot* srcSlot);


	int tt_face_load_cpal(tt_info* face, uint8_t* b,
		sfnt_header* sp);

	void tt_face_free_cpal(tt_info* face);

	int tt_face_palette_set(tt_info* face,
		uint32_t  palette_index);
	// 初始化颜色
	int tt_info::init_color()
	{
		font_impl_info* font_i = _font;
		const stbtt_fontinfo* font = &font_i->font;
		int i, count, stringOffset;
		uint8_t* fc = font->data;
		uint32_t offset = font->fontstart, table_size = 0, sbit_num_strikes = 0;
		uint32_t ebdt_start = 0, ebdt_size = 0;
		sfnt_header* ebdt_table = 0;
		auto cpal_table = get_tag(font_i, TAG_CPAL);
		auto colr_table = get_tag(font_i, TAG_COLR);
		if (!cpal_table || !colr_table)
			return 0;
		if (!colorinfo)
			uac.new_mem(1, colorinfo);
		tt_info* ttp = (tt_info*)font->userdata;
		uint8_t* b = fc + cpal_table->offset;
		uint8_t* b1 = fc + colr_table->offset;
		tt_face_load_cpal(this, b, cpal_table);
		tt_face_load_colr(this, b1, colr_table);

		return 0;
	}

	int tt_info::get_gcolor(uint32_t base_glyph, std::vector<uint32_t>& ag, std::vector<uint32_t>& col)
	{
		uint32_t aglyph_index = base_glyph;
		uint32_t acolor_index = 0;
		LayerIterator it = {};
		for (;;)
		{
			if (!tt_face_get_colr_layer(this, aglyph_index, &aglyph_index, &acolor_index, &it))
			{
				break;
			}
			ag.push_back(aglyph_index);
			col.push_back(get_c2(this, acolor_index).c);
		}
		return it.num_layers;
	}

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

	int tt_info::get_glyph_image(int gidx, double height, glm::ivec4* ot, Bitmap* bitmap, std::vector<char>* out, int lcd_type, uint32_t unicode_codepoint)
	{
		int ret = 0;
		if (gidx > 0)
		{
			double scale = get_scale_height(height);
			if (height < 0)
			{
				height *= -1;
			}
#ifndef _FONT_NO_BITMAP
			if (first_bitmap)
			{
				// 解析位图
				ret = get_glyph_bitmap(gidx, height, ot, bitmap, out);
				// 找不到位图时尝试用自定义解码
				if (!ret)
					ret = get_custom_decoder_bitmap(unicode_codepoint, height, ot, bitmap, out);
			}
#endif
			if (!ret)
			{
				// 解析轮廓并光栅化
				double advance = height;
				glm::vec2 lcds[] = { {1, 1}, {3, 1}, {1, 3}, {4, 1, } };
				font_dev::get_glyph_bitmap(_font, gidx, scale, ot, out, &advance, lcds[lcd_type]);
				if (bitmap)
				{
					auto hh = hhea;
					auto he = hhea.ascender + hhea.descender + hhea.lineGap;
					auto hef = hhea.ascender - hhea.descender + hhea.lineGap;

					double hed = ceil(scale * he);
					double hedf = ceil(scale * hef);
					double lg = ceil(scale * hhea.lineGap);
					if (out)
						bitmap->buffer = (unsigned char*)out->data();
					bitmap->width = bitmap->pitch = ot->z;
					bitmap->rows = ot->w;
					bitmap->advance = advance;
					bitmap->pixel_mode = PX_GRAY;	//255灰度图
					bitmap->lcd_mode = lcd_type;
					Fonts::init_bitmap_bitdepth(bitmap, 8);
					ret = 1;
				}
			}
			if (bitmap)
			{
				bitmap->x = ot->x;
				bitmap->y = ot->y;
			}
		}
		return ret;
	}

	/*
	获取量
	*/

	double tt_info::get_scale_height(int height)
	{
		return font_dev::getPixelHeightScale(_font, height);
		//{
		//	//LOCK_W(_lock);
		//	scale = _scale_lut[height];
		//	if (!(scale > 0))
		//	{
		//		scale =
		//		//glm::ivec4 glrc[3];
		//		//font_dev::getPixelGLScale(&font, glrc);
		//		//auto adl = hhea.ascender + abs(hhea.descender);
		//		//double ng = abs(glrc[0].y) + abs(glrc[1].w);
		//		//double s1 = height - 1;
		//		//auto n = s1 / ng;
		//		_scale_lut[height] = scale;// = n;
		//	}
		//}
		//return scale;
	}
	int tt_info::get_line_height(double height, bool islinegap)
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
#if 1

	glm::ivec3 tt_info::get_rbl_height(double height, glm::ivec3* out, bool* is)
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
		double ba = floor(f * scale);
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
					*is = true;
			}
		}
		return ret;
#endif
	}
#endif // 1

	double tt_info::get_base_line(double height)
	{
		float scale = get_scale_height(height);
		double f = hhea.ascender;
		//return ceil(f * scale);
		return floor(f * scale); // 向下取整
	}
	int tt_info::get_xmax_extent(double height, int* line_gap)
	{
		float scale = get_scale_height(height);
		double f = hhea.xMaxExtent, lig = hhea.lineGap;
		if (line_gap)
		{
			*line_gap = ceil(lig * scale);
		}
		return ceil(f * scale);
	}
	// 获取字体最大box
	glm::ivec4 tt_info::get_bounding_box(double scale, bool is_align0)
	{
		auto ret = font_dev::get_bounding_box(_font);
		if (is_align0)
		{
			if (ret.x != 0)
			{
				ret.z -= ret.x; ret.x = 0;
			}
			if (ret.y != 0)
			{
				ret.w -= ret.y; ret.y = 0;
			}
		}
		glm::vec4 s = ret;
		s *= scale;
		return ceil(s);
		//return round(s);
	}
	void tt_info::get_VMetrics(int* ascent, int* descent, int* lineGap)
	{
		font_dev::getFontVMetrics(_font, ascent, descent, lineGap);
	}
	void tt_info::get_HMetrics(int codepoint, int* advance, int* lsb)
	{
		int glyph = font_dev::getGlyphIndex(_font, codepoint);
		font_dev::getFontHMetrics(_font, glyph, advance, lsb);
		return;
	}
	void tt_info::get_HMetrics_idx(int glyphindex, int* advance, int* lsb)
	{
		font_dev::getFontHMetrics(_font, glyphindex, advance, lsb);
		return;
	}
	tt_info::info_one::info_one(int platform, int encoding, int language, int nameid, const char* name, int len)
	{
		platform_id = platform;
		encoding_id = encoding;
		language_id = language;
		name_id = nameid;
		length_ = len;
		std::wstring w;
		size_t wlen = length_ / 2;
		char* temp = (char*)name;
		if (*temp)
		{
			name_a.assign(temp, length_);
		}
		ndata.assign(temp, length_);
		auto ss = length_ / 2;

		name_.assign((wchar_t*)name, length_ / 2);
	}
	std::string tt_info::info_one::get_name(bool isu8)
	{
		std::string n, gbkstr, u8str, wstr;
		if (encoding_id)
		{
			/*#ifdef __json_helper_h__
			#ifdef _STD_STR_
						n = isu8 ? w_u8(to_wstr(name_, true)) : w_gbk(to_wstr(name_, true));
			#else
						//n = isu8 ? w_u8(name_.to_wstr(true)) : w_gbk(name_.to_wstr(true));
						n = isu8 ? u16_u8((uint16_t*)name_.c_str(),name_.size()) : u16_gbk((uint16_t*)name_.c_str(), name_.size());
			#endif
			#else
			#ifdef _STD_STR_
						n = isu8 ? w_u8(to_wstr(name_, true)) : w_gbk(to_wstr(name_, true));
			#else
						n = isu8 ? w_u8(name_.to_wstr(true)) : w_gbk(name_.to_wstr(true));
			#endif
			#endif // __json_helper_h__*/
			//gbkstr = u16_gbk((uint16_t*)name_.c_str(), name_.size());
			//u8str = u16_u8((uint16_t*)name_.c_str(), name_.size());
			//auto sw = (to_wstr(name_.data(), name_.size(), true));
			auto sws = ndata.size() * 0.5;
			std::string nd(ndata.c_str(), ndata.size());
			char* temp = (char*)nd.data();
			for (int i = 0; i < sws; ++i)
			{
				temp = Fonts::tt_ushort(temp);
			}
			n = isu8 ? icu_16u8((uint16_t*)nd.c_str(), sws) : u16_gbk((uint16_t*)nd.c_str(), sws);

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
					n = gbk_u8(n);
				}
				//else if (encoding_id == 1)
				//{
				//	char* tc = (char*)ndata.data();
				//	wchar_t* tw = (wchar_t*)ndata.data();
				//	char buf[128] = {}, b[10] = {};
				//	wchar_t bufw[128] = {};
				//	memcpy(buf, tc, ndata.size() < 128 ? ndata.size() : 128);
				//	memcpy(bufw, tw, ndata.size() < 128 ? ndata.size() : 128);
				//	n = u16_u8((uint16_t*)name_.c_str(), name_.size());
				//}
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
	void tt_info::info_one::print()
	{
		std::string n = get_name(false);
		printf("name：%s\tname_id：%d\tplatform_id：%d\tlanguage_id：%d\tencoding_id：%d\n", n.c_str(), name_id, platform_id, language_id, encoding_id);
	}
	//std::wstring tt_info::info_one::to_wstr(std::wstring str, bool is_swap)
	//{
	//	std::wstring ret = str;
	//	if (is_swap)
	//	{
	//		char* temp = (char*)ret.data();
	//		for (int i = 0; i < ret.size(); ++i)
	//		{
	//			temp = Fonts::tt_ushort(temp);
	//		}
	//	}
	//	return ret;
	//}
	std::wstring to_wstr(const wchar_t* str, int len, bool is_swap)
	{
		std::wstring ret;
		ret.reserve(len + 1);
		for (int i = 0; i < len; i++, str++)
			ret.push_back(*str);
		if (is_swap)
		{
			char* temp = (char*)ret.data();
			for (int i = 0; i < ret.size(); ++i)
			{
				temp = Fonts::tt_ushort(temp);
			}
		}
		return ret;
	}
	std::wstring to_wstr(uint16_t* str, int len, bool is_swap)
	{
		std::wstring ret;
		ret.reserve(len + 1);
		for (int i = 0; i < len; i++, str++)
			ret.push_back(*str);
		if (is_swap)
		{
			char* temp = (char*)ret.data();
			for (int i = 0; i < ret.size(); ++i)
			{
				temp = Fonts::tt_ushort(temp);
			}
		}
		return ret;
	}
	void glyph_shape::convert(stbtt_vertex* stbVertex, int verCount)
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
				// 二次曲线
				//mTess->insertVertex(current);
				glm::vec2 contrl;
				contrl.x = stbVer->cx;
				contrl.y = stbVer->cy;

				previous = current;

				current.x = stbVer->x;
				current.y = stbVer->y;

				fmt.push_back('Q');
				_path.push_back(contrl);
				_path.push_back(current);
				//insert the middle point
				//mTess->insertVertex(_getBezierPoint(previous, contrl, current, 0.5));
			}
			break;
			case STBTT_vcubic:
			{
				// 三次曲线
				glm::vec2 contrl, contrl1;
				contrl.x = stbVer->cx;
				contrl.y = stbVer->cy;
				contrl1.x = stbVer->cx1;
				contrl1.y = stbVer->cy1;

				previous = current;

				current.x = stbVer->x;
				current.y = stbVer->y;

				fmt.push_back('C');
				_path.push_back(contrl);
				_path.push_back(contrl1);
				_path.push_back(current);

			}break;
			default:break;
			}
		}
		fmt.push_back('z');
	}

	//invert 1倒置, 0正常


	/*
	todo			路径填充光栅化-function
	输入路径path，缩放比例scale、bpm_size{宽高xy,每行步长z}，
	xy_off偏移默认0
	输出灰度图
	*/

	//void get_path_bitmap(stbtt_vertex* vertices, int num_verts, std::vector<char>* bmp, glm::ivec3 bpm_size, glm::vec2 scale, glm::vec2 xy_off)
	//{
	//	get_glyph_bitmap_subpixel(vertices, num_verts, scale, { 0, 0 }, xy_off, bmp, bpm_size);
	//}
	// 支持移动、直线、二次贝塞尔曲线，构成的路径。		（三次曲线未测试过）
	unsigned char* get_glyph_bitmap_subpixel(stbtt_vertex* vertices, int num_verts, glm::vec2 scale, glm::vec2 shift, glm::vec2 xy_off
		, std::vector<unsigned char>* out, glm::ivec3 bpm_size, int invert)
	{
		stbtt__bitmap gbm;
		if ((int)scale.x == 0 || (int)scale.y == 0)
		{
			scale.x = scale.y = std::max(std::max(scale.x, scale.y), 1.0f);
		}
		// now we get the size
		gbm.w = bpm_size.x;
		gbm.h = bpm_size.y;
		gbm.pixels = NULL; // in case we error
		int bms = gbm.w * gbm.h;
		if (bpm_size.z < 1) bpm_size.z = gbm.w;
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

	void get_path_bitmap(stbtt_vertex* vertices, int num_verts, image_gray* bmp, glm::vec2 scale, glm::vec2 xy_off)
	{
		get_glyph_bitmap_subpixel(vertices, num_verts, scale, { 0, 0 }, xy_off, &bmp->_data, { bmp->width, bmp->height, 0 });
	}
	void build_blur(image_gray* grayblur, float blur, unsigned int fill, int blurcount, Image* dst, glm::ivec2 pos, bool iscp)
	{

		std::vector<unsigned char> rbd;
		unsigned char* bdata = grayblur->data();
		glm::ivec2 rbs = { grayblur->width, grayblur->height };
		if (iscp)
		{
			rbd.resize((int64_t)rbs.x * rbs.y);
			auto cpd = rbd.data();
			memcpy(cpd, bdata, rbd.size());
			bdata = cpd;
			grayblur->ud = cpd;
		}
		// 模糊
		Fonts::Blur(bdata, rbs.x, rbs.y, rbs.x, blur, blurcount);
		// 转成rgba
		if (dst)
		{
			glm::ivec4 src4 = { 0, 0, rbs.x, rbs.y };
			dst->copy2(grayblur, pos, src4, fill);
			//dst->copy_to_image(bdata, rbs.x, src4, fill, 2);
		}
	}


	//!
	unsigned char fdata[2180] = {
		0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
		0x00, 0x00, 0x02, 0xF0, 0x00, 0x00, 0x00, 0x2A, 0x01, 0x03, 0x00, 0x00, 0x00, 0xEF, 0x52, 0xFD,
		0x59, 0x00, 0x00, 0x00, 0x03, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0xDB, 0xE1, 0x4F, 0xE0,
		0x00, 0x00, 0x00, 0x06, 0x50, 0x4C, 0x54, 0x45, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x55, 0xC2,
		0xD3, 0x7E, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0E, 0x9C, 0x00, 0x00,
		0x0E, 0x9C, 0x01, 0x07, 0x94, 0x53, 0xDD, 0x00, 0x00, 0x00, 0x1C, 0x74, 0x45, 0x58, 0x74, 0x53,
		0x6F, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x41, 0x64, 0x6F, 0x62, 0x65, 0x20, 0x46, 0x69,
		0x72, 0x65, 0x77, 0x6F, 0x72, 0x6B, 0x73, 0x20, 0x43, 0x53, 0x36, 0xE8, 0xBC, 0xB2, 0x8C, 0x00,
		0x00, 0x00, 0x16, 0x74, 0x45, 0x58, 0x74, 0x43, 0x72, 0x65, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x20,
		0x54, 0x69, 0x6D, 0x65, 0x00, 0x31, 0x32, 0x2F, 0x31, 0x30, 0x2F, 0x32, 0x30, 0x4A, 0xBE, 0xBD,
		0xE5, 0x00, 0x00, 0x07, 0xCB, 0x49, 0x44, 0x41, 0x54, 0x48, 0x89, 0xAD, 0xD6, 0x5F, 0x6C, 0x14,
		0xC7, 0x19, 0x00, 0x70, 0x13, 0xA3, 0x1C, 0x8A, 0x48, 0x5C, 0x13, 0xA9, 0xBA, 0x07, 0x83, 0xA3,
		0x82, 0x1A, 0xF1, 0xD0, 0x2A, 0x4D, 0x54, 0xC5, 0xFC, 0x8B, 0x41, 0xED, 0x0B, 0x2A, 0x6D, 0xF2,
		0x80, 0xED, 0x2A, 0x04, 0xDC, 0x88, 0x4A, 0xA9, 0x82, 0x8C, 0x4D, 0x08, 0x77, 0x29, 0x9B, 0xE3,
		0x72, 0x3D, 0xA9, 0x26, 0xAA, 0xCB, 0x29, 0x42, 0x15, 0x95, 0x90, 0xB9, 0x07, 0x94, 0xA2, 0xE0,
		0x82, 0x43, 0x2D, 0xDF, 0x62, 0x6F, 0xC6, 0x1B, 0xB3, 0x84, 0x93, 0xD5, 0x86, 0x93, 0x52, 0xA1,
		0x85, 0x2E, 0x7B, 0x13, 0x74, 0x45, 0x2B, 0xD5, 0xEC, 0x7D, 0xB6, 0x4E, 0xEE, 0x60, 0x8F, 0xF7,
		0xA6, 0xDF, 0xEC, 0xDE, 0x81, 0x6D, 0x6C, 0xE7, 0x8F, 0x3A, 0x2F, 0x77, 0x3B, 0xBB, 0xFB, 0x9B,
		0x6F, 0xBF, 0xF9, 0x66, 0x76, 0x6B, 0x44, 0xBB, 0x10, 0xDC, 0xD3, 0xC5, 0xD7, 0x68, 0xB3, 0xF1,
		0x54, 0x1C, 0x16, 0xE9, 0x2F, 0x2B, 0x67, 0x5E, 0xA9, 0x59, 0xA2, 0x89, 0x76, 0x26, 0x38, 0xA7,
		0xF2, 0xB2, 0xAF, 0x1A, 0xC3, 0x83, 0xA2, 0xCE, 0x66, 0xC5, 0xB4, 0x48, 0xE7, 0xE8, 0x40, 0x76,
		0x0E, 0xCF, 0xED, 0x8F, 0x97, 0xE2, 0xF3, 0xC3, 0x05, 0x35, 0x1A, 0xF0, 0xFB, 0xF3, 0x4D, 0x3C,
		0xD4, 0xB4, 0x3F, 0x25, 0xBC, 0xF2, 0xFD, 0xC9, 0xDB, 0x4D, 0x3F, 0x59, 0xD9, 0xB0, 0xEB, 0xF1,
		0x15, 0x8F, 0x3F, 0x73, 0xDC, 0xE8, 0x69, 0xDA, 0xF1, 0xDC, 0x67, 0x3B, 0x37, 0xAC, 0x05, 0x8C,
		0xBD, 0x2C, 0x3C, 0xD1, 0x09, 0x54, 0x63, 0x73, 0x79, 0x3A, 0xB1, 0x14, 0x4F, 0x67, 0x6E, 0xB4,
		0xEA, 0x0C, 0x8E, 0xE1, 0x65, 0x7B, 0x7B, 0xCD, 0xC3, 0xC7, 0xE8, 0xDE, 0xA8, 0xE0, 0x22, 0x7A,
		0x30, 0x6F, 0x76, 0x64, 0xAC, 0x0E, 0xAA, 0x75, 0x7E, 0xDF, 0xEC, 0x88, 0x98, 0x2D, 0x99, 0x1B,
		0x07, 0x2E, 0x10, 0x56, 0xE1, 0x31, 0x45, 0x3C, 0xE0, 0xD9, 0x57, 0xF2, 0x13, 0x6D, 0xBF, 0xD0,
		0x99, 0xBB, 0x0F, 0xAF, 0xB2, 0x6D, 0x7A, 0x38, 0x56, 0xB6, 0xA3, 0x71, 0x56, 0xAB, 0xBE, 0x7A,
		0xDD, 0x74, 0x2F, 0xAA, 0x2E, 0x1D, 0x86, 0x1F, 0xB6, 0x21, 0xCF, 0x90, 0xEF, 0xFF, 0x56, 0xFC,
		0x8C, 0xD3, 0xFA, 0x32, 0x2B, 0x36, 0x35, 0xD7, 0x96, 0x6D, 0xFA, 0xC9, 0xF8, 0x93, 0x2F, 0x15,
		0xDE, 0xF0, 0x40, 0xE4, 0xBF, 0xB8, 0xF8, 0xA2, 0x7B, 0xE2, 0xC5, 0x3B, 0x74, 0x08, 0xFE, 0xD2,
		0xB6, 0x3F, 0xB2, 0x89, 0x65, 0x06, 0xA3, 0xFD, 0x31, 0xC9, 0x5F, 0xDF, 0x7A, 0xE8, 0x7A, 0xC3,
		0xFA, 0xF4, 0xE6, 0xE8, 0xEA, 0xAD, 0xC6, 0xD5, 0x3F, 0xBF, 0xBE, 0xE1, 0x4F, 0x3B, 0x9F, 0x6F,
		0xE0, 0xB0, 0x0C, 0x5F, 0xFA, 0xA8, 0x9D, 0x15, 0x5F, 0xC2, 0x99, 0xB3, 0x69, 0x7C, 0xCF, 0xDB,
		0xB6, 0x7D, 0x4A, 0x40, 0xAD, 0xFD, 0xC5, 0x45, 0x8C, 0x3E, 0xF3, 0x03, 0x8C, 0xBE, 0x55, 0xED,
		0x8C, 0x8C, 0xB0, 0xCC, 0x30, 0xA3, 0x31, 0x8E, 0x3C, 0x66, 0x4C, 0x55, 0x3A, 0xE8, 0x30, 0x6B,
		0xB5, 0x3A, 0x32, 0x0A, 0x0E, 0x7B, 0x40, 0x1D, 0x59, 0x26, 0x7A, 0x31, 0x71, 0x64, 0xAC, 0xDD,
		0x4F, 0x8E, 0x67, 0xE7, 0xC5, 0x8E, 0x29, 0xDB, 0x66, 0xF8, 0xC8, 0xE4, 0xD6, 0xEF, 0x4D, 0x77,
		0x84, 0xC9, 0xDC, 0xB7, 0x9E, 0xEA, 0xFC, 0x2E, 0x8D, 0xAA, 0x9B, 0xC7, 0xF3, 0x9E, 0xE4, 0xAF,
		0x59, 0x6E, 0xF7, 0xBB, 0x2E, 0x1D, 0xF2, 0xF9, 0x59, 0x96, 0xB9, 0xCC, 0xD4, 0x63, 0xCB, 0xF1,
		0x33, 0x37, 0x3E, 0xEA, 0xE7, 0x54, 0xA6, 0x13, 0x65, 0xE5, 0x88, 0x6D, 0x77, 0xE2, 0xD4, 0xC6,
		0x8B, 0x19, 0x9C, 0x5A, 0x8E, 0xFC, 0xC1, 0x56, 0xB3, 0xF3, 0xA0, 0x19, 0x55, 0x8F, 0x8E, 0xE7,
		0xED, 0x4A, 0xF4, 0xBA, 0xE2, 0x56, 0xA2, 0x17, 0xB2, 0x3F, 0xB7, 0x1C, 0x4F, 0x67, 0x0A, 0x99,
		0x76, 0x2C, 0x4C, 0x28, 0xF3, 0x42, 0x4F, 0x78, 0xF2, 0xC4, 0xB6, 0x42, 0x3A, 0xE6, 0x95, 0xBD,
		0xC9, 0xE7, 0x9A, 0x8C, 0x95, 0xEF, 0xFE, 0x23, 0x74, 0xA9, 0xE6, 0x95, 0x5F, 0x66, 0x7B, 0xB6,
		0x17, 0x72, 0x47, 0xC7, 0x7A, 0xD6, 0xE2, 0xA4, 0x88, 0x2B, 0x5B, 0x27, 0xAE, 0x35, 0xFC, 0x34,
		0x35, 0xB0, 0x63, 0xF7, 0xAE, 0xC2, 0xD5, 0x77, 0x0E, 0x19, 0x47, 0xC7, 0x8A, 0xEB, 0x96, 0x8B,
		0x5E, 0xCE, 0x3E, 0xAE, 0x5A, 0x2A, 0x1E, 0x54, 0xB2, 0x37, 0x67, 0x25, 0xF9, 0x4B, 0xCD, 0xA9,
		0xAC, 0xDA, 0x58, 0x2A, 0xF8, 0xC3, 0x75, 0x7F, 0x09, 0x42, 0x70, 0x07, 0x2D, 0x2B, 0xE9, 0x65,
		0xF8, 0xE5, 0xDA, 0x6C, 0x7C, 0xFE, 0xF1, 0x44, 0x73, 0xD0, 0xEB, 0xF7, 0x67, 0x0B, 0x7E, 0x5F,
		0x33, 0x26, 0xF3, 0x5B, 0xF2, 0x4B, 0x8D, 0x2A, 0x72, 0x4B, 0x9D, 0x8A, 0xF3, 0x4F, 0xE7, 0xF1,
		0xA6, 0x88, 0x0B, 0xC7, 0xD3, 0x83, 0x73, 0x5F, 0x0B, 0x4F, 0xDD, 0xE7, 0x71, 0xB6, 0xC4, 0xB9,
		0xDC, 0x6F, 0xA7, 0xB6, 0xCF, 0xE5, 0xA1, 0xFF, 0xE0, 0x16, 0x85, 0x05, 0x9B, 0x4E, 0xD9, 0x81,
		0x54, 0xE3, 0xE5, 0x4D, 0x5F, 0x1A, 0x98, 0x63, 0xCA, 0x44, 0x61, 0x09, 0x82, 0x94, 0xE5, 0xB5,
		0x71, 0x91, 0x9E, 0x15, 0x69, 0xB8, 0x37, 0x3F, 0x7F, 0x34, 0x56, 0x9E, 0x17, 0x3D, 0xB4, 0x77,
		0xB4, 0x1C, 0x41, 0x5E, 0xFA, 0x65, 0x2B, 0x15, 0x01, 0xED, 0xBF, 0x45, 0x12, 0xC5, 0xED, 0x93,
		0x4F, 0xDA, 0xC5, 0xBA, 0x55, 0x5D, 0xCF, 0x74, 0x1D, 0x4F, 0xAE, 0xDC, 0x3E, 0xF9, 0xD8, 0x0B,
		0xC9, 0xEB, 0x75, 0xAB, 0x56, 0x3C, 0xF6, 0xC4, 0xAA, 0xFA, 0xD0, 0x48, 0x39, 0x78, 0x52, 0xCA,
		0xE3, 0x14, 0x8A, 0x62, 0x01, 0x2F, 0xE6, 0xF3, 0xB3, 0x7F, 0xDC, 0x3D, 0x05, 0x0C, 0xDA, 0x30,
		0x3D, 0x65, 0x02, 0x11, 0xD8, 0x02, 0x1E, 0x51, 0xF0, 0x6E, 0xAF, 0xF8, 0x37, 0x88, 0xB8, 0x9A,
		0x7A, 0xD1, 0x86, 0xBC, 0x3B, 0xFD, 0x42, 0x87, 0xAD, 0x46, 0xC6, 0x0F, 0xA8, 0x6F, 0xDA, 0xC3,
		0xF0, 0x90, 0xC7, 0x7A, 0x2B, 0x57, 0xF9, 0xF8, 0xE2, 0xBC, 0xE8, 0x78, 0x7B, 0x4A, 0x30, 0x48,
		0x21, 0xEF, 0x11, 0x60, 0xC5, 0xCB, 0xBF, 0x41, 0x3E, 0x57, 0x9E, 0x58, 0x57, 0x3C, 0x6F, 0x1E,
		0x71, 0x2D, 0x96, 0x47, 0xDE, 0x9E, 0x1E, 0xD8, 0x67, 0x2B, 0x11, 0xA1, 0xA8, 0x6F, 0x8E, 0x9B,
		0xDF, 0x8C, 0x2F, 0xE7, 0x8F, 0xBC, 0x15, 0x67, 0xD0, 0x1F, 0xF0, 0x70, 0x46, 0x3B, 0x17, 0x33,
		0x0E, 0xE7, 0x88, 0x10, 0x31, 0xE4, 0x3D, 0x2B, 0x42, 0x91, 0x27, 0xFC, 0xC7, 0xC8, 0xD7, 0xC5,
		0x95, 0xE1, 0xFF, 0x8C, 0x9B, 0x54, 0x97, 0xBB, 0x77, 0xB6, 0xEE, 0xA9, 0xAD, 0xBF, 0x9A, 0xF8,
		0x67, 0xEA, 0x44, 0xAA, 0xEB, 0xD7, 0x93, 0xC9, 0x2B, 0xCF, 0xAE, 0x7E, 0xFA, 0x7B, 0x4F, 0xEC,
		0xAC, 0x0F, 0x3D, 0xCA, 0xD3, 0xCD, 0xE7, 0x31, 0xFA, 0xDD, 0xA1, 0x89, 0x46, 0x8E, 0xBC, 0xAB,
		0xF7, 0x2B, 0xD6, 0x1F, 0xE4, 0xD2, 0x52, 0xCE, 0x9B, 0x4F, 0x7A, 0x37, 0xBB, 0xE8, 0x28, 0xE4,
		0xB7, 0xCD, 0x0C, 0xEE, 0x33, 0x94, 0xF6, 0x34, 0xF2, 0x0C, 0x74, 0x9F, 0xD7, 0x22, 0xAE, 0xA5,
		0xB9, 0x9F, 0x43, 0x91, 0x1E, 0x1D, 0x38, 0x68, 0x6B, 0x67, 0x40, 0x3E, 0x99, 0xD6, 0x89, 0xE7,
		0x16, 0xE4, 0xBE, 0xEE, 0x35, 0xC9, 0x53, 0x8C, 0x1E, 0x79, 0x7A, 0xAF, 0xBD, 0x5F, 0x21, 0x5C,
		0x60, 0x72, 0xF8, 0xF9, 0x9C, 0x8C, 0xFE, 0xF6, 0x5E, 0x4C, 0x0E, 0xF2, 0x54, 0xE9, 0x04, 0x65,
		0xF8, 0xDF, 0xE3, 0x15, 0xBE, 0x10, 0xF1, 0x6E, 0x6E, 0x71, 0x6D, 0x7C, 0x3F, 0x1E, 0x55, 0x91,
		0xD7, 0x9B, 0xE5, 0xD0, 0x26, 0x5D, 0x18, 0x3D, 0xBE, 0x45, 0x5E, 0xFB, 0x2B, 0xF2, 0x72, 0x99,
		0x33, 0x02, 0x23, 0xA0, 0xF9, 0x3C, 0x4E, 0xAD, 0xB8, 0xA4, 0x21, 0x1F, 0xE4, 0x7E, 0x26, 0xB3,
		0x8F, 0xFE, 0x4C, 0xF2, 0x53, 0xE3, 0x10, 0xD7, 0x47, 0x90, 0xB7, 0x22, 0x9E, 0xC5, 0x4B, 0xC8,
		0x37, 0x07, 0x3C, 0x5D, 0x9C, 0x87, 0xB2, 0xFD, 0xEA, 0x05, 0x1D, 0xEB, 0xFE, 0x13, 0x51, 0x62,
		0x16, 0x56, 0x8E, 0xA6, 0x2A, 0x44, 0xCD, 0x61, 0x61, 0x4E, 0xE7, 0x35, 0x59, 0x39, 0x45, 0x59,
		0x39, 0xFF, 0xCA, 0x74, 0x50, 0x35, 0x02, 0x0A, 0x99, 0xB2, 0x21, 0xEA, 0xF3, 0x7E, 0x72, 0xC0,
		0x96, 0xC9, 0x91, 0xFC, 0x08, 0x28, 0xDB, 0x70, 0x68, 0x75, 0x91, 0xA9, 0xDD, 0xF2, 0x56, 0x9C,
		0xCB, 0x12, 0x28, 0x31, 0x27, 0x19, 0x6A, 0x6C, 0x78, 0x87, 0x1B, 0xB9, 0x9C, 0x4E, 0xD9, 0xF4,
		0x9D, 0xEF, 0xD4, 0x85, 0xBB, 0xD6, 0x4F, 0x1E, 0x4F, 0xDE, 0xDE, 0xBE, 0x26, 0x79, 0x38, 0x75,
		0xED, 0xF9, 0x75, 0x5B, 0x63, 0x53, 0x3D, 0xD7, 0x42, 0x3E, 0x3F, 0x5A, 0xB7, 0x6E, 0x75, 0x17,
		0x9E, 0xBB, 0xB7, 0x6B, 0xCD, 0xEF, 0x7E, 0x64, 0x5C, 0x79, 0xAA, 0xB1, 0xA1, 0x77, 0x6A, 0x0C,
		0x1E, 0x99, 0xDA, 0xA0, 0xA4, 0x3C, 0x1D, 0x79, 0x31, 0xE7, 0x23, 0x46, 0x9F, 0xB3, 0x54, 0xCA,
		0xFE, 0x92, 0x16, 0xA5, 0xEA, 0x71, 0xEA, 0x66, 0xF5, 0x5F, 0xB1, 0x72, 0x0F, 0x96, 0xA8, 0xDF,
		0x00, 0x37, 0x85, 0x47, 0xF8, 0xA0, 0xCD, 0xD9, 0x86, 0x17, 0xB4, 0x72, 0xF3, 0x23, 0x5D, 0x9F,
		0x06, 0x55, 0x3E, 0x29, 0x44, 0xA3, 0x3F, 0xE0, 0x64, 0xD0, 0xDD, 0x28, 0x63, 0x5D, 0x9C, 0xFF,
		0xFF, 0x37, 0x26, 0x68, 0x8D, 0x70, 0xE5, 0xBF, 0xB4, 0xA7, 0x7F, 0xE3, 0xBB, 0xEF, 0x88, 0x6E,
		0x47, 0xA4, 0x97, 0x3E, 0x3F, 0xCB, 0x9A, 0x3F, 0xAC, 0x11, 0x96, 0x7C, 0x4D, 0xD1, 0x60, 0xCB,
		0x5C, 0x26, 0x43, 0x8F, 0x36, 0xD0, 0x99, 0x25, 0x16, 0xFB, 0xE6, 0xAC, 0x34, 0xCE, 0x28, 0xAF,
		0x01, 0xCB, 0x35, 0xB4, 0x1C, 0x65, 0xC1, 0x75, 0xDC, 0x61, 0x86, 0xC1, 0x13, 0x4E, 0xC2, 0xC8,
		0xCA, 0xB1, 0x74, 0x23, 0x0B, 0xA1, 0xD1, 0x50, 0x6D, 0xAD, 0x11, 0x4E, 0x8E, 0xD6, 0x37, 0xD6,
		0x37, 0x65, 0x6B, 0xB3, 0xB5, 0xA3, 0x2B, 0xC2, 0xE1, 0x8D, 0xE1, 0xFA, 0x04, 0xD0, 0xB9, 0x9B,
		0x7E, 0xAE, 0xB4, 0x70, 0x28, 0xE4, 0x45, 0x0D, 0xA8, 0xEE, 0xE0, 0x1E, 0x15, 0x3F, 0x03, 0xFD,
		0x93, 0xDC, 0xCA, 0x12, 0x52, 0xD2, 0x2D, 0x42, 0x34, 0x3C, 0x10, 0x94, 0x68, 0xA0, 0x11, 0x0D,
		0x8F, 0x70, 0xB3, 0xD3, 0xF4, 0x81, 0x5E, 0xFF, 0xA0, 0x83, 0x90, 0x3E, 0xA2, 0x29, 0x15, 0xBE,
		0x19, 0xBF, 0x69, 0x4B, 0xA2, 0x39, 0x57, 0x6A, 0x4E, 0x2D, 0xC6, 0x0B, 0x77, 0xB0, 0x4D, 0xC5,
		0xD7, 0xB2, 0xFF, 0x62, 0xE6, 0x84, 0x49, 0x5E, 0xE7, 0x84, 0xC8, 0xA9, 0x01, 0xF2, 0xA1, 0x83,
		0x9E, 0xAA, 0x4A, 0x5E, 0xD5, 0x4F, 0xF6, 0xC6, 0x48, 0x8C, 0x10, 0x40, 0xDE, 0xD2, 0x78, 0x85,
		0xD7, 0xF1, 0x29, 0x5D, 0x59, 0xC8, 0x7A, 0x35, 0x7A, 0x98, 0xC7, 0x27, 0x54, 0x9F, 0xDF, 0x28,
		0x2F, 0x66, 0xC8, 0x7B, 0xE3, 0x3E, 0xEF, 0x08, 0x48, 0xB0, 0xDE, 0x16, 0x53, 0x8B, 0x23, 0x1F,
		0x47, 0xBE, 0x5F, 0x3F, 0x75, 0x41, 0xF2, 0x3A, 0x10, 0xAD, 0xCF, 0x32, 0xF1, 0x79, 0x1F, 0xF0,
		0xB0, 0x0C, 0x6F, 0x65, 0x5F, 0xEE, 0x47, 0x3E, 0x5B, 0xE1, 0x3D, 0x6E, 0x75, 0x9F, 0x65, 0x16,
		0x71, 0x80, 0xE2, 0x87, 0x7F, 0x8B, 0xD9, 0x1D, 0x27, 0xDD, 0xDD, 0x92, 0x4F, 0xE9, 0x27, 0x25,
		0x9F, 0xA4, 0x40, 0xAC, 0x3E, 0xA7, 0xCA, 0x67, 0xCF, 0x15, 0x46, 0x8D, 0xE4, 0xE8, 0xD9, 0x54,
		0xE1, 0x12, 0xAC, 0x58, 0x6F, 0x3C, 0x7B, 0xB7, 0x7E, 0x34, 0xB9, 0xA9, 0x7E, 0xC3, 0xFB, 0xE1,
		0x44, 0x95, 0x2F, 0xF5, 0x7D, 0x80, 0xBC, 0xF9, 0x9E, 0x48, 0x60, 0x36, 0x18, 0x77, 0x2C, 0xAD,
		0x97, 0x39, 0x43, 0x7E, 0x5C, 0xAC, 0xC5, 0x0A, 0xA2, 0x4F, 0xCA, 0xE8, 0x07, 0x06, 0x63, 0xC9,
		0x98, 0xEA, 0xF8, 0x3C, 0x40, 0xC0, 0x6B, 0xA7, 0x6D, 0x4C, 0x97, 0xD6, 0x0B, 0x5C, 0x85, 0x4C,
		0x2F, 0x39, 0x7D, 0xEB, 0x73, 0x4C, 0x9E, 0x16, 0xB1, 0x88, 0xC6, 0xB0, 0x5C, 0x24, 0x3F, 0x75,
		0x77, 0x4F, 0x35, 0x39, 0xC8, 0x33, 0xAE, 0xE1, 0x6B, 0xC5, 0xFA, 0x0C, 0xAB, 0x15, 0x03, 0x68,
		0x31, 0x88, 0xE4, 0x0F, 0xF9, 0xB9, 0xEF, 0x1B, 0x8C, 0xE9, 0x31, 0x95, 0x03, 0x31, 0x4F, 0xE2,
		0xB3, 0x01, 0x93, 0x77, 0x94, 0x88, 0x2E, 0x79, 0x02, 0xBC, 0x1F, 0x32, 0x04, 0x1B, 0x95, 0xC9,
		0x1B, 0x74, 0x2C, 0xE6, 0x47, 0x2F, 0x44, 0x62, 0xEA, 0xC6, 0x81, 0xEA, 0xD4, 0x22, 0x0F, 0xDE,
		0x7B, 0xC8, 0x13, 0x4F, 0xE6, 0x90, 0xB1, 0x16, 0x0D, 0x79, 0x5D, 0x57, 0x1E, 0xF2, 0x9A, 0x87,
		0xBC, 0x69, 0x55, 0xF9, 0xBF, 0x23, 0x9F, 0x80, 0x2C, 0xF2, 0xA9, 0x0A, 0x6F, 0xE0, 0xF9, 0x0B,
		0x60, 0x41, 0x85, 0xB7, 0x06, 0x6F, 0xBC, 0x21, 0x2A, 0x99, 0xA4, 0x84, 0x51, 0xC2, 0x49, 0xC0,
		0x63, 0xE5, 0x88, 0xB3, 0xF2, 0x0E, 0xA2, 0x07, 0x85, 0xD9, 0x37, 0x14, 0xD3, 0x7F, 0x8E, 0x7F,
		0x89, 0x83, 0x3C, 0x82, 0x7E, 0x72, 0x90, 0xD7, 0x41, 0x27, 0x7E, 0x72, 0x7C, 0x5E, 0x0E, 0xDF,
		0xE7, 0x38, 0x50, 0xCD, 0xFD, 0xE0, 0x60, 0x44, 0xBE, 0x4F, 0x70, 0x81, 0xA7, 0xA9, 0x85, 0x85,
		0xC9, 0x35, 0x1B, 0xD3, 0xEB, 0xC8, 0xBA, 0x77, 0xB0, 0xE4, 0x35, 0xAD, 0xC2, 0x93, 0x3E, 0x0D,
		0x6B, 0x5F, 0x8E, 0x0D, 0x26, 0xA9, 0xF2, 0x03, 0xA7, 0xED, 0x21, 0x02, 0x06, 0x1E, 0x69, 0x92,
		0x3F, 0x7D, 0x8B, 0x12, 0x77, 0x08, 0x2C, 0xCB, 0xB1, 0xAB, 0xBC, 0xA1, 0xBD, 0x2F, 0xE4, 0xA6,
		0x40, 0x45, 0x5A, 0x77, 0x46, 0x13, 0xC6, 0x97, 0xD9, 0x02, 0x18, 0x38, 0x3C, 0xAE, 0x5A, 0xC7,
		0x08, 0x87, 0xB2, 0xA1, 0xDA, 0xC6, 0x6C, 0x38, 0xC9, 0xEA, 0x57, 0x6C, 0x5C, 0x9B, 0x6D, 0xCC,
		0x36, 0x24, 0xB3, 0x06, 0x98, 0xE1, 0x6C, 0x6D, 0xC0, 0x8F, 0x9D, 0x2B, 0x5C, 0x35, 0x92, 0x24,
		0x9C, 0xE4, 0xF5, 0xC9, 0x35, 0x61, 0xE3, 0xDC, 0xDD, 0x50, 0xB2, 0x74, 0x15, 0xAC, 0x0F, 0x20,
		0x51, 0x49, 0x8E, 0xFF, 0x69, 0xEC, 0xE9, 0x92, 0x9F, 0xB7, 0xCD, 0x2F, 0x58, 0xE3, 0xD1, 0xE0,
		0x67, 0xFA, 0x41, 0xC7, 0x1D, 0xEF, 0xE1, 0x32, 0xC5, 0x45, 0xA8, 0xF9, 0x7F, 0xF4, 0xEA, 0xAE,
		0x05, 0x72, 0x4B, 0x7B, 0xC0, 0x07, 0x0B, 0x64, 0x5E, 0x5B, 0xC0, 0xD7, 0x89, 0x85, 0x0D, 0xA5,
		0xEA, 0x7E, 0x6E, 0x08, 0xD1, 0xED, 0xFF, 0x69, 0xBC, 0x5F, 0xE9, 0x49, 0xFB, 0xEE, 0xFF, 0x00,
		0xAA, 0xCF, 0x5C, 0xA0, 0xB7, 0x28, 0x68, 0xF7, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44,
		0xAE, 0x42, 0x60, 0x82
	};

	/*
	新宋体ascii 94个符号位图数据
	// 12号像素是6*14宽高
	char r[2] = { 0x21, 0x7e };
	12：6*14=564
	14：7*16=658
	16：8*18=752
	*/
	void nsimsun_ascii(bitmap_ttinfo* obt)
	{
		//hz::stbimage_cx icx;
		std::string str = R"(!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";
		//if (icx.load_mem(fdata, 2180))
		{
			Image* img = obt->_buf;
			if (!img)
			{
				img = Image::create_mem((char*)fdata, 2180);
				//img->saveImage("font_en.png");
				obt->_buf = img;
			}
			else
			{
				img->loadMemImage((char*)fdata, 2180);
			}
			if (img->empty())
			{
				obt->_chsize.clear();
				obt->_unicode_rang.clear();
				return;
			}
			// 支持的大小
			obt->_chsize = { {12, 6}, {14, 7}, {16, 8} };
			// 范围
			obt->_unicode_rang = { {0x21, 0x7e}, {0x21, 0x7e}, {0x21, 0x7e} };
		}
	}




#ifndef NO_COLOR_FONT

	/**************************************************************************
	 *
	 * `COLR' table specification:
	 *
	 *   https://www.microsoft.com/typography/otspec/colr.htm
	 *
	 */



	 /* NOTE: These are the table sizes calculated through the specs. */
#define BASE_GLYPH_SIZE            6
#define LAYER_SIZE                 4
#define COLR_HEADER_SIZE          14


	struct BaseGlyphRecord
	{
		uint16_t  gid;
		uint16_t  first_layer_index;
		uint16_t  num_layers;

	};


	struct Colr
	{
		uint16_t  version;
		uint16_t  num_base_glyphs;
		uint16_t  num_layers;

		uint8_t* base_glyphs;
		uint8_t* layers;

		/* The memory which backs up the `COLR' table. */
		void* table;
		unsigned long  table_size;

	};



	int	tt_face_load_colr(tt_info* face, uint8_t* b, sfnt_header* sp)
	{
		int   error = 0;
		//FT_Memory  memory = face->root.memory;

		uint8_t* table = b;
		uint8_t* p = NULL;

		Colr* colr = NULL;

		uint32_t  base_glyph_offset, layer_offset;
		uint32_t  table_size = sp->logicalLength;
		auto cp = face->colorinfo;

		/* `COLR' always needs `CPAL' */
		//if (!face->cpal)
		//	return FT_THROW(Invalid_File_Format);

		//error = face->goto_table(face, TTAG_COLR, stream, &table_size);
		//if (error)
		//	goto NoColr;
		do {

			if (table_size < COLR_HEADER_SIZE)break;
			//	goto InvalidTable;

			//if (FT_FRAME_EXTRACT(table_size, table))
			//	goto NoColr;

			p = table;
			face->uac.new_mem(1, colr);
			if (!(colr))
				break;

			colr->version = NEXT_USHORT(p);
			if (colr->version != 0)
			{
				error = -1; break;
			}

			colr->num_base_glyphs = NEXT_USHORT(p);
			base_glyph_offset = NEXT_ULONG(p);

			if (base_glyph_offset >= table_size)
			{
				error = -1; break;
			}
			if (colr->num_base_glyphs * BASE_GLYPH_SIZE >
				table_size - base_glyph_offset)
			{
				error = -1; break;
			}

			layer_offset = NEXT_ULONG(p);
			colr->num_layers = NEXT_USHORT(p);

			if (layer_offset >= table_size)
			{
				error = -1; break;
			}
			if (colr->num_layers * LAYER_SIZE > table_size - layer_offset)
			{
				error = -1; break;
			}

			colr->base_glyphs = (uint8_t*)(table + base_glyph_offset);
			colr->layers = (uint8_t*)(table + layer_offset);
			colr->table = table;
			colr->table_size = table_size;

			cp->colr = colr;


		} while (0);

		return error;
	}


	void tt_face_free_colr(tt_info* p)
	{
		//sfnt_header* sp = face->root.stream;
		////FT_Memory  memory = face->root.memory;

		//Colr* colr = (Colr*)face->colr;


		//if (colr)
		//{
		//	FT_FRAME_RELEASE(colr->table);
		//	FT_FREE(colr);
		//}
	}


	static bool find_base_glyph_record(uint8_t* base_glyph_begin,
		int            num_base_glyph,
		uint32_t           glyph_id,
		BaseGlyphRecord* record)
	{
		int  min = 0;
		int  max = num_base_glyph - 1;


		while (min <= max)
		{
			int    mid = min + (max - min) / 2;
			uint8_t* p = base_glyph_begin + mid * BASE_GLYPH_SIZE;

			uint16_t  gid = NEXT_USHORT(p);


			if (gid < glyph_id)
				min = mid + 1;
			else if (gid > glyph_id)
				max = mid - 1;
			else
			{
				record->gid = gid;
				record->first_layer_index = NEXT_USHORT(p);
				record->num_layers = NEXT_USHORT(p);

				return 1;
			}
		}

		return 0;
	}


	bool tt_face_get_colr_layer(tt_info* face,
		uint32_t            base_glyph,
		uint32_t* aglyph_index,
		uint32_t* acolor_index,
		LayerIterator* iterator)
	{
		Colr* colr = (Colr*)face->colorinfo->colr;
		BaseGlyphRecord  glyph_record = {};
		auto cp = face->colorinfo;

		if (!colr)
			return 0;

		if (!iterator->p)
		{
			uint32_t  offset;


			/* first call to function */
			iterator->layer = 0;

			if (!find_base_glyph_record(colr->base_glyphs,
				colr->num_base_glyphs,
				base_glyph,
				&glyph_record))
				return 0;

			if (glyph_record.num_layers)
				iterator->num_layers = glyph_record.num_layers;
			else
				return 0;

			offset = LAYER_SIZE * glyph_record.first_layer_index;
			if (offset + LAYER_SIZE * glyph_record.num_layers > colr->table_size)
				return 0;

			iterator->p = colr->layers + offset;
		}

		if (iterator->layer >= iterator->num_layers)
			return 0;

		*aglyph_index = NEXT_USHORT(iterator->p);
		*acolor_index = NEXT_USHORT(iterator->p);

		if (*aglyph_index >= (uint32_t)(face->num_glyphs) ||
			(*acolor_index != 0xFFFF &&
				*acolor_index >= cp->palette_data.num_palette_entries))
			return 0;

		iterator->layer++;

		return 1;
	}

#define PALETTE_FOR_LIGHT_BACKGROUND  0x01
#define PALETTE_FOR_DARK_BACKGROUND   0x02
	Color_2 get_c2(tt_info* face1, uint32_t color_index)
	{
		Color_2 c = {};
		auto face = face1->colorinfo;
		if (color_index == 0xFFFF)
		{
			if (face->have_foreground_color)
			{
				c.b = face->foreground_color.blue;
				c.g = face->foreground_color.green;
				c.r = face->foreground_color.red;
				c.alpha = face->foreground_color.alpha;
			}
			else
			{
				if (face->palette_data.palette_flags &&
					(face->palette_data.palette_flags[face->palette_index] &
						PALETTE_FOR_DARK_BACKGROUND))
				{
					/* white opaque */
					c.b = 0xFF;
					c.g = 0xFF;
					c.r = 0xFF;
					c.alpha = 0xFF;
				}
				else
				{
					/* black opaque */
					c.b = 0x00;
					c.g = 0x00;
					c.r = 0x00;
					c.alpha = 0xFF;
				}
			}
		}
		else
		{
			c = face->palette[color_index];
		}
		return c;
	}
	int tt_face_colr_blend_layer(tt_info* face1,
		uint32_t       color_index,
		GlyphSlot* dstSlot,
		GlyphSlot* srcSlot)
	{
		int  error = 0;
		auto face = face1->colorinfo;
		uint32_t  x, y;
		uint8_t  b, g, r, alpha;

		uint32_t  size;
		uint8_t* src;
		uint8_t* dst;
#if 0

		if (!dstSlot->bitmap.buffer)
		{
			/* Initialize destination of color bitmap */
			/* with the size of first component.      */
			dstSlot->bitmap_left = srcSlot->bitmap_left;
			dstSlot->bitmap_top = srcSlot->bitmap_top;

			dstSlot->bitmap.width = srcSlot->bitmap.width;
			dstSlot->bitmap.rows = srcSlot->bitmap.rows;
			dstSlot->bitmap.pixel_mode = FT_PIXEL_MODE_BGRA;
			dstSlot->bitmap.pitch = (int)dstSlot->bitmap.width * 4;
			dstSlot->bitmap.num_grays = 256;

			size = dstSlot->bitmap.rows * (unsigned int)dstSlot->bitmap.pitch;

			error = ft_glyphslot_alloc_bitmap(dstSlot, size);
			if (error)
				return error;

			FT_MEM_ZERO(dstSlot->bitmap.buffer, size);
		}
		else
		{
			/* Resize destination if needed such that new component fits. */
			int  x_min, x_max, y_min, y_max;


			x_min = FT_MIN(dstSlot->bitmap_left, srcSlot->bitmap_left);
			x_max = FT_MAX(dstSlot->bitmap_left + (int)dstSlot->bitmap.width,
				srcSlot->bitmap_left + (int)srcSlot->bitmap.width);

			y_min = FT_MIN(dstSlot->bitmap_top - (int)dstSlot->bitmap.rows,
				srcSlot->bitmap_top - (int)srcSlot->bitmap.rows);
			y_max = FT_MAX(dstSlot->bitmap_top, srcSlot->bitmap_top);

			if (x_min != dstSlot->bitmap_left ||
				x_max != dstSlot->bitmap_left + (int)dstSlot->bitmap.width ||
				y_min != dstSlot->bitmap_top - (int)dstSlot->bitmap.rows ||
				y_max != dstSlot->bitmap_top)
			{
				FT_Memory  memory = face->root.memory;

				uint32_t  width = (uint32_t)(x_max - x_min);
				uint32_t  rows = (uint32_t)(y_max - y_min);
				uint32_t  pitch = width * 4;

				uint8_t* buf = NULL;
				uint8_t* p;
				uint8_t* q;


				size = rows * pitch;
				if (FT_ALLOC(buf, size))
					return error;

				p = dstSlot->bitmap.buffer;
				q = buf +
					(int)pitch * (y_max - dstSlot->bitmap_top) +
					4 * (dstSlot->bitmap_left - x_min);

				for (y = 0; y < dstSlot->bitmap.rows; y++)
				{
					FT_MEM_COPY(q, p, dstSlot->bitmap.width * 4);

					p += dstSlot->bitmap.pitch;
					q += pitch;
				}

				ft_glyphslot_set_bitmap(dstSlot, buf);

				dstSlot->bitmap_top = y_max;
				dstSlot->bitmap_left = x_min;

				dstSlot->bitmap.width = width;
				dstSlot->bitmap.rows = rows;
				dstSlot->bitmap.pitch = (int)pitch;

				dstSlot->internal->flags |= FT_GLYPH_OWN_BITMAP;
				dstSlot->format = FT_GLYPH_FORMAT_BITMAP;
			}
		}

		if (color_index == 0xFFFF)
		{
			if (face->have_foreground_color)
			{
				b = face->foreground_color.blue;
				g = face->foreground_color.green;
				r = face->foreground_color.red;
				alpha = face->foreground_color.alpha;
			}
			else
			{
				if (face->palette_data.palette_flags &&
					(face->palette_data.palette_flags[face->palette_index] &
						FT_PALETTE_FOR_DARK_BACKGROUND))
				{
					/* white opaque */
					b = 0xFF;
					g = 0xFF;
					r = 0xFF;
					alpha = 0xFF;
				}
				else
				{
					/* black opaque */
					b = 0x00;
					g = 0x00;
					r = 0x00;
					alpha = 0xFF;
				}
			}
		}
		else
		{
			b = face->palette[color_index].blue;
			g = face->palette[color_index].green;
			r = face->palette[color_index].red;
			alpha = face->palette[color_index].alpha;
		}

		/* XXX Convert if srcSlot.bitmap is not grey? */
		src = srcSlot->bitmap.buffer;
		dst = dstSlot->bitmap.buffer +
			dstSlot->bitmap.pitch * (dstSlot->bitmap_top - srcSlot->bitmap_top) +
			4 * (srcSlot->bitmap_left - dstSlot->bitmap_left);

		for (y = 0; y < srcSlot->bitmap.rows; y++)
		{
			for (x = 0; x < srcSlot->bitmap.width; x++)
			{
				int  aa = src[x];
				int  fa = alpha * aa / 255;

				int  fb = b * fa / 255;
				int  fg = g * fa / 255;
				int  fr = r * fa / 255;

				int  ba2 = 255 - fa;

				int  bb = dst[4 * x + 0];
				int  bg = dst[4 * x + 1];
				int  br = dst[4 * x + 2];
				int  ba = dst[4 * x + 3];


				dst[4 * x + 0] = (uint8_t)(bb * ba2 / 255 + fb);
				dst[4 * x + 1] = (uint8_t)(bg * ba2 / 255 + fg);
				dst[4 * x + 2] = (uint8_t)(br * ba2 / 255 + fr);
				dst[4 * x + 3] = (uint8_t)(ba * ba2 / 255 + fa);
			}

			src += srcSlot->bitmap.pitch;
			dst += dstSlot->bitmap.pitch;
		}
#endif
		return error;
	}



	/**************************************************************************
	 *
	 * `CPAL' table specification:
	 *
	 *   https://www.microsoft.com/typography/otspec/cpal.htm
	 *
	 */

	 /* NOTE: These are the table sizes calculated through the specs. */
#define CPAL_V0_HEADER_BASE_SIZE  12
#define COLOR_SIZE                 4


  /* all data from `CPAL' not covered in FT_Palette_Data */
	struct Cpal
	{
		uint16_t  version;        /* Table version number (0 or 1 supported). */
		uint16_t  num_colors;               /* Total number of color records, */
		/* combined for all palettes.     */
		uint8_t* colors;                              /* RGBA array of colors */
		uint8_t* color_indices; /* Index of each palette's first color record */
		/* in the combined color record array.        */

/* The memory which backs up the `CPAL' table. */
		void* table;
		uint32_t  table_size;

	};



	int tt_face_load_cpal(tt_info* face1, uint8_t* b,
		sfnt_header* sp)
	{
		int   error = 0;
		//FT_Memory  memory = face->root.memory;
		auto face = face1->colorinfo;
		uint8_t* table = b;
		uint8_t* p = NULL;

		Cpal* cpal = NULL;

		uint32_t  colors_offset = 0;
		uint32_t  table_size = sp->logicalLength;

#if 1
		//error = face->goto_table(face, TTAG_CPAL, stream, &table_size);
		//if (error)
		//	goto NoCpal;
		do {

			if (table_size < CPAL_V0_HEADER_BASE_SIZE)
				break;

			//if (FT_FRAME_EXTRACT(table_size, table))
			//	goto NoCpal;

			p = table;
			face1->uac.new_mem(1, cpal);
			if (!cpal)
				break;

			cpal->version = NEXT_USHORT(p);
			if (cpal->version > 1)
			{
				error = -1; break;
			}

			face->palette_data.num_palette_entries = NEXT_USHORT(p);
			face->palette_data.num_palettes = NEXT_USHORT(p);

			cpal->num_colors = NEXT_USHORT(p);
			colors_offset = NEXT_ULONG(p);

			if (CPAL_V0_HEADER_BASE_SIZE +
				face->palette_data.num_palettes * 2U > table_size)
			{
				error = -1; break;
			}

			if (colors_offset >= table_size)
			{
				error = -1; break;
			}
			if (cpal->num_colors * COLOR_SIZE > table_size - colors_offset)
			{
				error = -1; break;
			}

			if (face->palette_data.num_palette_entries > cpal->num_colors)
			{
				error = -1; break;
			}

			cpal->color_indices = p;
			cpal->colors = (uint8_t*)(table + colors_offset);

			if (cpal->version == 1)
			{
				uint32_t    type_offset, label_offset, entry_label_offset;
				uint16_t* array = NULL;
				uint16_t* limit;
				uint16_t* q;


				if (CPAL_V0_HEADER_BASE_SIZE +
					face->palette_data.num_palettes * 2U +
					3U * 4 > table_size)
				{
					error = -1; break;
				}

				p += face->palette_data.num_palettes * 2;

				type_offset = NEXT_ULONG(p);
				label_offset = NEXT_ULONG(p);
				entry_label_offset = NEXT_ULONG(p);

				if (type_offset)
				{
					if (type_offset >= table_size)
					{
						error = -1; break;
					}
					if (face->palette_data.num_palettes * 2 >
						table_size - type_offset)
					{
						error = -1; break;
					}

					if (!face1->uac.new_mem(array, face->palette_data.num_palettes))
					{
						error = -2; break;
					}

					p = table + type_offset;
					q = array;
					limit = q + face->palette_data.num_palettes;

					while (q < limit)
						*q++ = NEXT_USHORT(p);

					face->palette_data.palette_flags = array;
				}

				if (label_offset)
				{
					if (label_offset >= table_size)
					{
						error = -1; break;
					}
					if (face->palette_data.num_palettes * 2 >
						table_size - label_offset)
					{
						error = -1; break;
					}

					//if (FT_QNEW_ARRAY(array, face->palette_data.num_palettes))
					if (!face1->uac.new_mem(array, face->palette_data.num_palettes))
					{
						error = -2; break;
					}

					p = table + label_offset;
					q = array;
					limit = q + face->palette_data.num_palettes;

					while (q < limit)
						*q++ = NEXT_USHORT(p);

					face->palette_data.palette_name_ids = array;
				}

				if (entry_label_offset)
				{
					if (entry_label_offset >= table_size)
					{
						error = -1; break;
					}
					if (face->palette_data.num_palette_entries * 2 >
						table_size - entry_label_offset)
					{
						error = -1; break;
					}

					if (!face1->uac.new_mem(array, face->palette_data.num_palette_entries))
					{
						error = -2; break;
					}

					p = table + entry_label_offset;
					q = array;
					limit = q + face->palette_data.num_palette_entries;

					while (q < limit)
						*q++ = NEXT_USHORT(p);

					face->palette_data.palette_entry_name_ids = array;
				}
			}

			cpal->table = table;
			cpal->table_size = table_size;

			face->cpal = cpal;

			/* set up default palette */
			if (!face1->uac.new_mem(face->palette,
				face->palette_data.num_palette_entries))
			{
				error = -2; break;
			}

			if (tt_face_palette_set(face1, 0))
			{
				error = -1; break;
			}
			error = 0;
			break;

		} while (0);
		if (error < 0)
		{
			//InvalidTable:
			//	error = -1;// FT_THROW(Invalid_Table);

			//NoCpal:
				//FT_FRAME_RELEASE(table);
			face1->uac.free_mem(cpal, 1);

			face->cpal = NULL;

		}
		/* arrays in `face->palette_data' and `face->palette' */
		/* are freed in `sfnt_done_face'                      */
#endif
		return error;
	}


	void tt_face_free_cpal(tt_info* face)
	{
		//sfnt_header* sp = face->colorinfo.;
		//FT_Memory  memory = face->root.memory;

		//Cpal* cpal = (Cpal*)face->cpal;


		//if (cpal)
		{
			//	FT_FRAME_RELEASE(cpal->table);
			//	FT_FREE(cpal);
		}
	}

	int tt_face_palette_set(tt_info* face, uint32_t  palette_index)
	{
		Cpal* cpal = (Cpal*)face->colorinfo->cpal;
		auto cp = face->colorinfo;
		uint8_t* offset;
		uint8_t* p;

		Color_2* q;
		Color_2* limit;

		uint16_t  color_index;

		if (!cpal || palette_index >= cp->palette_data.num_palettes)
			return -6;// FT_THROW(Invalid_Argument);

		offset = cpal->color_indices + 2 * palette_index;
		color_index = PEEK_USHORT(offset);

		if (color_index + cp->palette_data.num_palette_entries >
			cpal->num_colors)
			return -7;// FT_THROW(Invalid_Table);

		p = cpal->colors + COLOR_SIZE * color_index;
		q = (Color_2*)cp->palette;
		limit = q + cp->palette_data.num_palette_entries;

		while (q < limit)
		{
			q->blue = NEXT_BYTE(p);
			q->green = NEXT_BYTE(p);
			q->red = NEXT_BYTE(p);
			q->alpha = NEXT_BYTE(p);

			q++;
		}

		return 0;
	}






#endif // !NO_COLOR_FONT










}
//!hz
