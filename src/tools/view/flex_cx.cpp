
// flex src https://github.com/xamarin/flex.git
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <tgmath.h>
#include <ntype.h>
#include <base/camera.h>
#include "flex_cx.h"

#include <base/hl_time.h>
#include <base/print_time.h>
#include <view/image.h>

#ifndef _no_hb_

#include <random>
#ifndef _WIN32
#define HAVE_PTHREAD
#endif
#include <harfbuzz/hb.h>
//#include <harfbuzz/hb-ft.h>
//#include <harfbuzz/hb-utf.hh>
//#include <harfbuzz/hb-icu.h> /* Alternatively you can use hb-glib.h */


#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/utypes.h>
#include <unicode/ucsdet.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/ubidi.h>
#include "scrptrun.h"
//#include <cairo/cairo.h>
//#include <cairo/cairo-ft.h>
#include <font/font_cx.h>
#endif




U_NAMESPACE_USE;
//// 内存泄漏检测
//#ifdef _DEBUG
//#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
//#endif

#define TEST_EQUAL1(expr, val) \
    TEST_EQUAL_T(expr, val, \
            (_Generic((expr), \
                      unsigned int: "%u", \
                      int: "%d", \
                      bool: "%d", \
                      float: "%0.1f")))



static inline struct flex_item*
flex_item_with_size(float width, float height)
{
	struct flex_item* item = flex_item_new(1);
	flex_item_set_width(item, width);
	flex_item_set_height(item, height);
	return item;
}

typedef enum {
	FlexInherit,
	FlexStretch,
	FlexStart,
	FlexCenter,
	FlexEnd,
	FlexSpaceBetween,
	FlexSpaceAround,
	FlexBaseline,
} FlexAlign;
struct flex_node_t {
	FlexAlign alignItems;
	FlexAlign alignSelf;
	FlexAlign alignContent;
	FlexAlign justifyContent;
};
#if 0
void testflex()
{
	flex_node_t node[1] = {};
	assert(node->alignSelf == FlexInherit || node->alignSelf == FlexStretch || node->alignSelf == FlexStart || node->alignSelf == FlexCenter || node->alignSelf == FlexEnd || node->alignSelf == FlexBaseline);
	assert(node->alignItems == FlexStretch || node->alignItems == FlexStart || node->alignItems == FlexCenter || node->alignItems == FlexEnd || node->alignItems == FlexBaseline);
	assert(node->justifyContent == FlexStart || node->justifyContent == FlexCenter || node->justifyContent == FlexEnd || node->justifyContent == FlexSpaceBetween || node->justifyContent == FlexSpaceAround);
	assert(node->alignContent == FlexStretch || node->alignContent == FlexStart || node->alignContent == FlexCenter || node->alignContent == FlexEnd || node->alignContent == FlexSpaceBetween || node->alignContent == FlexSpaceAround);
}
void setflex_self_sizing(struct flex_item* item, float size[2])
{
	printf("\n");
}
#endif
#ifdef _WIN32
void test_justify_content1(void)
{
	auto ctx = hz::flex_cx::get_item_func();

	int ss = flex_item_mem_size();
	hz::flex_cx fc;

	//div_value{ width, height, children_count }
	std::vector<hz::flex::div_value> v = {
		{500, 200, 3},
		{80, 60, 2},
		{20, 20, 0}, {20, 30, 0},
		{80, 100, 1},
		{50, 60, 1},
		{20, 30, 0},
		{120, 50, 3},
		{30, 20, 0}, {20, 30, 0}, {50, 30, 0},
	};

	// 创建一组布局
	flex_item* rt = fc.set_layout(v.data(), v.size(), 0);
	float fs = NAN;
	char kfd[10] = {};
	memcpy(kfd, &fs, 4);
	flex_item_set_wrap(rt, FLEX_WRAP_WRAP);
	//flex_item_set_shrink(v[1].p, 1);
	//flex_item_set_basis(v[1].p, 130);
	//flex_item_set_basis(v[4].p, 130);
	//flex_item_set_basis(v[7].p, 150);
	// 横向
	flex_item_set_justify_content(rt, FLEX_ALIGN_CENTER);
	// 纵向
	flex_item_set_align_content(rt, FLEX_ALIGN_START);
	flex_item_set_direction(rt, FLEX_DIRECTION_ROW);
	// 孩子纵轴
	flex_item_set_align_items(rt, FLEX_ALIGN_CENTER);
	flex_item_set_margin_left(v[7].p, -20);
	flex_item_set_margin_left(v[4].p, 20);
	flex_item_set_margin_right(v[4].p, 10);
	flex_item_set_wrap(v[4].p, FLEX_WRAP_WRAP);
	flex_item_set_justify_content(v[4].p, FLEX_ALIGN_CENTER);
	flex_item_set_wrap(v[7].p, FLEX_WRAP_WRAP);
	// 单个子纵轴对齐
	flex_item_set_align_self(v[8].p, FLEX_ALIGN_CENTER);
	fc.do_layout();
	auto img = hz::Image::create_null(600, 600);
	img->clear_color(0xff000000);
	glm::vec2 opos = { 10, 10 };
	for (auto& it : v) {
		auto pos = fc.get_item_merge_pos(it.p) + opos;
		auto size = fc.get_item_size(it.p);
		img->draw_rect({ pos, size }, 0xff00ff00, 0);
		printf("%d\n", flex_item_count(it.p));
	}
	img->saveImage("test_layout.png");

	hz::Image::destroy(img);
}


void test_flex()
{
	test_justify_content1();

}
#endif
/*
// Create a new flex item.
struct flex_item* flex_item_new(void);

// Free memory associated with a flex item and its children.
// This function can only be called on a root item.
void flex_item_free(struct flex_item* item);

// Manage items.
void flex_item_add(struct flex_item* item, struct flex_item* child);
void flex_item_insert(struct flex_item* item, unsigned int index,
	struct flex_item* child);
struct flex_item* flex_item_delete(struct flex_item* item,
	unsigned int index);
unsigned int flex_item_count(struct flex_item* item);
struct flex_item* flex_item_child(struct flex_item* item,
	unsigned int index);
struct flex_item* flex_item_parent(struct flex_item* item);
struct flex_item* flex_item_root(struct flex_item* item);

// Layout the items associated with this item, as well as their children.
// This function can only be called on a root item whose `width' and `height'
// properties have been set.
void flex_layout(struct flex_item* item);

// Retrieve the layout frame associated with an item. These functions should
// be called *after* the layout is done.
float flex_item_get_frame_x(struct flex_item* item);
float flex_item_get_frame_y(struct flex_item* item);
float flex_item_get_frame_width(struct flex_item* item);
float flex_item_get_frame_height(struct flex_item* item);

*/
namespace hz {
	flex_cx::flex_cx()
	{
		atcb = get_item_func();
	}



	flex_cx::~flex_cx()
	{
		free_layout();
	}

	flex_item* flex_cx::data()
	{
		return _item;
	}

	size_t flex_cx::size()
	{
		return size_t();
	}


	void flex_cx::free_layout()
	{
		if (_item)
			flex_item_free(_item);
	}
	void flex_cx::do_layout()
	{
		flex_layout(_item);
	}
	glm::vec2 flex_cx::get_item_merge_pos(flex_item* it)
	{
		assert(it);
		glm::vec2 ret = get_item_pos(it);
		flex_item* p = it;
		do
		{
			p = flex_item_parent(p);
			if (p)
			{
				ret += get_item_pos(p);
			}
		} while (p);
		return ret;
	}
	//// 内边距
	//glm::vec2 get_padding_pos(flex_item* it)
	//{
	//	return { flex_item_get_padding_left(it), flex_item_get_padding_top(it) };
	//}
	//// 外边距
	//glm::vec2 get_margin_pos(flex_item* it)
	//{
	//	return { flex_item_get_margin_left(it), flex_item_get_margin_top(it) };
	//}
	glm::vec2 flex_cx::get_item_pos(flex_item* it)
	{
		return glm::vec2(flex_item_get_frame_x(it), flex_item_get_frame_y(it));
	}
	glm::vec2 flex_cx::get_item_size(flex_item* it)
	{
		auto ret = glm::vec2(flex_item_get_frame_width(it), flex_item_get_frame_height(it));
		if (ret.x < 1)
		{
			ret.x = flex_item_get_width(it);
		}
		if (ret.y < 1)
		{
			ret.y = flex_item_get_height(it);
		}
		return ret;
	}
	void flex_cx::set_item_size(flex_item* it, const glm::vec2& s)
	{
		if (it)
		{
			if (s.x > 0)
				flex_item_set_width(it, s.x);
			if (s.y > 0)
				flex_item_set_height(it, s.y);
		}
	}
	/*
		std::vector<hz::flex_cx::div_value> v = {
			{300,500,3},
				{100,100,2},
					{20,20,0},{20,30,0},
				{80,100,1},
					{50,60,1},
						{20,30,0},
				{120,100,3},
					{30,20,0},{20,30,0},{50,30,0},
		};
		// 创建一组布局
		flex_item* rt = fc.new_layout(v.data(), v.size());

	*/
	void flex_cx::resize(size_t n)
	{
		auto p = _item;
		if (_item)
		{
			p = flex_item_resize(_item, n, _item_cap, true);
		}
		else {
			p = flex_item_new(n);
			_item = p;
		}
		return;
	}
	flex_item* flex_cx::set_layout(flex::div_value* v, size_t count, int stride)
	{
		assert(v && count > 0);
		resize(count);
		auto p = _item;
		assert(p);
		if (!p)return p;
		char* ct = (char*)v;
		auto it = p;
		std::stack<flex_item*> psk;
		int n = count;
		int itsize = sizeof(flex::div_value);
		if (stride < itsize) { stride = itsize; }
		//int inc = 0;
		for (;;) {
			flex_item* parent = 0;
			for (; psk.size();) {
				parent = psk.top();
				size_t pn = flex_item_count(parent);
				size_t apn = (size_t)flex_item_get_managed_ptr(parent);
				if (pn == apn) { psk.pop(); parent = 0; }
				else { break; }
			}
			if (n == 0) { break; }
			flex::div_value* t = 0; memcpy(&t, &ct, sizeof(flex::div_value*));
			if (t->width > 0)
				flex_item_set_width(it, t->width);
			if (t->height > 0)
				flex_item_set_height(it, t->height);
			flex_item_set_managed_ptr(it, (void*)t->n);
			t->p = it;
			set_item_info(it, t->info);
			if (parent) { flex_item_add(parent, it); }
			if (t->n > 0) { psk.push(it); }
			it = flex_item_idx(it, 1);
			ct += stride;	n--;
		}
		//dc.push_cf(p);
		return p;
	}



