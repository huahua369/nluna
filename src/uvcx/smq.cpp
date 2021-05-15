

#include <unordered_set>
#ifndef NO_LIBUV
//#if (__has_include(<uv.h>))
//#include <uv.h>
//#include <uvw/util.hpp>
//#else
#include <uv.h>
#include <uvw/util.hpp>
//#endif
#include <sem_async_uv.h>
#endif // !NO_LIBUV

#define SMQ_IMPLEMENTATION
#include "smq.h"
/*
注意事项
	需要uv_close的类型：
uv_tcp_t
uv_pipe_t
uv_tty_t
uv_udp_t
uv_poll_t
uv_timer_t
uv_prepare_t
uv_check_t
uv_idle_t
uv_async_t
uv_signal_t
uv_process_t
uv_fs_event_t
uv_fs_poll_t

*/
#define RECV_BUF_SIZE 2048
namespace hz
{
	class ut_t;
	class bind_con_t;
	std::set<uv_handle_t*> ah;
	int a_ct = 0;
	int dev_uv_init(uv_handle_t* handle, int u = 0, void* u1 = nullptr)
	{
		int ret = -1;

		if (!handle)
		{
			return ret;
		}
		uv_loop_t* loop = handle->loop;
		switch (handle->type) {
		case UV_TCP:
			ret = uv_tcp_init(loop, (uv_tcp_t*)handle);
			break;

		case UV_NAMED_PIPE:
			ret = uv_pipe_init(loop, (uv_pipe_t*)handle, u);
			break;

		case UV_TTY:
			ret = uv_tty_init(loop, (uv_tty_t*)handle, u, (long)u1);
			break;

		case UV_UDP:
			ret = uv_udp_init(loop, (uv_udp_t*)handle);
			break;

		case UV_POLL:
			ret = uv_poll_init(loop, (uv_poll_t*)handle, u);
			break;

		case UV_TIMER:
			ret = uv_timer_init(loop, (uv_timer_t*)handle);
			break;

		case UV_PREPARE:
			ret = uv_prepare_init(loop, (uv_prepare_t*)handle);
			break;

		case UV_CHECK:
			ret = uv_check_init(loop, (uv_check_t*)handle);
			break;

		case UV_IDLE:
			ret = uv_idle_init(loop, (uv_idle_t*)handle);
			break;

		case UV_ASYNC:
			ret = uv_async_init(loop, (uv_async_t*)handle, (uv_async_cb)u1);
			break;

		case UV_SIGNAL:
			ret = uv_signal_init(loop, (uv_signal_t*)handle);
			break;

		case UV_PROCESS:
			ret = uv_spawn(loop, (uv_process_t*)handle, (uv_process_options_t*)u1);
			break;

		case UV_FS_EVENT:
			ret = uv_fs_event_init(loop, (uv_fs_event_t*)handle);
			break;

		case UV_FS_POLL:
			ret = uv_fs_poll_init(loop, (uv_fs_poll_t*)handle);
			//uv__handle_closing(handle);
			break;

		default:
			/* Not supported */
			abort();
		}
		if (ret == 0)
		{
			ah.insert(handle);
		}
		return ret;
	}
	void dev_uv_close(uv_handle_t* handle, uv_close_cb ccb)
	{
		uv_close(handle, ccb);
		ah.erase(handle);
		a_ct--;
	}
	size_t get_dev_size()
	{
		return a_ct;
	}


	class loop_t
	{
	private:
		uv_loop_t _loops = {};
		uv_loop_t* _loop = nullptr;
		bool _is_default = false;

		int64_t _thrid = 0;
		char _rbuf[RECV_BUF_SIZE] = {};
		size_t _rlen = RECV_BUF_SIZE;
	public:
		loop_t(bool use_default = false) :_is_default(use_default)
		{
			if (use_default)
			{
				_loop = uv_default_loop();
			}
			else
			{
				_loop = &_loops;
				if (uv_loop_init(_loop))
				{
					throw std::runtime_error("uv_loop_init error");
				}
			}
			_loop->data = this;
		}

		~loop_t()
		{
		}
		int64_t get_tid() {
			return _thrid;
		}
		void set_tid(int64_t id) {
			_thrid = id;
		}
	public:
		static void alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
		{
			if (!handle || !handle->data)
			{
				buf->len = 0;
				return;
			}
			auto p = (loop_t*)handle->loop->data;
			if (p) {
				buf->base = p->_rbuf;
				buf->len = p->_rlen;
			}
		}
	public:
		uv_loop_t* get() { return _loop; }
		int run()
		{
			return uv_run(_loop, UV_RUN_DEFAULT);
		}
		int run_once()
		{
			return uv_run(_loop, UV_RUN_ONCE);
		}

		int run_nowait()
		{
			return uv_run(_loop, UV_RUN_NOWAIT);
		}
		void update_time()
		{
			uv_update_time(_loop);
		}
		int64_t now()
		{
			return uv_now(_loop);
		}
		void stop()
		{
			uv_stop(_loop);
		}
		void close()
		{
			uv_loop_close(_loop);
		}
	private:

	};
	class timer_t
	{
	private:
		uv_timer_t handle = {};
		std::function<void(timer_t*)> _func;
		bool _isptr = false;
		void(*_kcb)(void*) = nullptr;
		void* _ud = nullptr;
	public:
		timer_t()
		{
			handle.data = this;
		}

		~timer_t()
		{
		}
	public:
		int init(uv_loop_t* loop, bool isptr)
		{
			int r = uv_timer_init(loop, &handle);
			_isptr = isptr;
			return r;
		}
		int start(uint64_t timeout, uint64_t repeat, std::function<void(timer_t*)>func)
		{
			_func = func;
			int r = uv_timer_start(&handle, tcb, timeout, repeat);
			return r;
		}

		int stop()
		{
			return uv_timer_stop(&handle);
		}
		// 重启
		int again()
		{
			return uv_timer_again(&handle);
		}
		void set_repeat(uint64_t repeat)
		{
			uv_timer_set_repeat(&handle, repeat);
		}
		uint64_t get_repeat()
		{
			uv_timer_get_repeat(&handle);
		}
		void kill_timer(void* ud, void(*kcb)(void*))
		{
			_ud = ud;
			_kcb = kcb;
			dev_uv_close((uv_handle_t*)&handle, close_cb);
		}
	private:
		void cb()
		{
			if (_func)
			{
				_func(this);
			}
			//auto i = sum("1", 1);
		}
		template<class T1, class T2>
		auto sum(T1 t1, T2 t2)
		{
			return t1 + t2;
		}
		static void tcb(uv_timer_t* t)
		{
			if (t && t->data)
			{
				auto p = (timer_t*)t->data;
				p->cb();
			}
		}
		static void close_cb(uv_handle_t* t)
		{
			if (t && t->data)
			{
				auto p = (timer_t*)t->data;
				if (p->_isptr)
					delete p;
				if (p->_kcb)
				{
					p->_kcb(p->_ud);
				}
			}
		}
	};


