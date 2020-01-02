#ifndef __HTTP_H__
#define __HTTP_H__

#include "libuv/uv.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#ifdef _WIN32
#include <memory.h>
#endif
#include <string>
#include <map>
#include <vector>
#include <functional>

#if defined(WIN32)
#define snprintf _snprintf
#endif
#define SERVER_STRING "Server: pnguo http/1.1.0\r\n"
#if 0

0关闭所有格式，还原为初始状态
1粗体 / 高亮显示
2模糊（※）
3斜体（※）
4下划线（单线）
5闪烁（慢）
6闪烁（快）（※）
7交换背景色与前景色
8隐藏（伸手不见五指，啥也看不见）（※）
30 - 37前景色，即30 + x，x表示不同的颜色（参见下面的“颜色表”）
40 - 47背景色，即40 + x，x表示不同的颜色（参见下面的“颜色表”）
颜色值x  0   1   2   3   4   5   6   7
颜色     黑  红  绿  黄  蓝  紫  青  白

\033、\x1b和\e
可用分号隔开，"\x1b[32;1m%s"和"\x1b[1;32m%s"效果一样
#endif
#ifdef _PTID_
#define PrintTID do{uint32_t ati = Event::pthreadID();printf("\x1b[32;1m<%s> \x1b[31mTID\x1b[0m: \x1b[32m%d\x1b[0m\n",__func__, ati);}while(0)
#else
#define PrintTID
#endif // _WIN32
//HTTP v3, a tiny web server based on libuv, by liigo, 2013-6-30.
namespace hz {
	class Request :public Res
	{
	public:

		typedef struct _sData
		{
			void* data = 0;
			bool is_close = true;
		}sData, * psData;
	public:
		std::string _data_heads, _data_body, _data;
		ssize_t content_len = 0;
		uv_stream_t* _client = 0;
		std::string _url;
		njson _request;
		std::map<std::string, std::string> _heads, heads_lower;
		std::vector<std::string> headv;
		ssize_t _heads_pos = 0;

		void* _user_data = 0;
		std::string _protocol;
		LockS _lk;
	public:
		Request()
		{
		}
		Request(uv_stream_t* c) :_client(c)
		{
		}

		~Request()
		{
		}
		void clear_data()
		{
			LOCK_W(_lk);
			_heads.clear();
			headv.clear();
			heads_lower.clear();
			_request = njson();
			_heads_pos = content_len = 0;
			_data = _data_heads = _data_body = _url = "";
		}
	public:
		static Request* create(uv_stream_t* c)
		{
			return new Request(c);
		}
		static  void init(void* p)
		{
			Request* rp = (Request*)p;
			if (rp)
				rp->clear_data();
		}
		std::string get_heads()
		{
			return _data_heads;
		}
	public:
		int append_data(void* data, ssize_t len)
		{
			LOCK_W(_lk);
			_data.append((char*)data, (char*)data + len);
			if (_protocol.empty() && _data.size() > 4)
			{
				if (Nluna::is_headstr((char*)_data.data()))
				{
					_protocol = Nluna::get_headstr();
				}
				else
				{
					_protocol = "HTTP";
				}
			}
			if (_protocol == "HTTP")
			{
				if (_heads_pos <= 0)
				{
					//File::save_binary_file("tem.log", (char*)data, len);
					_data_heads.append((char*)data, (char*)data + len);
					_heads_pos = _data_heads.find("\r\n\r\n");
					if (_heads_pos != -1)
						handle_heads(_data_heads.data(), _heads_pos);
				}
				else {
					_data_body.append((char*)data, (char*)data + len);
				}
			}
			return 0;
		}
		// 解析请求头
		void handle_heads(const char* data, ssize_t len)
		{
			ssize_t heads_len = len + 4;
			if (_data_heads.size() > heads_len)
			{
				auto blen = _data_heads.size() - heads_len;
				_data_body.clear();
				_data_body.append((char*)data + heads_len, (char*)data + heads_len + blen);
				_data_heads.resize(heads_len);
			}
			int numchars;
			char url[255] = { 0 };
			std::vector<std::string> dat;
			std::string d(data, len);
			hstring::split(d, "\r\n", dat);

			hstring::split(dat[0], " ", headv);
			if (*headv[1].rbegin() == '/')
			{
				headv[1] += "index.html";
			}
			std::string tembd;
			for (size_t i = 1; i < dat.size(); ++i)
			{
				auto& it = dat[i];
				auto pos = it.find(':');
				if (pos != -1)
				{
					std::string k = it.substr(0, pos);
					auto v = hstring::trim(it.substr(pos + 1));
					_heads[k] = v;
					heads_lower[hstring::toLower(k)] = v;
				}
			}
			content_len = std::atoll(heads_lower["content-length"].c_str());
			_request = { { "heads", heads_lower } };
		}
		bool is_accept()
		{
			bool ret = false;
			uint32_t ati = *((uint32_t*)(_data.data() + _data.size() - 4));

			std::string at;
			at.resize(4);
			memcpy((char*)at.data(), _data.data() + _data.size() - 4, 4);
			ret = (_heads_pos > 0) && (_data_body.size() >= content_len);
			if (memcmp(_data.data(), "POST", 4) == 0)
			{

			}
			//isa = (at == "\r\n");
			if (ret)
				_heads_pos = 0;
			return ret;
		}