	int flex_cx::get_item_sizeof()
	{
		return flex_item_mem_size();
	}

	void flex_cx::set_item_info(flex_item* it, flex::flex_info_t* p)
	{
		if (!p || !it)return;
		if (p->size) {
			atcb->item_set_width(it, p->size->width);
			atcb->item_set_height(it, p->size->height);
		}
		if (p->rect) {
			atcb->item_set_left(it, p->rect->left);
			atcb->item_set_right(it, p->rect->right);
			atcb->item_set_top(it, p->rect->top);
			atcb->item_set_bottom(it, p->rect->bottom);
		}
		if (p->padding) {
			atcb->item_set_padding_left(it, p->padding->left);
			atcb->item_set_padding_right(it, p->padding->right);
			atcb->item_set_padding_top(it, p->padding->top);
			atcb->item_set_padding_bottom(it, p->padding->bottom);
		}
		if (p->margin) {
			atcb->item_set_margin_left(it, p->margin->left);
			atcb->item_set_margin_right(it, p->margin->right);
			atcb->item_set_margin_top(it, p->margin->top);
			atcb->item_set_margin_bottom(it, p->margin->bottom);
		}
		if (p->align) {
			atcb->item_set_justify_content(it, (flex_align)p->align->justify_content);
			atcb->item_set_align_content(it, (flex_align)p->align->align_content);
			atcb->item_set_align_items(it, (flex_align)p->align->align_items);
			atcb->item_set_align_self(it, (flex_align)p->align->align_self);
		}
		if (p->wrap) {
			atcb->item_set_position(it, (flex_position)p->wrap->position);
			atcb->item_set_direction(it, (flex_direction)p->wrap->direction);
			atcb->item_set_wrap(it, (flex_wrap)p->wrap->wrap);
		}
		if (p->base) {
			atcb->item_set_grow(it, p->base->grow);
			atcb->item_set_shrink(it, p->base->shrink);
			atcb->item_set_order(it, p->base->order);
			atcb->item_set_basis(it, p->base->basis);
		}
	}

# ifndef FATE
#  define FATE(name, type, def) flex_item_get_##name,flex_item_set_##name,
# endif
	attribute_func* flex_cx::get_item_func()
	{
		static attribute_func cbs = {
#include "flex_attribute.h"
		};
		return &cbs;
	}
}
#ifndef _no_hb_


namespace hz {

	static hb_script_t get_script(const char* str)
	{
#define KN(v,v1) {#v1,#v},{#v,#v}
		static std::map<std::string, std::string> ms =
		{
			KN(Adlm, Adlam),
			KN(Aghb, Caucasian_Albanian),
			KN(Ahom, Ahom),
			KN(Arab, Arabic),
			KN(Armi, Imperial_Aramaic),
			KN(Armn, Armenian),
			KN(Avst, Avestan),
			KN(Bali, Balinese),
			KN(Bamu, Bamum),
			KN(Bass, Bassa_Vah),
			KN(Batk, Batak),
			KN(Beng, Bengali),
			KN(Bhks, Bhaiksuki),
			KN(Bopo, Bopomofo),
			KN(Brah, Brahmi),
			KN(Brai, Braille),
			KN(Bugi, Buginese),
			KN(Buhd, Buhid),
			KN(Cakm, Chakma),
			KN(Cans, Canadian_Aboriginal),
			KN(Cari, Carian),
			KN(Cham, Cham),
			KN(Cher, Cherokee),
			KN(Chrs, Chorasmian),
			KN(Copt, Coptic),
			KN(Cprt, Cypriot),
			KN(Cyrl, Cyrillic),
			KN(Deva, Devanagari),
			KN(Diak, Dives_Akuru),
			KN(Dogr, Dogra),
			KN(Dsrt, Deseret),
			KN(Dupl, Duployan),
			KN(Egyp, Egyptian_Hieroglyphs),
			KN(Elba, Elbasan),
			KN(Elym, Elymaic),
			KN(Ethi, Ethiopic),
			KN(Geor, Georgian),
			KN(Glag, Glagolitic),
			KN(Gong, Gunjala_Gondi),
			KN(Gonm, Masaram_Gondi),
			KN(Goth, Gothic),
			KN(Gran, Grantha),
			KN(Grek, Greek),
			KN(Gujr, Gujarati),
			KN(Guru, Gurmukhi),
			KN(Hang, Hangul),
			KN(Hani, Han),
			KN(Hani, Hans),
			KN(Hani, Hant),
			KN(Hano, Hanunoo),
			KN(Hatr, Hatran),
			KN(Hebr, Hebrew),
			KN(Hira, Hiragana),
			KN(Hluw, Anatolian_Hieroglyphs),
			KN(Hmng, Pahawh_Hmong),
			KN(Hmnp, Nyiakeng_Puachue_Hmong),
			KN(Hrkt, Katakana_Or_Hiragana),
			KN(Hung, Old_Hungarian),
			KN(Ital, Old_Italic),
			KN(Java, Javanese),
			KN(Kali, Kayah_Li),
			KN(Kana, Katakana),
			KN(Khar, Kharoshthi),
			KN(Khmr, Khmer),
			KN(Khoj, Khojki),
			KN(Kits, Khitan_Small_Script),
			KN(Knda, Kannada),
			KN(Kthi, Kaithi),
			KN(Lana, Tai_Tham),
			KN(Laoo, Lao),
			KN(Latn, Latin),
			KN(Lepc, Lepcha),
			KN(Limb, Limbu),
			KN(Lina, Linear_A),
			KN(Linb, Linear_B),
			KN(Lisu, Lisu),
			KN(Lyci, Lycian),
			KN(Lydi, Lydian),
			KN(Mahj, Mahajani),
			KN(Maka, Makasar),
			KN(Mand, Mandaic),
			KN(Mani, Manichaean),
			KN(Marc, Marchen),
			KN(Medf, Medefaidrin),
			KN(Mend, Mende_Kikakui),
			KN(Merc, Meroitic_Cursive),
			KN(Mero, Meroitic_Hieroglyphs),
			KN(Mlym, Malayalam),
			KN(Modi, Modi),
			KN(Mong, Mongolian),
			KN(Mroo, Mro),
			KN(Mtei, Meetei_Mayek),
			KN(Mult, Multani),
			KN(Mymr, Myanmar),
			KN(Nand, Nandinagari),
			KN(Narb, Old_North_Arabian),
			KN(Nbat, Nabataean),
			KN(Newa, Newa),
			KN(Nkoo, Nko),
			KN(Nshu, Nushu),
			KN(Ogam, Ogham),
			KN(Olck, Ol_Chiki),
			KN(Orkh, Old_Turkic),
			KN(Orya, Oriya),
			KN(Osge, Osage),
			KN(Osma, Osmanya),
			KN(Palm, Palmyrene),
			KN(Pauc, Pau_Cin_Hau),
			KN(Perm, Old_Permic),
			KN(Phag, Phags_Pa),
			KN(Phli, Inscriptional_Pahlavi),
			KN(Phlp, Psalter_Pahlavi),
			KN(Phnx, Phoenician),
			KN(Plrd, Miao),
			KN(Prti, Inscriptional_Parthian),
			KN(Rjng, Rejang),
			KN(Rohg, Hanifi_Rohingya),
			KN(Runr, Runic),
			KN(Samr, Samaritan),
			KN(Sarb, Old_South_Arabian),
			KN(Saur, Saurashtra),
			KN(Sgnw, SignWriting),
			KN(Shaw, Shavian),
			KN(Shrd, Sharada),
			KN(Sidd, Siddham),
			KN(Sind, Khudawadi),
			KN(Sinh, Sinhala),
			KN(Sogd, Sogdian),
			KN(Sogo, Old_Sogdian),
			KN(Sora, Sora_Sompeng),
			KN(Soyo, Soyombo),
			KN(Sund, Sundanese),
			KN(Sylo, Syloti_Nagri),
			KN(Syrc, Syriac),
			KN(Tagb, Tagbanwa),
			KN(Takr, Takri),
			KN(Tale, Tai_Le),
			KN(Talu, New_Tai_Lue),
			KN(Taml, Tamil),
			KN(Tang, Tangut),
			KN(Tavt, Tai_Viet),
			KN(Telu, Telugu),
			KN(Tfng, Tifinagh),
			KN(Tglg, Tagalog),
			KN(Thaa, Thaana),
			KN(Thai, Thai),
			KN(Tibt, Tibetan),
			KN(Tirh, Tirhuta),
			KN(Ugar, Ugaritic),
			KN(Vaii, Vai),
			KN(Wara, Warang_Citi),
			KN(Wcho, Wancho),
			KN(Xpeo, Old_Persian),
			KN(Xsux, Cuneiform),
			KN(Yezi, Yezidi),
			KN(Yiii, Yi),
			KN(Zanb, Zanabazar_Square),
			KN(Zinh, Inherited),
			KN(Zyyy, Common),
			KN(Zzzz, Unknonn)
		};
#undef KN
		hb_script_t ret = HB_SCRIPT_UNKNOWN;
		auto it = ms.find(str);
		if (it != ms.end())
		{
			ret = hb_script_from_string(it->second.c_str(), 4);
		}
		return ret;
	}