	class ipaddr_t
	{
	public:
		enum class proto
		{
			null,
			tcp,
			udp
		};
	private:
		// 0=ip4，1=ip6
		int t = 0;
		union addr_u
		{
			struct sockaddr_in ip4 = {};
			struct sockaddr_in6 ip6;
		}u;
		proto _proto = proto::null;
		std::string _protocol, _ip;
		std::string _old_addr;
		std::string spc = "://";
		int _port = 0;
		bool ok = false;
	public:
		ipaddr_t() {}
		// tcp://127.0.0.1:5050
		ipaddr_t(const std::string& addr)
		{
			init(addr);
		}

		~ipaddr_t()
		{
		}
		void swap(ipaddr_t& a)
		{
			a = *this;
		}
	public:
		void init(const std::string& addr)
		{
			std::vector<std::string> v = hstring::split(addr, spc);
			std::string protocol, ip;
			_old_addr = addr;
			int port = 0;
			if (v.size() == 2)
			{
				protocol = v[0];
				int64_t pos = v[1].rfind(':');
				if (pos > 0)
				{
					auto vip = hstring::split_m(v[1].substr(0, pos), "[]", false);
					if (vip.size())
						ip = vip[0];

					port = std::atoi(v[1].substr(pos + 1).c_str());
				}
			}
			if (ip.empty() || protocol.empty() || port < 1)
			{
				//throw std::runtime_error("addr error!");
			}
			else
			{
				init(protocol, ip, port);
			}
		}
		void init(const std::string& protocol, const std::string& ip, int port)
		{
			if (ok)
			{
				return;
			}
			if (protocol == "tcp")
			{
				_proto = proto::tcp;
			}
			else if (protocol == "udp")
			{
				_proto = proto::udp;
			}
			_protocol = protocol;
			int ips = ip.find('.');
			int ret = 0;
			if (ips > 0)
			{
				ret = uv_ip4_addr(ip.c_str(), port, &u.ip4);
			}
			else
			{
				ret = uv_ip6_addr(ip.c_str(), port, &u.ip6);
			}
			if (!ret)
			{
				_ip = ip;
				_port = port;
				ok = true;
			}
		}
	public:
		proto get_type()
		{
			return _proto;
		}
		const struct sockaddr* get_addr()
		{
			return (const struct sockaddr*)(ok ? &u : nullptr);
		}
		struct sockaddr_in* get4()
		{
			return ok ? &u.ip4 : nullptr;
		}

		struct sockaddr_in6* get6()
		{
			return ok ? &u.ip6 : nullptr;
		}
	};

	template<class _T>
	class auto_gc :public palloc_cx
	{
	private:
		// 回收的对象
		std::vector<void*> _rc;
		// 运行中的tcp client
		std::set<void*> _runs;
		// 缓存数量达到mcc就清理
		size_t _max_cache_cleanup = 64;
	public:
		auto_gc()
		{
			palloc_cx::init(sizeof(void*) * _max_cache_cleanup);
		}

		~auto_gc()
		{
		}
		void set_max(int n)
		{
			if (n > 0)
				_max_cache_cleanup = n;
		}
	public:
		_T* new_c()
		{
			int cs = sizeof(_T);
			_T* p = nullptr;
			if (_rc.size())
			{
				p = (_T*)_rc.back(); _rc.pop_back();
				memset(p, 0, cs);
			}
			else
			{
				p = (_T*)pcalloc(cs);
			}
			if (p)
			{
				_runs.insert(p);
			}
			return p;
		}
		void push_rc(void* c)
		{
			_rc.emplace_back(c);
			_runs.erase(c);
			if (_runs.empty() && _rc.size() > _max_cache_cleanup)
			{
				_rc.clear();
				palloc_cx::reset_pool();
			}
		}
	private:

	};
	class sbuf_t
	{
	public:
		uv_stream_t* client = nullptr;
		const struct sockaddr* addr = nullptr;
		std::string data;
		palloc_cx* ac = nullptr;
		bind_con_t* bct = nullptr;
	public:
		sbuf_t(const char* d, int len)
		{
			if (len > 0)
			{
				data.resize(len);
				memcpy(data.data(), d, len);
			}
		}

		~sbuf_t()
		{
		}
	public:

	};
	// tcp/udp发送结构
	struct swrite_data_t {
		union {
			uv_write_t t;
			uv_udp_send_t u;
		}req;
		uv_buf_t* bufs = nullptr;
		unsigned int nbufs = 0;
		bind_con_t* ctx = nullptr;
		union
		{
			struct sockaddr_in a4;
			struct sockaddr_in6 a6;
		}addr;
		int alen;
	};
	// todo tcp数据列队
	struct tcp_data_t {
		short t = 0;
		void* client = nullptr;
		int data_size = 0;
		char* data = nullptr;
	};
	// todo udp数据列队
	struct udp_data_t {
		short t = 0;
		union
		{
			struct sockaddr_in a4 = {};
			struct sockaddr_in6 a6;
		}addr;
		int alen = 0;
		int data_size = 0;
		char* data = nullptr;
	};
#ifndef OBJP_BUF_SIZE
#define OBJP_BUF_SIZE 1024
#endif // !OBJPBUF_SIZE
	struct op_buf_t {
		short type = 0;
		char buf[OBJP_BUF_SIZE - sizeof(short)] = {};
	};
	struct tcp_handl_t {
		uv_tcp_t p = {};
		uv_connect_t reqt = {};
	};
	// 绑定/连接
	class bind_con_t :public ipaddr_t
	{
	public:
		enum class BCTYPE {
			tNULL,
			tBIND,
			tCON
		};

	private:
		ut_t* _ctx = nullptr;
		ut_dev_t* _dev = nullptr;
		tcp_handl_t* tcp_p = nullptr;
		uv_udp_t* udp_p = nullptr;
		uv_udp_t* tem_udp_p = nullptr;
		palloc_cx* _alloc = nullptr;
		int _acs = 1024;
		// bind_con_t内存专用
		palloc_cx* pcx = nullptr;
		// 连接的客户端用
		palloc_cx* cli_alloc = nullptr;

		uv_loop_t* _loop = nullptr;
		loop_t* _loop_h = nullptr;
		// 绑定/连接类型
		int _bctype = 0;

		sa_t _async;

		std::unordered_set<uv_tcp_t*> _tcp_clients;
		//std::unordered_map<std::string, cli_udp_cx*> _mcli_udp;

		//std::set<cli_tcp_cx*> qs_tcp;
		//std::set<cli_udp_cx*> qs_udp;
		// 是否接收新连接
		bool _is_accept = true;
		bool _is_close = false;
		spin_mutex lksm;
		// 发送列队
		std::queue<tcp_data_t*> s_tcp;
		std::queue<udp_data_t*> s_udp;

		// 对象池
		obj_pool<swrite_data_t> _op_tcp_wd, _op_udp_wd;
		// 缓冲区对象池
		obj_pool<op_buf_t> _op_buf;
		// tcp对象池
		obj_pool<tcp_handl_t> _op_tcp_handle_req;
		// tcp cli池
		obj_pool<uv_tcp_t> _op_tcp_handle;
		// udp句柄
		obj_pool<uv_udp_t> _op_udp_handle;

		int _close_inc = 0;

