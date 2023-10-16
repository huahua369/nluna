#pragma once
// c
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
// cpp
#include <vk_core/view_info.h>
#include <vk_core/view.h>
#include <vk_core/world.h>
#include <view/flex_cx.h>
//#include <view/path.h>

#ifndef HZ_BIT
//位左移
#define HZ_BIT(x) (((uint64_t)1)<<(x))
#endif

/*
	窗口	：系统提供
	画布	：每个窗口可以绑定n个画布。
	事件	：管理事件监听，随时可以和窗口绑定解绑
	ui	：依赖事件、画布。列表、树、按钮、菜单、编辑框、标签
	布局	：flex

	页面管理器：管理

	开发流程：设计展示页面、数据结构、业务逻辑


	layout_text管理familys
	view系列：
		数据结构 + 模型 + 视图
					（事件）

	模型/视图都有处理事件。
	视图类: list_view、table_view、tree_view、custom_view
	模  型：节点渲染结构，文本、图标、矢量图等组合，有自己的布局
	数  据：数组、链表、树
*/
#if (__cplusplus >= 201100)
#define HZ_OFFSETOF(_TYPE,_MEMBER)  offsetof(_TYPE, _MEMBER)                    // Offset of _MEMBER within _TYPE. Standardized as offsetof() in C++11
#else
#define HZ_OFFSETOF(_TYPE,_MEMBER)  ((size_t)&(((_TYPE*)0)->_MEMBER))           // Offset of _MEMBER within _TYPE. Old style macro.
#endif

#ifndef __avtcd__
#define __avtcd__
struct av_time_t
{
	int second = 0;		// 返回秒数
	int hms[3] = {};	// Hour minute second
};
// 在回调函数修改一次设置成功后自动还原默认值
struct ctrl_data_t;

#endif // 0
struct videoplayinfo_t
{
	// play_ctx*fp;
	void* fp = 0;
	glm::ivec2 size = {};
	av_time_t vtime = {};
	hz::ui::bcs_t* gp = 0;
	hz::image_cs* pimg = 0;
	hz::ui::scroll_view_u* sp = 0;		// 在滚动视图显示
	hz::world_cx* ctx = 0;
	hz::dvk_texture* yuvtex = 0;
	hz::upload_cx* up = 0;
	hz::pipeline_ptr_info* yuvpipe = 0;
	glm::vec2 tvs, tps;//设置
	std::atomic_bool* _ts = 0;
	std::atomic_bool* isfree = 0;

	// 操作函数
	void (*ff_set)(void* fp, ctrl_data_t* c) = 0;
	// 获取时间
	av_time_t(*ff_get_time)(void* fp) = 0;
	// 删除本对象，也可以退出时自动删除
	void (*free_video)(videoplayinfo_t* vp) = 0;
	void (*set_pos)(videoplayinfo_t* p, const glm::vec2& ps);
	void (*set_size)(videoplayinfo_t* p, const glm::vec2& v);
	glm::vec2(*get_size)(videoplayinfo_t* p);
};

namespace hz
{
#if 1
	// ui基本图形
	struct line_cs;
	struct rect_cs;
	struct ellipse_cs;
	struct circle_cs;
	// 三角形基于矩形内
	struct triangle_cs;
	// 路径可以填充可线条
	struct path_cs;
	// 图像基本属性有：区域、偏移、九宫格；
	struct image_cs;
	// text渲染对象
	struct text_cs;
	//struct image_cs1;
	// 图像min：区域、偏移；
	struct image_m_cs;
	struct box_shadow_t;
	struct Move2d;
#endif

#include "css_base.h"
	using entt::entity;
	class event_master;
	class bw_sdl;
	class sem_s;
	class sdl_form;
	class page_view;
	class Image;
	class tt_info;
	class path_stb;
	class effect_cx;
	class menu_page_t;
	template<class T>
	class asyn_queue_x;
	// "新宋体","宋体","楷体","微软雅黑","微软繁隶书","思源黑体",
	// "Arial", "Amiri", "Consolas", "Segoe UI Emoji", "Times New Roman", "entypo", "Microsoft Uighur"
	//struct ts_ttinfo_t {

	//	hz::tt_info* nsimsun, * simsun, * simkai, * msyh, * msfls, * SourceHanSans,
	//		* Arial, * Amiri, * Consolas, * seguiemj, * tnr, * entypo, * Uighur
	//		;
	//	auto_destroy_cx adc;
	//};
	struct font_style_c;
	struct ui_mem_t {
		//auto_destroy_cx adc;
		event_master* emt = 0;			// 事件管理器
		layout_text* lt = 0;
		font_style_c* _ust = 0;
	};
	// 图标
	// canvas->draw_image(icp->img, a + icon->offset, icon->rect);
	struct icon_rt
	{
		Image* img = 0;			// 所在图像
		glm::ivec4 rect;		// 区域
		glm::ivec2 offset;		// 渲染偏移
	};

	enum class dc_e :uint8_t {
		e_null,
		e_font_style,
		e_shadow,
		e_bg,
		e_border,
		e_content,
		e_max
	};
	//enum class gte :uint8_t {
	//	e_null = (uint8_t)dc_e::e_max,
	//	e_rect,
	//	e_circle,
	//	e_triangle,
	//	e_path,
	//	e_text, e_image,
	//	e_max
	//};

	struct font_style_c
	{
		dc_e type = dc_e::e_font_style;
		void* next = 0;
		int font_size = 12;		// 字号
		int font_dpi = 96;		// 分辨率
		glm::vec2 size;			// 大小*
		float row_span = 2.0;	// 行距
		bool autobr = false;	// 自动换行
		int font_height = 0;
		int font_row_span = 0;
	};
	//layout_text*font_style_c*

	int mk_rs(font_style_c* ust);

	struct shadow_c
	{
		dc_e type = dc_e::e_shadow;
		void* next = 0;
		icon_rt shadow;		// 阴影生成目标
		glm::ivec2 offset;		// X方向的偏移像素，Y方向的偏移像素
		uint32_t blur;			// 模糊的像素值
		uint32_t n;				// 次数
		uint32_t color;			// 阴影颜色
	};
	//
	struct size_c
	{
		glm::ivec4 rc;	// 矩形
	};
	// 背景边框内容可以用不同的阴影
	// 背景		只有两种矩形、图片，阴影可选
	struct bg_c
	{
		dc_e type = dc_e::e_bg;
		void* next = 0;
		uint32_t fill;			// 填充颜色
		float radius;			// 圆角

		icon_rt img;		// 图片
		shadow_c* shadow = 0;	// 阴影
	};
	// 边框
	struct border_c
	{
		dc_e type = dc_e::e_border;
		void* next = 0;
		uint32_t color;			// 颜色
		float thickness;		// 粗
		float radius;			// 圆角
		shadow_c* shadow = 0;	// 阴影
	};
	// 内容
	struct content_c
	{
		dc_e type = dc_e::e_content;
		void* next = 0;				// 可存text_c\image_c之类
		uint32_t color;				// 颜色
		glm::ivec4 clip;			// 裁剪区域
	};


	/*
	*
		前端ui基本属性：形状、样式、坐标
		形状：矩形、圆、三角形、路径、文本
		样式：粗细、圆角、颜色、纹理、大小、字号、字体、阴影、模糊、渐变...更多样式
		坐标：受控于布局、绝对位置、相对位置

		一个实体由n个形状、n个样式组成，每套样式可对应一个或多个形状，
	*/







	/*	todo 形状结构：颜色、填充、粗细在另一结构

	*	矩形
	*	┌───────┐
	*	│       │
	*	└───────┘
	*	圆角矩形
	*	╭───╮
	*	│   │
	*	╰───╯
	*/
	struct rect_e
	{
		gte type = gte::e_rect;
		void* next = 0;
		glm::vec2 size;			// 大小
		glm::ivec4 rounding;	// 四个圆角，x大小0时，yzw小于0，则全部用一种圆角
	};
	//	圆		radius.xy一样则为圆，不同则是椭圆
	struct circle_e
	{
		gte type = gte::e_circle;
		void* next = 0;
		glm::vec2 radius;
	};
	/*
		三角形
		┌───┐
		│ ╲ │
		│  ╲│
		│  ╱│
		│ ╱ │
		└───┘
		这是一个右方向三角形
	*/
	struct triangle_e
	{
		gte type = gte::e_triangle;
		void* next = 0;
		uint8_t dir = 0;		// 尖角方向，0上，1右，2下，3左
		uint8_t pos = 50;		// 尖角点位置0-100，默认中间50
		glm::vec2 size;			// 大小
	};
	/*
		路径
			<path> 标签用来定义路径
		命令	名称	参数
		M	moveto  移动到	(x y)+
		Z	closepath  关闭路径	(none)
		L	lineto  画线到	(x y)+
		H	horizontal lineto  水平线到	x+
		V	vertical lineto  垂直线到	y+
		C	curveto  三次贝塞尔曲线到	(x1 y1 x2 y2 x y)+		（x1,y1 是曲线起点的控制点坐标x2,y2 是曲线终点的控制点坐标 x,y 是曲线的终点坐标）
		S	smooth curveto  光滑三次贝塞尔曲线到	(x2 y2 x y)+
		Q	quadratic Bézier curveto  二次贝塞尔曲线到	(x1 y1 x y)+
		T	smooth quadratic Bézier curveto  光滑二次贝塞尔曲线到	(x y)+
		A	elliptical arc  椭圆弧	(rx ry x-axis-rotation large-arc-flag sweep-flag x y)+
		R	Catmull-Rom curveto*  Catmull-Rom曲线	x1 y1 (x y)+		暂无实现
		以上所有命令均允许小写字母。大写表示绝对定位，小写表示相对定位。
	*/
	struct path_e
	{
		gte type = gte::e_path;
		void* next = 0;
		t_string	cmdstr;					// 解析命令CCCz表示需要有6个vec2数据对应
		vector_vec2 path;					// 坐标比如，M需要一个vec2，而C要三个vec2
		glm::vec2 scale = { 1.0, 1.0 };		// 缩放
	};

	struct text_c
	{
		gte type = gte::e_text;
		void* next = 0;
		uint32_t color;				// 颜色
		t_string str;				// 文本
		item_box_info* rbox = 0;	// 渲染盒
		//shadow_c* shadow = 0;		// 阴影
	};
	struct image_c
	{
		gte type = gte::e_image;
		void* next = 0;
		icon_rt img;			// 渲染盒
		//shadow_c* shadow = 0;		// 阴影
	};
	struct geo_e
	{
		gte type = gte::e_null;
		void* next = 0;
	};

	class geo_ac
	{
	private:
		// pmr内存池
#ifdef _pmr_pool_nt
		uspool_t usp;
#endif // _pmr_pool_nt
		t_vector<geo_e*> _data;
	public:
		geo_ac();
		~geo_ac();
		// 在p后面添加一个对象
		template<class T>
		T* new_geo(const T& v, void* p)
		{
			T* t = 0;
#ifdef _pmr_pool_nt
			t = (T*)usp.allocate(sizeof(T));
			*t = v;
			//::new(t)T(v);
#else
			t = new T(v);
#endif
			_data.push_back((geo_e*)t);
			if (p)
			{
				auto e = (geo_e*)p;
				t->next = e->next; e->next = t;
			}
			return t;
		}
		void clear();