	struct bidi_item
	{
		hb_script_t sc;
		std::string s;
		uint32_t first, second;
		bool rtl = false;
	};
	class hb_cx
	{
	public:
		enum class hb_dir
		{
			invalid = 0,
			ltr = 4,	// 左-右
			rtl,		// 右-左
			ttb,		// 竖
			btt			// 倒竖
		};
	private:
		std::map<hz::tt_info*, hb_font_t*> _font;
		hb_buffer_t* _buffer = 0;
	public:
		hb_cx();
		~hb_cx();
		tt_info* push_font(hz::tt_info* p);
		void shape(const str_info_t& str, hz::tt_info* ttp, hb_dir dir);
		void shape(const str_info_t* str, hz::tt_info* ttp, hb_dir dir);
		//int tolayout(const std::string& str, hz::tt_info* ttf, double fontheight, bool is_word1, word_key& wks, std::vector<lt_item_t>& outlt);
		int tolayout(const std::string& str, layout_text::dlinfo_t* info, std::vector<lt_item_t>& outlt);
		glm::ivec2 draw_to_image(const std::string& str, hz::tt_info* ttf, double fontheight, hz::Image* img, glm::ivec3 dstpos);
	public:

	private:
		void free_font();
	};

	typedef struct ScriptRecord
	{
		UChar32 startChar;
		UChar32 endChar;
		UScriptCode scriptCode;
	} ScriptRecord;

	typedef struct BidiScriptRunRecords {
		bool isRtl;
		std::deque<ScriptRecord> records;
	} BidiScriptRunRecords;
	static void collectBidiScriptRuns(BidiScriptRunRecords& scriptRunRecords,
		const UChar* chars, int32_t start, int32_t end, bool isRTL) {
		scriptRunRecords.isRtl = isRTL;

		ScriptRun scriptRun(chars, start, end);
		while (scriptRun.next()) {
			ScriptRecord scriptRecord;
			scriptRecord.startChar = scriptRun.getScriptStart();
			scriptRecord.endChar = scriptRun.getScriptEnd();
			scriptRecord.scriptCode = scriptRun.getScriptCode();

			scriptRunRecords.records.push_back(scriptRecord);
		}
	}
#if 0
	//所有维文、柯尔克孜文的unicode字符集
	static std::set<int> HugeMap = { 0x0626, 0x0627, 0x0628, 0x062a, 0x062c, 0x062d, 0x062e, 0x062f
		, 0x0631, 0x0632, 0x0633, 0x0634, 0x0639, 0x063a, 0x0641, 0x0642, 0x0644, 0x0645, 0x0648,
		0x0649, 0x064a, 0x0675, 0x0676, 0x0677, 0x0678, 0x067e, 0x0686, 0x0698, 0x06ad, 0x06af,
		0x06be, 0x06c5, 0x06c6, 0x06c7, 0x06c8, 0x06c9, 0x06cb, 0x06d0, 0x06d5 };

	//维文独有的字符集
	static std::set<int> WeiMap = { 0x0698, 0x062e, 0x063a };

	// static int[] HazahMap = {0x0401,0x0402,0x0403,0x0404,0x0405,0x0406,0x0407,0x0408,0x0409,0x0410};

	//柯尔克孜文独有的字符集
	static std::set<int> Kerzmap = { 0x06c5, 0x06c9, 0x0649, 0x0626 };
	static bool locate(int c, std::set<int>& s)
	{
		return s.find(c) != s.end();
	}
	static bool checkWei(uint16_t* str) {

		int weiTotal = 0;
		int wei = 0;
		int hazarh = 0;
		int kerz = 0;
		int len = 0;
		uint16_t* t = str;
		for (; *str; str++) {

			int c = *str;// (high << 8) | (low & 0xff);

			if (locate(c, HugeMap)) {
				weiTotal++;
			}
			if (locate(c, WeiMap)) {
				wei++;
			}
			// if(locate(c, HazahMap)){
			//哈萨克文的unicode字符集(0x0401 0x0491)
			if (0x0491 >= c && c >= 0x0401) {
				hazarh++;
			}
			if (locate(c, Kerzmap)) {
				kerz++;
			}
		}
		len = str - t;
		//System.out.println("isWei: " + weiTotal / (probuffer.length / 2.0));
		//System.out.println("isHazard: " + hazarh / (probuffer.length / 2.0));

		//统计并返回判断结果
		if (weiTotal / len < 0.7 && hazarh / len < 0.5)
			return false;
		double weiWeight = wei / (1.0 * weiTotal);
		double harzarhWeight = hazarh / (1.0 * 144);
		double kerzWeight = kerz / (1.0 * weiTotal);
		printf("wei: %.03f\n", weiWeight);
		printf("harz: %.03f\n", harzarhWeight);
		printf("karz: %.03f\n", kerzWeight);

		if (kerzWeight * 15 >= harzarhWeight * 12 && kerzWeight * 20 >= weiWeight * 20) return false;
		if (harzarhWeight * 12 > weiWeight * 20 && harzarhWeight * 12 >= kerzWeight * 15) return false;
		return true;
	}
	static bool isNewEmoji(uint32_t c) {
		// Emoji characters new in Unicode emoji 11
		// From https://www.unicode.org/Public/emoji/11.0/emoji-data.txt
		// TODO: Remove once emoji-data.text 11 is in ICU or update to 11.
		if (c < 0x1F6F9 || c > 0x1F9FF) {
			// Optimization for characters outside the new emoji range.
			return false;
		}
		return c == 0x265F || c == 0x267E || c == 0x1F6F9 || (0x1F94D <= c && c <= 0x1F94F) ||
			(0x1F96C <= c && c <= 0x1F970) || (0x1F973 <= c && c <= 0x1F976) || c == 0x1F97A ||
			(0x1F97C <= c && c <= 0x1F97F) || (0x1F998 <= c && c <= 0x1F9A2) ||
			(0x1F9B0 <= c && c <= 0x1F9B9) || (0x1F9C1 <= c && c <= 0x1F9C2) ||
			(0x1F9E7 <= c && c <= 0x1F9FF);
	}

	static bool isEmoji(uint32_t c) {
		return isNewEmoji(c) || u_hasBinaryProperty(c, UCHAR_EMOJI);
	}

	static bool isEmojiModifier(uint32_t c) {
		// Emoji modifier are not expected to change, so there's a small change we need to customize
		// this.
		return u_hasBinaryProperty(c, UCHAR_EMOJI_MODIFIER);
	}

