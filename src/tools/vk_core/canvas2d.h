#pragma once
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <ntype.h>
#include <base/camera.h>
#include "view_info.h"
#include <view/draw_info.h>
// 生成矢量图三角形，画单像素线时，坐标+0.5才不会模糊
namespace hz {
	namespace css
	{
		struct text_shadow;
		struct box_t;
		struct box_shadow;
		// 径向渐变
		struct radial_gradient;
		// 线性渐变
		struct linear_gradient;
		struct poly_gradient;

	}

	class path_stb;
	class css_text;
	// 保存对象的缩放、旋转、平移
	struct motion_t;

	struct item_box_info;
	class stb_packer;


	// 图集
	class image_atlas_u
	{
	public:
		Image* img = 0;
		stb_packer* _packer = 0;
		const int ds = 1024;
	public:
		image_atlas_u();
		~image_atlas_u();
		// 重设大小，会清空
		void resize(int width, int height);
		// 清空
		void clear();
		/*
			todo把一个矩形块装箱返回位置
			rc大小，src输入图像，空则不复制到缓存
			ot输出image渲染信息
			return {-1,-1}、nullptr则装不下了
		*/
		glm::ivec2 push_rect(glm::ivec2 rc);
		image_cs* push_rect(glm::ivec2 rc, image_cs* ot);
		image_m_cs* push_rect(glm::ivec2 rc, image_m_cs* ot);
		// 一次插入多个矩形, rc->z=width,rc->w=height;
		int push_rect(glm::ivec4* rc, int n);
		// 复制数据到图集
		void cp2atlas(glm::ivec2& pos, glm::ivec4 src4, Image* src);
		// 复制灰度图到图集
		void cp2atlas(glm::ivec2& pos, glm::ivec4 src4, image_gray* src, uint32_t color);
	private:

	};

	// 32位索引
#if 1
	typedef unsigned int DrawIdx;
#else
	typedef unsigned short DrawIdx;
#endif
	// 图像纹理指针
	struct dsimage_t {
		union
		{
			void* p;
			Image* img;
			dvk_texture* t;
		}p;
		int type;			// 0=Image*，1=dvk_texture*
	};

	// draw命令
	struct DrawCmd
	{
		// 索引数量、第一个绘图的索引位置、顶点偏移	（单位为数量(非字节)）
		uint32_t indexCount = 0, firstIndex = 0, vertexOffset = 0;
		glm::vec4 clipRect = { 0, 0, -1, -1 };	// 裁剪矩形x,y,width,height
		dsimage_t image = {};					// 纹理
		motion_t* ubo = 0;						// 运动数据
		uint32_t dynamic_state = 0;
		uint8_t topology = 0;					// VkPrimitiveTopology
		void* pipe = 0;
	};
	// 顶点属性
	struct vert_t
	{
		glm::vec3 pos;
		glm::vec2 uv;
		uint32_t  col = -1;
	public:
		vert_t() {}
		vert_t(glm::vec3 p, glm::vec2 u, uint32_t  c) :pos(p), uv(u), col(c) { }
		vert_t(glm::vec2 p, glm::vec2 u, uint32_t  c) :pos(p.x, p.y, 0.0f), uv(u), col(c) { }
	};
	struct rect_temp_c {
		glm::ivec2 size;
		int rounding;
		int thickness;
		bool operator!=(const rect_temp_c& t1) {
			return t1.size != size || t1.rounding != rounding || t1.thickness != thickness;
		}
	};
	class can_path
	{
	public:
		enum class commands {
			P_MOVETO = 0,
			P_LINETO = 1,
			P_BEZIERTO = 2,
			P_CLOSE = 3,
			P_WINDING = 4,
		};
	private:
		vector_vec2      _points;
		// 细分容差。对于高度细分的曲线（质量越高，多边形越多），则“减小”可降低质量。
		float CurveTessellationTol = 1.25f;

