#pragma	once
#include <math.h>
#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <string>
void test_flex();

#include "flex.h"

#ifdef __cplusplus
extern "C" {
#endif
	struct flex_item;

# ifndef FATE
#  define FATE(name, type, def) \
     type (*item_get_##name)(struct flex_item *item); \
     void (*item_set_##name)(struct flex_item *item, type value);
# endif
	struct attribute_func
	{
#include "flex_attribute.h"
	};
#undef FATE
#ifdef __cplusplus
}
#endif
#include "layout_info.h"
namespace hz {
	class Fonts;
	struct familys_t;
	class flex_cx
	{
	private:
		flex_item* _item = 0;
		size_t _item_cap = 0;
		auto_destroy_cx dc;
	public:
		attribute_func* atcb = 0;
	public:
		flex_cx();

		~flex_cx();
		flex_item* data();
		size_t size();
	public:
		void resize(size_t n);
		// stride结构体大小, 返回根flex_item
		flex_item* set_layout(flex::div_value* v, size_t count, int stride = 0);


		void free_layout();
		// 执行布局计算,	需要是根flex_item
		void do_layout();
		void set_item_info(flex_item* it, flex::flex_info_t* p);

		// 获取it的包含父节点坐标
		static glm::vec2 get_item_merge_pos(flex_item* it);
		// 获取节点坐标
		static glm::vec2 get_item_pos(flex_item* it);
		// 获取节点大小
		static glm::vec2 get_item_size(flex_item* it);
		static void set_item_size(flex_item* it, const glm::vec2& s);
		// 返回flex_item大小
		static int get_item_sizeof();
		// 返回操作属性函数指针组
		static attribute_func* get_item_func();
	private:

	};




	class word_key
	{
	private:
		// hb_script_t
		std::set<int> _sc;
		std::set<char> _split_ch;
	public:
		word_key();
		~word_key();
		void push(int sc);
		// hb_script_t
		bool is_word(int sc);
		int get_split(char c);
	private:

	};
	// 文本布局
	class layout_text
	{
	public:
		enum class lt_dir
		{
			invalid = 0,
			ltr = 4,	// 左-右
			rtl,		// 右-左
			ttb,		// 竖
			btt			// 倒竖
		};
		struct dlinfo_t
		{
			hz::tt_info* font;
			double font_height;
			lt_dir dir;
			//Image* img;
			// x,y ,z=baseline
			//glm::ivec3 pos;
			bool is_word1;
			word_key* wks = 0;
			// out
			glm::ivec2 rc;
		};
		std::vector<tt_info*> _familys;
	private:
		hb_cx* _hb = 0;
		//auto_destroy_cx dc;
		word_key wks;
	public:
		layout_text();
		~layout_text();
		// 设置字体集合
		void set_font_family(hz::tt_info** font_family, int count);
		void cpy(layout_text* p);
		void get_familys(hz::tt_info** bufptr, int first, int count);
		hz::tt_info* get_familys(const std::string& name, const std::string& style);
		size_t size();
		double get_base_line(double height);
		Fonts* get_ctx();
		void do_text(item_box_info* info);
		// familys要在set_font_family里先设置
		void do_text(item_box_info* info, hz::familys_t* familys);
	private:
		void do_layout(const std::string& u8str, dlinfo_t* info, std::vector<lt_item_t>& outlt);
		void make_extent(item_box_info& info);

	};
	/*
			layout_text lt;
		std::vector<tt_info*> familys = { SourceHanSans,nsimsun,Consolas, Majalla, Uighur,  seguiemj, utsaah };
		lt.set_font_family(familys.data(), familys.size());	// 设置字体集

		item_box_info res;
		res.s.str = str.c_str();
		res.s.count = get_utf8_count(res.s.str, str.size());
		res.s.first = 0;
		res.fontheight = 16;
		hz::text_extent str_rc;
		// 设置视图宽度
		str_rc.set_view_width(520);
		{
			print_time ftpt("do_text");
			lt.do_text(res);
		}
		{
			print_time ftpt("layout computing");
			lt.make_extent(res, &str_rc);
		}


	*/
	void test_hb(hz::tt_info* Consolas, hz::tt_info* seguiemj, hz::tt_info* nsimsun, hz::tt_info* SourceHanSans
		, hz::tt_info* Majalla, hz::tt_info* Uighur, hz::tt_info* utsaah);

	unsigned short* utf32to16(uint32_t* wcs, unsigned short* utf16, int len);
	uint32_t* utf16_to32(unsigned short* utf16, uint32_t* wcs, int len);
}
// !hz
