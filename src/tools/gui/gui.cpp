
#include <ntype.h>
#include <base/camera.h>
#include <view/image.h>
#include <view/file.h>
#include <view/flex_cx.h>
#include <font/font_cx.h>
#include <vk_core/vk_cx.h> 
#include <vk_core/view.h>
#include <vk_core/vkc.h>
#include <base/base_util.h>
#include <base/print_time.h>
#include <base/text/buffer.h>
#include <base/text/pieceTable.h>
#include <base/func.h>
#include <vk_core/canvas2d.h>
#include <SDL2/SDL_keyboard.h>
#include <vk_core/bw_sdl.h>
#include <stb_src/stb/stb_image.h>
#include <stb_src/stb/stb_image_write.h>
#include <base/ecc_sv.h>
#include "hz_gui.h"
// 内存泄漏检测
#ifdef mem_DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
#include <base/Action.h>

#include <shared.h>

using namespace entt;
using namespace hz;

// cpp
namespace hz {

	item_box_info::item_box_info() {}
	item_box_info::~item_box_info() {
		free_edit(_edit);

	}
	void item_box_info::set_font_height(double fs)
	{
		if (fontheight < 6)
		{
			fontheight = 6;
		}
		fs = ceil(fs);
		fontheight = fs;
		isup = true;
	}
	void item_box_info::set_row_span(double row_span, bool autobr)
	{
		auto rs = ceil(fontheight * row_span);
		row_span = rs;
		autobr = autobr;
	}
	void item_box_info::set_style(font_style_c* st)
	{
		if (!st)return;
		mk_rs(st);
		double fontheight = st->font_height;
		if (fontheight < 6)
		{
			fontheight = 6;
		}
		set_font_height(fontheight);
		set_row_span(st->font_row_span, st->autobr);
		isup = true;
	}

	void item_box_info::update(layout_text* lt, const char* str, int size, familys_t* familys)
	{
		if (title.size() != size || title.compare(0, title.size(), str, size) != 0 || familys != _familys)
		{
			title = str;
			isup = true;
		}
		//if (!_familys && !familys)return;
		if (lt && isup)
		{
			auto bal = lt->get_base_line(fontheight);
			base_line = bal;
			s.str = title.c_str();
			s.count = get_utf8_count(s.str, title.size());
			s.first = 0;
			lt->do_text(this, familys);
			_familys = familys;
			mkimg();

			isup = false;
		}
	}
	void item_box_info::update(layout_text* lt, const t_string& str, familys_t* familys)
	{
		if (str != title)
		{
			title = str;
			isup = true;
		}
		if (lt && isup)
		{
			auto bal = lt->get_base_line(fontheight);
			base_line = bal;
			s.str = title.c_str();
			s.count = get_utf8_count(s.str, title.size());
			s.first = 0;
			lt->do_text(this, familys);
			mkimg();
			isup = false;
		}
	}
	size_t item_box_info::get_count() { return _n; }
	void item_box_info::mkimg()
	{
		//size_t n = 0;
		//for (auto& it : vtf)
		//{
		//	for (auto& ct : it.item)
		//	{
		//		glm::ivec2 size = { ct._rect.z, ct._rect.w };
		//		if (size.x > 0 && size.y > 0)
		//		{
		//			n++;
		//		}
		//	}
		//}
		_n = 0;
		if (vtf.empty())return;

		int rh = box_size.y;
		unsigned int view_width = extent ? extent->_view_width : -1;
		int my = 0, cx = 0, cy = 0;
		int height = fontheight + row_span;
		int base_line1 = 0;// base_line;
		size_t count = 0;
		size_t i = 0;
		size_t idx = 0;
		buf.clear();
		for (auto& it : vtf)
		{
			auto nt = it.item.size();
			count = 0;
			bool nauto = true;
			// 计算自动换行
			if (cx > 0 && ((unsigned int)(cx + it.div.width) >= view_width))
			{
				cy += height;
				cx = my = 0; nauto = false;
				idx++;
			}
			for (auto& ct : it.item)
			{
				glm::ivec2 dpos = { cx + ct._dwpos.x + ct._offset.x
					, cy + base_line1 + ct._dwpos.y + ct.adv.y + ct._offset.y };
				glm::ivec2 size = { ct._rect.z, ct._rect.w };
				if (size.x > 0 && size.y > 0)
				{
					image_m_cs m = {};
					m.img = ct._image;
					m.rect = ct._rect;
					m.offset = dpos;
					m.size = size;
					m.color = ct.color;// ? ct.color : color;
					m.cluster = idx;
					m.advx = ct.adv.x;
					buf.push_back(m);
				}
				else
					cx = cx;
				count += ct.chlen;
				cx += ct.adv.x;
				if (ct.ch[0] == splitch)
				{
					idx++;
				}
				if (ct.ch[0] == '\n')
				{
					if (count > 1 || nauto)
						cy += height;
					cx = my = 0;
					idx++;
				}
			}
			//idx++;
		}
		_n = buf.size();
	}

	void item_box_info::to_image(const glm::vec2& pos1, unsigned int color, image_m_cs* opt, size_t n, size_t first, bool isbl)
	{
		auto pos2 = pos1;
		if (isbl)
		{
			pos2 += base_line;
		}
		if (_n && first < _n)
		{
			auto p = buf.data();
			if (buf.size() > 10)
			{
				p = p;
			}
			n = std::min(_n - first, n);
			for (size_t i = 0; i < n; i++)
			{
				opt[i] = p[first + i];
				opt[i].pos = pos1;
				//if (!opt[i].color || off_color)
				//	opt[i].color = color;
			}
		}
	}


	//!flex
	unsigned int get_cols(glm::ivec3** ctp, size_t i, unsigned int& dc,
		unsigned int oc) {
		auto& ct = *ctp;
		if (ct) {
			if (ct->y <= i - ct->x && dc != oc) {
				ct++;
				dc = oc;
			}
			if (ct->x == i) {
				dc = ct->z;
			}
		}
		return dc;
	}
	void mk_colors(item_box_info* res_box) {
		if (!res_box /*|| res_box->_mcolors.empty()*/)
			return;
		int64_t i = 0;
		std::vector<lt_item_t*> rv;
		int64_t f1 = -1;
		auto f2 = f1;
		auto f3 = f2;
		for (auto& it : res_box->vtf) {
			auto nt = it.item.size();
			if (it.rtl) {
				if (f1 == -1)
					f1 = rv.size();
			}
			else if (f1 != -1) {
				f2 = f1;
				f3 = rv.size() - f1;
				std::reverse(rv.begin() + f1, rv.end());
				f1 = -1;
			}

			for (auto& ct : it.item) {
				auto cl = ct.chlen;
				for (size_t x = 0; x < cl; x++) {
					rv.push_back(&ct);
				}
			}
		}
		i = 0;
		for (auto& rct : rv) {
			auto& ct = *rct;
			for (auto it : res_box->_mcolors) {
				if (i < it.x || i >= it.x + it.y) {
					continue;
				}
				ct.color = it.z;
				break;
			}
			auto cl = 1; // ct.chlen;
			for (size_t x = 0; x < cl; x++) {
				if (ct.ch[x] == '\r' || ct.ch[x] == '\n')
					cl--;
			}
			i += cl;
		}
	}
	//text_view_t* gui_test(event_master* emt) {
	//	constexpr char panda_url[] = "src='https://upload.wikimedia.org/wikipedia/"
	//		"commons/f/fa/Panda_bear_at_memphis_zoo.JPG'";
	//	{


	//		html<
	//			head<title<"HTML++ = C++ Templates + HTML">>,
	//			body<
	//			h1<"This was a mistake">, h2<"Here are some pictures:">,
	//			img<"src='https://afd.cp/ad.png'", "alt='A panda bear'", "width=300">,
	//			br, h2<"Here is some text">,
	//			div_<p<"Hello, World!">,
	//			p<"Yes, this document structure has been typechecked :)">>,
	//			a<"href='https://en.cppreference.com/'", "This website is helpful">
	//			>
	//			>
	//			page;

	//		std::cout << page.content;
	//	}
	//	text_view_t* evt = 0;
	//	return evt;
	//}
	text_view_t* init_text_view(text_view_t* tvp, ui_text_style_t style, font_style_c* st, const char* str, ui_mem_t* uam)
	{
		auto evt = tvp;
		if (!evt)return tvp;
		for (; evt;)
		{
			if (uam)
				evt->lt = uam->lt;
			newop(evt->res_box);
			if ((int)style & (int)ui_text_style::e_is_read_only)
			{
				newop(evt->buf);
				newop(evt->_editing);
				evt->res_box->next = evt->_editing;
			}
			if ((int)style & (int)ui_text_style::e_select)
			{
			}

			double fontheight = st->font_size * st->font_dpi / 72.0;
			if (fontheight < 6)
			{
				fontheight = 6;
			}
			evt->set_font_height(fontheight);
			if ((int)style & (int)ui_text_style::e_event)
			{
				newop(evt->text_ed);
				newop(evt->str_rc);
				evt->res_box->extent = evt->str_rc;
				glm::ivec2 edit1pos = { 0, 0 };
				auto edit1 = text_event_t::make_event_cb(evt->text_ed, evt->res_box, evt->buf, evt);
				evt->ep = edit1;
				evt->text_ed->set_nodepos(edit1pos);
				text_event_t::set_rect(edit1, { 0, 0, st->size.x, st->size.y }, HZ_CB_s3(text_up_cb, evt));
				if (uam && uam->emt)
				{
					text_set_event_master(evt, uam->emt);
				}
				evt->set_size(st->size);
				evt->set_row_span(st->row_span, st->autobr);
			}
			if (evt->buf)
				evt->buf->set_data(str, str ? strlen(str) : 0);
			if (str && *str)
			{
				if (evt->buf)
					evt->str = evt->buf->get_row_str();
				else
					evt->str = str;
				auto& pw = evt->str;
				evt->res_box->s.str = pw.c_str();
				evt->res_box->s.count = get_utf8_count(evt->res_box->s.str, pw.size());
				evt->res_box->s.first = 0;
				double fonth = evt->_editing->fontheight = evt->res_box->fontheight = 20;
				//evt->res_box->_mcolors = 0; // t_vector<glm::ivec3>();
				//auto rs = (evt->res_box->row_span > 1) ? evt->res_box->row_span : ceil(evt->res_box->fontheight * evt->res_box->row_span);
			}
			evt = evt->next;
			if (evt == tvp)evt = 0;
		}
		return tvp;
	}

	void text_set_event_master(text_view_t* tvp, event_master* emt)
	{
		if (emt)
		{
			text_event_t::set_ctx(tvp->ep, emt->get_bwctx());
			tvp->text_ed->order = edit_order;
			emt->push(tvp->text_ed);
		}
	}
	int mk_rs(font_style_c* ust)
	{
		auto fh = ceil(ust->font_size * ust->font_dpi / 72.0);
		auto rs = /*(ust->row_span > 1) ? ust->row_span :*/ ceil(fh * ust->row_span);
		ust->font_row_span = rs;
		ust->font_height = fh;
		return rs;
	}
	struct item_w
	{
		int weight = 0;	// 宽/高度
		int n = 0;		// 数量
	};


#if 0

	auto ps = evt->buf->get_node_bitsize();
	evt->buf->append((char*)u8"你好fi中\ttt\n", -1);
	std::string str1a =
		(char*)u8"用颜色名称来指定颜色 空间。参\t阅：颜色关键字\n#RRGGBB 或 "
		u8"#RGB、RGB(R,G,B)、#RRGGBB "
		u8"或#RGB、HSL(H,S,L)、HSLA(H,S,L,A)\nAlpha透明度。取值0~1之间。";
	char32_t wt[] = { 0x1F4A8, 0xfe49, 0xfe4d, 0x2508, 0x2509 };
	// evt->buf->append(str1a.c_str(), str1a.size());
	str1a = (char*)u8"工النصوص العربية你ag好";
	evt->buf->append(str1a.c_str(), str1a.size());
	// PieceTree ptree;
	// ptree.insert_v(0, "pos123\n456789\nplk");
	// auto astr1_ = ptree.getLinesRawContent();
	// printf("0:\t%s\n", astr1_.c_str());
	////ptree.insert_v(3, "masdf5655757");
	////ptree.insert_v(7, "abcdefghfklsadfjklfjkasdf");
	// astr1_ = ptree.getLinesRawContent();
	////printf("1:\t%s\n", astr1_.c_str());
	////ptree.remove_v(12, 10);
	// astr1_ = ptree.getValueInRange({ 1,1,2,2 }, "");
	// auto str1_f = ptree.get_range({ 1,1 }, { 2,2 });
	// std::string astrsss[] = {
	// ptree.getLineContent(1),ptree.getLineContent(2),ptree.getLineRawContent(3)
	// }; printf("remove1:\t%s\n", astr1_.c_str());
#endif

	text_view_t::text_view_t() {}
	text_view_t::~text_view_t()
	{
		delop(res_box);
		delop(text_ed);
		delop(str_rc);
		delop(buf);
		delop(_editing);
	}
	void text_view_t::set_text(const char* str, int len)
	{
		//if (len == this->str.size() && str == this->str)return;
		if (buf)
		{
			buf->set_data(str, len);
		}
		else
		{
			if (len < 0)len = strlen(str);
			this->str.assign(str, len);
		}
		text_up_cb(ep, true, false, this);

	}
	void text_view_t::set_text(const std::string& str)
	{
		//if (str == this->str)return;
		if (buf)
		{
			buf->set_data(str.c_str(), str.size());
		}
		else
		{
			this->str = str;
		}
		text_up_cb(ep, true, false, this);
	}
#ifndef _NO_PMR_


	void text_view_t::set_text(const t_string& str)
	{
		//if (str.c_str() == this->str)return;
		if (buf)
		{
			buf->set_data(str.c_str(), str.size());
		}
		else
		{
			this->str = str.c_str();
		}
		text_up_cb(ep, true, false, this);
	}
#endif // !_NO_PMR_
	t_string text_view_t::get_text()
	{
		return c_str();
	}
	const char* text_view_t::c_str()
	{
		return res_box->s.str;
	}
	size_t text_view_t::c_size()
	{
		return res_box->s.count;
	}
	void text_view_t::set_pos(const glm::ivec2& pos)
	{
		if (text_ed)
			text_ed->set_nodepos(pos);
	}

	glm::ivec2 text_view_t::get_pos()
	{
		glm::ivec2 ret;
		if (text_ed)
			ret = text_ed->get_nodepos();
		return ret;
	}
	void text_view_t::set_font_height(double fs)
	{
		assert(res_box);
		fs = ceil(fs);
		if (lt)
		{
			auto bal = lt->get_base_line(fs);
			res_box->base_line = bal;
			if (_editing)
				_editing->base_line = bal;
		}
		res_box->fontheight = fs;
		if (_editing)
		{
			_editing->fontheight = fs;
		}
	}
	void text_view_t::set_row_span(double row_span, bool autobr)
	{
		auto rs = (row_span > 1) ? row_span : ceil(res_box->fontheight * row_span);
		str_rc->set_view_width(autobr ? _clip.z : -1, rs);
		res_box->row_span = rs;
		res_box->autobr = autobr;
		if (_editing)
		{
			_editing->row_span = rs;
		}
	}
	void text_view_t::set_size(const glm::ivec2& s)
	{
		res_box->_size = s;
		uphot();

		str_rc->set_view_width(res_box->autobr ? _clip.z : -1, res_box->row_span);

		if (str_rc)
		{
			str_rc->_ev_size = { _clip.z, _clip.w };
		}
	}
	//void text_view_t::set_color(uint32_t border, uint32_t background)
	//{
	//	bcc.background = background;
	//	bcc.border_color = border;
	//}
	void text_view_t::set_text_color(uint32_t tc, uint32_t sc, uint32_t cursor_color)
	{
		trt::set_color(res_box, tc, sc, cursor_color);
	}

	double text_view_t::row_size()
	{
		return  res_box->fontheight + res_box->row_span;
	}
	glm::ivec2 text_view_t::size()
	{
		return res_box->_size;
	}

	bool text_view_t::isinput()
	{
		return text_ed->is_input;
	}
	bool text_view_t::ishot()
	{
		return text_ed->ishot();
	}

	void text_view_t::set_visible(bool is)
	{
		text_ed->visible = is;
	}

	void text_view_t::uphot()
	{
		if (bcc.border_radius >= 0)
		{
			bcc.radius.x = bcc.radius.y = bcc.radius.z = bcc.radius.w = bcc.border_radius;
		}
		glm::vec4 v4 = { 0, 0, res_box->_size };
		v4.x += bcc.radius.x;
		v4.y += bcc.radius.y;
		v4.z -= bcc.radius.x + bcc.radius.z;
		v4.w -= bcc.radius.y + bcc.radius.w;
		text_ed->set_v4(0, v4, true);
		_clip = v4;
	}

	void text_view_t::update(cpg_time* t)
	{
		trt::update(t, res_box);
	}
	void text_view_t::draw(cpg_time* t, canvas_cx* canvas)
	{
		glm::vec4 clip = _clip;//
		auto b4 = glm::vec4{ get_pos(), size() };
		if (bcc.border_color || bcc.background)
			canvas->draw_rect(b4, bcc.border_color, bcc.background, bcc.radius, bcc.border);
		if (res_box)
		{
			clip.x += b4.x;
			clip.y += b4.y;
			// 裁剪
			clicp_x ccp(canvas, clip, true);
			trt::draw_ptext(t, canvas, res_box);
		}
	}


	void text_up_cb(edit_es_t* p, bool is_input, bool is_editing, text_view_t* evt)
	{
		//text_view_t* evt = this;
		if (evt->iec)return;
		if (is_input) {
			auto pw = evt->buf->get_row_str();

			if (evt->on_change)
			{
				evt->iec++;
				std::string pw1 = pw;
				evt->on_change(&pw1, evt->str);
				if (pw1 != pw)
				{
					pw1.swap(pw);
					evt->set_text(pw);
				}
				evt->iec--;
			}
			if (evt->_pwd)
			{
				size_t wn = pw.size();
				pw.clear();
				pw.reserve(wn * evt->_pwd->size());
				for (size_t i = 0; i < wn; i++)
				{
					pw.append(*(evt->_pwd));
				}
			}
			evt->str = pw;
			evt->res_box->s.str = evt->str.c_str();
			evt->res_box->s.count = get_utf8_count(evt->res_box->s.str, evt->str.size());
			//print_time ftpt("do_text");
			evt->lt->do_text(evt->res_box);
			auto ep = evt->res_box;
			mk_colors(evt->res_box);
		}
		if (is_editing) {
			//print_time ftpt("do_text res_editing");
			evt->lt->do_text(evt->_editing);
			auto ep = evt->_editing;
			//auto rs = (ep->row_span > 1) ? ep->row_span : ceil(ep->fontheight * ep->row_span);
			//ep->row_span = rs;
			//evt->_editing.base_line = evt->res_box->base_line;
		}
	}

#if 1
	template<class T>
	double dcscroll(double cox, double isx, double scroll_increment_x, T& scrollx)
	{
		double ret = 0.0;
		if (cox < scrollx)
		{
			ret = floor(std::max(0.0, cox - scroll_increment_x));
			scrollx = ret;
		}
		else if (cox - isx >= scrollx)
		{
			ret = floor(cox - isx + scroll_increment_x);
			scrollx = ret;
		}
		return ret;
	}

	template <typename T>
	inline T clamp(T v, T mn, T mx) { return (v < mn) ? mn : (v > mx) ? mx : v; }
	// 输入：视图大小、内容大小、滚动宽度
	// 输出：x水平大小，y垂直大小，z为水平的滚动区域宽，w垂直滚动高
	glm::vec4 calcu_scrollbar_size(const glm::vec2 vps, const glm::vec2 content_width, const glm::vec2 scroll_width, const glm::ivec4& count)
	{
		//scrollbar_size_v是实际大小
		auto scw = scroll_width;
		scw.x = scroll_width.x * count.x + count.z;
		scw.y = scroll_width.y * count.y + count.w;
		auto dif = vps - scw;
		bool isx = (dif.x < content_width.x) && vps.x < content_width.x;
		bool isy = (dif.y < content_width.y) && vps.y < content_width.y;
		int inc = (isx ? 1 : 0) + (isy ? 1 : 0);
		if (!isy)
			scw.x -= count.z;
		if (!isx)
			scw.y -= count.w;
		auto calc_cb = [](double vw, double cw, double scw, double sw)
			{
				double win_size_contents_v = cw,
					win_size_avail_v = vw - scw,
					scrollbar_size_v = win_size_avail_v;
				auto win_size_v = std::max(std::max(win_size_contents_v, win_size_avail_v), 1.0);
				auto GrabMinSize = sw;			// std::clamp
				auto grab_h_pixels = clamp(scrollbar_size_v * (win_size_avail_v / win_size_v), GrabMinSize, scrollbar_size_v);
				auto grab_h_norm = grab_h_pixels / scrollbar_size_v;
				return glm::vec2{ grab_h_pixels, scrollbar_size_v };
			};
		auto x = calc_cb(vps.x, content_width.x, scw.x, scroll_width.x);
		auto y = calc_cb(vps.y, content_width.y, scw.y, scroll_width.y);
		if (!(x.x < x.y) || !isx)
			x.x = 0;
		if (!(y.x < y.y) || !isy)
			y.x = 0;
		glm::vec4 ret = { x.x, y.x, x.y, y.y };
		return ret;
	}


#endif


	// todo flex
#if 1

	namespace flex {
#define NO_MPTR
#define EMIT_ALIGN
# ifdef FATE
#undef FATE
# endif
		njson get_value(flex_item* it, attribute_func* cbs)
		{
			njson v;
#define FATE(name, type, def) v[#name]=cbs->item_get_##name(it);
#include <view/flex_attribute.h>
#undef FATE
			return v;
		}
		void set_value(flex_item* it, const njson& v, attribute_func* cbs)
		{
#define FATE(name, type, def) if(v.find(#name)!=v.end())cbs->item_set_##name(it, v[#name]);
#include <view/flex_attribute.h>
#undef FATE
			if (v.find("padding") != v.end())
			{
				glm::vec4 v4 = toVec4(v["padding"]);
				cbs->item_set_padding_left(it, v4.x);
				cbs->item_set_padding_top(it, v4.y);
				cbs->item_set_padding_right(it, v4.z);
				cbs->item_set_padding_bottom(it, v4.w);
			}
			if (v.find("margin") != v.end())
			{
				glm::vec4 v4 = toVec4(v["margin"]);
				cbs->item_set_margin_left(it, v4.x);
				cbs->item_set_margin_top(it, v4.y);
				cbs->item_set_margin_right(it, v4.z);
				cbs->item_set_margin_bottom(it, v4.w);
			}
			if (v.find("rect") != v.end())
			{
				glm::vec4 v4 = toVec4(v["rect"]);
				cbs->item_set_left(it, v4.x);
				cbs->item_set_top(it, v4.y);
				cbs->item_set_right(it, v4.z);
				cbs->item_set_bottom(it, v4.w);
			}
			if (v.find("pos") != v.end())
			{
				glm::vec2 v2 = toVec4(v["pos"]);
				cbs->item_set_left(it, v2.x);
				cbs->item_set_top(it, v2.y);
			}
			//if (v.find("size") != v.end())
			//{
			//	glm::vec2 v2 = toVec4(v["size"]);
			//	cbs->item_set_width(it, v2.x);
			//	cbs->item_set_height(it, v2.y);
			//}
			//printf("%s\n", v.dump().c_str());
			return;
		}
	}

	easy_flex::easy_flex()
	{
	}

	easy_flex::~easy_flex()
	{
	}

	void easy_flex::set_view(const glm::vec2& size)
	{
		//if (_data.empty())
		{
			_data.resize(1);
		}
		_data[0].width = size.x;
		_data[0].height = size.y;
	}

	void easy_flex::push(const glm::vec2& size)
	{
		_data[0].n++;
		_data.push_back({ size.x, size.y, 0 });
	}
	void easy_flex::push2(const glm::vec2& size, const glm::vec2& size1)
	{
		_data[0].n++;
		_data.push_back({ size.x, size.y, 1 });
		_data.push_back({ size1.x, size1.y, 0 });
	}
	void easy_flex::pop()
	{
		if (_data.size() > 1)
		{
			_data[0].n--;
			_data.pop_back();
		}
	}
	void easy_flex::erase(size_t idx)
	{
		if (_data.size() > 1 && idx > 0 && idx < _data.size())
		{
			_data[0].n--;
			_data.erase(_data.begin() + idx);
		}
	}
	void easy_flex::init()
	{
		flex_item* rt = fc.set_layout(_data.data(), _data.size(), 0);
	}

	void easy_flex::set_it_style(size_t idx, const njson& st)
	{
		if (idx < _data.size() && _data[idx].p)
		{
			assert(_data[idx].p && fc.atcb);
			flex::set_value(_data[idx].p, st, fc.atcb);
		}
	}
	void easy_flex::layout()
	{
		fc.do_layout();
	}
	size_t easy_flex::size() {
		return _data.size();
	}
	glm::vec4 easy_flex::get_merge(size_t idx)
	{
		glm::vec4 ret;
		if (idx < _data.size() && _data[idx].p)
		{
			assert(_data[idx].p && fc.atcb);
			auto pos1 = fc.get_item_merge_pos(_data[idx].p);
			auto size = fc.get_item_size(_data[idx].p);
			ret = { pos1, size };
		}
		return ret;
	}

	bool easy_flex::get_isabs(size_t idx)
	{
		bool ret = false;
		if (idx < _data.size() && _data[idx].p)
		{
			assert(_data[idx].p && fc.atcb);
			ret = fc.atcb->item_get_position(_data[idx].p);
		}
		return ret;
	}

	// 输入容器大小、子项大小、样式、输出子项坐标大小
	void easy_flex::build(layout_new_info_t* p, std::function<void(int idx, const glm::vec4& rc, bool isabs)> cb)
	{
		easy_flex dv;
		auto v = p->root_css;
		glm::vec4 v4 = toVec4(v["padding1"], 0);
		//glm::vec2 pad2 = toVec2(v["padding2"], 0);
		auto size = p->size;
		if ((size.x * size.y) < 1)
		{
			size = toVec2(v["size"]);
		}
		// padding1用于根div内边距扩展
		if ((v4.x * v4.y * v4.z * v4.w) > 0)
		{
			size.x -= v4.x + v4.z;
			size.y -= v4.y + v4.w;
		}
		dv.set_view(size);
		glm::vec2 cs;
		char* t = (char*)p->v2ptr;
		double mx = 0;
		for (int i = 0; i < p->n; i++)
		{
			glm::vec2 pc = *(glm::vec2*)t;
			//pc += pad2;
			dv.push(pc);
			mx += pc.x;
			t += p->v2offset;
		}
		int np = mx / p->n;
		bool auto_left = toBool(v["auto_left"]);
		if (auto_left && cs.x > 0 && abs(cs.x - np) < p->aw)
		{
			int cn = size.x / cs.x;
			if (cn > 0)
			{
				int cdn = cn - (p->n % cn);	// 补空位数量
				if (p->n <= cn)cdn = 0;
				cs.y = 0;
				for (int i = 0; i < cdn; i++) dv.push(cs);
			}
		}
		dv.init();
		dv.set_it_style(0, v);
		auto& st2 = p->child_css;
		do
		{
			if (st2.empty())break;
			if (st2.is_array())
			{
				auto n = std::min(dv.size() - 1, st2.size());
				for (size_t i = 0; i < n; i++)
				{
					dv.set_it_style(i + 1, st2[i]);
				}
			}
			else if (st2.is_object()) {
				auto dvn = dv.size() - 1;
				for (size_t i = 0; i < dvn; i++)
				{
					dv.set_it_style(i + 1, st2);
				}
			}
		} while (0);
		dv.layout();
		if (cb)
		{
			for (size_t i = 0; i < p->n; i++)
			{
				auto rc = dv.get_merge(i + 1);
				rc.x += v4.x;
				rc.y += v4.y;
				cb(i, rc, dv.get_isabs(i + 1));
			}
		}

	}


#endif // 1

	geo_ac::geo_ac()
	{

	}

	geo_ac::~geo_ac()
	{

	}

	void geo_ac::clear()
	{
		for (auto p : _data)
		{
			switch (p->type)
			{
			case gte::e_text:
			{
				auto t = (text_c*)p;
				free_geo(t->rbox);
				free_geo(t);
			}
			break;
			case gte::e_path:
			{
				free_geo((path_e*)p);
			}
			break;//				case gte::e_rect:				case gte::e_circle:				case gte::e_triangle:
			default:
				break;
			}
		}
	}

	namespace ui {

		//void parse_svg_path(const t_string& str, float scale, Image* img, t_vector<Path>* path)
		//{
		//	//svg a;
		//	//a.load_file("garc.svg");
		//	//a.load_file_svg("gcheckbox.svg");
		//	//a.load_file(R"(E:\code\c\libsvgtiny\test\data\tiger.svg)");
		//	//a.scale = scale;
		//	//a.load_mem(str.c_str(), str.size(), img, path);
		//	/*
		//	auto n = xj::xml2json(str.c_str());njson pathn;
		//	do
		//	{
		//		if (n.is_object() && n.end() != n.find("path")) {
		//			pathn = n["path"]["@d"];
		//			if (pathn.empty())break;
		//		}
		//		auto ts = toStr(pathn);
		//		auto t = ts.c_str();
		//		auto t1 = t + ts.size() - 1;
		//		char c = 0;
		//		for (; *t; t++)
		//		{
		//			if (isalpha(*t))
		//			{
		//				c = *t;
		//				continue;
		//			}

		//		}

		//	} while (0);*/
		//}



		const char* group_rcb_u::checkbox_u::c_str()
		{
			return _label.title.c_str();
		}
		const char* group_rcb_u::checkbox_u::v_str()
		{
			return v ? "true" : "false";
		}

		group_rcb_u::group_rcb_u(bool isradio) :is_radio(isradio)
		{
		}

		group_rcb_u::~group_rcb_u()
		{
			clear();
		}
		void group_rcb_u::clear()
		{
			if (binfo.reg)
			{
				ecs_at::free_em(binfo.reg, ety.data(), ety.size());
			}
			child.clear();
			ety.clear();
		}
		group_rcb_u::checkbox_u* group_rcb_u::get_ptr()
		{
			return child.data();
		}
		void group_rcb_u::init(entt::registry* reg, event_data_cx* listen, layout_text* lt, font_style_c* ust)
		{
			binfo._listen = listen;
			binfo._lt = lt;
			binfo._ust = ust;
			if (reg)
			{
				binfo.reg = reg;
			}
		}
		void group_rcb_u::set_title(const t_vector<t_string>& tv)
		{
			auto n = tv.size();
			if (n)
			{
				resize(0, n);
				auto p = child.data();
				int wh = 0;
				for (size_t i = 0; i < n; i++)
				{
					auto it = &p[i];
					auto st = &(it->_label);
					st->set_style(binfo._ust);
					// todo text
					st->update(binfo._lt, tv[i], 0);
					int w = p[i].square_sz + st->box_size.x + bt_space;
					if (w > wh)wh = w;
				}
				for (size_t i = 0; i < n; i++)
				{
					p[i].size = { wh, p[i].square_sz };
				}
			}
		}


		group_rcb_u::checkbox_u* group_rcb_u::resize(int w, int n)
		{
			checkbox_u* p = child.data();
			auto reg = binfo.reg;
			if (n > 0 && n != child.size())
			{
				clear();
				child.resize(n);
				ety.resize(n);
				p = child.data();
				auto t = p + 1;
				if (reg)
				{
					printf("%d\n", n);
					ecs_at::new_em(reg, ety.data(), n);
				}

				for (size_t i = 0; i < n; i++)
				{
					p[i].e = ety[i];
					p[i].id = i;
					p[i].is_radio = is_radio;
					if (w > 0)
						p[i].square_sz = w;
					p[i].size = { p[i].square_sz, p[i].square_sz };
					if (i + 1 == n)t = p;
					p[i].g_next = t++;
					if (reg)
					{
						//ecs_at::set_visible(reg, ety[i], true);
						// 注册单击回调函数
						click_cb_et* cb1 = 0;
						auto rcb = ecs_at::get_ptr(reg, ety[i], cb1);
						if (cb1)
						{
							cb1->ptr = &p[i];
							cb1->cb = HZ_CB_3(&group_rcb_u::onclick_g, this);
						}
					}
				}
			}

			return p;
		}
		void group_rcb_u::set_size_pos(const glm::vec2& s, const glm::vec2& pos)
		{
			_pos = pos;
			_size = s;
			auto n = child.size();
			if (n)
			{
				//	FLEX_ALIGN_SPACE_BETWEEN,	//两端对齐，两端间隔0，中间间隔1
				//	FLEX_ALIGN_SPACE_AROUND,	//分散居中,两端间隔0.5，中间间隔1
				//	FLEX_ALIGN_SPACE_EVENLY,	//分散居中,每个间隔1
				njson st;
				st["justify_content"] = FLEX_ALIGN_SPACE_AROUND;	// 对齐
				//st["justify_content"] = FLEX_ALIGN_START;			// 左对齐
				st["align_content"] = FLEX_ALIGN_SPACE_EVENLY;
				//st["direction"] = FLEX_DIRECTION_COLUMN;
				st["direction"] = FLEX_DIRECTION_ROW;

				st["auto_left"] = true;
				{
					layout_new_info_t np = {};
					np.size = _size;
					np.root_css = st;
					np.v2offset = sizeof(child[0]);
					np.v2ptr = ((char*)child.data()) + HZ_OFFSETOF(checkbox_u, size);
					np.n = child.size();
					//np.auto_left = true;
					//np.child_css;
					easy_flex::build(&np, [=](int idx, const glm::vec4& rc, bool is)
						{
							auto& c = child[idx];
							c.pos = { rc.x, rc.y };
							hot_et* hp = 0;
							ecs_at::get_ptr(binfo.reg, c.e, hp);
							hp->rc = { 0, 0, c.size };
							hp->pos = c.pos + _pos;
						});
				}


			}
		}
		void group_rcb_u::onclick_g(void* ptr, int idx, int count)
		{
			if (!ptr)return;
			auto c = (checkbox_u*)ptr;
			if (c->is_radio)
			{
				set_radio(c);
			}
			else {
				set_check(c, !c->v);
			}
			// 执行用户回调函数
			if (on_click)on_click(c);
		}
		void group_rcb_u::set_pos(const glm::vec2& pos)
		{
			_pos = pos;
			//if (idx >= 0 && idx < child.size())
			//{
			//	auto& c = child[idx];
			//}
		}
		void group_rcb_u::set_radio(int idx)
		{
			if (idx >= 0 && idx < child.size())
			{
				set_radio(&child[idx]);
			}
		}
		void group_rcb_u::set_radio(checkbox_u* p)
		{
			auto t = p->g_next;
			p->v = true;
			do
			{
				if (!t || t == p)break;
				t->v = false;
				t = t->g_next;
			} while (true);
		}
		void group_rcb_u::set_check(checkbox_u* p, bool is)
		{
			if (p)
			{
				p->v = is;
			}
		}
		void group_rcb_u::set_check(int idx, bool is)
		{
			if (idx >= 0 && idx < child.size())
			{
				child[idx].v = is;
			}
		}
		void group_rcb_u::update_draw(cpg_time* t, canvas_cx* canvas)
		{
			auto p = child.data();
			auto cs = child.size();
			auto pos = _pos; pos += binfo._listen->get_nodepos();
			canvas->draw_rect({ pos, _size }, 0x80ff9e40, 0x20000000, { 6, 6, 6, 6 }, 1);
			for (int i = 0; i < cs; i++)
			{
				draw_checkbox(canvas, p++);
			}
		}
		void render_close(canvas_cx* canvas, glm::vec2 pos, glm::ivec4 col, float sz)
		{
			float radius = sz * 0.5;
			auto center = pos + glm::vec2(radius, radius);
			unsigned int col_hovered = col.y ? col.y : col.w;
			unsigned int cross_col = col.z;
			float t = 2.0;
			if (radius < 2)radius = 2;
			canvas->draw_circle(center, radius, 0, col.x, 16, 0);

			canvas->draw_circle(center, radius, 0, col_hovered, 16, 0);

			float cross_extent = sz * 0.5f * 0.7071f - 1.0f;

			center -= glm::vec2(0.5f, 0.5f);
			canvas->draw_line(center + glm::vec2(+cross_extent, +cross_extent), center + glm::vec2(-cross_extent, -cross_extent), cross_col, t);
			canvas->draw_line(center + glm::vec2(+cross_extent, -cross_extent), center + glm::vec2(-cross_extent, +cross_extent), cross_col, t);

		}
		void group_rcb_u::renderCheckMark(canvas_cx* canvas, glm::vec2 pos, uint32_t col, float sz)
		{
			float thickness = std::max(sz / 5.0f, 1.0f);
			sz -= thickness * 0.5f;
			pos += glm::vec2(thickness * 0.25f, thickness * 0.25f);

			float third = sz / 3.0f;
			float bx = pos.x + third;
			float by = pos.y + sz - third * 0.5f;
			auto path = canvas->get_cpath();
			path->line_to(glm::vec2(bx - third, by - third));
			path->line_to(glm::vec2(bx, by));
			path->line_to(glm::vec2(bx + third * 2.0f, by - third * 2.0f));
			canvas->PathStroke(col, false, thickness);
		}