	static bool isEmojiBase(uint32_t c) {
		// These two characters were removed from Emoji_Modifier_Base in Emoji 4.0, but we need to keep
		// them as emoji modifier bases since there are fonts and user-generated text out there that
		// treats these as potential emoji bases.
		if (c == 0x1F91D || c == 0x1F93C) {
			return true;
		}
		// Emoji Modifier Base characters new in Unicode emoji 11
		// From https://www.unicode.org/Public/emoji/11.0/emoji-data.txt
		// TODO: Remove once emoji-data.text 11 is in ICU or update to 11.
		if ((0x1F9B5 <= c && c <= 0x1F9B6) || (0x1F9B8 <= c && c <= 0x1F9B9)) {
			return true;
		}
		return u_hasBinaryProperty(c, UCHAR_EMOJI_MODIFIER_BASE);
	}

	static UCharDirection emojiBidiOverride(const void* /* context */, UChar32 c) {
		if (isNewEmoji(c)) {
			// All new emoji characters in Unicode 10.0 are of the bidi class ON.
			return U_OTHER_NEUTRAL;
		}
		else {
			return u_charDirection(c);
		}
	}
	static bool isEmoji1(int value)
	{
		//bool kn = isNewEmoji(value);
		//bool k = isEmojiBase(value);
		//bool k1 = u_hasBinaryProperty(value, UCHAR_EMOJI_MODIFIER_BASE);
		//0x200d -- 连接符  0xd83c----好像是颜色分割符 或者是旗帜?  0xfe0f ---好像是表情结束符  0xfe0f0x20e3--特殊符号的表情
		if ((value >= 0xd800 && value <= 0xdbff))
		{
			//count = 2;
			return true;
		}
		else if ((0x2100 <= value && value <= 0x27ff && value != 0x263b)
			|| (0x2b05 <= value && value <= 0x2b07)
			|| (0x2934 <= value && value <= 0x2935)
			|| (0x3297 <= value && value <= 0x3299)
			|| value == 0xa9 || value == 0xae || value == 0x303d || value == 0x3030
			|| value == 0x2b55 || value == 0x2b1c || value == 0x2b1b || value == 0x2b50
			|| value == 0x231a)
		{
			//count = 1;
			return true;
		}
		return false;
	}

#endif


	static void do_bidi(UChar* testChars, int len, std::vector<bidi_item>& info)
	{
		//print_time ftpt("ubidi");
		static auto icub = hz::get_icu(U_ICU_VERSION_MAJOR_NUM);
		do {
			if (!icub)
			{
				icub = hz::get_icu(U_ICU_VERSION_MAJOR_NUM);
			}
			if (!icub)return;
			if (!icub->_ubidi_open)
			{
				return;
			}
		} while (0);


		UBiDi* bidi = icub->_ubidi_open();
		UBiDiLevel bidiReq = UBIDI_DEFAULT_LTR;
		int stringLen = len;
		if (bidi) {
			UErrorCode status = U_ZERO_ERROR;
			// Set callbacks to override bidi classes of new emoji
			//ubidi_setClassCallback(bidi, emojiBidiOverride, nullptr, nullptr, nullptr, &status);
			if (!U_SUCCESS(status)) {
				printf("error setting bidi callback function, status = %d", status);
				return;
			}
			info.reserve(len);
			icub->_ubidi_setPara(bidi, testChars, stringLen, bidiReq, NULL, &status);
			if (U_SUCCESS(status)) {
				//int paraDir = ubidi_getParaLevel(bidi);
				size_t rc = icub->_ubidi_countRuns(bidi, &status);

				//print_time ftpt("ubidi1");
				for (size_t i = 0; i < size_t(rc); ++i) {
					int32_t startRun = -1;
					int32_t lengthRun = -1;
					UBiDiDirection runDir = icub->_ubidi_getVisualRun(bidi, i, &startRun, &lengthRun);
					bool isRTL = (runDir == UBIDI_RTL);
					//printf("Processing Bidi Run = %lld -- run-start = %d, run-len = %d, isRTL = %d\n", i, startRun, lengthRun, isRTL);
					BidiScriptRunRecords scriptRunRecords;
					collectBidiScriptRuns(scriptRunRecords, testChars, startRun, lengthRun, isRTL);

					//print_time ftpt("ubidi_2");
					while (!scriptRunRecords.records.empty()) {
						ScriptRecord scriptRecord;
						if (scriptRunRecords.isRtl) {
							scriptRecord = scriptRunRecords.records.back();
							scriptRunRecords.records.pop_back();
						}
						else {
							scriptRecord = scriptRunRecords.records.front();
							scriptRunRecords.records.pop_front();
						}

						uint32_t     start = scriptRecord.startChar;
						uint32_t     end = scriptRecord.endChar;
						UScriptCode code = scriptRecord.scriptCode;
						auto scrn = icub->_uscript_getName(code);
						auto sc = get_script(scrn);
						std::string u8strd = u16_u8((uint16_t*)(testChars + start), end - start);
						info.push_back({ sc, u8strd, start, end, isRTL });
						//printf("Script '%s' from %d to %d.\t%d\n", scrn, start, end, sc);
					}
				}
			}
		}
		return;
	}
	static int icn = 0;
	hb_cx::hb_cx()
	{
		_buffer = hb_buffer_create();
		icn++;
	}

	hb_cx::~hb_cx()
	{
		free_font();
		icn--;
		auto kuf = hb_buffer_get_unicode_funcs(_buffer);
		hb_buffer_destroy(_buffer);
		if (icn == 0)
		{
			hb_unicode_funcs_destroy(kuf);
		}
	}

	tt_info* hb_cx::push_font(hz::tt_info* p)
	{
		tt_info* ret = 0;
		if (!p || _font.find(p) != _font.end())
		{
			return 0;
		}
		hb_blob_t* blob = hb_blob_create((char*)p->data, p->dataSize, HB_MEMORY_MODE_READONLY, 0, 0); //HB_MEMORY_MODE_WRITABLEhb_blob_create_from_file(ttfn);
		if (hb_blob_get_length(blob) > 0)
		{
			hb_face_t* face = hb_face_create(blob, p->_index);
			hb_font_t* hbf = hb_font_create(face);
			if (face && hbf)
			{
				//unsigned int upem = hb_face_get_upem(face);
				//hb_font_set_scale(hbf, upem, upem);
				//hb_font_set_ppem(hbf, ppem, ppem);
				_font[p] = hbf;
				ret = p;
			}
		}
		if (blob)
			hb_blob_destroy(blob);
		return ret;
	}
	void hb_cx::shape(const str_info_t& s, hz::tt_info* ttp, hb_dir dir)
	{
		shape(&s, ttp, dir);
	}
	void hb_cx::shape(const str_info_t* str, hz::tt_info* ttp, hb_dir dir)
	{
		if (_font.find(ttp) == _font.end())
		{
			return;
		}
		auto font = _font[ttp];
		if (font)
		{
			hb_buffer_t* buf = _buffer;
			hb_buffer_clear_contents(buf);
			hb_buffer_set_direction(buf, (hb_direction_t)dir);
#if 0
			switch (str->type)
			{
			case 0:
				hb_buffer_add_utf8(buf, (char*)str->str.p8, -1, 0, -1);
				break;
			case 1:
				hb_buffer_add_utf16(buf, str->str.p16, -1, 0, -1);
				break;
			case 2:
				hb_buffer_add_utf32(buf, str->str.p32, -1, 0, -1);
				break;
			default:
				assert(0);
				return;
				break;
		}
#else
			typedef void(*bufadd_func)(hb_buffer_t* buffer, const void* text, int text_length, unsigned int item_offset, int item_length);
			static bufadd_func cb[3] = { (bufadd_func)hb_buffer_add_utf8, (bufadd_func)hb_buffer_add_utf16, (bufadd_func)hb_buffer_add_utf32 };
			if (str->type < 3)
			{
				cb[str->type](buf, str->str.p8, str->text_length, str->item_offset, str->item_length);
				hb_buffer_guess_segment_properties(buf);
				hb_shape(font, buf, nullptr, 0);
			}
#endif // 0

	}
}