		t_set<glm::ivec2> _sv;
		vector_vec2 _temp;
	public:
		friend class canvas_cx;
		can_path();

		~can_path();
		size_t size();
	public:
		void append_commands(float* vals, int nvals);

		void clear();
		void line_to(const glm::vec2& pos);
		void path_merge();
		void PathLineToMergeDuplicate(const glm::vec2& pos);

		void arc_to(const glm::vec2& centre, float radius, int num_segments, bool closure, vector_vec2* path = nullptr);
		size_t arc_to(const glm::vec2& centre, const glm::vec2& r, int num_segments, bool closure, vector_vec2* path = nullptr);
		void arc_to(const glm::vec2& centre, const glm::vec2& r, int num_start, int num_end, int num_segments, vector_vec2* path = nullptr);
		size_t PathArcToFast(const glm::vec2& centre, float radius, int amin, int amax, float scale = 1.0f);
		static void PathBezierToCasteljau(vector_vec2* path
			, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level);
		void PathBezierCurveTo(const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, int num_segments);
		void catMullRomSpline(const vector_vec2& inputPoints, vector_vec2* outs, int numSpace = 100);
		void PathRect(const glm::vec2& a, const glm::vec2& b, float rounding = 0.0f, int rounding_corners = ~0);
		glm::ivec4 PathRect(const glm::vec2& a, const glm::vec2& b, const glm::vec4& rounding);
		size_t path_ellipse_circle(const glm::vec2& centre, glm::vec2 radius, int num_segments,
			vector_vec2* path = nullptr, double pos = 0, glm::ivec2 range = { 0, -1 },
			double offset = 0.0f, bool closure = true, glm::vec4* rect_scale = nullptr);

		void rect2(const glm::vec2& a, const glm::vec2& b, const glm::vec4& rounding);
		void path_line(const glm::vec2& a, const glm::vec2& b);
		void sol1(const vector_vec2& path, unsigned int col); //绘制控制多边形的轮廓//n控制点的个数
		double sol2(int nn, int k);  //计算多项式的系数C(nn,k)
		void sol3(double t, glm::vec2* path, size_t count, size_t n);  //计算Bezier曲线上点的坐标
		void sol4(glm::vec2* path, double m, size_t n, bool first); //根据控制点，求曲线上的m个点
	private:

	};
#define PATHCf(t) ((float)hz::can_path::commands::##t)

	class dvk_texture;
	class dvk_set;
	class layout_text;
	struct ubo_set_t;
	class sdl_form;

	class canvas_cx
	{
	public:
		friend class page_view;
		struct vidptr_t {
			vert_t* vtx = 0;
			DrawIdx* idx = 0;
			size_t vsize = 0;
			size_t isize = 0;
			size_t vc = 0;
			size_t ic = 0;
		};
	public:
		int order = 0;
		uint32_t _vcrc = 0, _icrc = 0;
		bool updt = false;
	private:
		t_vector<vert_t>	_vtx_data;				// 顶点数据,vbo
		t_vector<DrawIdx>	_idx_data;				// 索引数据,ibo
		t_vector<DrawCmd>	cmd_data;				// 指令数据

		vidptr_t _wptr = {};

		glm::vec2 _uv_white_pixel = { 0.0, 0.0 };		// 默认白色纹理uv
		glm::vec4 viewport = { 0, 0, -1, -1 };		// 视区矩形x,y,width,height
		glm::vec4 vp0 = { 0, 0, 0, 0 };		// 视区矩形x,y,width,height

		// 路径
		can_path _path;
		// 裁剪栈
		std::stack<glm::vec4> _clip_rect;
		// 纹理
		std::stack<dsimage_t> _image_stack;
		// 动作
		std::stack<motion_t*> _ubo_stack;
		// 缓存更新的图像/纹理/动作
		t_unordered_set<Image*> _img_set;
		t_unordered_set<dvk_texture*> _tex_set;
		t_unordered_set<motion_t*> _ubo_set;
		// 本画布在vbo、ibo的偏移
		uint64_t _vbo_offset = 0;
		uint64_t _ibo_offset = 0;