	private:

	};
	class AutoReq
	{
	public:
		AutoReq(Request* p) :_p(p)
		{
		}

		~AutoReq()
		{
			Request::init(_p);
		}

	private:
		Request* _p = nullptr;
	};

	/*
	 TODO http server
	 *
	 **/
	class http :public thread_pool
	{
	public:
		http()
		{
			set_name("http server");
		}

		~http()
		{
		}
		typedef struct _sData
		{
			void* data = 0;
			bool is_close = true;
		}sData, * psData;
	private:
		// todo data
		uv_tcp_t    _server;
		uv_tcp_t    _client;
		uv_loop_t* _loop = NULL;
		std::string _doc_root_path;
		std::string default_content_type = "text/html;charset=utf-8";
		LockS _lock_rc;
		std::queue<uv_tcp_t*> _rc, _runc;
		// 跨域
		std::vector<std::string> _origin = { "localhost","127.0.0.1" };
	public:
		std::function<bool(const char* pathinfo, njson * request, void* client)> route_func;
		std::function<void(const char* pathinfo, const njson & request, njson & response)> route;
	public:
		void set_default_content_type(const std::string& ct)
		{
			default_content_type = ct;
		}
	public:

		static std::string expand_user(std::string path) {
			if (!path.empty() && path[0] == '~') {
				assert(path.size() == 1 || path[1] == '/');  // or other error handling
				char const* home = getenv("HOME");
				if (home || (home = getenv("USERPROFILE"))) {
					path.replace(0, 1, home);
				}
				else {
					char const* hdrive = getenv("HOMEDRIVE"),
						* hpath = getenv("HOMEPATH");
					assert(hdrive);  // or other error handling
					assert(hpath);
					path.replace(0, 1, std::string(hdrive) + hpath);
				}
			}
			return path;
		}

		//static void on_connection(uv_stream_t* server, int status);

		//start web server, linstening ip:port
		//ip can be NULL or "", which means "0.0.0.0"
		//doc_root_path can be NULL, or requires not end with /
		static http* gs()
		{
			static http* s = new http();
			return s;
		}
		static int start(uv_loop_t* loop, const char* ip, int port, const char* doc_root_path, bool isopen = false) {
			http* hs = gs();
			struct sockaddr_in addr;
			uv_ip4_addr((ip && ip[0]) ? ip : "0.0.0.0", port, &addr);

			hs->_loop = loop;
			if (doc_root_path && doc_root_path[0])
			{
				hs->_doc_root_path = doc_root_path;
#ifdef _WIN32
				if (doc_root_path[1] != ':')
				{
					hs->_doc_root_path = File::getAP() + doc_root_path;

				}
#else
				auto apd = File::getAP();
				auto dir = hstring::split(apd, "/"), drp = hstring::split(doc_root_path, "/");
				if (doc_root_path[0] != '/')
				{
					hs->_doc_root_path = apd + doc_root_path;
				}
#endif
			}

			uv_tcp_init(hs->_loop, &hs->_server);
			uv_tcp_bind(&hs->_server, (const struct sockaddr*) & addr, 0);
			int ret = uv_listen((uv_stream_t*)&hs->_server, 8, on_connection);
			if (ret != 0)
			{
				printf("error: %d Fail to monitor port!\n", ret);
			}
			else
			{
				std::string sip = ip;
				if (sip == "")
				{
					sip = "127.0.0.1";
				}
				printf("Http is started, listening port %d...\n", port);
				printf("Please access http://%s:%d from you web browser.\n", sip.c_str(), port);
#ifdef _WIN32
				std::string lh = "start http://" + sip + ":" + std::to_string(port);
				if (isopen)
					system(lh.c_str());
#endif
			}
			return ret;
		}

		static void after_uv_close_client(uv_handle_t* client) {
			PrintTID;
			http* hp = (http*)((Request*)client->data)->_user_data;
			hp->pushRC(client);
			//Request::destroy((Request*)client->data); //see: on_connection()
			//client->data = 0;
			//free(client);
		}

		static void after_uv_write(uv_write_t* w, int status) {
			PrintTID;
			psData ps = (psData)w->data;
			if (ps->data)
				free(ps->data); //copyed data
			if (ps->is_close)
			{
				if (!uv_is_closing((uv_handle_t*)w->handle))
				{
					uv_close((uv_handle_t*)w->handle, after_uv_close_client);
				}
			}
			free(ps);
			free(w);

		}

