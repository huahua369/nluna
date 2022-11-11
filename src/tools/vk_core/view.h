#pragma once

#include <ntype.h>
#include "view_info.h"
#include <view/flex_cx.h>
#include "vk_cx.h"
#include "vk_cx1.h"
#include "ecs.h"

namespace hz {

	template <class _T, typename... Args>
	_T* newop(_T*& p, Args... args)
	{
		p = new _T(args...);
		return p;
	}


	using entt::entity;

	struct matubo_t
	{
		ubo_set_t us;
		glm::mat4 m;
	};

	class ubo_ac
	{
	public:

		struct ubo_capinfo_t
		{
			ubo_set_t dsb = {};
			uint32_t upos = 0, apos = 0;				// 局部偏移，大偏移
		};
		pipeline_ptr_info* _pipe = 0;					//管线
		t_vector<dvk_buffer*> allubo;
		dvk_buffer* _ubo = 0;
		size_t _ubo_count = 1;
		size_t _ubo_cap = 0;							// 当前分配的
		dvk_set* _dset = 0;								// set管理
		int dev_maxubo_size = 0;						// 设备支持的ubo大小
		int ubo_stride = 0;								// shader使用的ubo大小
		// UBO分配
		ubo_capinfo_t ucap = {};
		std::vector<std::string> ubon, texn;


		// 支持纹理、image
		std::map<void*, ubo_set_t> image_setm;
		std::map<void*, ubo_set_t> ubo_setm;
	public:
		ubo_ac();
		~ubo_ac();
		// setidx=-1则全部，
		void init(pipeline_ptr_info* pipe);

		//void write_image_ubo(dvk_texture* p, int ubosize, int pos);
		ubo_set_t write_image(dvk_texture* p);

		std::array<ubo_set_t, 2> get_set2(motion_t* ubo, void* img);
		//ubo_set_t new_ubo(size_t size = sizeof(glm::mat4));
		void mk_ubo(ubo_set_t& us);
		void push_ubo(motion_t* m);

		bool needed(size_t n, size_t stride);
		void update(std::map<canvas_cx*, matubo_t>& vpcubo);
	private:

		void make_sets();
	};

	// 页面渲染管理
	class page_view
	{
	private:
		struct last_dset_t
		{
			VkDescriptorSet dset = 0;
			int cap = 0;									// 未分配的set
		};
		pipeline_ptr_info* _pipe = 0;					//管线
		dvk_buffer* _vbo = 0;
		dvk_buffer* _ibo = 0;

		std::vector<VkDescriptorSet> ubo_sets;			// ubo分配完的set
		last_dset_t last_set;							// 只有一个vbo时
		std::map<std::string, last_dset_t> lastsets;

		//std::vector<ubo_set_t> vpset;
		std::map<canvas_cx*, int> vcm;

		std::map<canvas_cx*, matubo_t> _vpcubo;
		std::vector<glm::ivec4> ubo_viewport;			// ubo视口数据
		//std::vector<motion_t*> ubo_data;				// ubo运动数据
		std::set<motion_t*> old_ubo_data;				// ubo运动数据


		std::set<canvas_cx*> canvas_list;				// 显示的2d画布
		std::stack<canvas_cx*> _adc, _fdc;
		std::recursive_mutex _mtx;
		dvk_texture* tex_white = 0;
		ubo_set_t white_set = {};
		std::map<Image*, dvk_texture*> image_map;
		upload_cx* p_upload = 0;
		// 纹理缓存
		std::queue<dvk_texture*> image_gc;
		std::set<dvk_texture*> meimg;
		std::set<dvk_texture*> tex_free;
		float znear = 0.0f, zfar = 10000.0f;
		std::vector<glm::mat4> uboview;
#ifdef _OBJ_POOLf
		obj_pool<motion_t, false>* _motp = 0;
		palloc_cx* uboalloc = 0;
#else

#ifdef _pmr_pool_nt
		mbpool_t _alloc;
#endif 
#endif // pcx_lock_type
		Fonts* _ftctx = 0;
		ubo_ac _uac;
		auto_destroy_cx dc;
		bool _isdx = false;
		bool _isinit = false;
	public:
		page_view();
		~page_view();
		template<class T>
		void free2clear(T& s)
		{
			for (auto it : s)
			{
				if (it)
					delete it;
			}
			s.clear();
		}
		// 管线、cap_count预先分配顶点(单位m)
		int init(pipeline_ptr_info* pipe, Fonts* ftctx);
		// bool iscap是否分配set
		motion_t* new_mot(int n, bool iscap = true);
		// 清空所有mot
		void clear_mot();
		canvas_cx* new_canvas(bool is_draw);
		// 显示画布
		void push_canvas(canvas_cx* p);
		void pop_canvas(canvas_cx* p);
		ubo_set_t get_idxubo(canvas_cx* p);

