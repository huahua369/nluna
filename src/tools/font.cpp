#include "tools.h"
#include "font.h"

namespace hz
{
	// todo		渲染到image

	void Fonts::build_text(Image* dst, Fonts::tt_info* font, const std::string& str, size_t count, size_t first_idx, double font_size, unsigned int color, glm::ivec2 pos, unsigned char blur_size, glm::ivec2 blur_pos, unsigned int color_blur, double dpi, bool first_bitmap)
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
			ftits = ft->make_cache(font, fks, fns, nullptr);
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
			ftits = make_cache(nullptr, fks, fns, csst->get_font_family());

			if (ch > 128)
			{
				ch = ch;
			}
			if (ftits.size())
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
			tpx += adv + csst->_fzpace;
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
			auto t = (unsigned int)it.unser_data;
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
}
//!hz