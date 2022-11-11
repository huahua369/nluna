#pragma once

#include <functional>
#include <vector>
/*
* 枚举、结构体
* date:2022/09/01 添加yuv info
* date:2020/12/29
*/
#ifndef YUV_INFO_ST
#define YUV_INFO_ST
struct yuv_info_t {
	void* ctx = 0;
	void* data[3] = {};
	uint32_t size[3] = {};
	uint32_t width = 0, height = 0;
	int8_t format = 0;		// 0=420, 1=422, 2=444
	int8_t b = 8;			// bpp=8,10,12,16
	int8_t t = 0;			// 1plane时422才有0=gbr, 1=brg
	int8_t plane = 0;		// 1 2 3
};
#endif
namespace hz {

	class sdl_form;
	class bw_sdl;
	class fbo_cx;
	class css_text;
	class Fonts;
	class draw_font_info;
	class Action;
	class animate_node;
	class ActionManager;
	class canvas_cx;
	class event_data_cx;
	struct drop_info_cx;
	struct item_box_info;
	class vk_render_cx;
	class dvk_device;
	class dvk_texture;
	class shader_hp;
	class page_view;
	class queue_ctx;
	class gui_ctx;
	class event_master;
	class Image;
#ifndef t_vector
#define t_vector std::vector
#endif // !t_vector


	using vector_ivec2 = t_vector<glm::ivec2>;
	//using vector_ivec2 = std::vector<glm::ivec2>;
	using vector_ivec3 = t_vector<glm::ivec3>;
	//using vector_ivec3 = std::vector<glm::ivec3>;
	using vector_vec2 = t_vector<glm::vec2>;
	//using vector_vec2 = std::vector<glm::vec2>;
	using vector_vec3 = t_vector<glm::vec3>;
	//using vector_vec3 = std::vector<glm::vec3>;
	using vector_vec4 = t_vector<glm::vec4>;
	//using vector_vec4 = std::vector<glm::vec4>;

	/*
		a:link{color:#fff}  未访问时的状态（鼠标点击前显示的状态）
 		a:hover{color:#fff}  鼠标悬停时的状态
 		a:visited{color:#fff}  已访问过的状态（鼠标点击后的状态）
 		a:active{color:#fff}  鼠标点击时的状态
 		a:focus{color:#fff}  点击后鼠标移开保持鼠标点击时的状态（只有在<a href="#"></a>时标签中有

	*/
	struct mouse_state_t
	{
		// rc.xy 矩形宽高zw 圆z=半径w=0
		glm::ivec4 rc;
		uint32_t normal;
		uint32_t hover;
		uint32_t active;
		uint32_t now;
	};


	// 保存对象的缩放、旋转、平移
	struct motion_t {
		glm::vec3 pos;				// 平移坐标
		glm::vec3 scale = { 1.0, 1.0, 1.0 };	// 缩放
		glm::qua<float> rotate;		// 四元数保存旋转量
		glm::vec3 rotate_pos;		// 旋转中心坐标
		// private
		void* viewport_ptr = 0;		// 视区用于一次计算mvp
	};
	/*
	todo 拾取测试
		基本形状：	★矩形、★圆形、像素
					横排/竖排{固定长度、不定宽}
		事件类型
	*/
	enum class event_type :uint32_t {
		none = 0,
		mouse_move,		// 鼠标移动，
		mouse_down,		// 鼠标按下
		mouse_up,		// 鼠标弹起
		mouse_wheel,	// 滚轮消息
		// on需要拾取目标（*矩形、*圆形、自定义判断）才能触发，单击、双击、三击（指定毫秒内）
		on_keypress,	// 按键，需要输入焦点
		on_input,		// 输入法字符，需要输入焦点
		on_editing,		// 输入法: 显示编辑的字符