		void pop_image_tex(Image* img);
		// 创建yuv纹理
		dvk_texture* new_yuvtex(yuv_info_t* p, upload_cx* up = 0);
		upload_cx* get_upptr();
	public:
		// 写入vbo\ibo空间不够自动扩大
		void update_data();
		glm::mat4 ortho(float width, float height);
	public:
		void draw_call(dvk_cmd* cmd, canvas_cx** p, size_t count);
	private:
		// 格式化裁剪矩形
		glm::ivec4 mk_scissor(glm::vec4 src, canvas_cx* c);

		void push_image(Image* img);
		void push_ubo(motion_t* m);
		dvk_texture* get_image_tex(Image* img);
	};
	// !page_view
	// 
	// todo 表格结构{例如文本的鼠标拖选}
	class table_view_e
	{
	public:
		struct item_t
		{
			int weight;		// 宽/高度
			int n = 1;		// 数量
		};

	private:
		// 每行宽度信息
		t_vector<t_vector<item_t>> _line;
		// 垂直方向，高度
		t_vector<item_t> _vertical;
	public:
		table_view_e();
		~table_view_e();
	public:
		// 初始化行
		void init_row(int n, int height);
		// 设置行高
		void set_row(int idx, int height);
	private:

	};

	class math_cx
	{
	private:

	public:
		math_cx()
		{
		}

		~math_cx()
		{
		}
		//判断矩形是否相交
		static bool rect_cross(const glm::vec4& r1, const glm::vec4& r2);
		// 计算两个点之间距离
		static int distance(const glm::vec2& s, const glm::vec2& p);

		// 判断点p是否在圆内{c.xy=pos ,c.z=radius}
		static bool inCircle(const glm::vec3& c, const glm::vec2& p);
		// 判断点是否在矩形{x,y,w,h}内，o输出o-=r.xy
		static bool inRect(const glm::vec4& r, const glm::vec2& p, glm::vec2* o = 0);
		/*
			判断坐标是否在一组热区
			type="343434";
			统一格式化float4
			多个匹配输出到outidx
			isone匹配到第一个退出
		*/
		static size_t inData(const t_vector<glm::vec4>* d, const t_string& type, const glm::vec2& p, t_set<int>* outidx);
		static size_t in_rect(const t_vector<glm::vec4>* d, const glm::vec2& p, t_set<int>* outidx);
	private:

	};

	// 滚轮
	struct wheel_info_t {

		double _maxW = 100, _minW = 0, _step = 1;	//滚轮最大最小、滚动量
		int _nWheel = 0;						//滚轮值
	};
	// 拖动
	struct drag_info_t {
		glm::ivec2 _firstpos, _lastpos;
		// 按下坐标，拖动最后坐标
		glm::ivec2 _pos_down;
		glm::ivec2 _pos_end;
		glm::ivec4 _rect;
		glm::ivec2 first_pos;	// 第一个手动格式化坐标
		glm::ivec4 re_rect();
		void set_drag_begin(int x, int y, const glm::ivec2& pos);
		void set_drag_end(int x, int y, const glm::ivec2& pos);
	};
	// 组件事件监听数据结构
	class event_data_cx
	{
	public:
		friend class event_master;
		using ecb_t = std::function<void(event_km_t* e)>;
		using vcb_t = std::function<void()>;
		struct cd_t
		{
			uint64_t* p = 0;
			uint64_t v = 0;
		};
	public:
		// 排序
		int order = 0;
		event_data_cx* next = 0;
		ui::base_u* pridata = 0;
		event_master* _em = 0;
		glm::ivec2 cm_pos = {};				// move当前坐标
		int child_count = 0;	// ecs命中
		int mchild_count = 0;	// ecs命中移动
		glm::vec4 lv4;
		size_t lidx = -1;
		bool* pvisible = 0;
		bool is_input = false;	// 是否可以输入
		bool visible = true;
		bool _is_hot = false;
	private:
		t_set<int> _hotidx;		// 命中集合
		// 热区判断
		t_vector<glm::vec4> _hot;	// 热区
		t_string _ht;				// '3'圆，'4'矩形
		// ecs
		//entt::registry reg;