		void group_rcb_u::draw_checkbox(canvas_cx* canvas, checkbox_u* c)
		{
			int is_hover = binfo.reg->has<hover_et>(c->e);
			int is_down = binfo.reg->has<down_et>(c->e);
			c->hover = is_hover;
			uint32_t hc = 0;

			if (is_down)
			{
				hc = c->down_col;
			}
			auto pos = c->pos + _pos; pos += binfo._listen->get_nodepos();
			auto k = c->square_sz - c->_label.mheight;

			if (c->is_radio)
			{
				float radius = (c->square_sz) * 0.5f;
				pos += radius;
				canvas->draw_circle(pos, radius, 0, c->fill_col, 16, 0);
				if (hc)
					canvas->draw_circle(pos, radius, 0, hc, 16, 0);
				if (is_hover)
					canvas->draw_circle(pos, radius, 0, c->hover_col, 16, 0);
				if (c->v)
				{
					const float pad = std::max(1.0f, floor(c->square_sz / 4.0f));
					canvas->draw_circle(pos, radius - pad, 0, c->check_col, 16, 0);
				}
				pos -= radius;
			}
			else {
				bool isclose = false;
				if (isclose)
				{
					render_close(canvas, pos, glm::ivec4(is_hover ? c->fill_col : 0, hc, c->check_col, c->hover_col * 0), c->square_sz);
				}
				else
				{
					canvas->draw_rect({ pos.x, pos.y, c->square_sz, c->square_sz }, 0, c->fill_col, c->radius, 0);
					if (hc)
						canvas->draw_rect({ pos.x, pos.y, c->square_sz, c->square_sz }, 0, hc, c->radius, 0);
					if (is_hover)
						canvas->draw_rect({ pos.x, pos.y, c->square_sz, c->square_sz }, 0, c->hover_col, c->radius, 0);
					if (c->v)
					{
						const float pad = std::max(1.0f, floor(c->square_sz / 6.0f));
						renderCheckMark(canvas, pos + glm::vec2(pad, pad), c->check_col, c->square_sz - pad * 2.0f);
					}
				}
			}
			pos.x += bt_space + c->square_sz;

			//auto str = u8_gbk(c->c_str());
			//if (c->_label.base_line > c->_label.mheight)
			if (k >= 0)
				k -= (c->_label.base_line - c->_label.base_line_b);
			pos.y += k;
			canvas->draw_text(&c->_label, pos, _text_color);
		}
#if 1

		group_btn_u::group_btn_u()
		{
		}

		group_btn_u::~group_btn_u()
		{
		}

		void group_btn_u::init(entt::registry* reg, event_data_cx* listen, gtac_c* pt, int font_height)
		{
			binfo._listen = listen;
			binfo.ptm = pt;
			binfo.font_height = font_height;
			if (reg)
			{
				binfo.reg = reg;
			}
		}
		void group_btn_u::set_title(const t_vector<t_string>& tv)
		{
			auto n = tv.size();
			if (n)
			{
				resize(0, n);
				auto p = child.data();
				int wh = 0;
				int maxwidth = 0;
				for (size_t i = 0; i < n; i++)
				{
					auto it = &p[i];
					binfo.ptm->set_fontheight({ 0.5,0.5 }, binfo.font_height);
					auto& str = tv[i];
					glm::ivec2 box = { abs(c_size.x), abs(c_size.y) };
					auto pd = binfo.ptm->mk_text(str.c_str(), str.size(), box, p->dtc);
					if (pd && pd != p->dtc)
					{
						p->dtc = pd;
						p->dtc->color = p->text_color;
					}
					if (box.x > maxwidth) {
						maxwidth = box.x;
					}
					float cw = c_size.x < 0 ? box.x - c_size.x : c_size.x;
					float ch = c_size.y < 0 ? box.y - c_size.y : c_size.y;
					p[i].size = { cw,ch };
				}

				//	FLEX_ALIGN_SPACE_BETWEEN,	//两端对齐，两端间隔0，中间间隔1
				//	FLEX_ALIGN_SPACE_AROUND,	//分散居中,两端间隔0.5，中间间隔1
				//	FLEX_ALIGN_SPACE_EVENLY,	//分散居中,每个间隔1
				njson st;
				st["justify_content"] = FLEX_ALIGN_SPACE_EVENLY;	// 水平方向
				//st["justify_content"] = FLEX_ALIGN_START;			// 水平左对齐
				st["align_content"] = FLEX_ALIGN_SPACE_EVENLY;		// 垂直方向
				//st["direction"] = FLEX_DIRECTION_COLUMN;
				st["direction"] = FLEX_DIRECTION_ROW;
				st["auto_left"] = true;
				{
					layout_new_info_t np = {};
					np.size = _size;
					np.root_css = st;
					np.v2offset = sizeof(child[0]);
					np.v2ptr = ((char*)child.data()) + HZ_OFFSETOF(btn_info_u, size);
					np.n = child.size();
					//np.auto_left = true;
					//np.child_css;
					easy_flex::build(&np, [=](int idx, const glm::vec4& rc, bool is)
						{
							auto& c = child[idx];
							c.pos = { rc.x, rc.y };
							hot_et* hp = 0;
							ecs_at::get_ptr(binfo.reg, c.e, hp);
							hp->rc = { 0, 0, c.size };
							hp->pos = c.pos + _pos;
						});
				}
			}

		}


		btn_info_u* group_btn_u::resize(int w, int n)
		{
			btn_info_u* p = child.data();
			auto reg = binfo.reg;
			if (n > 0 && n != child.size())
			{
				clear();
				child.resize(n);
				ety.resize(n);
				p = child.data();
				auto t = p + 1;
				if (reg)
				{
					printf("%d\n", n);
					ecs_at::new_em(reg, ety.data(), n);
				}

				for (size_t i = 0; i < n; i++)
				{
					p[i].e = ety[i];
					button_init(&p[i], { 10,10,10,10 }, "", color);
					if (reg)
					{
						//ecs_at::set_visible(reg, ety[i], true);
						// 注册单击回调函数
						click_cb_et* cb1 = 0;
						auto rcb = ecs_at::get_ptr(reg, ety[i], cb1);
						if (cb1)
						{
							cb1->ptr = &p[i];
							cb1->cb = HZ_CB_3(&group_btn_u::onclick_g, this);
						}
					}
				}
			}
			return nullptr;
		}

		// 更新绘图
		void group_btn_u::update_draw(cpg_time* t, canvas_cx* canvas)
		{
			auto p = child.data();
			auto cs = child.size();
			auto pos = _pos; pos += binfo._listen->get_nodepos();
			canvas->draw_rect({ pos, _size }, 0x80ff9e40, 0x20000000, { 6, 6, 6, 6 }, 1);
			for (int i = 0; i < cs; i++)
			{
				update_btn(p, pos);
				gui_draw_btn(canvas, p++);
			}
		}

		void group_btn_u::clear()
		{
			if (binfo.reg)
			{
				ecs_at::free_em(binfo.reg, ety.data(), ety.size());
			}
			child.clear();
			ety.clear();
		}
		void group_btn_u::set_size(const glm::vec2& s)
		{
			_size = s;
		}
		void group_btn_u::set_pos(const glm::vec2& pos)
		{
			_pos = pos;
		}
		void group_btn_u::update_btn(btn_info_u* p, const glm::vec2& pos)
		{
			if (!p)return;
			p->pos1 = pos;
			int& bs = p->bs;
			if (!binfo.reg->has<down_et>(p->e))
			{
				bs &= ~(int)BTN_STATE::STATE_ACTIVE;
			}
			if (binfo.reg->has<hover_et>(p->e)) {

				bs = (int)BTN_STATE::STATE_NOMAL;
				if (!(bs & (int)BTN_STATE::STATE_ACTIVE) && p->mEnabled)
				{
					bs = (int)BTN_STATE::STATE_HOVER;
					if (binfo.reg->has<down_et>(p->e))
					{
						bs = (int)BTN_STATE::STATE_ACTIVE;
					}
				}
			}
			else
			{
				if (bs != (int)BTN_STATE::STATE_NOMAL)
				{
					if (!(bs & (int)BTN_STATE::STATE_ACTIVE) || (bs & (int)BTN_STATE::STATE_HOVER))
					{
						bs = (int)BTN_STATE::STATE_NOMAL;
						printf("leave:%p\n", p);
					}
				}
			}
			btn_set_state(p, (int)bs);
		}
		void group_btn_u::onclick_g(void* ptr, int idx, int count)
		{
			if (!ptr)return;
			auto c = (btn_info_u*)ptr;
			// 执行用户回调函数
			if (on_click && c->mEnabled)on_click(c);
		}


		charts_u1::charts_u1()
		{
		}

		charts_u1::~charts_u1()
		{
		}
		bool charts_u1::is_num(const char* s)
		{
			bool ret = true;
			for (; *s; s++)
			{
				if ((*s < '0' || *s>'9'))
				{
					if (!(*s == '.' || *s == '-'))
					{
						ret = false;
						break;
					}
				}
			}
			return ret;
		}
		// 绘制单个glm::vec4 v;			// 开盘	    最高	    最低	    收盘
		// pos;			// 中线坐标,
		void draw_candlestick(canvas_cx* canvas, glm::vec2 pos, glm::vec4 v, candlestick_css* ct)
		{
			pos.y += ct->height;
			auto c = v.x < v.w ? ct->color.x : ct->color.y;
			glm::vec2 a[2] = { {pos.x, pos.y - v.x}, {pos.x, pos.y - v.w} };
			glm::vec2 a1 = pos, a2 = pos;
			a1.y -= v.y; a2.y -= v.z;
			auto cy = ct->thickness.y - abs(a[0].y - a[1].y);
			if (cy > 0)
			{
				a[(v.x < v.w) ? 0 : 1].y += cy;
			}
			canvas->draw_line(a1, a2, c, ct->thickness.y, false);
			canvas->draw_line(a[0], a[1], c, ct->thickness.x, false);
		}
		void charts_u1::set_rect(const glm::vec4& r)
		{
			_pos = { r.x, r.y };
			_size = { r.z, r.w };
			_ct.height = _size.y - _ct.thickness.x;
		}
		double cdif(glm::vec2 src, glm::vec2 dst) {
			return  (dst.x - dst.y) / (src.x - src.y);
		}
		double between0_1(double x, glm::vec2 m, double dif) {
			return m.y + dif * (x - m.x);
		}
		// 映射到范围
		void between0_1(glm::vec4& xx, glm::vec2 m, double dif) {
			xx.x = m.y + dif * (xx.x - m.x);
			xx.y = m.y + dif * (xx.y - m.x);
			xx.z = m.y + dif * (xx.z - m.x);
			xx.w = m.y + dif * (xx.w - m.x);
		}
		/*
			// {数据最大值，最小值} ， {显示范围高度、最低点}
			auto dif = cdif({ ma, mi }, { _ct.height, _ct.thickness.x });
			// 归一化vec4
			for (size_t i = 0; i < n; i++)
			{
				between0_1(v[i], { mi, _ct.thickness.x }, dif);
			}

		*/


		// 计算MA均线
		void calculateMA(int dayCount, glm::vec4* v, int count, float space, float ic, t_vector<glm::vec2>& result)
		{
			result.clear();
			int rc = count / dayCount;
			result.reserve(rc);
			double sp = 0;
			double sum = 0;
			for (int i = 0; i < dayCount; i++) {
				sum += v[i].w;// +收盘价
				sp += space;
			}
			result.push_back({ sp - space, ic - sum / dayCount });
			for (int i = dayCount; i < count; i++, sp += space) {
				sum += v[i].w;
				sum -= v[i - dayCount].w;
				result.push_back({ sp, ic - sum / dayCount });
			}
			return;
		}
		void charts_u1::set_data(glm::vec4* v, int n, glm::ivec4 idx)
		{
			glm::ivec4 defidx = { 0, 1, 2, 3 };
			kdata.resize(n);
			memcpy(kdata.data(), v, n * sizeof(glm::vec4));
			v = kdata.data();
			float mi = v->z, ma = v->y;
			if (idx != defidx)
			{
				for (size_t i = 0; i < n; i++)
				{
					float* p = (float*)&v[i];
					glm::vec4 tv4;
					tv4.x = p[idx.x];
					tv4.y = p[idx.y];
					tv4.z = p[idx.z];
					tv4.w = p[idx.w];
					*((glm::vec4*)p) = tv4;
					if (mi > v[i].z)mi = v[i].z;
					if (ma < v[i].y)ma = v[i].y;
				}
			}
			else {
				for (size_t i = 0; i < n; i++)
				{
					if (mi > v[i].z)mi = v[i].z;
					if (ma < v[i].y)ma = v[i].y;
				}
			}
			// 数据最大值，最小值 ， 显示范围高度、最低点
			auto dif = cdif({ ma, mi }, { _ct.height, _ct.thickness.x });
			// 归一化
			for (size_t i = 0; i < n; i++)
			{
				between0_1(v[i], { mi, _ct.thickness.x }, dif);
			}
			std::array<int, 4> mas = { 5, 10, 20, 30 };
			for (auto it : mas) {
				make_ma(it);
			}
			printf("min max:%f\t%f\n", mi, ma);
		}
		void charts_u1::make_ma(int n)
		{
			calculateMA(n, kdata.data(), kdata.size(), _ct.space + _ct.thickness.x, _ct.height, ma[n]);
		}
		void charts_u1::update_draw(cpg_time* t, canvas_cx* canvas)
		{
			auto pos = _pos;
			pos += _ct.thickness.x;

			canvas->draw_rect({ _pos, _size }, 0x80ff9e40, 0x20000000, { 4, 4, 4, 4 }, 1);
			auto p = kdata.data();
			for (size_t i = 0; i < kdata.size(); i++)
			{
				auto it = p[i];
				draw_candlestick(canvas, pos, it, &_ct);
				pos.x += _ct.space + _ct.thickness.x;
			}
			int i = 0;
			pos = _pos;
			pos += _ct.thickness.x;
			for (auto& [k, v] : ma)
			{
				canvas->draw_polyline(pos, v.data(), v.size(), _ct.macol[i++], false, _ct.tks, true);
			}
		}


#endif // 1



		void gui_draw_btn(canvas_cx* g, btn_info_u* p)
		{
			float x = p->pos.x + p->pos1.x,
				y = p->pos.y + p->pos1.y,
				w = p->size.x, h = p->size.y;
			int pushed = p->mPushed ? 0 : 1;
			auto label_pos = p->pos + p->pos1;
			unsigned int gradTop = p->gradTop;
			unsigned int gradBot = p->gradBot;
			unsigned int borderDark = p->borderDark;
			unsigned int borderLight = p->borderLight;
			double oa = p->opacity;
			auto ns = p->size;
			if (p->dtc)
				ns -= p->dtc->rc;
			ns *= 0.5;
			if (p->mPushed) {
				//ns.x += 1.0f; ns.y += 1.0f;
			}
			label_pos += ns;
			auto bc = set_alpha_xf(p->back_color, oa);
			double rounding = p->rounding;
			glm::vec2 ns1 = { w * 0.5, h * 0.5 };
			auto nr = (int)std::min(ns1.x, ns1.y);
			if (rounding > nr)
			{
				rounding = nr;
			}

			if (is_alpha(bc))
			{
				bc = set_alpha_f(bc, oa);
				g->draw_rect({ x + 1, y + 1, w - 1, h - 2 }, 0, bc, rounding);
				if (p->mPushed) {
					gradTop = set_alpha_f(gradTop, 0.8f);
					gradBot = set_alpha_f(gradBot, 0.8f);
				}
				else {
					double v = 1 - get_alpha_f(p->back_color);
					auto gv = p->mEnabled ? v : v * .5f + .5f;
					gradTop = set_alpha_f(gradTop, gv);
					gradBot = set_alpha_f(gradBot, gv);
				}
			}
			auto gt = to_c4(gradTop);
			auto gt1 = to_c4(gradBot);
			gradTop = set_alpha_xf(gradTop, oa);
			gradBot = set_alpha_xf(gradBot, oa);
			borderLight = set_alpha_xf(borderLight, oa);
			borderDark = set_alpha_xf(borderDark, oa);
			// 渐变
			glm::vec4 r;
			if (rounding > 0)
			{
				r = { rounding, rounding, rounding, rounding };
			}
			g->draw_rect_filled_multi_color({ x + 1, y + 1, w - 1, h - 2 }, gradTop, gradTop, gradBot, gradBot, false, r);
			//g->draw_rect_filled_multi_color({ x + 1, y + 1, w - 1, h - 2 }, 0xffff0000, 0xffff0000, 0xff00ff00, 0xff00ff00, false, r);
			// 渲染标签
			//g->draw_text(&p->_label, label_pos, p->text_color);
			if (p->dtc)
				g->draw_shape(p->dtc, 1, label_pos);
			// 边框
			g->draw_rect({ x + 0.5f, y + (p->mPushed ? 0.5f : 1.5f), w, h - (p->mPushed ? 0.0f : 1.0f) }, borderLight, 0, rounding);
			g->draw_rect({ x + 0.5f, y + 0.5f, w, h - 0.5 }, borderDark, 0, rounding);

		}


		const char* btn_info_u::c_str()
		{
			return str.c_str();
		}


		void button_init(btn_info_u* p, glm::ivec4 rect, const t_string& text, unsigned int back_color, unsigned int text_color)
		{
			auto& info = *p;
			info.pos = { rect.x, rect.y };
			info.size = { rect.z, rect.w };
			info.rounding = 2;
			info.back_color = back_color;
			info.text_color = text_color;
			info.opacity = 1;
			info.str = text.c_str();
			info.borderLight = 0xff5c5c5c;
			info.borderDark = 0xff1d1d1d;

			return;
		}
		void btn_set_state(btn_info_u* p, int bst)
		{
			if (!p)return;
			auto& info = *p;

			// (sta & hz::BTN_STATE::STATE_FOCUS)
			unsigned int gradTop = 0xff4a4a4a;
			unsigned int gradBot = 0xff3a3a3a;

			info.mPushed = (bst & (int)BTN_STATE::STATE_ACTIVE);
			info.mMouseFocus = (bst & (int)BTN_STATE::STATE_HOVER);
			if (bst & (int)BTN_STATE::STATE_DISABLE)
				info.mEnabled = false;
			if (info.mPushed) {
				gradTop = 0xff292929;
				gradBot = 0xff1d1d1d;
			}
			else if (info.mMouseFocus && info.mEnabled) {
				gradTop = 0xff404040;
				gradBot = 0xff303030;
			}
			info.gradTop = gradTop;
			info.gradBot = gradBot;
		}






		// todo input
		input_u::input_u()
		{
			_is_click = true;
			init(16, true);
		}

		input_u::~input_u()
		{
		}


		input_u::input_u(int font_height, bool single_line)
		{
			init(font_height, single_line);
		}



		std::string to_string1(double _Val) {
#ifdef _WIN32
			const auto _Len = static_cast<size_t>(_CSTD _scprintf("%g", _Val));
#else
			const size_t _Len = 128;
#endif
			std::string _Str(_Len, '\0');
			//_CSTD sprintf_s(&_Str[0], _Len + 1, "%g", _Val);
			snprintf(&_Str[0], _Len + 1, "%g", _Val);
			return _Str;
		}

		void input_u::init(int font_height, bool single_line)
		{
			_edit->bcc.border_color = _edit->bcc.background = 0;
			if (font_height > 5)
				_font_height = font_height;
			mk_rs(_stc);
			auto aka = init_text_view(_edit, ui_text_style::e_all, _stc, nullptr);
			int ey = 0;

			_edit->text_ed->pvisible = &iseditor;
			_edit->single_line = single_line;
			for (int i = 0; i < 1; i++)
			{
				_edit[i].set_pos({ 100, 100 + ey });
				_edit[i].set_font_height(_font_height);
				auto ss = glm::vec2({ 200, border + _edit[i].row_size() });
				set_size(ss);
				_edit[i].set_size(ss);
				ey += ss.y + 10;
				v2 = true;
			}
			text_event_t::set_ick_ibeam(_edit->ep, ick, is_ibeam);
			_edit->on_input = [=](std::string* s)
				{
					if (s->size())
					{
						// 删除非数字符号
						//if (_is_number || _is_float)
						//{
						//	auto& str = *s;
						//	str.erase(remove_if(str.begin(), str.end(), [](int c)
						//		{								
						//			return c > 0 && c < 255 && !(isdigit(c) || c == '-' || c == '.');
						//		}), str.end());
						//}
						if (on_input)
						{
							on_input(s);
						}
					}
				};

			_edit->on_change = [=](std::string* str, const std::string& old)
				{
					do {
						if (!(_is_number || _is_float))
						{
							if (upbd && _td)
							{
								if (_tk != "")
									(*_td)[_tk] = *str;
								else
									(*_td) = *str;
							}
						}
						call_vcb();
						if (on_change)
						{
							on_change(str, old);
							return;
						}
						if (str->empty() || !(_is_number || _is_float))break;
						std::string s;
						size_t nk = 0;
						try
						{
							int inc = -1;
							s = *str;
							if (_is_float)
							{
								// 删除非数字符号
								s.erase(remove_if(s.begin(), s.end(), [&inc](int c) {
									{
										if (c == '-')
										{
											if (inc & 1)
											{
												inc &= ~1;
												return false;
											}
											return true;
										}
										if (c == '.')
										{
											if (inc & 2)
											{
												inc &= ~2;
												return false;
											}
											return true;
										}
										return (c > 0 && c < 255 && !(isdigit(c)));
									}
									}), s.end());
								double d = 0;
								if (s != "-" && s != "." && s != "-.")
								{
									d = stod(s, &nk);
									s = to_string1(d);
									if (upbd && _td)
									{
										if (_tk != "")
											(*_td)[_tk] = d;
										else
											(*_td) = d;
									}
								}
							}
							else {
								// 删除非数字符号
								s.erase(remove_if(s.begin(), s.end(), [&inc](int c) {
									{
										if (c == '-')
										{
											if (inc < 0)
											{
												inc++;
												return false;
											}
											return true;
										}
										return c > 0 && c < 255 && !(isdigit(c));
									}
									}), s.end());
								int64_t d = 0;
								if (s != "-")
								{
									d = stoll(s, &nk);
									s = std::to_string(d);
								}

								if (upbd && _td)
								{
									if (_tk != "")
										(*_td)[_tk] = d;
									else
										(*_td) = d;
								}
							}
						}
						catch (const std::exception& e)
						{
							printf("%s\n", e.what());
							s = old;
						}
						*str = s;
						call_vcb();
					} while (0);
				};
		}
		void input_u::set_multi_line(bool is)
		{
			_edit->single_line = !is;
		}
		void input_u::set_bsize(const glm::ivec2& s)
		{
			set_size(s);
			_edit->set_size(s);
			v2 = true;
		}
		void input_u::set_rowcount(int n, int width, int height)
		{
			auto ss = get_size();
			if (width > 0)ss.x = width;
			if (n < 1)n = 1;
			{
				int b2 = border * 2;
				glm::ivec2 size1 = { ss.x < _font_height ? (_font_height + border) : ss.x, (b2 + _edit->row_size()) * n };
				//if (height > size1.y)
				//	size1.y = height * n + (b2 * n > 1 ? 1 : 0);
				if (height > 0)
				{
					size1.y = height;
				}
				set_size(size1);
				_edit->set_size(size1);
				v2 = true;
			}
		}
		void input_u::set_number(bool is, bool is_float)
		{
			_is_number = is;
			_is_float = is_float;
		}
		void input_u::set_layout(layout_text* lt)
		{
			if (lt)
				_edit->lt = lt;
			_edit->set_font_height(_font_height);
		}



		void input_u::set_color(uint32_t tc, uint32_t sc, uint32_t border_c, uint32_t background_c, uint32_t cursor_color)
		{
			dcss.font_color = tc;
			dcss.border_color = border_c;
			dcss.background_color = background_c;
			_edit->set_text_color(tc, sc, cursor_color);
		}

		void input_u::set_text(const std::string& str)
		{
			if (_str != str)
			{
				_str = str;
			}
			v2 = true;
		}
		//void input_u::set_text(const t_string& str)
		//{
		//	_edit->set_text(str);
		//}

		std::string input_u::get_text()
		{
			return _edit->c_str();
		}

		void input_u::set_placeholder(const std::string& str)
		{
			_tph = str; v2 = true;
		}

		const char* input_u::c_str()
		{
			return _edit->c_str();
		}

		size_t input_u::c_size()
		{
			return _edit->str.size();
		}

		void input_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			incedit++;
			//iseditor = true;
			glm::ivec2 spos = pos + pos1;
			auto ss = get_size();
			//auto strf = u8_gbk(_edit->c_str());
			//if (strf.size())
			//{
			//	strf.size();
			//}
			if (spos != ltpos)
			{
				auto tclip = canvas->get_topclip();
				bool isv = true;
				//if (tclip.z > 1 && tclip.w > 1)
				//{
				//	glm::vec4 a = { spos,ss };
				//	isv = (is_rect_intersect(tclip, a));
				//	//if (((spos.x + ss.x) < tclip.x) || ((spos.x) > (tclip.x + tclip.z)) || ((spos.y + ss.y) < tclip.y) || ((spos.y) > (tclip.y + tclip.w)))
				//	if (isv)
				//	{
				//	}
				//}
				_edit->set_visible(isv);
				ltpos = spos;
				_edit->set_pos(spos);
			}

			canvas->draw_rect(spos, ss, dcss.col, dcss.fill, rounding, thickness);

			_edit->draw(t, canvas);

		}

		void input_u::update(cpg_time* t)
		{
			if (!reg)return;
			if (_edit)
			{
				assert(reg);
				dcss.fill = dcss.background_color;
				dcss.col = dcss.border_color;
				if (_edit->isinput())
				{
					dcss.col = dcss.active_border_color;
				}
				else if (reg->has<hover_et>(e) || _edit->ishot())
				{
					dcss.col = dcss.hover_color;
				}
				if (has_mte(mouse_type_e::e_enter))
				{
					dec_mte(mouse_type_e::e_enter);
				}
			}
			if (v2)
			{
				if (no_em)
				{
					auto es = reg->get<event_src_et>(ety_null);
					if (es.p && es.lt)
					{
						_emp = es.p;
						no_em = false;
						set_layout(es.lt);
						text_event_t::set_ctx(_edit->ep, _emp->get_bwctx());
					}
				}
				//if (!_str.empty())
				{
					_edit->set_text(std::move(_str)); _str.clear();
				}
				up_entity();
			}
			set_event(iseditor);
			_edit->update(t);

			if (!dtc || _font_height != fontheight)
			{
				fontheight = _font_height;
				v2 = true;
			}
			if (v2)
			{
				v2 = false;
				if (_tph.size())
				{
					placeholder = _tph;
				}

				{
					if (ptm)
					{
						if (placeholder.size())
						{
							glm::ivec2 size1 = get_size();
							auto dt = set_dtc(placeholder.c_str(), placeholder.size(), size1, { 0.0,0.1 });
							if (dt)
							{
								dt->color = set_alpha_f(dcss.font_color, pha);
								glm::vec4 ps = _edit->_clip;
								tpos = glm::vec2(ps.x, ps.y);
								_tph.clear();
							}
						}

					}
				}
			}
			auto_drawtext = (_edit->isinput() || _edit->c_size()) ? false : true;
			base_u::update(t);
			// 用于判断是否渲染
			iseditor = (incedit > 0);
			incedit = 0;
		}

		void input_u::on_parent_visible(bool is)
		{
			if (_edit)
			{
				_edit->set_visible(is);
			}
		}

		void input_u::set_password(const char* pwd)
		{
			if (pwd && *pwd)
			{
				_password = pwd;
				_edit->_pwd = &_password;
			}
			else
			{
				_edit->_pwd = 0;
				_password.clear();
			}
		}

		void input_u::set_input_cursor(bool is)
		{
			is_ibeam = is;
			text_event_t::set_ick_ibeam(_edit->ep, ick, is_ibeam);
		}

		void input_u::set_read_only(bool is)
		{
			_read_only = is;
			text_event_t::set_read_only(_edit->ep, is);
		}
		void input_u::set_event(bool is)
		{
			auto o = get_root_order();
			if (old_order != o)
			{
				old_order = o;
				_edit->text_ed->order = o + 1;
			}
			if (is != _no_event)
			{
				_no_event = is;
				//text_event_t::set_no_event(_edit->ep, is);
				if (_emp)
				{
					//_edit->text_ed->order = edit_order;
					if (is)
					{
						_emp->push(_edit->text_ed);
					}
					else
					{
						_emp->pop(_edit->text_ed);
					}
				}
			}
		}

		bool input_u::is_input()
		{
			return _edit->isinput();
		}

		void input_u::bind_dt(njson* td, const char* n)
		{
			_td = td;
			if (n && *n)
				_tk = n;
		}



		// --------------------------------------------------------------------------------------------------

		switch_u::switch_u() {
			_is_click = true;
			base_u::set_size({ 60, 30 });
			path = new path_stb();
			//ec = new effect_cx();
		}

		switch_u::~switch_u() {
			delop(path);
			//delop(ec);
			delop(img_blur);
		}

		void switch_u::set_size(const glm::vec2& s, float r)
		{
			base_u::set_size(s);
			if (r > 0)
				radius = r;
			auto p = up_entity();
		}

		void switch_u::set_color(glm::ivec3 c)
		{
			color = c;
		}

		void switch_u::set_time(float t)
		{
			if (t > 0)
			{
				gt = t;

			}
		}

		void switch_u::set_shape(image_m_cs* t)
		{
			img2 = t;
		}

		void switch_u::set_shape(triangle_cs* t, glm::ivec4 mask)
		{
			tr2 = t;
			tr_mask = mask;
		}


		void switch_u::set_value(bool v)
		{
			value2 = v;
			bv.set_value(v);
		}

		void switch_u::set_value()
		{
			set_value(!value2);
		}

		void switch_u::bind_value(void* p, uint16_t set1, uint8_t size, uint8_t count)
		{
			bv.v = p;
			bv.b = set1;
			bv.size = size;
			bv.count = count;
		}
		void switch_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{

			glm::vec2 spos = pos1 + this->pos;
			auto ss = get_size();
			float cie = ss.y * 0.5;
			auto s = ss;
			s *= 1;// bs.spread;
			s += blur * 3;
			auto s1 = s;
			s1 -= blur;
			style_cs st = {};
			//path_cs scs = {};
			st.closed = true;
			//scs.path = path;
			//scs.t = &st;
			st.color = -1;
			glm::vec2 dif = { .0,100 };
			//canvas->draw_shape(shape_base::path, (shape1_cs*)&scs, 1, spos + dif);
			dif.y += 20;
			//canvas->draw_rect(spos, ss, 0x800080ff, 0, cie, 1.0f);
			auto ipos = spos;
			ipos += (ss * 0.5f);
			image_cs rst = {};
			rst.img = img_blur;
			if (img_blur)
			{
				rst.pos = s1 * -0.5f;// { -0.5 * s1, -0.5 * img->height };
				rst.rect = { 4,4, s1 };
				rst.size = s1;// {img->width, img->height };
				canvas->draw_image(&rst, ipos, 0);
			}
			if (img2 && img2->img)
			{
				canvas->draw_shape(img2, 2, 0, spos);
			}
			else if (tr2)
			{
				style_cs s[2] = {};
				tr2->t = &s[0];
				tr2[1].t = &s[1];
				s[0].color = c[0] * tr_mask.x;
				s[0].fill = c[0] * tr_mask.y;
				s[1].color = c[1] * tr_mask.z;
				s[1].fill = c[1] * tr_mask.w;
				spos += ((ss - tr2->size) * 0.5f);
				canvas->draw_shape(tr2, 2, spos);
			}
			else
			{
				canvas->draw_rect(spos, ss, 0, c[0], cie);
				canvas->draw_rect(spos, ss, 0, c[1], cie);
				spos += cie; spos.x += cpos;
				canvas->draw_circle(spos, radius, 0, c[2], 32);
			}
		}

		void switch_u::update(cpg_time* t)
		{
			auto ss = get_size();
			set_hover_mc((int)mouse_cursor::SC_HAND, 0);
			if (bv.v)
			{
				value2 = bv.get_value();
			}

			if (is_click(0))
			{
				set_value();
				ani = true;

				if (click_cb)
					click_cb(this);
				call_vcb();
			}
			if (value2 != value)
			{
				value = value2;
				v2 = true;
				if (change_cb)
					change_cb(this, value, uptr);
				if (on_change_cb)
					on_change_cb(this, value);
			}
			float k = t->deltaTime;//t->smoothDeltaTime
			float cie = ss.y * 0.5;
			c = { color[0],color[1],color[2] };
			float a = 0;
			auto tv2 = v2;
			glm::vec2 ps1 = { blur + 4, blur + 4 };
			//box_shadow_t bs = {};
			//auto s = ss;
			//s *= bs.spread;
			//s += blur * 3;
			if (v2)
			{
				if (ani)
				{
					glm::vec3 f = { 0,gt, k };//t->deltaTime
					dcr.y = (value) ? 1 : -1;
					if (AM::up_animate(f, &dcr, true))
					{
						ani = false;
					}
				}
				else {
					dcr.x = (value) ? gt : 0;
				}
				//auto b = blur;
				//if (b > 0)
				//{
				//	bs.blur = blur; bs.color = color[value ? 1 : 0];
				//	bs.h_shadow = 0; bs.v_shadow = 0;
				//	bs.count = 2;
				//	path->BeginPath();
				//	path->Rect(0, 0, ss.x, ss.y, cie);

				//	img_blur = ec->do_box_shadow(&bs, path, img_blur, s, ps1);
				//	//img->saveImage("temp/switch_u.png");
				//	ps1 += glm::vec2{ bs.h_shadow, bs.v_shadow };
				//}
			}
			a = dcr.x / gt;
			cpos = a * (ss.x - cie * 2);	// 白色圆坐标 
			c[0] = set_alpha_f(c[0], 1 - a);
			c[1] = set_alpha_f(c[1], a);	// 开关透明度

			base_u::update(t);
		}



		slider_u::slider_u() {
			_is_click = true;
			base_u::set_size({ 100, 7 });
			path = new path_stb();
			//ec = new effect_cx();
			has_drag = true;
		}