		on_click,		// 单击双击三击时鼠标在目标范围才能触发
		on_dblclick,
		on_tripleclick,
		on_enter,		// 鼠标进入
		on_leave,		// 鼠标离开
		on_move,		// 鼠标在元素内移动
		on_down,		// 鼠标在元素内按下
		on_up,			// 鼠标弹起，触发条件：先down，鼠标位置不限
		on_hover,		// 鼠标停留在目标区域n毫秒触发
		on_scroll,		// 滚动条，目标范围内触发
		on_fοcus,		// todo获得焦点
		on_blur,		// todo失去焦点

		on_drag,		// 元素被拖动时鼠标移动触发
		on_dragstart,	// 当拖拽元素开始被拖拽的时候触发的事件，此事件作用在被拖曳元素上
		on_dragend,		// 当拖拽完成后触发的事件，此事件作用在被拖曳元素上

		on_dragenter,	// 当拖曳元素进入目标元素的时候触发的事件，此事件作用在目标元素上
		on_dragleave,	// 当被鼠标拖动的对象离开其容器范围内时触发此事件，此事件作用在目标元素上
		on_dragover,	// 拖拽元素在目标元素上移动的时候触发的事件，此事件作用在目标元素上
		on_drop,		// 被拖拽的元素在目标元素上同时鼠标放开触发的事件，此事件作用在目标元素上

		on_ole_drag,	// 使用OLE拖放对象到别的位置
		on_ole_dragover,// 接收OLE拖放在目标元素上移动的时
		on_ole_drop,	// 接收OLE拖放

		on_touch,		// 触控事件

		et_max_num
	};

	DR_ENUM(event_type);

	struct event_km_t
	{
		//event_type:事件类型
		uint32_t etype = 0;
		sdl_form* form = 0;
		float	MousePos[2] = {};                       // Mouse position, in pixels. Set to ImVec2(-FLT_MAX,-FLT_MAX) if mouse is unavailable (on another screen, etc.)
		bool	MouseDown[5] = {};                   // Mouse buttons: 0=left, 1=right, 2=middle + extras. ImGui itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
		float	MouseWheel = 0.0;                     // Mouse wheel Vertical: 1 unit scrolls about 5 lines text.
		float	MouseWheelH = 0.0;                    // Mouse wheel Horizontal. Most users don't have a mouse with an horizontal wheel, may not be filled by all back-ends.
		int		wheel = 0;
		int		wheelH = 0;
		int		mouse_idx = 0;							//  0=left, 1=right, 2=middle + extras
		bool	KeyCtrl = false;                        // Keyboard modifier pressed: Control
		bool	KeyShift = false;                       // Keyboard modifier pressed: Shift
		bool	KeyAlt = false;                         // Keyboard modifier pressed: Alt
		bool	KeySuper = false;                       // Keyboard modifier pressed: Cmd/Super/Windows
		bool	KeysDown[512] = {};	// scancode
		char	ch = 0;				// sym
		char	sym = 0;				// sym
		int		key = 0;
		int		keycode = 0;
		const char* kn = 0;
		bool	pressed = false;
		// 冒泡传到父级，默认关闭
		bool	_bubble = false;
		char	text[32] = {};
		int		str_size = 0;
		glm::ivec2 editing;
		glm::ivec2 pos;
		// 修改的pos
		glm::ivec2 pos1;
		glm::ivec2 pos2;
		void* user_ret = 0;			// 返回数据
	};

#ifndef LBIT
	//位左移
#define LBIT(x) (1<<x)
#endif
	// 类型
	enum class mouse_type_e :uint32_t
	{
		e_null = 0,
		e_enter = LBIT(0),		// 鼠标进入
		e_leave = LBIT(1),		// 鼠标离开
		e_move = LBIT(2),		// 鼠标在元素内移动
		e_down = LBIT(3),		// 鼠标在元素内按下
		e_up = LBIT(4),			// 鼠标弹起，触发条件：先down，鼠标位置不限
		e_hover = LBIT(5),		// 鼠标停留在目标区域n毫秒触发
		e_scroll = LBIT(6),		// 滚动条，目标范围内触发
		e_fοcus = LBIT(7),		// todo获得焦点
		e_blur = LBIT(8),		// todo失去焦点
	};
	DR_ENUM(mouse_type_e);