		//event_type 事件类型，回调函数
		t_map<uint32_t, t_vector<ecb_t>> _on_cbs;
		t_map<uint32_t, t_map<uint64_t*, uint64_t>> _on_data;	// 快速修改值
		t_vector<bool>	_maskcb;
		glm::ivec2 node_pos = {};				// 对象坐标，拖动时用到，需要手动更新
		wheel_info_t wheel_info = {};
		bool enabled = true;					// 是否接收事件
		bool is_wheel = false;
		bool is_oledrag = false;
		bool is_drag = false;
		bool is_drop = false;
		bool is_auto_drag = false;
		bool _is_upcd = true;						//是否更新了事件数据
		int curr_oledrag = 0;//正在OLE拖动
		int curr_drag = 0;//正在拖动
		int curr_drop = 0;
		unsigned int	_nMouseState = 1;			//鼠标状态
		drop_info_cx* _oledrop = 0;
		// 拖放的数据
		void* _drag_data = 0;
		size_t _drag_size = 0;
#ifdef _pmr_pool_nt


		//uspool_t _alloc0;	// 内存分配
#endif
		// 另一个线程执行事件
		bool is_athread = false;
		std::queue<event_km_t> event_runcb;
		std::queue<vcb_t> exe_cbs;
		std::recursive_mutex _mtx;
		// 在自定义线程执行的事件类型
		t_set<event_type_t> _thread_exetype;
		// 线程id
		uint64_t ay_tid = 0;
		// 父级
		event_data_cx* _parent = 0;
	public:
		event_data_cx();
		~event_data_cx();
		static void run_thr(event_data_cx* p);
	public:
		void set_parent(event_data_cx* p) { _parent = p; }
		void set_athread() { is_athread = true; }
		void set_no_athread() { is_athread = false; }
		//void add_exetype(event_type_t e);
		//void add_exetype(const t_vector<event_type_t>& e);
		void post_vcb(vcb_t cb);
		void send_vcb(vcb_t cb);
		void push_event(event_km_t* e);
		void send_event(event_km_t* e);
		bool is_atexe(event_type_t e);
		void at_run(bool is_one);
		size_t size();
		// 获取第一个命中idx
		int  get_begin();
		int  get_rbegin();		// 最后一个命中的
		int  get_hot_count();
	public:
		void mket(event_type t);
		// 注册事件改变数据，value改变时需要重新注册
		void set_cd(event_type t, uint64_t* result, uint64_t value);
		// 注册事件回调函数
		template<typename Function, typename... Args>
		void set_cb(event_type t, const Function& func, Args... args)
		{
			auto& f = _on_cbs[(uint32_t)t];
			mket(t);
			f.push_back(std::move([func, args...](event_km_t* e) { return func(e, args...); }));
		}

		/*
		·类成员函数
		·this->then( this,&Test::func, str, i);
		*/
		template<typename Function, typename Self, typename ...Args>
		void set_cb(event_type t, Self* self, Function func, Args ...args)
		{
			auto& f = _on_cbs[(uint32_t)t];
			mket(t);
			f.push_back(std::move([func, self, args...](event_km_t* e) { return (self->*func)(e, args...); }));
		}
	public:
		bool is_upcd();
		void set_nodepos(const glm::ivec2& pos)
		{
			node_pos = pos;
		}
		glm::ivec2 get_nodepos()
		{
			return node_pos;
		}
		// 设置pos自动拖动鼠标更新
		void set_dragable(bool is);
		bool get_dragable();

		// 增加拾取区域，坐标原点使用组件坐标系
		// c.xy=pos,z=width,w=height
		size_t add_rect(const glm::vec4& c);
		// c.xy=pos,z=radius
		size_t add_circle(const glm::vec4& c);
		// 修改指定热区
		void set_v4(size_t idx, const glm::vec4& c, bool is_rect = true);
		glm::vec4 get_v4(size_t idx);
		void clear_hot();
		std::string get_dropstr();
		bool ishot();
		// 设置拖放数据
		void set_drag_data(void* data, size_t size);
	public:
		void call_cb(event_km_t* e, bool is_data = true);
		void call_acb(event_km_t* e, bool is_data = true);
		// 是否有单击
		bool is_click();
		// 是否有双击
		bool is_dblclick();
		// 是否有三击
		bool is_tripleclick();

		bool is_type2on(event_type et);
		void on_scroll(int x, int y, event_km_t* e);
	private:
		bool is_contains(glm::ivec2 pos);
		void mk_oledrop();
	public:
		// ecs
	};