		slider_u::~slider_u() {
			delop(path);
			//delop(ec);
			delop(img);
		}

		void slider_u::set_size(const glm::vec2& s, float r)
		{
			base_u::set_size(s); radius = r;
			v2 = true;
		}

		void slider_u::set_value(float v)
		{
			if (v < 0)v = 0;
			if (v > 1)v = 1;
			value = v;
			if (change_cb)change_cb(this, value, uptr);
			v2 = true;
		}

		void slider_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto c = color;
			auto ss = get_size();
			glm::vec2 spos = pos1 + this->pos;
			glm::vec2 ps = { hotv.x, hotv.y };
			if (img)
				canvas->draw_image(img, { spos.x - blurpos.x,spos.y - blurpos.y,img->width,img->height });
			canvas->draw_rect(spos, ss, 0, c[0], r);
			canvas->draw_rect(spos, { cpos, ss.y }, 0, c[1], r);
			canvas->draw_circle(spos + ps, radius, c[1], c[2], 32, 2);
		}

		void slider_u::update(cpg_time* t)
		{
			auto hv = get_hover();
			auto pb = get_btnet();

			set_hover_mc((int)mouse_cursor::SC_HAND, 1);

			//printf("slider_u %p\n", pb);
			auto ss = get_size();
			if (hit)
			{
				if (has2c<click_ut>())
				{
					if (hv && pb->pidx == 0)
					{
						set_value(hv->pos1.x / ss.x);
						hv->idx = 1;//转移所有权到滑块拖动
					}
				}
			}
			if (pb && pb->pidx == 1)
			{
				auto pd = get_drag_ut();
				if (pd)
				{
					int dx = (pd->pos.x - pos.x - radius);
					if (dx != dragv)
					{
						//printf("%d\t%d\n", dx, dragv);
						dragv = dx;
						set_value(dragv / ss.x);
					}
				}
			}

			if (v2)
			{
				auto p = up_entity();
				glm::vec2 cie = { (ss.y * 0.5),(ss.y * 0.5) };
				cpos = value * (ss.x);	// 滑块坐标 
				auto ps = cie; ps.x = 0;
				ps.x += cpos;
				hotv.x = ps.x;
				hotv.y = ps.y;
				hotv.z = radius;
				// todo 添加一个响应
				p->last = &hotv;
				p->n = 1;
			}
			float k = t->deltaTime;
			glm::vec2 cie = { (ss.y * 0.5),(ss.y * 0.5) };

			blurpos = { blur + radius, blur + radius };
			auto s = ss;
			s += (ceil(radius) + blur) * 2;
			glm::vec2 ps = { hotv.x, hotv.y };

			if (v2)
			{
				v2 = false;
				/*auto b = blur;
				b = 0;
				if (b > 0)
				{
					box_shadow_t bs = {};
					bs.blur = blur; bs.color = color[1];
					bs.h_shadow = 0; bs.v_shadow = 0;
					bs.count = 2;
					path->BeginPath();
					path->Rect(0, 0, ss.x, ss.y, r);
					path->Circle(ps.x, ps.y, radius);

					img = ec->do_box_shadow(&bs, path, img, s, blurpos);
					//img->saveImage("temp/slider.png");
					path->BeginPath();
					path->Circle(ps.x, ps.y, radius * 10);
					path->Rect(0, 0, ss.x * 2, ss.y * 6, r);
					blurpos += glm::vec2{ bs.h_shadow, bs.v_shadow };
					//ps1 *= bs.spread;
				}*/
			}

			base_u::update(t);
		}

		progress_u::progress_u(float pf) :percentage(pf)
		{
			_is_click = true;
			set_size({ 250, stroke_width });
			set_color_idx(0);
		}

		progress_u::~progress_u()
		{
		}

		void progress_u::set_stroke_width(float sw, float width, float rounding)
		{
			if (sw < 3)
			{
				sw = 3;
			}
			if (width < 3)
			{
				width = 0;
			}
			set_size({ width ,sw });
			stroke_width = sw;
			if (rounding < 0)rounding = sw * 0.5;
			r = rounding;
			v2 = true;
		}
		void progress_u::set_percentage(float f)
		{
			if (f < 0)f = 0;
			if (f > 100)f = 100;
			percentage = f;
			set_value((float)f / 100.0);
		}
		void progress_u::set_value(float v)
		{
			if (v < 0)v = 0;
			if (v > 1)v = 1;
			value = v;
			v2 = true;
		}

		void progress_u::bind_value(float* p)
		{
			p_value = p;
		}
		void progress_u::set_color_bc(int v)
		{
			color[0] = v;
		}
		void progress_u::set_color_idx(int v)
		{
			if (v < 0)v = 0;
			if (v > 3)v = 3;
			color[1] = dcol[v];
			//v2 = true;
		}
		void progress_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto ss = get_size();
			auto spos = pos1 + pos;
			auto c = color;
			canvas->draw_rect(spos, ss, 0, c[0], r);
			int cpos1 = cpos;
			int apos1 = apos;
			if (apos1 < 0)
			{
				cpos1 = cpos + apos;
				apos1 = 0;
			}
			else if (cpos1 + apos1 > ss.x)
			{
				cpos1 = ss.x - apos1;
			}
			glm::vec4 cr = { spos ,cpos1,ss.y };
			if (cpos1 < r * 2)
			{
				auto cp1 = cpos1;
				cpos1 = r * 2;
				cr.z = abs(cp1);
				if (apos1 > ss.x * 0.5)
				{
					spos.x += ss.x - r * 2;
					cr.z = r * 2;
				}
				else {
					spos.x += apos1;
				}
				cr.x += apos1;
			}
			else {
				cr.x += apos1;
				spos.x += apos1;
			}
			// 进度
			if (cpos1 > 0 && cr.z > 0)
			{
				canvas->push_clipRect(cr, true);
				canvas->draw_rect(spos, { cpos1, ss.y }, 0, c[1], r);
				canvas->pop_clipRect();
			}
		}
		void progress_u::update(cpg_time* t)
		{
			if (p_value && value != *p_value)
			{
				set_value(*p_value);
				*p_value = value;
			}
			glm::vec3 f = { -value, 1 + value,t->deltaTime * duration };
			auto ss = get_size();
			if (v2)
			{
				v2 = false;
				set_size({ ss.x,stroke_width });
				cpos = ss.x * value;
				auto p = up_entity();
			}
			if (indeterminate)
			{
				// 更新动画
				AM::up_animate(f, &dcr, false);
				apos = dcr.x * ss.x;
			}
			else {
				apos = dapos;
			}

			base_u::update(t);
		}


		// todo tag

		tag_u::tag_u(const char* str)
		{
			_is_click = true;
			set_label(str);
		}

		tag_u::~tag_u()
		{
			//printf("free tag %p\n", this);
		}

		void tag_u::set_label(const std::string& str)
		{
			set_label(str.c_str());
		}
		void tag_u::set_label(const char* str)
		{
			do
			{
				if (!str || !*str)break;
				if (label == str)
				{
					return;
				}
				label = str;

			} while (0);
			isupstr = true;
		}
		void tag_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto spos = pos + pos1;

			auto pss = get_pos(true);
			auto size1 = get_size();
			auto col = fill_color ? fill_color : dcol[(int)type];
			auto bc = border_color ? border_color : col;
			auto fill = col;
			if (effect == uTheme::dark)
			{
				col = 0;
			}
			if (effect == uTheme::light)
			{
				bc = set_alpha_xf(bc, light * 3);
				fill = set_alpha_xf(col, light);
			}
			if (effect == uTheme::plain)
			{
				bc = set_alpha_xf(col, light * 3);
				fill = fill_color;
			}
			auto sp = spos;
			sp += mflex[0];
			if (bc || fill)
				canvas->draw_rect(sp, size1, bc, fill, rounding);

			mflex[2];//关闭按钮坐标
		}

		void tag_u::update(cpg_time* t)
		{
			if (update_cb)
			{
				update_cb(this, uptr);
			}
			if (isupstr)
			{
				isupstr = false;

				if (label.empty())
				{
					//set_size({ height, height });
				}
				else {
					{
						auto ss = ssize1;// get_size();
						bool upsize = true;// ss.x* ss.y <= 0;
						//if (height > 0 && upsize)
						if (ss.y < 1) {
							ss.y = (int)height;
						}
						int ny0 = ss.y;
						glm::ivec2 size1 = ss;

						{
							auto sps1 = sps;

							auto t = set_dtc(label.c_str(), label.size(), size1, sps1);
							if (t)
							{
								auto textcolor = color ? color : dcol[(int)type];
								if (effect == uTheme::dark)
								{
									textcolor = -1;
								}
								//if (effect == uTheme::plain)
								//{
								//	printf("%d\n", textcolor);
								//}
								t->color = textcolor;
								auto ss2 = ss;
								ss2.x = t->pos.x * 2 + t->rc.x + t->rc.y;
								ss2.y = std::max(height, t->rc.y);
								ss.x = std::max(ss.x, ss2.x);
								ss.y = std::max(ss.y, ss2.y);

								if (upsize)
								{
									if ((ssize1.x > 0 && ssize1.y > 0))
										size1 = ssize1;
									else
										size1 = ss;
									ptm->mk_box(t, size1, sps1);
									set_size(size1);
								}
							}
						}
					}
				}

				up_entity();

			}


			base_u::update(t);
		}

		// todo button 
		btn_css_t button_u::defcol(int idx) {
			static njson cols = { {"#ffffff", "#409eff", "#409eff", "#66b1ff", "#e6e6e6", "#0d84ff", "#0d84ff"},
							{"#ffffff", "#67c23a", "#67c23a", "#85ce61", "#e6e6e6", "#529b2e", "#529b2e"},
							{"#ffffff", "#909399", "#909399", "#a6a9ad", "#e6e6e6", "#767980", "#767980"},
							{"#ffffff", "#e6a23c", "#e6a23c", "#ebb563", "#e6e6e6", "#d48a1b", "#d48a1b"},
							{"#ffffff", "#f56c6c", "#f56c6c", "#f78989", "#e6e6e6", "#f23c3c", "#f23c3c"} };
			if (idx < 0)idx = 0;
			if (idx >= cols.size())
				idx = idx % (cols.size());
			btn_css_t ret = {};
			uint32_t t[7] = {};
			uint32_t* pt = t;
			auto& it = cols[idx];
			for (auto& ct : it)
			{
				*pt = (toColor(ct, 0)); pt++;
			}
			ret = { t[0],t[1],t[2],t[3],t[4],t[5],t[6] };
			return ret;

		}
		button_u::button_u(const std::string& str)
		{
			_is_click = true;
			set_label(str);
		}

		button_u::~button_u()
		{
			delop(imginfo);
			delop(_nbtn);
			//delop(tdc);
			delop(_defbdc);
			delop(rotate_text);
		}

		void button_u::set_label(const std::string& str) {
			if (_label != str.c_str())
			{
				_label = str;
				v2 = true;
			}
		}
		void button_u::set_label(const void* str) {
			if (_label != (char*)str)
			{
				_label = (char*)str;
				v2 = true;
			}
		}
		void button_u::set_color_idx(int idx)
		{
			bdc = ui::button_u::defcol(idx);
		}
		void button_u::set_color(btn_css_t* c) {
			bdc = *c;
		}
		void button_u::set_color(const btn_css_t& c)
		{
			bdc = c;
		}
		/*
			uint32_t font_color = 0xFF222222;
			uint32_t background_color = 0xFFffffff;
			uint32_t border_color = 0xFF222222;
			uint32_t hover_color = 0xFFf78989;
			uint32_t active_font_color = 0xFFe6e6e6;
			uint32_t active_background_color = 0xFFf2f2f2;
			uint32_t active_border_color = 0xFF3c3c3c;
			uint32_t hover_border_color = 0;
		*/
		// #8296D8
		//void button_u::set_color(uint32_t fill, uint32_t border, uint32_t text_color)
		//{
		//	bdc.font_color = (text_color);
		//	bdc.background_color = set_alpha_xf(fill, 0.8);
		//	bdc.border_color = set_alpha_xf(border, 0.8);
		//	bdc.hover_color = set_alpha_xf(fill, 0.5);
		//	bdc.active_font_color = set_alpha_xf(text_color, 1);
		//	bdc.active_background_color = set_alpha_xf(fill, 0.9);
		//	bdc.active_border_color = set_alpha_xf(border, 0.9);
		//	bdc.hover_border_color = set_alpha_xf(border, 0.5);
		//}
		void button_u::set_round(float r, bool is_circle) {
			if (r < 0)r = 0;
			if (r > 1)r = 1;
			rounding = r; _is_circle = is_circle;
		}
		void button_u::set_rotate_text(anim_a* rot)
		{
			rotate_text = rot;
			mtt = (motion_t*)rotate_text;
			if (!rot)
			{
				mtt = 0;
			}
		}
		//
		void button_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto spos = pos1 + pos;
			auto ss = get_size();
			auto size1 = ss;
			uint32_t col = dcol;
			uint32_t fill = dfill;
			if (!borderable)
			{
				col = 0;
			}
			if (!fillable)
			{
				fill = 0;
			}
			switch (type)
			{
			case 0:
			{
				if (_is_circle)
				{
					auto sp = spos;
					auto r = lround(height * 0.5);
					sp += r;
					canvas->draw_circle(sp, r, col, fill, height * 3, thickness);
				}
				else
				{
					canvas->draw_rect(spos, size1, col, fill, dround, thickness);
				}
			}break;
			case 1:
			{
				if (_nbtn)
				{
					_nbtn->pos1 = spos;
					gui_draw_btn(canvas, _nbtn);
				}
			}break;
			case 2:
			{
				if (imginfo)
				{
					auto idx = iidx;
					if (!imginfo[idx].img)
						idx = 0;
					if (imginfo[idx].img)
						canvas->draw_shape(&imginfo[idx], 1, -1, spos);
				}
			}
			default:
				break;
			}

			if (_ck_icon)
				canvas->draw_shape(_ck_icon, 1, spos);
			if (_icon_r)
				canvas->draw_shape(_icon_r, 1, spos);
		}


		void button_u::update(cpg_time* t)
		{
			bool isck = is_click(0);
			if (!_disabled)
			{
				//auto hp = get_hover();
				if (hit)
				{
					//hp->inc++;hp->inc == 1 &&
					if (enter_cb)
					{
						elinc = 1;
						enter_cb(this, uptr);
					}
				}
				//if (elinc)
				{
					if (isleave)
					{
						if (leave_cb)
							leave_cb(this, uptr);
						elinc = 0;
						isleave = false;
					}
				}
				//else
				{
					//isleave = false;
				}
				if (isck && hit)
				{
					if (click_cb0)
						click_cb0(this, uptr);
					if (click_cb1)
						click_cb1(this, uptr);
					if (click_cb)
						click_cb(this);
					call_vcb();
				}
				if (rotate_text)
				{
					auto rpos = get_size();;
					rpos *= 0.5;
					rpos += get_pos2();//获取全局坐标
					rotate_text->up_rotate(isck, rpos, t);
				}
			}
			bool hover = uhover || hit;// reg->has<hover_et>(e);
			auto pdc = &bdc;
			if (reg)
			{
				if (_disabled)
				{
					hover = false;
					dfill = pdc->background_color;
					dcol = pdc->border_color;
					if (effect == uTheme::dark)
					{
						dcol = 0;
						if (dtc)dtc->color = (text_color) ? text_color : pdc->font_color;
					}
					if (effect == uTheme::light)
					{
						dcol = set_alpha_xf(dcol, light * 3);
						dfill = set_alpha_xf(dfill, light);
						if (dtc)dtc->color = (text_color) ? text_color : pdc->border_color;
					}
					if (effect == uTheme::plain)
					{
						dfill = 0;
						if (dtc)dtc->color = (text_color) ? text_color : pdc->border_color;
					}
					dcol = set_alpha_x(dcol, disabled_alpha);
					dfill = set_alpha_x(dfill, disabled_alpha);
					if (dtc)
						dtc->color = set_alpha_x(dtc->color, disabled_alpha);
					if (_nbtn)
						_nbtn->bs &= ~(int)BTN_STATE::STATE_DISABLE;
				}
				else
				{
					if (_is_hand)
						set_hover_mc((int)mouse_cursor::SC_HAND, 0);
					bool isdown = is_down(0);
					if (_nbtn)
					{
						if (isdown)
							_nbtn->bs = (int)BTN_STATE::STATE_ACTIVE;
						else
							_nbtn->bs &= ~(int)BTN_STATE::STATE_ACTIVE;
					}
					if (isdown)
					{
						dfill = pdc->active_background_color;
						dcol = pdc->active_border_color;
						if (effect == uTheme::plain)
						{
							dfill = set_alpha_xf(dcol, light);
						}
						else {
							if (dtc)dtc->color = (text_color) ? text_color : pdc->active_font_color;
						}
						if (effect == uTheme::light)
						{
							//dcol = pdc->active_border_color;
							dfill = set_alpha_f(dfill, light * 5);
						}
						if (effect == uTheme::dark)
						{
							dcol = 0;
						}
						iidx = (int)btn_state_e::click;
						if (down_cb)
							down_cb(this, t);
					}
					else if (hover)
					{
						if (_nbtn)
						{
							auto& bs = _nbtn->bs;
							//bs = (int)BTN_STATE::STATE_NOMAL;
							if (!(bs & (int)BTN_STATE::STATE_ACTIVE) && _nbtn->mEnabled)
							{
								bs = (int)BTN_STATE::STATE_HOVER;
								//if (binfo.reg->has<down_et>(p->e))
								//{
								//	bs = (int)BTN_STATE::STATE_ACTIVE;
								//}
							}
						}

						uint32_t ac = pdc->hover_color;
						dfill = pdc->hover_color;
						if (pdc->hover_border_color)
							dcol = pdc->hover_border_color;

						if (effect == uTheme::plain)
						{
							dcol = pdc->border_color;// set_alpha_xf(, light * 3);
							dfill = set_alpha_xf(dcol, light);
						}
						if (effect == uTheme::light)
						{
							//dcol = pdc->active_border_color;
							dfill = set_alpha_f(dfill, light * 5);
							if (dtc)dtc->color = (text_color) ? text_color : pdc->font_color;
						}
						//printf("hover_et%p\n",this);
						iidx = (int)btn_state_e::hover;
					}
					else {
						if (_nbtn)
						{
							auto& bs = _nbtn->bs;
							if (bs != (int)BTN_STATE::STATE_NOMAL)
							{
								if (!(bs & (int)BTN_STATE::STATE_ACTIVE) || (bs & (int)BTN_STATE::STATE_HOVER))
								{
									bs = (int)BTN_STATE::STATE_NOMAL;
									//printf("leave:%p\n", p);
								}
							}
						}

						dfill = pdc->background_color;
						dcol = pdc->border_color;
						if (effect == uTheme::dark)
						{
							dcol = 0;
							if (dtc)dtc->color = (text_color) ? text_color : pdc->font_color;
						}
						if (effect == uTheme::light)
						{
							dcol = set_alpha_xf(dcol, light * 3);
							dfill = set_alpha_xf(dfill, light);
							if (dtc)dtc->color = (text_color) ? text_color : pdc->border_color;
						}
						if (effect == uTheme::plain)
						{
							dfill = 0;
							if (dtc)dtc->color = (text_color) ? text_color : pdc->border_color;
						}
						iidx = (int)btn_state_e::normal;
						//imginfo[iidx];
					}
				}
				if (_nbtn)
					btn_set_state(_nbtn, _nbtn->bs);
			}
			if (v2)
			{
				v2 = false;
				if (!cptext)
				{

					auto ss = get_size();
					if (_label.empty())
					{
						if (height > 0 && (ss.x < 1 || ss.y < 1))
							set_size({ height, height });
					}
					else {
						bool upsize = ss.x * ss.y <= 0;
						if (height > 0 && upsize)
						{
							ss.y = height;
							ss.x = (_is_circle) ? height : 0;
						}
						if (width > 0 && upsize)
						{
							ss.x = width;
						}
						int ny0 = ss.y;
						glm::ivec2 size1 = ss;
						auto tc = (text_color) ? text_color : pdc->font_color;
						if (effect == uTheme::dark)
							tc = -1;
						if (effect == uTheme::plain)
							tc = pdc->border_color;

						if (!_is_circle)
							ss.x = size1.x;
						//if (!dtc)
						{
							//std::string kc = (char*)"\xef\x85\x92";
							//if (kc == _label.c_str())
							//{
							//	kc.size();
							//}
							auto t = set_dtc(_label.c_str(), _label.size(), size1, posf);
							if (t)
							{
								if ((width < t->rc.x))
									ss.x = size1.x;
								auto ss2 = ss;
								ss2.x = t->pos.x * 2 + t->rc.x;
								ss2.y = std::max(height, t->rc.y);
								ss.x = std::max(ss.x, ss2.x);
								ss.y = std::max(ss.y, ss2.y);

								if (_is_circle)
								{
									ss.x = std::max(ss.x, ss.y);
									ss.y = ss.x; upsize = true;
								}
								else if (width < 1)
								{
									if (0 < height || upsize)
									{
										auto kx = ny0 - t->rc.y;
										t->pos.x += kx * 0.5;
										ss.x += kx; upsize = true;
									}
								}
								int pxx = t->pos.x;
								if (pxx == 0 && !(posf.x > 0) && thickness > 0)
								{
									auto kx = ny0 - t->rc.y;
									t->pos.x += kx * 0.5;
								}
								if (upsize)
									set_size(ss);
							}
						}
					}
				}
				if (_disabled)
				{
					iidx = (int)btn_state_e::disable;
				}

				auto ss = get_size();
				if (roundingi > 0)
					dround = roundingi;
				else
					dround = std::min(ss.x, ss.y) * rounding;
				if (_nbtn)
					_nbtn->size = ss;
				up_entity();
			}
			if (dtc && dtc->count)
			{
				tpos = posi;
				if (!_disabled && down_offset > 0 && is_down(0))
					tpos += down_offset;
				auto_drawtext = _is_text;
			}

			base_u::update(t);
		}
		btn_css_t* button_u::get_color()
		{
			return &bdc;
		}
		//void button_u::set_cp_text(text_dc* t)
		//{
		//	cptext = true;
		//	//if (!tdc)tdc = new text_dc();
		//	//tdc->cp(t);
		//	dtcs = t->dtext;
		//	dtc = &dtcs;
		//}
		void button_u::set_height(int h)
		{
			if (!imginfo)
			{
				height = h; v2 = true;
			}
		}

		void button_u::set_image(btn_state_e idx, Image* img, glm::ivec4 rect, glm::vec2 size, glm::vec4 sliced)
		{
			type = 2;
			width = height = 0;
			if (!imginfo)
			{
				//auto n = ptm->new_mem((int)btn_state_e::maxbs, imginfo);
				imginfo = new image_cs[(int)btn_state_e::maxbs];
			}
			int i = (int)idx;
			auto& it = imginfo[i];
			it.img = img;
			if (size.x * size.y <= 0)
				size = get_size();
			if (!(rect.z > 0 && rect.w > 0))
			{
				rect.z = size.x; rect.w = size.y;
			}
			it.size = size;
			it.rect = rect;
			it.sliced = sliced;
		}

		void button_u::bind_image(btn_state_e idx, Image* img)
		{
			if (!imginfo)
			{
				imginfo = new image_cs[(int)btn_state_e::maxbs];
			}
			int i = (int)idx;
			auto& it = imginfo[i];
			it.img = img;
		}

		void button_u::set_gradient(unsigned int back_color, unsigned int tc, int rounding, icon_rt* icon)
		{
			type = 1;
			width = height = 0;
			if (!_nbtn)_nbtn = new btn_info_u();
			if (icon)
				_nbtn->icon = *icon;
			text_color = tc;
			button_init(_nbtn, {}, "", back_color, text_color);
			_nbtn->rounding = rounding;
		}

		template<class _Ty>
		void _set1u(_Ty* v, size_t pos, bool val) {
			static ptrdiff_t _Bitsperword = CHAR_BIT * sizeof(_Ty);
			auto& _Selected_word = v[pos / _Bitsperword];
			const auto _Bit = _Ty{ 1 } << pos % _Bitsperword;
			if (val) {
				_Selected_word |= _Bit;
			}
			else {
				_Selected_word &= ~_Bit;
			}
		}
		template<class _Ty>
		bool _subscript(_Ty* v, size_t _Pos) {
			static ptrdiff_t _Bitsperword = CHAR_BIT * sizeof(_Ty);
			return (v[_Pos / _Bitsperword] & (_Ty{ 1 } << _Pos % _Bitsperword)) != 0;
		}


		void bdata_t::set_value(bool is) {
			if (v)
			{
				switch (size) {
				case 1: { _set1u(((uint8_t*)v), b, is); }break;
				case 4: { _set1u(((uint32_t*)v), b, is); }break;
				case 8: { _set1u(((uint64_t*)v), b, is); }break;
				}
			}
		}

		bool bdata_t::get_value()
		{
			bool r = false;
			if (v)
			{
				switch (size) {
				case 1: { r = _subscript((uint8_t*)v, b); }break;
				case 4: { r = _subscript((uint32_t*)v, b); }break;
				case 8: { r = _subscript((uint64_t*)v, b); }break;
				}
			}
			return r;
		}
		// todo checkbox_u
		checkbox_u::checkbox_u()
		{
			_is_click = true;
			set_size({ square_sz ,square_sz });
		}

		checkbox_u::~checkbox_u()
		{

		}

		void checkbox_u::set_mixed(bool v)
		{
			ck.mixed = v;
		}
		void checkbox_u::set_height(int h)
		{
			height = h;
		}
		void checkbox_u::set_value(bool v)
		{
			value2 = v;
			bv.set_value(v);
		}

		void checkbox_u::bind_value(void* p, uint16_t set1, uint8_t size, uint8_t count)
		{
			bv.v = p;
			bv.b = set1;
			bv.size = size;
			bv.count = count;
		}
		bool checkbox_u::get_value()
		{
			return value;
		}
		void checkbox_u::set_value()
		{
			value2 = !value2;
			bv.set_value(value2);
			//if (b_value)
			//{
			//	*b_value = value2;
			//}
		}
		void checkbox_u::set_label(const std::string& str) {
			if (label != str.c_str())
			{
				label = str;
				v2 = true;
			}
		}
		void checkbox_u::set_label2(const std::string& str, const std::string& str0)
		{
			label = str;
			label2 = str0;
			v2 = true;
		}
		void checkbox_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto spos = pos + pos1;
			//canvas->draw_rect(spos, get_size(), col, fill, 4, 1);
			if (!only_text)
				canvas->draw_shape(&ck, 1, spos);
			if (value && str2)
			{
				canvas->draw_shape(str2, spos, 0);
			}
		}

		void checkbox_u::update(cpg_time* t)
		{
			if (t)
			{
				auto hv = get_hover();
				auto pb = get_btnet();
				if (ckt2 < cktime2)
					ckt2 += t->deltaTime;
				upvalue();
				if (is_click(0))
				{
					if (hv && pb->pidx == 0)
					{
						//printf("checkbox: %d\n", hit);
						if (ckt2 > cktime2)
						{
							ckt2 = 0;
							set_value();
							upvalue();
							ani = true;
							if (click_cb)
								click_cb(this);
							call_vcb();
						}
					}
				}
				if (ani)
				{
					glm::vec3 f = { 0 , 1 , t->deltaTime * ts };
					dcr.y = (value) ? 1 : -1;
					if (AM::up_animate(f, &dcr, true))
						ani = false;
					ck.new_alpha = dcr.x;
				}
				else {
					ck.new_alpha = (value) ? 1 : 0;
				}
			}
			float otx = 1.0;
			if (only_text)
			{
				otx = 0;
				ck.new_alpha = 0;
			}
			int tx = 0;
			if (v2 || !dtc)
			{
				v2 = false;
				auto str = label;
				auto psf = posf;
				psf.x = 0;
				glm::ivec2 size1 = get_size();// { 0, square_sz };
				//size1.x -= square_sz;
				if (label2.size())
				{
					auto ts = mk_str(label2.c_str(), label2.size(), size1, psf, str2);
					if (ts)
					{
						str2 = ts;
						str2->color = col;
					}
				}
				if (!str.empty())
				{
					auto ss = get_size();
					ss.x = 0;
					ss.y = height;
					auto t = set_dtc(str.c_str(), str.size(), size1, psf);
					if (t) {
						{
							//t->pos.x += (stp + square_sz) * otx;
							ss.x = t->pos.x + t->rc.x;
							ss.y = std::max(square_sz, t->rc.y);
						}
						if (str2) {
							//str2->pos.x += (stp + square_sz) * otx;
							auto ss2 = ss;
							ss2.x = str2->pos.x * 2 + str2->rc.x;
							ss2.y = std::max(square_sz, str2->rc.y);
							ss.x = std::max(ss.x, ss2.x);
							ss.y = std::max(ss.y, ss2.y);
						}
						if (size1.x < ss.x)
							set_size(ss);
					}
				}
				//dround = std::min(size.x, size.y) * rounding;
				up_entity();
			}
			if (dtc) {
				dtc->color = (value) ? col : textcol;
				if (!add_tx && str2)
				{
					dtc->color = 0;
				}
				tx = dtc->rc.x + stp;
			}
			if (str2)
			{
				tx = str2->rc.x + stp;
			}
			auto dfill = value ? col : fill;
			auto dcol = value ? col : col0;
			auto cc = check_col;
			if (ani)
				dfill = col;
			if (reg && reg->has<hover_et>(e))
				dcol = col;

			auto ss = get_size();
			ck.pos.x = ceil(posf.x * (ss.x - (square_sz + tx)));
			if (dtc)
				dtc->pos.x = ck.pos.x + stp + square_sz;
			if (str2)
				str2->pos.x = ck.pos.x + stp + square_sz;
			ck.pos.y = ceil(posf.y * (ss.y - square_sz));
			ck.col = dcol;
			ck.fill = dfill;

			base_u::update(t);
		}


		void checkbox_u::upvalue()
		{
			if (bv.v)
			{
				value2 = bv.get_value();
			}
			if (value2 != value)
			{
				value = value2;
				if (change_cb)
					change_cb(this, value, uptr);
				v2 = true;
			}
		}

		// todo radio_u
		radio_u::radio_u(group_radio_u* g)
		{
			_is_click = true;
			set_size({ radius * 2, radius * 2 });
			set_g(g);
		}

		radio_u::~radio_u()
		{
			if (gr && gr->active == this)
			{
				delop(gr);
			}
		}
		radio_u* radio_u::new1()
		{
			return new radio_u();
		}
		group_radio_u* radio_u::new_g()
		{
			if (!gr)gr = new group_radio_u();
			return gr;
		}
		void radio_u::set_label2(const std::string& str, const std::string& str0)
		{
			label = str;
			label2 = str0;
			v2 = true;
		}

		void radio_u::set_g(group_radio_u* g)
		{
			if (g)
			{
				gr = g;
				if (value || !gr->active)
				{
					gr->active = this;
				}
			}
		}

		group_radio_u* radio_u::get_g()
		{
			return gr;
		}

		void radio_u::set_value(bool v)
		{
			if (v != value2)
			{
				value2 = v;
			}
			if (value2 && gr)
			{
				if (gr->active && gr->active != this)
				{
					gr->active->set_value(false);
				}
				gr->active = this;
			}
			bv.set_value(v);
		}
		void radio_u::set_value()
		{
			set_value(true);
		}

		void radio_u::bind_value(void* p, uint16_t set1, uint8_t size, uint8_t count)
		{
			bv.v = p;
			bv.b = set1;
			bv.size = size;
			bv.count = count;
		}
		void radio_u::set_label(const std::string& str)
		{
			if (label != str.c_str())
			{
				label = str;
				v2 = true;
			}
		}
		void radio_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto spos = pos + pos1 + cpos;

			auto dfill = value ? col : fill;
			auto dcol = value ? col : col0;
			if (ani)
				dfill = col;
			if (hit)
				dcol = col;
			auto ss = get_size();
			if (uimg)
			{
				//测试矩形
				//canvas->draw_rect(pos + pos1, ss, -1, 0);
				canvas->draw_shape(uimg, ucount, set_alpha_f(ucol, (1.0 - dcr.x) + apf), pos + pos1);
				if (hit)
					canvas->draw_shape(uimg, ucount, set_alpha_f(hit_col, hita), pos + pos1);
			}
			else if (!only_text)
			{
				canvas->draw_circle(spos, radius, dcol, dfill, radius * 3, 1);
				canvas->draw_circle(spos, swidth, 0, fill, radius * 3, 1);
			}
			if (value && str2)
			{
				canvas->draw_shape(str2, spos, 0);
			}
			//printf("radio_u:%p\t%p\n", this, parent);
		}

		void radio_u::update(cpg_time* t)
		{
			if (t)
			{
				auto hv = get_hover();
				auto pb = get_btnet();

				if (_is_hand)
				{
					set_hover_mc((int)mouse_cursor::SC_HAND, 0);
				}
				if (t && ckt2 < cktime2)
					ckt2 += t->deltaTime;
				upvalue();
				if (is_click(0))
				{
					if (hv && pb->pidx == 0)
					{
						if (ckt2 > cktime2)
						{
							ckt2 = 0;
							set_value();
							upvalue();
							ani = true;
							if (click_cb)
								click_cb(this);
							call_vcb();
						}
					}
				}
				if (ani)
				{
					auto dt = t->deltaTime;//smoothDeltaTime;
					glm::vec3 f = { maxborder , 1 ,dt * ts };
					dcr.y = (value) ? -1 : 1;
					if (AM::up_animate(f, &dcr, true))
						ani = false;
					swidth = dcr.x * (radius - 1);
				}
				else {
					//swidth = (value ?  maxborder:1 ) * (radius - 1);
				}
			}

			float otx = 1.0;
			if (only_text)
			{
				otx = 0;
			}
			if (v2)
			{
				v2 = false;
				auto r2 = radius * 2;
				auto str = label;
				glm::ivec2 size1 = get_size();
				if (uimg)
				{
					//size1 = uimg->size;// posf = { 0.5,0.49 };
					//set_size(size1);
				}
				if (label2.size())
				{
					auto ts = mk_str(label2.c_str(), label2.size(), size1, posf, str2);
					if (ts)
					{
						str2 = ts;
						str2->color = col;
					}
				}
				if (str.empty())
				{
					set_size({ r2 , r2 });
					cpos = glm::vec2(radius, radius);
				}
				else {

					auto t = set_dtc(str.c_str(), str.size(), size1, posf);
					if (t && !uimg)
					{
						t->pos.x += (stp + r2) * otx;
						auto ss = size1;
						ss.x = t->pos.x + t->rc.x;
						ss.y = std::max(r2, t->rc.y);
						if (str2)
						{
							auto ss2 = size1;
							str2->pos.x += (stp + r2) * otx;
							ss2.x = str2->pos.x + str2->rc.x;
							ss2.y = std::max(r2, str2->rc.y);
							ss.x = std::max(ss.x, ss2.x);
							ss.y = std::max(ss.y, ss2.y);
						}
						if (size1.x < ss.x)
							set_size(ss);
						cpos = glm::vec2(radius, lround(ss.y * 0.5));
					}
				}
				up_entity();
			}
			if (dtc) {
				dtc->color = (value && !static_tc) ? col : textcol;
				if (!add_tx && str2)
				{
					dtc->color = 0;
				}
			}

			auto dfill = value ? col : fill;
			auto dcol = value ? col : col0;
			if (ani)
				dfill = col;
			if (reg && reg->has<hover_et>(e))
				dcol = col;


			base_u::update(t);

		}

		void radio_u::upvalue()
		{
			if (bv.v)
			{
				value2 = bv.get_value();
			}
			if (value2 != value)
			{
				value = value2;
				ani = true;
				if (change_cb)
					change_cb(this, value, uptr);
				if (change_cb1)
					change_cb1(this, value, uptr);
				v2 = true;
			}
		}

		group_radio_u::group_radio_u()
		{
		}

		group_radio_u::~group_radio_u()
		{
		}
		void group_radio_u::push(radio_u* p)
		{
			if (srp.empty())
			{
				first = p;
			}
			srp.insert(p);
		}


		// todo div
		void do_layout(const glm::vec2& size, glm::vec4* child, size_t n, njson style, njson child_css)
		{
			njson& st = style;
			{
				if (st.find("justify_content") == st.end())
					st["justify_content"] = flex::e_align::align_space_around;	// 横向对齐
				if (st.find("align_content") == st.end())
					st["align_content"] = flex::e_align::align_space_around;		// 纵向
				if (st.find("align_items") == st.end())
					st["align_items"] = flex::e_align::align_center;		//孩子纵轴（上下）
				if (st.find("direction") == st.end())
					st["direction"] = flex::e_direction::row;				// row = 0, row_reverse, column, column_reverse
			}
			auto m1 = toFloat(st["margin1"], 0.0);
			if (m1 > 0)
			{
				if (child_css.is_array())
				{
					for (auto& it : child_css)
					{
						if (it.find("margin") == it.end())
						{
							it["margin"] = m1;
						}
					}
				}
				else if (child_css.is_null() || (child_css.is_object() && child_css.find("margin") == child_css.end()))
				{
					child_css["margin"] = m1;	// 加外边距
				}
			}

			if (child && n)
			{
				layout_new_info_t np = {};
				np.size = size;
				np.root_css = st;
				np.v2offset = sizeof(glm::vec4);
				np.v2ptr = ((char*)child) + sizeof(glm::vec2);
				np.n = n;
				np.child_css = child_css;
				easy_flex::build(&np, [=](int idx, const glm::vec4& rc, bool is)
					{
						if (!is)
						{
							auto& c = child[idx];
							c.x = rc.x; c.y = rc.y;
						}
					});
			}
		}

		int do_layout(glm::vec2 size, njson styles, ui::base_u** child, size_t n, glm::vec2 pos)
		{
			if (!child || n == 0)return 0;
			// 布局计算
			t_vector<glm::vec4> vcs;
			t_vector<ui::base_u*> vc1;

			njson style_c;// = c.child_css;
			int ic = 0;
			vcs.clear();
			vcs.reserve(n);
			for (size_t i = 0; i < n; i++)
			{
				auto it = child[i];
				auto se = it->get_size();
				if (!it->visible)
					continue;
				style_c.push_back(it->styles);
				vcs.push_back({ it->pos, se });
				vc1.push_back(it);
				if (it->styles.size())
					ic++;
			}
			if (ic == 0)
			{
				style_c = {};
			}
			int vic = 0;
			auto ss = size;
			do_layout(ss, vcs.data(), vcs.size(), styles, style_c);
			auto n1 = vc1.size();
			// 四舍五入
			for (size_t i = 0; i < n1; i++)
			{
				auto& it = vc1[i];
				glm::vec2 npos = { llround(vcs[i].x + pos.x), llround(vcs[i].y + pos.y) };
				auto ks = it->get_size();
				bool isv = ks.x * ks.y > 0;
				if (!isv && it->visible)
				{
					vic++;
				}
				it->isdrawble = (isv);
				it->set_pos(npos);
			}
			return vic;
		}