	// 类型
	enum class geo_type :uint32_t
	{
		null,
		line,
		polyline,
		polygon,
		poly_filled,
		path_filled,
		path,
		rect,
		rect_filled_gradient,
		quad,
		triangle,					// vec2[3]
		dir_triangle,				// vec2(dir,spos), vec2(size)
		circle,						// vec2(centre), vec2(radius),		num_segments在样式保存
		ellipse,					// vec2(centre), vec2(radius), 
		arc,
		bezier_curve,
		//grid,
		radial_gradient,
		linear_gradient,
		poly_gradient,
		image,
		text,
		label,
		nums
	};
	DR_ENUM(geo_type);
	// 白色
#define col_white 0xffffffff
#define col_red 0xff0000ff
	/*
		颜色支持写法  0xaabbggrr、#rrggbb、rgb(255,0,0)、rgba(255,0,0,0.6)
	*/
	// 多段线 <polyline points="20,20 40,25 60,40 80,120 120,140 200,180" 	style ="fill:none;stroke:0xff000000;stroke-width:3"/>
	struct polyline_info_t {
		geo_type_t _type = geo_type::polyline;
		int z_index = 0;
		glm::vec2 pos;
		glm::vec2* points;  int points_count;
		unsigned int col; unsigned int fill; // 线颜色stroke和填充
		bool closed;
		float thickness; //stroke-width
		bool anti_aliased;//抗锯齿
	};
	// 多边形<polygon points="200,10 250,190 160,210"	style = "fill:lime;stroke:purple;stroke-width:1"/>
	struct polygon_info_t {
		geo_type_t _type = geo_type::polygon;
		int z_index = 0;
		glm::vec2 pos;
		glm::vec2* points;
		int points_count;
		unsigned int col; unsigned int fill;
		float thickness; bool anti_aliased;
	};
	// 填充多颜色cols
	struct convex_poly_filled_info_t {
		geo_type_t _type = geo_type::poly_filled;
		int z_index = 0;
		glm::vec2 pos;
		glm::vec2* points;  int points_count; unsigned int col; bool anti_aliased;
		vector_ivec3* cols = nullptr;
	};
	// 填充多边形
	struct convex_path_filled_info_t {
		geo_type_t _type = geo_type::path_filled;
		int z_index = 0;
		glm::vec2 pos;
		glm::vec2* points;  int points_count; unsigned int col; bool anti_aliased;
		vector_ivec3* cols = nullptr;
	};