	class timer_pex :public promise_cx
	{
	private:
		mp_timer mt;
	public:
		timer_pex();
		~timer_pex();
		void init_task();
		bool call_cb();
	public:
	};
	struct edp_ar {
		event_data_cx* p;
		bool ispush = false;
	};
	// 事件管理
	class event_master
	{
	private:
		t_vector<event_data_cx*> nodes;
		t_set<event_data_cx*> nodes_drop;
		t_set<event_data_cx*> hover_vn;
		glm::ivec4 _viewport;				// 视图区域
		std::recursive_mutex _mtx;
		std::queue<edp_ar> _tem_ar;			// 多线程操作增加删除
		// 鼠标拾取的对象
		event_data_cx* _hotnode = 0, * _lasthotnode = 0;
		event_data_cx* _olehotnode = 0;
		event_data_cx* _dragnode = 0;	// 正在拖动的元素
		event_data_cx* _keynode = 0;	// 可接收键盘的元素，比如tab切换焦点
		glm::ivec2 last_mouse = {};
		int last_nc = 0;
		// 悬停触发时间
		int _hover_ms = 200;
		timer_cx* _hover_act = 0;

		// 双击时间
		int _dbt = 500;
		int _cks = 1;
		mp_timer _t;
		// 鼠标坐标
		glm::ivec2  _mousepos;
		// 拖动信息
		drag_info_t drag_info = {};

		event_km_t te = {};
		// 窗口管理
		bw_sdl* _bw_ctx = 0;

		entt::registry _reg;
		std::atomic_int sort_inc = 0;
	public:
		event_master();
		~event_master();
	public:
		void set_bwctx(bw_sdl* p);
		bw_sdl* get_bwctx();
		void push(event_data_cx* p);
		void pop(event_data_cx* p);
		// 更新排序
		void up_sort();
		// 设置视图区域
		void set_viewport(glm::ivec4 viewport);
		bool is_contains_hit(glm::vec2 pos);
		bool is_contains(glm::vec2 pos);
	public:
		drop_info_cx* call_mouse_move_oledrop(int x, int y);
		bool call_mouse_move(int x, int y, event_km_t* e);
		// idx=0左1右2中
		void call_mouse_down(int idx, int x, int y, event_km_t* e);
		void call_mouse_up(int idx, int x, int y, event_km_t* e);
		// 滚轮消息 vertical =y,	horizontal =x
		void call_mouse_wheel(int x, int y, event_km_t* e);
		void call_key(event_km_t* e);
		void call_input(event_km_t* e);
	private:
		void run_newdel();
		void onMouseMove(int x, int y);
		void onMouseEnter(event_data_cx* node);
		void onMouseLeave(event_data_cx* node, event_type_t t);
		void onMouseDown(int idx, int x, int y);
		void onMouseUp(int idx, int x, int y);
		void ondragend(event_data_cx* node, event_km_t& te, int x, int y);
		void on_oledrag(event_data_cx* node);
		void on_drag(int x, int y);
		void mulit_click(event_data_cx* node);
		void on_node_up(event_data_cx* node, int idx);
	};


	class bw_sdl;
	class cpg_time;
	class queue_ctx;
	class gui_ctx
	{
	private:
		sdl_form* main_form = 0;
		fbo_cx* _fbo = 0;
		bw_sdl* sdl_ctx = 0;
		vk_render_cx* _vkr = 0;
		dvk_device* _dev = 0;
		queue_ctx* qctx = 0;
	public:
		gui_ctx(queue_ctx* qc);

		~gui_ctx();
		void init(bw_sdl* bw, vk_render_cx* vkr, dvk_device* dev);
		//sdl_form* new_form(const char* title, int width = 1280, int height = 720, uint32_t back_color = 0xff302d2d, sdl_form* pParent = 0);
		void push_form(sdl_form* form);

		void begin_thr();
		void set_update_cb(std::function<void(cpg_time*)> update_cb, std::function<void()>init_cb);

		void wait_end();
		void set_waitms(int ms, int minfps);
		bool loop();
	private:

	};


	//// 页面由多个div组成
	//struct page_node
	//{
	//	block_node body = {};				// body根对象
	//	t_map<t_string, block_node*> _msb;	// 所有对象的查找表
	//};
	//class block_view
	//{
	//private:
	//	t_vector<page_node*> _pages;	// 页面
	//	njson _style;					// 样式

	//public:
	//	block_view();
	//	~block_view();
	//	// 添加一个页面
	//	void push_page(page_node* p);
	//private:

	//};
	struct cursor_blink_t
	{
		int height = 0;					// 光标高
		unsigned int color = 0xffffffff; // 颜色
		double timecap = 0;				//
		int timecapi = 0;				//
		float thickness = 1.0;			// 线粗
		bool iscap = false;				// 计算是否显示
		double st = 0.5;				// 隔间秒
		glm::ivec2 pos = {};			// 显示光标的坐标
		lines_info_t _line = {};
		draw_info_t* _ud = 0;
		time_u tu;
	};
	struct background_et
	{
	};
	struct selection_et
	{
	};
	struct display_et
	{
	};
	// 行信息
	struct row_info_t
	{
		entity_t _background;		// 背景颜色rect_info_t
		entity_t _selection;		// 渲染选中效果rect_info_t
		glm::ivec2  _size;
		glm::ivec2  _pos;
	};
	struct str2_t
	{
		const char* str = 0;
		const char* str1 = 0;
	};
	struct text_area_t;


	class text_area_cx
	{
	private:
		// 文本域实体：每个文本域有n行text_area_t和一组row_info_t
		t_vector<entity_t> _area_entity;

		// 闪烁光标
		entity_t _cursor_blink_entity = {};
		bool is_cursor = true;
		// 默认选中背景色
		uint32_t selecol = 0x80ff9033;
		size_t selection1, selection2;
		std::string selection_str;

		// 选中热区判断
		t_vector<glm::vec4> _selection_hot;
		glm::ivec4 pos_first, pos_second;
		text_area_t* pta_first = 0, * pta_second = 0;
		glm::ivec2 rpos[2];
		bool is_select = false;
		bool is_dragstart = false;
		bool is_drag_me = false;
		bool is_cs = true;
		// 事件监听
		event_data_cx _listen;
		// 焦点文本域 idx < _area.size()
		//t_set<int> _hotidx;
		t_set<int> _select_hotidx;
		// 最后拾取的idx
		int lasthot = -1;

		entt::registry* _reg = 0;
		// 自动化析构
		auto_destroy_cx dc;
		// 鼠标光标
		uint8_t _sc = 0;
	public:
		text_area_cx();
		~text_area_cx();
	public:

		event_data_cx* get_elnptr() { return &_listen; }
		entity_t new_cursor_blink();

		// 增加n个<p>返回首个索引实体
		entity_t add_paragraph(size_t n);
		entity_t add_row(t_vector<entity_t>& row_entity, size_t n);

		cursor_blink_t& get_cursor_blink(entity_t e);

		rect_info_t& get_rect(entity_t e);		// 绑定矩形，比如选中效果/背景色
		void mk_row(t_vector<entity_t>& row_entity, size_t idx);
		row_info_t* get_row(t_vector<entity_t>& row_entity, size_t idx);

		void set_text(entity_t idx, css_text* csst, glm::ivec2 pos, const char* str, size_t count, size_t first, bool is_up);

		// 更新显示
		void draw_update(cpg_time* t, canvas_cx* cp);
	private:
		glm::ivec4 on_first_pos(glm::ivec2 pos, int* outlast, text_area_t** op, bool is_clear_select, bool isupcursor, bool* israng = 0);

		// 输入事件
		void on_input(event_km_t* e);
		// 键盘事件
		void on_key(event_km_t* e);

		void on_down(event_km_t* e);
		void on_up(event_km_t* e);

		void on_enter(event_km_t* e);
		void on_dragstart(event_km_t* e);
		void on_ole_drop(event_km_t* e);
		void on_ole_drag(event_km_t* e);
		void on_ole_dragover(event_km_t* e);
		void on_leave(event_km_t* e);
		void on_move(event_km_t* e);
		// 判断是否在选中的文本里
		bool in_selection(glm::ivec2 pos);

		bool is_range(glm::ivec2 pos);

	};
	class edit_es_t;
	class buffer_t;
	struct text_view_t;
	namespace text_event_t
	{
		edit_es_t* make_event_cb(event_data_cx* listen, item_box_info* p, buffer_t* buf, text_view_t* tvt);
		void set_rect(edit_es_t* p, glm::ivec4 v4, std::function<void(edit_es_t*, bool is_input, bool is_editing)> upcb);
		void set_ctx(edit_es_t* p, bw_sdl* ctx);
		void set_ick_ibeam(edit_es_t* p, int* pick, bool is_ibeam);
		void set_read_only(edit_es_t* p, bool is);
		void set_no_event(edit_es_t* p, bool is);
	}
	namespace trt
	{
		void update(cpg_time* t, item_box_info* p);
		void draw_ptext(cpg_time* t, canvas_cx* cp, item_box_info* p);
		void set_color(item_box_info* p, uint32_t text_color, uint32_t select_color, uint32_t cursor_color);
	}