#if 1

		// todo view
		view_u::view_u()
		{
			_child = new asyn_queue_x<ui::base_u*>();
			if (!_horizontal)
			{
				// 创建滚动条
				auto psc = new scroll_u[2]();
				_horizontal = psc; _vertical = psc + 1;
				_horizontal->set_dir(0);
				// 更新滚动坐标
				_horizontal->change_cb = [=](scroll_u* p) {
					if (p)
					{
						auto x = p->_offset;
						scroll_pos.x = -x;
						_pos.x = -x;
						int idx = 1;
						idx *= p->_is_down ? 1 : -1;
						if (on_scroll_cb)
							on_scroll_cb({ p->_offset, _vertical->_offset }, idx);
					}

					};
				_vertical->change_cb = [=](scroll_u* p) {
					if (p) {
						auto y = p->_offset;
						scroll_pos.y = -y;
						_pos.y = -y;
						int idx = 2;
						idx *= p->_is_down ? 1 : -1;
						if (on_scroll_cb)
							on_scroll_cb({ _horizontal->_offset, p->_offset }, idx);
					}
					};
			}
			ptm = new gtac_c();
			ac = ptm;
		}

		view_u::~view_u()
		{
			clear();
			delop(_child);
			delop(ptm);
			delop(_horizontal, true); //delop(vertical);
		}
		void view_u::set_dlt(div_text_info_e dt)
		{
			ptm->ctx = dt;
		}
		//void view_u::set_rc(const glm::vec4& rc)
		//{
		//	_pos = { rc.x,rc.y }; _size = { rc.z,rc.w };
		//}
		void view_u::set_bpos(const glm::vec4& pos)
		{
			bpos = pos;
		}
		void view_u::set_num(int row, int col)
		{
			if (!(row > 0 && col > 0))return;
			// 行数
			row_num = row;
			// 列数
			col_num = col;

			//v.resize(col_num);
			//for (size_t i = 0; i <col_num ; i++)
			//{
			//	auto& it = v[i];
			//	it.resize(row_num);
			//}

		}
		void view_u::set_direction(int d)
		{
			styles["direction"] = d;
		}
		void view_u::set_row_column(bool row)
		{
			styles["direction"] = row ? flex::e_direction::row : flex::e_direction::column;
		}
		void view_u::set_position(bool is_absolute)
		{
			styles["position"] = is_absolute ? flex::e_position::absolute : flex::e_position::relative;
			sort_inc++;
		}
		void view_u::set_padding1(int v)
		{
			styles["padding1"] = v;
		}
		void view_u::set_align(int justify_content, int align_content, bool isrow)
		{
			styles["justify_content"] = justify_content > 0 ? justify_content : (int)flex::e_align::align_space_evenly;
			styles["align_content"] = align_content > 0 ? align_content : (int)flex::e_align::align_space_evenly;
		}
		void view_u::set_align_items(int align_items)
		{
			styles["align_items"] = align_items > 0 ? align_items : (int)flex::e_align::align_stretch;
		}

		base_u* view_u::push(base_u* p, int idx)
		{

			do {
				if (!p)break;
				//if (idx < v.size())
				v[idx].push_back(p);

				if (p->is_div)
				{
					//p->dparent = this;
				}
				if (p->reg)
				{
					//assert(0);// p->parent && "先删除再添加");
					break;
				}
				//p->parent = this;
				if (!p->ptm && ptm)
					p->ptm = ptm;
				if (ptm && ptm->ctx.lt)
					p->set_lt(ptm->ctx);
				if (reg)
				{
					p->set_order(p->get_order());
					p->bind_event(reg);
				}
				_child->push(p);
				//tadd.push(p);
				//adinc++;
			} while (0);
			return p;
		}
		void view_u::pop(base_u* p)
		{
			if (p->reg != reg)
			{
				if (0 != (int)p->e && p->reg)
				{
					p->reg->destroy(p->e);
				}
				p->e = ety_null; p->reg = 0;
			}
			p->parent = 0;
			_child->pop(p);
			//tdel.push(p);
			//adinc++;
		}
		void view_u::clear()
		{
			_child->rad();
			while (_child->_d2.size())
			{
				auto p = _child->get2();
				delop(p);
			}
			for (auto it : _child->v) { delop(it); }
		}
		void view_u::move2last(base_u* p)
		{
			_last = p;
		}
		size_t view_u::count()
		{
			return _child->v.size();
		}
		void view_u::set_scroll_step(int h, int v)
		{
			scroll_step = { h,v };
		}
		void view_u::set_scroll_count(int c)
		{
			scroll_count = c;
		}

		void view_u::ndrag(drag_ut* dp)
		{
			if (dp->state == 0)
			{
				_horizontal->on_dragstart(dp->pos);
				_vertical->on_dragstart(dp->pos);
				dp->last = dp->pos; dp->last--;
			}
			if (dp->last != dp->pos)
			{
				auto k1 = _horizontal->down_idx;
				_horizontal->down_idx = _horizontal->_thumb_idx;
				auto k2 = _vertical->down_idx;
				_vertical->down_idx = _vertical->_thumb_idx;

				_horizontal->on_drag(dp->pos);
				_vertical->on_drag(dp->pos);
				dp->last = dp->pos;

				_horizontal->down_idx = k1;
				_vertical->down_idx = k2;
			}
		}

		void view_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto spos = pos1;
			glm::vec2 cp = scroll_pos;
			for (auto& [x, cp1] : *_layer)
			{
				auto vt = v[x];
				clicp_x ccpa(vt.size() ? canvas : nullptr, mk_clicp(cp1, clip), true);
				base_u* lt = 0;
				for (auto& it : vt)
				{
					if (it->visible)
					{
						if (it != _last)
							it->draw(t, canvas, spos + it->mask_scroll * cp);				// 控件自定义绘制
						else
							lt = it;
					}
				}
				// 同一层最后渲染
				if (lt)
				{
					lt->draw(t, canvas, spos + lt->mask_scroll * cp);
				}
			}


		}
		void view_u::draw_last(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto spos = pos1;

			// 统一绘制的文本

			for (auto& [lt, cp1] : *_layer)
			{
				auto vt = v[lt];
				clicp_x ccpa(canvas, mk_clicp(cp1, clip));
				for (auto& it : vt)
				{
					if (!it->visible || !it->auto_drawtext || !it->dtc || it->dtc->count <= 0)
						continue;
					glm::vec2 cp = it->mask_scroll * (glm::vec2)scroll_pos;
					if (it->mtt)
						canvas->push_ubo(it->mtt);
					canvas->draw_shape(it->dtc, 1, spos + it->pos + it->tpos + cp);
					if (it->mtt)
						canvas->pop_ubo();
				}
			}
			if (_has_scroll)
			{
				scroll_u* scroll[2] = { _horizontal,_vertical };
				for (int i = 0; i < 2; i++)
				{
					if (scroll[i]->visible)
						scroll[i]->draw(t, canvas, pos1);
				}
			}
		}
		void view_u::update(cpg_time* t)
		{
			if (!reg)return;
			_child->rad([=](base_u* p, bool a) {
				if (a && !p->reg && reg)
				{
					p->set_order(p->get_order());
					p->bind_event(reg);
				}
				});
			if (sort_inc != 0 && v.size())
			{
				sort_inc = 0;
				for (auto& [k, v0] : v)
				{
					std::stable_sort(v0.begin(), v0.end(), [](ui::base_u* p1, ui::base_u* p2) { return (p1->order < p2->order); });
				}
			}
			{
				if (_horizontal)
				{
					if (!_horizontal->reg)
					{
						_horizontal->bind_event(reg);
						_vertical->bind_event(reg);
					}
					if (scroll_step.x > 0)
					{
						_horizontal->set_step(scroll_step.x); scroll_step.x = 0;
					}
					if (_vertical)
					{
						if (scroll_step.y > 0)
						{
							_vertical->set_step(scroll_step.y); scroll_step.y = 0;
						}
						if (scroll_count > 0)
						{
							_horizontal->scroll_count = scroll_count;
							_vertical->scroll_count = scroll_count;
							scroll_count = 0;
						}
					}
				}
			}
			for (auto it : _child->v)
			{
				if (it->visible)
					it->update(t);
			}

			{
				auto scroll = _horizontal;
				for (int i = 0; i < 2; i++, scroll++)
				{
					if (!_has_scroll)
						scroll->set_visible(false);
					scroll->update(t);
				}
			}

		}
		int view_u::mouse_motion(const glm::ivec2& p, bool ts)
		{
			int ret = 0;
			if (_horizontal)
			{
				auto scroll = _horizontal;
				for (int i = 0; i < 2; i++, scroll++)
				{
					if (!scroll->visible)continue;
					bool ic = scroll->contains(p, 0);
					if (ic)
					{
						ret++;
						break;
					}
				}
			}
			if (ret)
			{
				return ret;
			}
			// 子控件不独占事件
			bool contained = false;
			for (auto vt1 = v.rbegin(); vt1 != v.rend(); ++vt1)
			{
				auto& vt = vt1->second;
				for (auto it = vt.rbegin(); it != vt.rend(); ++it) {
					auto bt = *it;
					if (!bt->visible)
						continue;
					int idx = -1;
					auto ps = p - (scroll_pos * (glm::ivec2)bt->mask_scroll);
					contained = bt->contains(ps, &idx);
					int cc = 0;
					do {
						if (!contained)break;
						ret++;
						if (bt->mouse_motion_cb)
						{
							cc = bt->mouse_motion_cb(bt, ps, ts);
						}
						cc += bt->mouse_motion(ps, ts);
						ret += cc;
					} while (0);
					bt->set_hit((!cc && contained && ts), idx, ps + bt->hotd.pos);

					if (contained)
						break;//old_contained = true;//
				}
				if (contained)
					break;
			}
			return _only_e ? ret : 0;
		}
		void view_u::on_scroll(int wheel, int wheelH)
		{
			if (_vertical && _vertical->scroll_cb)
			{
				_vertical->scroll_cb(_vertical, wheel, wheelH);
			}
		}

		void view_u::up_scroll(cpg_time* t, glm::vec2 ws, int padd, int item_height, int n, glm::ivec4& clip, int& nheight, int& max_content_width)
		{
			if (!_has_scroll || !_horizontal)return;
			int nh = n;
			if (nh != nheight || max_content_width > 0)
			{
				glm::ivec2 mk = { max_content_width > 0 , 1 };
				auto ss = ws;
				ss -= padd * 2;
				clip = { 0,0, ss };
				ss.x -= bpos.x;
				ss.y -= bpos.w;
				nheight = nh;
				glm::vec2 pos[2] = { {padd + bpos.x,ws.y - padd - scroll_width },{ws.x - padd - scroll_width , padd + bpos.w} };
				if (bpos.y > 0 && pos[0].y > bpos.y)
				{
					pos[0].y = bpos.y;
				}
				if (bpos.z > 0 && pos[1].x > bpos.z)
				{
					pos[1].x = bpos.z;
				}
				// vc.x滚动条长度，vc.y内容长度
				glm::vec2 vc[2] = { {ss.x - scroll_width, max_content_width},{ss.y - scroll_width, nh * item_height} };

				int v0 = vc[0].y - vc[0].x;
				int v1 = vc[1].y - vc[1].x;
				if (v0 < 1)
				{
					vc[1].x += scroll_width;
				}
				else {
					clip.w -= scroll_width;
				}
				if (v1 < 1)
				{
					vc[0].x += scroll_width;
				}
				else {
					clip.z -= scroll_width;
				}
				{
					_horizontal->set_visible(v0 > 0);
					_vertical->set_visible(v1 > 0);
				}
				auto scroll = _horizontal;
				for (int i = 0; i < 2; i++, scroll++)
				{
					if (mk[i] != 1)continue;
					// 滚动条长度，内容长度，滚动条宽度
					int vx = scroll->set_view_content(vc[i].x, vc[i].y, scroll_width, scroll_scale);
					scroll->set_visible(vx > 0);
					scroll->set_position(true);
					scroll->set_pos(pos[i]);
					//scroll->update_content(true);
					// 背景色
					scroll->set_color(0xffc2c2c2, 0xffffffff, 0x20666666);
				}
				max_content_width = 0;
			}
		}


		// todo div_u
		div_u::div_u()
		{
			is_div = true; _ui = new asyn_queue_x<ui::base_u*>();
		}

		div_u::div_u(div_u* p)
		{
			is_div = true; dparent = p; _ui = new asyn_queue_x<ui::base_u*>();
		}

		div_u::~div_u()
		{
			if (_udfreecb)
			{
				_udfreecb(_ud);
			}
			for (auto it : _gcimg)
			{
				it->ref_count--;
				if (it->ref_count == 0)
				{
					delop(it);
				}
			}
			if (_ckm && tm)
			{
				tm->free_mem(_ckm, 1);
			}
			if (form)
			{
				form->_em->pop(_listen);
				if (!_autofree)
					form->pop_ui(this);
			}
			delop(tm);
			//delop(tdc);
			delop(_img);
			clear();
			delop(_ui);
			if (isnew)
			{
				delop(pdt);
				delop(_listen);
				delop(lt);
				delop(ust);
				//printf("free reg %p\n", reg);
				delop(reg);
			}
		}

		void div_u::set_root()
		{
			if (isnew || dparent)
				return;
			isnew = true;
			{
				reg = new entt::registry();
				e = reg->create();
				_listen = new event_data_cx();
				lt = new layout_text();
				ust = new font_style_c();
				auto& dt = reg->emplace<div_text_info_e>(e);
				pdt = dt.ptext = new item_box_info();
				dt.lt = lt;
				tm = new gtac_c();
				ptm = tm;
				tm->ctx = get_lts();
				auto& dee = reg->emplace<div_event_et>(e);
				dee.p = this;
				_listen->set_athread();
				dee.mouse_motion = [](const glm::ivec2& p, ui::div_u* d, bool ts)->int
					{
						if (d)
							return d->mouse_motion(p, ts);
						return 0;
					};
				em_system::bind_listen(_listen, reg);
			}
		}
		bool div_u::is_root()
		{
			return isnew;
		}
		int div_u::memsize()
		{
			return sizeof(div_u) + isnew ? (sizeof(event_data_cx) + sizeof(layout_text) + sizeof(font_style_c) + sizeof(entt::registry)) : 0;
		}

		void div_u::set_event_master(sdl_form* p, bool front)
		{
			assert(!parent);
			do {
				if (parent || (p && form == p))
					break;
				if (form)
				{
					form->_em->pop(_listen);
					if (!p)
					{
						form->pop_ui(this);
						break;
					}
				}
				if (!p)
				{
					break;
				}
				{
					form = p;
					event_src_et* et = 0;
					ecs_at::get_ptr(reg, e, et);
					auto& est = *et;
					est.lt = lt;
					est.p = p->_em;
					est.ctx = p->_ctx;
					est.form = p;
					est.edc = _listen;
					if (front)
					{
						_listen->pridata = this;
					}
					p->_em->push(_listen);
					p->push_ui(this);

				}
			} while (0);
		}

		void div_u::set_cp2(div_u* p)
		{
			assert(p && p->reg);
			if (isnew || !p || !p->reg)return;

			if (reg && reg != p->reg)
			{
				printf("free reg %p\t%d\n", reg, e);
				reg->destroy(e);
			}
			reg = p->reg;
			//e = reg->create();
			_listen = p->_listen;
			lt = p->lt;
			ust = p->ust;
			pdt = p->pdt;
			ptm = p->ptm;
			//push(p);
		}

		void div_u::set_has_event(bool is)
		{
			has_event = is;
		}

		ui::base_u* div_u::get_idxc(int idx)
		{
			if (idx < 0 || idx >= _ui->v.size())return nullptr;
			return _ui->v[idx];
		}

		float div_u::get_thickness()
		{
			return r4c.thickness;
		}

		glm::vec2 div_u::get_pos()
		{
			glm::vec2 ret;
			if (isnew)
				ret = _listen->get_nodepos();
			else
				ret = base_u::get_pos();
			return ret;
		}

		void div_u::set_pos(const glm::vec2& pos1)
		{
			if (pos == pos1)
				return;
			if (isnew)
			{
				_listen->set_nodepos(pos1);
			}
			else {
				base_u::set_pos(pos1);
				styles["pos"] = { pos1.x, pos1.y };
			}
		}
		void div_u::set_dragable(bool is_dragable) {
			if (isnew)
				_listen->set_dragable(is_dragable);
		}

		bool div_u::get_dragable()
		{
			return (isnew) ? _listen->get_dragable() : false;
		}

		void div_u::set_size(const glm::vec2& s)
		{
			if (!(s.x > 0 && s.y > 0))return;
			if (isnew)
			{
				_listen->set_v4(0, { 0, 0, s }, true);
			}
			base_u::set_size(s);
		}

		void div_u::set_font_family(hz::tt_info** font_family, int count)
		{
			if (lt)
			{
				lt->set_font_family(font_family, count);
			}
		}

		void div_u::set_bf_color(uint32_t border, uint32_t fill, float rounding, float thickness)
		{
			r4c.border = border;
			r4c.fill = fill;
			r4c.rounding = rounding;
			r4c.thickness = thickness;
		}

		void div_u::set_font_family(layout_text* p)
		{
			if (lt && p)
			{
				lt->cpy(p);
			}
		}

		void div_u::set_color_bs(glm::ivec2 color, box_shadow_t* bs)
		{
		}

		base_u* div_u::at(size_t idx)
		{
			base_u* p = 0;
			if (_ui && _ui->v.size() > idx)
			{
				p = _ui->v[idx];
			}
			return p;
		}
		base_u* div_u::push(base_u* p)
		{
			if (p)
			{
				if (p->is_div)
				{
					p->dparent = this;
				}
				if (p->parent)
				{
					assert(0);// p->parent && "先删除再添加");
					return p;
				}
				p->parent = this;
				if (!p->ptm && ptm)
					p->ptm = ptm;
				p->set_lt(get_lts());
				p->bind_event(reg);
				_ui->push(p);
				_ui->rad([=](ui::base_u* p, bool a) {
					if (a)
					{
						p->set_order(p->get_order());
					}
					});
			}
			return p;
		}
		void div_u::pop(base_u* p)
		{
			if (p->reg != reg)
			{
				if (0 != (int)p->e && p->reg)
				{
					p->reg->destroy(p->e);
				}
				p->e = ety_null; p->reg = 0;
			}
			p->parent = 0;
			_ui->pop(p);

		}
		void div_u::push(Image* p)
		{
			p->ref_count++;
			_gcimg.insert(p);
		}
		void div_u::pop(Image* p)
		{
			p->ref_count--;
			_gcimg.erase(p);
		}
		void div_u::clear()
		{
			auto& child = _ui->v;
			_ui->rad();
			for (auto p : child)
			{
				delop(p);
			}
			child.clear();
		}
		size_t div_u::count()
		{
			return _ui->v.size();
		}

		void div_u::mk_blur(const glm::ivec2& ss, int r, int blur, uint32_t color)
		{
			auto b = blur;
			if (b > 0)
			{
				/*path_stb path[1] = {};
				effect_cx ec[1] = {};
				box_shadow_t bs = {};
				bs.blur = blur; bs.color = color;
				bs.h_shadow = 0; bs.v_shadow = 0;
				bs.count = 2;
				path->BeginPath();
				path->Rect(0, 0, ss.x, ss.y, r);
				glm::vec2 ps = { blur * 2 , blur * 2 };
				auto s = ss;
				s += blur * 4;
				auto img = ec->do_box_shadow(&bs, path, _img, s, ps);
				_img = img;
				bool sa = false;
				if (sa)
					img->saveImage("temp/mk_blur_" + std::to_string((uint64_t)this) + ".png");
				blurpos += glm::vec2{ bs.h_shadow, bs.v_shadow };

				image_cs& rst = _blur;
				rst.img = img;
				if (img)
				{
					rst.pos = -ps;// { -blur, -blur };
					rst.size = { img->width, img->height };
					rst.rect = { 0,0, rst.size };
				}*/
			}
		}
		div_text_info_e div_u::get_lts()
		{
			return div_text_info_e{ lt, pdt };
		}

		// 窗口会先执行update然后才会执行draw
		void div_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			//auto tid = get_tid();
			static double ttk = 0;
			double nt = t->time;
			if (nt > ttk)
			{
				ttk = nt;
				//log_format("draw ui: %f\t%lld\n", t->time, tid);
			}

			if (!_ui)return;
			if (!visible)
			{
				printf("visible false\n");
			}
			bool bf = get_alpha_f(r4c.border) > 0, ff = get_alpha_f(r4c.fill) > 0;
			float tk = r4c.thickness * bf;
			auto& child = _ui->v;//_dv;//
			auto spos = pos + pos1 + rpos;
			auto ws = get_size();
			if (ff)
				canvas->draw_rect(spos, ws, 0, r4c.fill, r4c.rounding, r4c.thickness);
			if (_blur.img)
				canvas->draw_shape(&_blur, 1, 0, spos + blurpos);
			if (_backimg.img)
				canvas->draw_shape(&_backimg, 1, 0, spos);
			canvas_cx* cpx = nullptr;
			if (_has_clip) {
				clip.x = spos.x + tk;
				clip.y = spos.y + tk;
				clip.z = ws.x - tk * 2.0;
				clip.w = ws.y - tk * 2.0;
				cpx = canvas;
			}
			{
				clicp_x ccp(cpx, clip, true);
				for (auto it : child)
				{
					if (it->visible && it->isdrawble)
					{
						it->draw(t, canvas, spos);				// 控件自定义绘制
					}
				}

				// 统一绘制的文本
				for (auto it : child)
				{
					if (!it->isdrawble || !it->visible || !it->auto_drawtext || !it->dtc || it->dtc->count <= 0)
						continue;
					if (it->mtt)
						canvas->push_ubo(it->mtt);
					canvas->draw_shape(it->dtc, 1, spos + it->pos + it->tpos);
					if (it->mtt)
						canvas->pop_ubo();
				}
				//if (ws.y > 500 && ws.x > 500)
				//{
				//	auto ps = spos;
				//	ws *= 0.251;
				//	ps += ws; 
				//	//canvas->DrawGrid(ps, 10, 40, { 0xaf00ff00,0xaf0000ff,0x8fcccccc }, 2);
				//	canvas->DrawGrid2d(ps, 10, 40, { 0x8fcccccc,0xaf00ff00 }, { 1,1 });
				//}
			}
			if (bf)
				canvas->draw_rect(spos, ws, r4c.border, 0, r4c.rounding, r4c.thickness);


		}
		// 更新ui数据事件之类
		void div_u::update(cpg_time* t)
		{
			if (!_ui)return;
			if (dparent)
			{
				set_cp2(dparent); dparent = 0;
			}

			if (isnew)
			{
				auto no = get_order();
				if (_listen->order != no)
				{
					_listen->order = no;
					if (_listen->_em)
						_listen->_em->up_sort();
				}
				_listen->visible = visible;
				_listen->at_run(false); 		// 根容器处理事件
				rpos = _listen->get_nodepos();	// 获取全局坐标

				if (has_mte(mouse_type_e::e_enter))
				{
					dec_mte(mouse_type_e::e_enter);
				}

				if (!visible)
				{
					return;
				}
				// 修改鼠标指针
				set_hover_mc((int)mouse_cursor::SC_ARROW, 0);
			}
			auto r = _ui->sort_inc;
			if (_ui->size())
			{
				_ui->rad([=](ui::base_u* p, bool a) {
					if (a)
					{
						p->set_order(p->get_order());
					}
					});
				r = _ui->sort_inc - r;
			}
			_ui->sort_inc += sort_inc;
			sort_inc = 0;

			bool ic = _ui->sort([](ui::base_u* p1, ui::base_u* p2) { return (p1->order < p2->order); });
			//if (r > 0)
			//{
			//	_dv.resize(_ui->v.size());
			//	memcpy(_dv.data(), _ui->v.data(), _dv.size() * sizeof(void*));
			//}
			if (ic || vic > 0)
			{
				styles["auto_left"] = _auto_left;
				vic = do_layout(get_size(), styles, _ui->v.data(), _ui->v.size());
			}

			if (visible)
			{
				for (auto it : _ui->v)
				{
					if (it->visible)
					{
						it->update(t);
					}
				}
				hideinc = 0;
			}
			else {
				hideinc++;
				if (hideinc == 1)
					set_mouse_cursor((int)mouse_cursor::SC_ARROW);;
			}
			if (isnew)
			{
				update_ecs(t);
			}
			base_u::update(t);
		}
		// todo 需要优化
		int div_u::mouse_motion(const glm::ivec2& p, bool ts) {
			auto ps = p - (glm::ivec2)pos;
			int ret = 0;
			if (!_ui)return 0;
			auto& child = _ui->v;
			for (auto it = child.rbegin(); it != child.rend(); ++it) {
				auto bt = *it;
				if (!bt->visible)
					continue;
				int idx = -1;
				bool contained = bt->contains(ps, &idx);
				int cc = 0;
				do {
					if (!contained)break;
					ret++;
					if (bt->mouse_motion_cb)
					{
						cc = bt->mouse_motion_cb(bt, ps, ts);
					}
					//if (bt->is_div)
					{
						cc +=/* dynamic_cast<div_u*>*/(bt)->mouse_motion(ps, ts);
					}
					ret += cc;
				} while (0);
				if (!cc && contained && ts)
				{
					bt->set_hit(true, idx, ps - bt->hotd.pos);
				}
				else {
					bt->set_hit(false, idx, ps + bt->hotd.pos);
				}
				if (contained)
				{
					return ret;
				}
			}
			return 0;
		}
		void div_u::set_lt(div_text_info_e t)
		{
			lt = t.lt;
			pdt = t.ptext;
		}
		void div_u::on_parent_visible(bool is)
		{
			for (auto it : _ui->v)
			{
				if (it)
				{
					it->on_parent_visible(is);
				}
			}
		}
		void div_u::update_ecs(cpg_time* t)
		{
			// 更新动画
			{
				auto view = reg->view<action_t>();
				for (auto e : view) {
					auto& d = view.get<action_t>(e);
					float ct = 0;
					int hr = d.updata_t(t->deltaTime, ct);
					auto ptr = d.ptr;
					if (ptr->aidx.x >= 0)
					{
						auto ps = d.get<glm::vec2>(ptr->aidx.x);
						if (ps)
							ptr->set_pos(*ps);// 移动
					}
					if (ptr->aidx.y >= 0)
					{
						auto ss = d.get<glm::vec2>(ptr->aidx.y);
						if (ss)
						{
							ptr->set_size(*ss);//变形
						}
					}
					if (hr == 1) {
						d.clear();
						reg->remove<action_t>(e);// 结束动画
					}


				}
			}
			{
				// 显示隐藏
				auto view = reg->view<ui_show_t>();
				for (auto e : view) {
					auto& d = view.get<ui_show_t>(e);
					if (d.ptr)
					{
						if (d.wait > 0)
						{
							d.wait -= t->deltaTime;
							continue;
						}
						else
						{
							d.ptr->set_visible(d.v);
						}
					}
					reg->remove<ui_show_t>(e);
				}
			}
			// 更新auto大小
			//if (isupauto)
			do {
				auto view0p = reg->view<csize_empty>();
				int inc = 0;
				for (auto e : view0p) {
					auto& v = view0p.get<csize_empty>(e);
					reg->remove<csize_empty>(e);
					inc++;
				}
				if (inc == 0)
				{
					break;
				}
				auto view = reg->view<setsize_t>();
				for (auto e : view) {
					auto& v = view.get<setsize_t>(e);
					auto ns = v.pad;
					auto pr = v.ptr->parent;
					if (!pr || v.ptr->aidx.y >= 0)
					{
						continue;
					}
					auto pss = pr->get_size();
					auto olds = v.ptr->get_size();
					auto nx = (ns.x + pss.x) * ns.z;
					auto ny = (ns.y + pss.y) * ns.w;
					if (nx > 0 && ns.z > 0)
					{
						olds.x = nx;
					}
					if (ny > 0 && ns.w > 0)
					{
						olds.y = ny;
					}
					v.ptr->set_size(olds);
				}
			} while (0);

		}