		// 当前重试次数
		int _cur_try_count = 0;
		// 等待ts毫秒后重试
		int try_space_ms = 0;
		timer_t* reset_timer = nullptr;
		Timer_t tj;
		void(*_close_cb)(void*, bind_con_t* c) = nullptr;
		void* _close_ud = nullptr;
	public:
		bind_con_t()
		{

		}
		bind_con_t(ut_t* ctx) :_ctx(ctx)
		{

		}

		~bind_con_t()
		{
			_alloc->pdelete(pcx);
		}
		// 关闭所有
		void close(void* ud, void(*cb)(void*, bind_con_t* c))
		{
			_is_close = true;
			for (auto c : _tcp_clients)
			{
				close_cli_tcp(c);
			}
			_close_inc = _tcp_clients.size() + 2;
			reset_timer->kill_timer(this, a_close_cb);
			close_tcp();
			close_udp();
			_close_ud = ud;
			_close_cb = cb;
			_async.close();
		}
		void set_ctx(ut_t* ctx, uv_loop_t* loop, ut_dev_t* dev, palloc_cx* alloc, int acs)
		{
			_alloc = alloc;
			_acs = acs;
			pcx = alloc->pnew1<palloc_cx>(_acs, alloc->get_alloc());

			_op_tcp_wd.init(alloc->get_alloc());
			_op_udp_wd.init(alloc->get_alloc());
			_op_buf.init(alloc->get_alloc());
			_op_tcp_handle_req.init(alloc->get_alloc());
			_op_tcp_handle.init(alloc->get_alloc());
			_op_udp_handle.init(alloc->get_alloc());

			_ctx = ctx;
			if (dev)
			{
				_dev = dev;
				_dev->close_ptr = close_ptr;
				_dev->set_accept = set_accept;
				_dev->handle = this;
			}
			_async.init(loop, acb, this);
			_async.set_close_cb(a_close_cb);

			reset_timer = pcx->pnew1<timer_t>();
			reset_timer->init(loop, false);
		}
		ut_dev_t* get_dev()
		{
			return _dev;
		}
		ut_t* get_ctx()
		{
			return _ctx;
		}
		static void a_close_cb(void* u)
		{
			auto p = (bind_con_t*)u;
			if (p)
			{
				p->accb();
			}
		}
		void accb()
		{
			if (_close_inc > 0)
			{
				_close_inc--;
				printf("a_close_cb:%d\n", _close_inc);
				if (_close_cb && _close_inc < 1)
				{
					_close_cb(_close_ud, this);
				}
			}
		}
	public:
		static void set_accept(void* p, bool is)
		{
			auto t = (bind_con_t*)p;
			if (t)
			{
				t->_is_accept = is;
			}
		}
		static void close_ptr(void* p)
		{
			auto t = (bind_con_t*)p;
			if (t)
			{
				t->close_tcp();
				t->close_udp();
			}
		}
	public:
		void set_looph(loop_t* lh)
		{
			_loop_h = lh;
		}
		// s 监听绑定
		int bind(const char* addr_, uv_loop_t* loop)
		{
			int ret = -1;
			if (loop)
				_loop = loop;
			if (_bctype || !_loop)
			{
				return ret;
			}
			ipaddr_t::init(addr_);
			if (get_type() == ipaddr_t::proto::tcp)
			{
				close_tcp();
				tcp_p = _op_tcp_handle_req.get();// pcx->onew<tcp_handl_t>();
				tcp_p->p.data = this;
				uv_tcp_init(loop, (uv_tcp_t*)tcp_p);
				uv_tcp_bind((uv_tcp_t*)tcp_p, get_addr(), 0);
				ret = uv_listen((uv_stream_t*)tcp_p, 64, bct_on_connection);
			}
			else if (get_type() == ipaddr_t::proto::udp)
			{
				close_udp();
				udp_p = _op_udp_handle.get();// pcx->onew<uv_udp_t>();
				udp_p->data = this;
				uv_udp_init(loop, udp_p);
				uv_udp_bind(udp_p, get_addr(), 0);
				ret = uv_udp_recv_start(udp_p, loop_t::alloc_buf, bct_udp_recv);
			}
			_bctype = (int)BCTYPE::tBIND;
			return ret;
		}
		// todo c 连接
		int connect(const char* addr_, uv_loop_t* loop)
		{
			int ret = -1;
			if (loop)
				_loop = loop;
			if (_bctype || !_loop)
			{
				return ret;
			}
			ipaddr_t::init(addr_);
			_loop = loop;
			ret = reset_connect();
			_bctype = (int)BCTYPE::tCON;
			return ret;
		}
		bool is_bind()
		{
			return (_bctype == (int)BCTYPE::tBIND);
		}
	private:

		// todo 重连tcp/udp服务器
		int reset_connect()
		{
			int ret = -1;
			if (!_dev)return ret;
			_dev->try_count;
			_dev->try_space_ms;
			ipaddr_t::proto k = get_type();
			switch (k)
			{
			case hz::ipaddr_t::proto::tcp:
				close_tcp();
				tcp_p = _op_tcp_handle_req.get();// _op_udp_handle.get();
				tcp_p->p.data = this;
				tcp_p->reqt.data = this;
				uv_tcp_init(_loop, (uv_tcp_t*)tcp_p);
				ret = uv_tcp_connect(&tcp_p->reqt, (uv_tcp_t*)tcp_p, get_addr(), bct_on_tcp_connect);
				break;
			case hz::ipaddr_t::proto::udp:
				close_udp();
				udp_p = _op_udp_handle.get();
				udp_p->data = this;
				uv_udp_init(_loop, udp_p);
				ret = uv_udp_connect((uv_udp_t*)udp_p, get_addr());
				break;
			default:
				break;
			}
			//printf("提交连接状态: %d\t%s\n", ret, uv_strerror(ret));
			return ret;
		}
	private:
		void close_tcp()
		{
			if (tcp_p)
			{
				_close_inc++;
				tcp_p->p.data = this;
				dev_uv_close((uv_handle_t*)tcp_p, close_tu_cb);
			}
		}
		void close_udp()
		{
			if (udp_p)
			{
				_close_inc++;
				udp_p->data = this;
				dev_uv_close((uv_handle_t*)udp_p, close_tu_cb);
			}
		}
		void close_tcpudp_cb(uv_handle_t* handle)
		{
			int ty = uv_handle_get_type(handle);
			if (ty == UV_TCP)
			{
				auto tp = (tcp_handl_t*)handle;
				if (tp)
				{
					_op_tcp_handle_req.push_rc(tp);
					printf("%p\t", handle);
				}
			}
			else if (ty == UV_UDP)
			{
				auto tp = (uv_udp_t*)handle;
				if (tp)
				{
					_op_udp_handle.push_rc(tp);
					printf("%p\t", handle);
				}
			}
			a_close_cb(this);
		}
		static void close_tu_cb(uv_handle_t* handle)
		{
			if (handle->data)
			{
				auto p = (bind_con_t*)handle->data;
				if (p)
				{
					p->close_tcpudp_cb(handle);
				}
			}
		}
		palloc_cx* get_cli_alloc()
		{
			if (!cli_alloc)
				cli_alloc = pcx->pnew1<palloc_cx>(_acs, _alloc->get_alloc());
			return cli_alloc;
		}
	public:
		template<class T>
		T* get_opbuf(int len)
		{
			T* ret = nullptr;
			if (len < OBJP_BUF_SIZE)
			{
				auto ob = _op_buf.get();
				if (!ob)
				{
					return ret;
				}
				*ob = {};
				ob->type = 1;
				ret = (T*)ob;
			}
			else
			{
				ret = (T*)pcx->calloc(len, 1);
				if (!ret)
				{
					return ret;
				}
				ret->t = 2;
			}
			return ret;
		}