		template<class T>
		T* new_obj(const T& v)
		{
			T* t = 0;
#ifdef _pmr_pool_nt
			t = (T*)usp.allocate(sizeof(T));
			*t = v; //::new(t)T(v);
#else
			t = new T(v);
#endif
			return t;
		}

		template<class T>
		void free_geo(T* t) {
			if (t)
			{
#ifdef _pmr_pool_nt
				t->~T();
				usp.deallocate(t, sizeof(T));
#else
				delete t;
#endif
			}
		}
	private:
	};
	struct box_color_c {

		glm::ivec4 padding = { 2, 2, 2, 2 };
		uint32_t background = 0xF0414141;
		uint32_t border_color = 0xFF313131;
		uint32_t border = 1;
		int border_radius = 4;
		glm::ivec4 radius;
	};

	// 对话框Modal


	// flex的生成结构
	struct layout_new_info_t
	{
		// 容器大小
		glm::vec2 size = {};
		// 子项大小vec2指针，数量，偏移
		void* v2ptr = 0;
		size_t n = 0;
		int v2offset = 0;
		// auto_left大小范围
		int aw = 6;
		// 父级样式{}
		njson root_css;
		// 数组则数量和n一至，全部则为对象，[{},{}]、{}
		njson child_css;
		// 是否自动左对齐补全
		//bool auto_left = true;
	};
	// todo easy flex
	class easy_flex
	{
	public:
		t_vector<flex::div_value> _data;
		flex_cx fc;
	public:
		easy_flex();
		~easy_flex();
		// 设置根大小
		void set_view(const glm::vec2& size);
		void push(const glm::vec2& size);
		void push2(const glm::vec2& size, const glm::vec2& size1);
		void pop();
		void erase(size_t idx);
		// 初始化节点
		void init();
		void set_it_style(size_t idx, const njson& st);
		void layout();
		size_t size();
		glm::vec4 get_merge(size_t idx);
		bool get_isabs(size_t idx);
	public:
		// 输入容器大小、子项大小、样式、输出子项坐标大小
		static void build(layout_new_info_t* p, std::function<void(int idx, const glm::vec4& rc, bool isabs)> cb);
	private:

	};



	class cpg_time;
	// 旋转效果等
	struct anim_a
	{
		motion_t mot = {};
		float acce = 0;	// 加速倍数
		float urx = 1;	// 旋转乘倍数
		glm::vec2 rot_pos;			// * 旋转中心
		//private:
		glm::vec2 dst = {};			// 结果缓存
		glm::vec3 ft = { 0,1,0 };	// 中间值缓存
		bool indefinite = true;		// * indefinite是否来回循环,false则每次从头开始
		bool isck = false;			// * 控制执行
		bool isexe = false;			// 判断是否到头了
	public:
		// 大于0
		void set_acce(float v);
		double get();
		void up_rotate(bool isck, const glm::vec2& rpos, cpg_time* t, bool smoothtime = false);
		void up_rotate(const glm::vec2& rpos, cpg_time* t);

		// (value) ? 1 : -1;方向；返回是否到头
		bool update(double delta_time, int value);
		void calc_pos(glm::vec2 pos, glm::vec2 pos1);
		void calc_scale(glm::vec2 pos, glm::vec2 pos1);
		void calc_rotate(float q1, float q2);
		void calc_rotate(double rx);
	};




	class buffer_t;
	// *号必选
	struct text_view_t
	{
		text_view_t* next = 0;					// 链表方便批量处理

		item_box_info* res_box = nullptr;		// *渲染盒

		layout_text* lt = nullptr;				// *可共用，管理familys

		buffer_t* buf = nullptr;				// 编辑存储

		event_data_cx* text_ed = nullptr;		// 事件管理
		edit_es_t* ep = nullptr;				// 编辑器事件处理、协同渲染
		text_extent* str_rc = nullptr;			// 鼠标拾取、坐标定位
		item_box_info* _editing = nullptr;		// 编辑中

		std::string str;						// *文本数据
		std::string* _pwd = 0;
		box_color_c bcc;

		glm::vec4 _clip;
		bool single_line = false;
		// change可以修改str数据
		std::function<void(std::string* str, const std::string& old)> on_change;
		std::function<void(std::string* s)> on_input;
		int iec = 0;
	public:
		text_view_t();
		~text_view_t();
		void set_text(const char* str, int len);
		void set_text(const std::string& str);
#ifndef _NO_PMR_
		void set_text(const t_string& str);

#endif // !_NO_PMR_

		t_string get_text();
		const char* c_str();
		size_t c_size();
		void set_pos(const glm::ivec2& pos);
		glm::ivec2 get_pos();
		// 设置像素高
		void set_font_height(double fs);
		void set_row_span(double row_span, bool autobr);
		void set_size(const glm::ivec2& size);
		//void set_color(uint32_t border, uint32_t background);
		void set_text_color(uint32_t tc, uint32_t sc, uint32_t cursor_color);
		// 行高
		double row_size();
		// 总大小
		glm::ivec2 size();

		bool isinput();
		bool ishot();
		void set_visible(bool is);
		//void text_up_cb(edit_es_t* p, bool is_input, bool is_editing);
		void update(cpg_time* t);
		void draw(cpg_time* t, canvas_cx* canvas);
	private:
		void uphot();

	};



	void text_up_cb(edit_es_t* p, bool is_input, bool is_editing, text_view_t* evt);

	text_view_t* gui_test(event_master* emt);
	// 0就是只有显示，1事件，只读、鼠标选择
	enum class ui_text_style :uint8_t
	{
		e_none = 0,
		e_event = 1,
		e_is_read_only = 2,
		e_select = 4,
		e_all = 0xff
	};
	DR_ENUM(ui_text_style);
	/*
		event_data_cx *text_ed坐标、拾取区域设置
	*/

	text_view_t* init_text_view(text_view_t* tvp, ui_text_style_t style, font_style_c* st, const char* str, ui_mem_t* uam = 0);
	void text_set_event_master(text_view_t* tvp, event_master* emt);
	/*
	scrollview
	风格：	e_none不滚动，
			e_vertocal垂直
			e_horizontal水平
			e_drag自由拖动
	*/
	enum class ui_scroll_style :uint8_t
	{
		e_none = 0,
		e_vertocal = 1,
		e_horizontal = 2,
		e_drag = 4,
	};
	DR_ENUM(ui_scroll_style);


	struct icon_rt;
	enum class cols_type :uint8_t {
		normal,		//（常规列，无需设定）
		custom,		// (自定义)
		checkbox,	//（复选框列）
		radio,		//（单选框列）
		numbers,	//（序号列）
		space,		//（空列）
	};
	struct col_info_t
	{
		t_string key;			// 字段名
		t_string title;			// 标题名
		double width = 0.0;		// 小于0则自动列宽
		double minWidth = 0.0;	// 最小列宽，可选
		cols_type type = cols_type::normal;			// 类型
		bool totalRow = false;	// 合计计算
		bool sort = false;		// 排序按钮
		bool unresize = false;	// 不可改变列宽
		bool visible = true;	// 可见
		njson custom;			// 自定义本列渲染方式，按钮、编辑、图标、颜色块等
	};

	enum class ct_type :uint8_t {
		normal,
		div_u,
		scroll_view_u,
		button_u,
		tag_u,
		switch_u,
		radio_u,
		checkbox_u,
		input_u,
		combobox_u,
		progress_u,
		slider_u,
	};

	namespace flex {

		void set_value(flex_item* it, const njson& v, attribute_func* cbs);
		njson get_value(flex_item* it, attribute_func* cbs);
	}

	namespace ui {


		class gtac_c;
		// 颜色
		struct box_color_u
		{
			uint32_t color = 0;				// 文本/内容颜色
			uint32_t fill_color = 0;		// 填充颜色
			uint32_t border_color = 0;		// 边框颜色
		};
		// 圆、圆角的属性
		struct radius_u
		{
			glm::ivec4 radius;
		};

		// 每种状态可以绑定多个形状、样式
		class ui_base_u
		{
		public:
			t_vector<entity> normal;			// 正常状态
			t_vector<entity> hover_pressed;	// 悬浮+点中
			t_vector<entity> pressed;			// 点中
			t_vector<entity> hover;			// 悬浮
			t_vector<entity> disabled;		// 禁用
		public:
			ui_base_u();
			~ui_base_u();

		private:

		};

		struct btn_info_u
		{
			t_string str;
			int idx = 0;
			entity e;
			//const char* icon = nullptr;
			icon_rt icon = {};
			size_t len = 0;
			//tt_info* font_icon = nullptr;
			//int icno_fs = 16, icon_dpi = 96;
			//css_text ct = {};
			glm::vec2 pos;
			glm::vec2 size;
			int rounding = 4;

			unsigned int back_color = 0;
			unsigned int text_color = 0;
			double opacity = 1.0;
			// private
			unsigned int gradTop = 0;
			unsigned int gradBot = 0;
			unsigned int borderLight = 0;
			unsigned int borderDark = 0;
			bool mPushed = false;
			bool mChecked = false;
			bool mMouseFocus = false;
			bool mEnabled = true;
			bool is_muilt = true;
			int  bs = 0;
			glm::vec2 pos1;
			//item_box_info _label = {};
			text_cs* dtc = 0;
		public:
			const char* c_str();
		};
		/*
		1.普通状态2，鼠标hover状态  3.active 点击状态  4.focus 取得焦点状态  4.disable禁用状态
		*/
		enum class BTN_STATE :uint8_t
		{
			STATE_NOMAL = HZ_BIT(0),
			STATE_HOVER = HZ_BIT(1),
			STATE_ACTIVE = HZ_BIT(2),
			STATE_FOCUS = HZ_BIT(3),
			STATE_DISABLE = HZ_BIT(4),
		};
		void button_init(btn_info_u* info, glm::ivec4 rect, const t_string& text, unsigned int back_color = 0, unsigned int text_color = -1);
		void btn_set_state(btn_info_u* info, int bst);
		void gui_draw_btn(canvas_cx* g, btn_info_u* p);


		struct base_g_info_t {

			event_data_cx* _listen = 0;
			entt::registry* reg = 0;
			layout_text* _lt = 0;
			font_style_c* _ust = 0;
			gtac_c* ptm = 0;
			int font_height = 16;
		};
		// checkbox/radio组
		class group_rcb_u
		{
		public:
			struct checkbox_u {
				checkbox_u* g_next = 0;	// 链表
				bool v = false;			// 值