#endif
		//!div_u

		// todo scroll_u
		scroll_u::scroll_u()
		{
			has_drag = true;
			_is_click = true;
			hotd.last = hot;
			set_arrow_btn(false);
			drag_cb = on_drag_cb;
			// 滑块颜色
			_mbc.background_color = 0xffc2c2c2;
			_mbc.hover_color = 0xffe2e2e2;
			// 背景色
			_mbc.active_background_color = 0x80666666;
			scroll_cb = [](base_u* ptr, int wheel, int wheelH) {
				auto sp = (scroll_u*)ptr;
				if (sp)
				{
					if (wheel > 0)
						sp->set_up(wheel * sp->scroll_count);
					if (wheel < 0)
						sp->set_down(-wheel * sp->scroll_count);
				}
				};
		}

		scroll_u::~scroll_u()
		{
		}

		void scroll_u::set_dir(int dir)
		{
			_dir = (dir == 0 ? 0 : 1);
		}

		void scroll_u::set_color(uint32_t col, uint32_t hover, uint32_t back)
		{
			_mbc.background_color = col;
			_mbc.hover_color = hover;
			// 背景色
			_mbc.active_background_color = back;
		}

		void scroll_u::set_step(int s)
		{
			_step = s;
			if (_step < 1)
			{
				_step = 1;
			}
		}

		// 输入：视图大小、内容大小、滚动宽度 ,count
		// 输出：x水平大小，y为水平的滚动区域宽，z是否显示滚动条
		glm::vec3 calcu_scrollbar_size(int vps, int content_width, int scroll_width, int count)
		{
			//计算去掉按钮时视图大小
			auto scw = scroll_width;
			scw = scroll_width * count;
			auto dif = vps - scw;
			bool isx = (dif < content_width) && vps < content_width;

			auto calc_cb = [](double vw, double cw, double scw, double sw)
				{
					double win_size_contents_v = cw,
						win_size_avail_v = vw - scw,
						scrollbar_size_v = win_size_avail_v;
					auto win_size_v = std::max(std::max(win_size_contents_v, win_size_avail_v), 1.0);
					auto GrabMinSize = sw;
					auto grab_h_pixels = clamp(scrollbar_size_v * (win_size_avail_v / win_size_v), GrabMinSize, scrollbar_size_v);
					auto grab_h_norm = grab_h_pixels / scrollbar_size_v;
					return glm::vec2{ grab_h_pixels, scrollbar_size_v };
				};
			auto x = calc_cb(vps, content_width, scw, scroll_width);
			if (!(x.x < x.y) || !isx)
				x.x = 0;
			return glm::vec3(x.x, x.y, isx);
		}

		int scroll_u::set_view_content(int view, int content, int width, float scale)
		{
			if (_view_size == view && _content_size == content)return thumb_size_m.x;
			//if (_offset > 0 /*&& content < _content_size*/)return;
			//int ns = content - thumb_size_m[thumb_size_m.z];
			//if (ns < _offset)
			//{
			//	_offset = ns;
			//}
			_view_size = view;
			_content_size = content;
			_rc_width = width;
			_btn_width = _thumb = width;

			glm::vec2 bs = { _rc_width , _rc_width };
			auto pts = bs;
			bs *= scale;
			auto rg = bs.x * 0.5;
			pts -= bs;
			pts *= 0.5;
			if (!has_arrow_btn)
			{
				_btn_width = pts.x;
			}
			// 箭头、滑块按钮初始大小一样
			int vrc = _view_size - _rc_width;
			auto vs = _view_size;
			glm::ivec3 sbs = calcu_scrollbar_size(vs, _content_size, _btn_width, 2);
			thumb_size_m = sbs;
			// todo 滚动范围
			auto cvs = thumb_size_m.y;// _view_size - _rc_width * 2;
			dcv = _content_size - cvs;
			if (dcv < 0)
			{
				dcv = 0;
			}
			if (sbs.x == 0)
			{
				cvs += _rc_width;
			}

			bool is0 = (sbs.x > 0);

			// 背景
			if (sbs.x > 0 && dcv > 0)
			{
				glm::vec2 rc = {};
				rc[_dir] = _view_size;
				rc[(_dir ? 0 : 1)] = _rc_width;
				set_size(rc);
			}
			// corner 边角
			if (sbs.x - sbs.y == 0)
			{
			}
			// 滑块
			auto& thumb = hot[_thumb_idx - 1];
			thumb[2] = thumb[3] = _rc_width;
			thumb[2 + _dir] = thumb_size_m.x;
			thumb[_dir] = _btn_width;
			// 按钮
			for (size_t i = 1; i < 3; i++)
			{
				auto& abtn = hot[i];
				abtn[2] = abtn[3] = _rc_width;
				if (i > 1)
					abtn[_dir] = _view_size - _rc_width;
			}

			set_thumb_pos(_offset_f);
			_r2[0].size[_dir] = _view_size;
			_r2[0].size[_dir ? 0 : 1] = _rc_width;
			_r2[0].pos = {};
			_r2[0].t = &_stc[0];
			_r2[1].t = &_stc[1];
			auto ks = _r2[0].size;
			_r2[1].size = scale * ks;
			_r2[1].size[_dir] = thumb_size_m.x;
			if (thumb_size_m.x > 0 && thumb_size_m.x < _r2[1].size[_dir ? 0 : 1])
			{
				_r2[1].size[_dir] = _r2[1].size[_dir ? 0 : 1];
			}
			_stc[0].set_rounding(_rc_width * 0.5 - 1);
			_stc[1].set_rounding(rg - 1);
			_r2[1].pos[_dir ? 0 : 1] = pts.x;
			_btn[0].size = _btn[1].size = bs;
			_btn[0].t = &_stc[2];
			_btn[1].t = &_stc[3];
			_btn[0].pos = _btn[1].pos = pts;
			_btn[1].pos[_dir] += hot[2][_dir];
			_btn[0].spos = _btn[1].spos = 50;
			//尖角方向，0上，1右，2下，3左
			glm::ivec2 dir = { 3,1 };
			if (_dir)
			{
				dir = { 0, 2 };
			}
			_btn[0].dir = dir.x; _btn[1].dir = dir.y;
			return thumb_size_m.x;
		}

		void scroll_u::set_arrow_btn(bool is)
		{
			has_arrow_btn = is;
			hotd.n = has_arrow_btn ? 3 : 1;
		}

		// 向上滚动n个单位_step，-1到顶
		bool scroll_u::set_up(int n)
		{
			bool ret = false;
			if (dcv > 0)
			{
				auto y = _offset;
				if (n == -1)
				{
					_offset = 0;
				}
				else if (_offset - (n * _step) >= 0)
				{
					_offset -= n * _step;
				}
				else {
					_offset = 0;
				}
				if (y != _offset)
				{
					_offset_f = _offset / dcv;
					set_thumb_pos(_offset_f);
					_is_down = false;
					up_pos(_offset_f, false);
					ret = true;
				}
			}
			else {
				_offset = 0;
				_offset_f = 0;
				set_thumb_pos(_offset_f);
				_is_down = false;
				up_pos(_offset_f, false);
				ret = true;
			}
			return ret;
		}

		bool scroll_u::set_down(int n)
		{
			bool ret = false;
			if (dcv > 0)
			{
				auto y = _offset;
				if (n == -1)
				{
					_offset = dcv;
				}
				else if (_offset + (n * _step) <= dcv)
				{
					_offset += n * _step;
				}
				else {
					_offset = dcv;
				}
				if (y != _offset)
				{
					_offset_f = _offset / dcv;
					set_thumb_pos(_offset_f);
					_is_down = true;
					up_pos(_offset_f, false);
					ret = true;
				}
			}
			else {
				_offset = 0;
				_offset_f = 0;
				set_thumb_pos(_offset_f);
				_is_down = false;
				up_pos(_offset_f, false);
				ret = true;
			}
			return ret;
		}


		void scroll_u::update_content(bool is)
		{
			if (is)
			{
				auto evs = _view_size;
				auto cs = _content_size;
				auto ey = evs / _step;
				ey *= _step;
				dcscroll(cs, ey, _step, _offset);
			}
		}


		void scroll_u::on_down()
		{
			//auto hp = get_hover();
			auto bp = get_btnet();
			if (bp)
			{
				if (bp->inc == 0) {
					down_idx = bp->pidx;
					first_down = bp->pos;

				}
				bp->inc++;
			}
		}

		bool scroll_u::on_keep_down(cpg_time* t, const glm::vec2& pos1)
		{
			bool ret = false;
			cudown_ms += t->deltaTime;
			if (cudown_ms < down_ms)
			{
				return ret;
			}
			if (down_idx == _thumb_idx)
			{
				return ret;
			}
			cudown_ms = 0;

#if 1
			// 箭头按钮
			if (down_idx == _thumb_idx + 1)
			{
				ret = set_up(1);
			}
			else if (down_idx == _thumb_idx + 2)
			{
				ret = set_down(1);
			}
			if (down_idx == 0)
			{
				auto tbrc = hot[0];
				down_idx = _thumb_idx;				// 转移控制权到滑块实现拖动
				auto nps = pos1[_dir] - tbrc[2 + _dir] * 0.5;
				set_thumb_posi(nps);

				up_pos(_offset_f, true);
				first_pos[_dir] = nps;
				ret = true;
			}
#endif
			return ret;
		}

		void scroll_u::on_dragstart(const glm::vec2& pos1)
		{
			auto ps = pos1 - pos;
			glm::ivec2 tps = hot[0];
			first_pos = (glm::ivec2)ps - tps;
		}

		void scroll_u::on_drag(const glm::vec2& pos1)
		{
			do {
				auto kf = _offset_f;
				if (down_idx == _thumb_idx)
				{
					auto ps = pos1 - pos;
					// 设置水平滚动滑块坐标
					auto ps1 = ps[_dir] - first_pos[_dir];
					set_thumb_posi(ps1);
				}
				else {
					break;
				}
				if (kf > _offset_f || kf < _offset_f)
					up_pos(_offset_f, true);
			} while (0);
		}

		bool scroll_u::is_resize_content(int w)
		{
			int cs = _content_size;
			return cs != w || isup_listen;;
		}

		void scroll_u::set_thumb_pos(float ps)
		{
			auto npos = ps;
			npos *= (thumb_size_m.y - thumb_size_m.x);
			set_thumb_posi(npos + _btn_width);
		}
		void scroll_u::set_thumb_posi(int ps)
		{
			{
				auto& it = hot[0];
				int ix = it[_dir];
				it[_dir] = ps;					// 设置水平滚动滑块坐标
				int t = _btn_width + thumb_size_m.y - thumb_size_m.x;
				if (it[_dir] < _btn_width)
				{
					it[_dir] = _btn_width;
				}
				else if (it[_dir] > t)
				{
					it[_dir] = t;
				}
				// 计算百分比
				_offset_f = (double)(it[_dir] - _btn_width) / (thumb_size_m.y - thumb_size_m.x);
				if (ix != it[_dir])
					isup_listen = true;
			}
		}
		void scroll_u::up_pos(float ps, bool is)
		{
			auto old = _offset;
			if (is)
			{
				_offset = ps * dcv;
				_is_down = (old < _offset);
			}
			if (change_cb)
				change_cb(this);
		}
		void scroll_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto spos = pos + pos1;
			// 滑道
			// 滑块
			if (_r2->t)
				canvas->draw_shape(_r2, 2, spos);
			// 箭头按钮
			if (has_arrow_btn)
				canvas->draw_shape(_btn, 2, spos);
		}

		void scroll_u::update(cpg_time* t)
		{

			if (is_down(0))
			{
				on_down();
				on_keep_down(t, first_down);
			}

			// 滑道坐标
			_r2[1].pos[_dir] = hot[0][_dir];
			_stc[0].color = 0;
			_stc[0].fill = _mbc.active_background_color;
			for (int i = 1; i < 4; i++)
			{
				_stc[i].fill = _mbc.background_color;
				_stc[i].color = 0;
			}
			auto hp = get_hover();
			if (hp)
			{
				int idx = hp->idx;
				switch (idx) {
				case 0:
					break;
					//case 1:
					//	break;
					//case 2:
					//	break;
					//case 3:
					//	break;
				default:
					_stc[idx].fill = _mbc.hover_color;
					break;
				}
			}
			if (is_down(0))
			{
				_stc[1].fill = _mbc.hover_color;
			}
			base_u::update(t);
		}

		void scroll_u::on_drag_cb(base_u* ptr)
		{
			auto sp = (scroll_u*)ptr;
			if (sp)
			{
				auto dp = sp->get_drag_ut();
				if (dp)
				{
					if (dp->state == 0)
					{
						sp->on_dragstart(dp->pos); dp->last = dp->pos; dp->last--;
					}
					if (dp->last != dp->pos)
					{
						sp->on_drag(dp->pos);
						dp->last = dp->pos;
					}
				}
			}
		}
		// todo ComboBox
		combobox_u::combobox_u()
		{
			style = njson::parse(R"({
			  "border_color": 0,
			  "background_color": "0x15ff8000",
			  "hover_color": "0x25ff8000",
			  "hover_border_color": "0x80ff8000",
			  "active_background_color": "0x35ff8000",
			  "active_border_color": "0xf5ff8000"
			})");
			upbd = false;
			input_u::set_multi_line(0);
			set_color(text_color, 0, 0xFF999999, -1, 0);
			auto anip = new anim_a();
			anip->acce = 5; // 5倍加速
			anip->urx = -180; // 旋转角度dot
			anip->set_acce(5);
			_icon_ra = anip;
			set_rowcount(1, 0, 0);
			set_isedit(false);
		}

		combobox_u::~combobox_u()
		{
			//if (haspage)
			//	delop(mpage);
			delop(_icon_ra);
			//delop(tdc0);
		}
		void combobox_u::set_type(int t, bool is_read) {

		}

		void combobox_u::set_placeholder(const void* str)
		{
			if (str)
				input_u::set_placeholder((char*)str);
		}
		void combobox_u::add(const char* str, int len)
		{
			{
				len = len < 0 ? strlen(str) : len;
				item_c itc = { nullptr, std::move(t_string(str, len)) };
				items.push_back(itc);
				//if (mpage)
				//{
				//	auto p = mpage->add_item(str, len);
				//	if (p)
				//		p->click_hide = !_multi;
				//	vp = true;
				//}
				//else 
				{
					tstr.push_back(itc.text);
				}
			}
		}
		void combobox_u::add(const void* str, int len)
		{
			add((char*)str, len);
		}

		void combobox_u::remove(int idx)
		{
			delitems.insert(idx);
			vp = true;
		}



		void combobox_u::clear()
		{
			//ltd_clear(lst);
			items.clear();
		}

		button_u* combobox_u::get_idx(int idx)
		{
			button_u* p = 0;
			do
			{
				if ((idx < 0 || idx >= items.size()))break;
				p = items[idx].btn;
			} while (0);
			return p;
		}
		item_c* combobox_u::get_it(int idx)
		{
			item_c* p = 0;
			do
			{
				if ((idx < 0 || idx >= items.size()))break;
				p = &items[idx];
			} while (0);
			return p;
		}

		void combobox_u::set_icon(image_m_cs* p, int idx)
		{
			do
			{
				//if (!lst)break;
				if (idx < 0)
				{
					//icon_select = *p;
					//icon_select.size = {};
				}
			} while (0);
		}

		//void combobox_u::set_ctx(sdl_form* pform, layout_text* plt)
		//{
		//	_pform = pform;
		//	_plt = plt;
		//	haspage = true;
		//	mpage = new menu_page_t(_pform->menuctx);
		//	mpage->style = style;
		//	mpage->on_click = [=](menu_item_t* pt, int idx) {
		//		printf("click menu: idx:%d\t%s\n", idx, pt->text.c_str());
		//		do
		//		{
		//			auto cp = (combobox_u*)pt->ud;
		//			if (!cp)break;
		//			//cp->set_text(pt->text.c_str());
		//			cp->mz = true;
		//			cp->ispop = false;
		//			cp->set_value(idx);
		//		} while (0);
		//	};
		//	for (auto& it : tstr)
		//	{
		//		auto p = mpage->add_item(it.c_str(), it.size());
		//		if (p)
		//			p->click_hide = !_multi;
		//	}
		//	if (tstr.size())
		//		vp = true;
		//}

		void combobox_u::set_isedit(bool is)
		{
			is_edit = is;
			set_input_cursor(is);
			set_read_only(!is);
		}
		void combobox_u::set_multi(bool is)
		{
			_multi = is;
			//if (mpage)
			//{
			//	auto p = mpage->ptr();
			//	p->click_hide = is;
			//}
		}
		void combobox_u::set_fontheight(int h)
		{
			if (h > 5)
				_font_height = h;
			set_layout(0);
		}
		void combobox_u::set_value(int idx)
		{
			do {
				if (!_multi)
				{
					_its.clear();
					_idx = idx;
					_its.insert(idx);
				}
				else {
					if (_idx == idx)
					{
						_its.erase(idx);
					}
					else {
						_its.insert(idx);
					}
				}
				//if (!mpage)break;
				//auto p = mpage->ptr();

				//if (p && idx < p->child.size())
				{
					std::string str;
					for (auto id : _its)
					{
						auto it = tstr[id].c_str();
						if (str.size())str += spstr;
						str += it;
					}
					if (str.empty())
						printf("error combobox\n");
					set_text(str.c_str());
				}
				//printf("点中：%d\n", idx);
			} while (0);
		}

		void combobox_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			input_u::draw(t, canvas, pos1);
			if (vpos != pos1)
				vpos = pos1;
			if (dtc)
			{
				auto spos = pos1 + pos;
				if (_icon_ra)
					canvas->push_ubo((motion_t*)_icon_ra);
				//canvas->draw_shape(&it->tdc->dtext, spos + it->pos + it->tdc->tpos);
				canvas->draw_shape(dtc, spos, 0);
				//em_system::draw_ui(canvas, &tdc0->sn, spos);
				if (_icon_ra)
					canvas->pop_ubo();
			}
		}

		void combobox_u::update(cpg_time* t)
		{
			if (!reg)return;
			if (vp)
			{
				vp = false;
				auto ws = get_size();
				itemy = (fontheight + 6);
			}
			bool isck = ick[0];
			bool isck0 = is_click(0);
			if (uck)//&& mpage && mpage->is_ud(this) && !mpage->get_visible() /*|| !is_input()*/)
			{
				//isck = true;
				uck = false;
			}
			//if (is_input())

			bool uck1 = uck;
			if (!mz)
			{
				if (isck || isck0)
				{
					ick[0] = 0;
					uck = !uck;
					printf("uck %d\t%d\n", uck, ick[0]);
					//if ((int)(_icon_ra->get()) == 1 && !mpage->get_visible())
					if (uck)
					{
						auto s2 = get_size();
						//auto ps1 = get_pos2() + get_fpos();
						auto ps2 = get_pos() + vpos + get_fpos();
						ps2.y += s2.y;
						//if (mpage)
						//{
						//	bool is = mpage->get_visible(), is1 = mpage->is_ud(this);
						//	printf("\tmpage %d\t%d\n", is, isw);
						//	do {
						//		if ((isw == 10) && is1) { ispop = false; break; }
						//		{
						//			mpage->set_ud(this);
						//			mpage->show(ps2, _pform);
						//			ispop = true;
						//		}
						//	} while (0);
						//}
					}
				}
			}
			if (rightch.size() && ptm)
			{
				// todo dtc

				style_text_cs cs = {};
				cs.font_height = fontheight;
				cs.ps = { 0.0, 0.5 };
				cs.box = get_size();
				cs.color = set_alpha_f(dcss.font_color, pha);

				auto p = ptm->get_text(rightch.c_str(), dtc, &cs);
				//p->pos1 = glm::vec2{ size1.x - tdc0->boxsize.x - 4, 0 };
				rightch.clear();
			}

			//if (mpage) {
			//	if (mpage->is_ud(this) && mz)
			//	{
			//		mz = false;
			//		uck = false;
			//	}
			//}
			bool iss1 = false;
			if (_icon_ra && dtc)
			{
				//获取全局坐标
				//auto ps = get_pos2() + tsn->pos;
				auto ps = get_pos() + vpos + dtc->pos;
				glm::vec2 rpos = dtc->rc;
				rpos.x *= 0.5;
				rpos.y *= 1.25;
				ps += rpos;
				int ik = _icon_ra->get() * 10.0;
				if (ik == 0)
				{
					ispop = false;
				}
				isw = ik;
				if (uck1 == uck && !uck && ik == 10)
					uck1 = true;
				_icon_ra->up_rotate(uck1 != uck, ps, t);
				iss1 = uck1 != uck;
			}

			/*		if (mpage)
					{

						if (mpage->is_ud(this) && !mpage->get_visible())
						{
							mz = false;
							uck = false;
							uck1 = uck1;
						}
						if (!mpage->get_visible())
						{
							mz = false;
						}

					}*/

			input_u::update(t);

			if (_idx != cidx && on_change) {
				cidx = _idx;
				auto t = input_u::c_str();

				if (_td)
				{
					if (_tk != "")
						(*_td)[_tk] = _idx;
					else
						(*_td) = _idx;
				}
				on_change(_idx, t);
			}
		}




		// todo base_u
		base_u::base_u()
		{
		}
		base_u::base_u(const glm::vec2& s) :size(s)
		{

		}
		base_u::~base_u()
		{
			if (_ucbs)
			{
				delop(_ucbs);
			}
			if (0 != (int)e && reg)
			{
				if (reg->valid(e))
					reg->destroy(e);
			}
		}

		void base_u::add_ucb(uint64_t idx, void* ud, void(*cb)(uint64_t idx, void*))
		{
			if (!_ucbs)
			{
				_ucbs = new std::vector<ucbinfo_t>();
			}
			if (_ucbs)
			{
				for (auto& it : *_ucbs)
				{
					if (it.id == idx)
					{
						it.ud = ud;
						it.cb = cb;
						return;
					}
				}
				_ucbs->push_back({ idx, ud, cb });
			}
		}
		// 直线移动，时间秒，目标，原坐标可选
		action_t* base_u::move2w(float mt, const glm::vec2& target, glm::vec2* src, float wait)
		{
			auto p = ecs_at::get_p<action_t>(reg, e);
			if (p)
			{
				p->ptr = this;
				p->count = 0;
				p->add_wait(wait);
				aidx.x = p->add(mt, src ? *src : pos, target);
			}
			return p;
		}
		// 在原坐标为原点增加移动
		action_t* base_u::move2inc(const glm::vec2& pad, float mt, float wait)
		{
			auto p = ecs_at::get_p<action_t>(reg, e);
			if (p)
			{
				p->ptr = this;
				p->count = 0;
				p->add_wait(wait);
				aidx.x = p->add(mt, pos, pos + pad);
			}
			return p;
		}
		action_t* base_u::at_size(const glm::vec2& dst, float mt)
		{
			auto p = ecs_at::get_p<action_t>(reg, e);
			if (p)
			{
				p->ptr = this;
				p->count = 0;
				auto ss = get_size();
				aidx.y = p->add(mt, ss, dst);
			}
			return p;
		}
		ui_show_t* base_u::wait_show(bool visible, float wait)
		{
			auto p = ecs_at::get_p<ui_show_t>(reg, e);
			if (p)
			{
				p->ptr = this;
				p->wait = wait;
				p->v = visible;
			}
			return p;

		}
		void base_u::set_autosize(const glm::ivec4& ps, bool enable)
		{
			if (enable)
			{
				auto p = ecs_at::get_p<setsize_t>(reg, e);
				p->pad = ps;
				p->ptr = this;
			}
			else {
				reg->remove_if_exists<setsize_t>(e);
			}
		}


		// 显示
		void base_ushow_e(bool v)
		{
			//auto p = ecs_at::get_p<ui_show_t>(reg, e);
			//if (p)
			//{
			//	p->ptr = this;
			//	p->v = v;
			//}

		}

		hover_et* base_u::get_hover() {
			hover_et* p = 0;
			return ecs_at::get_ptr0(reg, e, p);
		}

		drag_ut* base_u::get_drag_ut()
		{
			drag_ut* p = 0;
			ecs_at::get_ptr0(reg, e, p);
			return p;
		}
		click_et base_u::get_click_et()
		{
			click_et* p = 0, r = {};
			if (hit)
			{
				ecs_at::get_ptr0(reg, e, p);
				if (p) {
					r = *p;
					p->n = 0;
				}
			}
			return r;
		}
		button_et* base_u::get_btnet()
		{
			button_et* p = 0;
			return ecs_at::get_ptr0(reg, e, p);
		}

		bool base_u::is_down(int idx)
		{
			button_et* btn = ecs_at::get_btn_ptr(reg, e);
			return btn && btn->idx == idx && btn->button == 1;
		}
		bool base_u::is_click(int idx)
		{
			bool ret = hit && (has2c<click_ut>());

			return ret;
		}

		void base_u::update(cpg_time* t)
		{

			if (hit1 != hit)
			{
				hit1 = hit;
				if (hit_cb1)
					hit_cb1(this, hit1, uptr);
			}

		}
		void base_u::set_position(bool is_absolute)
		{
			styles["position"] = is_absolute ? flex::e_position::absolute : flex::e_position::relative;
			_absolute = is_absolute;
			if (parent)
			{
				parent->sort_inc++;
			}
		}
		void base_u::set_padding1(int v)
		{
			if (v > 0)
				styles["padding1"] = v;
		}
		void base_u::set_margin1(int v)
		{
			if (v > 0)
				styles["margin1"] = v;
		}
		void base_u::set_padding(int v)
		{
			if (v > 0)
				styles["padding"] = v;
		}
		void base_u::set_margin(int v)
		{
			if (v > 0)
				styles["margin"] = v;
		}
		void base_u::set_padding(njson v)
		{
			if (v > 0)
				styles["padding"] = v;
		}
		void base_u::set_margin(njson v)
		{
			if (v > 0)
				styles["margin"] = v;
		}
		void base_u::set_align(int justify_content, int align_content)
		{
			styles["justify_content"] = justify_content > 0 ? justify_content : (int)flex::e_align::align_space_evenly;
			styles["align_content"] = align_content > 0 ? align_content : (int)flex::e_align::align_space_evenly;
			//styles["direction"] = isrow ? flex::e_direction::row : flex::e_direction::column;
		}
		void base_u::set_direction(int d)
		{
			styles["direction"] = d;// isrow ? flex::e_direction::row : flex::e_direction::column;
		}
		void base_u::set_align_items(int align_items)
		{
			styles["align_items"] = align_items > 0 ? align_items : (int)flex::e_align::align_stretch;
		}
		void base_u::set_order(int i)
		{
			if (i > 0)
				i = i;
			last_order = i; up_entity();
		}

		int base_u::get_order()
		{
			return  (last_order != order) ? last_order : order;
		}
		int base_u::get_root_order()
		{
			int ret = 0;
			if (parent)
			{
				ret += parent->get_root_order();
			}
			else
			{
				ret = get_order();
			}
			return ret;
		}

		void base_u::set_pos(const glm::vec2& s)
		{
			if (s != pos)
				is_size = true;
			pos = s;
			_bsn.pos = pos;
			up_entity();
		}

		void base_u::set_size(const glm::vec2& s)
		{
			if (s != size)
				is_size = true;
			size = s;
			if (s.x > 0 && s.y > 0)
			{
				up_entity();
			}
		}

		glm::vec2 base_u::get_size()
		{
			return size;
		}

		void base_u::set_visible(bool is)
		{
			if (visible != is)
			{
				if (parent)
					parent->sort_inc++;
				on_parent_visible(is);
			}
			visible = is;

		}

		bool base_u::get_visible()
		{
			return visible;
		}

		glm::vec2 base_u::get_pos(bool isparent)
		{
			glm::vec2 ret = pos;

			if (!parent)ret -= pos;
			if (isparent && parent)
			{
				ret += parent->get_pos(isparent);
			}
			return ret;
		}
		glm::vec2 base_u::get_pos1()
		{
			glm::vec2 ret = {};
			if (parent)
			{
				ret += parent->get_pos(1);
			}
			return ret;
		}

		glm::vec2 base_u::get_fpos()
		{
			glm::vec2 ret;
			if (reg->has<event_src_et>(ety_null))
			{
				auto& es = reg->get<event_src_et>(ety_null);
				if (es.form)
				{
					ret += es.form->get_pos();
				}
			}
			return ret;
		}
		void* base_u::get_form()
		{
			void* ret = 0;
			if (reg->has<event_src_et>(ety_null))
			{
				auto& es = reg->get<event_src_et>(ety_null);
				ret = (es.form);
			}
			return ret;
		}

		glm::vec2 base_u::get_pos2(bool isrt)
		{
			glm::vec2 ret = pos;
			if (parent)
			{
				ret += parent->get_pos2(isrt);
			}
			else if (isrt) {
				auto& es = reg->get<event_src_et>(ety_null);
				if (es.edc)
				{
					ret += es.edc->get_nodepos();
				}
			}
			return ret;
		}
		div_u* base_u::get_root_div()
		{
			div_u* p = 0;
			if (is_div)
				p = (div_u*)this;
			if (parent && parent->is_div)
			{
				p = parent->get_root_div();
			}
			return p;
		}
		void base_u::set_mouse_cursor(int idx)
		{
			//mouse_cursor::SC_HAND
			auto es = reg->get<event_src_et>(ety_null);
			if (es.ctx)
			{
				es.ctx->set_cursor((mouse_cursor)idx);
				if (idx > 0)
				{
					//printf("set_mouse_cursor %p div %d\n", this, is_div);
					idx = idx;
				}
			}
		}

		void base_u::set_hover_mc(int idx, int pidx)
		{

			// todo 有bug
			//hv = 0;
			do
			{
				break;
				auto hv = get_hover();
				bool ih = reg->has<hover_et>(e);
				bool isdown = reg->has<down_et>(e);
				if ((hit || isdown) && hv && hv->idx == pidx)
				{
					//auto nhit = contains(hv->pos1, 0);
					//if (mcinc == 0)
					{
						set_mouse_cursor(idx);
						//printf("set_hover_mc%d\t%p\n", mcinc, this);
					}
					mcinc++;
				}
				if (!hit && !isdown)
				{
					if (mcinc > 0)
					{
						isleave = true;
						//printf("leave  set_hover_mc%p\n", this);
						set_mouse_cursor((int)mouse_cursor::SC_ARROW);
					}
					mcinc = 0;
				}
			} while (0);
		}


		hot_et* base_u::up_entity() {
			hot_et* ret = 0;
			if (last_order != order)
			{
				order = last_order;
				is_size = true;
			}
			if (is_size)
			{
				if (reg) {
					auto p = ecs_at::get_p<csize_empty>(reg, e);
					p->p = this;
				}
				is_size = false;
				hotd.rc = { 0, 0, size };
				hotd.pos = pos;
				if (parent)
				{
					parent->sort_inc++;
				}

			}

			return &hotd;
		}

		void base_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1) {
			auto spos = pos1 + pos;
			//canvas->draw_rect(spos, size, -1, 0, 3);
			if (_bsn.n && _bsn.t == shape_base::line)
			{
				canvas->draw_shape(_bsn.v.l, 1, spos);
			}
			if (_bsn.n && _bsn.t == shape_base::rect)
			{
				canvas->draw_shape(_bsn.v.r, 1, spos);
			}
		}

		void base_u::draw_last(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1) {
		}

		void base_u::bind_event(entt::registry* r)
		{
			if (!r)
			{
				return;
			}
			if (0 != (int)e && reg != r)
			{
				// pop时删除了
				reg->destroy(e);
				e = ety_null;
			}
			do
			{
				// todo 
				if (is_div && reg)
					break;
				reg = r;
			} while (0);
			if (parent)
			{
				if (!ptm)
				{
					ptm = parent->ptm;
				}
			}
			if (reg)
			{
				if ((int)e == 0)
					e = reg->create();
				auto bptr = ecs_at::get_btn_ptr(reg, e);
				if (_is_click)
				{
					// 注册单击回调函数
					click_cb1_et* cb1 = 0;
					auto rcb = ecs_at::get_ptr(reg, e, cb1);
					if (cb1)
					{
						cb1->ptr = this;
						cb1->cb = 0;
					}
				}
				if (has_drag || drag_cb || _drag_pos.x > 0 || _drag_pos.y > 0)
				{
					has_drag = true;
					// 注册接收拖动消息
					drag_cb1_et* dcb1 = 0;
					ecs_at::get_ptr(reg, e, dcb1);
					if (dcb1)
					{
						dcb1->ptr = this;
						dcb1->cb = [](void* ptr, const glm::ivec2& pos, const glm::ivec2& pos1, int state) {
							auto p = (base_u*)ptr;
							if (p->drag_cb)
								p->drag_cb(p);
							};

					}
				}
				if (scroll_cb)
				{
					scroll_cb1_et* scb = 0;
					ecs_at::get_ptr(reg, e, scb);
					if (scb)
					{
						scb->ptr = this;
						scb->cb = (void (*)(void*, int, int)) scroll_cb;
					}
				}
			}
		}
		int base_u::mouse_motion(const glm::ivec2& p, bool ts)
		{
			return 0;
		}
		void base_u::set_shape(rect_cs* r, style_cs* sc)
		{
			if (r && sc)
			{
				_r = *r;
				_sc = *sc;
				_r.t = &_sc;
				_bsn.n = 1;
				_bsn.t = shape_base::rect;
				_bsn.v.r = &_r;
			}
		}
		void base_u::set_shape(line_cs* r, style_cs* sc)
		{
			if (r && sc)
			{
				_line = *r;
				_sc = *sc;
				_line.t = &_sc;
				_bsn.n = 1;
				_bsn.t = shape_base::line;
				_bsn.v.l = &_line;
			}
		}

		text_cs* base_u::set_dtc(const char* str, int n, glm::ivec2 box, glm::vec2 ps)
		{
			text_cs* t = 0;
			if (ptm)
			{
				ptm->fontheight = fontheight;
				ptm->ps = ps;
				dstr = str;
				t = ptm->mk_text(str, n, box, dtc);
				if (t)
				{
					dtc = t;
				}
			}
			return t;
		}
		text_cs* base_u::mk_str(const char* str, int n, glm::ivec2 box, glm::vec2 ps, text_cs* t)
		{
			if (ptm)
			{
				ptm->fontheight = fontheight;
				ptm->ps = ps;
				t = ptm->mk_text(str, n, box, t);
			}
			return t;
		}

		//uibs_dt* base_u::get_uibs()
		//{
		//	if (!reg || !reg->has(e))
		//	{
		//		assert(reg);
		//		return nullptr;
		//	}
		//	return ecs_at::get_p<uibs_dt>(reg, e);;
		//}

		//void base_u::push_shape(triangle_cs* t, style_cs* cs)
		//{
		//	auto sp = new shape_n();
		//	auto csp = cs;// new style_cs();
		//	auto tp = t;// new triangle_cs();
		//	sp->t = shape_base::triangle;
		//	sp->n = 1;
		//	sp->v.t = tp;

		//}

		bool base_u::contains(const glm::ivec2& pos, int* pidx)
		{
			if (!reg)return false;
			auto& hs = hotd;
			auto npos = pos - hs.pos;
			bool ishot = false, ih = false;
			auto p = &hs;
			int idx = 0;
			bool ret = false;
			do
			{
				ishot = (hs.rc.w == 0) ? math_cx::inCircle(*(glm::vec3*)(&p->rc), npos) : math_cx::inRect(p->rc, npos);
				if (!ishot)break;
				for (int i = p->n - 1; i >= 0; i--)
				{
					auto& rc = p->last[i];
					ih = (rc.w == 0) ? math_cx::inCircle(rc, npos) : math_cx::inRect(rc, npos);
					if (ih)
					{
						idx = i + 1;
						break;
					}
				}

			} while (0);
			ret = ishot || ih;
			if (pidx)
			{
				*pidx = idx;
			}
			else {
				set_hit(ret, idx, npos);
			}
			return ret;
		}

		void base_u::set_hit(bool is, int idx, const glm::ivec2& npos)
		{
			if (!has_event)
				is = false;
			if (is)
			{
				if (reg)
				{
					hover_et* ph = 0;
					auto he = ecs_at::get_ptr(reg, e, ph);
					he->idx = idx;
					he->pos1 = npos;
					he->inc = 0;
					he->p = this;
				}
			}
			else {
				hover_et* ph = get_hover();
				if (reg && ph)
				{
					isleave = true;
					reg->remove_if_exists<hover_et>(e);		// 删除鼠标离开的对象属性
				}
			}
			hit = is;
		}

		void base_u::add_mte(mouse_type_e_t e)
		{
			_mte |= e;
		}

		void base_u::dec_mte(mouse_type_e_t e)
		{
			_mte &= ~(int)(e);
		}

		bool base_u::has_mte(mouse_type_e_t e)
		{
			return _mte & e;
		}

		void base_u::call_vcb()
		{
			if (ck_cbs)
			{
				static uint64_t ams = 0;
				// todo 双击问题
#ifdef _WIN32
				int st = GetDoubleClickTime();
#else
				int st = 500;
#endif
				auto nms = get_ms();
				if (nms - ams < st)
				{
					printf("间隔时间: %lld\n", nms - ams);
					//return;
				}
				ams = get_ms();
				auto& k = *ck_cbs;
				for (auto cb : k)
				{
					if (cb)
						cb(this);
				}
			}
			if (_ucbs)
			{
				auto& k = *_ucbs;
				for (auto& it : k)
				{
					if (it.cb)
						it.cb(it.id, it.ud);
				}

			}
		}


#if 0
		text_dc::text_dc()
		{
		}

		text_dc::~text_dc()
		{

		}

		void text_dc::set_fontheight(int fh)
		{
			if (fh > 5)
				fontheight = fh;
		}



		glm::vec4 text_dc::get_text_img(div_text_info_e* dt, const char* str, int sl, float font_height, glm::ivec2& box, const glm::vec2& ps, uint32_t textcolor, t_vector<image_m_cs>* ot)
		{
			glm::vec4 ret = {};
			do {
				if (!dt || !ot)break;
				auto pt = dt->ptext;
				pt->set_font_height(font_height);
				pt->splitch = splitch;
				pt->update(dt->lt, str, sl, 0);
				auto nc = pt->get_count();
				if (!nc)break;
				auto cidx = ot->size();
				ot->resize(ot->size() + nc);
				auto baseline = pt->base_line;
				pt->to_image({}, textcolor, ot->data() + cidx, -1, 0, false);
				auto box_size = pt->box_size;
				if (box.y < 1)
				{
					box.y = pt->mheight;
				}
				auto bl = baseline;// < box_size.y ? baseline : box_size.y;
				auto bor1 = border_scale * (box.y - box_size.y);
				if (box.x < 1)
					box.x = (int)(pt->box_size.x + bor1 * 2);
				glm::vec2 bor = (box - box_size);
				bor *= ps;
				bor.y = 0;
				bor.y += bl;
				float k = box.y - pt->mheight;
				bor.y -= (k * ps.y);
				if (k >= 0)
					k -= (pt->base_line - pt->base_line_b);
				bor.y += k;
				bor = ceil(bor);
				ret = { box_size, bor };
			} while (0);
			return ret;
		}
		void text_dc::make_text(base_u* p, const char* t, int n, glm::ivec2& box, const glm::vec2& ps, int fh)
		{
			if (fh > 5)
				fontheight = fh;

			uint32_t tc = -1;
			auto nc = Crc::crc32_dword(t, n);
			if (nc != tcrc || strbuf.empty())
			{
				tcrc = nc;
				auto idx = strbuf.size();
				auto dt = ecs_at::get_p0<div_text_info_e>(p->reg, ety_null);
				auto bs = get_text_img(dt, t, n, fontheight, box, ps, tc, &strbuf);
				dtext.first = idx;
				dtext.t = strbuf.data();//如果后面还有则要重新绑定
				dtext.count = strbuf.size() - idx;
				dtext.pos = { bs.z, bs.w };
				dtext.rc = { bs.x, bs.y };
				boxsize = bs;
			}
		}

		size_t text_dc::make_text(const char* t, int n, int height, div_text_info_e* ctx)
		{
			glm::ivec2 box = { 0, height };

			float text2px = 0;
			uint32_t tc = -1;
			vdtext.clear();
			strbuf.clear();

			auto bs = get_text_img(ctx, t, n, fontheight, box, ps, tc, &strbuf);
			dtext.first = 0;
			dtext.t = strbuf.data();//如果后面还有则要重新绑定
			dtext.count = strbuf.size();
			dtext.pos = { bs.z, bs.w };
			dtext.rc = { bs.x, bs.y };
			dtext.next = 0;
			boxsize = bs;
			//dtext.pos += posi;
			if (splitch > 0)
			{
				auto tn = strbuf.size();
				auto t = strbuf.data();
				int cluster = 0;
				int inc = 0;
				text_cs tcs = dtext;
				tcs.count = 0;
				tcs.rc = { 0, boxsize.y };
				for (size_t i = 0; i < tn; i++, t++)
				{
					if (cluster == t->cluster)
					{
						tcs.count++;
					}
					else {
						vdtext.push_back(tcs);
						cluster = t->cluster;
						tcs = dtext;
						tcs.first = i;
						tcs.count = 1;
						tcs.t = dtext.t;
						tcs.rc = { 0, boxsize.y };
					}
					tcs.rc.x += t->advx;
				}
				if (tcs.count > 0)
				{
					vdtext.push_back(tcs);
				}
				dtext = vdtext[0];
				auto pt = &dtext;
				for (size_t i = 1; i < vdtext.size(); i++)
				{
					pt->next = &vdtext[i];
					auto ps1 = pt->next->t[pt->next->first];
					pt->next->pos.x += text2px;
					pt = pt->next;
				}
			}
			auto nk = vdtext.size();
			return nk;
		}
		void text_dc::cp(text_dc* t)
		{
			*this = *t;
			if (vdtext.size())
				dtext = vdtext[0];
			dtext.t = strbuf.data();
			auto pt = &dtext;
			if (vdtext.size() == 1)
				t = t;
			for (size_t i = 1; i < vdtext.size(); i++)
			{
				pt->next = &vdtext[i];
				pt->next->t = dtext.t;
				pt = pt->next;
			}
		}
		shape_n* text_dc::get_tsn(const t_string& str, glm::ivec2 size1, uint32_t textcolor, base_u* p)
		{
			dtext.color = textcolor;
			make_text(p, str.c_str(), str.size(), size1, ps, fontheight);
			auto tsn = &sn; tsn->t = shape_base::text;
			tsn->n = 1;
			tsn->v.text = &dtext;
			return tsn;
		}