		//close the connect of a client
		static void close_client(uv_stream_t* client) {
			uv_close((uv_handle_t*)client, after_uv_close_client);
		}

		//write data to client
		//data: the data to be sent
		//len:  the size of data, can be -1 if data is string
		//need_copy_data: copy data or not
		//need_free_data: free data or not, ignore this arg if need_copy_data is non-zero
		//write_uv_data() will close client connection after writien success, because Http use short connection.
		static void write_uv_data(uv_stream_t* client, const void* data, unsigned int len, int need_copy_data, int need_free_data, bool isclose = true)
		{
			uv_buf_t buf;
			uv_write_t* w;
			void* newdata = (void*)data;

			if (data == NULL || len == 0) return;
			if (len == (unsigned int)-1)
				len = strlen((char*)data);

			if (need_copy_data) {
				newdata = malloc(len);
				memcpy(newdata, data, len);
			}

			buf = uv_buf_init((char*)newdata, len);
			w = (uv_write_t*)malloc(sizeof(uv_write_t));
			psData ps = (psData)malloc(sizeof(sData));
			w->data = ps;
			ps->is_close = isclose;
			ps->data = (need_copy_data || need_free_data) ? newdata : NULL;
			uv_write(w, client, &buf, 1, after_uv_write); //free w and w->data in after_uv_write()
		}

		void handle_request(uv_stream_t* client, const std::string& url, njson& request, std::vector<std::string>* data)
		{
			bool rf = false;
			if (route_func)
			{
				rf = route_func(url.c_str(), &request, client);
			}
			if (!rf && route)
			{
				njson response;
				response["statusCode"] = 200;
				response["heads"]["Content-Type"] = default_content_type;
				//NDEBUG
				route(url.c_str(), request, response);
#if 0
				try
				{
					route(url.c_str(), request, response);
				}
				catch (nlohmann::json::exception & e)
				{
					// output exception information  
					njson errordata = { {"error","json error"},{"message",e.what()},{"exception_id",e.id} };
					response["body"] = errordata.dump(2);
				}
				catch (const std::exception & e)
				{
					printf("message: %s\n", e.what());
				}
				catch (...)
				{
					printf("error\n");
				}
#endif
				sendToClient(client, request, response);
				return;
			}

			{

				_404_not_found(client, request, url.c_str());
			}
		}

		void sendToClient(uv_stream_t* client, const njson& request, njson& response)
		{
			PrintTID;
			if (response.find("body") == response.end())
			{
				njson errordata = { {"error","json error"},{"message","no data!"} };
				response["body"] = errordata.dump(2);
			}
			std::string content = jsont::getStr2(response["body"]);
			sendData(client, request, content, toInt(response["statusCode"]), response["heads"]);
		}