		void rc_tcp(tcp_data_t* c, swrite_data_t* wd)
		{
			if (c)
			{
				switch (c->t)
				{
				case 1:
					_op_buf.push_rc((op_buf_t*)c);
					break;
				case 2:
					pcx->free(c);
					break;
				default:
					break;
				}
			}
			_op_tcp_wd.push_rc(wd);
		}
		void rc_udp(udp_data_t* c, swrite_data_t* wd)
		{
			if (c)
			{
				auto addr1 = c->addr;
				switch (c->t)
				{
				case 1:
					_op_buf.push_rc((op_buf_t*)c);
					break;
				case 2:
					pcx->free(c);
					break;
				default:
					break;
				}
			}
			_op_udp_wd.push_rc(wd);
		}
		/*
		todo 服务器2客户端
			tcp服务器给客户端发消息传 uv_stream_t*
			udp传const struct sockaddr*
		*/

		int write_data(void* client, const char* data, int len, bool is_send, bool is_copy)
		{
			int ret = -1;
			ipaddr_t::proto k = get_type();
			if (!data || len <= 0 || _is_close)
			{
				return ret;
			}
			auto tid = get_tid();
			bool ismt = tid == _loop_h->get_tid();
			int nlen = (is_copy) ? len : 0;
			switch (k)
			{
			case ipaddr_t::proto::tcp:
			{
				tcp_data_t* tdp = get_opbuf<tcp_data_t>(nlen + sizeof(tcp_data_t));
				if (!tdp)
				{
					return ret;
				}
				tdp->client = client ? client : tcp_p;
				tdp->data_size = len;
				if (is_copy)
				{
					char* tb = (char*)tdp;
					tb += sizeof(tcp_data_t);
					tdp->data = tb;
					auto tb1 = (char*)(&tdp->data) + sizeof(char*);
					memcpy(tdp->data, data, len);
				}
				else {
					tdp->data = (char*)data;
				}

				if (is_send && ismt)
				{
					dev_tcp_write(tdp, 1);
					return 0;
				}
				else
				{
					auto_lock al(&lksm);
					s_tcp.push(tdp);
				}
			}
			break;
			case ipaddr_t::proto::udp:
			{
				const struct sockaddr* addr = (const struct sockaddr*)client;
				std::string ka;
				if (addr) {
					ka.assign((char*)addr, addr->sa_family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
				}
				udp_data_t* sdp = get_opbuf<udp_data_t>(nlen + sizeof(udp_data_t));
				if (!sdp)
				{
					return ret;
				}
				sdp->alen = ka.size();
				if (sdp->alen)
				{
					memcpy(&sdp->addr, addr, ka.size());
				}
				sdp->data_size = len;
				if (is_copy)
				{
					sdp->data = (char*)(&sdp->data) + sizeof(char*);
					memcpy(sdp->data, data, len);
				}
				else {
					sdp->data = (char*)data;
				}
				if (is_send && ismt)
				{
					dev_udp_write(sdp, 1);
					return 0;
				}
				else
				{
					auto_lock al(&lksm);
					s_udp.push(sdp);
				}
			}
			break;
			default:
				ret = -1;
				break;
			}
			if (is_send)
			{
				async_send();
			}
			return ret;
		}


		void wait_write()
		{
			_async.wait_send();
		}
		void async_send()
		{
			_async.async_send();
		}

	private:

		static int acb(void* t)
		{
			int ret = -1;
			auto p = (bind_con_t*)t;
			if (p)
			{
				ret = p->async_func();
			}
			return ret;
		}
		// 在loop线程异步执行
		int async_func()
		{
			int ret = 0;
			dev_send_data();
			async_nc_handle();
			return ret;
		}
		// 在loop循环创建或关闭tcp/udp句柄
		void async_nc_handle()
		{

		}
		void dev_send_data()
		{
			for (; s_tcp.size();)
			{
				tcp_data_t* p = nullptr;
				{
					auto_lock al(&lksm);
					p = s_tcp.front();
					s_tcp.pop();
				}
				if (p)
				{
					dev_tcp_write(p, 1);
				}
			}
			for (; s_udp.size();)
			{
				udp_data_t* p = nullptr;
				{
					auto_lock al(&lksm);
					p = s_udp.front();
					s_udp.pop();
				}
				if (p)
				{
					dev_udp_write(p, 1);
				}
			}
			return;
		}

		void dev_tcp_write(tcp_data_t* c, int n)
		{
			if (c && c->data_size > 0 && n > 0 && !_is_close)
			{
				size_t nbufs = n;
				std::vector<uv_buf_t> bufs;
				swrite_data_t* wd = _op_tcp_wd.get();
				if (wd)
				{
					bufs.resize(n);
					for (size_t i = 0; i < nbufs; i++)
					{
						bufs[i] = uv_buf_init(c[i].data, c[i].data_size);
					}
					char* td = c[0].data;
					wd->req.t.data = c;
					wd->ctx = this;
					int er = uv_write((uv_write_t*)wd, (uv_stream_t*)c->client, bufs.data(), nbufs, tcp_send_end);
					if (er < 0)
					{
						rc_tcp(c, wd);
						printf("dev_tcp发送状态: %d\t%s\n", er, uv_strerror(er));
						if (_dev && _dev->tcp_status)
						{
							tcp_status_t ts = { UTS_send_ing,(uv_stream_t*)tcp_p, (uv_stream_t*)c->client, er,get_dev() };
							_dev->tcp_status(&ts);
						}
						start_reset_timer();
					}
				}
			}
		}
		void dev_udp_write(udp_data_t* c, int n)
		{
			if (c && c->data_size > 0 && n > 0 && !_is_close)
			{
				size_t nbufs = n;
				std::vector<uv_buf_t> bufs;
				const sockaddr* addr = nullptr;
				swrite_data_t* wd = _op_udp_wd.get();
				if (wd)
				{
					bufs.resize(n);
					for (size_t i = 0; i < nbufs; i++)
					{
						bufs[i] = uv_buf_init(c[i].data, c[i].data_size);
					}
					char* td = c[0].data;
					wd->req.t.data = c;
					wd->ctx = this;
					if (c->alen > 0)
						addr = (const sockaddr*)&c->addr;
					int er = uv_udp_send((uv_udp_send_t*)wd, udp_p, bufs.data(), nbufs, addr, udp_send_cb);
					if (er < 0)
					{
						rc_udp(c, wd);
						printf("dev_udp发送状态: %d\t%s\n", er, uv_strerror(er));
						if (_dev && _dev->udp_status)
						{
							udp_status_t us = { UTS_send_ing, udp_p, addr, er,get_dev() };
							_dev->udp_status(&us);
						}
					}
				}
			}
		}

		// 发送结束
		static void tcp_send_end(uv_write_t* w, int status) {
			auto wd = (swrite_data_t*)w;
			auto ctx = (bind_con_t*)wd->ctx;
			if (ctx)
			{
				ctx->tcp_send_cb(w, status);
			}
		}
		void tcp_send_cb(uv_write_t* w, int status)
		{
			auto wd = (swrite_data_t*)w;
			auto c = (tcp_data_t*)w->data;
			if (_dev && _dev->tcp_status)
			{
				tcp_status_t ts = { UTS_send_end,(uv_stream_t*)tcp_p, (uv_stream_t*)c->client, status,get_dev() };
				_dev->tcp_status(&ts);
			}
			rc_tcp(c, wd);
			printf("tcp_send_cb状态: %d\t%s\n", status, uv_strerror(status));
			if (is_bind())
			{
				if (_tcp_clients.empty() && _op_tcp_wd.size())
				{
					_op_tcp_wd.clear();
					printf("clear tcp mem\n");
				}
			}
			else if (status < 0)
			{
				start_reset_timer();
			}
		}
		// udp
		static void udp_send_cb(uv_udp_send_t* req, int status)
		{
			auto wd = (swrite_data_t*)req;
			auto ctx = (bind_con_t*)wd->ctx;
			if (req && req->data)
			{
				ctx->udp_send_cb1(req, status);
			}
		}
		void udp_send_cb1(uv_udp_send_t* req, int status)
		{
			if (status != 0)
				printf("udp发送状态: %d\t%s\n", status, uv_strerror(status));

			auto wd = (swrite_data_t*)req;
			auto c = (udp_data_t*)req->data;
			auto addr = (const struct sockaddr*)(&(wd->addr));

			if (_dev && _dev->udp_status)
			{
				udp_status_t us = { UTS_send_end, req->handle, addr, status,get_dev() };
				_dev->udp_status(&us);
			}
			if (_bctype == (int)BCTYPE::tCON)
			{
				if (tem_udp_p != req->handle)
				{
					uv_udp_recv_start(req->handle, loop_t::alloc_buf, bct_udp_recv);
					auto ip = get_sockname(req->handle);
					printf("link udp://%s\t\t%p\t%p\n", ip.c_str(), tem_udp_p, req->handle);
					tem_udp_p = req->handle;
				}
			}
			else
			{
				// 服务器处理
			}
			rc_udp(c, wd);
		}
	private:
		static void on_resolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res)
		{
			auto ctx = (bind_con_t*)resolver->data;
			if (status == -1) {
				//fprintf(stderr, "getaddrinfo callback error %s\n", uv_err_name());
				return;
			}
			//char addr[17] = { '\0' };
			//uv_ip4_name((struct sockaddr_in*) res->ai_addr, addr, 16);
			//fprintf(stderr, "%s\n", addr);

			//uv_connect_t* connect_req = (uv_connect_t*)malloc(sizeof(uv_connect_t));
			//uv_tcp_t* socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
			//dev_uv_init(loop, socket);

			//connect_req->data = (void*)socket;
			//uv_tcp_connect(connect_req, socket, *(struct sockaddr_in*) res->ai_addr, on_connect);

			uv_freeaddrinfo(res);
			ctx->pcx->pfree(resolver);
		}