	/*
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
	struct svg_path_t
	{
		geo_type_t _type = geo_type::path;
		int z_index = 0;
		t_string* cmdstr;				// 解析命令CCCz表示需要有6个vec2数据对应
		vector_vec2* path;					// 坐标比如，M需要一个vec2，而C要三个vec2
		glm::vec2 pos;
		glm::vec2 scale = { 1.0, 1.0 };
		unsigned int col = -1;
		unsigned int fillcol = 0;
		double thickness = 1.0;

	};

	// 直线	<line x1="0" y1="0" x2="200" y2="200" style = "stroke:rgb(255,0,0);stroke-width:2" />
	struct lines_info_t {
		geo_type_t _type = geo_type::line;
		glm::ivec4 a;
		vector_vec4* d; unsigned int col; float thickness; bool anti_aliased;
	};
	// 矩形、填充多颜色	<rect width="300" height="100" style="fill:rgb(0,0,255);stroke-width:1;stroke:rgb(0,0,0)"/>
	struct rect_info_t {
		geo_type_t _type = geo_type::rect;
		int z_index = 0;
		glm::ivec4 a;
		vector_ivec3* cols = nullptr;	// rect_filled_multi_color
		glm::ivec4* cols4 = nullptr;	// rect_filled_multi_color
		unsigned int col; unsigned int fill;
		float rounding = 0.0f; int rounding_corners_flags = ~0; float thickness = 1.0f;
	};
	//// 填充阴影效果
	//struct rect_filled_gradient_info_t {
	//	geo_type_t _type = geo_type::rect_filled_gradient;
	//	glm::vec4 rect;
	//	glm::ivec3 center;  glm::ivec3* inner_ = 0; float rounding = 0.0f; unsigned int col_outer = 0;
	//};
	// 径向渐变
	struct radial_gradient_info_t {
		geo_type_t _type = geo_type::radial_gradient;
		int z_index = 0;
		glm::ivec3 point;
		glm::ivec2 center; vector_ivec3* shape;	int num_segments;
		glm::vec4* rect_scale = nullptr;
		glm::vec4* clip = nullptr;
		unsigned int tcol = 0;
	};
	// 线性渐变
	struct linear_gradient_info_t {
		geo_type_t _type = geo_type::linear_gradient;
		int z_index = 0;
		glm::ivec4 rect;
		t_vector<glm::ivec2>* pstops; glm::vec2 dire = { 1, 0 };
	};
	// 渐变
	struct poly_gradient_info_t {
		geo_type_t _type = geo_type::poly_gradient;
		int z_index = 0;
		glm::ivec3 center;
		double radius; unsigned int c; int n = 2; unsigned int bc = 0; int nums = 360;
	};
	// 四边形
	struct quad_info_t {
		geo_type_t _type = geo_type::quad;
		int z_index = 0;
		glm::vec2 a;  glm::vec2 b;  glm::vec2 c;  glm::vec2 d;
		unsigned int col; unsigned int fill; float thickness = 1.0f;
	};
	// 三角形
	struct triangle_info_t {
		geo_type_t _type = geo_type::triangle;
		int z_index = 0;
		glm::vec2 a;  glm::vec2 b;  glm::vec2 c;
		unsigned int col; unsigned int fill; float thickness = 1.0f; bool anti_aliased = true;
	};
	// 圆形
	/*
	<circle cx="100" cy="50" r="40" stroke="black"  stroke-width="2" fill="red"/>
	*/
	struct circle_info_t {
		geo_type_t _type = geo_type::circle;
		int z_index = 0;
		glm::vec2 centre; float radius;
		unsigned int col; unsigned int fill; int num_segments; float thickness = 1.0f;
	};
	// 椭圆
	/*
	<ellipse cx="300" cy="80" rx="100" ry="50"  style="fill:yellow;stroke:purple;stroke-width:2"/>
	*/
	struct ellipse_info_t {
		geo_type_t _type = geo_type::ellipse;
		int z_index = 0;
		glm::vec2 centre; float rx; float ry;
		unsigned int col; unsigned int fill; int num_segments; float thickness = 1.0f;
	};
	// 弧形
	struct arc_info_t {
		geo_type_t _type = geo_type::arc;
		int z_index = 0;
		glm::vec2 centre; float rx; float ry;
		unsigned int col; unsigned int fill; int num_start; int num_end; int num_segments; float thickness = 1.0f;
	};
	// 贝塞尔曲线
	struct bezier_curve_info_t {
		geo_type_t _type = geo_type::bezier_curve;
		int z_index = 0;
		// glm::vec2 pos0; glm::vec2 pos1; glm::vec2 cp0;  glm::vec2 cp1;
		// vec4[0]{xy=pos0,zw=pos1},[1]{xy=cp0,zw=cp1}
		vector_vec4* point_ctrl;
		unsigned int col; float thickness; int num_segments; glm::vec4* idx_pos = nullptr;
		vector_vec2* outpath = nullptr;
	};
	struct image_info_t
	{
		geo_type_t _type = geo_type::image;
		int z_index = 0;
		Image* user_image = nullptr;
		// a.xy=pos坐标，a.zw=width,height渲染宽高
		glm::vec4 a;
		// 图像区域
		glm::vec4 rect = glm::vec4{ -1 };
		// 是否九宫格缩放
		glm::vec4 sliced = glm::vec4{ -1 };
		unsigned int col = -1;
		//{repeat参数支持1个三角形，2个三角形}
		int repeat = 0;
	};
	struct text_info_t
	{
		geo_type_t _type = geo_type::text;
		int z_index = 0;
		css_text* csst;
		glm::ivec2 pos;
		const char* str;
		size_t count = 0; size_t first_idx = 0;
		const char* str_b;
		draw_font_info* out = nullptr;
	};
	struct label_info_t {
		geo_type_t _type = geo_type::label;
		int z_index = 0;
		item_box_info* itbox = 0;
		glm::ivec2 pos;
		unsigned int color = -1;
	};
	struct draw_info_t {
		geo_type_t _type = geo_type::null;
		int z_index = 0;
		// ...
	};