		void sendData(uv_stream_t* client, const njson& request, std::string content, int code, njson heads)
		{
			njson req = request;
			std::string status = getStatus(code);
			std::string respone = "HTTP/1.1 " + status + "\r\n";

			std::string ae = jsont::getStr(req["heads"]["Accept-Encoding"]);
			std::string ct = jsont::getStr(heads["Content-Type"]);

			std::string acao = (is_cors(toStr(req["heads"]["origin"])));
			if (acao.size() && heads.find("Access-Control-Allow-Origin") == heads.end())
			{
				// 指定允许其他域名访问
				heads["Access-Control-Allow-Origin"] = acao;
				heads["Vary"] = "Accept-Encoding, Origin";
			}
			// 只压缩文本
#ifdef __LIBZIP__H__
			bool isgzip = ct.find("text") != -1 || ct.find("json") != -1;
			if (ae.find("gzip") != -1 && isgzip && content.size() > 512)
			{
				// 压缩
				auto gd = Zip::gzcompress(content.data(), content.size());
				// 解压
				//auto avd = Zip::gzdecompress(gd.data(), gd.size());
				content = std::string((char*)gd.data(), gd.size());
				heads["Content-Encoding"] = "gzip";
			}
#endif
			for (auto it = heads.begin(); it != heads.end(); it++)
			{
				respone += it.key() + ": " + jsont::getStr(it.value()) + "\r\n";
			}

			respone += "Content-Length: " + std::to_string(content.size()) + "\r\n";
			respone += "\r\n";
			if (content.size() < 1024)
			{
				respone += content;
				write_uv_data(client, respone.data(), respone.size(), 1, 0);
			}
			else {
				write_uv_data(client, respone.data(), respone.size(), 1, 0, false);
				// send body
				write_uv_data(client, content.data(), content.size(), 1, 0);
			}
		}
		static std::string& getStatus(int code)
		{
			static std::unordered_map<int, std::string> s = {
			{ 100,"100 Continue" },//继续。客户端应继续其请求
			{ 101,"101 Switching Protocols" },//切换协议。服务器根据客户端的请求切换协议。只能切换到更高级的协议，例如，切换到HTTP的新版本协议
			{ 200,"200 OK" },//请求成功。一般用于GET与POST请求
			{ 201,"201 Created" },//已创建。成功请求并创建了新的资源
			{ 202,"202 Accepted" },//已接受。已经接受请求，但未处理完成
			{ 203,"203 Non-Authoritative Information" },//非授权信息。请求成功。但返回的meta信息不在原始的服务器，而是一个副本
			{ 204,"204 No Content" },//无内容。服务器成功处理，但未返回内容。在未更新网页的情况下，可确保浏览器继续显示当前文档
			{ 205,"205 Reset Content" },//重置内容。服务器处理成功，用户终端（例如：浏览器）应重置文档视图。可通过此返回码清除浏览器的表单域
			{ 206,"206 Partial Content" },//部分内容。服务器成功处理了部分GET请求
			{ 300,"300 Multiple Choices" },//多种选择。请求的资源可包括多个位置，相应可返回一个资源特征与地址的列表用于用户终端（例如：浏览器）选择
			{ 301,"301 Moved Permanently" },//永久移动。请求的资源已被永久的移动到新URI，返回信息会包括新的URI，浏览器会自动定向到新URI。今后任何新的请求都应使用新的URI代替
			{ 302,"302 Found" },//临时移动。与301类似。但资源只是临时被移动。客户端应继续使用原有URI
			{ 303,"303 See Other" },//查看其它地址。与301类似。使用GET和POST请求查看
			{ 304,"304 Not Modified" },//未修改。所请求的资源未修改，服务器返回此状态码时，不会返回任何资源。客户端通常会缓存访问过的资源，通过提供一个头信息指出客户端希望只返回在指定日期之后修改的资源
			{ 305,"305 Use Proxy" },//使用代理。所请求的资源必须通过代理访问
			{ 306,"306 Unused" },//已经被废弃的HTTP状态码
			{ 307,"307 Temporary Redirect" },//临时重定向。与302类似。使用GET请求重定向
			{ 400,"400 Bad Request" },//客户端请求的语法错误，服务器无法理解
			{ 401,"401 Unauthorized" },//请求要求用户的身份认证
			{ 402,"402 Payment Required" },//保留，将来使用
			{ 403,"403 Forbidden" },//服务器理解请求客户端的请求，但是拒绝执行此请求
			{ 404,"404 Not Found" },//服务器无法根据客户端的请求找到资源（网页）。通过此代码，网站设计人员可设置"您所请求的资源无法找到"的个性页面
			{ 405,"405 Method Not Allowed" },//客户端请求中的方法被禁止
			{ 406,"406 Not Acceptable" },//服务器无法根据客户端请求的内容特性完成请求
			{ 407,"407 Proxy Authentication Required" },//请求要求代理的身份认证，与401类似，但请求者应当使用代理进行授权
			{ 408,"408 Request Time-out" },//服务器等待客户端发送的请求时间过长，超时
			{ 409,"409 Conflict" },//服务器完成客户端的PUT请求是可能返回此代码，服务器处理请求时发生了冲突
			{ 410,"410 Gone" },//客户端请求的资源已经不存在。410不同于404，如果资源以前有现在被永久删除了可使用410代码，网站设计人员可通过301代码指定资源的新位置
			{ 411,"411 Length Required" },//服务器无法处理客户端发送的不带Content-Length的请求信息
			{ 412,"412 Precondition Failed" },//客户端请求信息的先决条件错误
			{ 413,"413 Request Entity Too Large" },//由于请求的实体过大，服务器无法处理，因此拒绝请求。为防止客户端的连续请求，服务器可能会关闭连接。如果只是服务器暂时无法处理，则会包含一个Retry-After的响应信息
			{ 414,"414 Request-URI Too Large" },//请求的URI过长（URI通常为网址），服务器无法处理
			{ 415,"415 Unsupported Media Type" },//服务器无法处理请求附带的媒体格式
			{ 416,"416 Requested range not satisfiable" },//客户端请求的范围无效
			{ 417,"417 Expectation Failed" },//服务器无法满足Expect的请求头信息
			{ 500,"500 Internal Server Error" },//服务器内部错误，无法完成请求
			{ 501,"501 Not Implemented" },//服务器不支持请求的功能，无法完成请求
			{ 502,"502 Bad Gateway" },//充当网关或代理的服务器，从远端服务器接收到了一个无效的请求
			{ 503,"503 Service Unavailable" },//由于超载或系统维护，服务器暂时的无法处理客户端的请求。延时的长度可包含在服务器的Retry-After头信息中
			{ 504,"504 Gateway Time-out" },//充当网关或代理的服务器，未及时从远端服务器获取请求
			{ 505,"505 HTTP Version not supported" },//服务器不支持请求的HTTP协议的版本，无法完成处理				
			};
			auto it = s.find(code);
			return it != s.end() ? it->second : s[200];
		}
		std::string is_cors(const std::string& rs)
		{
			std::string ret;
			//auto rs = req->heads_lower["origin"];
			for (auto& it : _origin)
			{
				int pos = rs.find(it);
				if (pos >= 0)
				{
					ret = rs;
					break;
				}
			}
			return ret;
		}
		// 跨域
		int OPTIONS(uv_stream_t* client, const njson& request, Request* req)
		{
			std::string ret; bool Credentials = false;
			char buf[1024];
			std::string origin = (is_cors(req->heads_lower["origin"]));
			int cd = 403;
			if (origin.size())
			{
				Credentials = true;
				cd = 200;
				//_404_not_found(client, request, "");
				//return 0;
				//acao = "";
			}
			//ret += "Access-Control-Allow-Origin: *\r\n";
			//ret += "Access-Control-Allow-Headers:Authorization,access-token,Accept,x-requested-with,access-control-allow-credentials,access-control-allow-headers,content-type,mode\r\n";
			//ret += "Access-Control-Allow-Headers:Origin, X-Requested-With, Content-Type, Accept, Authorization";

			ret += "<HTML><HEAD><TITLE>OPTIONS\r\n";
			ret += "</TITLE></HEAD>\r\n";
			ret += "<BODY><P>HTTP request method OPTIONS.\r\n";
			ret += "</BODY></HTML>\r\n";
			njson heads;
			heads["Content-Type"] = "text/plain";
			heads["Access-Control-Allow-Credentials"] = Credentials;
			heads["Access-Control-Allow-Headers"] = "X-Token,X-PINGOTHER,Content-Type,Access-Token";
			heads["Access-Control-Allow-Origin"] = origin;
			heads["Access-Control-Allow-Methods"] = "POST,GET,OPTIONS";
			heads["Access-Control-Max-Age"] = "86400";
			heads["Keep-Alive"] = "timeout=2, max=100";
			heads["Vary"] = "Accept-Encoding, Origin";
			//printf(heads.dump(1).c_str());
			//printf(ret.data());
			//printf(req->_data.c_str());
			sendData(client, request, ret, 200, heads);
			return 0;
		}