		// 连接接收
		static void bct_tcp_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
		{
			((uv_buf_t*)buf)->len = nread;
			assert(client);
			if (!client || (client && !client->data))
			{
				return;
			}
			auto c = (bind_con_t*)client->data;
			if (c)
			{
				c->on_tcp_read(client, nread, buf);
			}
		}
		// 服务器bind读取回调 收数据
		static void bct_udp_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
		{
			auto bct = (bind_con_t*)handle->data;
			if (bct)bct->on_udp_read(handle, nread, buf, addr, flags);
			char ip_addr[128];
			//auto ips = bct->get_sockname(handle);
			if (addr)
			{
				auto ip = bct->get_ipn(addr);
				auto ip2 = bct->get_ipn(addr);
				//uv_ip4_name((struct sockaddr_in*)addr, ip_addr, 128);
				int port = ntohs(((struct sockaddr_in*)addr)->sin_port);
				printf("udp://%s nread = %d\t\n", ip.c_str(), nread);
			}
			if (nread > 0 && buf->base)
			{
				char* str_buf = (char*)buf->base;
				str_buf[nread] = 0;
				printf("\x1b[32;1m%s\x1b[0m:\t%s\n", "udp", str_buf);
			}
		}

	public:
		// 客户端
		static void bct_on_tcp_connect(uv_connect_t* req, int status)
		{
			auto bct = (bind_con_t*)req->data;
			if (bct)
			{
				bct->on_tcp_connect(req, status);
			}
		}
		// 客户端连接成功设置读回调
		void on_tcp_connect(uv_connect_t* req, int status)
		{
			if (status == 0)
			{
				auto ip2 = get_peername((uv_tcp_t*)tcp_p);
				int r = uv_read_start((uv_stream_t*)tcp_p, loop_t::alloc_buf, bct_tcp_read);//客户端开始接收服务器的数据
				auto ip = get_sockname((uv_tcp_t*)tcp_p);
				printf("link tcp://%s\n", ip.c_str());
			}
			else
			{
				printf("连接状态: %d\t%s\n", status, uv_strerror(status));
			}
			if (_dev)
			{
				if (_dev->tcp_connect)
				{
					tcp_connect_t tc = { req, status, get_dev() };
					_dev->tcp_connect(&tc);
				}
				if (status != 0)
				{
					start_reset_timer();
				}
				else
				{
					//_dev->try_ct = 0;
				}
			}
		}
		void start_reset_timer()
		{
			if (_dev->try_ct < _dev->try_count && !is_bind())
			{
				printf("%d毫秒后重试连接\n", _dev->try_space_ms);
				tj.reset();
				reset_timer->start(_dev->try_space_ms, 0, [=](timer_t* pt) {
					printf("正在重连：%dms\n", (int)tj.elapsed());
					reset_connect();
					_dev->try_ct++;
					});
			}
		}
		// ————————————————
		// 服务器bind->con
		static void bct_on_connection(uv_stream_t* server, int status)
		{
			auto bct = (bind_con_t*)server->data;
			if (bct)
			{
				bct->on_tcp_connection(server, status);
			}
		}
		void on_tcp_connection(uv_stream_t* server, int status)
		{
			if (!_is_accept)
			{
				// 停止接收新连接
				printf("stop accept\n");
				return;
			}
			if (status == 0) {
				auto client = new_tcp_c();
				if (client)
				{
					auto c = (uv_stream_t*)client;
					client->data = this;
					uv_accept(server, c);
					int r = uv_read_start((uv_stream_t*)c, loop_t::alloc_buf, on_read);
					if (_dev && _dev->tcp_connection)
					{
						connection_info_t ic = { server, (uv_tcp_t*)c, status, get_dev() };
						_dev->tcp_connection(&ic);
					}
					auto ip = get_sockname((uv_tcp_t*)c);
					printf("new tcp://%s\n", ip.c_str());
				}
			}
			else
			{
				printf("监听连接状态: %d\t%s\n", status, uv_strerror(status));
			}
		}
#if 1
		// todo 新建tcp客户端
		uv_tcp_t* new_tcp_c()
		{
			uv_tcp_t* t = _op_tcp_handle.get();
			if (t)
			{
				*t = {};
				t->data = this;
				uv_tcp_init(_loop, t);
				_tcp_clients.insert(t);
			}
			return t;
		}
		// todo 删除client
		void free_cli_tcp(uv_tcp_t* p)
		{
			if (p)
			{
				if (_dev && _dev->tcp_status)
				{
					tcp_status_t ts = { UTS_client_close,(uv_stream_t*)tcp_p, (uv_stream_t*)p, UV_EOF, get_dev() };
					_dev->tcp_status(&ts);
				}
				_tcp_clients.erase(p);
				_op_tcp_handle.push_rc(p);
				a_close_cb(this);
			}
		}
#endif // 1

