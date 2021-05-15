#pragma once
/*
	通讯传输模块
*/
#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>  /* MAXHOSTNAMELEN on Solaris */
#endif // _WIN32
#include <palloc/palloc.h>


#ifdef __cplusplus

#include <unordered_map>
#include <utility>
#include <mutex>
#include <atomic>
#include <memory_resource>
#include <string>

//typedef std::pmr::monotonic_buffer_resource palloc_cx;

extern "C" {
#endif
#ifdef SMQ_IMPLEMENTATION0
#define UT_P ut_t
#define DEV_P bind_con_t

#else
#define UT_P void
#define DEV_P void
#define TCC_P void

#endif // !SMQ_IMPLEMENTATION

#ifdef UV_H
#define LOOP_P uv_loop_t
#define STREAM_P uv_stream_t
#define TCP_P uv_tcp_t
#define UDP_P uv_udp_t
#else
#define LOOP_P void
#define STREAM_P void
#define TCP_P void
#define UDP_P void
#endif // UV_H
	typedef struct dut_s dut_t;
	typedef struct ut_dev_s ut_dev_t;
	union saddr_u
	{
		struct sockaddr_in a4 = {};
		struct sockaddr_in6 a6;
	};
	typedef struct {
		STREAM_P* server; TCP_P* client; int status;
		ut_dev_t* _dev;
	}connection_info_t;
	typedef struct tcp_recv_s {
		STREAM_P* client;
		int64_t nread;
		uint32_t len;
		char* buf;
		// 可选回复数据
		const char* reply_data;
		int reply_len;
		ut_dev_t* _dev;
	}tcp_recv_t;
	typedef struct udp_recv_s {
		UDP_P* handle;
		int64_t nread;
		uint32_t len;
		char* buf;
		const struct sockaddr* addr;
		int addr_len;
		unsigned int flags;
		// 可选回复数据
		const char* reply_data;
		int reply_len;
		ut_dev_t* _dev;
	}udp_recv_t;
	enum UT_STATUS_E
	{
		UTS_null = 0,
		UTS_send_ing,
		UTS_send_end,
		UTS_client_close,
		UTS_connection,
		UTS_connect,
	};
	/*

	*/
	typedef struct {
		int t;
		UDP_P* src; const struct sockaddr* addr; int status;
		ut_dev_t* _dev;
	}udp_status_t;
	typedef struct {
		int t;
		STREAM_P* src; STREAM_P* dst; int status;
		ut_dev_t* _dev;
	}tcp_status_t;
	typedef struct {
		void* req; int status;
		ut_dev_t* _dev;
	}tcp_connect_t;

	typedef void(*tcp_connection_cb_t)(connection_info_t*);
	typedef void(*tcp_connect_cb_t)(tcp_connect_t*);
	typedef void(*tcp_recv_cb_t)(tcp_recv_t*);
	typedef void(*udp_recv_cb_t)(udp_recv_t*);
	typedef void(*udp_status_cb_t)(udp_status_t*);
	typedef void(*tcp_status_cb_t)(tcp_status_t*);

	typedef const struct sockaddr* udpk_p;

	struct ut_dev_s {
		//private
		DEV_P* handle = nullptr;
		// bind=1，connect=2
		int _type = 0;
		// 通过init创建则addr设置地址
		const char* _addr = nullptr;
		bool isfree = false;
		// 关闭连接或绑定
		void (*close_ptr)(void* handle) = nullptr;
		// 设置停止/开始接收新连接
		void (*set_accept)(void* p, bool is) = nullptr;
		//public
		void* user_data = nullptr;
		// 6秒后尝试重新连接一次
		int try_space_ms = 6000;
		// 重试次数失败放弃连接,默认3次
		int try_count = 3;
		// 当前重试次数，开启重试则在回调函数修改try_ct=0
		int try_ct = 0;
		// bind/connect后设置（服务器）
		tcp_connection_cb_t tcp_connection = nullptr;
		// 客户端连接状态返回tcp_connect可修改try_space_ms、try_count
		tcp_connect_cb_t tcp_connect = nullptr;
		tcp_recv_cb_t tcp_recv = nullptr;
		udp_recv_cb_t udp_recv = nullptr;
		tcp_status_cb_t tcp_status = nullptr;
		udp_status_cb_t udp_status = nullptr;
	};

	struct dut_s {
		UT_P* ctx = nullptr;
		LOOP_P* loop = nullptr;
		// 已用内存，uv使用的内存分配器
		palloc_cx* _alloc = 0;
		//int alloc_size = 0;
		bool is_free = false;
		// 功能函数
		int (*ut_run)(dut_t* pt) = nullptr;
		void (*ut_close)(dut_t* p) = nullptr;
		// 异步或同步创建批量绑定/连接
		int (*ut_init_dev)(dut_t* p, ut_dev_t* info, int n) = nullptr;
		// ut_bind/ut_connect只能在loop run前或loop线程执行
		ut_dev_t* (*ut_bind)(dut_t* p, const char* addr_) = nullptr;
		ut_dev_t* (*ut_connect)(dut_t* p, const char* addr_) = nullptr;
		int (*ut_write)(ut_dev_t* c, const char* data, int len, void* client_addr, bool is_send, bool is_copy) = nullptr;
		void (*ut_close_dev)(ut_dev_t* c) = nullptr;
		// 关闭bind的持有的客户端
		void (*ut_close_client)(ut_dev_t* c, TCP_P* client, bool is_await) = nullptr;
		//public
		// run_type 0创建新线程执行，1则在主线程
		int run_type = 0;
	};

	// 初始化一个loop管理
	dut_t* ut_init(dut_t* dt, bool loop_default, int alloc_size);

	// 进入loop循环
	int ut_run(dut_t* pt);
	void ut_close(dut_t* p);
	// 异步或同步创建批量绑定/连接
	int ut_init_dev(dut_t* p, ut_dev_t* info, int n);
	// ut_bind/ut_connect只能在loop run前或loop线程执行
	ut_dev_t* ut_bind(dut_t* p, const char* addr_);
	ut_dev_t* ut_connect(dut_t* p, const char* addr_);
	void ut_close_dev(ut_dev_t* c);
	int ut_write(ut_dev_t* c, const char* data, int len, void* client_addr, bool is_send, bool is_copy);
	/*
		说明
		ut_write、ut_close可以随意执行线程，其它函数需在loop线程或未执行run前随意。
		todo ut_close 关闭整个ut
		todo ut_bind、ut_connect创建绑定/连接对象
		addr_可写tcp/udp、ip4/6
		例子1：std::string protocol[] = { "tcp://", "udp://" };
		std::string addr = "[::1]:";
		std::string port = "6060";
		(protocol[i] + addr + port).c_str()
		例子2："tcp://127.0.0.1:6060"

		ut_write  执行线程不限
		tcp服务器给客户端发消息传 uv_stream_t*
		udp传const struct sockaddr*
		tcp/udp客户端给连接的服务器发数据则client=nullptr
		is_send是否立即发送

	*/
#ifdef __cplusplus
}

#endif
//!cpp

//!hz
#endif