	/*
		todo 列表事件

	*/
	struct list_info_st
	{
		// horizontal/vertical水平1/垂直0
		int type = 0;
		// 大小
		glm::ivec2 size;
		// 宽度
		int weight;
	};
	class list_event_t
	{
	private:
		// 事件数据
		event_data_cx edc;
		list_info_st info;
	public:
		list_event_t();
		~list_event_t();
	public:
		void set_pos(glm::ivec2 pos);
		// horizontal/vertical水平1/垂直0
		void set_type(int t);
		// 设置大小
		void set_size(int w, int h);
		// 设置单元高/宽度
		void set_weight(int w);
	private:

	};
	// todo *为需要自己绑定的属性，也是可选的
	// 有hot_et就是可见
	// struct visible_et {};
	// *热区信息
	struct hot_et
	{
		glm::vec4 rc;			// 矩形xyzw、圆xyz w=0; 用户设置或布局算法设置
		glm::ivec2 pos;			// 坐标；一般由布局算法设置
		glm::vec4* last = 0;	// 多个组合时，数组自行管理内存
		int n = 0;				// last数量
		int order = 0;			// 排序
	};
	namespace ui {
		class base_u;
		class div_u;
	}
	struct div_event_et
	{
		ui::div_u* p = 0;
		int (*mouse_motion)(const glm::ivec2& p, ui::div_u* d, bool ts) = 0;
	};
	// 鼠标进入的对象
	struct hover_et
	{
		ui::base_u* p = 0;
		glm::ivec2 pos1 = {};		// 鼠标减去hot_spot_et坐标
		int idx = 0;				// 多个热区的idx
		int inc = 0;				// 执行次数
	};
	struct button_et
	{
		int idx = -1;		// 鼠标
		int button = 0;		// 1按下，0释放
		int pidx = 0;		// 点中索引，0就是默认的，1就是hot_et last开头
		glm::ivec2 pos = {};
		int inc = 0;
	};
	// 激活的对象
	struct active_et {};
	// 按下状态
	struct down_et {};
	// 单击状态
	struct click_et
	{
		int idx = -1;	// 单击时鼠标左右中
		int n = 0;		// 单击、双击三击
	};
	// 用户单击状态
	struct click_ut {};
	// *绑定单击事件
	struct click_cb_et
	{
		void* ptr = 0;
		std::function<void(void* ptr, int idx, int count)> cb;
	};
	struct click_cb1_et
	{
		void* ptr = 0;
		void(*cb)(void* ptr, int idx, int count) = nullptr;
	};
	// *绑定拖动事件
	struct drag_cb_et
	{
		void* ptr = 0;
		std::function<void(void* ptr, const glm::ivec2& pos, const glm::ivec2& pos1, int state)> cb;
	};
	struct drag_cb1_et
	{
		void* ptr = 0;
		void (*cb)(void* ptr, const glm::ivec2& pos, const glm::ivec2& pos1, int state) = nullptr;
	};
	struct drag_ut
	{
		glm::ivec2 pos = {}, pos1 = {}, last = { -1,-1 }; int state = 0;
	};
	struct scroll_cb1_et
	{
		void* ptr = 0;
		void (*cb)(void* ptr, int wheel, int wheelH) = nullptr;
	};


	struct div_text_info_e
	{
		layout_text* lt = 0;
		item_box_info* ptext = 0;
	};
	struct event_src_et {
		event_master* p = 0;
		layout_text* lt = 0;
		event_data_cx* edc = 0;
		sdl_form* form = 0;
		bw_sdl* ctx = 0;
	};
	/*
		三个渲染不同顺序
		如有多个则在shape_n* pNext
	*/
	struct uibs_dt
	{
		shape_n* first = 0;
		shape_n* second = 0;
		shape_n* last = 0;
	};