		static void close_cli_tcp(uv_tcp_t* t)
		{
			if (t && t->data)
			{
				dev_uv_close((uv_handle_t*)t, close_clitcp_cb);
			}
		}
	private:
		// todo on tcp read
		static void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
		{
			((uv_buf_t*)buf)->len = nread;
			assert(client);
			if (client && client->data)
			{
				auto c = (bind_con_t*)client->data;
				if (c)
				{
					c->tcp_read(client, nread, buf);
				}
			}
		}

		void tcp_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
		{
			// 收到用户数据
			int64_t nr = nread;

			tcp_recv_t tr = {};
			if (_dev && _dev->tcp_recv)
			{
				tr.buf = buf->base;
				tr.len = buf->len;
				tr.nread = nread;
				tr.client = client;
				tr._dev = get_dev();
				_dev->tcp_recv(&tr);

				if (tr.reply_data)
				{
					if (tr.reply_len < 1)
					{
						tr.reply_len = strlen(tr.reply_data);
					}
					write_data(client, tr.reply_data, tr.reply_len, true, false);
				}
			}
			if (nr < 0)
			{
				printf("tcp_read error %s\n", uv_strerror(nr));
				close_cli_tcp((uv_tcp_t*)client);
			}
			if (nread == UV_EOF)
			{
				// end
				return;
			}
		}
	private:
		// 删除客户端
		static void close_clitcp_cb(uv_handle_t* handle)
		{
			if (handle && handle->data)
			{
				auto ctx = (bind_con_t*)handle->data;
				ctx->free_cli_tcp((uv_tcp_t*)handle);
				printf("close_cb\t%p\n", handle);

			}
		}
	public:
		// 连接接收服务器数据
		void on_tcp_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
		{
			tcp_recv_t tr = {};
			if (_dev && _dev->tcp_recv)
			{
				tr.buf = buf->base;
				tr.len = buf->len;
				tr.nread = nread;
				tr.client = client;
				tr._dev = get_dev();
				_dev->tcp_recv(&tr);
			}
			if (nread > 0)
			{
				if (tr.reply_data)
				{
					if (tr.reply_len < 1)
					{
						tr.reply_len = strlen(tr.reply_data);
					}
					write_data(client, tr.reply_data, tr.reply_len, true, false);
				}
			}

			if (nread == UV_EOF)
			{
				// end
				return;
			}
			if (buf->base && nread > 0)
			{
				std::string d((char*)buf->base, nread);
				printf("\x1b[32;1m%s\x1b[0m:\t%s\n", "收到tcp服务器发来", d.c_str());
			}
			else {
				return;
			}
		}
		// todo on_udp_read
		void on_udp_read(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned int flags)
		{
			int64_t nr = nread;
			udp_recv_t tr = {};
			std::string saddr;
			if (addr)
			{
				tr.addr_len = addr->sa_family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
				saddr.assign((char*)addr, tr.addr_len);
			}
			auto ka = get_ipn(addr);

			if (_dev && _dev->udp_recv)
			{
				tr.buf = buf->base;
				tr.len = buf->len;
				tr.nread = nread;
				tr.handle = handle;
				tr.addr = addr;
				tr.flags = flags;
				tr._dev = get_dev();
				_dev->udp_recv(&tr);
			}
			if (tr.reply_data)
			{
				int buflen = tr.reply_len > 0 ? tr.reply_len : strlen(tr.reply_data);
				//uv_buf_t nbuf = uv_buf_init((char*)tr.reply_data,);
				//udp_write_data(pc, addr, &nbuf, 1);
				write_data((void*)addr, tr.reply_data, buflen, true, false);
			}
			if (_bctype == (int)BCTYPE::tBIND)
			{
			}
			else
			{
				if (buf->base && nread > 0)
				{
					std::string d((char*)buf->base, nread);
					printf("\x1b[32;1m%s\x1b[0m:\t%s\n", "收到udp服务器发来", d.c_str());
				}
			}
		}
	public:
#if 1
		std::string get_sockname(const uv_udp_t* p)
		{
			struct sockaddr name[2] = {};
			int len = sizeof(name);
			int r = uv_udp_getpeername(p, name, &len);
			if (r < 0)
			{
				printf("get_sockname error %s\n", uv_strerror(r));
			}
			return get_ipn(name);
		}
		std::string get_peername(const uv_udp_t* p)
		{
			struct sockaddr name[2] = {};
			int len = sizeof(name);
			int r = uv_udp_getpeername(p, name, &len);
			if (r < 0)
			{
				printf("get_peername error %s\n", uv_strerror(r));
			}
			return get_ipn(name);
		}
		std::string get_sockname(const uv_tcp_t* tcp)
		{
			struct sockaddr name[2] = {};
			int len = sizeof(name);
			int r = uv_tcp_getpeername(tcp, name, &len);
			if (r < 0)
			{
				printf("get_sockname error %s\n", uv_strerror(r));
			}
			return get_ipn(name);
		}
		std::string get_peername(const uv_tcp_t* tcp)
		{
			struct sockaddr name[2] = {};
			int len = sizeof(name);
			int r = uv_tcp_getpeername(tcp, name, &len);
			if (r < 0)
			{
				printf("get_peername error %s\n", uv_strerror(r));
			}
			return get_ipn(name);
		}
		std::string get_ipn(const struct sockaddr* addr)
		{
			uv_getnameinfo_t ni = {};
			int r = uv_getnameinfo(_loop, &ni, 0, addr, 0);
			//inet_pton(AF_INET, ip, &foo.sin_addr);   //  代替 foo.sin_addr.addr=inet_addr(ip);
			typedef struct iptem
			{
				short t;
				short port;
				char ip[sizeof(ni.storage)];
			}iptem_t;
			auto ipc = (iptem_t*)&ni.storage;
			//auto port = ttUSHORT((uint8_t*)&ipc->port);
			//char str[INET_ADDRSTRLEN];
			//int r3 = uv_inet_ntop(AF_INET, &ipc->ip, str, sizeof(str));
			char str6[1024] = {};
			char c6[1024] = {};
			char* ipd = (char*)&ni.storage;
			char* ipd4 = ipd + (ipc->t == AF_INET ? 4 : 8);
			int r36 = uv_inet_ntop(ipc->t, ipd4, str6, sizeof(str6));
			std::string ret;
			if (str6[0])
			{
				if (ipc->t == AF_INET6)
				{
					ret += "[";
					ret += str6;
					ret += "]";
				}
				else {
					ret = str6;
				}
			}
			else if (ni.host[0])
			{
				ret = ni.host;
			}
			ret += ":";
			ret += ni.service;
			int ik = sizeof(struct sockaddr_in6);
			//int ik0 = sizeof(SOCKADDR_IN6_W2KSP1);
			//memcpy(c6, addr, 128);
			return ret;
		}
		void get_addrinfo(const std::string& addr)
		{
			struct addrinfo hints;
			hints.ai_family = PF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = 0;

			uv_getaddrinfo_t* resolver = pcx->onew<uv_getaddrinfo_t>();
			//fprintf(stderr, "irc.freenode.net is... ");
			resolver->data = this;
			int r = uv_getaddrinfo(_loop, resolver, on_resolved, addr.c_str(), "80", &hints);
		}
#endif
	};
	class ut_t
	{
	private:
		std::atomic<uint64_t> _main_tid = 0;
		std::atomic_bool _is_run = true;
		std::atomic_int _tc = 0;
		palloc_cx* _pcx = nullptr;
		//uv_tty_t g_tty;