		// 默认文本布局
		layout_text* lt = 0;
		// 临时数据
		vector_vec2 temp_normalsd;
		// 缓存
		rect_temp_c _rcc;

		int imginc = 0;
		bool ispopimg = true;
		bool depth_test = false;
		bool depth_write = false;
	public:
		canvas_cx();

		~canvas_cx();
		// 返回vbo/ibo大小
		glm::ivec2 data_size();
		vert_t* vdata() { return _vtx_data.data(); }
		size_t vsize() { return _vtx_data.size() * sizeof(vert_t); }
		DrawIdx* idata() { return _idx_data.data(); }
		size_t isize() { return _idx_data.size() * sizeof(DrawIdx); }
		t_vector<DrawCmd>& get_cmd() { return cmd_data; }
		void set_viewport(const glm::ivec4& v);

		glm::ivec4 get_viewport();
		glm::ivec4 get_viewport0();
		void clear();
		void set_layout_text(layout_text* p) { lt = p; }
		layout_text* get_layout_text() { return lt; }

		bool nohas_clip(glm::vec4 a);
		glm::vec4 get_topclip();
	public:
		// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
		void draw_polyline(const glm::vec2& pos, const glm::vec2* points, const int points_count, unsigned int col, bool closed, float thickness, bool anti_aliased);
		// 填充多颜色cols
		void draw_convex_poly_filled(const glm::vec2& pos, const glm::vec2* points, const int points_count, unsigned int col, bool anti_aliased, vector_ivec3* cols = nullptr);
		// 填充多边形
		void draw_convex_path_filled(const glm::vec2& pos, const glm::vec2* points, const int points_count, unsigned int col, bool anti_aliased, vector_ivec3* cols = nullptr);
		void draw_line(const glm::vec2& a, const glm::vec2& b, unsigned int col, float thickness = 1.0f, bool anti_aliased = true);
		void draw_lines(const glm::vec2& pos, const glm::vec2* d, size_t n, unsigned int col, float thickness, bool anti_aliased);
		void draw_rect_filled_multi_color(const glm::vec4& a, vector_ivec3* cols = nullptr, float rounding = 0.0f, int rounding_corners_flags = ~0);
		void draw_rect(const glm::vec2& pos, const glm::vec2& size, unsigned int col, unsigned int fill, float rounding = 0.0f, float thickness = 1.0f);
		void draw_rect(const glm::ivec4& a, unsigned int col, unsigned int fill, float rounding = 0.0f, int rounding_corners_flags = ~0, float thickness = 1.0f);
		void draw_rect(const glm::vec4& a, unsigned int col, unsigned int fill, const glm::vec4& rounding, float thickness);
		void draw_rect_filled_multi_color(glm::ivec4 a4, unsigned int col_upr_left, unsigned int col_upr_right, unsigned int col_bot_right, unsigned int col_bot_left, bool diagonal = false, const glm::vec4& r = {});