				bool hover = false;
				bool is_radio = false;
				glm::vec4 radius = { 2, 2, 2, 2 };
				glm::vec2 pos;
				glm::vec2 size;
				float square_sz = 18;
				uint32_t fill_col = 0x80FF9E40;
				uint32_t hover_col = 0x50ffffff;
				uint32_t down_col = 0x30ffffff;
				uint32_t check_col = 0xaaffffff;
				entity e = {};
				item_box_info _label = {};
				int id = -1;
			public:
				const char* c_str();
				const char* v_str();
			};
			base_g_info_t binfo;
			t_vector<checkbox_u> child;		// 基础数据
			t_vector<entity> ety;		// 绑定 文本、样式等
			bool is_radio = false;
			// 组的坐标、大小
			glm::vec2 _pos;
			glm::vec2 _size;
			uint32_t _text_color = 0xffffffff;
			uint32_t _back_color = 0xffffffff;
			int bt_space = 4;
			// 单击回调
			std::function<void(checkbox_u*)> on_click;
		public:
			group_rcb_u(bool isradio = false);
			~group_rcb_u();
			void init(entt::registry* reg, event_data_cx* listen, layout_text* lt, font_style_c* ust);

			checkbox_u* resize(int w, int n);
			void set_title(const t_vector<t_string>& tv);
			checkbox_u* get_ptr();

			void set_size_pos(const glm::vec2& s, const glm::vec2& pos);
			void set_pos(const glm::vec2& pos);
			void set_radio(checkbox_u* p);
			void set_radio(int idx);
			void set_check(checkbox_u* p, bool is);
			void set_check(int idx, bool is);
			// 清空元素
			void clear();
			void update_draw(cpg_time* t, canvas_cx* canvas);
		public:
			static void renderCheckMark(canvas_cx* canvas, glm::vec2 pos, uint32_t col, float sz);
			void draw_checkbox(canvas_cx* canvas, checkbox_u* c);
			void onclick_g(void* ptr, int idx, int count);
		};
		// 按钮组
		class group_btn_u
		{
		public:
			base_g_info_t binfo;
			// 单击回调
			std::function<void(btn_info_u* p)> on_click;
			// 组的坐标、大小
			glm::vec2 _pos;
			glm::vec2 _size = { 100,100 };
			glm::vec2 c_size = { -36,-16 };
			uint32_t color = 0x25000000;

		private:
			t_vector<btn_info_u>	child;		// 默认按钮状态数据
			t_vector<entity>	ety;		// 按钮实体		绑定自定义属性
			entity _gety = {};				// 组实体		用于设置边框底纹之类

		public:
			group_btn_u();
			~group_btn_u();
			//void init(entt::registry* reg, event_data_cx* listen, layout_text* lt, font_style_c* ust);
			void init(entt::registry* reg, event_data_cx* listen, gtac_c* pt, int font_height);
			void set_size(const glm::vec2& s);
			void set_pos(const glm::vec2& pos);
			void set_title(const t_vector<t_string>& tv);

			void clear();
			btn_info_u* resize(int w, int n);
			// 更新绘图
			void update_draw(cpg_time* t, canvas_cx* canvas);
		private:
			void update_btn(btn_info_u* p, const glm::vec2& pos);
			void onclick_g(void* ptr, int idx, int count);
		};
		// 蜡烛台
		struct candlestick_css {
			glm::ivec2 color = { 0xff0020ff, 0xff00ff20 };		// 显示颜色：阳、阴
			glm::vec2 thickness = { 5, 1 };	// 宽度，线粗
			float height;			// 图表高
			float space = 1;		// 间隔
			float tks = 1;			// 线粗
			uint32_t macol[4] = { 0xffC67054, 0xff75CC91, 0xff58C8FA, 0xff6666EE };
		};
		// 蜡烛台图表
		class charts_u1
		{
		private:
			t_vector<glm::vec4> kdata;
			glm::vec2 _pos;
			glm::vec2 _size;
			candlestick_css _ct = {};

			t_map<int, t_vector<glm::vec2>> ma;
		public:
			charts_u1();
			~charts_u1();
			void set_rect(const glm::vec4& r);
			void set_data(glm::vec4* v, int n, glm::ivec4 idx = { 0, 1, 2, 3 });
			void make_ma(int n);
			static bool is_num(const char* s);
		public:

			void update_draw(cpg_time* t, canvas_cx* canvas);
		private:

		};

		//
#define edit_order 100
#define ety_null ((entity)0)

		struct r4color_c
		{
			uint32_t border = 0, fill = 0;// 0xffcccccc;		// 边框、背景色
			float rounding = 6;									// 圆角
			float thickness = 1;								// 线粗
		};

		struct rect_3c {
			shape_n sn = {};
			style_cs  _st = {};
			rect_cs _rt = {};
			void init() {
				sn.t = shape_base::rect;
				sn.n = 1;
				sn.v.r = &_rt;
				_rt.t = &_st;
			}
		};

		class shape_cx
		{
		public:
			t_vector<shape_n*> data;
		public:
			shape_cx();
			~shape_cx();
			rect_cs* add_rect(int n);
			ellipse_cs* add_ellipse(int n);
			circle_cs* add_circle(int n);
			triangle_cs* add_triangle(int n);
			path_cs* add_path(int n);
			image_cs* add_img(int n);
			image_m_cs* add_imgm(int n);
			text_cs* add_text(int n);
		private:

		};
		struct ui_show_t
		{
			ui::base_u* ptr = 0;
			float wait = 0.0f;
			bool v = false;
		};
		// 折叠就隐藏
		struct a_fold_t {
			base_u* ptr = 0;
			base_u* parent = 0;
			glm::vec2 pos;	// 保存
		};
		// 展开
		struct a_open_t {
			base_u* ptr = 0;
		};

		// 动画到目标
		template<class T>
		class vat_t
		{
		public:
			int t = 0;
			float mt = 1;
			// 开始、目标
			T first = {}, target = {};
			// ct当前时间，mt设置时间
			float ct = 0;
			// 返回值：0不执行，1执行结束，2执行中
			int updata_t(float deltaTime, T& dst);
		};

		struct action_t
		{
		public:
			char data[128] = {};
			base_u* ptr = 0;
			char* avs = 0;
			int count = 0;	// 动画数量
			int cap = 0;	// 内存分配大小
			int size = 0;	// 数据大小
			int type = 0;	// 类型，0顺序，1同时执行
			int cidx = 0;	// 当前执行
			float* dst = 0;	// 结果
			int dcap = 0;
		private:
			bool _pause = false;
		public:
			action_t();
			~action_t();
			// 等待n秒后执行
			void add_wait(float st);
			// mt移动所需时间(秒),从坐标first，移动到target
			int add(float mt, glm::vec2 first, glm::vec2 target);
			int add(float mt, float first, float target);
			int add(float mt, glm::vec3 first, glm::vec3 target);
			void play();	// 执行
			void pause();	// 暂停
			// 返回值：0不执行，1执行结束，2执行中
			int updata_t(float deltaTime, float& ct);
			// 分配新内存
			template<class T>
			int add1(float mt, const T& first, const T& target);
			template<class T>
			T* get(int x);
			void clear();
		};

		/* 动画命令
		enum class at_e {
			e_null,
			e_sleep,
			e_move_to,
			e_size_to
		};
		struct action_t
		{
			int cap = 0;		// 分配大小
			int count = 0;		// 动画数量
			char* data = 0;		// 数据
			base_u* ptr = 0;	// 对象，可选
			glm::vec2* pos = 0;	// 修改坐标，可选，与ptr二选一
			action_t* next = 0;	// 顺序执行
			gtac_c* ac = 0;		// 内存分配器
			void add_wait(float t);
			void add_moveto(float t, glm::vec2 to);
			void add_moveto(float t, glm::vec2 from, glm::vec2 to);
		};
		struct sleep_t
		{
			int t = (int)at_e::e_sleep;
			float st = 0.1f;		// 等待秒
		};
		struct move_to
		{
			int t = (int)at_e::e_move_to;
			float st = 1.0f;		// 执行所需时间
			glm::vec2 from, to;		// 从from到to坐标
		};
		struct size_to
		{
			int t = (int)at_e::e_size_to;
			float st = 1.0f;		// 执行所需时间
			glm::vec2 from, to;		// 从from到to大小
		};

		action_t* new_action(gtac_c* ac, ui::base_u* ptr);
		void free_action(action_t* p);
		*/
		class div_u;
		// todo base_u
		class base_u
		{
		public:
			entity e = {};
			entt::registry* reg = 0;
			int id = 0;
			int id2 = 0;
			int pid = 0;
			// 排序
			int order = 0;
			base_u* parent = 0;
			div_u* dparent = 0;
			void* _form = 0;
			// 更新重新排序
			int sort_inc = 0;
			// 样式
			njson styles;
			// 文本渲染用
			//text_dc* tdc = 0;
			gtac_c* ptm = 0;
			text_cs* dtc = 0;
			const char* dstr = 0;
			motion_t* mtt = 0;
			glm::vec2 tpos = { };
			glm::vec2 mask_scroll = { 1.0,1.0 };	// scroll控制
			int fontheight = 16;
			int last_order = 0;
			int mcinc = 0;
			glm::vec2 pos = {};
			hot_et hotd = {};

			//t_vector<shape_n*> psobj;
			//t_set<shape_n*> freeobj;
			// 是否接收拖动事件
			void (*drag_cb)(base_u* ptr) = 0;

			void (*scroll_cb)(base_u* ptr, int wheel, int wheelH) = 0;
			int (*mouse_motion_cb)(base_u* ptr, const glm::ivec2& p, bool ts) = 0;

			void (*hit_cb1)(base_u* p, bool v, void* uptr) = 0;
			// typedef std::vector<std::function<void(void* ptr)>> vcbs_t;
			vcbs_t* ck_cbs = 0;
			struct ucbinfo_t
			{
				uint64_t id;
				void* ud;
				void(*cb)(uint64_t, void*);
			};
			std::vector<ucbinfo_t>* _ucbs = 0;
		private:
			rect_cs _r;
			line_cs _line;
			style_cs _sc = {};
			shape_n _bsn = {};
			glm::vec2 size = {};
		public:
			int hideinc = 0;
			int vinc = 0;
			// 进入离开
			int elinc = 0;
			// 鼠标状态
			mouse_type_e_t _mte = {};
			// 动画，x坐标，y大小
			glm::ivec2 aidx = { -1,-1 };
			void* uptr = 0;
			// 拖动改变坐标，x,y一般等于0或1
			glm::vec2 _drag_pos = {};
			bool has_drag = false;
			// 自动合并文本渲染
			bool auto_drawtext = true;
			// 可见
			bool visible = true;
			bool isdrawble = true;
			bool is_child = false;
			bool is_size = true;
			bool _is_click = false;

			bool is_div = false;
			bool hit = false;
			bool hit1 = true;
			bool isleave = false;
			bool _absolute = false;
			bool has_event = true;
			bool _autofree = false;
			bool isupauto = true;
		public:
			base_u();
			base_u(const glm::vec2& s);

			virtual ~base_u();
			template<class T>
			bool has2c() {
				bool ret = false;
				if (reg && reg->has<T>(e))
				{
					ret = true; ecs_at::set_entity<T>(reg, e, false);
				}
				return ret;
			}