		char* format_http_respone(const char* status, const char* content_type, const void* content, int content_length, int* respone_size) {
			int totalsize, header_size;
			char* respone;
			if (content_length < 0)
				content_length = content ? strlen((char*)content) : 0;
			totalsize = strlen(status) + strlen(content_type) + content_length + 128;
			respone = (char*)malloc(totalsize);
			header_size = sprintf(respone, "HTTP/1.1 %s\r\n"
				"Content-Type:%s;charset=utf-8\r\n"
				"Content-Length:%d\r\n\r\n",
				status, content_type, content_length);
			assert(header_size > 0);
			if (content) {
				memcpy(respone + header_size, content, content_length);
			}
			if (respone_size)
				*respone_size = header_size + content_length;
			return respone;
		}
		void format_http_respone(const char* status, const char* content_type, const void* content, int content_length, std::string& respone) {
			int totalsize, header_size;
			if (content_length < 0)
				content_length = content ? strlen((char*)content) : 0;
			totalsize = strlen(status) + strlen(content_type) + content_length + 128;
			respone.resize(totalsize);
			header_size = sprintf((char*)respone.data(), "HTTP/1.1 %s\r\n"
				"Content-Type:%s;charset=utf-8\r\n"
				"Content-Length:%d\r\n\r\n",
				status, content_type, content_length);
			assert(header_size > 0);
			if (content) {
				memcpy((char*)respone.data() + header_size, content, content_length);
			}
			return;
		}

		void _404_not_found(uv_stream_t* client, const njson& request, const char* pathinfo) {
			char* respone;
			char buffer[1024];
			snprintf(buffer, sizeof(buffer), "<h2>404 Not Found</h2><p>%s</p>", pathinfo);

			njson heads = { { "Content-Type","text/html" } };
			sendData(client, request, buffer, 404, heads);
		}

		//invoked by Http when a file is request. see on_request_get()
		void _on_send_file(uv_stream_t* client, const njson& request, const char* content_type, const char* file, const char* pathinfo)
		{
			int file_size, read_bytes, respone_size;
			unsigned char* file_data, * respone;
			auto fd = File::read_binary_file(file);
			if (fd.size())
			{
				njson heads = { {"Content-Type",content_type } };
				sendData(client, request, std::string(fd.data(), fd.size()), 200, heads);
				return;
			}
			_404_not_found(client, request, pathinfo);
		}
		bool is_file(const char* file)
		{
			bool ret = File::fn_access(file);
			return ret;
		}
		//only identify familiar postfix currently
		static const char* _get_content_type(const char* postfix) {
			if (strcmp(postfix, "html") == 0 || strcmp(postfix, "htm") == 0)
				return "text/html";
			else if (strcmp(postfix, "js") == 0)
				return "text/javascript";
			else if (strcmp(postfix, "css") == 0)
				return "text/css";
			else if (strcmp(postfix, "jpeg") == 0 || strcmp(postfix, "jpg") == 0)
				return "image/jpeg";
			else if (strcmp(postfix, "png") == 0)
				return "image/png";
			else if (strcmp(postfix, "gif") == 0)
				return "image/gif";
			else if (strcmp(postfix, "txt") == 0)
				return "text/plain";
			else
				return "application/octet-stream";
		}