		//	0 = rect 或 1 = circle 或 2 = ellipse
		// 填充阴影效果
		// center和inner二选一，inner可选
		// glm::ivec3 center = { 270,280,0x800000ff };
		// glm::ivec3 inner = { 10,10,0x50800000 };
		void draw_rect_filled_gradient(const glm::vec4& rect, const glm::ivec3& center, const glm::ivec3* inner_ = 0, float rounding = 0.0f, unsigned int col_outer = 0);
		/*
			径向渐变
			ivec3 point坐标x,y颜色z
			center={中心点}
			shape=半径，x=y则是圆，否则是椭圆，z颜色，w=椭圆num_segments
			rect_scale矩形缩放
			clip剪裁
		*/
		void draw_radial_gradient(const glm::ivec3& point, const glm::ivec2& center, vector_ivec3* shape,
			int num_segments, glm::vec4* rect_scale = nullptr, glm::vec4* clip = nullptr, unsigned int tcol = 0);
		//void draw_radial_gradient(css::radial_gradient* radial);
		void draw_radial_gradient(radial_gradient_info_t* radial);
		/*
			 线性渐变
			 rect x,y width,height
			 颜色数组glm::ivec2{x.color, y.offset=0-100%};
			  1,0 x+ y
			  0,1 x  y+

					水平渐变 y12固定p2 = { a.x, c.y };//1,0
					--------
					--------
					垂直渐变 x12固定p2 = { c.x, a.y };//0,1
					|	|
					|	|
					|	|

		*/
		void draw_linear_gradient(glm::ivec4 rect, vector_ivec2* pstops, glm::vec2 dire = { 1, 0 });
		//void draw_linear_gradient(css::linear_gradient* linear);
		void draw_linear_gradient(linear_gradient_info_t* linear);
		void draw_poly_gradient(glm::ivec3 center, double radius, unsigned int c, int n = 2, unsigned int bc = 0, int nums = 360);
		void draw_poly_gradient(css::poly_gradient* info);