	int hb_cx::tolayout(const std::string& str, layout_text::dlinfo_t* dinfo, std::vector<lt_item_t>& outlt)
		//int hb_cx::tolayout(const std::string& str, hz::tt_info* ttf, double fontheight, bool is_word1, word_key& wks, std::vector<lt_item_t>& outlt)
	{
		hb_buffer_t* buf = _buffer;
		unsigned int count = hb_buffer_get_length(buf);
		if (count == 0)
		{
			return 0;
		}
		hz::tt_info* ttf = dinfo->font;
		double fontheight = dinfo->font_height;
		bool is_word1 = dinfo->is_word1;
		word_key& wks = *dinfo->wks;
		hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buf, nullptr);
		hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buf, nullptr);
		auto scale_h = ttf->get_scale_height(fontheight);
		//auto dir = hb_buffer_get_direction(buf);
		auto pstr = str.c_str();
		//auto uc = hz::get_utf8_count(u8p, str.size());
		outlt.reserve(outlt.size() + count);
		size_t bidx = outlt.size();
		size_t n = 0;
		size_t sc = str.size();
		const char* laststr = 0;
		int64_t last_cluster = infos[0].cluster; char32_t cp32;
		bool isn = sc != count;
		std::vector<int64_t> cluster;
		int ic = 1;
		bool isback = (dinfo->dir == layout_text::lt_dir::ltr) || (dinfo->dir == layout_text::lt_dir::ttb);
		if (!isback)
			cluster.push_back(sc);
		// 收集cluster判断是否连写
		for (unsigned int i = 0; i < count; i++)
		{
			cluster.push_back(infos[i].cluster);
		}
		if (isback) { cluster.push_back(sc); }
		else {
			ic = 0;	//std::reverse(cluster.begin(), cluster.end());
		}

		for (unsigned int i = 0; i < count; i++)
		{
			hb_glyph_info_t* info = &infos[i];
			hb_glyph_position_t* pos = &positions[i];
			glm::vec2 adv = { ceil(pos->x_advance * scale_h), ceil(pos->y_advance * scale_h) };
			glm::vec2 offset = { ceil(pos->x_offset * scale_h), -ceil(pos->y_offset * scale_h) };
			unsigned int cp = 0;
			laststr = Fonts::get_u8_last(pstr + info->cluster, &cp);
			cp32 = cp;
			auto git = ttf->mk_glyph(info->codepoint, cp, fontheight);

			lt_item_t lti = {};
			//memcpy(&lti, &git, sizeof(ftg_item));
			lti._image = git._image;
			lti._glyph_index = git._glyph_index;
			lti._rect = git._rect;
			lti._dwpos = git._dwpos;
			lti.advance = git.advance;
			lti.rtl = !isback;
			lti.adv = adv;
			lti._offset = offset;
			lti.ch[0] = cp;
			lti.chlen = 1;
			if (isn)
			{
				int64_t clu[2] = { info->cluster, cluster[i + ic] };
				if (clu[0] > clu[1]) { std::swap(clu[0], clu[1]); }
				int64_t dif = clu[1];
				if (clu[0] != clu[1])
				{
					int c = 0;
					unsigned int cp1 = 0;
					auto uc = get_utf8_count(pstr + clu[0], clu[1] - clu[0]);
					if (uc > 1)
					{
						laststr = pstr + clu[0];
						for (int k = 0; k < uc; k++)
						{
							laststr = Fonts::get_u8_last(laststr, &cp1);
							assert(c < 8);
							lti.ch[c++] = cp1;
						}
					}
					lti.chlen = uc;
				}
				if (dif == 1)
				{
					cp32 = 0;
				}
			}
			bool isctr = false;
			{

				static std::string ctn = R"(~!@#$%^&*()-+/\|/;'',.<>?`)";
				static std::u16string cn = uR"(【】、；‘：“’”，。、《》？（）——！￥)";
				if (cn.find(cp) != std::u16string::npos)
					isctr = true;
				if (cp < 255 && ctn.find(cp) != std::string::npos)
					isctr = false;
				if (cp < 255 && iscntrl(cp))
				{
					do
					{
						isctr = true;
					} while (0);
					if (cp != '\t')
						lti.adv.x = 0;
				}
			}
			lti.last_n = 0;
			lti.cluster = info->cluster;
			bool issplit = (cp < 255 && wks.get_split(cp));
			bool ispush = (is_word1 && cp > 255) || count - i == 1;
			//if (abs(info->cluster - last_cluster) > 2)
			//{
			//	issplit = true;
			//}
			last_cluster = info->cluster;
			//ispush = true;
			if (issplit || isctr)
			{
				if (n > 0)
				{
					outlt[bidx].last_n = n;
					bidx += n; n = 0;
				}
				ispush = true;
			}
			n++;
			outlt.push_back(lti);
			// 分割
			if (ispush)
			{
				outlt[bidx].last_n = n;
				bidx += n; n = 0;
			}
			//printf("%d\t", (int)adv.x);
		}

		//printf("\n");
		return count;
	}
	glm::ivec2 hb_cx::draw_to_image(const std::string& str, hz::tt_info* ttf, double fontheight, hz::Image* img, glm::ivec3 dstpos)
	{
		hb_buffer_t* buf = _buffer;
		unsigned int count = hb_buffer_get_length(buf);
		hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buf, nullptr);
		hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buf, nullptr);
		auto scale_h = ttf->get_scale_height(fontheight);

		auto bl = ttf->get_base_line(fontheight);
		double base_line = dstpos.z < 0 ? bl : dstpos.z;			// 基线
		//ttf ->get_VMetrics(&finfo[0], &finfo[1], &finfo[2]);
		int x = dstpos.x, y = dstpos.y;
		auto dir = hb_buffer_get_direction(buf);
		glm::ivec2 ret;
		glm::ivec2 dpos;
		auto u8p = str.c_str();
		//printf("%f\n", scale_h);
		for (unsigned int i = 0; i < count; i++)
		{
			hb_glyph_info_t* info = &infos[i];
			hb_glyph_position_t* pos = &positions[i];
			int adv = pos->x_advance * scale_h;
			int advy = pos->y_advance * scale_h;
			//printf("cluster %d	glyph 0x%x at	(%d,%d)+(%d,%d) %d\n",
			//	info->cluster,
			//	info->codepoint,
			//	pos->x_offset,
			//	pos->y_offset,
			//	pos->x_advance,
			//	pos->y_advance, adv);

			dpos.x = x + (pos->x_offset * scale_h);
			dpos.y = y - (pos->y_offset * scale_h) + base_line;
			unsigned int cp = 0;
			u8p = Fonts::get_u8_last(u8p, &cp);
			//auto git = ttf->mk_glyph(info->codepoint, cp, fontheight);
			auto size = hz::Fonts::mk_fontbitmap(ttf, info->codepoint, cp, fontheight, dpos, -1, img);
			//printf("%d\t", dpos.x);
			x += adv;
			//x += size.z;
			if (dir == HB_DIRECTION_TTB && abs(advy) < size.y)
				advy = -size.y;
			y -= advy;
			ret.x += adv;
			if (size.y > ret.y)
			{
				ret.y = size.y;
			}
		}
		//printf("\n");
		return ret;
	}
	void hb_cx::free_font()
	{
		for (auto& [k, v] : _font)
		{
			auto face = hb_font_get_face(v);
			hb_face_destroy(face);
			hb_font_destroy(v);
		}
		//hb_font_funcs_destroy(fcb);
		_font.clear();

	}

	layout_text::layout_text()
	{
		_hb = new hb_cx();// dc.ac<hb_cx>();
	}

	layout_text::~layout_text()
	{
		delop(_hb);
	}
	void layout_text::set_font_family(hz::tt_info** font_family, int count)
	{
		assert(_hb);
		if (font_family)
		{
			for (int i = 0; i < count; i++)
			{
				if (font_family[i])
				{
					auto p = _hb->push_font(font_family[i]);
					if (p)
						_familys.push_back(p);
				}
			}
		}
	}
	void layout_text::cpy(layout_text* p)
	{
		set_font_family(p->_familys.data(), p->_familys.size());
	}
	void layout_text::get_familys(hz::tt_info** bufptr, int first, int count)
	{
		do
		{
			if (count < 0)
				count = _familys.size() - first;
			if (!bufptr || (first + count > _familys.size()) || first < 0)
				break;
			std::copy_n(_familys.data() + first, count, bufptr);
		} while (0);
	}
	tt_info* layout_text::get_familys(const std::string& name, const std::string& style)
	{
		tt_info* ret = 0;
		tt_info* ret1 = 0;
		for (auto it : _familys)
		{
			if (style.size() && it->_style != style)
			{
				continue;
			}
			if (it->_name == name || it->_aname == name)
			{
				ret = it;
				break;
			}
			if (it->_name.find(name) || it->_aname.find(name))
			{
				ret1 = it;
			}
		}
		return ret;
	}
	size_t layout_text::size()
	{
		return _familys.size();
	}
	void layout_text::do_layout(const std::string& u8str, dlinfo_t* info, std::vector<lt_item_t>& outlt)
	{
		_hb->shape({ u8str.c_str(), (int)u8str.size(), 0, -1 }, info->font, (hb_cx::hb_dir)info->dir);
		_hb->tolayout(u8str, info, outlt);
		//info->rc = _hb->draw_to_image(u8str, info->font, info->font_height, info->img, info->pos);
	}


#if 0
#define DECLARE_WCS2UTF16CONV int _len; (_len);  uint32_t * _wcs; (_wcs); unsigned short * _utf16; (_utf16)
#define WCS2UTF16(wcs) \
	(((_wcs = wcs) == NULL) ? NULL : (\
									  _len = (utf16len(_wcs) + 1), \
									  wcs2utf16(_wcs, (unsigned short*)alloca(_len * 2), _len)))
#define UTF162WCS(utf16) \
		(((_utf16 = utf16) == NULL) ? NULL : (\
											  _len = (xwcslen(_utf16) + 1), \
											  utf162wcs(_utf16, (uint32_t*)alloca(_len * 2), _len)));