	/*
	方向三角形
	┌───┐
	│ ╲ │
	│  ╲│
	│  ╱│
	│ ╱ │
	└───┘
	这是一个右方向三角形，dir=1
	*/
	struct triangle_o
	{
		void* next = 0;			// 单向链表，triangle_o
		uint8_t dir = 0;		// 尖角方向，0上，1右，2下，3左
		uint8_t spos = 50;		// 尖角点位置0-100，中间就是50
		glm::vec2 size;			// 大小
		glm::vec2 pos;			// 坐标
	};

	namespace css
	{
		/*
		方向三角形
		┌───┐
		│ ╲ │
		│  ╲│
		│  ╱│
		│ ╱ │
		└───┘
		比如这个右方向三角形，dir=1，spos=50相当于0.5
		*/
		struct dir_triangle_shape_t
		{
			uint8_t dir = 0;		// 尖角方向，0上，1右，2下，3左
			uint8_t spos = 50;		// 尖角点位置0-100，中间就是50
			glm::vec2 size;			// 大小
		};
		struct dir_triangle2_shape_t
		{
			float dir = 0;		// 尖角方向，0上，1右，2下，3左
			float spos = 0.5;	// 尖角点位置0-1，中间就是.5
			glm::vec2 size;			// 大小
		};

		// 圆椭圆信息
		struct ec_shape_t
		{
			glm::vec2 centre;	// 中心点
			glm::vec2 radius;	// 半径，y等于0时则和x一样

		};
		struct color_stroke_t {
			unsigned int color = -1;	// 颜色，用于边框
			unsigned int fill = 0;		// 填充颜色
			float thickness = 1.0f;		// 线粗
			//bool aa = true;				// 抗锯齿
		};
		// 是否启用抗锯齿
		struct anti_aliased {
			bool v = true;
		};