	namespace ecs_at {
		// 创建实体绑定hot_spot_et/button_et
		entt::entity new_em(entt::registry* reg, const glm::vec4& rc, const glm::ivec2& pos);
		// 创建n个实体，o要有n个空间，默认绑定hot_spot_et/button_et
		entt::entity* new_em(entt::registry* reg, entt::entity* o, int n, bool bind_hb = true);
		// 删除n个实体
		void free_em(entt::registry* reg, entt::entity* e, int n);
		button_et* get_btn_ptr(entt::registry* reg, entt::entity e);
		// 获取组件（数据）指针，没有则自动创建
		template<class T>
		inline T* get_ptr(entt::registry* reg, entt::entity e, T*& ret)
		{
			assert(reg);
			T* p = 0;
			p = (reg->has<T>(e)) ? &reg->get<T>(e) : &reg->emplace<T>(e);
			ret = p;
			return p;
		}
		// 获取组件指针，没有不会创建
		template<class T>
		inline T* get_ptr0(entt::registry* reg, entt::entity e, T*& ret)
		{
			assert(reg);
			T* p = 0;
			p = (reg->has<T>(e)) ? &reg->get<T>(e) : 0;
			ret = p;
			return p;
		}
		template<class T>
		inline T* get_p(entt::registry* reg, entt::entity e)
		{
			assert(reg);
			T* p = 0;
			p = (reg->has<T>(e)) ? &reg->get<T>(e) : &reg->emplace<T>(e);
			return p;
		}
		template<class T>
		inline T* get_p0(entt::registry* reg, entt::entity e)
		{
			assert(reg);
			T* p = 0;
			p = (reg->has<T>(e)) ? &reg->get<T>(e) : 0;
			return p;
		}
		template<class T>
		inline T* get_ptr(entt::registry& reg, entt::entity e, T*& ret)
		{
			return get_ptr(&reg, e, ret);
		}
		// 设置/删除一个属性
		template<class T>
		void set_entity(entt::registry* reg, entt::entity e, bool is)
		{
			assert(reg);
			if (is)
			{
				if (!reg->has<T>(e))
					reg->emplace<T>(e);
			}
			else
			{
				reg->remove_if_exists<T>(e);
			}
			return;
		}
		template<class T>
		void set_entity(entt::registry& reg, entt::entity e, bool is)
		{
			set_entity<T>(&reg, e, is);
		}

		template<class T>
		inline bool get_remove(entt::registry* reg, entt::entity e, T* r)
		{
			T* p = (reg->has<T>(e)) ? &reg->get<T>(e) : nullptr;
			if (p)
			{
				if (p->n)
					reg->remove<T>(e);
				if (r)
				{
					*r = *p;
				}
			}
			return p;
		}
		void sort_reg(entt::registry* reg);
		//void set_visible(entt::registry* reg, entt::entity e, bool is);
		//bool get_visible(entt::registry* reg, entt::entity e);

		hot_et* get_hotptr(entt::registry* reg, entt::entity e);
		hover_et* get_hover_p(entt::registry* reg, entt::entity e);
	}
	namespace em_system {
		// todo 1 直接绑定固定处理
		void bind_listen(event_data_cx* ld, entt::registry* reg);
		// todo 2 轮循
		// 更新鼠标移动
		int update_move(entt::registry* reg, const glm::ivec2& pos, bool ts);

		// 需要hover_et才触发，down会激活对象
		int update_on_down(entt::registry* reg, int idx);
		int update_on_up(entt::registry* reg, int idx);
		int update_on_click(entt::registry* reg, int idx, int count);
		int update_on_hover(entt::registry* reg, const glm::ivec2& pos);
		int update_on_enter2leave(entt::registry* reg, const glm::ivec2& pos, bool is);
		// 需要active_et

		void update_on_scroll(entt::registry* reg, int wheel, int wheelH);
		void update_on_key(entt::registry* reg, event_km_t* e);
		void update_on_input(entt::registry* reg, const char* str, int len);
		int update_drag(entt::registry* reg, const glm::ivec2& pos, const glm::ivec2& pos1, int state);

		void draw_ui(canvas_cx* cp, shape_n* p, const glm::vec2& pos1);
	}
	namespace em_render {
		void draw_item(entt::registry* reg, entt::entity e, cpg_time* t, canvas_cx* cp, const glm::ivec2& pos, font_style_c* ft_style);

	}

	/*			flex_size_t* size = 0;
	flex_rect_t* rect = 0;
	flex_padding_t* padding = 0;
	flex_margin_t* margin = 0;
	flex_align_t* align = 0;
	flex_wrap_t* wrap = 0;
	flex_base_t* base = 0;
	item_ptr_t本体

		 align_space_between两端对齐，两端间隔0，中间间隔1
		 align_space_around分散居中,两端间隔0.5，中间间隔1
		 align_space_evenly分散居中,每个间隔1
	*/
	// flex ecs
	class flex_system
	{
	public:
		entt::registry* reg = 0;
		attribute_func* acb = 0;	// 设置flex::item属性操作函数
	public:
		flex_system(entt::registry* r);
		~flex_system();
	public:
		// 创建布局实体，空参数则创建一个，带返回空间则创建n个
		entity new_entity(entity* result = 0, int n = 0);
		// 释放实体
		void free_entity(entity* result, int n);
		// 输入实体，创建布局句柄
		flex::item_ptr_t* new_item(entity e);
		void free_item(entity e);
		// 实体e添加子元素c
		void item_add(entity e, entity c);
		// e从父级脱离
		void item_detach(entity e);
		// 把实体属性更新到item句柄
		void item_make(entity e);
		// 计算布局
		void do_layout(entity e);
		// 获取布局计算完的坐标、大小
		glm::vec4 get_merge(entity e);