			void add_ucb(uint64_t idx, void* ud, void(*cb)(uint64_t idx, void*));
			// 直线移动，时间秒，目标，原坐标可选
			action_t* move2w(float mt, const glm::vec2& target, glm::vec2* src, float wait);
			// 在原坐标为原点增加移动
			action_t* move2inc(const glm::vec2& pad, float mt, float wait);
			action_t* at_size(const glm::vec2& dst, float mt);
			ui_show_t* wait_show(bool visible, float wait);
			// 基于父级改变大小
			void set_autosize(const glm::ivec4& ps, bool enable);
		public:
			hover_et* get_hover();
			drag_ut* get_drag_ut();
			button_et* get_btnet();
			click_et get_click_et();
			bool is_down(int idx);
			bool is_click(int idx);

			void set_order(int i);
			int get_order();
			int get_root_order();
			// 设置相对坐标或绝对坐标
			void set_position(bool is_absolute);
			// padding1用于根div内边距扩展
			void set_padding1(int v);
			// 设置元素的
			void set_margin1(int v);
			// 设置自己的
			void set_padding(int v);
			void set_margin(int v);
			void set_padding(njson v);
			void set_margin(njson v);
			void set_align(int justify_content, int align_content);
			// row = 0, row_reverse, column, column_reverse
			void set_direction(int d);
			void set_align_items(int align_items);
			virtual void set_pos(const glm::vec2& s);
			virtual void set_size(const glm::vec2& s);
			glm::vec2 get_size();
			void set_visible(bool is);
			bool get_visible();
			glm::vec2 get_pos(bool isparent = false);
			glm::vec2 get_pos1();
			// 获取全局坐标
			glm::vec2 get_pos2(bool isrt = true);
			glm::vec2 get_fpos();	//	窗口坐标
			void* get_form();
			div_u* get_root_div();
			// 设置鼠标
			void set_mouse_cursor(int idx);
			void set_hover_mc(int idx, int pidx);
			// 设置基本形状渲染
			void set_shape(rect_cs* r, style_cs* sc);
			void set_shape(line_cs* r, style_cs* sc);
			//uibs_dt* get_uibs();
			//void push_shape(triangle_cs* t, style_cs* cs);
			text_cs* set_dtc(const char* str, int n, glm::ivec2 box, glm::vec2 ps);
			text_cs* mk_str(const char* str, int n, glm::ivec2 box, glm::vec2 ps, text_cs* t);
		public:
			//void collect_obj(t_vector<shape_n*>* ot);
			hot_et* up_entity();
			virtual void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			virtual void draw_last(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			virtual void update(cpg_time* t);
			virtual void bind_event(entt::registry* r);
			virtual int mouse_motion(const glm::ivec2& p, bool ts);
			bool contains(const glm::ivec2& pos, int* pidx);
			void set_hit(bool is, int idx, const glm::ivec2& npos);
			virtual void set_lt(div_text_info_e t) {}
			virtual void on_parent_visible(bool is) {}
		public:
			void add_mte(mouse_type_e_t e);
			void dec_mte(mouse_type_e_t e);
			bool has_mte(mouse_type_e_t e);
			void call_vcb();

		private:
		};

		struct dcolor_css {
			uint32_t col = 0;
			uint32_t fill = 0;
			uint32_t background_color = 0;
			uint32_t border_color = 0xFF222222;
			uint32_t hover_color = 0xFF888989;
			uint32_t active_border_color = 0xFFff9e40;
			uint32_t font_color = 0xFF222222;
		};
#if 0
		class text_dc
		{
		public:
			// 文本
			t_vector<image_m_cs> strbuf;
			int fontheight = 16;
			text_cs dtext = {};
			// 实现自定义分块
			t_vector<text_cs> vdtext = {};

			glm::vec4 boxsize = {};
			// 用户设置偏移
			glm::vec2 tpos = {};
			uint32_t tcrc = 0;
			float border_scale = 0.5;
			glm::vec2 ps = { 0.5,0.5 };
			shape_n sn = {};

			// 不等于0时分割成n部分
			char splitch = 0;
		public:
			text_dc();
			~text_dc();
			void set_fontheight(int fh);
			void make_text(base_u* p, const char* t, int n, glm::ivec2& box, const glm::vec2& ps, int fontheight);
			size_t make_text(const char* t, int n, int height, div_text_info_e* ctx);
			// 从另一个tdc复制来
			void cp(text_dc* t);
			shape_n* get_tsn(const t_string& str, glm::ivec2 size1, uint32_t textcolor, base_u* p);
		private:

			// 返回box_size、bor、strbuf
			glm::vec4 get_text_img(div_text_info_e* dt, const char* str, int sl, float font_height, glm::ivec2& box, const glm::vec2& ps, uint32_t textcolor, t_vector<image_m_cs>* strbuf);

		};
#endif

		//enum class ec_t :uint32_t
		//{
		//	null0,
		//	img,
		//	img_m,
		//	line,
		//	triangle,
		//	circle,
		//	ellipse,
		//	rect,
		//	path,
		//	text,
		//	max_ec
		//}; 
		struct bcs_t;
		//graphics text图形 文本 分配器
		class gtac_c :public usp_ac
		{
		public:
			glm::vec2 ps = { 0.5, 0.5 };	// 0.5就是居中对齐
			float border_scale = 0.0;
			int fontheight = 16;
			div_text_info_e ctx = {};		// 用于生成图像渲染信息
			// 本类创建的图文对象
			t_set<bcs_t*> _gs;
		public:
			gtac_c();
			~gtac_c();
		public:
			// 创建n个bcs
			bcs_t* new_bcs(int n);
			// 释放一批bcs
			void free_bcs(bcs_t* p);
			// 设置坐标，可以设置同时在n个坐标显示
			void bcs_set_pos(bcs_t* p, glm::vec2* ps, int n);
			void bcs_set_pos(bcs_t* p, glm::vec2 ps);
			// 创建一个bcs，创建n个显示对象。用free_bcs释放或者本视图析构时自动释放
			bcs_t* new_base(shape_base t, size_t n, bcs_t* p = 0);
			// 有多个元素都会创建
			anim_a* set_anim(bcs_t* p);
			// 设置文本
			text_cs* set_text(text_cs* tp, const std::string& str, glm::ivec2 box, style_text_cs* css);
		public:
			text_cs* set_text(text_cs* tp, const std::string& str, int fontheight, glm::ivec2 box, glm::vec2 ps, uint32_t color);
			void set_fontheight(glm::vec2 ps, int fontheight);
			// 生成文本渲染对象，height盒子高度
			text_cs* mk_text(const char* t, int n, glm::ivec2& box, text_cs* o);
			text_cs* mk_text(const char* t, int n, glm::ivec2& box, text_cs* o, style_text_cs* css);
			text_cs* get_text(const std::string& str, text_cs* o, style_text_cs* css);
			// 删除文本
			void remove(text_cs* p);
		public:
			glm::vec4 mk_box(text_cs* ot, glm::ivec2& box, glm::vec2 ps);
		private:
			glm::vec4 get_text_img(const char* str, int n, float font_height, glm::ivec2& box, glm::vec2 ps, uint32_t textcolor, text_cs* ot, familys_t*, float& baseline);
			// 释放资源
			void free_bcsg(bcs_t* p);

		};


		// todo input
		class input_u :public base_u
		{
		public:
			text_view_t _edit[1] = {};
			font_style_c _stc[1] = {};
			std::string _str;
			// 占位符
			std::string placeholder, _tph = (char*)u8"请输入内容";
			std::string _password;		// 密码
			float pha = 0.3;			// placeholder的文本颜色透明度
			dcolor_css dcss = {};
			int _font_height = 16;
			int border = 4;
			int rounding = 4;
			int thickness = 1;
			glm::ivec2 ltpos = {};

			event_master* _emp = 0;
			njson* _td = 0;
			std::string _tk;
			std::function<void(std::string* str, const std::string& old)> on_change;
			std::function<void(std::string* s)> on_input;
			int ick[3] = {};
			int old_order = -1000;
			int incedit = 1;// 渲染计数
			bool _is_number = false;
			bool _is_float = false;
			bool v2 = true;
			bool no_em = true;
			bool _read_only = false;
			bool _no_event = false;
			bool is_ibeam = true;
			bool iseditor = true;
			// 是否更新绑定的json数据
			bool upbd = true;
		public:
			input_u();
			input_u(int font_height, bool single_line);
			~input_u();
			void init(int font_height, bool single_line);
			void set_multi_line(bool is);
			void set_bsize(const glm::ivec2& s);
			// 设置显示的行数
			void set_rowcount(int n, int width, int height);
			void set_number(bool is, bool isfloat);
			void set_password(const char* pwd);
			void set_layout(layout_text* lt);
			void set_input_cursor(bool is);
			void set_read_only(bool is);
			// 是否关闭事件
			void set_event(bool is);
			void set_placeholder(const std::string& str);
			//void set_pos(const glm::vec2& pos);
			void set_color(uint32_t tc, uint32_t sc, uint32_t border, uint32_t background, uint32_t cursor_color);
			void set_text(const std::string& str);
			//void set_text(const t_string& str);
			std::string get_text();
			const char* c_str();
			size_t c_size();
			bool is_input();
			// 绑定数据
			void bind_dt(njson* td, const char* n);
		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
			void on_parent_visible(bool is);

		};


		struct bdata_t
		{
			//  绑定属性指针
			void* v = 0;
			// 所占位位置
			uint16_t b = 0;
			// 1 4 8
			uint8_t size = 1;
			uint8_t count = 1;
			void set_value(bool is);
			bool get_value();



		};

		// 开关
		struct switch_u :public base_u
		{
			// 颜色：x=左，y=右，z=小圆
			glm::ivec3 color = { 0xffff8000,0xff0000ff,0xffffffff };
			// 大小
			//glm::ivec2 size = { 60, 30 };
			// 小圆大小
			float radius = 12;
			// 切换时间
			float gt = 0.3;
			//private:
			// dcr.x输出插值，y保存方向值
			glm::vec2 dcr = {};

			int blur = 0;
			Image* img_blur = 0;
			path_stb* path = 0;
			effect_cx* ec = 0;
			// 自定义图标渲染
			image_m_cs* img2 = 0;
			triangle_cs* tr2 = 0;
			glm::ivec4 tr_mask = { 1,1,1,1 };
			// 临时变量
			glm::ivec3 c = {};
			float cpos = 0;
			// 用户设置
			void (*change_cb1)(switch_u* p, bool v, void* uptr) = 0;
			std::function<void(switch_u* p, bool v, void* uptr)> change_cb = {};
			std::function<void(switch_u* p, bool v)> on_change_cb = {};
			std::function<void(switch_u* p)> click_cb = {};
			//  绑定属性指针
			//bool* b_value = 0;
			bdata_t bv = {};
			bool v2 = true;
			bool ani = false;
			bool value = false;
			bool value2 = false;
		public:
			switch_u();
			~switch_u();
			void set_size(const glm::vec2& s, float r);
			void set_color(glm::ivec3 c);
			void set_time(float t);
			void set_shape(image_m_cs* t);
			void set_shape(triangle_cs* t, glm::ivec4 tr_mask);
			void set_value(bool is);
			void set_value();
			//void bind_value(bool* v);