		static void on_uv_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
			*buf = uv_buf_init((char*)malloc(suggested_size), suggested_size);
		}

		static void on_uv_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
		{
			http* hs = (http*)((Request*)client->data)->_user_data;
			char* crln2, * body = 0;
			Request* req = (Request*)client->data;
			if (nread > 0)
			{
				assert(req);
				req->append_data(buf->base, nread);
#ifdef _DEBUG_accept
				std::string bs(buf->base, nread);
				std::string fn = Crc::crc32_str((char*)bs.data(), bs.size());
				std::string pa = "temp/";
				File::check_make_path(pa);

				FILE* fp;
				time_t t;
				fp = fopen((pa + std::to_string((int64_t)client)).c_str(), "a");
				if (fp)
				{
					fwrite(bs.c_str(), 1, bs.size(), fp);
					fclose(fp);
				}
				//File::save_binary_file(pa + fn, (char*)bs.data(), bs.size());
#endif
			}
			if (req->is_accept())
			{
#ifdef _thread_pool_v
				auto ts = task_struct::create();
				ts->add_task2(&http::accept_request, hs, client);
				hs->async_get(ts);
#else
				hs->accept_request(client);
#endif // _thread_pool_v
			}
			if (nread == UV_EOF || nread == 0)
			{
			}
			if (nread == -1)
			{
				close_client(client);
			}

			if (buf->base)
				free(buf->base);

			((uv_buf_t*)buf)->len = nread;
		}
		int accept_request(uv_stream_t* client)
		{
			PrintTID;
			Timer t;
			Request* pr = (Request*)client->data;
			AutoReq ar(pr);
			printf("\x1b[32m [%s] [INFO]\x1b[0m - %s \x1b[33m%gms\x1b[0m\n", Date::getDateNow().c_str(), "accept_request", 0);
			if (!pr || pr->headv.empty())
			{
				printf("accept_request error\n");
				return -4;
			}
			std::vector<std::string> dat;
			hstring::split(pr->get_heads(), "\r\n", dat);
			std::string method = dat[0];
			if (method == "")
			{
				try
				{
					close_client(client);
				}
				catch (const std::exception&)
				{
				}
				//unimplemented(client);
				return -5;
			}
#if 1
			method = pr->headv[0];
			std::map<std::string, std::string> heads;
			std::string tembd;
			for (size_t i = 1; i < dat.size(); ++i)
			{
				auto& it = dat[i];
				auto pos = it.find(':');
				if (pos != -1)
				{
					std::string k = hstring::toLower(it.substr(0, pos));
					heads[k] = hstring::trim(it.substr(pos + 1));
				}
			}
			if (method.find("OPTIONS") != std::string::npos)
			{
				OPTIONS(client, { { "heads", heads } }, pr);
				return 0;
			}
			char* query_string = NULL;
			std::string surl;
			if (method == "GET")
			{
				query_string = (char*)pr->headv[1].c_str();
				while ((*query_string != '?') && (*query_string != '\0'))
					query_string++;
				if (*query_string == '?')
				{
					//cgi = 1;
					*query_string = '\0';
					surl = pr->headv[1].c_str();
					*query_string = '?';
					query_string++;
				}
				else
				{
					surl = pr->headv[1].c_str();
				}
			}
			std::vector<std::string> urls;
			hstring::split(pr->headv[1], "?", urls);

			std::string qst = query_string ? query_string : (urls.size() > 1 ? urls[1] : "");
			njson qget;
			//std::string boundary = hstring::sub_str(pr->_data_heads, "boundary=", "\r\n");
			long long length = std::atoll(heads["content-length"].c_str());

			// 解析body
			size_t headlen = pr->content_len;

			static std::map<std::string, std::function<void(Request * pr, njson & q, const std::string & qstr)>> ctfb = {
				{ "application/x-www-form-urlencoded", [](Request* pr,njson& q,const std::string& qstr) {
				auto qs = hstring::split(qstr, "&");
				for (auto& it : qs)
				{
					if (it != "")
					{
						auto v = hstring::split(it, "=");
						if (v.size() > 1)
						{
							njson mv = jsont::makeValue(v[1]);
							if (mv.is_string())
							{
								mv = uristr::URLDecode(jsont::getStr(mv));
							}
							q[v[0]] = mv;
						}
					}
				}}}
				,{ "multipart/form-data", [](Request* pr,njson& q,const std::string& qstr) {
#if 0
					//std::string boundary = hstring::sub_str(qstr,"boundary=","\r\n");
					std::string mfd = boundary.size() ? hstring::sub_str(qstr, "--" + boundary, boundary + "--") : "";
					auto mv = hstring::split(mfd, boundary);
					// auto bd = get_boundary(dtm);
					for (auto& it : mv)
					{
						auto nv = hstring::sub_str(it, "name=\"", "\r\n--");
						auto v = hstring::split(nv, "\r\n\r\n");
						if (v.size() > 1)
							q[v[0]] = jsonT::makeValue(v[1]);
					}
#endif
					std::string boundary = hstring::sub_str(pr->_data_heads, "boundary=", "\r\n");
					char* dtm = (char*)qstr.data();
					int64_t dlen = qstr.size();
					std::string n; std::string c;
					njson& fd = q;
					for (; dtm && *dtm;)
					{
						char* tm = mutipart_form_data(dtm, dlen, boundary,n,c);
						if (!tm)
						{
							break;
						}
						dlen -= tm - dtm;
						dtm = tm;
						if (n.find("filename") == std::string::npos)
						{
							n = hstring::trim(n, "\"");
							fd[n] = jsont::makeValue(c);
						}
						else
						{
							auto k = gethkv(n,fd,c);
							if (k)
							{
								fd[n] = c;
							}
						}
					}
					if (fd.size())
					{
						//printf("\n");
					}
					return;
				}}
					,{ "application/json",
					[](Request* pr,njson& q,const std::string& qstr) {
					try
					{
						q = njson::parse(qstr);
					}
					catch (nlohmann::json::exception & e)
					{
						// output exception information  
						printf("message: %s\nexception id: %d\n", e.what(), e.id);
					}
				}
				}
			};

			njson qpost;
			auto cts = heads.find("content-type");
			if (cts != heads.end())
			{
				auto vt = hstring::split(cts->second, ";");
				if (jsont::is_json(pr->_data_body.data(), pr->_data_body.size()))
				{
					qpost = njson::parse(pr->_data_body);
				}
				else
				{
					auto fb = ctfb.find(vt[0]);
					if (fb != ctfb.end())
					{
						// heads.find("Content-Disposition") != heads.end() ? *dat.rbegin() : "";
						fb->second(pr, qpost, pr->_data_body);
					}
				}
			}
			if (qst.size())
			{
				ctfb["application/x-www-form-urlencoded"](pr, qget, qst);
			}

			std::map<std::string, std::string> vk;
			std::string path = _doc_root_path;
			if (pr->headv.size() > 1)
			{
				path.append(pr->headv[1]);
				struct stat st;
				if (stat(path.c_str(), &st) == -1)
				{
					std::string ph = jsont::getStr(heads["Referer"]);
					ph = hstring::replace(ph, "//", "");
					std::regex rg1("/((?:.*))/"); std::smatch r1;
					if (std::regex_search(ph, r1, rg1))
					{
						ph = r1[0].str();
						if (*ph.rbegin() == '/')
							ph.pop_back();
						pr->headv[1] = ph + pr->headv[1];
						path = _doc_root_path + pr->headv[1];
					}
					ph = File::getPath(ph.c_str(), File::pathdir);
				}
			}
			njson req = { { "heads", heads },{ "uns",pr->headv },{ "get",qget },{ "post",qpost } };
			if (pr->_data_body.size())
			{
				req["body"] = pr->_data_body;
			}
			auto dpos = surl.find(".");
			bool isf = is_file(path.c_str());
			if (dpos != -1)
			{
				printf("http file:\t%s\n", path.c_str());
			}
			if (dpos != -1 && isf)
			{
				if (_doc_root_path.size())
				{
					auto postfix = File::getPath(surl.c_str()); postfix.erase(postfix.begin());
					_on_send_file(client, req, _get_content_type(postfix.c_str()), path.c_str(), surl.c_str());
				}
				else {
					_404_not_found(client, req, surl.c_str());
				}
			}
			else
			{
				handle_request(client, urls[0], req, &dat);
			}
			//int bb = t.elapsed();
#endif
			int us = t.elapsed_micro();
			double bb = (double)us / 1000.0;
			printf("\x1b[32m [%s] [INFO]\x1b[0m - %s \x1b[33m%gms\x1b[0m\n", Date::getDateNow().c_str(), dat[0].c_str(), bb);
			return 0;
		}