#endif
	unsigned short* utf32to16(uint32_t* wcs, unsigned short* utf16, int len)
	{
		int wc;
		uint32_t* end = wcs + len;
		unsigned short* ret = utf16;
		while (wcs < end)
		{
			wc = *(wcs++);
			if (wc > 0xFFFF)
			{
				wc -= 0x0010000UL;
				*utf16++ = 0xD800 | (wc >> 10);
				*utf16++ = 0xDC00 | (wc & 0x03FF);
			}
			else
			{
				*utf16++ = wc;
			}
		}
		return ret;
	}
	uint32_t* utf16_to32(unsigned short* utf16, uint32_t* wcs, int len)
	{
		int utf;
		unsigned short h;
		unsigned short l;
		uint16_t* end = utf16 + len;
		uint32_t* ret = wcs;
		while (utf16 < end)
		{
			utf = *utf16++;
			if (utf < 0xD800 || utf>0xDFFF)
			{
				*wcs++ = utf;
			}
			else
			{
				h = utf - 0xD800;
				l = *(utf16++) - 0xDC00;
				*wcs++ = ((h << 10) | l) + 0x10000;
			}
		}
		return ret;
	}


	word_key::word_key()
	{
		_sc.insert(HB_SCRIPT_HAN);
		_split_ch.insert(' ');
		_split_ch.insert('\t');
	}

	word_key::~word_key()
	{
	}
	void word_key::push(int sc)
	{
		_sc.insert(sc);
	}
	bool word_key::is_word(int sc)
	{
		return _sc.find(sc) != _sc.end();
	}
	int word_key::get_split(char c)
	{
		return _split_ch.find(c) != _split_ch.end();
	}
	static void WINAPI_STDCALL set_self_sizing(struct flex_item* item, float size[2])
	{
		size[1] *= 2;
		printf("");
	}