			void bind_value(void* p, uint16_t set1 = 0, uint8_t size = 1, uint8_t count = 1);
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
		};

		// 滑块
		class slider_u :public base_u
		{
		public:
			float value = 0.0;
			// 颜色：x=背景0xffEDE7E4,，y=覆盖，z=小圆
			uint32_t color[3] = { 0xffF5EEEB,0xffff9e40,0xffffffff };
			// 大小
			//glm::ivec2 size = { 100, 6 };
			// 小圆大小
			float radius = 8;
			// 圆角0-1.0
			float r = 3;
			// 切换时间
			float step = 1.0;
			glm::vec2 range = { 0.0, 1.0 };
			//private:
			int blur = 0;
			Image* img = 0;
			path_stb* path = 0;
			effect_cx* ec = 0;
			glm::vec4 hotv = {};

			float cpos = 0;
			glm::vec2 blurpos = {};
			// 用户设置
			void (*change_cb)(slider_u* p, float v, void* uptr) = 0;
			int dragv = -1;
			bool v2 = true;
		public:
			slider_u();
			~slider_u();
			void set_size(const glm::vec2& s, float r);
			void set_value(float v);
		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);

		};
		class progress_u :public base_u
		{
		public:
			float value = 0.0;
			float* p_value = 0;
			float percentage = 0.0f;		// 进度0-100
			float stroke_width = 6.0f;
			float r = 3;
			uint32_t color[2] = { 0xffF5EEEB, 0xffff9e40 };
			uint32_t dcol[4] = { 0xffFF9E40, 0xff3AC267,0xff3CA2E6, 0xff6C6CF5 };
			float duration = 1;				//	控制动画进度条速度

			float cpos = 0;
			// dcr.x输出插值，y保存方向值
			glm::vec2 dcr = {};
			float apos = 0;
			float dapos = 0;

			bool indeterminate = false;		//	是否为动画进度条	boolean - false

			bool v2 = true;
		public:
			progress_u(float pf = 0.0);
			~progress_u();
			void set_stroke_width(float sw, float width, float rounding = -1.0);
			// 0-100
			void set_percentage(float f);
			// 0-1.0
			void set_value(float v);
			void set_color_idx(int idx);
			void bind_value(float* p);
			void set_color_bc(int v);
		public:

			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
		};



		// todo tag 标签,用于标记和选择。0蓝，1青，2灰，3橙，4红
		enum class uType :uint8_t {
			primary, success, info, warning, danger
		};
		// 填充颜色字白色，边框字颜色中间半透明，边框字颜色
		enum class uTheme :uint8_t {
			dark, light, plain,
		};
		class tag_u :public base_u
		{
		public:
			void* uptr = nullptr;
			// 用户设置
			void (*click_cb)(tag_u* p, void* uptr) = 0;
			void (*close_cb)(tag_u* p, void* uptr) = 0;
			// 是否选中
			void (*change_cb)(tag_u* p, bool v, void* uptr) = 0;

			void (*update_cb)(tag_u* p, void* uptr) = 0;

		public:  /** Tag type */
			uType type = uType::primary;
			/** Tag theme */
			uTheme effect = uTheme::light;
			t_string label;
			uint32_t border_color = 0;
			uint32_t fill_color = 0;
			// 颜色为0时用type
			uint32_t color = 0;
			uint32_t dcol[5] = { 0xffFF9E40, 0xff3AC267,0xff999390,0xff3CA2E6,0xff6C6CF5 };
			int rounding = 0;
			// 高度

			float height = 32;
			float light = 0.12;
			glm::vec2 sps = { 0.5, 0.5 };
			glm::vec2 ssize1 = { 0, 0 };
			// 布局偏移
			glm::ivec2 mflex[3] = {};

			bool checked = false;	//是否选中
			bool isborder = false;
			/** 是否可关闭Whether Tab can be removed */
			bool closable = false;

			/** 是否禁用渐变动画Whether the removal animation is disabled */
			bool disableTransitions = false;

			/** 是否有边框描边Whether Tag has a highlighted border */
			//bool hit = false;
			bool isupstr = true;
		public:
			tag_u(const char* str = 0);
			~tag_u();
			void set_label(const char* str);
			void set_label(const std::string& str);
		public:

			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
		private:

		};

		// todo 按钮
		struct btn_css_t {
			uint32_t font_color = 0xFF222222;
			uint32_t background_color = 0xFFffffff;
			uint32_t border_color = 0xFF222222;
			uint32_t hover_color = 0xFFf78989;
			uint32_t active_font_color = 0xFFe6e6e6;
			uint32_t active_background_color = 0xFFf2f2f2;
			uint32_t active_border_color = 0;// 0xFF3c3c3c;
			uint32_t hover_border_color = 0;
		};
		enum class btn_state_e {
			normal,//（正常态）：也被称为激活态，表示按钮可正常点击和使用的状态，也是按钮里最常见的状态之一
			hover,//（鼠标滑过）：表示鼠标停留在这个按钮上但尚未点击的状态
			click,//（鼠标点击）
			disable,//（禁用态）：表示这个按钮不能点击，表现样式上一般是置灰，或者在normal的基础上降低不透明度
			maxbs
		};
		class button_u :public base_u
		{
		public:
			button_u* pnext = 0;
			void* ud1 = 0;
			void (*click_cb0)(button_u* p, void* uptr) = 0;
			std::function<void(button_u* p, void* uptr)> click_cb1;
			std::function<void(button_u* p)> click_cb;
			std::function<void(button_u* p, cpg_time* t)> down_cb;
			//std::function<void(button_u* p)> btn_up_cb;
			// 鼠标进入
			std::function<void(button_u* p, void* uptr)> enter_cb;
			std::function<void(button_u* p, void* uptr)> leave_cb;

			btn_css_t bdc = {};
			btn_css_t* _defbdc = 0;
			image_m_cs _icon = {};
			t_string _label;
			t_string name;
			// 按钮类型，0默认，1渐变，2图片
			int type = 0;
			// 图片按钮
			image_cs* imginfo = 0;
			int iidx = 0;
			// 渐变按钮样式
			btn_info_u* _nbtn = 0;
			// 勾选效果
			check_mark_cs* _ck_icon = {};
			triangle_cs* _icon_r = {};
			// 圆角0-1；
			float rounding = 0.1;
			int roundingi = 0;
			float dround = 0;
			float light = 0.12;
			float active_a = 0.12;
			float thickness = 1.0;
			//uint32_t afill = 0;
			uint32_t dfill = 0, dcol = 0;
			uint32_t text_color = 0;
			uTheme effect = uTheme::light;
			float width = 0;
			float height = 40;
			// 水平垂直位置
			glm::vec2 posf = { 0.5,0.5 };
			glm::ivec2 posi = { };
			// 
			//text_dc* tdc = 0;
			text_cs dtcs = {};
			anim_a* rotate_text = 0;
			void* user_ptr = 0;
			// 是否按下时偏移文本
			int down_offset = 1;

			int disabled_alpha = 0x30;
			// 是否为圆形按钮
			bool _is_circle = false;
			// 鼠标进入时切换手形鼠标
			bool _is_hand = true;
			// 是否有边框
			bool borderable = true;
			bool fillable = true;
			// 是否显示标签文本
			bool _is_text = true;
			// 内部更新文本之类用
			bool v2 = true;
			bool cptext = false;
			bool uhover = false;
			bool _disabled = false;				// 是否禁用

		public:
			button_u(const std::string& str = "");
			~button_u();

			void set_label(const std::string& str);
			void set_label(const void* str);
			void set_color_idx(int idx);
			void set_color(btn_css_t* c);
			void set_color(const btn_css_t& c);
			void set_round(float r, bool is_circle = false);
			void set_rotate_text(anim_a* rot);
			btn_css_t* get_color();
			// 复制文本数据显示
			//void set_cp_text(text_dc* t);
			void set_height(int h);
			// 默认风格以外还有两种，图片、渐变
			// 设置btn_state_e状态idx、图片、区域、大小(可以和按钮不同)、九宫格渲染 left top right bottom
			void set_image(btn_state_e idx, Image* img, glm::ivec4 rect, glm::vec2 size = {}, glm::vec4 sliced = {});
			// 修改绑定图片到状态
			void bind_image(btn_state_e idx, Image* img);
			// 设置渐变样式按钮
			void set_gradient(unsigned int back_color = 0, unsigned int text_color = -1, int rounding = 2, icon_rt* icon = 0);
		public:

			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
		public:
			static btn_css_t defcol(int idx);
		private:


		};
		// todo checkbox
		class checkbox_u :public base_u
		{
		public:
			// 用户设置
			std::function<void(checkbox_u* p, bool v, void* uptr)> change_cb = {};
			std::function<void(checkbox_u* p)> click_cb = {};
			t_string name;
			// 显示标签
			t_string label;
			t_string label2;

			text_cs* str2 = 0;
			float height = 40;
			uint32_t col = 0xffFF9E40, fill = -1, col0 = 0x88666666, textcol = 0xff666666;
			uint32_t check_col = -1;
			float square_sz = 14;
			float rounding = 2;
			check_mark_cs ck = {};

			float maxborder = 0.4;
			// 间隔
			float stp = 10;
			// 动画控制
			float ts = 5;
			glm::vec2 dcr = {};
			//  绑定属性指针
			//bool* b_value = 0;
			bdata_t bv = {};
			//// 自定义渲染效果
			//shape_n* usp = 0;
			//rect_3c rt1 = {};
			// 水平垂直位置
			glm::vec2 posf = { 0.0,0.5 };
			glm::ivec2 posi = { };
			float cktime2 = 0.25f, ckt2 = 0.0f;

			// 鼠标进入时切换手形鼠标
			bool _is_hand = true;
			bool ani = true;
			bool only_text = false;
			bool add_tx = false;
			bool value = false;		// checked
			bool value2 = false;		// checked
			bool v2 = true;
		public:
			checkbox_u();
			~checkbox_u();
			// 设置显示混合
			void set_mixed(bool v);
			void set_height(int h);
			bool get_value();
			void set_value(bool v);

			// 绑定位, 字节数size=[1,4,8]，位设置set1默认为0则会设置data成0/1
			void bind_value(void* p, uint16_t set1 = 0, uint8_t size = 1, uint8_t count = 1);
			void set_value();
			void set_label(const std::string& str);
			// 设置两种状态不同文本
			void set_label2(const std::string& str, const std::string& str0);
		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
			void upvalue();
		private:

		};
		class radio_u;
		class group_radio_u
		{
		public:
			t_string name;
			radio_u* first = 0, * active = 0;
			// 保存单选组名
			t_set<radio_u*> srp;
		public:
			group_radio_u();
			~group_radio_u();
			void push(radio_u* p);
		private:

		};



		class radio_u :public base_u
		{
		public:
			// 用户设置