		void draw_quad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, unsigned int col, unsigned int fill, float thickness = 1.0f);
		void draw_triangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, unsigned int col, unsigned int fill, float thickness = 1.0f, bool aa = true);
		void draw_triangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, unsigned int col, const glm::ivec3& fill, float thickness, bool aa);

		//void draw_triangle(const glm::vec2& pos, triangle_o* tp, unsigned int col, unsigned int fill, float thickness = 1.0f, bool aa = true);
		//void draw_triangle(const glm::vec2& pos, triangle_o* tp, unsigned int col, const glm::ivec3& fill, float thickness = 1.0f, bool aa = true);
		void draw_triangle(const glm::vec2& pos, triangle_cs* tp);

		void draw_circle(const glm::vec2& centre, float radius, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f);
		void draw_ellipse(const glm::vec2& centre, float rx, float ry, unsigned int col, unsigned int fill, int num_segments, float thickness = 1.0f);
		void draw_arc(const glm::vec2& centre, float rx, float ry, unsigned int col, unsigned int fill, int num_start, int num_end, int num_segments, float thickness = 1.0f);
		void draw_bezier_curve(const glm::vec2& pos0, const glm::vec2& cp0, const glm::vec2& cp1, const glm::vec2& pos1, unsigned int col, float thickness, int num_segments, glm::vec4* idx_pos = nullptr, vector_vec2* outpath = nullptr);
		void draw_bezier_curve(bezier_curve_info_t* p);
		// 三阶贝塞尔曲线 1段线2个控制点,point节点，ctrl控制点，col颜色，thickness线宽，num_segments段数
		void draw_bezier_curve_multi(const vector_vec2& point, const vector_vec2& ctrl, unsigned int col, float thickness, int num_segments = 0);
		// todo		文本渲染
		glm::ivec2 draw_text(css_text* csst, glm::ivec2 pos, const std::string& str, size_t count = -1, size_t first_idx = 0, draw_font_info* out = nullptr);
		glm::ivec2 draw_text(text_info_t* p, size_t n = 1);
		// todo 渲染hb布局文本  颜色数组colors=ivec3{x=first, y=second, z=color}
		void draw_text(item_box_info* itbox, glm::ivec2 pos1, unsigned int color);
		void draw_image(Image* user_image, glm::vec4 a, glm::vec4 rect = glm::vec4{ -1 }, const glm::vec4& sliced = glm::vec4{ -1 }, unsigned int col = -1);
		void draw_image(draw_image_info* info);

		// 普通图片显示 {支持显示部分区域、九宫格}	
		void draw_image(image_cs* info, const glm::vec2& pos, uint32_t color);
		void draw_image(image_m_cs* p, const glm::vec2& pos1, uint32_t color);
		//void draw_image(image_cs1* p, const glm::vec2& pos1, uint32_t color);
		// 平铺图像	{tn参数支持1个三角形，2个三角形}
		void draw_image_repeat(Image* user_image, glm::vec4 rect, int tn = 2, unsigned int col = -1);

		// 可用于拉伸显示，abcd四个点，四个uv
		void draw_image_quad(Image* user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d
			, const glm::vec2& uv_a, const glm::vec2& uv_b, const glm::vec2& uv_c, const glm::vec2& uv_d, unsigned int col = -1);
		// 渲染自定义uv图像
		void draw_image_uv(Image* user_image, glm::vec4 rect, glm::vec4 uv_top = { 0, 0, 0, 1 }, glm::vec4 uv_bottom = { 1, 1, 1, 0 }, bool isswap = false);
		// 使用三角形渲染图像，显示区域为rect
		void draw_image3(Image* user_image, glm::vec4 rect, glm::vec4 uv_top = { 0, 0, 1, 0 }, glm::vec2 uv_bottom = { 0, 1 });	
		// gif专用
		void draw_image_gif(Image* user_image, glm::vec2 pos, int idx = -1);
		void draw_rect(const glm::vec2& pos, const glm::vec2& size, unsigned int col, unsigned int fill, glm::vec4 r, float thickness);

		// 渲染ui基本图形
		//void draw_shape(shape_base_t type, shape1_cs* v, const glm::vec2& pos);
		void draw_shape(line_cs* r, int n, const glm::vec2& pos);
		void draw_rect2(rect4d_cs* r, const glm::vec2& pos);
		void draw_shape(rect_cs* r, int n, const glm::vec2& pos);
		void draw_shape(rect4d_cs* r, int n, const glm::vec2& pos);
		void draw_shape(ellipse_cs* e, int n, const glm::vec2& pos);
		void draw_shape(circle_cs* c, int n, const glm::vec2& pos);
		// 三角形基于矩形内
		void draw_shape(triangle_cs* t, int n, const glm::vec2& pos);
		// 路径可以填充可线条
		void draw_shape(path_cs* p, int n, const glm::vec2& pos/*, const glm::vec2& pos1 = {}*/);
		void draw_shape(check_mark_cs* p, int n, const glm::vec2& pos);
		// 图像基本属性有：区域、偏移、九宫格；64字节
		void draw_shape(image_cs* img, int n, uint32_t color, const glm::vec2& pos);
		//void draw_shape(image_cs1* img, int n, uint32_t color, const glm::vec2& pos);
		// 图像min：区域、偏移；48字节
		void draw_shape(image_m_cs* imgm, int n, uint32_t color, const glm::vec2& pos);
		// text渲染本质由image_m_cs图像区域组成
		void draw_shape(text_cs* text, int n, const glm::vec2& pos);
		// 链表文本
		void draw_shape(text_cs* text, const glm::vec2& pos, image_m_cs* t);


		static void pad5s(glm::vec2& p, float p1 = 0.5f);
	public:
		/*
			画网格
		*/
		struct grid_info_t {
			glm::vec3 pos;			// pos坐标（有motion_t*时推荐设置0）
			glm::vec2 size;			// size边框大小
			glm::ivec2 subdivs;		// subdivs{细分每格宽高}
			glm::ivec3 color;		// {边框颜色，网格颜色，背景颜色}默认黑色
			vector_ivec2* x_width = nullptr, * y_height = nullptr;//单独设置宽高,glm::ivec2{x位置,y宽/高度}
		};
		void draw_grid(grid_info_t* git);

		void draw_svg_path(svg_path_t* p);
	public:
		can_path* get_cpath();
		void PathFillConvex(unsigned int col, bool isclear = true, bool a = true, vector_ivec3* cols = nullptr);
		void PathStroke(unsigned int col, bool closed, float thickness = 1.0f, bool a = true);
		vidptr_t PrimReserve(int idx_count, int vtx_count);
		void PrimRect(const glm::vec2& a, const glm::vec2& c, unsigned int col);

		void add_rect_a(const glm::vec2& a, const glm::vec2& b, unsigned int col, unsigned int fill, float rounding = 0.0f, int rounding_corners_flags = ~0, float thickness = 1.0f);
		void add_rect_filled(const glm::vec2& a, const glm::vec2& b, unsigned int col, float rounding = 0.0f, int rounding_corners_flags = ~0, vector_ivec3* cols = nullptr, float offset = 0.5);
		void add_rect_filled(const glm::vec2& a, const glm::vec2& b, unsigned int col, const glm::vec4& rounding, vector_ivec3* cols);

		void add_bezier_multi(const vector_vec2& path, unsigned int col, float thickness, int num_segments, int cp = 0, unsigned int control_col = 0);
		void add_bezier_multi(const vector_vec2& point, const vector_vec2& ctrl,
			unsigned int col, float thickness, int num_segments, unsigned int control_col = 0, int ctrl_count = 2);
		// Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::push_clipRect() to affect logic (hit-testing and widget culling)
		void push_clipRect(glm::vec2 cr_min, glm::vec2 cr_max, bool intersect_with_current_clip_rect = false);
		void push_clipRect(glm::vec4 cr, bool intersect_with_current_clip_rect = false);
		void push_full_clip();

		void pop_clipRect();

		void push_image(Image* user_image);
		void push_image(dvk_texture* user_image);
		void pop_image();
		void push_ubo(motion_t* user_ubo);
		void pop_ubo();
		// 批量增加渲染对象draw_info_t* p
		void add_obj(const void* p, size_t n, size_t stride = 0, size_t offset = 0);
	private:
		bool is_last(DrawCmd* prev_cmd);
		void AddDrawCmd();
		DrawCmd* update_clipRect();
		DrawCmd* update_image();
		DrawCmd* update_ubo();
	public:
		static size_t get_path2_index(int64_t points_count, t_vector<DrawIdx>& out, int offset, int offset1, bool is_close = true);
		static size_t get_point_path_index(int64_t points_count, t_vector<DrawIdx>& out, int offset = 1, int base_point = 0);
		void AddImage_b(Image* user_image, const glm::vec2& a, const glm::vec2& b, const glm::vec2& uv_a, const glm::vec2& uv_b, unsigned int col = -1);
		void AddImage_a(Image* user_image, const glm::ivec4& a, const glm::ivec2& texsize, const glm::ivec4& rect = glm::ivec4{ -1 }, unsigned int col = -1);
		void make_sliced(Image* user_image, const glm::ivec4& a, const glm::ivec4& sliced = glm::ivec4{ 10.0f, 10.0f, 10.0f, 10.0f }, const glm::ivec4& rect = glm::ivec4{ -1 }, unsigned int col = -1, glm::ivec2 texsize = {});

		// Render a triangle to denote expanded/collapsed state
		void addDirTriangle(const glm::vec2 s, glm::vec2 pos, int dir, unsigned int fill, unsigned int color = 0);
		// 加入一个顶点和idx
		void PrimVtx(const glm::vec2& pos, const glm::vec2& uv, unsigned int col);
	private:
	};

	bool is_rect_intersect(glm::vec4 r1, glm::vec4 r2);
	class clicp_x
	{
	public:
		canvas_cx* cp = 0;
	public:
		clicp_x(canvas_cx* p, const glm::vec4& c, bool intop = false)
		{
			if (c.z * c.w > 0)
				cp = p;
			if (cp)
			{
				cp->push_clipRect(c, intop);
			}
		}

		~clicp_x()
		{
			if (cp)
			{
				cp->pop_clipRect();
			}
		}

	private:

	};

	// 自动绑定ubo
	class ubo_x
	{
	public:
		motion_t* p = 0;
		canvas_cx* cp = 0;
	public:
		ubo_x(motion_t* p1, canvas_cx* cp1) :p(p1), cp(cp1)
		{
			if (cp && p)
				cp->push_ubo(p);
		}

		~ubo_x()
		{
			if (cp && p)
				cp->pop_ubo();
		}

	private:

	};


	class path_stb
	{
	public:
		typedef struct
		{
			float x, y, cx, cy, cx1, cy1;
			unsigned char type, padding;
		} vertex_f;
		enum winding_e {
			P_CCW = 1,			// Winding for solid shapes
			P_CW = 2,			// Winding for holes
		};

		enum solidity_e {
			P_SOLID = 1,		// CCW
			P_HOLE = 2,			// CW
		};
	public:
		t_vector<stbtt_vertex> v;
		t_vector<vertex_f> vf;
		// 渲染用
		//image_cs* img = 0;
		t_vector<t_vector<glm::vec2>> _points;
		t_string windings;
		glm::vec4 bounds;
		glm::vec2 scale = { 1,1 };
		t_vector<float> commands;
	private:
		//std::vector<float> vf;
		int ccommands = 0;
		int ncommands = 0;
		float tessTol = 1.25f;
		float distTol = 0;
		float fringeWidth = 0;
		float devicePxRatio = 0;
		float commandx = 0, commandy = 0;
		/// state
		//float xform[6];
		bool stroke = true;
		float thickness = 1.0;
	public:
		path_stb();
		~path_stb();
		void clear();
		bool empty();
	public:
		// Calculate pixel ration for hi-dpi devices.
		// pxRatio = (float)fbWidth / (float)winWidth;
		void setDevicePixelRatio(float ratio);
		// 设置是否有笔画线
		void set_stroke(bool is, float thickness);
		// Clears the current path and sub-paths.
		void BeginPath();

		// Starts new sub-path with specified point as first point.
		void MoveTo(float x, float y);
		void move_to(glm::vec2 pos);

		// Adds line segment from the last point in the path to the specified point.
		void LineTo(float x, float y);
		void line_to(glm::vec2 pos);

		// Adds cubic bezier segment from last point in the path via two control points to the specified point.
		void BezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
		void bezier_to(glm::vec2 c1, glm::vec2 c2, glm::vec2 pos);

		// Adds quadratic bezier segment from last point in the path via a control point to the specified point.
		void QuadTo(float cx, float cy, float x, float y);

		// Adds an arc segment at the corner defined by the last path point, and two specified points.
		void ArcTo(float x1, float y1, float x2, float y2, float radius);

		// Closes current sub-path with a line segment.
		void ClosePath();

		// Sets the current sub-path winding, see NVGwinding and NVGsolidity. 
		void PathWinding(int dir);

		// Creates new circle arc shaped sub-path. The arc center is at cx,cy, the arc radius is r,
		// and the arc is drawn from angle a0 to a1, and swept in direction dir (NVG_CCW, or NVG_CW).
		// Angles are specified in radians.
		void Arc(float cx, float cy, float r, float a0, float a1, int dir);

		// Creates new rectangle shaped sub-path.
		void Rect(float x, float y, float w, float h);

		// Creates new rounded rectangle shaped sub-path.
		void Rect(float x, float y, float w, float h, float r);

		// Creates new rounded rectangle shaped sub-path with varying radii for each corner.
		void RectVarying(float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft);

		// Creates new ellipse shaped sub-path.
		void Ellipse(float cx, float cy, float rx, float ry, int st = -1);

		// Creates new circle shaped sub-path. 
		void Circle(float cx, float cy, float r);

		// Fills the current path with current fill style.
		//void Fill();

		// Fills the current path with current stroke style.
		//void Stroke();
		void flattenPaths(bool is_stroke);
		static void tesselateBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int level, int type, float tessTol, t_vector<glm::vec2>* p);

		void appendCommands(float* vals, int nvals, int st = -1);
	private:
	};
	// 获取两个矩形的交集
	glm::vec4 rect_cross1(glm::vec4 a, glm::vec4 b);

}
//!hz
