#pragma once
#include <string>
#include <map>
#include <vector>

#if (__has_include(<base/ecc_sv.h>))
#include <base/ecc_sv.h>
#endif
#include <data/json_helper.h>
#include <base/promise_cx.h>
#if 1
namespace hz
{
	class res_data
	{
	public:
		std::string url;
		std::string data;
		const char* cdata = nullptr;
		int clen = 0;
		std::map<std::string, std::string> formdata;
		std::string form_urlencoded;
		njson* ndata = nullptr;

		std::map<std::string, std::string> req_headers;
		std::vector<std::string> mkheaders;
		bool ismk = false;
		void* mimeptr = nullptr;
		// return
		std::string body;
		std::vector<std::string> header;
		std::string raw_data;
		// 解析后的数据
		njson _header, _header_lower;
		njson _nbody;
		std::string content_type;

	public:
		res_data();
		res_data(const char* u);
		res_data(const std::string& u);
		res_data(const std::string& u, njson* nd);
		~res_data();
	public:
		void set_header(const std::string& k, const std::string& v);
		void set_header(const njson& n);
		void set_body(const char* d, int len, const std::string& ct = "");
		void set_body(std::string& d, const std::string& ct = "");

		void set_body(njson* nd);
		void set_body_url(njson* nd);
		bool is_bodyjson();
	public:
		void mk(bool isget);
		void mk_header();
	public:

#if 1
		static std::string toLower(const std::string& str);
		static std::string toUpper(const std::string& str);
		static char Dec2HexChar(short int n);
		static short int HexChar2Dec(char c);

		static std::string EncodeURL(const std::string& URL);

		static std::string DecodeURL(const std::string& URL);
		static std::string json2url(njson& n);
#endif
	private:

	};
	// 初始化curl全局
	class curl_global_cx
	{
	public:
	public:
		curl_global_cx();

		~curl_global_cx();
	public:

	private:

	};
	class curl_m_cx :public promise_cx
	{
	private:
		typedef void CURLM;
		CURLM* cm = nullptr;
		std::vector<res_data> urls;
		std::string method;
		int max_wait_msecs = 30;
	public:
		curl_m_cx();
		~curl_m_cx();
		void set(std::vector<std::string> url);
		res_data* send_get(std::string url);
		res_data* send_post(std::string url, njson* ndata);

		res_data* get_req(int idx);
		std::string* get_body(int idx);
		njson* get_nbody(int idx);
		njson get_header(int idx, njson* out_header);
	private:
		bool cm_init();
		bool pools();
		bool get_info();
		bool cleanup();
	private:

		void get_curl_info(CURLM* cm, int* msgs_left);
		void init_easy(CURLM* cm, res_data* r, const std::string& url, const std::string& method);
		static size_t cb(char* d, size_t n, size_t l, void* p);
		static size_t header_cb(const char* ptr, size_t size, size_t nmemb, void* p);
	public:

	};


	class curl_example
	{
	private:
		std::string url;
		std::string token;
		std::string skey;

#ifdef ecc_sv_h
		// 验证登录用
		ecc_c mc;
		// 通讯用
		ecc_c c;
#endif // ecc_sv_h
		const void* cipher = nullptr;

		promise_run* ctx = nullptr;
	public:
		curl_example()
		{
#ifdef ecc_sv_h
			// todo加载mc私钥

			// 新建通讯私钥
			c.new_key();
			// skey使用登录返回的公钥和c计算出
#endif
		}

		~curl_example()
		{
		}
		void run()
		{

			send_post("/mag/cipher_list", nullptr, [=](curl_m_cx* ccm) { /*on_done(ccm);*/ });
		}
	private:

		void set_next()
		{
			//promise::set_next();
		}
		// 计算签名、加密
		void de_data(njson* pv, res_data* req, bool is_sign)
		{
			if (!pv || pv->empty())return;
			if (is_sign && pv->is_object())
			{
				std::string s;
				njson& v = *pv;
				for (auto& [k, va] : v.items())
				{
					s.append(k + toStr(va));
				}

#ifdef ecc_sv_h
				// 计算摘要
				auto dgst = ecc_c::sha256(s.c_str(), s.size());
				// 计算签名，使用登录的私钥mc
				auto sig = mc.do_sign(dgst);
				v["sign"] = ecc_c::b64_encode(sig);
#endif
			}
			if (!req)return;
			if (skey.size())
			{
				auto sbd = pv->dump();
				std::string edb;

#ifdef ecc_sv_h
				ecc_c::decrypt(cipher, sbd.data(), sbd.size(), skey, edb);
				if (edb.empty())
					return;
#else
				edb = sbd;
#endif
				req->set_body(edb);
			}
			else {
				req->set_body(pv);
			}
		}

		curl_m_cx* send_get(const std::string& path, njson* v, std::function<void(curl_m_cx*)> cb)
		{
			assert(ctx);
			curl_m_cx* ccm = new curl_m_cx();
			de_data(v, nullptr, token.empty());
			auto req = ccm->send_get(url + path + "?" + res_data::json2url(*v));
			if (token.size())
			{
				req->set_header("token", token);
			}

			ccm->set_done_cb([=]() { if (cb) { cb(ccm); } { set_next(); delete ccm; } });
			// 提交异步执行
			ctx->push(ccm);
			return ccm;
		}
		curl_m_cx* send_post(const std::string& path, njson* v, std::function<void(curl_m_cx*)> cb)
		{
			assert(ctx);
			curl_m_cx* ccm = new curl_m_cx();
			auto req = ccm->send_post(url + path, nullptr);
			de_data(v, req, token.empty());
			if (token.size())
			{
				req->set_header("token", token);
			}

			ccm->set_done_cb([=]() { if (cb) { cb(ccm); } { set_next(); delete ccm; } });
			// 提交异步执行
			ctx->push(ccm);
			return ccm;
		}
	};


}
#endif
// !curl