#endif
		// gtac_c
#if 1
		gtac_c::gtac_c()
		{
		}

		gtac_c::~gtac_c()
		{
		}


#if 1
		void newbg(size_t n, shape_base t, gtac_c* tm, bcs_t* r)
		{
			bcs_t& c = *r;
			c.n = n; c.type = t;

			switch (t)
			{
			case shape_base::image:
			{
				auto p = tm->new_mem<image_cs>(n);
				c.p.img = p;
			}
			break;
			case shape_base::image_m:
			{
				auto p = tm->new_mem<image_m_cs>(n);
				c.p.im = p;
			}
			break;
			case shape_base::triangle:
			{
				auto p = tm->new_mem<triangle_cs>(n);
				tm->new_mem(n, c.ct);
				for (size_t i = 0; i < n; i++)
				{
					p[i].t = c.ct + i;
				}
				c.p.t = p;
			}
			break;
			case shape_base::line:
			{
				auto p = tm->new_mem<line_cs>(n);
				tm->new_mem(n, c.ct);
				for (size_t i = 0; i < n; i++)
				{
					p[i].t = c.ct + i;
				}
				c.p.l = p;
			}
			break;
			case shape_base::circle:
			{
				auto p = tm->new_mem<circle_cs>(n);
				tm->new_mem(n, c.ct);
				for (size_t i = 0; i < n; i++)
				{
					p[i].t = c.ct + i;
				}
				c.p.c = p;
			}
			break;
			case shape_base::ellipse:
			{
				auto p = tm->new_mem<ellipse_cs>(n);
				tm->new_mem(n, c.ct);
				for (size_t i = 0; i < n; i++)
				{
					p[i].t = c.ct + i;
				}
				c.p.e = p;
			}
			break;
			case shape_base::rect:
			{
				auto p = tm->new_mem<rect_cs>(n);
				tm->new_mem(n, c.ct);
				for (size_t i = 0; i < n; i++)
				{
					p[i].t = c.ct + i;
				}
				c.p.r = p;
			}
			break;
			case shape_base::path:
			{
				auto p = tm->new_mem<path_cs>(n);
				tm->new_mem(n, c.ct);
				for (size_t i = 0; i < n; i++)
				{
					p[i].t = c.ct + i;
				}
				c.p.p = p;
			}
			break;
			case shape_base::text:
			{
				auto p = tm->new_mem<text_cs>(n);
				c.p.tp = p;
			}
			break;
			case shape_base::grid2d:
			{
				auto p = tm->new_mem<grid2d_cs>(n);
				c.p.g = p;
			}
			break;
			case shape_base::check:
			{
				auto p = tm->new_mem<check_mark_cs>(n);
				c.p.ck = p;
			}
			default:
				break;
			}
		}
		bcs_t* gtac_c::new_bcs(int n)
		{
			bcs_t* r = 0;
			do
			{
				if (n < 1)break;
				r = new_mem<bcs_t>(n);
				if (r) {
					r->bn = n;
					_gs.insert(r);
				}
			} while (0);
			return r;
		}

		void gtac_c::free_bcs(bcs_t* p)
		{
			auto kn = p->bn;
			if (kn < 1)
				kn = 1;
			auto p1 = p;
			for (size_t i = 0; i < kn; i++, p++)
			{
				free_bcsg(p);
			}
			free_mem(p1, p1->bn ? p1->bn : 1);
		}
		// 释放一个
		void gtac_c::free_bcsg(bcs_t* p)
		{
			if (_gs.find(p) != _gs.end())
			{
				free_mem(p->pos, p->psn);
				free_mem(p->ct, p->n);
				free_mem(p->a, p->n);
				switch (p->type)
				{
				case shape_base::image:
				{
					free_mem(p->p.img, p->n);
				}
				break;
				case shape_base::image_m:
				{
					free_mem(p->p.im, p->n);
				}
				break;
				case shape_base::triangle:
				{
					free_mem(p->p.t, p->n);
				}
				break;
				case shape_base::line:
				{
					free_mem(p->p.l, p->n);
				}
				break;
				case shape_base::circle:
				{
					free_mem(p->p.c, p->n);
				}
				break;
				case shape_base::ellipse:
				{
					free_mem(p->p.e, p->n);
				}
				break;
				case shape_base::rect:
				{
					free_mem(p->p.r, p->n);
				}
				break;
				case shape_base::path:
				{
					free_mem(p->p.p, p->n);
				}
				break;
				case shape_base::text:
				{
					free_mem(p->p.tp, p->n);
				}
				break;
				case shape_base::grid2d:
				{
					free_mem(p->p.g, p->n);
				}
				break;
				case shape_base::check:
				{
					free_mem(p->p.ck, p->n);
				}
				break;
				default:
					break;
				}
				p->n = 0;
			}
		}
		void gtac_c::bcs_set_pos(bcs_t* p, glm::vec2 ps)
		{
			bcs_set_pos(p, &ps, 1);
		}
		void gtac_c::bcs_set_pos(bcs_t* p, glm::vec2* ps, int n)
		{
			do
			{
				if (!p || n < 1 || !ps)break;
				if (!p->pos || p->psn != n)
					free_mem(p->pos, p->psn);
				else
				{
					glm::vec2* pp = 0;
					new_mem(n, pp);
					if (pp)
					{
						p->pos = pp;
						p->psn = n;
					}
				}
				if (p->pos)
					memcpy(p->pos, ps, n * sizeof(glm::vec2));
			} while (0);
		}


		bcs_t* gtac_c::new_base(shape_base t, size_t n, bcs_t* p)
		{
			assert(n);
			if (!(n > 0))
			{
				return nullptr;
			}
			auto r = p;
			if (!p)
				p = new_bcs(1);
			do
			{
				if (!p)
				{
					break;
				}
				free_bcsg(p);
				newbg(n, t, this, p);
				if (!p->p.p0)
				{
					if (!p)
						free_mem(p, 1);
					p = 0;
					break;
				}
			} while (0);
			return p;
		}
		anim_a* gtac_c::set_anim(bcs_t* p)
		{
			anim_a* a = 0;
			if (p && !p->a && p->n > 0)
			{
				a = new_mem(p->a, p->n);
			}
			return a;
		}
		text_cs* gtac_c::set_text(text_cs* tp, const std::string& str, int fontheight, glm::ivec2 box, glm::vec2 ps, uint32_t color)
		{
			assert(tp);
			auto c = this;
			c->fontheight = fontheight;
			c->ps = ps;
			auto dt = c->mk_text(str.c_str(), str.size(), box, tp);
			if (dt)
			{
				dt->color = color;
			}
			return dt;
		}

		text_cs* gtac_c::set_text(text_cs* tp, const std::string& str, glm::ivec2 box, style_text_cs* css)
		{
			assert(tp);
			auto c = this;
			//c->fontheight = fontheight;
			c->ps = css->ps;
			auto dt = c->mk_text(str.c_str(), str.size(), box, tp, css);

			return dt;
		}
#endif

		void gtac_c::set_fontheight(glm::vec2 ps1, int fh)
		{
			ps = ps1;
			if (fontheight > 5)
			{
				fontheight = fh;
			}
		}

		text_cs* gtac_c::mk_text(const char* t, int n, glm::ivec2& box, text_cs* p)
		{
			auto nc = ecc_c::crc32u(t, n);
			do
			{
				if (p && (p->crc == nc) && p->crc)
				{
					p = 0;
					break;
				}
				float text2px = 0;
				uint32_t tc = -1;
				text_cs dtext = {};
				if (p)
				{
					dtext.t = p->t;
					dtext.count = p->count;
				}
				float baseline = 0;
				auto bs = get_text_img(t, n, fontheight, box, ps, tc, &dtext, 0, baseline);
				if (dtext.count > 0 && !p)
				{
					new_mem(1, p);
				}
				if (p)
				{
					*p = dtext;
					p->crc = nc;
				}
			} while (0);

			return p;
		}
		text_cs* gtac_c::mk_text(const char* t, int n, glm::ivec2& box, text_cs* p, style_text_cs* css)
		{
			if (!css)
				return mk_text(t, n, box, p);
			auto nc = ecc_c::crc32u(t, n);
			do
			{
				if (p && (p->crc == nc) && p->crc)
				{
					p = 0;
					break;
				}
				float text2px = 0;
				uint32_t tc = -1;
				text_cs dtext = {};
				if (p)
				{
					dtext.t = p->t;
					dtext.count = p->count;
				}
				auto bs = get_text_img(t, n, css->font_height, box, css->ps, css->color ? css->color : tc, &dtext, css->fst, css->baseline);
				//dtext.baseline = css->baseline;
				if (dtext.count > 0 && !p)
				{
					new_mem(1, p);
				}
				if (p)
				{
					*p = dtext;
					p->crc = nc;
				}
			} while (0);
			return p;
		}
		text_cs* gtac_c::get_text(const std::string& str, text_cs* p, style_text_cs* css)
		{
			glm::ivec2 box = {};
			if (!css)
				return mk_text(str.c_str(), str.size(), box, p);
			auto nc = ecc_c::crc32u(str.c_str(), str.size());
			do
			{
				if (p && (p->crc == nc) && p->crc)
				{
					p = 0;
					break;
				}
				float text2px = 0;
				uint32_t tc = -1;
				text_cs dtext = {};
				if (p)
				{
					dtext.t = p->t;
					dtext.count = p->count;
				}
				auto bs = get_text_img(str.c_str(), str.size(), css->font_height, css->box, css->ps, css->color ? css->color : tc, &dtext, css->fst, css->baseline);
				//dtext.baseline = css->baseline;
				if (dtext.count > 0 && !p)
				{
					new_mem(1, p);
				}
				if (p)
				{
					*p = dtext;
					p->crc = nc;
				}
			} while (0);
			return p;
		}

		void gtac_c::remove(text_cs* p)
		{
			if (p)
			{
				if (p->t)
					free_mem(p->t, p->count);
				free_mem(p, 1);
			}
		}
		glm::vec4 gtac_c::mk_box(text_cs* ot, glm::ivec2& box, glm::vec2 ps)
		{
			glm::vec4 ret = {};
			do {
				div_text_info_e* dt = &ctx;
				if (!dt || !dt->lt || !dt->ptext || !ot)break;
				auto pt = dt->ptext;
				auto box_size = pt->box_size;
				if (box.y < 1)
				{
					//box.y = box_size.y;// pt->mheight;
				}
				if (box.y < box_size.y)
				{
					box.y = box_size.y;
				}
				auto bor1 = border_scale * (box.y - box_size.y);
				if (box.x < 1)
					box.x = (int)(pt->box_size.x + bor1 * 2);
				glm::vec2 bor = (box - box_size);
				ps.y = 1.0 - ps.y;
				bor *= ps;
				bor.y = 0;

				bor.y += pt->base_line;
				float k = box.y - box_size.y;
				bor.y -= (k * ps.y);
				if (k >= 0)
					k -= (pt->base_line - pt->base_line_b);
				bor.y += k;
				bor = ceil(bor);
				ret = { box_size, bor };

				ot->pos0 = { bor.x, bor.y };
				// = { box_size.x, box_size.y };
			} while (0);
			return ret;
		}
		glm::vec4 gtac_c::get_text_img(const char* str, int n, float font_height, glm::ivec2& box, glm::vec2 ps, uint32_t textcolor, text_cs* ot, familys_t* ft, float& baseline)
		{
			glm::vec4 ret = {};
			do {
				div_text_info_e* dt = &ctx;
				if (!dt || !dt->lt || !dt->ptext || !ot)break;
				auto pt = dt->ptext;
				pt->set_font_height(font_height);

				pt->update(dt->lt, str, n, ft);
				auto nc = pt->get_count();
				if (!nc)break;
				if (ot->t && ot->count != nc)
				{
					free_mem(ot->t, ot->count);
					ot->t = 0;
					ot->count = 0;
				}
				if (!ot->t)
					ot->t = new_mem<image_m_cs>(nc);
				assert(ot->t);
				if (!ot->t)
				{
					break;
				}
				baseline = pt->base_line;
				pt->to_image({}, textcolor, ot->t, -1, 0, false);
				ot->count = nc;
				auto box_size = pt->box_size;
				if (box.y < 1)
				{
					//box.y = box_size.y;// pt->mheight;
				}
				if (box.y < box_size.y)
				{
					box.y = box_size.y;
				}
				auto bor1 = border_scale * (box.y - box_size.y);
				if (box.x < 1)
					box.x = (int)(pt->box_size.x + bor1 * 2);
				glm::vec2 bor = (box - box_size);
				ps.y = 1.0 - ps.y;
				bor *= ps;
				bor.y = 0;

				bor.y += baseline;
				float k = box.y - box_size.y;// pt->mheight;
				bor.y -= (k * ps.y);
				if (k >= 0)
					k -= (pt->base_line - pt->base_line_b);
				bor.y += k;
				bor = ceil(bor);
				ret = { box_size, bor };

				ot->pos0 = { bor.x, bor.y };
				ot->rc = { box_size.x, box_size.y };
			} while (0);

			return ret;
		}
#endif
		//!gtac_c







		shape_cx::shape_cx()
		{
		}

		shape_cx::~shape_cx()
		{
		}






		// todo menu_item_u
		menu_item_u::menu_item_u()
		{
		}
		menu_item_u::~menu_item_u()
		{
		}

		void menu_item_u::update(cpg_time* t1)
		{
			base_u::update(t1);
		}



		// todo tree_view_u
#if 1
		tree_view_u::tree_view_u()
		{
			//_is_click = true;
			_view = new view_u();

			_view->on_scroll_cb = [=](glm::vec2 ps, int idx) {
				if (abs(idx) == 1)
					on_vertical(ps.y);
				};
			_node = new tree_node_t();
			_node->_expand = true;
			// 把滚动消息转发给滚动条
			scroll_cb = [](base_u* ptr, int wheel, int wheelH) {
				auto sp = (tree_view_u*)ptr;
				if (sp && sp->_view)
				{
					sp->_view->on_scroll(wheel, wheelH);
				}
				};
			first_btn = last_btn = _show_node.end();
			// 三角形颜色
			hot_tscs[0].fill = -1;
			hot_tscs[0].color = 0;
			hot_tscs[1].fill = 0;
			hot_tscs[1].color = -1;
			tscs[0].fill = 0xffd6d6d6;
			tscs[0].color = 0;
			tscs[1].fill = 0;
			tscs[1].color = 0xffd6d6d6;
			//tscs[1].thickness = 1.2;
			// 鼠标进入显示
			r4s[0].color = set_alpha_f(hot_color, 0.5);
			r4s[0].fill = set_alpha_f(hot_color, 0.2);
			r4s[1].color = 0;
			r4s[1].fill = set_alpha_f(hot_color, 0.82);
			hot_act[0].t = &r4s[0];
			hot_act[1].t = &r4s[1];

			//_swvs.push_back(pw);
		}

		tree_view_u::~tree_view_u()
		{
			free_node(_node);
			delop(_view);
		}


		size_t tree_view_u::count()
		{
			return _node && _node->child ? _node->child->size() : 0;
		}

		tree_node_t* tree_view_u::insert(const t_string& str, void* raw, tree_node_t* parent)
		{
			auto p = new tree_node_t();
			p->title = str;
			p->raw = raw;
			p->parent = parent;
			p->level = 0;
			return insert(p, parent);
		}
		tree_node_t* tree_view_u::insert(const  char* str, void* raw, tree_node_t* parent)
		{
			auto p = new tree_node_t();
			p->title = str ? str : "";
			p->raw = raw;
			p->parent = parent;
			p->level = 0;
			return insert(p, parent);
		}
		tree_node_t* tree_view_u::insert(tree_node_t* c, tree_node_t* parent)
		{
			auto p = parent;
			if (!p) {
				if (!_node)_node = new tree_node_t();
				p = _node;		// 不提供parent则插入到根节点
			}
			if (c && p)
			{
				c->parent = p;
				if (!p->child) p->child = new t_vector<tree_node_t*>();
				if (p->child) p->child->push_back(c);
				_show_node.clear();
				cup_node = true;
			}
			return c;
		}
		void tree_view_u::remove(tree_node_t* p, bool is_free)
		{
			if (p && p->parent)
			{
				if (p->parent)
				{
					auto& t = p->parent->child;
					assert(t);
					t->erase(std::find(t->begin(), t->end(), p));
					_show_node.clear();
					cup_node = true;
				}
				if (is_free) free_node(p);
			}
		}
		void tree_view_u::free_node(tree_node_t* p)
		{
			if (p)
			{
				if (p->child)
				{
					auto n = p->child->size();
					for (auto it : *p->child) { free_node(it); }
					delop(p->child);
					p->child = 0;
				}
				delop(p);
			}
		}

		void tree_view_u::clear()
		{
			remove(_node, true);
		}

		void tree_view_u::set_idx(int idx)
		{
			set_act_node(idx, true);
		}

		void tree_view_u::up_show()
		{
			size_t first = 0;
			std::stack<tree_node_t*> ts;
			if (_show_node.size())return;
			auto skk = sizeof(t_list<tree_node_t*>::iterator);
			auto fp = _node;
			fp->level = -1;		// 根在0层
			size_t n = fp->child ? fp->child->size() : 0;
			// 展开树到链表
			{
				auto pt = fp;
				ts.push(pt);
				auto pt1 = pt;
				while (ts.size() > 0) {
					pt = ts.top(); ts.pop();
					if (pt->parent)
					{
						pt->idx = _show_node.size();
						_show_node.push_back(pt);
					}
					if (pt->_expand && pt->child && pt->child->size())
					{
						auto le = pt->level + 1;	// 生成层级
						for (auto it = pt->child->rbegin(); it != pt->child->rend(); it++)
						{
							(*it)->level = le;
							ts.push(*it);
						}
					}
				}

				first_btn = last_btn = _show_node.end();
			}
			return;
		}
		void tree_view_u::expand_node(tree_node_t* p)
		{
			size_t first = 0;
			std::stack<tree_node_t*> ts;
			if (_show_node.empty() || !p)return;
			auto fp = p;
			size_t n = fp->child ? fp->child->size() : 0;
			if (!n)
			{
				return;
			}
			// 如果展开了，不等于则没操作
			bool hassn = false;
			auto ct = fp->it0; ct++;
			if (ct != _show_node.end())
			{
				if ((*ct)->parent == fp)
					hassn = true;
			}
			if (fp->_expand && hassn || (!fp->_expand && !hassn))return;
			bool rt = !fp->_expand;
			size_t dn = 0;
			int fidx = fp->idx + 1;
			t_list<tree_node_t*> ln;
			// 展开树到链表
			{
				auto pt = fp;
				ts.push(pt);
				auto pt1 = pt;
				while (ts.size() > 0) {
					pt = ts.top(); ts.pop();
					if (pt->parent && pt != fp)
					{
						pt->idx = fidx++;
						ln.push_back(pt);
						dn++;
					}
					if (pt->_expand || rt)
					{
						rt = false;
						if (pt->child && pt->child->size())
						{
							auto le = pt->level + 1;	// 生成层级
							for (auto it = pt->child->rbegin(); it != pt->child->rend(); it++)
							{
								(*it)->level = le;
								ts.push(*it);
							}
						}
					}
					else {
					}
				}

				auto ipt = p->it0;
				ipt++;
				if (ln.size() && fp->_expand)
				{
					_show_node.insert(ipt, ln.begin(), ln.end());
				}
				if (dn && !fp->_expand)
				{
					int i = 0;
					for (auto it = ipt; i < dn && it != _show_node.end(); i++)
					{
						_show_node.erase(it++);
					}
				}
			}
			return;
		}
		size_t tree_view_u::calc_expand(tree_node_t* p)
		{
			if (!p)return 0;
			size_t n = 0;
			std::stack<tree_node_t*> ts;
			{
				auto pt = p;
				ts.push(pt);
				auto pt1 = pt;
				while (ts.size() > 0) {
					pt = ts.top(); ts.pop();
					if (pt->parent && pt != p)
					{
						n++;
					}
					if (pt->_expand)
					{
						if (pt->child && pt->child->size())
						{
							auto le = pt->level + 1;	// 生成层级
							for (auto it = pt->child->rbegin(); it != pt->child->rend(); it++)
							{
								(*it)->level = le;
								ts.push(*it);
							}
						}
					}
				}
			}
			return n;
		}

		void tree_view_u::remove(size_t idx)
		{

		}

		void tree_view_u::set_color(uint32_t text_col, uint32_t border_col, uint32_t back_col)
		{
			text_color = text_col; rc_col.x = border_col; rc_col.y = back_col;
		}

		void tree_view_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			glm::ivec2 spos = pos + pos1;

			auto ss = get_size();
			canvas->draw_rect(spos, ss, rc_col.x, rc_col.y, 4);
			auto kn = node_count;
			glm::vec2 itw = { ss.x - 6,item_height };
			spos += 1;
			// 裁剪
			clip.x = spos.x;
			clip.y = spos.y;
			//for (size_t i = 0; i < kn; i++)
			//{
			//	//canvas->draw_rect(spos, itw, 0xffFF9E40, 0, 4);
			//	spos.y += item_height;
			//}

			if (clip.z > 0 && clip.w > 0)
			{
				clicp_x ccp(canvas, clip);
				glm::vec2 cp = _view->scroll_pos;
				auto cpos = pos1 + pos;
				auto aps = cpos;
				canvas->draw_shape(&hot_act[0], 1, cpos);

				// 渲染控件
				if (_view)
				{
					_view->draw(t, canvas, cpos);
				}
				// 渲染文本
				cpos += cp;
				auto p = first_it;
				for (; p; )
				{
					if (p->np)
					{
						if (p->np->is_checked)
							canvas->draw_shape(&hot_act[1], 1, aps);
						canvas->draw_shape(p->dtext, cpos + p->pos, 0);
					}
					aps.y += item_height;
					p = p->next;
					if (p == first_it)
					{
						break;
					}
				}
			}
			if (_view)
			{
				_view->draw_last(t, canvas, pos1 + pos);
			}
		}

		void tree_view_u::update(cpg_time* t)
		{
			if (!_view)return;
			if (!_view->reg)
				_view->reg = reg;
			if (!tm.ctx.lt)
			{
				auto pr = (div_u*)parent;
				if (pr)
					set_lt(pr->get_lts());
			}
			if (!tm.ctx.lt)
				return;
			up_show();
			auto ws = get_size();
			if (vp)
			{

				//if (parent && parent->is_div)
				//	tm.ctx = ((div_u*)parent)->get_lts();

				vp = false;
			}
			//(cpg_time * t, glm::vec2 ws, int padd, int item_height, int n, glm::ivec4 & clip, int& nheight, int& max_content_width);
			_view->up_scroll(t, ws, padd, item_height, _show_node.size(), clip, nheight, max_content_width);
			up_node();

			{
				_view->update(t);
			}
			base_u::update(t);
			if (update_cb)
				update_cb(t);
		}

		void tree_view_u::make_item(item_t* it, tree_node_t* np, int idx)
		{
			it->np = np;
			if (it->_expand_c)
			{
				it->_expand_c->set_visible(_expand && np && np->child && np->child->size());
			}
			if (it->_cbu)
			{
				it->_cbu->set_visible(_check && np);
			}
			if (!np)
			{
				return;
			}
			auto p = it->np;
			glm::ivec2 box = { 0, item_height };
			auto dt = tm.mk_text(p->title.c_str(), p->title.size(), box, it->dtext);
			if (dt && dt != it->dtext)
			{
				it->dtext = dt;
			}
			if (dt)
				dt->color = text_color;
			else
				return;
			it->pos = { padd ,padd };
			it->pos.x += p->level * level_tab * _indent;
			auto cps = it->pos;
			cps.y += idx * item_height;
			it->pos.y += idx * item_height;

			auto cpos = it->pos;

			it->size = { it->dtext->pos.x + it->dtext->rc.x , item_height };
			// todo 创建操作控件
			auto color = set_alpha(text_color, 0xee);
			if (_expand)
			{
				it->size.x += item_height;
				auto& pw = it->_expand_c;
				float tw = item_height * 0.5;
				if (!pw)
				{
					pw = new switch_u();
					auto tr = tm.new_mem<triangle_cs>(2);

					pw->set_color({ color,color,color });
					pw->set_size({ item_height ,item_height }, item_height);
					pw->set_shape(tr, { 1,0,0,1 });
					pw->set_time(0.015);
					{
						tr->dir = 1;
						tr->spos = 50;
						tr->size = { tw * 0.55, tw };
						tr->pos = { 0 , 0 };	// 关闭状态的三角形
					}
					{
						tr++;
						int dtw = tw * 0.45 * 0.5;
						tr->dir = 0;
						tr->spos = 100;
						tr->size = { tw - dtw, tw };
						tr->pos = { 0 - dtw, 0 - 1 }; // 打开状态三角形
					}
					pw->uptr = it;
					pw->change_cb = [=](switch_u* p, bool v, void* uptr) {
						auto pt = (item_t*)uptr;
						expand_node(pt->np);
						cup_node = true;
						};
					_view->push(pw, 0);
					pw->set_visible(np && np->child && np->child->size());
				}
				it->pos.x += item_height;
				pw->set_pos(cps);
				pw->bind_value(&it->np->_expand);
			}
			if (_check)
			{
				it->size.x += item_height;
				auto& cp = it->_cbu;
				if (!cp)
				{
					cp = new checkbox_u();
					cp->set_size({ item_height, item_height });
					cp->fill = 0;
					cp->col0 = color;
					cp->uptr = it;
					cp->change_cb = [=](checkbox_u* p, bool v, void* uptr) {
						auto pt = (item_t*)uptr;
						auto np = pt->np;
						if (np->parent)
						{
							auto n = np->parent;
							n->ck_inc += v ? 1 : -1;
							auto nc = n->child->size();
							n->_full = (nc == n->ck_inc);
							pt->_cbu->set_mixed(n->_full);
						}
						};
					_view->push(cp, 1);

				}
				it->pos.x += item_height;
				cps.x += item_height;
				cp->set_pos(cps);
				cp->bind_value(&it->np->is_checked);

			}
			// todo 扩展显示

		}
		void tree_view_u::newshowit()
		{
			if (nc_cap < node_count)
			{
				auto ws = get_size();
				if (!nc_cap)
				{
					show_idx = -1;
				}
				auto old = it0;
				tm.new_mem(node_count, it0);
				if (old)
				{
					memcpy(it0, old, sizeof(item_t) * nc_cap);
					tm.free_mem(old, nc_cap);
				}
				if (node_count > nc_cap)
					memset(it0 + nc_cap, 0, sizeof(item_t) * (node_count - nc_cap));
				ws.x -= padd * 2;
				_hotv.resize(node_count);
				hotd.last = _hotv.data();
				hotd.n = node_count;
				nc_cap = node_count;
				auto p = it0; auto hp = _hotv.data();
				for (size_t i = 0; i < node_count; i++, p++, hp++)
				{
					p->prev = p - 1;
					p->next = p + 1;

					*hp = { padd, padd + item_height * i, ws.x,item_height };
				}
				it0->prev = it0 + node_count - 1;
				auto last = it0->prev;
				last->next = it0;
				first_it = it0;
			}
		}
		void tree_view_u::set_hot_node(int idx)
		{
			if (idx >= mxhot)
			{
				idx = -1;
			}
			if (idx < 0)
			{
				hot_act->size = { };
			}
			else {
				auto ws = get_size();
				bool v = _view->_vertical->get_visible();
				hot_act->pos = { padd, padd + idx * item_height };
				hot_act->size = { ws.x - (padd * 2 + (scroll_width)*v + 2),item_height };
			}
		}
		void tree_view_u::set_act_node(int idx, bool ck)
		{
			if (idx < 0 || idx >= mxhot)
			{
				if (_single_check && at_node)
					at_node->is_checked = false;
				//if (_single_check && at_node)
				//{
				//	at_node->is_checked = false; at_node = 0;
				//}
				return;
			}
			auto ct = &hot_act[1];
			if (ck)
			{
				auto p = first_it;
				for (int i = 0; i < idx && p; i++, p = p->next) {

				}
				if (p)
				{
					auto str = p->np->title;
					printf("ck: %s\n", (p->np->title.c_str()));
					if (_single_check && at_node)
						at_node->is_checked = false;
					at_node = p->np;
					at_node->is_checked = true;
					if (on_click_item)
					{
						on_click_item(str.c_str(), idx, p->np);
					}
					call_vcb();
				}
				auto ws = get_size();
				bool v = _view->_vertical->get_visible();
				ct->pos = { padd, padd + 0 * item_height };
				ct->size = { ws.x - (padd * 2 + (scroll_width)*v + 2),item_height };
			}
		}
		void tree_view_u::up_node()
		{
			auto ws = get_size();
			auto hv = get_hover();
			if (hv)
			{
				auto& c = cidx;
				if (c != hv->idx)
				{
					int ch = hv->idx;
					c = hv->idx;
					if (c > 0)
					{
						ch--;
						set_hot_node(ch);
						//printf("hover\t%d\n", cidx);
					}
				}
				else {

				}
				if (is_click(0))
				{
					set_act_node(cidx - 1, true);
					//printf("click\t%d\n", cidx);
				}
			}
			//static std::map<void*, bool> ghit;
			//if (hit != ghit[this])
			//{
			//	ghit[this] = hit;
			//	printf("hover\t%d\n", hit);
			//}
			if (!hit)
			{
				set_hot_node(-1);
			}
			if (_show_node.size())
			{
				if (cup_node)
				{
					_node->amount = calc_expand(_node);
					last_idx = -1;
					on_vertical(_view->_vertical ? _view->_vertical->_offset : 0);
					cup_node = false;
				}
			}
		}

		void tree_view_u::up_shownode(size_t n)
		{
			if (n == last_idx)return;
			last_idx = n;
			{
				if (last_btn == _show_node.end())
				{
					last_btn = _show_node.begin();
				}
				glm::ivec2 r0;
				auto p = first_it;
				if (n > show_idx)
				{
					int k = n - show_idx;
					if (k >= 0)
						r0.x = k;
					for (int i = 0; i < k; i++) {
						last_btn++;
						p = p->next;
					}
					first_it = p;
				}
				if (n < show_idx)
				{
					int k = show_idx - n;
					if (k >= 0)
						r0.y = k;
					for (int i = 0; i < k; i++) {
						last_btn--;
						p = p->prev;
					}
					first_it = p;
				}
				show_idx = n;
				auto p0 = last_btn;
				float mcw = 0;
				mxhot = 0;
				for (size_t i = 0; i < node_count; i++)
				{
					if (p0 != _show_node.end())
					{
						auto& pb = *p0;
						pb->it0 = p0;
						make_item(p, pb, i);
						mcw = std::max(mcw, p->size.x + p->pos.x);
						p0++;
						mxhot++;
					}
					else {
						make_item(p, 0, i);
					}
					p = p->next;
				}
				do {
					if (mcw < cmc_width && (_view->_horizontal->_offset > 0))
						break;
					max_content_width = cmc_width = mcw;
				} while (0);
			}
		}

		void tree_view_u::on_vertical(float y)
		{
			int tsp = -y;
			if (_view->scroll_int)
			{
				tsp = 0;
			}
			else {
				int pad = tsp % item_height;
				tsp = pad;
			}
			_view->scroll_pos.y = tsp;
			if (_show_node.size())
			{
				auto ws = get_size();
				auto nc = y / item_height;
				size_t n = nc;

				if (iy != (int)ws.y)
				{
					iy = ws.y;
					auto kn = ceil(iy / item_height) + 2;
					node_count = kn;
					newshowit();
				}
				up_shownode(n);
			}
		}
		int tree_view_u::mouse_motion(const glm::ivec2& ps, bool ts)
		{
			return _view->mouse_motion(ps - (glm::ivec2)pos, ts);
		}
		void tree_view_u::set_lt(div_text_info_e t)
		{
			tm.ctx = t;
		}
#endif
		// !tree
		// todo menu_bar

		menu_bar_u::menu_bar_u()
		{
			set_root();
			set_size({ height,height });
			set_align((int)flex::e_align::align_start, (int)flex::e_align::align_center);
		}

		menu_bar_u::~menu_bar_u()
		{
			for (auto it : vmenu)
			{
				delop(it.p);
			}

		}
		void menu_bar_u::set_height(int h) {
			height = h; vnum++;
		}
		// 标题、下拉菜单
#ifdef _HAS_CXX20
		void menu_bar_u::add(const char8_t* t, menu_page_t* p)
		{
			add((char*)t, p);
		}
