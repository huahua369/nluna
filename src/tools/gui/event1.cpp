
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
		
		// 命中集合
		uint64_t _hotidx = 0;
		bool* pvisible = 0;
		bool is_input = false;	// 是否可以输入
		bool visible = true;
		bool _is_hot = false;
	private:
		// 热区判断
		t_vector<glm::vec4> _hot;	// 热区

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
		sem_d* _sem = 0;
	public:
		event_data_cx();
		~event_data_cx();
		static void run_thr(event_data_cx* p);
	public:
		void push_next(event_data_cx* p);
		void pop(event_data_cx* p);
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
		//int  get_hot_count();
	public:
		// 注册事件改变数据，value改变时需要重新注册
		void set_cd(event_type t, uint64_t* result, uint64_t value);
		// rc矩形4、圆3
		void set_cd(event_type t, uint64_t* result, uint64_t value, const glm::vec4& rc);

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
		bool has_input();
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
		void mket(event_type t);
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
		int nk = 0;
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
		event_data_cx* get_hotnode();
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

#if 1

	event_data_cx::event_data_cx()
	{
		_maskcb.resize((int)event_type::et_max_num);
		_sem = new sem_d();
	}

	event_data_cx::~event_data_cx()
	{
		if (_em)
		{
			_em->pop(this);
		}
#ifdef _WIN32
		delop(_oledrop);
#endif
		delop(_sem);
	}

	void event_data_cx::run_thr(event_data_cx* p)
	{
		while (p) { p->at_run(false); p = p->next; }
	}


	void event_data_cx::mket(event_type t)
	{
		switch (t)
		{
		case event_type::on_scroll:
			is_wheel = true;
			break;
		case event_type::on_drag:
		case event_type::on_dragstart:
		case event_type::on_dragend:
			is_drag = true;
			break;
		case event_type::on_dragenter:
		case event_type::on_dragleave:
		case event_type::on_dragover:
		case event_type::on_drop:
			is_drop = true;
			break;
		case event_type::on_ole_drag:
			is_oledrag = true;
			break;
		case event_type::on_ole_drop:
			mk_oledrop();
			break;
		default:
			break;
		}
		_maskcb[(int)t] = true;
	}
	void event_data_cx::set_cd(event_type t, uint64_t* result, uint64_t value)
	{
		auto& rt = _on_data[(uint32_t)t];
		rt[result] = value;
		_maskcb[(int)t] = true;
	}
	void event_data_cx::set_cd(event_type t, uint64_t* result, uint64_t vec4, const glm::vec4& rc)
	{
	}
	void event_data_cx::mk_oledrop()
	{
#ifdef _WIN32
		if (!_oledrop)
		{
			_oledrop = new drop_info_cx();
			_oledrop->on_drop_cb = [=](int type, int idx)
			{
				auto& it = _on_cbs[(int)event_type::on_ole_drop];
				for (auto& cb : it)
				{
					if (cb)	cb(0);
				}
			};
		}
#endif
	}
	std::string event_data_cx::get_dropstr()
	{
		std::string ret;
		if (_oledrop)
		{
			ret = _oledrop->_str;
		}
		return ret;
	}
	void event_data_cx::set_dragable(bool is)
	{
		is_auto_drag = is;
	}
	bool event_data_cx::get_dragable()
	{
		return is_auto_drag;
	}
	void event_data_cx::set_v4(size_t idx, const glm::vec4& c, bool is)
	{
		//auto_lock ak(&_mtx);
		int n = idx;
		if (n >= 0)
		{
			if (n >= _hot.size())
			{
				_hot.resize(n + 1);
			}
			_hot[n] = c;
			lidx = -1;
		}
	}
	glm::vec4 event_data_cx::get_v4(size_t idx)
	{
		if (idx == lidx)
		{
			return lv4;
		}
		//auto_lock ak(&_mtx);
		if (_hot.empty() || idx >= _hot.size())
			return glm::vec4();
		lv4 = _hot[idx]; lidx = idx;
		return lv4;
	}
	void event_data_cx::clear_hot()
	{
		_hot.clear();
	}
	size_t event_data_cx::add_rect(const glm::vec4& c)
	{
		size_t n = _hot.size();
		_hot.push_back(c);
		return n;
	}
	size_t event_data_cx::add_circle(const glm::vec4& c)
	{
		size_t n = _hot.size();
		auto c1 = c;
		c1.w = 0;
		_hot.push_back(c1);
		return n;
	}
	void event_data_cx::at_run(bool is_one)
	{
		if (!ay_tid)
			ay_tid = get_tid();
		for (; exe_cbs.size();)
		{
			vcb_t c = {};
			{
				auto_lock ak(&_mtx);
				c = exe_cbs.front();
				exe_cbs.pop();
			}
			if (c)
			{
				c();
			}
			if (is_one)
				break;
		}
		for (; event_runcb.size();)
		{
			event_km_t e = {};
			{
				auto_lock ak(&_mtx);
				e = event_runcb.front();
				event_runcb.pop();
			}
			call_acb(&e, true);
			if (is_one)
				break;
		}
	}
	size_t event_data_cx::size()
	{
		return _hot.size();
	}
	int event_data_cx::get_begin()
	{
		auto_lock ak(&_mtx);
		return _hotidx & 1;
	}
	int event_data_cx::get_rbegin()
	{
		auto_lock ak(&_mtx);
		if (_hot.empty())return -1;
		uint64_t n = _hot.size() - 1;
		return _hotidx & HZ_BIT(n);
	}
	void event_data_cx::post_vcb(vcb_t cb)
	{
		auto_lock ak(&_mtx);
		exe_cbs.push(cb);
	}
	void event_data_cx::send_vcb(vcb_t cb)
	{
		auto t = get_tid();
		if (t != ay_tid)
		{
			{
				auto_lock ak(&_mtx);
				exe_cbs.push([&]() { cb(); _sem->post(); });
			}
			_sem->wait();
		}
		else
		{
			cb();
		}

	}
	void event_data_cx::push_event(event_km_t* e)
	{
		auto_lock ak(&_mtx);
		event_runcb.push(*e);
	}
	void event_data_cx::send_event(event_km_t* e)
	{
		auto t = get_tid();
		bool isret = true;
		if (t != ay_tid)
		{
			auto_lock ak(&_mtx);
			exe_cbs.push([&]() { call_acb(e); isret = false; });
		}
		else
		{
			call_acb(e);
			isret = false;
		}
		while (isret)
		{
			sleep(1);
		}
	}
	bool event_data_cx::is_atexe(event_type_t e)
	{
		event_type_t ne = event_type::on_ole_drag;
		return is_athread && e < ne;
	}
	void event_data_cx::call_cb(event_km_t* e, bool is_data)
	{
		if (!(e->etype < _maskcb.size() && _maskcb[e->etype]))
		{
			return;
		}
		bool isa = is_atexe(e->etype);
		if (isa)
		{
			if (e->form)
			{
				push_event(e);	// 设置事件在自定义线程执行			
			}
			else {
				send_event(e);
			}
		}
		else {
			call_acb(e, is_data);
		}
	}
	void event_data_cx::call_acb(event_km_t* e, bool is_data)
	{
		// 执行回调函数
		const auto& it = _on_cbs.find(e->etype);
		if (it != _on_cbs.end())
		{
			for (auto& cb : it->second)
				if (cb)cb(e);
		}
		// 更新数据
		if (is_data && e->etype >= (uint32_t)event_type::on_click)
		{
			auto dt = _on_data.find(e->etype);
			if (dt != _on_data.end())
			{
				for (auto& [k, v] : dt->second)
				{
					*k = v;
				}
				if (dt->second.size())
				{
					_is_upcd = true;
				}
			}
			if (e->_bubble && _parent)
				_parent->call_cb(e, is_data);
		}
	}

	bool event_data_cx::is_upcd()
	{
		bool ret = _is_upcd;
		_is_upcd = false;
		return ret;
	}


	bool event_data_cx::is_click()
	{
		return is_type2on(event_type::on_click);
	}

	bool event_data_cx::ishot()
	{
		return curr_drag || _is_hot;
	}
	void event_data_cx::set_drag_data(void* data, size_t size)
	{
		_drag_data = data;
		_drag_size = size;
	}
	bool event_data_cx::has_input()
	{
		return (_maskcb.size() && _maskcb[(int)event_type::on_input]) && is_type2on(event_type::on_input);
	}
	bool event_data_cx::is_dblclick()
	{
		return is_type2on(event_type::on_click);
	}


	bool event_data_cx::is_tripleclick()
	{
		return is_type2on(event_type::on_click);
	}
	bool event_data_cx::is_type2on(event_type et)
	{
		auto it = _on_cbs.find((uint32_t)et);
		return (it != _on_cbs.end());
	}

	void event_data_cx::on_scroll(int x, int y, event_km_t* e)
	{
		if (is_wheel && is_type2on(event_type::on_scroll))
		{
			if (y < 0)
			{
				wheel_info._nWheel += wheel_info._step;
				if (wheel_info._nWheel > wheel_info._maxW)
				{
					wheel_info._nWheel = wheel_info._maxW;
				}
			}
			else  if (y > 0)
			{
				wheel_info._nWheel -= wheel_info._step;
				if (wheel_info._nWheel < wheel_info._minW)
				{
					wheel_info._nWheel = wheel_info._minW;
				}
			}
			auto te = *e;
			te.etype = (uint32_t)event_type::on_scroll;
			call_cb(&te);
		}
	}
	bool event_data_cx::is_contains(glm::ivec2 pos)
	{
		if (cm_pos == pos) {
			//printf("重复%p\t%d\n", this, _is_hot);
			//return false;
		}
		if (!enabled)return false;
		cm_pos = pos;
		pos -= node_pos;	//格式化为组件坐标
		auto_lock ak(&_mtx);
		static int ka = 0;
		if (_hot.size() > ka)
		{
			ka = _hot.size();
			printf("max hot:\t%d\n", ka);
		}
		_hotidx = 0;
		auto n = math_cx::inData(_hot.data(), _hot.size(), pos, &_hotidx, 1);
		_is_hot = n > 0;
		return n > 0;
	}

	// --------------------------------------------------------------------------------------------
	event_master::event_master()
	{
#ifdef _WIN32
		_dbt = GetDoubleClickTime();//获取系统双击间隔时间
#endif // _WIN32

		_hover_act = new timer_cx(_hover_ms, [](event_master* t)
			{
				if (t && t->_hotnode)
				{
					auto tem = t->te;
					tem.etype = (int)event_type::on_hover;
					t->_hotnode->call_cb(&tem);
					t->_hover_act->stop();
				}
			}, this);
	}

	event_master::~event_master()
	{
		delop(_hover_act);
	}
	void event_master::set_bwctx(bw_sdl* p)
	{
		assert(_bw_ctx != p);
		_bw_ctx = p;
		if (p)
		{
			//_bw_ctx->push_timer(_hover_act);
		}
	}
	bw_sdl* event_master::get_bwctx()
	{
		return _bw_ctx;
	}
	void event_master::push(event_data_cx* p)
	{
		if (p)
		{
			if (p->_em)
				p->_em->pop(p);
			auto_lock ak(&_mtx);
			_tem_ar.push({ p,true });
		}
	}
	void event_master::pop(event_data_cx* p)
	{
		if (p)
		{
			auto_lock ak(&_mtx);
			_tem_ar.push({ p, false });
			if (_keynode == p)
			{
				_keynode = 0;
			}
			if (_lasthotnode == p)
			{
				_lasthotnode = 0;
			}
			if (_dragnode == p)
			{
				_dragnode = 0;
			}
			p->_em = 0;
		}
	}
	void event_master::up_sort()
	{
		sort_inc++;
	}
	void event_master::run_newdel()
	{
		if (_tem_ar.size())
		{
			sort_inc++;
			auto_lock ak(&_mtx);
			for (; _tem_ar.size();) {
				auto it = _tem_ar.front();
				auto np = it.p;
				if (it.ispush) {
					if (np->_oledrop)
						nodes_drop.insert(np);
					nodes.push_back(np);
					np->_em = this;
				}
				else
				{
					if (np == _hotnode)
						_hotnode = 0;
					if (np == _lasthotnode)
						_lasthotnode = 0;
					if (np == _olehotnode)
						_olehotnode = 0;
					if (np == _keynode)
						_keynode = 0;
					nodes.erase(remove(nodes.begin(), nodes.end(), np), nodes.end());
					nodes_drop.erase(np);
				}
				_tem_ar.pop();
			}

		}

		if (sort_inc != 0 && nodes.size())
		{
			sort_inc = 0;
			std::stable_sort(nodes.begin(), nodes.end(), [](event_data_cx* p1, event_data_cx* p2) { return (p1->order < p2->order); });
		}
	}
	// 设置视图区域

	void event_master::set_viewport(glm::ivec4 viewport)
	{
		_viewport = viewport;
	}