#ifndef MAXINT
#define MAXUINT     ((unsigned int)~((unsigned int)0))
#define MAXINT      ((int)(MAXUINT >> 1))
#define MININT      ((int)~MAXINT)
#endif
	static void cbox(item_box_info& boxinfo)
	{
		glm::ivec3 pos = { 0, 0, -1 };
		pos.x = 0;
		pos.y = 0;
		glm::ivec2 box = { MAXINT, 0 };
		glm::ivec2 box_width = { MAXINT, 0 };
		std::vector<glm::ivec4>& rects = boxinfo._rects;
		std::vector<lt_item_t>& items = boxinfo.items;
		boxinfo.vtf.reserve(items.size());
		text_flex_t tft = {};
		glm::ivec2 fx = { MAXINT, 0 };
		int fxn = 0;

		glm::ivec2 box_wh;
		int lc = 1;
		size_t i = 0;
		for (auto& it : items)
		{
			glm::ivec4 r = { 0, 0, it._rect.z, it._rect.w };
			r.z = it.adv.x;
			if (r.w < 1)r.w = it.adv.y;
			r.x += it._offset.x + pos.x + it._dwpos.x;
			r.y += it._offset.y + pos.y + it._dwpos.y;
			// 判断最小最大y
			if (r.y < box.x)
			{
				box.x = r.y;
			}
			if (r.y + r.w > box.y)
			{
				box.y = r.y + r.w;
			}
			// x
			if (r.x < box_width.x)
			{
				box_width.x = r.x;
			}
			if (r.x + r.z > box_width.y)
			{
				box_width.y = r.x + r.z;
			}

			// 判断元素块
			if (r.x < fx.x)
			{
				fx.x = r.x;
			}
			if (r.x + r.w > fx.y)
			{
				fx.y = r.x + r.z;
			}
			tft.push(it);
			if (boxinfo.is_rect)
				rects.push_back(r);
			if (fxn == 0)
			{
				fxn = it.last_n;
			}
			if (i == 16)
			{
				i = i;
			}
			i++;
			if (it.ch[0] == '\r')
			{
				fxn = fxn;
			}
			if (it.ch[0] == '\n')
			{
				glm::ivec2 rw = { box_width.y - box_width.x, box.y - box.x };
				box_wh.y += rw.y;
				if (rw.x > box_wh.x)box_wh.x = rw.x;
				boxinfo._row_size.push_back(rw);
				box = { MAXINT, 0 };
				box_width = { MAXINT, 0 };
				lc++;
			}
			fxn--;
			if (fxn == 0)
			{
				double dxx = 0;
				for (auto& dt : tft.item)
				{
					dxx += dt.adv.x;
				}
				tft.div.width = dxx;	// fx.y - fx.x;
				tft.rc = { fx.x, 0, tft.div.width, 0 };
				tft.rtl = it.rtl;
				boxinfo.vtf.push_back(tft);
				tft = {}; fxn = 0; fx = { MAXINT, 0 };
			}
			pos.x += it.adv.x;
		}
		glm::ivec2 rw = { box_width.y - box_width.x, box.y - box.x };
		box_wh.y += rw.y;
		if (rw.x > box_wh.x)box_wh.x = rw.x;
		boxinfo._row_size.push_back(rw);
		boxinfo.box_size = box_wh;
		boxinfo.box_size.y += lc * boxinfo.row_span;
		boxinfo.mheight = box.y - box.x;
		boxinfo.base_line_b = abs(box.x);
		boxinfo.raw_size = box;
		// 更新高度
		for (auto& it : boxinfo.vtf)
		{
			it.div.height = boxinfo.mheight;
			it.rc.w = boxinfo.mheight;// box_size.y;
		}
	}

	void layout_text::do_text(item_box_info* p)
	{
		familys_t ps = {};
		ps.familys = _familys.data();
		ps.count = _familys.size();
		do_text(p, &ps);
	}
	void layout_text::do_text(item_box_info* pinfo, familys_t* tps)
	{
		// todo int size_T
		familys_t ps[1] = {};
		if (tps)
		{
			ps[0] = *tps;
		}
		else
		{
			ps[0] = { _familys.data(), _familys.size() };
		}
		assert(ps->familys && ps->count > 0);
		if (!pinfo || !(*(ps->familys)) || ps->count < 1)return;
		auto& info = *pinfo;
		auto t = utf8_char_pos(info.s.str, info.s.first);
		auto te = utf8_char_pos(t, info.s.count);
		auto wk = hz::u8_u16(t, te - t);
		const uint16_t* str = (const uint16_t*)wk.c_str();
		size_t n = wk.size();
		std::vector<bidi_item> bidiinfo;
		{
			//print_time ftpt("bidi a");
			do_bidi((UChar*)str, n, bidiinfo);
			std::sort(bidiinfo.begin(), bidiinfo.end(), [](const bidi_item& bi, const bidi_item& bi1) { return bi.first < bi1.first; });
		}
		tt_info* renderFont = 0;
		auto font = ps->familys[0];
		std::vector<ritem_str_t> rv;
		//njson rn;
		rv.reserve(bidiinfo.size());
		glm::vec3 ad = { 0,1 ,0 };
		{
			info._rects.clear();
			info.items.clear();
			info.vtf.clear();
			//print_time ftpt("script font");
			for (auto& it : bidiinfo)
			{
				ritem_str_t rit;
				//size_t its = it.s.size();
				const char* str8 = it.s.c_str();
				auto bstr8 = str8;
				for (; *str8;)
				{
					int gidx = 0;
					bool isst = (*str8 != ' ');
					str8 = font->get_glyph_index_u8(str8, &gidx, &renderFont, ps);
					//rn.push_back((uint64_t)renderFont);
					if (isst && (rit.font != renderFont))
					{
						if (rit.font)
						{
							rit.is_word1 = wks.is_word(it.sc);
							rv.push_back(rit);
							rit = {};
						}
						rit.rtl = it.rtl;
						rit.font = renderFont;
					}
					rit.s.append(bstr8, str8);
					bstr8 = str8;
				}
				if (rit.font)
				{
					if (rv.size()) {
						auto rt = rv.rbegin();	//合并相同字体的字符串
						if ((rit.rtl == rt->rtl && rit.font == rt->font && rit.sc == rt->sc)) {
							rt->s.append(rit.s);
							continue;
						}
					}
					rit.sc = it.sc;
					rit.is_word1 = wks.is_word(it.sc);
					rv.push_back(rit);
				}
			}
		}
		{
			layout_text::dlinfo_t rinfo = {};
			rinfo.wks = &wks;
			//print_time ftpt("hb_shape");
			std::set<tt_info*> fnts;
			int base_line = 0;
			auto rn = rv.size();
			for (size_t i = 0; i < rn; i++)
			{
				auto& it = rv[i];
				rinfo.dir = it.rtl ? hz::layout_text::lt_dir::rtl : hz::layout_text::lt_dir::ltr;
				rinfo.font = it.font;
				rinfo.font_height = info.fontheight;
				rinfo.is_word1 = it.is_word1;

				if (ad.x < it.font->ascender)
				{
					ad.x = it.font->ascender;
				}
				if (ad.y > it.font->descender)
				{
					ad.y = it.font->descender;
				}
				double line_height = it.font->get_line_height(info.fontheight);
				if (ad.z < line_height)
					ad.z = line_height;
				do_layout(it.s, &rinfo, info.items);
				fnts.insert(it.font);
			}
			for (auto it : fnts)
			{
				int b = it->get_base_line(rinfo.font_height);
				if (b > base_line)
				{
					base_line = b;
				}
			}
			//info.base_line_f = base_line;
		}
		cbox(info);
		info.ad = ad;
		if (info.extent)
		{
			make_extent(info);
		}
	}

	double layout_text::get_base_line(double height)
	{
		double ret = 0.0;
		std::vector<glm::vec4> bob;
		std::set<double> blv;
		for (auto ft : _familys) {
			double s = ft->get_scale_height(height);
			bob.push_back(ft->get_bounding_box(s, true));
			auto b = ft->get_base_line(height);
			blv.insert(b);
			if (b > ret)
			{
				ret = b;
			}
		}
		return ret;
	}

	Fonts* layout_text::get_ctx()
	{
		Fonts* p = 0;
		for (auto it : _familys)
		{
			if (it->ctx)
			{
				p = it->ctx;
				break;
			}
		}
		return p;
	}

	void layout_text::make_extent(item_box_info& info)
	{
		text_extent& ret = *info.extent;
		ret.clear();
		auto str = info.s.str;
		auto t = utf8_char_pos(str, info.s.first);
		int rh = info.box_size.y + info.row_span;
		rh = info.fontheight;
		if (info.row_span > 1)
		{
			rh += info.row_span;
		}
		else
		{
			rh += info.fontheight * info.row_span;
		}
		ret._size.z = rh;
		int my = 0, cx = 0;
		int height = rh;
		glm::ivec2 brc;
		size_t count = 0;
		size_t idx = 0;
		ret.mk_last_line(t);
		ret._ycpos.push_back({ 0, ret._ystr.size() });
		auto ycposit = ret._ycpos.rbegin();
		ret.pushy(rh, my, t, brc, height);
		for (auto& it : info.vtf)
		{
			t = utf8_char_pos(t, count);
			count = 0;
			if (it.div.width == 0)
				t = t;
			if (*t == '\n')
			{
				idx++;
				count = 0;
				brc.x = 0;
				ret.mk_last_line(t);
				ret._ycpos.push_back({ 0, ret._ystr.size() });
				ycposit = ret._ycpos.rbegin();
				t++;
				ret.pushy(rh, my, t, brc, height);
				cx = my = 0;
				continue;
			}
			// 计算自动换行
			if (cx > 0 && ((unsigned int)(cx + it.div.width) >= ret._view_width))
			{
				ycposit->x++;			// 增加自动换行计数行
				idx++;
				ret.mk_last_line(t);
				brc.x += ret.getlast_y();
				brc.y--;
				ret.pushy(rh, my, t, brc, height);
				cx = my = 0;
			}
			for (auto& ct : it.item)
			{
				if (ct.ch[0] == '\n')
					cx = cx;
				if (ct.adv.x <= 0)
					cx = cx;
				else
				{
					if (ct.chlen > 1)
					{
						int cw = ct.adv.x / ct.chlen;
						for (int i = 0; i < ct.chlen; i++)
						{
							ret.pushx(cw, ct.rtl);
						}
					}
					else
					{
						ret.pushx(ct.adv.x, ct.rtl);
					}
				}
				count += ct.chlen;
			}
			cx += it.div.width;
		}
		t = utf8_char_pos(t, count);
		//ycposit->x++;
		ret.mk_last_line(t);
		ret.pushy(-1, -1, t, brc, height);
		ret._endstr = t;
		return;
	}


	void test_hb(hz::tt_info* Consolas, hz::tt_info* seguiemj, hz::tt_info* nsimsun, hz::tt_info* SourceHanSans
		, hz::tt_info* Majalla, hz::tt_info* Uighur, hz::tt_info* utsaah)
	{

		char* tk = (char*)u8"هذه بعض abcالنصوص العربية12這是一些中,文0.01fiyml0【{[]}\\|<>01p0.Y.01LHd fsdfds😘\nसंस्कृता वाfdsfdsئۇيغۇرfabcABGPYjyqml\ni12303 word";
		//tk = (char*)u8"😘aylfipwm";
		//tk = (char*)texts[2];
		std::string str = (char*)u8"a😘0123456789\tabcy";
		str += tk;

		//str = (char*)u8"a😘0123\x1f361 abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123(进程 20912)已退出，代码为 0。要在调试停止时自动关闭控制台，请启用“工具”->“选项”->“调试”->";
		//str += (char*)u8"“调试停止时自动关闭控制台”。按任意键关闭此窗口. . .";
		//str = (char*)u8"ff fi fl ffi ffl fffl ft fifi\r\n f i";
		//str = (char*)u8"النصوص العربية";
		//str = (char*)u8"ئۇيغۇر";
		//str = (char*)u8"संस्कृता वा";
		//word_key wks;
		//auto wk = hz::u8_u16(str);
		layout_text lt;
		std::vector<tt_info*> familys = { SourceHanSans, nsimsun, Consolas, Majalla, Uighur, seguiemj, utsaah };
		lt.set_font_family(familys.data(), familys.size());	// 设置字体集

		item_box_info res;
		res.s.str = str.c_str();
		res.s.count = get_utf8_count(res.s.str, str.size());
		res.s.first = 0;
		res.fontheight = 16;
		hz::text_extent str_rc;
		res.extent = &str_rc;
		// 设置视图宽度
		str_rc.set_view_width(520, 10);
		{
			print_time ftpt("do_text");
			lt.do_text(&res);
		}


		int width = 2024, height = 1024;
		hz::Image* img = hz::Image::create_null(width, height);
		//glm::ivec2 dpos;
		glm::ivec3 pos;
		img->clear_color(0xff000000);

		//rinfo.pos = pos;
		//int my = 0;

		//float sht[] = { it.font->get_scale_height(fontheight),it.font->get_scale_height(-fontheight) };

		pos.x = 0;
		pos.y = res.box_size.y * 2;

		for (auto r : res._rects)
		{
			r.x += pos.x;
			r.y += pos.y;

			img->draw_rect(r, 0xff80ff00, 0);
		}
		double dy = res.box_size.y * 3;
		auto fs = res.vtf.size();
		std::vector<size_t> vbr;
		for (size_t i = 0; i < fs; i++)
		{
			auto& it = res.vtf[i];
			it.div.height = res.box_size.y;
			glm::ivec4 r = { it.rc.x, res.box_size.y * 4.1, it.rc.z, res.box_size.y };
			img->draw_rect(r, 0xff0080ff, 0);
			double dx = 0;
			for (auto ct : it.item)
			{
				glm::ivec2 dpos = { dx + r.x + ct._dwpos.x + ct._offset.x, dy + res.base_line_b + ct._dwpos.y + ct.adv.y + ct._offset.y };
				img->copyImage(ct._image, dpos, ct._rect);
				dx += ct.adv.x;
				if (i == fs - 2)
				{
					i = i;
				}
				for (int m = 0; m < 8; m++)
				{
					if (ct.ch[m] == '\n')
						vbr.push_back(i);
				}
			}
		}
		{
			glm::ivec4 r = { 0, dy, res.box_size.x, res.box_size.y };
			img->draw_rect(r, 0xff8000ff, 0);
		}


		hz::flex_cx fc;
		// 创建一组布局
		std::vector<text_flex_t> _root;
		_root.reserve(res.vtf.size());
		_root.push_back({});
		_root[0].div = { 800, 200, (int)(1 + vbr.size()) };
		size_t lbsy = 0;
		for (auto it : vbr)
		{
			_root.push_back({});
			_root.rbegin()->div = { 520, 2.0f * res.box_size.y, (int)(it - lbsy) };
			for (size_t i = lbsy; i < it; i++)
			{
				_root.push_back(res.vtf[i]);
			}
			lbsy = it;
		}
		lbsy++;
		if (lbsy < res.vtf.size())
		{
			_root.push_back({});
			size_t nfs = res.vtf.size();
			_root.rbegin()->div = { 520, 1.0f * res.box_size.y, (int)(nfs - lbsy) };
			for (size_t i = lbsy; i < nfs; i++)
			{
				_root.push_back(res.vtf[i]);
			}
		}
		//int sslen = sizeof(flex::flex_align_t);
		flex_item* rt = fc.set_layout((hz::flex::div_value*)_root.data(), _root.size(), sizeof(text_flex_t));
		auto fcb = hz::flex_cx::get_item_func();
		flex_item_set_wrap(rt, FLEX_WRAP_NO_WRAP);
		//flex_align fas[] = { FLEX_ALIGN_AUTO,
		//	FLEX_ALIGN_STRETCH,
		//	FLEX_ALIGN_CENTER,
		//	FLEX_ALIGN_START,
		//	FLEX_ALIGN_END,
		//	FLEX_ALIGN_SPACE_BETWEEN,	//两端对齐，两端间隔0，中间间隔1
		//	FLEX_ALIGN_SPACE_AROUND,	//分散居中,两端间隔0.5，中间间隔1
		//	FLEX_ALIGN_SPACE_EVENLY,	//分散居中,每个间隔1
		//};
		// 横向
		flex_item_set_justify_content(rt, FLEX_ALIGN_START);//root不能是FLEX_ALIGN_STRETCH
		// 纵向
		flex_item_set_align_content(rt, FLEX_ALIGN_START);
		// 方向
		flex_item_set_direction(rt, FLEX_DIRECTION_COLUMN);
		// 孩子纵轴（上下）FLEX_ALIGN_STRETCH - FLEX_ALIGN_END
		flex_item_set_align_items(rt, FLEX_ALIGN_START);
		//fcb->item_set_left(rt, opos.x);
		//fcb->item_set_top(rt, opos.y);
		//auto fcount = _root.size();
		//for (size_t i = 1; i < fcount; i++)
		//{
		//	auto it = res.items[i].div.p;
			// padding内边距
		fcb->item_set_padding_left(rt, 10);
		fcb->item_set_padding_right(rt, 10);
		fcb->item_set_padding_top(rt, 10);
		fcb->item_set_padding_bottom(rt, 10);
		//	// margin外边距
		//	fcb->item_set_margin_left(it, 2);
		//	fcb->item_set_margin_right(it, 2);
		//	fcb->item_set_margin_top(it, 2);
		//	fcb->item_set_margin_bottom(it, 2);
		//}
		auto rootp = rt;
		for (auto& ft : _root) {
			auto it = ft.div.p;
			if (ft.item.empty() && it != rootp) {
				flex_item_set_wrap(it, FLEX_WRAP_WRAP);
				/*		flex_item_set_direction(it, FLEX_DIRECTION_ROW);
						flex_item_set_justify_content(it, FLEX_ALIGN_START);
						flex_item_set_align_items(it, FLEX_ALIGN_CENTER);*/
						// 横向
				flex_item_set_justify_content(it, FLEX_ALIGN_START);
				// 纵向
				flex_item_set_align_content(it, FLEX_ALIGN_START);
				// 方向
				flex_item_set_direction(it, FLEX_DIRECTION_ROW);
				// 孩子纵轴（上下）FLEX_ALIGN_STRETCH - FLEX_ALIGN_END
				flex_item_set_align_items(it, FLEX_ALIGN_CENTER);

				//flex_item_set_align_self(it, FLEX_ALIGN_STRETCH);
				flex_item_set_self_sizing(it, set_self_sizing);
			}
			else
			{
				// margin外边距
				fcb->item_set_margin_left(it, 2);
				fcb->item_set_margin_right(it, 2);
				fcb->item_set_margin_top(it, 2);
				fcb->item_set_margin_bottom(it, 2);

			}
		}
		fc.do_layout();

		glm::vec2 opos = { 10, res.box_size.y * 5.6 };
		int inx = 0;
		for (auto& ft : _root) {
			auto it = ft.div;
			auto pos1 = fc.get_item_merge_pos(it.p) + opos;
			auto size = fc.get_item_size(it.p);
			inx++;
			//if (inx == 1)
			if (ft.item.empty())
				img->draw_rect({ pos1, size }, 0xFF0000ff, 0);
			double dx = 0;
			for (auto ct : ft.item)
			{
				glm::ivec2 dpos = { pos1.x + dx + ct._dwpos.x + ct._offset.x, pos1.y + res.base_line_b + ct._dwpos.y + ct.adv.y + ct._offset.y };
				img->copyImage(ct._image, dpos, ct._rect);
				dx += ct.adv.x;
			}
		}
		printf("base_line: %f\n", res.base_line_b);

		{
			struct flex_item* root = flex_item_with_size(100, 100);

			flex_item_set_wrap(root, FLEX_WRAP_NO_WRAP);
			flex_item_set_direction(root, FLEX_DIRECTION_COLUMN);
			flex_item_set_justify_content(root, FLEX_ALIGN_START);
			flex_item_set_align_items(root, FLEX_ALIGN_START);
			flex_item_set_align_content(rt, FLEX_ALIGN_STRETCH);

			flex_item_set_padding_top(root, 10);
			flex_item_set_padding_left(root, 10);
			flex_item_set_padding_right(root, 10);
			flex_item_set_padding_bottom(root, 10);

			struct flex_item* child1 = flex_item_with_size(80, 50);
			flex_item_set_align_self(child1, FLEX_ALIGN_STRETCH);
			flex_item_add(root, child1);
			struct flex_item* child2 = flex_item_with_size(10, 60);
			flex_item_set_align_self(child2, FLEX_ALIGN_STRETCH);
			flex_item_set_margin_top(child2, 5);
			flex_item_set_margin_left(child2, 5);
			flex_item_add(root, child2);

			flex_layout(root);
			std::vector<flex_item*> rv = { root, child1, child2 };
			//TEST_FRAME_EQUAL(child1, 10, 15, 75, 25);
			//auto p = root;
			opos.y += 220;
			for (auto it : rv) {
				auto pos1 = fc.get_item_merge_pos(it);
				auto size = fc.get_item_size(it);
				printf("%f,%f,%f,%f\n", pos1.x, pos1.y, size.x, size.y);
				pos1 += opos;
				img->draw_rect({ pos1, size }, 0xff0080ff, 0);
			}
			flex_item_free(root);
		}

		img->saveImage("a0harfbuzz_test1024.png");
		hz::Image::destroy(img);

	}