		loop_t* _loop = nullptr;
		// 需要删除的对象
		std::set<bind_con_t*> _fdevts;
		// 所有对象
		std::set<bind_con_t*> _bcts;

		// 异步任务
		cvq_t<thread_pool::task_struct*> _tq;
		obj_pool<thread_pool::task_struct> _optask;

		int _acs = 4096;
		bool _is_close = false;
		sa_t _async;
		void (*_ccb)(dut_t* p, ut_t* ut) = nullptr;
		dut_t* _dut = 0;
		std::recursive_mutex _lkc;
	public:
		ut_t(bool use_default, palloc_cx* pcx, int acs) :_pcx(pcx)
		{
			if (acs >= 100)
			{
				_acs = acs;
			}
			// todo pnew   loop_t
			//_loop = _pcx->pnew1<loop_t>(use_default);
			_loop = auto_ptr::new_ptr<loop_t>(_pcx->get_alloc(), 1);
			/*		uv_tty_init(_loop->get(), &g_tty, 1, 0);
					uv_tty_set_mode(&g_tty, 0);*/
			_optask.init(_pcx->get_alloc(), 2);
			_async.init(get_loop(), a_close_cb, this);
		}

		~ut_t()
		{
			auto_ptr::free_ptr(_loop, _pcx->get_alloc());
		}
	public:
		uv_loop_t* get_loop()
		{
			return _loop ? _loop->get() : nullptr;
		}
		void* socket(int type_)
		{

		}
		void close(dut_t* p, void (*ccb)(dut_t* p, ut_t* ut))
		{
			_dut = p;
			_ccb = ccb;
			_is_run = false;
			_async.async_send();
		}
		static void close_devt(ut_t* p, ut_dev_t* c)
		{
			std::lock_guard lg(p->_lkc);
			p->_fdevts.insert((bind_con_t*)c->handle);
			p->_async.async_send();
		}

		int setsockopt(int option_, const void* optval_, size_t optvallen_)
		{
			return 0;
		}
		int getsockopt(int option_, void* optval_, size_t* optvallen_)
		{
			return 0;
		}
	private:
		ut_dev_t* new_devt(const char* addr_, bool isbind, ut_dev_t* ret = nullptr)
		{
			int r = -1;
			auto oldr = ret;
			// todo pnew  bind_con_t
			bind_con_t* bct = nullptr;// _pcx->pnew1<bind_con_t>();
			bct = auto_ptr::new_ptr<bind_con_t>(_pcx->get_alloc(), 1);
			do
			{
				if (bct)
				{
					if (!ret)
					{
						ret = auto_ptr::new_ptr<ut_dev_t>(_pcx->get_alloc(), 1);// _pcx->onew<ut_dev_t>();
						ret->isfree = true;
					}
					bct->set_looph(_loop);
					// todo 分配alloc
					bct->set_ctx(this, get_loop(), ret, _pcx, _acs);
				}
				else {
					break;
				}
				int ka;
				if (isbind)
				{
					r = bct->bind(addr_, _loop->get());
				}
				else
				{
					r = bct->connect(addr_, _loop->get());
				}
			} while (0);
			if (r != 0)
			{
				if (oldr)
				{
					free_devt(oldr);
				}
				ret = nullptr;
			}
			else
			{
				_bcts.insert(bct);
			}
			return ret;
		}
		void free_devt(ut_dev_t* dt)
		{
			if (dt)
			{
				auto alloc = _pcx->get_alloc();
				auto_ptr::free_ptr((bind_con_t*)dt->handle, alloc);
				if (dt->isfree)
					auto_ptr::free_ptr((ut_dev_t*)dt, alloc);
			}
		}

		void clean_task_queue()
		{
			_tq.clear([=](thread_pool::task_struct* task) {
				_optask.push_rc(task);
				if (task->_auto_del)
				{
					//thread_pool::task_struct::destroy(task);
				}
				});
		}
		void run_tq() {
			_tq.get_all(this, one_task);
		}

		static int one_task(void* ud, thread_pool::task_struct* task)
		{
			printf("one_task:\t%d\n", get_tid());
			ut_t* ctx = (ut_t*)ud;
			if (task && task->_func.size())
			{
				bool isad = task->_auto_del;
#if 1
				for (const auto& it : task->_func)
				{
					it();
				}
				task->_done = true;
				if (task->_sig)
				{
					task->_sig->post();
				}
				else
				{
					ctx->_optask.push_rc(task);
				}
#endif // 1
				if (isad)
				{
					//thread_pool::task_struct::destroy(task);
				}
			}
			return  0;
		}
	public:
		template<typename Function, typename... Args>
		void add_task(bool is_await, const Function& func, Args... args)
		{
			//thread_pool::task_t task = [&func, args...]{ return func(args...); };
			thread_pool::task_struct* task = _optask.get();
			task->init();
			task->add_task(func, args...);
			auto tid = get_tid();
			if (tid == _main_tid && is_await)
			{
				one_task(this, task);
			}
			else
			{
				if (is_await)
					task->mk_wait();
				else
					task->mk_async(true, false);
				_tq.push(task);
				_async.async_send();
				if (is_await)
					task->wait(false);
				_optask.push_rc(task);
			}
		}

		int init_devt(ut_dev_t* info, int n)
		{
			int ret = 0;
			thread_pool::task_struct* task = _optask.get();
			task->init();
			for (size_t i = 0; i < n; i++)
			{
				auto& it = info[i];
				if (it._addr && !it.handle)
				{
					bool isb = (it._type == 1);
					task->add_task([=](const char* addr, bool is, ut_dev_t* d) {
						new_devt(addr, is, d);
						}, it._addr, isb, &it);
					ret++;
				}
			}
			if (ret > 0)
			{
				auto tid = get_tid();
				if (tid == _main_tid || !_main_tid)
				{
					one_task(this, task);
				}
				else
				{
					task->mk_wait();
					_tq.push(task);
					_async.async_send();
					task->wait(false);
					_optask.push_rc(task);
				}
			}
			return ret;
		}
		// s 监听绑定
		ut_dev_t* bind(const char* addr_)
		{
			return new_devt(addr_, true);
		}
		// c 连接
		ut_dev_t* connect(const char* addr_)
		{
			return new_devt(addr_, false);
		}