		// 设置大小
		void set_size(entity e, const glm::vec2& s);
		// 设置左右上下位置
		void set_rect_pos(entity e, const glm::vec4& s);
		// 内边距
		void set_padding(entity e, const glm::vec4& s);
		// 外边距
		void set_margin(entity e, const glm::vec4& s);
		// 对齐
		void set_align(entity e, flex::e_align justify_content = flex::e_align::align_start	// 横向，root不能是 FLEX_ALIGN_STRETCH
			, flex::e_align align_content = flex::e_align::align_stretch	// 纵向
			, flex::e_align align_items = flex::e_align::align_stretch	// 孩子纵轴（上下）FLEX_ALIGN_START、FLEX_ALIGN_STRETCH、FLEX_ALIGN_CENTER、FLEX_ALIGN_END
			, flex::e_align align_self = flex::e_align::align_auto		// 相当重载align_items，FLEX_ALIGN_AUTO、
		);
		void set_align_justify_content(entity e, flex::e_align justify_content = flex::e_align::align_start);
		void set_align_align_content(entity e, flex::e_align align_content = flex::e_align::align_stretch);
		void set_align_align_items(entity e, flex::e_align align_items = flex::e_align::align_stretch);
		void set_align_align_self(entity e, flex::e_align align_self = flex::e_align::align_auto);
		void set_align(entity e, const glm::ivec4& s);

		void set_wraps(entity e, flex::e_position position = flex::e_position::relative		// 相对坐标、绝对坐标
			, flex::e_direction direction = flex::e_direction::row		// 排列方向
			, flex::e_wrap wrap = flex::e_wrap::no_wrap					// 是否弹性FLEX_WRAP_WRAP;
		);
		void set_position(entity e, flex::e_position position = flex::e_position::relative);
		void set_direction(entity e, flex::e_direction direction = flex::e_direction::row);
		void set_wrap(entity e, flex::e_wrap wrap = flex::e_wrap::no_wrap);
		void set_wraps(entity e, const glm::ivec3& s);

		void set_base(entity e
			, float grow = 0.0											// 弹性盒子的扩展比率，如果不是弹性盒子则无效
			, float shrink = 1.0											// 空间不够时各元素如何收缩
			, float order = 0												// 排序号
			, float basis = NAN											// 指定item在主轴方向上的初始大小,优先级比width高，设置或检索弹性盒伸缩基准值
		);
		void set_base(entity e, const glm::vec4& s);

	private:

	};


	/*
	fbo封装
	视图大小、图元信息、执行输出纹理或图像
	*/
	class fbo_m
	{
	private:
		fbo_cx* fbo1 = 0;
		page_view* _page = 0;		// 管理vbo\ibo\ubo等数据
		dvk_queue* dq = 0;			// 设备列队
		dvk_fence* _fence = 0;		// 同步
		submit_infos* psub = 0;		// 提交执行信息

		canvas_cx* canvas = 0;
	public:
		fbo_m(world_cx* wdc, int queueidx);
		~fbo_m();
		operator canvas_cx* () { assert(canvas); return canvas; }
	public:
		// 设置视图大小
		void set_view(glm::ivec2 size, unsigned int color);
		canvas_cx* g_begin();
		void g_end();
		// 设置图元信息
		void add_data(const void* p, size_t n, size_t stride = 0, size_t offset = 0);
		// 提交执行
		void submit();

		void to_image(Image* img, int idx);
		void save_image(const std::string& fn);
		void set_unm(bool is);
	private:

	};
	/*

	int test_fbo(world_cx* wdc)
	{
	std::unique_ptr<fbo_m> fbo(new fbo_m(wdc, 1));
	fbo->set_view({512, 512});
	canvas_cx* canvas = *fbo;
	fbo->g_begin();
	// 添加图元
	canvas->draw_circle({100, 100}, 36, 0, 0x80ff5000, 36, 2);
	canvas->draw_circle({126, 100}, 36, 0, 0x800000ff, 36, 2);
	fbo->g_end();

	{
	print_time mt("fbo draw");
	fbo->submit();
	}
	{
	print_time mt("fbo save");
	fbo->save_image("temp/fbotest_");
	}
	return 0;
	}

	*/


}
// !hz