#endif // !_no_hb_


}
// !hz


#if 0
void list_view_div::set_size(const glm::vec2& s, const glm::vec2& itsize)
{
	njson& st = _style;
	st["justify_content"] = FLEX_ALIGN_SPACE_AROUND;// FLEX_ALIGN_START;
	st["align_content"] = FLEX_ALIGN_END;
	st["direction"] = FLEX_DIRECTION_ROW;		// 行优先
												//st["direction"] = FLEX_DIRECTION_COLUMN;		// 行优先
	st["wrap"] = FLEX_WRAP_WRAP;
	// 子元素属性
	_style_item["shrink"] = 0;
	//_style_item["margin_left"] = 6;
	//_style_item["margin_right"] = 6;
	//_style_item["margin_top"] = 6;
	//_style_item["margin_bottom"] = 6;
	_style_item["padding_left"] = 6;
	_style_item["padding_right"] = 6;
	_style_item["padding_top"] = 6;
	_style_item["padding_bottom"] = 6;
	//st["align_items"] = FLEX_ALIGN_CENTER;
	_size = s;
	_divctx.set_view(s);
	glm::ivec2 n2 = { ceil(s.x / itsize.x), ceil(s.y / itsize.y) };
	int n = n2.x * n2.y;
	//n = 32;
	for (size_t i = 0; i < n; i++)
	{
		_divctx.push(itsize);
		//_divctx.push2(itsize, itsize);
	}
	_divctx.init();
	_divctx.set_it_style(0, _style);
	for (size_t i = 0; i < _divctx.size(); i++)
	{
		//if (i > 0 && i % 2)
		//	_divctx.set_it_style(i, _style_item);
	}
	_divctx.layout();// 计算布局
	_dvs.resize(_divctx.size());
	int width = 2024, height = 1024;
	hz::Image* img = hz::Image::create_null(width, height);
	img->clear_color(0xff000000);
	glm::vec2 pos = { 100, 100 };
	for (size_t i = 1; i < _divctx.size(); i++)
	{
		auto& it = _dvs[i];
		auto rc = _divctx.get_merge(i);
		it = rc;
		njson kc;
		kc["a"] = rc.x;
		kc["b"] = rc.y;
		kc["c"] = rc.z;
		kc["d"] = rc.w;
		printf("%s\n", kc.dump().c_str());
		rc.x += pos.x;
		rc.y += pos.y;
		auto c = 0xffff8000;
		//if (i % 2)
		c = 0xff0080ff;
		img->draw_rect(rc, c, 0);
		rc.x += 6;
		rc.y += 6;
		rc.z += -6 * 2;
		rc.w += -6 * 2;
		img->draw_rect(rc, 0xffff8000, 0);
	}
	for (size_t i = 0; i < 1; i++)
	{
		auto& it = _dvs[i];
		auto rc = _divctx.get_merge(i);
		it = rc;
		njson kc;
		kc["a"] = rc.x;
		kc["b"] = rc.y;
		kc["c"] = rc.z;
		kc["d"] = rc.w;
		printf("%s\n", kc.dump().c_str());
		rc.x += pos.x;
		rc.y += pos.y;
		img->draw_rect(rc, 0xff00ff00, 0);
	}
	img->saveImage("temp/guitest_div.png");
	hz::Image::destroy(img);
	printf("");
}

#endif // 0
