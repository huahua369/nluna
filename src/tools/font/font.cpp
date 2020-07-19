
//#include "pch.h"

#include <data/json_helper.h>

#include "font.h"
#include "view/mapView.h"
#include <view/file.h>

namespace hz
{
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

	// 文件名，是否复制数据


	/* FIR filter used by the default and light filters */

	void Fonts::lcd_filter_fir(Fonts::Bitmap* bitmap, int mode, int weights_type)
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

	int Fonts::add_font_file(const std::string& fn)
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
		MapView* mvfp;
		{
			//print_time pti("add_font_file");
			mvfp = load_file(fn);
		}
#endif // 0
		if (mvfp && mvfp->getFileSize())
		{
			//print_time pti("add_font_file afm");
			auto& fdi = fd_data.emplace_back(fd_info{ mvfp });
			ret = add_font_mem(fdi.data(), fdi.size(), false);
		}
		return ret;
	}

	int Fonts::add_font_mem(const char* data, size_t len, bool iscp)
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
				//njson fot;
				//fot["ascender"] = (float)ascent / (float)fh;
				//fot["descender"] = (float)descent / (float)fh;
				//fot["lineh"] = (float)(fh + lineGap) / (float)fh;
				//fot["_ascent"] = ascent;
				//fot["_descent"] = descent;
				//fot["_lineGap"] = lineGap;
				auto name = ft.get_font_name(fi);
				//ftn.push_back(fot);
				//fp.push_back(fi);
				font->fh = fh;
				font->ascender = (float)ascent / (float)fh;
				font->descender = (float)descent / (float)fh;
				font->lineh = (float)(fh + lineGap) / (float)fh;
				font->_name = font->get_info_str(def_language_id);
				font->_aname = u8_gbk(font->_name);
				if (1)
				{
					// print_time pti("add_font_mem");
					font->init_post_table();
#ifndef _FONT_NO_BITMAP
					font->init_sbit();
#endif // !_FONT_NO_BITMAP
					LOCK_W(_lk);
					_tbs[font->_name].push_back(font);
					_fonts.push_back(font);
				}
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

	Fonts::tt_info* Fonts::get_font(const std::string& name, const std::string& st)
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

	Fonts::sfnt_header* Fonts::get_tag(font_impl_info* font_i, std::string tag)
	{
		auto it = font_i->_tb.find(tag);
		return it != font_i->_tb.end() ? &it->second : nullptr;
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

	Fonts::Glyph* Fonts::get_glyph_test(tt_info* font, unsigned int codepoint, short isize, short iblur, int bitmapOption)
	{
		int i, g, advance, lsb, x0, y0, x1, y1, gw, gh, gx = 0, gy = 0, x, y;
		float scale;
		Fonts::Glyph* glyph = NULL;
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

	void Fonts::save_cache(std::string fdn)
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

	glm::vec3 Fonts::make_outline_y(css_text* ct, size_t n)
	{
		glm::ivec3 ret = { 0,0,0 };
		std::vector<glm::ivec3> os;
		css_text* mt = 0;
		for (size_t i = 0; i < n; i++)
		{
			auto it = &ct[i];
			bool ism = false;
			auto& ft = it->get_fonts();
			double lthis = 1.0;
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

	std::vector<Fonts::ft_item*> Fonts::make_cache(tt_info* font, ft_key_s* key, double fns, std::vector<tt_info*>* fallbacks, int lcd_type)
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
				auto bit = rfont->get_glyph_image(gidx, fns, &rc, bitmap, &bitbuf[0], lcd_type);
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
#define _NO_CACHE_
	// todo		渲染到image
	void Fonts::build_text(image_text_info* info, const std::string& str, size_t first_idx, size_t count)
	{
		Fonts::tt_info* rfont = nullptr;
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
						py + dbl + bft->_baseline.y + info->blur_pos.y - info->blur_size };
					info->dst->draw_image2(bft->_image, bft->_rect, dps, info->color_blur);
				}
				glm::ivec2 dps = { kern + tpx + ftit->_baseline.x,py + dbl + ftit->_baseline.y };
				info->dst->draw_image2(ftit->_image, ftit->_rect, dps, info->color);
				adv = ftit->_advance;
			}
			tpx += adv;
#else
			int gidx = info->font->get_glyph_index(unicode_codepoint, &rfont, info->font_family);
			if (rfont)
			{
				auto bit = rfont->get_glyph_image(gidx, fns, &rc, bitmap, &bit_buf[0], info->lcd_type);
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
					glm::ivec4 frc = { kern + tpx + rc.x,py + dbl + rc.y, bitmap->width, bitmap->rows };
					if (info->lcd_type)
					{
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
	// todo		生成到draw_font_info

	glm::ivec2 Fonts::build_info(Fonts::css_text* csst, const std::string& str, size_t count, size_t first_idx, glm::ivec2 pos, draw_font_info* out)
	{
		glm::ivec2 ret;
		if (!csst || count < 1)
		{
			return ret;
		}
		Fonts::tt_info* rfont = nullptr;
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
		Fonts::ft_key_s fks[1] = {};
		auto& fk = fks->v;
		std::vector<Fonts::ft_item*> ftits;
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
			ftits = make_cache(nullptr, fks, fns, csst->get_font_family(), csst->lcd_type);

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
						dii.unser_data = (void*)unicode_codepoint;
						dii.adv = adv;
						dii.unser_data1 = ctip;
						vblurs.push_back(dii);
					}
				}
				glm::ivec2 dps = { tpx + ftit->_baseline.x,py + dbl + ftit->_baseline.y };
				draw_image_info dii;
				dii.user_image = ftit->_image;
				dii.a = { dps.x + kern, dps.y, ftit->_rect.z, ftit->_rect.w };
				dii.rect = ftit->_rect;
				dii.col = csst->color;
				dii.unser_data = (void*)unicode_codepoint;
				dii.adv = adv;
				dii.unser_data1 = ctip;
				double iy0 = (double)dii.a.y + dii.a.w - pos.y;
				iy0 -= fns;
				ctip->maxbs = std::max(ctip->maxbs, iy0);
				vitem->push_back(dii);
			}
			vpos->push_back({ adv, py });
			tpx += adv + spaces;
		}
		njson posn, dif;
		mx = std::max(tpx, mx);
		ret.x = mx;
		ret.y = pos.y;
		if (out)
		{
			out->base_line = ctip->base_line;
			out->diffbs = ctip->bs;
			out->awidth = mx;
		}
		float miny = pos.y;
		double cury = 0;
		int i = 0;
		for (auto& it : *vitem)
		{
			posn.push_back(v4to(it.a));
			double iy = (double)it.a.y + it.a.w - pos.y;
			iy -= fns;
			dif.push_back(iy);
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
				it.a.y = std::max(it.a.y, miny);
				if (it.a.y + it.a.w > maxh)
				{
					double diff = (double)it.a.y + it.a.w - maxh;
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
		glm::vec2 texsize = { info->user_image->width , info->user_image->height };
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
		int ret = 0;
		glm::ivec2 dpos = { a.x, a.y };
		dst->draw_image2(info->user_image, rect, dpos, info->col);

		if (info->out)
			*(info->out) = ret;
	}
	glm::ivec2 Fonts::build_to_image(Fonts::css_text* csst, const std::string& str, size_t count, size_t first_idx, glm::ivec2 pos, Image* dst)
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

	// 初始化位图信息

	int Fonts::tt_info::init_sbit()
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
#if 0
		//ndef nnDEBUG
		printf("<%s>包含位图大小\n", _aname.c_str());
		for (auto& it : ns)
			printf("%s\n", it.dump().c_str());
#endif // !nnDEBUG
		// 位图数据表ebdt
		b = fc + ebdt_table->offset;
		glm::ivec2 version = { stb_font::ttUSHORT(b + 0), stb_font::ttUSHORT(b + 2) };
		_sbit_table = sbit_table;
		_ebdt_table = ebdt_table;
		_ebsc_table = ebsc_table;
		return ns.size();
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

	int Fonts::tt_info::get_glyph_image(int gidx, double height, glm::ivec4* ot, Bitmap* bitmap, std::vector<char>* out, int lcd_type)
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
				glm::vec2 lcds[] = { { 1,1 },{ 3,1 },{ 1,3 },{ 4,1, } };
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
}
//!hz