		static void on_connection(uv_stream_t* server, int status)
		{
			PrintTID;
			http* hs = gs();
			uv_stream_t* _servers = (uv_stream_t*)&hs->_server;
			assert(server == _servers);
			if (status == 0) {
				uv_tcp_t* client = hs->getRC();// (uv_tcp_t*)malloc(sizeof(uv_tcp_t));

				uv_tcp_init(hs->_loop, client);
				uv_accept(server, (uv_stream_t*)client);
				uv_read_start((uv_stream_t*)client, on_uv_alloc, on_uv_read);

				//write_uv_data((uv_stream_t*)client, http_respone, -1, 0);
				//close client after uv_write, and free it in after_uv_close()
			}
		}
		void pushRC(void* c)
		{
			LOCK_W(_lock_rc);
			_rc.push((uv_tcp_t*)c);
		}

		uv_tcp_t* getRC()
		{
			uv_tcp_t* client = nullptr;
			{
				LOCK_W(_lock_rc);
				if (_rc.size() > 0)
				{
					client = _rc.front();
					Request::init(client->data);
					_rc.pop();
					_runc.push(client);
				}
			}
			if (!client)
			{
				client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
				client->data = Request::create((uv_stream_t*)client);
				((Request*)client->data)->_user_data = this;
			}
			return client;
		}
	public:
		std::string get_boundary(char* mfd)
		{
			char* p, * end;
			std::string boundary;
			int boundary_size;

			if ((p = strstr(mfd, "boundary=")) == NULL) {
				return "";
			}

			p += 9;
			if ((end = strstr(p, "\r\n")) == NULL) {
				return "";
			}

			boundary_size = end - p + 1;

			boundary.assign(p, boundary_size);
			//boundary[boundary_size - 1] = 0;

			return boundary;
		}