		struct text_shadow
		{
			glm::vec2 hv;		// 水平、垂直位置
			int blur;			// 模糊距离
			uint32_t color;		// 阴影的颜色
		};
		struct box_t {
			glm::ivec4 rect;		// 矩形
			// 小于0为百分比，大于0是px，等于0没有圆角
			glm::vec4 radius4;	// 圆角，顺时针x,y,z,w 0 1 2 3
			float radius;		// 全部一样设置这个
		};
		struct box_shadow
		{
			glm::vec2 hv;		// 水平、垂直位置
			int blur;			// 模糊距离
			int spread;			// 阴影的尺寸。扩展正大负缩小
			uint32_t color;		// 阴影的颜色
			bool inset = false;	// 将外部阴影 (outset) 改为内部阴影。
		};
		// 径向渐变
		struct radial_gradient
		{
			glm::ivec3 point;					// 坐标x,y，颜色z
			glm::ivec2 center;					// 中心点坐标
			std::vector<glm::ivec3>* shape;		// 可设置多个不同颜色的椭圆，xy半径，x=y则是圆，否则是椭圆。z颜色
			glm::vec4* rect_scale = nullptr;	// 矩形缩放
			glm::vec4* clip = nullptr;			// 剪裁区域
			unsigned int tcol = 0;				// 路径线框颜色
			int num_segments = 32;				// 边数
		};
		// 线性渐变
		struct linear_gradient {
			glm::ivec4 rect;					// xy坐标，zw=宽高
			std::vector<glm::ivec2>* pstops;	// 颜色数组，x颜色，y偏移百分比0-100%
			glm::vec2 dire = { 1, 0 };			// 水平渐变{1,0}，垂直渐变{0,1}
		};
		// 快速径向渐变
		struct poly_gradient {
			glm::ivec3 center;					// 坐标、中心颜色
			double radius;
			unsigned int c;						// 外圈颜色
			int n = 2;							// 1、2
			unsigned int bc = 0;				// 路径线框颜色
			int nums = 360;
		};
	}
	//!css


#if 1
	class path_stb;
	enum class shape_base :uint8_t {
		shape_null = 0,
		line,
		rect,
		ellipse,
		circle,
		triangle,
		path,
		image,
		image_m,
		text,
		check
	};
	DR_ENUM(shape_base);
	/*
		基本属性:
		rect, 大小、圆角
		ellipse,大小
		circle,大小
		triangle,大小、方向、位置

		外观属性
	*/
	struct style_cs
	{
		uint32_t	color = 0xff888888;			//颜色
		uint32_t	fill = 0x80cc7a00;			//填充颜色
		int			num_segments = 36;			//圆边点数
		int			position_swap = 0;			// 反转圆角，从1开始，left|right|top|bottom
		glm::ivec4	rounding = { 0,0,0,0 };		//圆角
		glm::ivec4	cols = {};					// 颜色数组，fill为0时使用
		float		thickness = 1;				//线粗
		int			id = 0;						// 
		bool		anti_aliased = true;		//是否抗锯齿
		bool		closed = true;				//是否封闭路径
		void set_rounding(int r) { rounding = { r,r,r,r }; }
	};
	struct familys_t;
	struct style_text_cs
	{
		int			id = 0;						// id
		familys_t* fst = 0;						// 字体集
		int font_height = 16;					// 字高
		uint32_t	color = 0xff888888;			// 颜色
		glm::vec2	ps = { 0.5f,0.5f };			// 对齐偏移
	};
	// 线
	struct line_cs
	{
		glm::vec2 pos = {};
		glm::vec2 pos2 = {};
		style_cs* t = 0;
	};
	// pos是局部坐标
	struct rect_cs
	{
		glm::vec2 pos = {};
		glm::vec2 size = {};
		style_cs* t = 0;
	};
	// 不规则圆角
	struct rect4d_cs
	{
		glm::vec2 pos = {};
		glm::vec2 size = {};
		int dir = 0;			// 方向，0上，1右，2下，3左
		style_cs* t = 0;
	};
	struct ellipse_cs
	{
		glm::vec2 pos = {};
		glm::vec2 r = {};
		style_cs* t = 0;
	};
	struct circle_cs
	{
		glm::vec2 pos = {};
		float r = 1.0f;
		style_cs* t = 0;
	};
	// 三角形基于矩形内
	struct triangle_cs
	{
		glm::vec2 pos = {};
		glm::vec2 size = { 10,10 };
		style_cs* t = 0;
		uint8_t dir = 0;		// 尖角方向，0上，1右，2下，3左
		uint8_t spos = 50;		// 尖角点位置0-100，中间就是50
	};
	// 路径可以填充可线条
	struct path_cs
	{
		glm::vec2 pos = {};
		style_cs* t = 0;
		path_stb* path1 = 0;
		t_vector<glm::vec2>* path = 0;
		int n = 1;
	};
	struct check_mark_cs {
		glm::vec2 pos = {};
		uint32_t col = 0xffFF9E40;
		uint32_t fill = 0xffffffff;
		uint32_t check_col = 0xffffffff;
		float rounding = 2;
		float square_sz = 14;
		float new_alpha = -1;
		bool mixed = false;					// 不满
	};
	// 图像基本属性有：区域、偏移、九宫格；
	struct image_cs
	{
		glm::vec2 pos = {};
		Image* img = 0;				// *图像类
		dvk_texture* tex = 0;		// *纹理，img\tex二选一
		glm::ivec4 rect = {};		// *区域
		glm::vec2 size = {};		// *大小	
		glm::vec4 sliced = {};		// 九宫格渲染 left top right bottom
		uint32_t color = col_white;	// 混合颜色，默认白色不混合
	};
	// 图像min：区域、偏移；cluster、advx渲染文本用
	struct image_m_cs
	{
		glm::vec2 pos = {};
		Image* img = 0;				// *所在图像
		glm::ivec4 rect = {};		// *区域
		glm::vec2 offset = {};		// 渲染偏移
		glm::vec2 size = {};		// *大小		
		uint32_t color = col_white;	// 混合颜色，默认白色不混合
		// 自定义用途变量
		uint32_t cluster = 0;		// 字体分块标志
		int advx = 0;
	};