		int run()
		{
			int ret = 0;
			int c = 0;
			int aks[] = { sizeof(struct sockaddr_in),sizeof(struct sockaddr_in6) };
			set_thr_name_cur("loop 线程");
			_is_run = true;
			_main_tid = get_tid();
			_loop->set_tid(_main_tid);
			do {
				try
				{
					//ret = _loop->run();
					ret = _loop->run_once();
					run_tq();
					c++;
				}
				catch (const std::exception& e)
				{
					std::cout << e.what() << std::endl;
				}
				sleep(1);
			} while (_is_run == true);
			_main_tid = 0;
			if (_is_close)
			{
				for (auto it : _bcts)
				{
					free_devt(it->get_dev());
				}
				_bcts.clear();
				if (_ccb)
				{
					_ccb(this->_dut, this);
				}
			}
			return ret;
		}
		int run_auto()
		{
			int ret = 0;
			if (_main_tid == 0)
			{
				std::thread([=]() {
					run();
					}).detach();
			}
			else {

			}
			return ret;
		}
		void run_stop()
		{
			_is_run = false;
		}

	private:
		void cdcb(bind_con_t* c)
		{
			free_devt(c->get_dev());
			_bcts.erase(c);
			printf("%p\n", c);
		}
		static void close_devt_cb(void* t, bind_con_t* c)
		{
			if (t && c)
			{
				((ut_t*)t)->cdcb(c);
			}
		}
		static int a_close_cb(void* t)
		{
			int ret = -1;
			auto p = (ut_t*)t;
			if (p)
			{
				ret = p->async_close_func();
			}
			return ret;
		}
		// 在loop线程异步执行
		int async_close_func()
		{
			int ret = 0;
			if (_is_run)
			{
				if (_fdevts.size())
				{
					std::lock_guard lg(_lkc);
					for (auto it : _fdevts)
					{
						it->close(this, close_devt_cb);
					}
					_fdevts.clear();
				}
			}
			else
			{
				_is_close = true;
				for (auto it : _bcts)
				{
					it->close(nullptr, nullptr);
				}
				_async.close();
				_loop->close();
			}
			return ret;
		}

	private:

	};


}// !hz

using namespace hz;
// c---------------------------------------------------------------------------------
static std::once_flag moncef;
static char a_pcxm[sizeof(palloc_cx)] = {};
static palloc_cx* a_pac = nullptr;

void* s_malloc_func(size_t size)
{
	return a_pac->malloc<char>(size);
	//return malloc(size);
}
void* s_realloc_func(void* ptr, size_t size)
{
	return a_pac->realloc(ptr, size);
	//return realloc(ptr, size);
}

void* s_calloc_func(size_t count, size_t size)
{
	return a_pac->calloc(count, size);
	//return (count * size > 0) ? calloc(count, size) : nullptr;
}

void  s_free_func(void* ptr)
{
	if (ptr)
	{
		a_pac->free(ptr);
		//free(ptr);
	}
}

void on_close_ut(dut_t* p, ut_t* ut)
{
	if (p && a_pac)
	{
		if (ut)
		{
			//delete ctx;
			a_pac->pdelete(ut);
		}
		//delete p;
		if (p->is_free)
			a_pac->pfree(p);
	}
}
dut_t* ut_new(bool loop_default, int alloc_size)
{
	return ut_init(nullptr, loop_default, alloc_size);
}
void ut_close(dut_t* p)
{
	if (p && a_pac)
	{
		if (p->ctx)
		{
			auto ctx = (ut_t*)p->ctx;
			ctx->close(p, on_close_ut);
		}
	}
}

void ut_close_dev(ut_dev_t* c)
{
	if (c)
	{
		if (c->handle)
		{
			auto ctx = (bind_con_t*)c->handle;
			ut_t::close_devt(ctx->get_ctx(), c);
		}
	}
}
int ut_init_dev(dut_t* pt, ut_dev_t* info, int n)
{
	int ret = -1;
	if (pt && info && n > 0 && pt->ctx)
	{
		auto p = (ut_t*)pt->ctx;
		ret = p->init_devt(info, n);
	}
	return ret;
}
ut_dev_t* ut_bind(dut_t* pt, const char* addr_)
{
	ut_dev_t* ret = nullptr;
	if (pt)
	{
		auto p = (ut_t*)pt->ctx;
		if (p)
		{
			ret = p->bind(addr_);
		}
	}
	return ret;
}

ut_dev_t* ut_connect(dut_t* pt, const char* addr_)
{
	auto p = (ut_t*)(pt ? pt->ctx : nullptr);
	ut_dev_t* ret = nullptr;
	if (p)
	{
		ret = p->connect(addr_);
	}
	return ret;
}


int ut_run(dut_t* pt)
{
	int ret = -1;
	if (pt)
	{
		auto p = (ut_t*)pt->ctx;
		if (pt->run_type)
		{
			ret = p->run();
		}
		else {
			ret = p->run_auto();
		}
	}
	return ret;
}

int ut_write(ut_dev_t* c, const char* data, int len, void* client, bool is_send, bool is_copy)
{
	int r = 0;
	if (c)
	{
		auto p = (bind_con_t*)c->handle;
		if (p)
		{
			r = p->write_data(client, data, len, is_send, is_copy);
		}
	}
	return r;
}
void ut_close_client(ut_dev_t* c, TCP_P* client, bool is_await)
{
	if (c)
	{
		auto p = (bind_con_t*)c->handle;
		if (p)
		{
			ut_t* ut = p->get_ctx();
			ut->add_task(is_await, [=](bind_con_t* b, TCP_P* cli) {
				b->close_cli_tcp(cli);
				}, p, client);
		}
	}
}

dut_t* ut_init(dut_t* dt, bool loop_default, int alloc_size)
{
	std::call_once(moncef, [=]() {
		a_pac = auto_ptr::init_ptr((palloc_cx*)a_pcxm, alloc_size);
		uv_replace_allocator(s_malloc_func, s_realloc_func, s_calloc_func, s_free_func);
		});
	if (!dt)
	{
		dt = a_pac->onew<dut_t>();
		dt->is_free = true;
	}
	int ws = sizeof(swrite_data_t);
	int ss = sizeof(saddr_u);
	int as = sizeof(struct sockaddr);
	int ast[] = { sizeof(dut_t),sizeof(ut_t),sizeof(loop_t) };
	//auto p = a_pac->pnew1<ut_t>(loop_default, a_pac, alloc_size);
	//auto p = new ut_t(loop_default, a_pac, alloc_size);
	auto p = auto_ptr::new_ptr<ut_t>(a_pac->get_alloc(), 1, loop_default, a_pac, alloc_size);
	dt->ctx = p;
	dt->_alloc = a_pac;
	dt->loop = p->get_loop();

	dt->ut_run = ut_run;
	dt->ut_close = ut_close;
	// 异步或同步创建批量绑定/连接
	dt->ut_init_dev = ut_init_dev;
	// ut_bind/ut_connect只能在loop run前或loop线程执行
	dt->ut_bind = ut_bind;
	dt->ut_connect = ut_connect;
	dt->ut_close_dev = ut_close_dev;
	dt->ut_write = ut_write;
	dt->ut_close_client = ut_close_client;
	return dt;
}