		static char* mutipart_form_data(char* mfd, int64_t len, std::string boundary, std::string& name, std::string& content)
		{
			char* p, * end;
			int size;
			int boundary_size;

			name = "";
			if ((p = strstr(mfd, "name=")) == NULL) {
				return nullptr;
			}
			p += 6;
			if ((end = strchr(p, '\"')) == NULL) {
				return nullptr;
			}

			// 获取name
			auto pos = strstr(p, "\r\n\r\n");
			name.assign(p, pos - p);
			// 获取内容
			pos += 4;
			boundary = "\r\n--" + boundary;
			char* pose = 0;
			int64_t bl = boundary.size();
			len -= pos - mfd;
			p = pos;
			auto mp = hstring::sunday_search(pos, boundary.c_str(), len, bl);
			if (mp < 0)
			{
				content = pos;
			}
			else
			{
				content = std::string(pos, mp);
			}
			mfd = pos + boundary.size() + mp;
			return mfd;
			if ((end = strstr(p, boundary.c_str())) == NULL) {
				return nullptr;
			}

			boundary_size = boundary.size();// strlen(boundary);
			mfd = end + boundary_size;

			while (*p == '\r' || *p == '\n') {
				p++;
			}

			end -= 3;
			while (*end == '\r' || *end == '\n') {
				end--;
			}

			if (p > end) {
				return nullptr;
			}

			size = end - p + 2;
			content = std::string(p, size - 1);
			if (*(mfd + 1) == '-') {
				// 说明到了最后了
				return nullptr;
			}
			return mfd;
		}

		static bool gethkv(const std::string& n, njson& vo, std::string& data)
		{
			auto k = n.substr(0, n.find("\""));
			char* t = (char*)n.data() + k.size() + 3;
			if (k.empty() || !t || !*t)return true;
			auto vs = hstring::split_m(t, "\r\n");
			njson& v = vo[k];
			bool ret = false;
			for (auto it : vs)
			{
				auto ks = hstring::split_m(it, "=:");
				if (ks.size() > 1)
				{
					v[ks[0]] = hstring::trim(ks[1].c_str() + 1, "\"");
				}
			}
			v["data"] = data;
			return ret;
		}
	private:
	};

}//!hz
#endif //__HTTP_H__
#if 0
// 用法
//#include "json.hpp"
//using njson = nlohmann::json;
int port = 8090;
int ret = 0;
bool iso = false;
std::string ips, root = webroot;

auto loop = uv_default_loop();
hz::http::gs()->set_default_content_type("application/json");
hz::http::gs()->route = [=](const char* pathinfo, const njson& request, njson& response) {
	try
	{
		//处理请求
		//std::vector<std::uint8_t> v_cbor = njson::to_cbor(request);
		//RTFct ct = { nullptr, pathinfo, v_cbor.data(), v_cbor.size(), write_json, &response, nullptr };
		//auto cret = hz::RouteTable::s()->call(&ct);
	}
	catch (nlohmann::json::exception & e)
	{
		// output exception information  
		auto ew = e.what();
		njson errordata = { {"error","json error"},{"exception_id",e.id} };
		if (ew)
		{
			errordata["message"] = ew;
		}
		response["body"] = errordata.dump(2);
	}
};
hz::http::gs()->start(loop, ips.c_str(), port++, root.c_str(), iso);

#endif // 0