			void (*change_cb1)(radio_u* p, bool v, void* uptr) = 0;
			std::function<void(radio_u* p, bool v, void* uptr)> change_cb = {};
			std::function<void(radio_u* p)> click_cb = {};
			t_string name;
			// 显示标签
			t_string label;
			t_string label2;
			text_cs* str2 = 0;
			group_radio_u* gr = 0;
			float height = 40;
			uint32_t col = 0xffFF9E40, fill = -1, col0 = 0x88666666, textcol = 0xff666666;
			float radius = 7;
			float swidth = 1;
			float maxborder = 0.4;
			// 叠加透明度
			float apf = 0.0;
			glm::vec2 cpos = {};
			// 间隔
			float stp = 10;
			// 动画控制
			float ts = 5;
			glm::vec2 dcr = { 1 , 0 };
			// 自定义渲染效果
			image_cs* uimg = 0;
			int ucount = 0;
			uint32_t ucol = -1;
			uint32_t hit_col = -1;
			float hita = 0.2;
			//private:
			style_cs  _stc[2] = {};
			circle_cs _cir[2] = {};
			// 水平垂直位置
			glm::vec2 posf = { 0.0,0.5 };
			glm::ivec2 posi = { };
			float cktime2 = 0.25f, ckt2 = 0.0f;
			//  绑定属性指针
			//bool* b_value = 0;

			bdata_t bv = {};
			// 鼠标进入时切换手形鼠标
			bool _is_hand = true;
			// 文本颜色不变
			bool static_tc = false;
			bool ani = true;
			bool only_text = false;
			bool add_tx = false;
			bool value = false;
			bool value2 = false;
			bool v2 = true;
		public:
			radio_u(group_radio_u* g = nullptr);
			~radio_u();
			static radio_u* new1();
			group_radio_u* new_g();
			// 对组
			bool get_value() { return value; }
			void set_value();
			void set_value(bool v);

			void bind_value(void* p, uint16_t set1 = 0, uint8_t size = 1, uint8_t count = 1);
			void set_label(const std::string& str);
			// 设置两种状态不同文本
			void set_label2(const std::string& str, const std::string& str0);
			void set_g(group_radio_u* g);
			group_radio_u* get_g();
		public:

			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
			void upvalue();
		};


		struct div_css0_u
		{
			// 顺序
			int order = 0;
			// 位置，默认0相对位置RELATIVE
			int position = 0;
			// 大小
			glm::vec2 size;
			// 绝对坐标计算
			glm::vec4 rect = { -1,-1,-1,-1 };
			// 内边距
			glm::vec4 padding = {};
			// 外边距
			glm::vec4 margin = {};
			// 对齐
			glm::vec4 align = { 3,1,1,0 };
			//(justify_content, flex_align_e, FLEX_ALIGN_START)
			//(align_content, flex_align_e, FLEX_ALIGN_STRETCH)
			//(align_items, flex_align_e, FLEX_ALIGN_STRETCH)
			//(align_self, flex_align_e, FLEX_ALIGN_AUTO)

			// 方向	 ROW = 0, ROW_REVERSE, COLUMN, COLUMN_REVERSE
			int direction = 0;
			// flex-wrap 属性规定flex容器是单行或者多行，同时横轴的方向决定了新行堆叠的方向。。
			// NO_WRAP = 0, WRAP=1, WRAP_REVERSE=2
			int wrap = 1;
			// 扩展比率
			float grow = 0.0;
			// 收缩规则, shrink1.0缩放
			float shrink = 0.0;
			// (单位像素)，伸缩基准值，设定一个长度或者自动填充>=0
			float basis = NAN;
			void* managed_ptr = NULL;
		};






		class scroll_u;
		// view
		class view_u
		{
		public:

			entt::registry* reg = 0;
			glm::vec2 _pos;
			//glm::vec2 _size;
			glm::ivec4 clip;							// 裁剪区域
			// 样式
			njson styles;
			// 更新重新排序
			int sort_inc = 0;
			// 内存分配器
			usp_ac* ac = 0;
			gtac_c* ptm = 0;

			//t_vector<base_u*> child;
			//// 添加/删除控件
			//std::queue<base_u*> tadd, tdel;
			//int64_t adinc = 0;

			asyn_queue_x<ui::base_u*>* _child = 0;

			t_map<int, t_vector<base_u*>> v;
			t_map<int, glm::vec4>* _layer = 0;
			// 行数
			int row_num = 0;
			// 列数
			int col_num = 0;
			scroll_u* _horizontal = 0, * _vertical = 0;	// 水平 / 垂直滚动条控件
			int scroll_width = 20;						// 滚动条宽度
			float scroll_scale = 0.6;
			glm::ivec2 scroll_pos = { };
			glm::ivec2 scroll_step = { 1,1 };
			std::function<void(glm::vec2 ps, int idx)> on_scroll_cb;
			base_u* _last = 0;
			// 滚动条坐标
			glm::vec4 bpos;
			int scroll_count = 0;
			// 滚动使用整行
			bool scroll_int = true;
			// 是否有滚动条
			bool _has_scroll = true;
			bool _only_e = false;
		public:
			view_u();
			~view_u();
		public:
			void set_dlt(div_text_info_e dt);
			// 设置坐标、大小
			//void set_rc(const glm::vec4& rc);
			void set_bpos(const glm::vec4& pos);

			// 表格。
			void set_num(int row, int col);
			/*
				row = 0,
				row_reverse=1,
				column=2,
				column_reverse=3
				设置方向
			*/
			void set_direction(int t);
			void set_row_column(bool row);
			// 设置相对坐标或绝对坐标
			void set_position(bool is_absolute);
			// padding1用于根div内边距扩展
			void set_padding1(int v);
			void set_align(int justify_content, int align_content, bool isrow);
			void set_align_items(int align_items);

			base_u* push(base_u* p, int idx);
			void pop(base_u* p);
			void clear();
			// 移到最后显示
			void move2last(base_u* p);
			// 子元素数量
			size_t count();
			void set_scroll_step(int h, int v);
			void set_scroll_color();

			void set_scroll_count(int c);

			void ndrag(drag_ut* dp);
		public:
			virtual void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			virtual void draw_last(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t1);
			int mouse_motion(const glm::ivec2& p, bool ts);
			void on_scroll(int wheel, int wheelH);
			// 更新滚动大小
			void up_scroll(cpg_time* t, glm::vec2 ws, int padd, int item_height, int n, glm::ivec4& clip, int& nheight, int& max_content_width);
		private:

		};

		// div容器
		class div_u :public base_u
		{
		public:
			image_cs _backimg = {};
			event_data_cx* _listen = 0;
			layout_text* lt = 0;
			item_box_info* pdt = 0;
			font_style_c* ust = 0;
			sdl_form* form = 0;

			// 添加/删除控件
			asyn_queue_x<ui::base_u*>* _ui = 0;
			std::vector<ui::base_u*> _dv;
			std::set<Image*> _gcimg;
			//text_dc* tdc = 0;
			gtac_c* tm = 0;
			t_string name;
			// 文本颜色
			uint32_t utc = -1;
			float tpx = 0;

			glm::vec2 umx = {};
			float u_padding1 = 3;
			void (*click_cb)(const char* str, int idx, void* uptr) = 0;
			glm::vec2 blurpos = {};
			check_mark_cs* _ckm = 0;
			njson* _ud = 0;								// 用户数据
			void (*_udfreecb)(njson* p) = 0;
			glm::ivec4 clip;							// 裁剪区域
			// 裁剪
			bool _has_clip = true;
			bool _auto_left = false;

		private:
			r4color_c r4c = {};			// 基本矩形颜色圆角
			int vic = 0;
			// listen全局坐标
			glm::vec2 rpos = {};
			image_cs _blur = {};
			Image* _img = 0;

			bool isnew = false;
		public:
			div_u();
			// 复制另一个div的字体样式
			div_u(div_u* p);
			~div_u();
			// root div需要new
			void set_root();
			bool is_root();
			// 占用内存大小
			int memsize();
			// 设置窗口事件管理器，传nullptr解除窗口绑定
			void set_event_master(sdl_form* p, bool front = false);

			void set_cp2(div_u* p);
			// 是否有事件
			void set_has_event(bool is);
			ui::base_u* get_idxc(int idx);
		public:
			float get_thickness();
			// 边框、背景色、圆角
			void set_bf_color(uint32_t border, uint32_t fill, float rounding, float thickness);
			glm::vec2 get_pos();
			void set_pos(const glm::vec2& pos);
			void set_dragable(bool is_dragable);
			bool get_dragable();
			void set_size(const glm::vec2& s);
			// 设置字体集合
			void set_font_family(hz::tt_info** font_family, int count);
			void set_font_family(layout_text* p);
			// 设置边框、底纹颜色，shadow
			void set_color_bs(glm::ivec2 color, box_shadow_t* bs);
			base_u* at(size_t idx);
			base_u* push(base_u* p);
			void pop(base_u* p);
			// 添加image自动析构
			void push(Image* p);
			void pop(Image* p);
			void clear();
			// 子元素数量
			size_t count();

			void mk_blur(const glm::ivec2& rc, int r, int blur, uint32_t color);
			div_text_info_e get_lts();
		public:

		public:
			virtual void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t1);

			int mouse_motion(const glm::ivec2& p, bool ts);
			virtual void set_lt(div_text_info_e t);
			void on_parent_visible(bool is);
			void update_ecs(cpg_time* t);
		private:
			//void collect(entity e);
		};

		// todo 滚动条vertocal/horizontal
		class scroll_u :public base_u
		{
		public:
			glm::vec2* bv2 = 0;			// 绑定输出滚动量
			int _content_size = 0;		// 内容大小
			int _view_size = 0;			// 视图大小
			int dcv = 0;				// content-view
			float _offset = 0;			// 滚动偏移
			float _offset_f = 0;		// 百分比偏移
			float _step = 1;			// 每次滚动量
			int _thumb = 0;				// 滑块大小
			int _thumb_idx = 1;			// 滑块id
			int _btn_width = 0;			// 两端箭头按钮宽度、高度
			int _rc_width = 0;
			int _dir = 1;				// 1是垂直，0是水平
			glm::ivec2 first_pos;		// 第一次拖动记录
			glm::ivec3 thumb_size_m;	// 滑块大小范围

			size_t scroll_count = 2;	// 滚动数量
			double  down_ms = 0.08;		// 100ms触发一次按下
			double  cudown_ms = 0.0;	// 当前触发记录
			int  down_idx = -1;			// 按下时的idx

			glm::vec4 hot[3] = {};		// *按钮热区
			glm::ivec2 first_down;		// 第一次按下坐标

			// 图形
			btn_css_t	_mbc = {};

			style_cs	_stc[4] = {};
			rect_cs		_r2[2] = {};
			triangle_cs _btn[2] = {};

			std::function<void(scroll_u* p)> change_cb = {};

			// bool
			bool _is_down = true;		// 当前状态是向下滚动还是向上
			bool has_arrow_btn = false;	// 按钮数量，1/3
			bool isup_listen = true;


		public:
			scroll_u();
			~scroll_u();
			// 设置方向  1是垂直，0是水平
			void set_dir(int dir);
			void set_color(uint32_t col, uint32_t hover, uint32_t back);
		public:

			void set_step(int s);
			// 滚动条长度，内容长度，滚动条宽度，
			int set_view_content(int view, int content, int width, float scale);
			void set_arrow_btn(bool is);
			// -=向上滚动n个单位_step，-1到顶
			bool set_up(int n);
			// +=向下滚动
			bool set_down(int n);

			// 修改内容时更新
			void update_content(bool is);

			// 触发按下
			void on_down();
			// 触发持续按下
			bool on_keep_down(cpg_time* t, const glm::vec2& pos);
			// 开始拖动
			void on_dragstart(const glm::vec2& pos);
			// 拖动改变坐标时
			void on_drag(const glm::vec2& pos);
			// 判断是否重置内容大小
			bool is_resize_content(int w);

			void set_thumb_pos(float pos);		// 百分比
			void set_thumb_posi(int pos);		// 像素
		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
			static void on_drag_cb(base_u* ptr);
		private:
			void up_pos(float ps, bool is);
		};

		struct item_c
		{
			button_u* btn = {};
			t_string text = {};
			void* ptr = 0;
		};
		// todo ComboBox
		class combobox_u :public input_u
		{
		public:
			std::function<void(int idx, const char* str)> on_change;

			// 列表
			//div_u* lst = 0;
			t_vector<item_c> items;
			t_vector<t_string> tstr;
			t_set<int> delitems;
			// 选中项
			t_set<int> _its;
			//menu_page_t* mpage = 0;
			njson style;
			check_mark_cs _ck_icon = {};
			int itemy = 0;
			int fontheight = 16;
			uint32_t text_color = 0xff222222;
			uint32_t text_color_lst = 0xffeeeeee;
			uint32_t blur_color = 0xff111111;
			glm::vec2 blur_pos = {};
			int _blurs = 16;
			int _idx = -1;
			int cidx = -1;
			int isw = 0;//开关
			anim_a* _icon_ra = 0;
			// 向下方向字符
			t_string rightch = (char*)"\xef\x85\x88";//(char*)u8"\xf148";
			t_string spstr = (char*)u8", ";
			//text_dc* tdc0 = 0;
			button_u* rcbtn = 0;
			//sdl_form* _pform = 0;
			//layout_text* _plt = 0;
			// 弹出坐标
			glm::vec2 vpos;
			// 是否可编辑
			bool is_edit = false;
			bool vp = true;
			bool uck = false;
			// 多单选
			bool _multi = false;
			bool haspage = false;
			bool mz = false;
			bool ispop = false;
		public:
			combobox_u();
			~combobox_u();
			// 0 button，1 input
			void set_type(int t, bool is_read);
			//void set_placeholder(const std::string& str);
			void set_placeholder(const void* str);
			//void set_placeholder(const char8_t* str);
			void set_isedit(bool is);
			void set_multi(bool is);
			void set_fontheight(int h);
			void set_value(int idx);
			//void set_cp2(menu_page_t* page);
			// 设置列表
			void add(const char* str, int len = -1);
			void add(const void* str, int len = -1);
			// 删除一项
			void remove(int idx);
			// 清空列表
			void clear();
			button_u* get_idx(int idx);
			item_c* get_it(int idx);
			// -1则是全局
			void set_icon(image_m_cs* p, int idx);
			// 设置窗口，下拉框字体集
			//void set_ctx(sdl_form* pform, layout_text* plt);
		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
		private:

		};

		// 菜单item
		class menu_item_u :public base_u
		{
		public:
			// 可选
			image_m_cs* icon = 0;
			// 文本
			t_string text;
			// 可选 快捷键
			t_string shortcut;
			bool checked = false;
		public:
			menu_item_u();
			~menu_item_u();
		public:

			void update(cpg_time* t1);
		private:

		};



		// todo tree_view_u
		struct tree_node_t
		{
			int level = 0;						// 层级，自动计算
			int idx = 0;						// 全局index，auto
			t_string title;						// 显示的标题文本
			void* raw = 0;						// 自定义数据，比如用于显示图标啥的
			tree_node_t* parent = 0;			// 父级
			t_vector<tree_node_t*>* child = 0;	// 孩子
			int64_t ck_inc = 0;					// 选中数量

			// 第一、最后位置
			t_list<tree_node_t*>::iterator it0;
			size_t amount = 0;					// 所有展开的数量
			bool _expand = false;				// 是否展开
			bool is_checked = false;			// 是否选中
			bool _full = false;
		};


		// tree_view_u::item_t
		class tree_view_u :public base_u
		{
		public:
			struct item_t
			{
				item_t* prev = 0;				// 上一个
				item_t* next = 0;				// 下一个
				tree_node_t* np = 0;
				rect_cs* _ico_hover = 0;		// 鼠标停留
				rect_cs* _ico_check = 0;		// 选中
				switch_u* _expand_c = 0;
				checkbox_u* _cbu = 0;
				text_cs* dtext = 0;
				void* ptr = 0;
				glm::vec2 pos = {}, size = {};
			};
			gtac_c tm;
			view_u* _view = 0;							// 管理控件事件
			tree_node_t* _node = 0;						// 根节点
			tree_node_t* at_node = 0;					// 点中节点
			t_list<tree_node_t*> _show_node;

			size_t show_idx = 0, last_idx = 0;			// 节点偏移
			size_t node_count = 0;						// 节点数量

			item_t* it0 = 0;
			size_t nc_cap = 0;							// 分配的节点数量
			t_vector<glm::vec4> _hotv;					// 节点事件热区

			item_t* first_it = 0, * last_it = 0;
			glm::ivec4 content;							// 内容区域
			glm::ivec4 clip;							// 裁剪区域
			uint32_t text_color = 0xffffffff;
			glm::ivec2 rc_col = { 0x80121212, 0x801f1f1f };

			int fontheight = 16;						// 字高
			int item_height = 20;
			double level_tab = 1.0;						// 缩进比例
			int padd = 3;								// 内容偏移


			int nheight = 0;
			int max_content_width = 0;
			int cmc_width = 0;


			// 选中项
			t_set<int> _its;
			t_list<tree_node_t*>::iterator first_btn, last_btn;
			int pinc = 0;
			int iy = 0;
			// 三角形样式
			style_cs tscs[2] = {};
			style_cs hot_tscs[2] = {};
			style_cs r4s[2] = {};

			t_map<tree_node_t*, item_t*> _tmap;			// 缓存
			rect_cs hot_act[2] = {};
			uint32_t hot_color = 0xffFF9E40;
			int cidx = -1;
			//相邻级节点间的水平缩进，单位为像素
			int _indent = 16;
			int mxhot = 0;
			int scroll_width = 20;
			std::function<void(const char* str, int idx, tree_node_t*)> on_click_item;
			std::function<void(cpg_time* t)> update_cb;
			// 使用箭头图标展开
			bool _expand = true;
			// 是否有复选框
			bool _check = false;
			bool _single_check = true;
			bool cup_node = true;
			bool vp = true;
		public:
			tree_view_u();
			~tree_view_u();
		public:
			size_t count();
			tree_node_t* insert(const t_string& str, void* raw, tree_node_t* parent);
			tree_node_t* insert(const char* str, void* raw, tree_node_t* parent);
			tree_node_t* insert(tree_node_t* np, tree_node_t* parent);
			// 从父级除移节点
			void remove(tree_node_t* p, bool is_free);
			// 释放节点包括所有孩子
			void free_node(tree_node_t* p);
			void clear();
			void set_idx(int idx);

			void up_show();
			void add(const char* str, int len, void* ptr);
#ifndef _NO_PMR_
			void add(const char8_t* str, int len, void* ptr);
#endif
			void remove(size_t idx);
			void set_color(uint32_t text_col, uint32_t border_col, uint32_t back_col);
		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
		private:
			// 更新显示节点
			void up_node();
			void up_shownode(size_t n);
			// 生成渲染项
			void make_item(item_t* it, tree_node_t* np, int idx);
			void newshowit();
			void seticonhot(item_t* p, hover_et* hp);
			void set_hot_node(int idx);
			void set_act_node(int idx, bool ck);
			// 展开显示中的item
			void expand_node(tree_node_t* p);
			size_t calc_expand(tree_node_t* p);
			void on_check();

			void on_vertical(float y);
			int mouse_motion(const glm::ivec2& p, bool ts);
			virtual void set_lt(div_text_info_e t);
		};




		// todo menubar菜单栏
		class menu_bar_u :public div_u
		{
		public:
			struct item_t
			{
				t_string t;
				button_u* btn = 0;
				menu_page_t* p = 0;
			};
			std::function<void(int idx, menu_bar_u* p)> click_cb = {};
			t_vector<item_t> vmenu;
			button_u* hot_btn = 0;
			int vnum = 0;
			int height = 30;
			int fontheight = 16;
			uint32_t text_color = -1;
			njson styles;
		public:
			menu_bar_u();
			~menu_bar_u();
			void set_height(int h);
			// 标题、下拉菜单
			void add(const char* t, menu_page_t* p);
#ifdef _HAS_CXX20
			void add(const char8_t* t, menu_page_t* p);
#endif
			// add 时没绑定则可以再设置
			void set_idx(size_t idx, menu_page_t* p);
			size_t it_count();
			void del_idx(size_t idx);
			// -1全部修改
			/*
				//uint32_t font_color = 0x01;
				//uint32_t background_color = 0x02;
				//uint32_t border_color = 0x04;
				//uint32_t hover_color = 0x08;
				//uint32_t active_font_color = 0x10;
				//uint32_t active_background_color = 0x20;
				//uint32_t active_border_color = 0x40;
				//uint32_t hover_border_color = 0x80;

			*/
			void set_color(btn_css_t* c, uint32_t mask);
			menu_page_t* get_page(size_t idx);
			menu_item_t* get(size_t idx);
		private:

		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
		};


		glm::vec2 get_imgsize0(image_cs* p);