#endif
		void menu_bar_u::add(const char* t, menu_page_t* p)
		{
			auto tp = new ui::button_u();
			tp->set_label(t ? t : "");
			//if (!tdc) {
			//	tdc = new ui::text_dc();
			//	tdc->set_fontheight(fontheight);
			//}
			div_text_info_e dt = { this->lt, this->pdt };
			auto& str = tp->_label;
			tp->set_color(ui::button_u::defcol(2));
			btn_css_t* bc = tp->get_color();
			bc->active_background_color = 0;
			bc->background_color = 0;
			bc->border_color = 0;
			bc->hover_color = 0xff3d3d3d;
			bc->hover_border_color = 0xff9e9e9e;

			auto h = height;
			tp->height = fontheight + 8;
			tp->fontheight = fontheight;
			tp->light = 0.5;
			tp->effect = ui::uTheme::light;
			tp->text_color = text_color;
			tp->set_label(str.c_str());
			//this->tdc->make_text(str.c_str(), str.size(), tp->height, &dt);
			//tp->set_cp_text(this->tdc);
			auto tm = ptm;
			style_text_cs cs = {};
			cs.font_height = fontheight;
			cs.ps = { 0,0.5 };
			cs.box.y = tp->height;
			auto pstr = tm->get_text(str.c_str(), dtc, &cs);
			auto fw = fontheight * 0.5;
			if (pstr)
			{
				dtc = pstr;
				glm::vec2 s = { pstr->rc.x, tp->height };
				s.x += fontheight; tp->width = s.x;
				//tp->height = s.y;
				tp->set_size(s);
			}
			tp->uptr = p;
			tp->ud1 = this;
			tp->click_cb = [=](button_u* pbtn) {
				/*auto mp = (menu_page_t*)pbtn->uptr;
				if (mp)
				{
					pbtn->uhover = true;
					auto ps = pbtn->get_pos2() + get_fpos();
					auto ss = pbtn->get_size();
					ps.y += ss.y;
					if (hot_btn && hot_btn != pbtn)
					{
						hot_btn->uhover = false;
						auto mf = ((menu_page_t*)hot_btn->uptr)->get_form();
						if (mf)
							mf->hide();
					}
					mp->show(ps, get_form());
					hot_btn = pbtn;
				}*/
				};

			//tp->hit_cb1 = [](base_u* p, bool v, void* uptr) {
			//	auto pb = (button_u*)p;
			//	if (pb)
			//	{
			//		auto ts = (menu_bar_u*)pb->ud1;
			//		if (ts->hot_btn && ts->hot_btn != pb)
			//			pb->click_cb(pb, pb->uptr);
			//	}
			//};

			auto& dtp = tp->dtcs;// tp->tdc->dtext;
			dtp.pos.x += fw;

			push(tp);
			vmenu.push_back({ t ? t : "",tp, p });
			vnum++;
		}
		// add 时没绑定则可以再设置
		void menu_bar_u::set_idx(size_t idx, menu_page_t* p)
		{
			if (idx < vmenu.size())
			{
				vmenu[idx].p = p;
				vmenu[idx].btn->uptr = p;
			}
		}
		size_t menu_bar_u::it_count()
		{
			return vmenu.size();
		}
		void menu_bar_u::del_idx(size_t idx)
		{
			vmenu.erase(vmenu.begin() + idx);
		}

		void menu_bar_u::set_color(btn_css_t* c, uint32_t mask)
		{
			if (!c)return;
			for (auto& it : vmenu)
			{
				auto tp = it.btn;
				if (!tp)continue;
				btn_css_t* bc = tp->get_color();
				uint32_t* d = (uint32_t*)(&bc->font_color);
				uint32_t* s = (uint32_t*)c;
				uint32_t m = 1;
				for (size_t i = 0; i < sizeof(btn_css_t) / sizeof(uint32_t); i++)
				{
					if (m & mask)
					{
						*d = *s;
					}
					m = m << 1; d++; s++;
				}
				//uint32_t font_color = 0xFF222222;
				//uint32_t background_color = 0xFFffffff;
				//uint32_t border_color = 0xFF222222;
				//uint32_t hover_color = 0xFFf78989;
				//uint32_t active_font_color = 0xFFe6e6e6;
				//uint32_t active_background_color = 0xFFf2f2f2;
				//uint32_t active_border_color = 0xFF3c3c3c;
				//uint32_t hover_border_color = 0;
			}
		}

		menu_page_t* menu_bar_u::get_page(size_t idx)
		{
			menu_page_t* p = 0;
			if (idx < vmenu.size())
			{
				p = vmenu[idx].p;
			}
			return p;
		}

		void menu_bar_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			auto vp0 = canvas->get_viewport0();
			glm::vec2 cs = { vp0.z,vp0.w };
			if (cs.x * cs.y > 0)
			{
				set_size({ cs.x, height });
			}
			div_u::draw(t, canvas, pos1);
		}
		void menu_bar_u::update(cpg_time* t)
		{
			div_u::update(t);
			if (vnum > 0 && vmenu.size())
			{
				vnum = 0;
				glm::vec2 ss = { height * 0.5,height };
				for (auto& it : vmenu)
					ss.x += it.btn->get_size().x;
				//set_size(ss);
			}
			if (hot_btn)
			{
				do
				{
					//auto mp = (menu_page_t*)hot_btn->uptr;
					//sdl_form* mf = 0;
					//if (mp)
					//{
					//	mf = mp->get_form();
					//	if (!mf)
					//		break;
					//	if (mf->get_visible())
					//	{
					//		break;
					//	}
					//}
					//mf->hide();
					hot_btn->uhover = false;
					hot_btn = 0;
				} while (0);
			}
		}


		// todo scroll_view_u
#if 1