#define VMK_UP 			HZ_BIT(0) //鼠标左键弹起
#define VMK_DOWN	 	HZ_BIT(1) //按下
#define VMK_MOVE 		HZ_BIT(2) //移动
#define VMK_ENTER 		HZ_BIT(3) //进入
#define VMK_LEAVE 		HZ_BIT(4) //离开
#define VMK_FOCUS 		HZ_BIT(5) //焦点
#define VMK_DISABLE 	HZ_BIT(6) //禁用
#define VMK_LBUTTON 	HZ_BIT(7) //鼠标左键
#define VMK_RBUTTON 	HZ_BIT(8) //鼠标右键
#define VMK_CLICK 		HZ_BIT(9) //单击
#define VMK_DBLCLK 		HZ_BIT(10) //双击
#define VMK_RDOWN 		HZ_BIT(11) //右键按下
#define VMK_RUP 		HZ_BIT(12) //右键弹起

	drop_info_cx* event_master::call_mouse_move_oledrop(int x, int y)
	{
		x -= _viewport.x;
		y -= _viewport.y;
		te.pos = { x, y };
		t_vector<event_data_cx*> n_hotnode;
		for (auto it : nodes_drop)
		{
			if (it)
			{
				if (it->is_contains({ x, y }))
				{
					n_hotnode.push_back(it);
				}
			}
		}
		drop_info_cx* ret = 0;
		if (n_hotnode.size())
		{
			_hover_act->runcb();
			ret = n_hotnode[0]->_oledrop;
			_olehotnode = n_hotnode[0];
			// 热区内移动
			auto tem = te;
			tem.etype = (int)event_type::on_ole_dragover;
			_olehotnode->call_cb(&tem);
			if (!tem.user_ret)
			{
				ret = 0;
			}
			//printf("hot move %d\n", (int)n_hotnode.size());
		}
		return ret;
	}
	bool event_master::is_contains_hit(glm::vec2 pos)
	{
		event_km_t ekm[1] = {};
		ekm->etype = (uint32_t)event_type::mouse_move;
		bool ret = call_mouse_move(pos.x, pos.y, 0);
		return math_cx::inRect(_viewport, pos, &pos) && ret;
	}
	event_data_cx* event_master::get_hotnode()
	{
		return _hotnode;
	}
	bool event_master::is_contains(glm::vec2 pos)
	{
		bool isin = math_cx::inRect(_viewport, pos, &pos);
		if (!isin)
		{
			_hover_act->stop();
			if (_lasthotnode)
			{
				if (_lasthotnode->_nMouseState & VMK_DOWN)
				{
					return true;
				}
				else {
					onMouseLeave(_lasthotnode, 0);
					_lasthotnode = 0;
				}
			}
		}
		return isin;
	}
	// 鼠标进入才会执行
	bool event_master::call_mouse_move(int x, int y, event_km_t* e)
	{
		run_newdel();
		t_vector<event_data_cx*> n_hotnode;
		glm::ivec2 pos = { x - _viewport.x, y - _viewport.y };
		if (e)
		{
			e->pos = pos;
			e->pos1 = pos;
			te = *e;
		}
		if (pos != last_mouse)
		{
			last_mouse = pos;
		}
		int mcc = 0;
		n_hotnode.clear();
		do {
			for (auto rit = nodes.rbegin(); rit != nodes.rend(); rit++)
			{
				auto it = *rit;
				bool ismc = false;
				if (it && it->visible && (!it->pvisible || *(it->pvisible)))
				{
					if (it->is_contains({ pos.x, pos.y }))
					{
						n_hotnode.push_back(it);
						if (e)
							it->call_cb(e);
						ismc = true;
						mcc += it->mchild_count;
					}
					if (it->mchild_count > 0)
					{
						hover_vn.insert(it);
					}
					else
					{
						hover_vn.erase(it);
					}
					if (mcc > 0)
					{
						break;
					}
				}
				else {
					_hotnode = _hotnode;
				}
			}
			if (n_hotnode.size())
				_hotnode = n_hotnode[0];
			else
				_hotnode = 0;

		} while (0);
		if (e)
			onMouseMove(pos.x, pos.y);
		return (_hotnode /*&& _hotnode->pridata->has_event*/) || mcc > 0;
	}


	void event_master::call_mouse_down(int idx, int x, int y, event_km_t* e)
	{
		run_newdel();
		x -= _viewport.x;
		y -= _viewport.y;
		e->pos1 = e->pos = { x, y };
		for (auto rit = nodes.rbegin(); rit != nodes.rend(); rit++)
		{
			auto it = *rit;
			if (it)
			{
				it->call_cb(e);
			}
		}
		onMouseDown(idx, x, y);
	}

	void event_master::call_mouse_up(int idx, int x, int y, event_km_t* e)
	{
		run_newdel();
		//printf("up:%p", e->p);
		x -= _viewport.x;
		y -= _viewport.y;
		e->pos1 = e->pos = { x, y };
		for (auto rit = nodes.rbegin(); rit != nodes.rend(); rit++)
		{
			auto it = *rit;
			if (it)
			{
				it->call_cb(e);
			}
		}
		onMouseUp(idx, x, y);
	}

	void event_master::call_mouse_wheel(int x, int y, event_km_t* e)
	{
		run_newdel();
		for (auto rit = nodes.rbegin(); rit != nodes.rend(); rit++)
		{
			auto it = *rit;
			if (it)
			{
				it->call_cb(e);
			}
		}
		if (_hotnode)
		{
			_hotnode->on_scroll(x, y, e);
		}
	}

	void event_master::call_key(event_km_t* e)
	{
		if (_keynode)
		{
			auto tem = *e;
			_keynode->call_cb(&tem);
		}
	}

	void event_master::call_input(event_km_t* e)
	{
		if (_keynode)
		{
			auto tem = *e;
			_keynode->call_cb(&tem);
		}
	}


	glm::ivec4 drag_info_t::re_rect()
	{
		glm::ivec4& ret = _rect;
		ret.x = std::min(_pos_down.x, _pos_end.x);
		ret.y = std::min(_pos_down.y, _pos_end.y);
		ret.z = std::max(_pos_down.x, _pos_end.x);
		ret.w = std::max(_pos_down.y, _pos_end.y);
		return ret;
	}
	// x,y鼠标坐标，pos对象坐标
	void drag_info_t::set_drag_begin(int x, int y, const glm::ivec2& pos)
	{
		_pos_down = _firstpos = { x - pos.x, y - pos.y };
	}
	void drag_info_t::set_drag_end(int x, int y, const glm::ivec2& pos)
	{
		_pos_end = { x - pos.x, y - pos.y };
		re_rect();
	}


	void event_master::on_oledrag(event_data_cx* node)
	{
		auto tem = te;
		if (node->is_oledrag && node->curr_oledrag == 1 && node->_is_hot)
		{
			node->curr_oledrag++;
			tem.etype = (int)event_type::on_ole_drag;
			node->call_cb(&tem);//执行拖动处理
			//node->_nMouseState = VMK_UP;
		}
	}
	void event_master::on_drag(int x, int y)
	{
		if (!_dragnode)return;
		//printf("on_drag: %d\n", _dragnode->child_count);
		auto tem = te;
		if (_dragnode->curr_drag > 0)
		{
			glm::vec2 pos = { x - drag_info._firstpos.x, y - drag_info._firstpos.y };
			drag_info.set_drag_end(x, y, _dragnode->node_pos);
			tem.pos1 = pos;
			if (_dragnode->curr_drag == 1)
			{
				tem.etype = (int)event_type::on_dragstart;
				drag_info.first_pos = pos;
				_dragnode->call_cb(&tem);//执行拖动开始
			}
			_dragnode->curr_drag++;
			tem.etype = (int)event_type::on_drag;
			if (!_dragnode->child_count && _dragnode->is_auto_drag)
			{
				_dragnode->node_pos = pos;
				//printf("on_drag: %d,%d\t%d,%d\n", x, y, (int)pos.x, (int)pos.y);
			}
			tem.pos2 = drag_info.first_pos;
			_dragnode->call_cb(&tem);//执行拖动处理
		}
	}
	void event_master::onMouseMove(int x, int y)
	{
		auto tem = te;
		on_drag(x, y);
		if (_hotnode)
		{
			_hotnode->_nMouseState |= VMK_MOVE;
			if (_hotnode->_nMouseState & VMK_DOWN && te.mouse_idx == 0)
			{
				on_oledrag(_hotnode);
			}
			if (_lasthotnode != _hotnode)
			{
				if (_lasthotnode && !_lasthotnode->ishot())
				{
					onMouseLeave(_lasthotnode, event_type::on_move);
				}
				_lasthotnode = _hotnode;
				onMouseEnter(_hotnode);
				// 有悬停事件则开始定时器
				if (_hotnode->is_type2on(event_type::on_hover))
					_hover_act->start();
			}
			else
			{
				// 热区内移动
				tem.etype = (int)event_type::on_move;
				_hotnode->call_cb(&tem);
				if (_dragnode && _hotnode != _dragnode)
				{
					tem.etype = (int)event_type::on_dragover;
					_hotnode->call_cb(&tem);
				}
			}
		}
		else
		{

			_hover_act->stop();
			if (_lasthotnode)
			{
				auto hp = _lasthotnode;
				if ((hp->_nMouseState & VMK_DOWN))
				{
					// 鼠标按下时也会触发热区内移动
					tem.etype = (int)event_type::on_move;
					hp->call_cb(&tem);
				}
				else {
					_lasthotnode = 0;
				}
				onMouseLeave(hp, event_type::mouse_move);
			}
		}
	}

	void event_master::onMouseEnter(event_data_cx* node)
	{
		node->_nMouseState &= ~VMK_LEAVE;

		node->_nMouseState |= VMK_ENTER;
		auto tem = te;
		tem.etype = (int)event_type::on_enter;
		node->call_cb(&tem, !(node->_nMouseState & VMK_DOWN));
		if (_dragnode && node != _dragnode)
		{
			tem.etype = (int)event_type::on_dragenter;
			node->call_cb(&tem);
		}
	}
	void event_master::onMouseLeave(event_data_cx* node, event_type_t t)
	{
		if (node->_nMouseState & VMK_DOWN)
		{
			//printf("%p,down\t%d\n", node, (int)t);
		}
		if (!(node->_nMouseState & VMK_ENTER) || (node->_nMouseState & VMK_DOWN))
		{
			return;
		}
		//printf("%p,onMouseLeave\n", node);
		node->_nMouseState &= ~VMK_ENTER;
		node->_nMouseState &= ~VMK_MOVE;
		node->_nMouseState |= VMK_LEAVE;
		//执行事件
		auto tem = te;
		tem.etype = (int)event_type::on_leave;
		node->call_cb(&tem, !(node->_nMouseState & VMK_DOWN));

		if (_dragnode && node != _dragnode)
		{
			tem.etype = (int)event_type::on_dragleave;
			node->call_cb(&tem);
		}
		node->_nMouseState &= ~VMK_DOWN;
		node->_nMouseState &= ~VMK_RDOWN;
		node->_nMouseState |= VMK_UP;
	}

	void event_master::onMouseDown(int idx, int x, int y)
	{
		if (!_hotnode)return;
		if (_hotnode->_is_hot)
		{
			_hotnode->_nMouseState |= VMK_DOWN;
			_hotnode->_nMouseState &= ~VMK_UP;
			//执行事件
			te.mouse_idx = idx;
			auto tem = te;
			tem.etype = (int)event_type::on_down;
			_hotnode->call_cb(&tem);
			if (idx == 0)
			{
				if (_hotnode->is_drag || _hotnode->is_auto_drag)
				{
					_hotnode->curr_drag = 1;
					_dragnode = _hotnode;
					drag_info.set_drag_begin(x, y, _hotnode->node_pos);
				}
				if (_hotnode->is_oledrag)
				{
					_hotnode->curr_oledrag = 1;
				}
				if (te.form && _hotnode->pridata)
				{
					te.form->set_now_ui(_hotnode->pridata);
				}
				if (_hotnode->has_input())
				{
					// 切换焦点
					if (_keynode)
						_keynode->is_input = false;
					auto oldkn = _keynode;
					_keynode = _hotnode;
					_keynode->is_input = true;
				}
			}
		}
	}
	void event_master::ondragend(event_data_cx* node, event_km_t& te, int x, int y)
	{
		if (node && node->curr_drag)
		{
			node->curr_drag = 0;
			drag_info.set_drag_end(x, y, node->node_pos);
			te.etype = (int)event_type::on_dragend;
			node->call_cb(&te);//执行拖动结束
		}
		if (_hotnode && _hotnode->curr_drop && _hotnode->_is_hot && te.mouse_idx == 0)
		{
			_hotnode->curr_drop = false;
			te.etype = (int)event_type::on_drop;
			_hotnode->call_cb(&te);//接收拖动结束
		}
	}
	void event_master::mulit_click(event_data_cx* node)
	{
		if (!node->_is_hot)return;
		node->_nMouseState |= VMK_CLICK;
		{
			auto tem = te;
			tem.etype = (int)event_type::on_click; // 默认单击
			int idx = te.mouse_idx;
			//printf("up\n%d,%p\n\n", idx, _lasthotnode);
			do
			{
				if (idx == 0 && (node->is_dblclick() || node->is_tripleclick()))
				{
					int es = _t.elapsed();
					if (es < _dbt)	//判断时间是否小于多击间隔
					{
						_cks++;
						node->_nMouseState &= ~VMK_DOWN;
						if (_cks == 2)
						{
							tem.etype = (int)event_type::on_dblclick;		//双击
						}
						if (_cks == 3)
						{
							tem.etype = (int)event_type::on_tripleclick;	//三击
						}
						break;
					}
				}
				_cks = 1;
			} while (0);
			if (_cks > 0)
			{
				_t.reset();
				node->call_cb(&tem);	// 执行点击事件
			}
		}
		node->_nMouseState &= ~VMK_CLICK;
	}
	void event_master::on_node_up(event_data_cx* node, int idx)
	{
		if (!node)return;
		node->_nMouseState |= VMK_UP;

		auto tem = te;
		tem.etype = (int)event_type::on_up;	// 不在区域内也可以触发
		node->call_cb(&tem);
		if (node != _hotnode)
			node->_nMouseState &= ~VMK_DOWN;

	}
	void event_master::onMouseUp(int idx, int x, int y)
	{
		te.mouse_idx = idx;
		auto tem = te;
		on_node_up(_dragnode, idx);
		ondragend(_dragnode, tem, x, y);
		if (_lasthotnode != _dragnode)
			on_node_up(_lasthotnode, idx);
		_dragnode = 0;
		if (_hotnode)
		{
			_hotnode->_nMouseState |= VMK_UP;
			if (_hotnode->_nMouseState & VMK_DOWN)
			{
				mulit_click(_hotnode);
			}
			_hotnode->_nMouseState &= ~VMK_DOWN;

		}
		_lasthotnode = 0;
	}


	timer_pex::timer_pex()
	{
		init_task();
	}

	timer_pex::~timer_pex()
	{
	}
	void timer_pex::init_task()
	{
		insert1(this, &timer_pex::call_cb);
	}
	bool timer_pex::call_cb()
	{
		return false;
	}


#endif // 1