		struct bcs_t
		{
			union ptr
			{
				void* p0;
				image_cs* img;
				image_m_cs* im;
				line_cs* l;
				triangle_cs* t;
				circle_cs* c;
				ellipse_cs* e;
				rect_cs* r;
				path_cs* p;
				text_cs* tp;
				grid2d_cs* g;
				check_mark_cs* ck;
			}p;
			int bn = 0;					// 根数组大小
			int n = 0;					// 数量
			style_cs* ct = 0;			// 颜色风格
			shape_base type = shape_base::shape_null;	// 类型
			int dv = 0;					// 层
			glm::vec2 mask_scroll = { 1.0,1.0 };	// scroll控制
			glm::vec2 pos0 = { 0.0,0.0 };	// 基础坐标
			glm::vec2* pos = 0;			// 多坐标显示
			int psn = 0;				// 坐标数量，动画数量
			anim_a* a = 0;				// 动画支持  
			bool visible = true;
			bool hasdv = false;			// 是否已经插入层
		public:
			void set_pos(const glm::vec2& ps1);
			void set_poss(glm::vec2* ps0, int n);
			void set_posi(glm::vec2 ps, int idx, bool lt);
			glm::vec2 get_size();
			glm::vec2 get_sizes(std::function<void(size_t idx, glm::vec2 ss)> cb);
			glm::vec2 get_pos(int idx);
			glm::vec2 get_size(int idx);
			void set_visible(bool is);
			// 返回0则没命中，大于0则
			int hit_test(const glm::vec2& ps);
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& cpos, const glm::vec2& cp);
		};
		typedef std::function<void(glm::vec2 ps, int idx)> tf_scroll_cb;
		typedef std::function<void(void* uptr, glm::ivec2 pos, click_et ck)> tf_click_cb;

		/*
			滚动视图
			内部布局由用户处理
		*/
		class scroll_view_u :public base_u
		{
		public:

			gtac_c tm;
			view_u* _view = 0;							// 管理控件事件
			// 事件

			std::function<void(void* uptr, glm::ivec2 pos, click_et ck)> _click_cb;
			// 更新动画
			std::function<void(cpg_time* t)> update_cb;
			// 滚动事件
			t_vector<tf_scroll_cb> on_scroll_cb;
			struct ckinfo_t {
				glm::ivec4 r;
				void* ptr;
				uint32_t* st;		// 鼠标状态 mouse_type_e
				tf_click_cb cb;
			};
			t_vector<ckinfo_t> on_click_cb;
			t_vector<event_data_cx*> _on_ed;
			glm::ivec4 content;							// 内容区域
			glm::ivec4 clip;							// 裁剪区域

			t_map<int, t_vector<bcs_t*>> _user_g;
			t_map<int, glm::vec4> _layer;
			// 本视图创建的对象
			//t_set<bcs_t*> _gs;
			Lock _lkbcs;
			std::queue<bcs_t*> _fbcs;
			sem_s* _sem_b = 0;

			uint32_t text_color = 0xffffffff;
			glm::ivec2 rc_col = { 0x80121212, 0x801f1f1f };
			float rounding = 0.0f;
			int fontheight = 16;						// 字高
			int item_height = 20;						// 行高
			double level_tab = 1.0;						// 缩进比例
			int padd = 3;								// 内容偏移
			int scroll_width = 20;						// 滚动条宽度
			float scroll_scale = 0.6;

			glm::ivec2 scroll_pos = { };
			glm::ivec2 _cs = {};

			glm::ivec2 first_down;		// 第一次按下坐标

			effect_cx* efc = 0;
			std::atomic_int refc = 0;
			// 滚动使用整行
			bool scroll_int = true;
			//// 是否有滚动条
			//bool _has_scroll = true;
			bool cup_node = true;
			bool vp = true;
			// 开启拖动视图
			bool drag_view = false;
		public:
			scroll_view_u();
			~scroll_view_u();
			// 增加一个滚动监听回调函数
			int add_scroll_cb(tf_scroll_cb cb);
			int add_event_cb(void* ptr, glm::ivec4 r, uint32_t* st, tf_click_cb cb);
			int add_event(event_data_cx* p, void* ptr = 0);

			// 设置滚动条初始坐标
			void set_bpos(const glm::vec4& pos);
			void set_show_scroll(bool is);
			void set_size(const glm::vec2& size);
			void set_drag_view(bool is);
		public:
#if 0
			// 添加文本，layer层数字低的显示在后面
			bcs_t* push(text_cs* p, int layer = 0);
			// 添加文本显示，多个坐标显示
			bcs_t* push(text_cs* p, glm::vec2* pos, int n, int layer = 0);

			// 添加矢量图
			bcs_t* push(rc_t rc, int layer = 0);			// 矩形批量
			bcs_t* push(rect_cs*, int n, int layer = 0);	// 矩形
			bcs_t* push(triangle_cs*, int n, int dv = 0);	// 三角形（基于矩形内	）		 
			bcs_t* push(line_cs*, int n, int layer = 0);	// 线段
			bcs_t* push(circle_cs*, int n, int layer = 0);	// 圆
			bcs_t* push(ellipse_cs*, int n, int layer = 0);	// 椭圆

			//bcs_t* push(image_cs1*, int n, int layer = 0);	// 图片

			bcs_t* push(image_cs*, int n, int layer = 0);	// 图片(区域九宫格)
			bcs_t* push(image_m_cs*, int n, int layer = 0);	// 图片(区域)
			bcs_t* push(path_cs* path, int layer = 0);		// 路径
#endif
		public:
			// 设置层裁剪区域
			void set_layer_clip(int layer, const glm::vec4 cp);
			// 添加控件，在图元层上方
			void push(base_u* p, int layer = 0);
			void pop(base_u* p);
			// 绑定图元到不同层
			void push(bcs_t* p, int layer);
			void pop(bcs_t* p);
		public:
			// 引用、删除引用
			void add_ref();
			void del_ref();
			// 设置行数
			void set_row_count(int n);
			// 设置行高
			void set_row_height(int h, int idx);
			void set_min_size(const glm::vec2& s);
			void set_max_size(const glm::vec2& s);

			void set_color(uint32_t text_col, uint32_t border_col, uint32_t back_col);
			gtac_c* get_mktext();
			gtac_c* get_tm();
			glm::ivec2 get_content_size();
			void set_content_size(glm::ivec2 cs);
			void set_scroll_step(int h, int v, int count = 1);
			// 设置滚动条位置，idx=0水平，1垂直
			void set_scroll_n(int n, bool isup, int idx);
		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);
			void update(cpg_time* t);
			int mouse_motion(const glm::ivec2& p, bool ts);
			virtual void set_lt(div_text_info_e t);
		private:

			// 更新显示节点
			void up_node();
			void up_shownode(size_t n);
			void up_scroll(cpg_time* t);

			void set_hot_node(int idx);
			void set_act_node(int idx, bool ck);
			void on_check();
			void on_horizontal(float x);
			void on_vertical(float y);

			// 开始拖动
			void on_dragstart(const glm::vec2& pos);
			// 拖动改变坐标时
			void on_drag(const glm::vec2& pos);
			static void on_drag_cb(base_u* ptr);
		};


		// 图元控件组
		struct widget_s
		{
			// 图形g包含有数量
			bcs_t* g = 0;
			// 控件数组
			ui::base_u** p = 0;
			// 控件数量
			int n = 0;
		};


		// 图表
		class charts_u :public base_u
		{
		public:
			struct line_st {
				unsigned int col = -1;
				float thickness = 1.0;
				bool anti_aliased = true;
			};

			t_map<int, t_vector<glm::vec2>> _polylines;
			t_map<int, line_st> _line_st;
			glm::ivec4 rc_col = { 0x80121212, 0x801f1f1f , 4, 1 };
		public:
			charts_u();
			~charts_u();
			// 设置数据
			void set_lines(int k, glm::vec2* p, int n);
			void set_st(int k, line_st st);
			// 添加轴
			void add_axis();
		public:
			void draw(cpg_time* t, canvas_cx* canvas, const glm::vec2& pos1);

			void update(cpg_time* t);

		private:

		};




		double cdif(glm::vec2 src, glm::vec2 dst);
		double between0_1(double x, glm::vec2 m, double dif);
		// 映射到范围
		void between0_1(glm::vec4& xx, glm::vec2 m, double dif);
		// 对话框
		struct dialog_u
		{
			// 根div
			div_u* rdiv = 0;
			div_u* top = 0;
			div_u* content = 0;
			div_u* bottom = 0;
			// 返回按钮数组指针
			button_u** btns = 0;
			int cap = 0;
			int count = 0;
			// 用来保存临时数据
			njson td;
			bool closetype = false;
		};


		struct listdiv_newinfo
		{
			//t_vector<t_string>* vstr;
			layout_text* plt = 0; div_u* p2 = 0;
			int font_height = 16, width = 300;
			uint32_t tc = -1, blur_color = 0;
			float tpx = 10;
			// 0无图标
			image_m_cs* icon = 0;
			void* uptr = 0;
			void (*click_cb)(const char* str, int idx, void* uptr) = 0;
		};
		div_u* new_listdiv(listdiv_newinfo* p);
		void ltd_remove(div_u* p, int idx);
		button_u* ltd_add(div_u* p, const char* str, int len, int idx, int height);
		void ltd_clear(div_u* p);
		//button_u* new_btn(div_u* p, const std::string& str, int idx, bool islast);
		// 旋转对象
		//void up_rotate(anim_a* p, bool isck, const glm::vec2& rpos, cpg_time* t, bool smoothtime = false);


		void show_div2f(div_u* div0, bool visible, bool isfront);
		button_u* add_close(div_u* div, int height = 22, uint32_t text_color = 0xff808080);
		/*
			初始化对话框
			info.title=(char*)u8"标题";
			info.size=[100,100];
			info.pos=[0,0];
			info.closeable=true;
			info.dragable=true;
			info.fill="0xff555555";
			info.tcolor=0;
			info.close_color="0xff202080";
			info.closeable=true;
			info.fontheight=16;
			info.rounding=6;
			info.btn_rounding=6;
			info.btns_color=[0,0];
			info.btns=["Ok","Cancel"];//按钮组
		*/
		dialog_u* init_dialog(dialog_u* p, njson info, sdl_form* form);
		// 绑定事件到按钮，idx=-1则绑定全部按钮
		void dialog_btn_bind(dialog_u* p, int idx, std::function<void(button_u* p)> click_cb);
		void dialog_show(dialog_u* p, bool isshow);
		/*
			size=容器大小，child[{坐标，大小}]，style容器样式flex参数，child_css元素样式：{统一样式},[{样式1},{样式2}],数量最好和n一样
			输出在child的vec4.x.y
		*/
		void do_layout(const glm::vec2& size, glm::vec4* child, size_t n, njson style, njson child_css);
		int do_layout(glm::vec2 size, njson styles, ui::base_u** child, size_t n, glm::vec2 pos = {});


	}
	// !ui

	class array2d_x
	{
	public:
		hz::ui::gtac_c* tdc = 0;
		t_vector<rc_t> rcv;
	public:
		array2d_x();
		~array2d_x();
		// 初始化添加文本对象内存分配器
		void init(hz::ui::gtac_c* p);

		// 批量创建矩形
		rc_t new_rect(glm::vec2 pos, glm::ivec2 cw, glm::vec2 size, uint32_t color, uint32_t fill, double scale);
		void free_rc(rc_t p);

		// 创建文本显示
		text_cs* new_text(std::string str, int fontheight, glm::ivec2 box, glm::vec2 ps, uint32_t color, text_cs* p);
		void free_text(text_cs* p);

		// 创建多坐标用于显示文本
		pos_t new_pos(int n);
		void free_pos(pos_t p);

		// 创建图标显示，需要外部设置区域、坐标
		image_m_cs* new_image(Image* img, int n, glm::vec2 size);
		void free_img(image_m_cs* p, int n);
	private:

	};
	glm::ivec4 mk_clicp(glm::ivec4 cp11, glm::ivec4 clip);




	// url=char*
	videoplayinfo_t* new_video(const void* url, const char* hw, world_cx* ctx, ui::scroll_view_u* sp);

}
//!hz
#endif