#if 1
#ifdef min
#undef min
#undef max
#endif

		glm::vec2 get_bounding(glm::vec2* p, int n, glm::vec2* bounding)
		{
			glm::vec2 mmin = glm::vec2(MAXINT32, MAXINT32);
			glm::vec2 mmax = glm::vec2(-MAXINT32, -MAXINT32);

			for (size_t i = 0; i < n; i++)
			{
				auto v = p[i];
				mmin.x = std::min(v.x, mmin.x);
				mmin.y = std::min(v.y, mmin.y);
				//mmin.z = std::min(v.z, mmin.z);
				mmax.x = std::max(v.x, mmax.x);
				mmax.y = std::max(v.y, mmax.y);
				//mmax.z = std::max(v.z, mmax.z);
			}
			if (bounding)
			{
				bounding[0] = mmin;
				bounding[1] = mmax;
			}
			glm::vec2 size = mmax - mmin;
			return size;
		}
		inline void add_bounding(glm::vec2 v, glm::vec2& mmin, glm::vec2& mmax)
		{
			mmin.x = std::min(v.x, mmin.x);
			mmin.y = std::min(v.y, mmin.y);
			//mmin.z = std::min(v.z, mmin.z);
			mmax.x = std::max(v.x, mmax.x);
			mmax.y = std::max(v.y, mmax.y);
			//mmax.z = std::max(v.z, mmax.z);		 
		}

		glm::vec2 bcs_t::get_size()
		{
			glm::vec2 r = {};
			auto st = this;
			auto kt = (shape_base)(st->type);
			auto dn = psn;
			if (dn < 1)dn = 1;

			glm::vec2 mmin = glm::vec2(MAXINT32, MAXINT32);
			glm::vec2 mmax = glm::vec2(-MAXINT32, -MAXINT32);

			switch (kt)
			{
			case shape_base::image:
				for (size_t i = 0; i < st->n; i++)
				{
					auto ps = get_imgsize(&st->p.img[i]);
					add_bounding(ps, mmin, mmax);
				}
				break;
			case shape_base::image_m:
				for (size_t i = 0; i < st->n; i++)
				{
					auto ps = st->p.im[i].img ? get_imgsize((image_cs*)&st->p.im[i]) : st->p.im[i].size;
					add_bounding(ps, mmin, mmax);
				}
				break;
			case shape_base::triangle:
				for (size_t i = 0; i < st->n; i++)
				{
					add_bounding(st->p.t[i].size, mmin, mmax);
				}
				break;
			case shape_base::line:
				for (size_t i = 0; i < st->n; i++)
				{
					auto s1 = glm::abs(st->p.l[i].pos2 - st->p.l[i].pos);
					add_bounding(s1, mmin, mmax);
				}
				break;
			case shape_base::circle:
				for (size_t i = 0; i < st->n; i++)
				{
					auto s1 = glm::vec2(st->p.c[i].r * 2.0, st->p.c[i].r * 2.0);
					add_bounding(s1, mmin, mmax);
				}
				break;
			case shape_base::ellipse:
				for (size_t i = 0; i < st->n; i++)
				{
					auto s1 = glm::vec2(st->p.e[i].r.x * 2.0, st->p.e[i].r.y * 2.0);
					add_bounding(s1, mmin, mmax);
				}
				break;
			case shape_base::rect:
				for (size_t i = 0; i < st->n; i++)
				{
					add_bounding(st->p.r[i].size, mmin, mmax);
				}
				break;
			case shape_base::check:
				for (size_t i = 0; i < st->n; i++)
				{
					auto x = st->p.ck[i].square_sz;
					glm::vec2 ss = { x,x };
					add_bounding(ss, mmin, mmax);
				}
				break;
			case shape_base::path:
			{
				//canvas->draw_shape(st->p.p, st->n, ps);
			}
			break;
			case shape_base::grid2d:
			{
				//canvas->draw_shape(st->p.g, st->n, ps);
			}
			break;
			case shape_base::text:
			{
				for (size_t i = 0; i < st->n; i++)
				{
					add_bounding(st->p.tp[i].rc, mmin, mmax);
				}
			}
			break;
			default:
				break;
			}
			r = mmax - mmin;
			return r;
		}
		glm::vec2 get_imgsize0(image_cs* p)
		{
			auto rect = p->rect;
			glm::ivec2 texsize = {};
			if (p->img) { texsize = { p->img->width, p->img->height }; }
			else if (p->tex) { texsize = { p->tex->width, p->tex->height }; }
			auto a = glm::vec2(p->size);
			if (rect.z < 0)
			{
				rect.z *= -texsize.x;
			}
			if (rect.w < 0)
			{
				rect.w *= -texsize.y;
			}
			if (a.x < 0)
				a.x *= -std::min(rect.z, texsize.x);
			if (a.y < 0)
				a.y *= -std::min(rect.w, texsize.y);
			return a;
		}
		glm::vec2 bcs_t::get_sizes(std::function<void(size_t idx, glm::vec2 ss)> cb)
		{
			glm::vec2 r = {};
			auto st = this;
			auto kt = (shape_base)(st->type);
			auto dn = psn;
			if (dn < 1)dn = 1;

			glm::vec2 mmin = glm::vec2(MAXINT32, MAXINT32);
			glm::vec2 mmax = glm::vec2(-MAXINT32, -MAXINT32);

			switch (kt)
			{
			case shape_base::image:
				for (size_t i = 0; i < st->n; i++)
				{
					auto ss = get_imgsize(&st->p.img[i]);// .size;
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
				break;
			case shape_base::image_m:
				for (size_t i = 0; i < st->n; i++)
				{
					auto ss = (st->p.im[i].img) ? get_imgsize((image_cs*)&st->p.im[i]) : st->p.im[i].size;
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
				break;
			case shape_base::triangle:
				for (size_t i = 0; i < st->n; i++)
				{
					auto ss = st->p.t[i].size;
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
				break;
			case shape_base::line:
				for (size_t i = 0; i < st->n; i++)
				{
					auto& it = st->p.l[i];
					auto ss = glm::abs(it.pos2 - it.pos);
					if (ss.x < it.t->thickness)
					{
						ss.x = it.t->thickness;
					}
					if (ss.y < it.t->thickness)
					{
						ss.y = it.t->thickness;
					}
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
				break;
			case shape_base::circle:
				for (size_t i = 0; i < st->n; i++)
				{
					auto ss = glm::vec2(st->p.c[i].r * 2.0, st->p.c[i].r * 2.0);
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
				break;
			case shape_base::ellipse:
				for (size_t i = 0; i < st->n; i++)
				{
					auto ss = glm::vec2(st->p.e[i].r.x * 2.0, st->p.e[i].r.y * 2.0);
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
				break;
			case shape_base::rect:
				for (size_t i = 0; i < st->n; i++)
				{
					auto ss = st->p.r[i].size;
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
				break;
			case shape_base::check:
				for (size_t i = 0; i < st->n; i++)
				{
					auto x = st->p.ck[i].square_sz;
					glm::vec2 ss = { x,x };
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
				break;
			case shape_base::path:
			{
				//canvas->draw_shape(st->p.p, st->n, ps);
			}
			break;
			case shape_base::grid2d:
			{
				//canvas->draw_shape(st->p.g, st->n, ps);
			}
			break;
			case shape_base::text:
			{
				for (size_t i = 0; i < st->n; i++)
				{
					auto ss = st->p.tp[i].rc;
					add_bounding(ss, mmin, mmax);
					cb(i, ss);
				}
			}
			break;
			default:
				break;
			}
			r = mmax - mmin;
			return r;
		}
		void bcs_t::set_poss(glm::vec2* ps, int n)
		{
			auto st = this;
			auto kt = (shape_base)(st->type);
			n = std::min(n, st->n);
			switch (kt)
			{
			case shape_base::image:
				for (size_t i = 0; i < n; i++)
				{
					st->p.img[i].pos = ps[i];
				}
				break;
			case shape_base::image_m:
				for (size_t i = 0; i < n; i++)
				{
					st->p.im[i].pos = ps[i];
				}
				break;
			case shape_base::triangle:
				for (size_t i = 0; i < n; i++)
				{
					st->p.t[i].pos = ps[i];
				}
				break;
			case shape_base::line:
				for (size_t i = 0; i < n; i++)
				{
					st->p.l[i].pos += ps[i];
					st->p.l[i].pos2 += ps[i];
				}
				break;
			case shape_base::circle:
				for (size_t i = 0; i < n; i++)
				{
					st->p.c[i].pos = ps[i];
				}
				break;
			case shape_base::ellipse:
				for (size_t i = 0; i < n; i++)
				{
					st->p.e[i].pos = ps[i];
				}
				break;
			case shape_base::rect:
				for (size_t i = 0; i < n; i++)
				{
					st->p.r[i].pos = ps[i];
				}
				break;
			case shape_base::check:
				for (size_t i = 0; i < n; i++)
				{
					st->p.ck[i].pos = ps[i];
				}
				break;
			case shape_base::path:
			{
				//canvas->draw_shape(st->p.p, n, ps);
			}
			break;
			case shape_base::grid2d:
			{
				//canvas->draw_shape(st->p.g, n, ps);
			}
			break;
			case shape_base::text:
			{
				for (size_t i = 0; i < n; i++)
				{
					st->p.tp[i].pos = ps[i];
				}
			}
			break;
			default:
				break;
			}
		}
		void bcs_t::set_posi(glm::vec2 ps, int i, bool lt)
		{
			auto st = this;
			auto kt = (shape_base)(st->type);
			if (i < 0 || i >= st->n)
			{
				return;
			}
			switch (kt)
			{
			case shape_base::image:
			{
				st->p.img[i].pos = ps;
			}
			break;
			case shape_base::image_m:
			{
				st->p.im[i].pos = ps;
			}
			break;
			case shape_base::triangle:
			{
				st->p.t[i].pos = ps;
			}
			break;
			case shape_base::line:
			{
				st->p.l[i].pos = ps;
			}
			break;
			case shape_base::circle:
			{
				if (lt)
					ps += st->p.c[i].r;
				st->p.c[i].pos = ps;
			}
			break;
			case shape_base::ellipse:
			{
				if (lt)
					ps += st->p.e[i].r;
				st->p.e[i].pos = ps;
			}
			break;
			case shape_base::rect:
			{
				st->p.r[i].pos = ps;
			}
			break;
			case shape_base::check:
			{
				st->p.ck[i].pos = ps;
			}
			break;
			case shape_base::path:
			{
				//canvas->draw_shape(st->p.p, n, ps);
			}
			break;
			case shape_base::grid2d:
			{
				//canvas->draw_shape(st->p.g, n, ps);
			}
			break;
			case shape_base::text:
			{

				{
					st->p.tp[i].pos = ps;
				}
			}
			break;
			default:
				break;
			}
		}
		glm::vec2 bcs_t::get_pos(int idx)
		{
			auto st = this;
			auto kt = (shape_base)(st->type);
			auto i = glm::max(0, idx);
			glm::vec2 ps = {};
			if (i < 0 || i >= st->n)
			{
				return ps;
			}
			switch (kt)
			{
			case shape_base::image:
			{
				ps = st->p.img[i].pos;
			}
			break;
			case shape_base::image_m:
			{
				ps = st->p.im[i].pos;
			}
			break;
			case shape_base::triangle:
			{
				ps = st->p.t[i].pos;
			}
			break;
			case shape_base::line:
			{
				ps = st->p.l[i].pos;
			}
			break;
			case shape_base::circle:
			{
				ps = st->p.c[i].pos;
			}
			break;
			case shape_base::ellipse:
			{
				ps = st->p.e[i].pos;
			}
			break;
			case shape_base::rect:
			{
				ps = st->p.r[i].pos;
			}
			break;
			case shape_base::path:
			{
				//canvas->draw_shape(st->p.p, n, ps);
			}
			break;
			case shape_base::grid2d:
			{
				//canvas->draw_shape(st->p.g, n, ps);
			}
			break;
			case shape_base::text:
			{
				ps = st->p.tp[i].pos;
			}
			break;
			case shape_base::check:
			{
				ps = st->p.ck[i].pos;
			}
			break;
			default:
				break;
			}
			return ps;
		}
		glm::vec2 bcs_t::get_size(int idx)
		{
			auto st = this;
			auto kt = (shape_base)(st->type);
			auto i = glm::max(0, idx);
			glm::vec2 ps = {};
			if (i < 0 || i >= st->n)
			{
				return ps;
			}
			switch (kt)
			{
			case shape_base::image:
			{
				ps = get_imgsize(&st->p.img[i]);// .size;
			}
			break;
			case shape_base::image_m:
			{
				ps = st->p.im[i].img ? get_imgsize((image_cs*)&st->p.im[i]) : st->p.im[i].size;
			}
			break;
			case shape_base::triangle:
			{
				ps = st->p.t[i].size;
			}
			break;
			case shape_base::line:
			{
				auto& it = st->p.l[i];
				auto ss = glm::abs(it.pos2 - it.pos);
				if (ss.x < it.t->thickness)
				{
					ss.x = it.t->thickness;
				}
				if (ss.y < it.t->thickness)
				{
					ss.y = it.t->thickness;
				}
				ps = ss;
			}
			break;
			case shape_base::circle:
			{
				ps = glm::vec2(st->p.c[i].r * 2.0, st->p.c[i].r * 2.0);
			}
			break;
			case shape_base::ellipse:
			{
				ps = glm::vec2(st->p.e[i].r.x * 2.0, st->p.e[i].r.y * 2.0);
			}
			break;
			case shape_base::rect:
			{
				ps = st->p.r[i].size;
			}
			break;
			case shape_base::check:
			{
				ps.x = ps.y = st->p.ck[i].square_sz;
			}
			break;
			case shape_base::path:
			{
				//canvas->draw_shape(st->p.p, n, ps);
			}
			break;
			case shape_base::grid2d:
			{
				//canvas->draw_shape(st->p.g, n, ps);
			}
			break;
			case shape_base::text:
			{
				ps = st->p.tp[i].rc;
			}
			break;
			default:
				break;
			}
			return ps;
		}



		void bcs_t::set_pos(const glm::vec2& ps1)
		{
			pos0 = ps1;
		}



		void bcs_t::set_visible(bool is)
		{
			visible = is;
		}
		int bcs_t::hit_test(const glm::vec2& ps)
		{
			return 0;
		}
		void bcs_t::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& cpos, const glm::vec2& cp)
		{
			assert(p.p0);
			if (!visible || !p.p0)return;
			auto st = this;
			auto kt = (shape_base)(st->type);
			auto dn = psn;
			if (dn < 1)dn = 1;
			auto pa = a;
			for (int i = 0; i < dn; i++)
			{
				auto ps = cpos + cp * mask_scroll;
				ps += pos0;
				if (pos)
					ps += pos[i];
				pa = a + i;
				if (pa)
				{
					if (pa->acce < 0)
					{
						pa = 0;
					}
					else
					{
						canvas->push_ubo((motion_t*)pa);
						pa->up_rotate(ps, t);	// 更新旋转角度
					}
				}
				switch (kt)
				{
				case shape_base::image:
					canvas->draw_shape(st->p.img, st->n, 0, ps);
					break;
				case shape_base::image_m:
					canvas->draw_shape(st->p.im, st->n, 0, ps);
					break;
				case shape_base::triangle:
					canvas->draw_shape(st->p.t, st->n, ps);
					break;
				case shape_base::line:
					canvas->draw_shape(st->p.l, st->n, ps);
					break;
				case shape_base::circle:
					canvas->draw_shape(st->p.c, st->n, ps);
					break;
				case shape_base::ellipse:
					canvas->draw_shape(st->p.e, st->n, ps);
					break;
				case shape_base::rect:
					canvas->draw_shape(st->p.r, st->n, ps);
					break;
				case shape_base::path:
				{
					canvas->draw_shape(st->p.p, st->n, ps);
				}
				break;
				case shape_base::grid2d:
				{
					canvas->draw_shape(st->p.g, st->n, ps);
				}
				break;
				case shape_base::check:
				{
					canvas->draw_shape(st->p.ck, st->n, ps);
				}
				break;
				case shape_base::text:
				{
					auto km = p.tp;
					auto rc = ps + km->rc;
					//if (is_rect_intersect({ clip.x,clip.y,clip.x + clip.z,clip.y + clip.w }, { ps,rc }))
					canvas->draw_shape(km, st->n, ps);

				}
				break;
				default:
					break;
				}
				if (pa)
				{
					canvas->pop_ubo();
				}
			}
		}
#endif
		scroll_view_u::scroll_view_u()
		{
			_view = new view_u();
			_view->_layer = &_layer;

			drag_cb = on_drag_cb;
			_view->on_scroll_cb = [=](glm::vec2 ps, int idx) {
				if (on_scroll_cb.size())
				{
					for (auto it : on_scroll_cb)
						if (it)
							it(ps, idx);
				}
				auto x = abs(idx);
				switch (x)
				{
				case 1:on_horizontal(ps.x); break;
				case 2:on_vertical(ps.y); break;
				default:
					break;
				}
				};
			// 把滚动消息转发给滚动条
			scroll_cb = [](base_u* ptr, int wheel, int wheelH) {
				auto sp = (scroll_view_u*)ptr;
				if (sp && sp->_view)
				{
					sp->_view->on_scroll(wheel, wheelH);
				}
				};

			// 鼠标进入显示
			//r4s[0].color = set_alpha_f(hot_color, 0.5);
			//r4s[0].fill = set_alpha_f(hot_color, 0.2);
			//r4s[1].color = 0;
			//r4s[1].fill = set_alpha_f(hot_color, 0.82);
			//hot_act[0].t = &r4s[0];
			//hot_act[1].t = &r4s[1];
		}
		scroll_view_u::~scroll_view_u()
		{
			auto es = reg->get<event_src_et>(ety_null);
			if (es.p)
			{
				for (auto p : _on_ed)
				{
					es.p->pop(p);
				}
			}
			for (; 0 < refc;)
			{
				sleep(10);
			}
			delop(_sem_b);
			delop(_view);
			delop(efc);
		}
		int scroll_view_u::add_scroll_cb(tf_scroll_cb cb)
		{
			auto r = on_scroll_cb.size();
			if (cb)
				on_scroll_cb.push_back(cb);
			return r;
		}
		int scroll_view_u::add_event_cb(void* ptr, glm::ivec4 r, uint32_t* st, tf_click_cb cb)
		{
			if (cb)
				on_click_cb.push_back({ r,ptr,st, cb });
			return 0;
		}
		int scroll_view_u::add_event(event_data_cx* p, void* ptr)
		{
			if (p)
			{
				_on_ed.push_back(p);
				auto es = reg->get<event_src_et>(ety_null);
				if (es.p)
				{
					es.p->push(p);
				}
			}
			return 0;
		}
		void scroll_view_u::set_show_scroll(bool is)
		{
			if (_view) {
				_view->_has_scroll = is;
				set_content_size(_cs);
			}
		}

		void scroll_view_u::set_size(const glm::vec2& size)
		{
			base_u::set_size(size);
			set_content_size(_cs);
		}

		void scroll_view_u::set_drag_view(bool is)
		{
			drag_view = is;
		}


		void scroll_view_u::set_bpos(const glm::vec4& pos)
		{
			if (_view)_view->set_bpos(pos);
		}
		// 设置行数
		void scroll_view_u::set_row_count(int n)
		{

		}
		// 设置行高
		void scroll_view_u::set_row_height(int h, int idx)
		{

		}
		void scroll_view_u::set_min_size(const glm::vec2& s)
		{

		}
		void scroll_view_u::set_max_size(const glm::vec2& s)
		{

		}

		void scroll_view_u::set_color(uint32_t text_col, uint32_t border_col, uint32_t back_col)
		{
			text_color = text_col; rc_col.x = border_col; rc_col.y = back_col;
		}

		gtac_c* scroll_view_u::get_mktext()
		{
			return &tm;
		}
		gtac_c* scroll_view_u::get_tm()
		{
			return &tm;
		}
#if 0
		bcs_t* scroll_view_u::push(text_cs* tcs, int layer)
		{
			return push(tcs, 0, 1, layer);
		}

		bcs_t* scroll_view_u::push(text_cs* p, glm::vec2* pos, int n, int layer)
		{
			bcs_t* r = 0;
			if (p && n > 0)
			{

				bcs_t c = {};
				c.n = 1; c.type = ec_t::text;
				c.p.tp = p;
				c.pos = pos;
				c.psn = n;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}

		//void scroll_view_u::pop(text_cs* p)
		//{
		//	if (p)
		//	{
		//		auto& it = _user_textm[p->layer];
		//		it.erase(p);
		//	}
		//}

		bcs_t* scroll_view_u::push(rc_t rc, int layer)
		{
			return push(rc.view, rc.n, layer);
		}

		bcs_t* scroll_view_u::push(rect_cs* p, int n, int layer)
		{
			bcs_t* r = 0;
			if (p && n > 0)
			{

				bcs_t c = {};
				c.n = n; c.type = ec_t::rect;
				c.p.r = p;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}

		bcs_t* scroll_view_u::push(triangle_cs* p, int n, int layer)
		{
			bcs_t* r = 0;
			if (p && n > 0)
			{

				bcs_t c = {};
				c.n = n; c.type = ec_t::triangle;
				c.p.t = p;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}



		bcs_t* scroll_view_u::push(image_cs* p, int n, int layer)
		{
			bcs_t* r = 0;
			if (p && n > 0)
			{

				bcs_t c = {};
				c.n = n; c.type = ec_t::img;
				c.p.img = p;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}
		//bcs_t* scroll_view_u::push(image_cs1* p, int n, int layer)
		//{
		//	bcs_t* r = 0;
		//	if (p && n > 0)
		//	{
		//		
		//		bcs_t c = {};
		//		c.n = n; c.type = ec_t::img1;
		//		c.p.img1 = p;
		//		r = tm.new_mem<bcs_t>(1);
		//		if (r)
		//		{
		//			*r = c;
		//			it.push_back(r);
		//		}
		//	}
		//	return r;
		//}
		bcs_t* scroll_view_u::push(image_m_cs* p, int n, int layer)
		{
			bcs_t* r = 0;
			if (p && n > 0)
			{

				bcs_t c = {};
				c.n = n; c.type = ec_t::img_m;
				c.p.im = p;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}
		/*
					image_cs* img;	0
					image_m_cs* im;	1
					line_cs* l;		2
					circle_cs* c;	3
					ellipse_cs* e;	4
					rect_cs* r;		5
					path_cs* p;		6
		*/

		bcs_t* scroll_view_u::push(line_cs* p, int n, int layer)
		{
			bcs_t* r = 0;
			if (p && n > 0)
			{

				bcs_t c = {};
				c.n = n; c.type = ec_t::line;
				c.p.l = p;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}


		bcs_t* scroll_view_u::push(circle_cs* p, int n, int layer)
		{
			bcs_t* r = 0;
			if (p && n > 0)
			{

				bcs_t c = {};
				c.n = n; c.type = ec_t::circle;
				c.p.c = p;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}
		bcs_t* scroll_view_u::push(ellipse_cs* p, int n, int layer)
		{
			bcs_t* r = 0;
			if (p && n > 0)
			{

				bcs_t c = {};
				c.n = n; c.type = ec_t::ellipse;
				c.p.e = p;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}

		bcs_t* scroll_view_u::push(path_cs* p, int layer)
		{
			bcs_t* r = 0;
			if (p && p->n > 0)
			{

				bcs_t c = {};
				c.n = 1; c.type = ec_t::path;
				c.p.p = p;
				r = tm.new_mem<bcs_t>(1);
				if (r)
				{
					*r = c;
					bind_bcs(r, layer);
					_gs.insert(r);
				}
			}
			return r;
		}
#endif
		void scroll_view_u::pop(bcs_t* p)
		{
			if (p)
			{
				if (!_sem_b)
					_sem_b = new sem_s();
				{
					LOCK(_lkbcs);
					_fbcs.push(p);
				}
				if (_sem_b)
				{
					_sem_b->wait(200);
				}
			}
		}

		void scroll_view_u::push(base_u* p, int layer)
		{
			if (!p)
			{
				assert(p);
				return;
			}
			//auto& it = _user_ui[layer];
			_layer[layer];
			//it.push_back(p);
			p->parent = this;
			if (!_view->reg)
				_view->reg = reg;
			_view->push(p, layer);
		}
		void scroll_view_u::pop(base_u* p)
		{
			if (!p)
			{
				assert(p);
				return;
			}
			_view->pop(p);
		}

		void scroll_view_u::set_layer_clip(int layer, const glm::vec4 cp)
		{
			_layer[layer] = cp;
		}
		void scroll_view_u::add_ref()
		{
			refc++;
		}
		void scroll_view_u::del_ref()
		{
			refc--;
		}
		void scroll_view_u::push(bcs_t* p, int layer)
		{
			if (p) {
				if (p->hasdv)
				{
					auto& dt = _user_g[p->dv];
					for (auto it = dt.begin(); it != dt.end(); it++)
					{
						if (p == *it)
						{
							dt.erase(it);
							break;
						}
					}
				}
				p->hasdv = true;
				auto& it = _user_g[layer]; _layer[layer];
				p->dv = layer;
				it.push_back(p);
			}
		}
		void scroll_view_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{

			glm::ivec2 spos = pos + pos1;

			auto ss = get_size();
			canvas->draw_rect(spos, ss, rc_col.x, rc_col.y, rounding);

			glm::vec2 itw = { ss.x - 6,item_height };
			spos += padd;
			// 裁剪
			clip.x = spos.x;
			clip.y = spos.y;

			if (clip.z > 0 && clip.w > 0)
			{
				clicp_x ccp(canvas, clip, true);
				glm::vec2 cp = _view->scroll_pos;
				auto cpos = pos1 + pos;
				auto aps = cpos;
				//print_time att("draw text");// 测试渲染命令生成时间
				for (auto& [lt, cp1] : _layer)
				{
					clicp_x ccpa(canvas, mk_clicp(cp1, clip), true);
					auto rt = _user_g.find(lt);
					if (rt != _user_g.end())
					{
						auto dt = rt->second.data(); auto ds = rt->second.size();
						for (auto i = 0; i < ds; i++)
						{
							auto st = dt[i];
							if (!st)continue;
							int bn = st->bn;
							st->draw(t, canvas, cpos, cp);
							for (size_t x = 1; x < bn; x++)
							{
								st[x].draw(t, canvas, cpos, cp);
							}
						}
					}
				}
				// 渲染控件
				if (_view)
				{
					_view->draw(t, canvas, cpos);

					_view->draw_last(t, canvas, pos1 + pos);
				}

			}
		}
		void scroll_view_u::update(cpg_time* t)
		{
			auto ws = get_size();
			if (update_cb)
				update_cb(t);
			if (vp)
			{

				vp = false;
			}
			if (!_view)return;
			if (!_view->reg)
				_view->reg = reg;
			//(cpg_time * t, glm::vec2 ws, int padd, int item_height, int n, glm::ivec4 & clip, int& nheight, int& max_content_width);
			int max_content_width = 20;
			int nheight = 0;
			//_view->up_scroll(t, ws, padd, item_height, 1, clip, nheight, max_content_width);
			clip.z = ws.x - padd * 2;
			clip.w = ws.y - padd * 2;

			if (is_click(0) && !(_view->_horizontal->hit || _view->_vertical->hit))
			{
				auto hv = get_hover();
				auto pb = get_btnet();
				auto ck = get_click_et();
				glm::ivec2 cp = pb->pos; cp -= _view->scroll_pos;
				auto npos = hv->pos1;
				for (auto& it : on_click_cb)
				{
					auto rc = it.r;
					if (rc.z < 0)
					{
						rc.z = clip.z;
					}
					if (rc.w < 0)
					{
						rc.w = clip.w;
					}
					auto ih = (it.r.w == 0) ? math_cx::inCircle(rc, npos) : math_cx::inRect(rc, npos);
					if (ih)
					{
						it.cb(it.ptr, cp, ck);
					}
				}
				cp = {};
			}
			up_node();

			if (_view) {
				_view->clip = clip;
				_view->sort_inc += sort_inc;
				_view->update(t);
				sort_inc = 0;
			}


			if (_fbcs.size())
			{
				{
					LOCK(_lkbcs);
					while (_fbcs.size())
					{
						auto p = _fbcs.front();
						_fbcs.pop();
						auto kn = p->bn;
						auto dp = p;
						for (size_t i = 0; i < kn; i++, dp++)
						{
							auto& dt = _user_g[dp->dv];
							for (auto it = dt.begin(); it != dt.end(); it++)
							{
								if (dp == *it)
								{
									dt.erase(it);
									break;
								}
							}
						}
					}
				}

				if (_sem_b)
				{
					_sem_b->post();
				}
			}
			base_u::update(t);
		}


		glm::ivec2 scroll_view_u::get_content_size()
		{
			return _cs;
		}
		void scroll_view_u::set_content_size(glm::ivec2 cs)
		{
			_cs = cs;
			if (_view)
			{
				auto ws = get_size();
				int max_content_width = cs.x;
				int nheight = 0;
				glm::ivec4 cp = {};
				_view->up_scroll(0, ws, padd, cs.y, 1, cp, nheight, max_content_width);
			}
		}

		void scroll_view_u::set_scroll_step(int h, int v, int count)
		{
			_view->set_scroll_step(h, v);
			_view->set_scroll_count(count);
		}
		int scroll_view_u::mouse_motion(const glm::ivec2& ps, bool ts)
		{

			return _view->mouse_motion(ps - (glm::ivec2)pos, ts);
		}
		// 更新显示节点
		void scroll_view_u::up_node()
		{

		}
		void scroll_view_u::up_shownode(size_t n)
		{

		}
		void scroll_view_u::up_scroll(cpg_time* t)
		{

		}

		void scroll_view_u::set_scroll_n(int n, bool isup, int idx)
		{
			auto p = idx ? _view->_vertical : _view->_horizontal;
			if (isup)
				p->set_up(n);
			else
				p->set_down(n);
		}



		void scroll_view_u::set_hot_node(int idx)
		{
			//int mxhot = 10000;
			//if (idx >= mxhot)
			//{
			//	idx = -1;
			//}
			//if (idx < 0)
			//{
			//	hot_act->size = { };
			//}
			//else {
			//	auto ws = get_size();
			//	bool v = _view->vertical->get_visible();
			//	hot_act->pos = { padd, padd + idx * item_height };
			//	hot_act->size = { ws.x - (padd * 2 + (scroll_width)*v + 2),item_height };
			//}
		}
		void scroll_view_u::set_act_node(int idx, bool ck)
		{
			/*
			if (idx < 0 || idx >= mxhot)
			{
				if (_single_check && at_node)
				{
					at_node->is_checked = false; at_node = 0;
				}
				return;
			}
			auto ct = &hot_act[1];
			if (ck)
			{
				auto p = first_it;
				for (int i = 0; i < idx && p; i++, p = p->next) {

				}
				if (p)
				{
					printf("ck: %s\n", u8_gbk(p->np->title.c_str()).c_str());
					if (_single_check && at_node)
						at_node->is_checked = false;
					at_node = p->np;
					at_node->is_checked = true;
				}
				auto ws = get_size();
				bool v = _view->vertical->get_visible();
				ct->pos = { padd, padd + 0 * item_height };
				ct->size = { ws.x - (padd * 2 + (scroll_width)*v + 2),item_height };
			}*/

		}
		void scroll_view_u::on_check()
		{

		}
		// 水平
		void scroll_view_u::on_horizontal(float x)
		{
			//printf("x: %f\n", x);
		}
		// 垂直
		void scroll_view_u::on_vertical(float y)
		{
			//printf("y: %f\n", y);
		}
		void scroll_view_u::set_lt(div_text_info_e t)
		{
			tm.ctx = t;
			_view->set_dlt(t);
		}


		void scroll_view_u::on_dragstart(const glm::vec2& pos1)
		{
			auto ps = pos1 - pos;
			glm::ivec2 tps = _view->scroll_pos;
			first_down = (glm::ivec2)ps - tps;
		}

		void scroll_view_u::on_drag(const glm::vec2& pos1)
		{
			glm::ivec2 ps = pos1 - pos;
			_view->scroll_pos = ps - first_down;
			//do {
			//	auto kf = _offset_f;
			//	if (down_idx == _thumb_idx)
			//	{
			//		auto ps = pos1 - pos;
			//		// 设置水平滚动滑块坐标
			//		auto ps1 = ps[_dir] - first_pos[_dir];
			//		set_thumb_posi(ps1);
			//	}
			//	else {
			//		break;
			//	}
			//	if (kf > _offset_f || kf < _offset_f)
			//		up_pos(_offset_f, true);
			//} while (0);
		}

		void scroll_view_u::on_drag_cb(base_u* ptr)
		{
			auto sp = (scroll_view_u*)ptr;
			if (sp&&sp->drag_view)
			{
				auto dp = sp->get_drag_ut();
				if (dp)
				{
					if (dp->state == 0)
					{
						sp->on_dragstart(dp->pos); dp->last = dp->pos; dp->last--;
					}
					if (dp->last != dp->pos)
					{
						sp->on_drag(dp->pos);
						dp->last = dp->pos;
					}
				}
			}
		}




		// 动画

		template<class T>
		int  vat_t<T>::updata_t(float deltaTime, T& dst)
		{
			if ((deltaTime > mt && mt > 0)) return 0;
			double kct = ct;
			ct += deltaTime;
			if (ct >= mt) {
				dst = target;
				return 1;
			}
			dst = glm::mix(first, target, ct / mt);
			return 2;
		}


		action_t::action_t()
		{

		}
		action_t::~action_t()
		{

		}

		void action_t::clear()
		{
			if (ptr && ptr->ptm)
			{
				gtac_c* ac = ptr->ptm;
				ac->free_mem(avs, cap);
				ac->free_mem(dst, dcap);
				cap = 0;
				dcap = 0;
			}
		}
		void action_t::add_wait(float st)
		{
			//printf("%p\n", ptr);
			float first = 0, target = st;
			add1(st, first, target);
		}
		template<class T>
		int action_t::add1(float mt, const T& first, const T& target)
		{
			vat_t<T> a;
			a.t = sizeof(T) / sizeof(float);
			a.mt = mt;
			a.first = first;
			a.target = target;
			int rdx = -1;
			auto ops = size;
			size += sizeof(a);
			char* p = 0;
			if (size < sizeof(data))
			{
				p = data;
				avs = p;
			}
			else {
				assert(ptr);
				if (cap < size && ptr)
				{
					gtac_c* ac = ptr->ptm;
					if (!ac)return rdx;
					ac->new_mem(p, 128 + cap);
					if (ops)
						memcpy(p, avs, ops);
					cap += 128;
					ac->free_mem(avs, cap);
				}
				else {
					return rdx;
				}
			}
			p += ops;
			*((vat_t<T> *)p) = a;
			rdx = count;
			auto kr = get<T>(rdx);
			count++;
			return rdx;
		}
		template<class T>
		T* action_t::get(int x)
		{
			if (x < 0 || x >= count || !dst)
			{
				return 0;
			}
			auto ps = ((T*)&dst[x]);
			return ps;
		}

		int action_t::add(float mt, float first, float target)
		{
			return add1(mt, first, target);
		}
		int action_t::add(float mt, glm::vec2 first, glm::vec2 target)
		{
			return add1(mt, first, target);
		}
		int action_t::add(float mt, glm::vec3 first, glm::vec3 target)
		{
			return add1(mt, first, target);
		}
		void action_t::play()
		{
			_pause = false;
		}
		void action_t::pause()
		{
			_pause = true;
		}
		int action_t::updata_t(float deltaTime, float& ct)
		{
			if (_pause /*|| (deltaTime > moveTime && moveTime > 0)*/) return 0;
			//if (wait > 0)
			//{
			//	wait -= deltaTime;
			//	return 0;
			//}
			auto ac = ptr ? ptr->ptm : nullptr;
			if (ac && (!dst || dcap < count * 3))
			{
				ac->free_mem(dst, dcap);
				ac->new_mem(dst, count * 3);
				dcap = count * 3;
			}
			if (size < sizeof(data))
			{
				avs = data;
			}
			auto pt = avs;
			float* dp = dst;
			if (!dp)return 0;
			int ret = 0;
			if (cidx >= count)cidx = 0;
			for (size_t i = 0; i < count; i++)
			{
				int t = *((int*)pt);
				switch (t)
				{
				case 1:
				{
					auto p = (vat_t<float>*)pt;
					pt += sizeof(*p);//0顺序，1同时执行
					if ((type == 0 && cidx == i) || type == 1)
					{
						float r = {};
						auto hr = p->updata_t(deltaTime, r);
						*dp = r;
						ct = p->ct;
						if (type == 0 && hr != 2)
						{
							cidx++;
						}
						if (hr == 1)
						{
							ret++;
						}
					}
					dp++;
					break;
				}
				case 2:
				{
					auto p = (vat_t<glm::vec2>*)pt;
					pt += sizeof(*p);
					if ((type == 0 && cidx == i) || type == 1)
					{
						glm::vec2 r = {};
						auto hr = p->updata_t(deltaTime, r);
						auto dp1 = (glm::vec2*)dp;
						*(dp1) = r;
						ct = p->ct;
						if (type == 0 && hr != 2)
						{
							cidx++;
						}
						if (hr == 1)
						{
							ret++;
						}
					}
					dp += 2;
					break;
				}
				case 3:
				{
					auto p = (vat_t<glm::vec3>*)pt;
					pt += sizeof(*p);
					if ((type == 0 && cidx == i) || type == 1)
					{
						glm::vec3 r = {};
						auto hr = p->updata_t(deltaTime, r);
						auto dp1 = (glm::vec3*)dp;
						*(dp1) = r;
						ct = p->ct;
						if (type == 0 && hr != 2)
						{
							cidx++;
						}
						if (hr == 1)
						{
							ret++;
						}
					}
					dp += 3;
					break;
				}
				default:
					break;
				}
			}

			return ret == count ? 1 : 2;
		}

		struct div_pack_t
		{
			// 第一个为头
			ui::base_u* p = 0;
			int n = 0;
			glm::ivec3 b = { 2,6,0 };	// y为子项扩展，z为后扩展大小，x为子项x偏移
		};
		struct foldpos_t
		{

			t_vector<ui::base_u*> a;
			// ext=true展开
			void fold(int idx, bool ext);
		};
		void foldpos_t::fold(int idx, bool ext)
		{
			do {
				if (idx < 0 || idx >= a.size())break;
				auto& it = a[idx];
				//if (it.size())
				{

				}
			} while (0);
		}





#endif // 1




#if 1

		charts_u::charts_u()
		{
		}

		charts_u::~charts_u()
		{
		}

		void charts_u::set_lines(int k, glm::vec2* p, int n)
		{
			auto& v = _polylines[k];
			v.resize(n);
			memcpy(v.data(), p, n * sizeof(glm::vec2));
		}

		void charts_u::set_st(int k, line_st st)
		{
			_line_st[k] = st;
		}


		void charts_u::draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1)
		{
			glm::ivec2 spos = pos + pos1;


			auto ss = get_size();
			canvas->draw_rect(spos, ss, rc_col.x, rc_col.y, rc_col.z, rc_col.w);

			for (auto& [k, v] : _polylines)
			{
				auto it = _line_st[k];
				canvas->draw_polyline(spos, v.data(), v.size(), it.col, false, it.thickness, it.anti_aliased);
			}
		}

		void charts_u::update(cpg_time* t)
		{

		}


#endif


#if 0
		rect_cs* shape_cx::add_rect()
		{
			return nullptr;
		}

		ellipse_cs* shape_cx::add_ellipse()
		{
			return nullptr;
		}
		circle_cs* add_circle()
		{
			return nullptr;
		}
		triangle_cs* add_triangle()
		{
			return nullptr;
		}
		path_cs* add_path()
		{
			return nullptr;
		}
		image_cs* add_img()
		{
			return nullptr;
		}
		image_m_cs* add_imgm()
		{
			return nullptr;
		}
		text_cs* add_text()
		{
			return nullptr;
		}
#endif



		void testuiinfo()
		{
			// 数组：可以自定义排序
			njson a;
			a.push_back({ "属性名", "值" });
			a.push_back({ "属性名1", 123 });
			// 对象：自动排序
			njson o;
			o["属性名"] = "值";
			o["属性名1"] = true;


		}

		div_u* new_listdiv(listdiv_newinfo* p)//t_vector<t_string>* vstr, layout_text* plt, div_u* p2, int font_height, int width, uint32_t tc, uint32_t blur_color, float tpx)
		{
			if (!p)
			{
				return nullptr;
			}
			if (p->font_height < 6)
			{
				p->font_height = 6;
			}
			float blur = 3;
			glm::ivec2 ms = { 300,300 };
			float fw = p->tpx;
			float lineheight = 5;
			auto lgr = new ui::div_u();
			auto div1 = lgr;
			float padding1 = div1->u_padding1;
			div1->utc = p->tc;
			div1->uptr = p->uptr;
			div1->click_cb = p->click_cb;
			if (p->p2)
			{
				lgr->set_cp2(p->p2);
				//p->p2->push(div1);
			}
			else if (p->plt)
			{
				lgr->set_root();
				lgr->set_font_family(p->plt);
			}
			//lgr->tdc = new ui::text_dc();
			//lgr->tdc->set_fontheight(p->font_height);
			div_text_info_e dt = { lgr->lt, lgr->pdt };
			lgr->set_padding1(padding1);
			t_vector<ui::button_u*> vbs;
			//auto tdc = lgr->tdc;
			//tdc->ps = { 0, 0.5 };
			// 使用tab分割
			//tdc->splitch = '\t';
			float ith = p->font_height + 6;
			float mx[2] = {};
			int i = 0;
#if 0
			for (auto it : *p->vstr)
			{
				auto tp = new ui::button_u();
				vbs.push_back(tp);
				tdc->make_text(it.c_str(), it.size(), ith, &dt);
				tp->set_cp_text(tdc);
				tp->uptr = p->uptr;
				auto pcb = p->click_cb;
				tp->click_cb = [=](button_u* pbtn, void* uptr) {
					pcb(pbtn->_label.c_str(), i, uptr);
					};
				i++;
				// 计算最宽的字符块
				mx[0] = std::max(mx[0], tdc->dtext.rc.x);
				if (tdc->dtext.next)
					mx[1] = std::max(mx[1], tdc->dtext.next->rc.x);
			}
#endif
			lgr->set_bf_color(0xff666666, 0xff1f1f1f, 0, 1);
			float ex = mx[0] + mx[1] + fw + ith * 2;
			ms.x = ex;
			div1->umx = { mx[0],mx[1] };
			if (p->width > ms.x)
			{
				ms.x = p->width;
			}
			glm::ivec2 itsize = { ms.x - padding1 * 2, ith };
			float sw = itsize.x - 6;
			ms.y = vbs.size() * ith + padding1 * 2;
			lgr->set_size(ms);
			line_cs r; style_cs sc = {};
			r.pos = { itsize.y + 6.0,2 };
			r.pos2 = r.pos; r.pos2.x = itsize.x - 1;
			sc.color = sc.fill = 0xff3d3d3d;
			sc.anti_aliased = false;


			auto vts = vbs.begin();
			float ps2 = mx[0] + ith + fw;
#if 0

			for (auto it : *p->vstr)
			{
				{
					auto tp = *vts;
					tp->set_label(it.c_str());
					tp->width = itsize.x;
					tp->height = itsize.y;
					tp->set_size(itsize);
					tp->_is_hand = false;
					tp->_icon = *p->icon;
					//tp->_icon.size = {};
					auto& dtp = tp->tdc->dtext;
					dtp.pos.x += fw + tp->_icon.size.x + tp->_icon.pos.x;
					if (dtp.next)
					{
						auto& ps1 = dtp.next->t[dtp.next->first];
						tp->tdc->dtext.next->pos.x += ps2 - ps1.offset.x;
					}
					tp->text_color = p->tc;
					//tp->down_offset = 0;
					tp->effect = ui::uTheme::plain;
					auto pc = tp->get_color();
					pc->background_color = 0;
					pc->active_border_color = 0xff9e9e9e;
					pc->active_background_color = 0xff333333;
					pc->border_color = 0;
					pc->hover_color = 0xff333333;
					pc->hover_border_color = 0xff9e9e9e;

					lgr->push(tp);
					vts++;
				}
			}

#endif		
			lgr->auto_drawtext = false;
			//tdc->dtext.count = 0;
			lgr->set_pos({ 0, 0 });
			lgr->set_align((int)flex::e_align::align_start, (int)flex::e_align::align_start);
			lgr->set_align_items((int)flex::e_align::align_stretch);
			lgr->set_direction(false);
			auto fsize = lgr->get_size();
			fsize += blur * 2;
			if (0)
			{
				// 异步任务在主线程创建窗口
				auto pe1 = new promise_cx([=]()
					{
						printf("pe1第1步\n");
						form_info_new_t p = {};
						p.size = fsize;  p.back_color = 0;
						p.title = (char*)u8"菜单窗口";
						p.is_alpha = true;
						p.borderless = true;
						p.skip_taskbar = true;
						p.popup_menu = true;
						p.is_size = false;
						p.is_move = false;
						//auto form = _ctx->new_form(&p);	// 创建窗口	
						//item->form = form;
						//lgr->set_event_master(form);
						// 先移出屏幕外
						//item->form->set_pos(fsize.x, -2 * fsize.y);
						return true;
					});
				// 任务结束
				pe1->set_done_cb([=]()
					{
						//item->form->set_pos(fpos);
						printf("结束\n");
						promise_cx* p = pe1;
						delop(p);
					});
				//_ctx->push(pe1);
				// 不同线程就等待创建完
				pe1->wait();
			}


			return div1;
		}

		void ltd_remove(div_u* p, int idx)
		{
			if (!p || p->_ui->v.empty() || idx < 0 || idx >= p->_ui->v.size())return;
			p->pop(p->_ui->v[idx]);
		}
		button_u* ltd_add(div_u* p, const char* str, int len, int idx, int height)
		{
			if (!p)return 0;
			t_string it;
			if (str)
			{
				len = len < 0 ? strlen(str) : len;
				it.assign(str);
			}
			auto tp = new ui::button_u();
			div_text_info_e dt = { p->lt, p->pdt };
			float ith = p->fontheight + 6;
			if (height > ith)
			{
				ith = height;
			}
			auto ms = p->get_size();
			glm::ivec2 itsize = { ms.x - p->u_padding1 * 2, ith };
			//p->tdc->make_text(it.c_str(), it.size(), ith, &dt);
			//tp->set_cp_text(p->tdc);
			tp->uptr = p->uptr;
			float fw = p->tpx;
			float ps2 = p->umx.x + ith + fw;
			auto pcb = p->click_cb;

			tp->click_cb1 = [=](button_u* pbtn, void* uptr) {
				pcb(pbtn->_label.c_str(), idx, uptr);
				};
			tp->set_label(it.c_str());
			tp->width = itsize.x;
			tp->height = itsize.y;
			tp->set_size(itsize);
			tp->_is_hand = false;
			p->push(tp);
			auto dtp = tp->set_dtc(it.c_str(), it.size(), { 0,ith }, { 0.5,0.5 });
			if (dtp)
			{
				dtp->pos.x += fw;
				if (dtp->next)
				{
					auto& ps1 = dtp->next->t[dtp->next->first];
					//tp->dtc->next->pos.x += ps2 - ps1.offset.x;
				}
			}
			tp->text_color = p->utc;
			//tp->down_offset = 0;
			tp->effect = ui::uTheme::plain;
			auto pc = tp->get_color();
			pc->background_color = 0;
			pc->active_border_color = 0xff9e9e9e;
			pc->active_background_color = 0xff333333;
			pc->border_color = 0;
			pc->hover_color = 0xff333333;
			pc->hover_border_color = 0xff9e9e9e;

			return tp;
		}

		void ltd_clear(div_u* p)
		{
			if (p)
			{
				p->clear();
			}
		}
#if 0
		button_u* new_btn(div_u* p, const std::string& str, int idx, glm::ivec2 size)
		{
			if (!p)return 0;

			auto tp = new ui::button_u();
			div_text_info_e dt = { p->lt, p->pdt };

			p->tdc->make_text(str.c_str(), str.size(), size.y, &dt);
			tp->set_cp_text(p->tdc);
			tp->click_cb = [=](button_u* pbtn, void* uptr) {
				//pcb(pbtn->_label.c_str(), idx, uptr);
				};
			tp->set_label(str.c_str());
			tp->width = size.x;
			tp->height = size.y;
			tp->set_size(size);
			tp->_is_hand = false;
			auto& dtp = tp->tdc->dtext;
			//dtp.pos.x += fw;
			if (dtp.next)
			{
				auto& ps1 = dtp.next->t[dtp.next->first];
				tp->tdc->dtext.next->pos.x += 0 - ps1.offset.x;
			}
			//tp->text_color = p->utc;
			//tp->down_offset = 0;
			tp->effect = ui::uTheme::plain;
			auto pc = tp->get_color();
			pc->background_color = 0;
			pc->active_border_color = 0xff9e9e9e;
			pc->active_background_color = 0xff333333;
			pc->border_color = 0;
			pc->hover_color = 0xff333333;
			pc->hover_border_color = 0xff9e9e9e;
			return tp;
		}		button_u* up_btn(div_u* p, button_u* tp, const std::string& str, int idx)
		{
			if (!p)return 0;
			div_text_info_e dt = { p->lt, p->pdt };
			float ith = tp->height;
			p->tdc->make_text(str.c_str(), str.size(), ith, &dt);
			tp->dtc = (p->tdc->dtext);
			return tp;
		}
#endif




		void show_div2f(div_u* div0, bool visible, bool isfront)
		{
			if (div0)
			{
				div0->set_order(0);
				if (isfront && div0->form && visible)
				{
					div0->form->set_now_ui(div0);
				}
				div0->set_visible(visible);
			}
		}
		button_u* add_close(div_u* div, int height, uint32_t text_color /*= 0xff808080*/)
		{
			button_u* ret = 0;
			if (div)
			{
				std::string kc = (char*)"\xef\x85\x92";
				auto closebtn = new button_u();
				closebtn->set_label(kc);
				closebtn->set_color_idx(2);
				closebtn->fontheight = height;
				closebtn->effect = uTheme::plain;
				closebtn->_is_circle = true;
				closebtn->height = height;
				closebtn->borderable = false;
				closebtn->text_color = text_color;
				closebtn->set_position(true);

				auto tk = div->get_thickness() * 2;
				int pad = height + tk;
				auto rs = div->get_size();
				closebtn->set_pos({ rs.x - pad, tk });
				closebtn->click_cb = [=](button_u* p) {
					show_div2f(div, false, false);
					};
				div->push(closebtn);
				ret = closebtn;
			}
			return ret;
		}
		dialog_u* init_dialog(dialog_u* p, njson info, sdl_form* form)
		{
			do
			{
				if (!p || !info.is_object())break;

				auto title = toStr(info["title"]);
				auto size = toVec2(info["size"], 100);
				auto pos = toVec2(info["pos"], 0);
				auto rounding = toInt(info["rounding"], 6);
				auto justify_content = toInt(info["title_jc"], (int)flex::e_align::align_start);
				auto fontheight = toInt(info["fontheight"], 16);
				auto title_fontheight = toInt(info["title_fontheight"], 0);
				auto btn_fontheight = toInt(info["btn_fontheight"], 0);
				auto closeable = toBool(info["closeable"], true);
				auto dragable = toBool(info["dragable"], true);
				auto visible = toBool(info["visible"], false);

				auto div_border_color = toColor(info["div_border_color"], 0);
				auto close_color = toColor(info["close_color"], 0x805046e6);
				if (info.find("fill") == info.end())
					info["fill"] = "0xff555555";
				if (btn_fontheight < 6)
					btn_fontheight = fontheight;
				if (title_fontheight < 6)
					title_fontheight = fontheight;

				njson rp;
				rp["size"] = v2to(size);
				rp["pos"] = v2to(pos);
				rp["fill"] = info["fill"];
				rp["justify_content"] = flex::e_align::align_space_around;
				rp["align_content"] = flex::e_align::align_space_around;
				rp["direction"] = flex::e_direction::column;
				rp["pid"] = 0;
				rp["visible"] = visible;

				rp["border"] = 0x808080ff;
				rp["dragable"] = dragable;
				rp["rounding"] = rounding;// middle["rounding"] = right["rounding"] = 5;

				auto ctx = form->_wctx;
				p->rdiv = (ui::div_u*)ctx->new_ui("div", rp);
				p->rdiv->set_event_master(form, true);
				p->rdiv->set_font_family(ctx->get_lt());
				auto& btns = info["btns"];
				njson dn;
				njson topn;
				auto ts = size;
				ts.x -= rounding * 2;
				ts.y = fontheight * 2;
				topn["size"] = v2to(ts);
				topn["border"] = div_border_color;
				topn["justify_content"] = justify_content;	// 水平方向
				topn["align_content"] = flex::e_align::align_center;	// 垂直方向
				topn["align_items"] = flex::e_align::align_center;		// 元素对齐方式
				dn.push_back(topn);
				njson bn;
				ts = size;
				ts.x -= rounding * 2;
				ts.y = fontheight * 2;
				bn["size"] = v2to(ts);
				bn["border"] = div_border_color;
				njson ctn;
				ts = size;
				ts.x -= rounding * 2;
				ts.y -= fontheight * 0.1;
				ts.y -= fontheight * 2;
				if (btns.size())
					ts.y -= fontheight * 2;
				ctn["size"] = v2to(ts);
				ctn["border"] = div_border_color;
				dn.push_back(ctn);
				if (btns.size())
					dn.push_back(bn);


				//	p->rdiv->tdc;
				auto tm = p->rdiv->tm;
				p->btns = tm->new_mem<button_u*>(dn.size());
				p->cap = dn.size();
				auto dps = (div_u**)&p->top;

				for (size_t i = 0; i < dn.size(); i++)
				{
					auto dp = (ui::div_u*)ctx->new_ui("div", dn[i]);
					if (dp) {
						p->rdiv->push(dp);
						dp->set_has_event(false);
						dps[i] = dp;
					}
				}

				njson tag1;
				tag1["s"] = title;
				tag1["fontheight"] = title_fontheight;
				tag1["height2fh"] = 1;
				tag1["c"] = toColor(info["tcolor"], 0xff111111);
				auto tg1 = (ui::tag_u*)ctx->new_ui("tag", tag1);
				if (tg1)
				{
					//tg1->sps.y = 0;
					p->top->push(tg1);
				}
				auto& btns_color = info["btns_color"];
				int bro = toInt(info["btn_rounding"]);
				int h1 = 1.8 * btn_fontheight;
				for (size_t i = 0; i < btns.size(); i++)
				{
					njson n;
					n["s"] = btns[i];
					n["fontheight"] = btn_fontheight;
					n["rounding"] = bro;
					n["size"] = v2to({ 60, h1 });
					auto p1 = (ui::button_u*)ctx->new_ui("button", n);
					if (p1)
					{
						btn_css_t* c = p1->get_color();
						auto c1 = toColor(btns_color[i], c->background_color);
						if (c1 > 0)
						{
							c->hover_color = set_alpha_xf(c1, 0.6);
							c->background_color = set_alpha_xf(c1, 1.0);
							c->active_background_color = set_alpha_xf(c1, 0.9);
						}
						p->btns[p->count++] = p1;
						p->bottom->push(p1);
					}
				}
				if (closeable)
				{
					add_close(p->rdiv, btn_fontheight * 1.4, close_color);
				}
			} while (0);
			return p;
		}
		void dialog_btn_bind(dialog_u* p, int idx, std::function<void(button_u* p)> click_cb)
		{
			do {
				if (!p || p->count < 1 || !p->btns)break;
				if (idx < 0)
				{
					for (size_t i = 0; i < p->count; i++)
					{
						p->btns[i]->click_cb = click_cb;
						p->btns[i]->ud1 = (void*)i;
					}
				}
				else {
					p->btns[idx]->click_cb = click_cb;
					p->btns[idx]->ud1 = (void*)idx;
				}
			} while (0);
		}
		void dialog_show(dialog_u* p, bool isshow)
		{
			show_div2f(p->rdiv, isshow, true);
		}






	}






	double anim_a::get() { return dst.x; }

	void anim_a::set_acce(float v)
	{

	}
	// 旋转中心要全局坐标
	void anim_a::up_rotate(bool isck, const glm::vec2& rpos, cpg_time* t, bool smoothtime)
	{
		auto p = this;
		if (p)
		{
			auto rp = rot_pos + rpos;
			p->mot.rotate_pos = rp;
			if (isck)
			{
				p->isexe = true;
			}
			if (p->isexe)
			{
				auto dt = smoothtime ? t->smoothDeltaTime : t->deltaTime;
				if (p->update(dt * p->acce, 0))
					p->isexe = false;
				p->calc_rotate(p->urx);
			}
		}
	}

	void anim_a::up_rotate(const glm::vec2& rpos, cpg_time* t)
	{
		auto p = this;
		if (p)
		{
			auto rp = rot_pos + rpos;
			p->mot.rotate_pos = rp;
			if (isck)
			{
				p->isexe = true; isck = false;
			}
			if (p->isexe)
			{
				auto dt = t->deltaTime;
				if (p->update(dt * p->acce, 0))
					p->isexe = false;
				p->calc_rotate(p->urx);
			}
		}
	}

	bool anim_a::update(double delta_time, int value)
	{
		if (value != 0)
		{
			dst.y = (value > 0) ? 1 : -1;
		}
		ft.z = delta_time;
		return 0;// (AM::up_animate(ft, &dst, indefinite));
	}
	void anim_a::calc_pos(glm::vec2 pos, glm::vec2 pos1)
	{
		mot.pos = glm::mix(pos, pos1, dst.x);
	}
	void anim_a::calc_scale(glm::vec2 pos, glm::vec2 pos1)
	{
		mot.scale = glm::mix(pos, pos1, dst.x);
	}
	void anim_a::calc_rotate(float q1, float q2)
	{
		//mot.rotate = glm::normalize(glm::slerp(q1, q2, dst.x));
		mot.rotate = glm::radians(glm::mix(q1, q2, dst.x));
	}
	void anim_a::calc_rotate(double rx)
	{
		//mot.rotate = glm::quat(glm::radians(glm::vec3(0, 0, rx * dst.x)));
		mot.rotate = glm::radians(rx * dst.x);
	}




	// !ui
	// todo gui lua
#if 0
	gui_lua::gui_lua(lua_cx* p) :ctx(p)
	{

	}

	gui_lua::~gui_lua()
	{
	}

	/*
		把函数注册到anj表
		gui.sleep(100);
	*/
	void gui_lua::init()
	{
		ctx->insert_pn("gui");
		makedata();

		ctx->begin_pn("gui", 0);
		{
			ctx->reg_cb1("sleep", sleep);



		}
		ctx->end_pn();
	}

	void gui_lua::makedata()
	{
		//std::unordered_map<std::string, int>
		//auto vc = vkcode_s();
		//ctx->add_table(*vc);
		//ctx->set_field("vk_code");
	}

#endif // 1

	glm::ivec4 mk_clicp(glm::ivec4 cp1, glm::ivec4 clip)
	{
		auto cp11 = cp1;
		if ((int)(cp1.w * cp1.z) != 0) {
			cp11.x += clip.x;
			cp11.y += clip.y;
			if (cp1.z < 0)
			{
				cp11.z = clip.z - cp1.x;
			}
			if (cp1.w < 0)
			{
				cp11.w = clip.w - cp1.y;
			}
		}
		else {
			cp11 = clip;
		}
		return cp11;
	}


#if 1
	//ndef no_videorender
	void free_video(videoplayinfo_t* vp)
	{
		do {
			if (!vp || !vp->ctx || !vp->sp)break;
			if (!(*vp->isfree))
			{
				*vp->isfree = true; break;	// 通知视频操作线程
			}
		} while (0);
	}
	void free_video1(videoplayinfo_t* vp)
	{
		do {
			if (!vp || !vp->ctx || !vp->sp)break;
			auto page = vp->ctx->get_page_view();
			auto sh = vp->ctx->get_sh();
			page->free_tex(vp->yuvtex);
			sh->free_pipe(vp->yuvpipe);
			delop(vp->up);

			delop(vp->isfree);
			delop(vp->_ts);
			vp->sp->tm.free_mem(vp, 1);
		} while (0);
	}
	void set_vp_pos(videoplayinfo_t* p, const glm::vec2& ps)
	{
		if (p)
		{
			p->tps = ps;
			*p->_ts = true;
		}
	}
	void set_vp_size(videoplayinfo_t* p, const glm::vec2& v)
	{
		if (p)
		{
			if (p->yuvtex && p->sp)
			{
				glm::vec2 yt = { p->yuvtex->width, p->yuvtex->height }, st = p->sp->get_size(), mxt = { 10, 10 }, vt = v;
				if (v.x < 10)
				{
					vt.x = yt.x * v.x;
					vt.y = yt.y * v.y;
				}
				auto nt = glm::max(mxt, vt);
				p->pimg->size = nt;
			}
			else {
				p->tvs = v;
				*p->_ts = true;
			}
		}
	}
	glm::vec2 vget_size(videoplayinfo_t* p)
	{
		glm::ivec2 r = {};
#ifdef FFVIDEO 
		if (p && p->fp)
			ff_get_size(p->fp, &r.x, &r.y);
#endif
		return r;
	}


	videoplayinfo_t* new_video(const void* url, const char* hw, world_cx* ctx, ui::scroll_view_u* sp)
	{
		videoplayinfo_t* p = 0;
#ifdef FFVIDEO 
		void* fp = ff_open((char*)url, hw, [](yuv_info_t* py) {
			{
				if (!py || !py->ctx)
				{
					return;
				}
				auto vp = (videoplayinfo_t*)ff_get_ptr(py->ctx);
				if (!vp || !vp->ctx)
				{
					return;
				}
				if (ff_has_exit(py->ctx) && !py->data[0])
				{
					auto sp = vp->sp;
					sp->pop(vp->gp);
					free_video1(vp);
					sp->del_ref();
					py->ctx = 0;
					return;
				}
				if ((*vp->isfree) || vp->ctx->is_exit())
				{
					ctrl_data_t c = {};
					c.is_exit = true;
					ff_set(py->ctx, &c);
					free_video(vp);
					return;
				}

				auto& yuvtex = vp->yuvtex;
				if (!yuvtex)
				{
					vp->up = new upload_cx();
					vp->up->isprint = false;
					vp->up->init(vp->ctx->get_dev(), py->width * py->height * 4 * 2, 1);
					auto page = vp->ctx->get_page_view();
					yuvtex = page->new_yuvtex(py, vp->up);
					auto sh = vp->ctx->get_sh();
					std::string yuvn = "yuv420" + std::to_string((uint64_t)yuvtex->sampler);
					vp->yuvpipe = sh->new_pipe1(yuvn, "base3d", (VkRenderPass)vp->ctx->_render_pass, yuvtex->sampler);
					assert(vp->yuvpipe);
					yuvtex->pipe = vp->yuvpipe;
					vp->pimg->tex = yuvtex;
					auto p1 = vp->pimg;
					glm::ivec2 yt = { yuvtex->width, yuvtex->height }, st = vp->sp->get_size(), mxt = { 10, 10 };
					auto nt = glm::min(glm::min(yt, st), mxt);
					p1->size = yt;
					p1->rect = { 0,0,yuvtex->width, yuvtex->height };
					//c.ploop = 2;	// 循环播放2次
					// 获取视频时长
					auto tt = ff_get_time(py->ctx);
				}
				else {
					yuvtex->up_yuv(*py, vp->up);	// 更新纹理
				}
				if (*vp->_ts)
				{
					set_vp_size(vp, vp->tvs);
					vp->pimg->pos = vp->tps;
					*vp->_ts = false;
				}
			}

			}
		);
		p = sp->tm.new_mem<videoplayinfo_t>(1);
		p->isfree = new std::atomic_bool();
		p->_ts = new std::atomic_bool();
		p->sp = sp;
		p->ctx = ctx;
		sp->add_ref();
		(*p->isfree) = 0;
		// 创建显示图形
		{

			auto tm = p->sp->get_tm();
			auto bc = tm->new_bcs(1);
			auto bc1 = bc;
			for (size_t i = 0; i < 1; i++, bc1++)
			{
				p->sp->push(bc1, 1);
			}
			ui::bcs_t* pr1 = tm->new_base(shape_base::image, 1, bc);
			p->pimg = pr1->p.img;
			auto p1 = p->pimg;
			p1->pos = { 10, 10 };
			p1->color = -1;
			p->gp = bc;
		}
		p->fp = fp;
		p->size = vget_size(p);
		p->vtime = ff_get_time(fp);
		p->ff_set = ff_set;
		p->ff_get_time = ff_get_time;
		p->set_pos = set_vp_pos;
		p->set_size = set_vp_size;
		p->get_size = vget_size;
		ff_set_ptr(fp, p);
#endif
		return p;
	}
#else
	videoplayinfo_t* new_video(const void* url, world_cx* ctx, ui::scroll_view_u* sp)
	{
		return nullptr;
	}
#endif
}
// namespace hz

#if 0
void on_down(event_km_t* e);
void on_move(event_km_t* e);
void on_hover(event_km_t* e);
void on_up(event_km_t* e);
void on_scroll(event_km_t* e);
void on_leave(event_km_t* e);
void on_enter(event_km_t* e);
void on_dragstart(event_km_t* e);
void on_drag(event_km_t* e);
void on_ole_drop(event_km_t* e);
void on_ole_drag(event_km_t* e);
void on_ole_dragover(event_km_t* e);
void on_input(event_km_t* e);
void on_editing(event_km_t* e);
void on_key(event_km_t* e);

void on_click(event_km_t* e);
void on_dblclick(event_km_t* e);
void update_on_scroll(cpg_time* t);
void onctx_t::on_down(event_km_t* e)
{
}

void onctx_t::on_move(event_km_t* e)
{
}

void onctx_t::on_hover(event_km_t* e)
{
}

void onctx_t::on_up(event_km_t* e)
{
}

void onctx_t::on_scroll(event_km_t* e)
{
}

void onctx_t::on_leave(event_km_t* e)
{
}

void onctx_t::on_enter(event_km_t* e)
{
}

void onctx_t::on_dragstart(event_km_t* e)
{
}

void onctx_t::on_drag(event_km_t* e)
{
}

void onctx_t::on_ole_drop(event_km_t* e)
{
}

void onctx_t::on_ole_drag(event_km_t* e)
{
}

void onctx_t::on_ole_dragover(event_km_t* e)
{
}

void onctx_t::on_input(event_km_t* e)
{
}

void onctx_t::on_editing(event_km_t* e)
{
}

void onctx_t::on_key(event_km_t* e)
{
}

void onctx_t::on_click(event_km_t* e)
{
}

void onctx_t::on_dblclick(event_km_t* e)
{
}

void onctx_t::update_on_scroll(cpg_time* t)
{
}
#endif // 0