	// text渲染本质由图像区域组成
	struct text_cs
	{
		glm::vec2 pos = {};			// 局部坐标
		glm::vec2 rc = {};			// 整个文本的区域大小
		image_m_cs* t = 0;			// 数组
		style_text_cs* st = 0;		// 风格可选
		text_cs* next = 0;			// 链表
		unsigned int color = 0;		// 不等于0时用t的颜色
		unsigned int count = 0;		// 数量
		unsigned int first = 0;		// 在数组的位置
		unsigned int crc = 0;
		glm::vec2 pos1 = {};		// 局部坐标1
		bool visible = true;
	};
	struct shape_n
	{
		shape_base_t t = 0;
		shape_n* pNext = 0;
		motion_t* mt = 0;				// ubo、旋转缩放平移
		int n = 0;						// 数量，v如果是数组的话
		glm::vec2 pos = {};				// 本组v的一级坐标
		union {
			struct line_cs* l;
			struct rect_cs* r;
			struct ellipse_cs* e;
			struct circle_cs* c;
			// 三角形基于矩形内
			struct triangle_cs* t;
			// 路径可以填充可线条
			struct path_cs* p;
			// 图像基本属性有：区域、偏移、九宫格；64字节
			struct image_cs* img;
			// 图像min：区域、偏移；48字节
			struct image_m_cs* imgm;
			// text渲染对象
			struct text_cs* text;
			// check\radio
			struct check_mark_cs* ck;
		}v;
		void* uptr = 0;
	};
	struct shape1_cs {
		union {
			struct rect_cs r;
			struct ellipse_cs e;
			struct circle_cs c;
			// 三角形基于矩形内
			struct triangle_cs t;
			// 路径可以填充可线条
			struct path_cs p;
			// 图像基本属性有：区域、偏移、九宫格；64字节
			struct image_cs img;
			// 图像min：区域、偏移；48字节
			struct image_m_cs imgm;
			// text渲染本质由图像区域组成
			struct text_cs text;
		}v;
	};

	struct rc_t
	{
		rect_cs* view = 0;
		style_cs* css = 0;
		int n = 0;
	};
	struct pos_t
	{
		glm::vec2* pos = 0;
		int n = 0;
	};
#endif

	struct form_info_new_t
	{
		//private
		void* ctx = 0;
		void* form = 0;
		//public
		const char* title = 0;
		glm::ivec2 size;
		unsigned int back_color = 0;
		bool borderless = false;
		bool is_fullscreen = false;
		bool is_move = true;
		bool is_size = true;			// 是否可以用鼠标改变大小
		bool visible = true;
		bool before = true;
		bool close_type = true;
		bool is_alpha = false;
		bool always_on_top = false;		// 窗口应该总是在其他窗口之上
		bool skip_taskbar = false;		// 不在任务栏
		bool utility = false;			// 窗口应被视为实用程序窗口
		bool tooltip = false;			// 工具提示
		bool popup_menu = false;		// 弹出菜单
		bool is_gdi = false;			// GDI窗口
		bool is_vulkan = true;			// 是否使用vulkan渲染
		bool is_defevent = true;
		bool focus_lost_hide = false;	// 失去焦点时隐藏窗口
	};

	typedef std::vector<std::function<void(void* ptr)>> vcbs_t;

}
//!hz
