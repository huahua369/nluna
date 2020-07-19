
#include "curl_cx.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <curl/curl.h>
#ifdef __cplusplus
}
#endif
#if 1
namespace hz
{
	res_data::res_data()
	{
	}

	res_data::res_data(const char* u) :url(u)
	{
	}
	res_data::res_data(const std::string& u) : url(u)
	{
	}
	res_data::res_data(const std::string& u, njson* nd) : url(u), ndata(nd)
	{
	}

	res_data::~res_data()
	{
	}

	void res_data::set_header(const std::string& k, const std::string& v)
	{
		req_headers[k] = v;
	}
	void res_data::set_header(const njson& n)
	{
		for (auto& [k, v] : n.items())
		{
			req_headers[k] = toStr(v);
		}
	}
	void res_data::set_body(const char* d, int len, const std::string& ct)
	{
		if (d && len > 0)
		{
			cdata = d;
			clen = len;
			req_headers["Content-Type"] = ct.empty() ? "application/octet-stream" : ct;
		}
	}
	void res_data::set_body(std::string& d, const std::string& ct)
	{
		if (d.size())
		{
			data.swap(d);
			cdata = data.data();
			clen = data.size();
			req_headers["Content-Type"] = ct.empty() ? "application/octet-stream" : ct;
		}
	}

	void res_data::set_body(njson* nd)
	{
		ndata = nd;
		if (ndata && ndata->size())
		{
			data = ndata->dump();
			cdata = data.c_str();
			req_headers["Content-Type"] = "application/json";
		}
	}
	void res_data::set_body_url(njson* nd)
	{
		if (nd && nd->is_object() && nd->size())
		{
			form_urlencoded = json2url(*nd);
			if (form_urlencoded.size())
			{

				cdata = form_urlencoded.c_str();
				req_headers["Content-Type"] = "application/x-www-form-urlencoded";
			}
		}
	}
	bool res_data::is_bodyjson()
	{
		return _nbody.size();
	}

	void res_data::mk(bool isget)
	{
		if (ismk)return;
		ismk = true;
		for (auto& [k, v] : req_headers)
		{
			mkheaders.push_back(k + ": " + v);
		}
		bool isw = isget && url.rfind('?') == std::string::npos && form_urlencoded.size();
		if (isw)
		{
			url = url + "?" + form_urlencoded;
		}
	}
	void res_data::mk_header()
	{
		for (auto& it : header)
		{
			int pos = it.find(':');
			int pose = it.find(' ');
			std::string k, v;
			if (pos > 0)
			{
			}
			else if (pose > 0)
			{
				pos = pose;
			}
			k = it.substr(0, pos);
			if (k.substr(k.size() - 2) == "\r\n")
			{
				k.resize(k.size() - 2);
			}
			if (k.empty())continue;
			const char* t = it.c_str() + pos;
			for (; *t == ':' || *t == ' '; t++);
			v = t;
			if (v.substr(v.size() - 2) == "\r\n")
			{
				v.resize(v.size() - 2);
			}
			_header[k] = v;
			_header_lower[toLower(k)] = v;
		}
		content_type = toStr(_header_lower["content-type"]);
		if (content_type.find("/json") != std::string::npos)
		{
			try
			{
				_nbody = njson::parse(body);
			}
			catch (const std::exception& e)
			{
				printf("%s\n", e.what());
			}
		}
		return;
	}

#if 1
	// 转小写
	std::string res_data::toLower(const std::string& str_)
	{
		auto str = str_;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}
	// 转大写
	std::string res_data::toUpper(const std::string& str_)
	{
		auto str = str_;
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		return str;
	}
	char res_data::Dec2HexChar(short int n)
	{
		if (0 <= n && n <= 9) {
			return char(short('0') + n);
		}
		else if (10 <= n && n <= 15) {
			return char(short('A') + n - 10);
		}
		else {
			return char(0);
		}
	}

	short int res_data::HexChar2Dec(char c)
	{
		if ('0' <= c && c <= '9') {
			return short(c - '0');
		}
		else if ('a' <= c && c <= 'f') {
			return (short(c - 'a') + 10);
		}
		else if ('A' <= c && c <= 'F') {
			return (short(c - 'A') + 10);
		}
		else {
			return -1;
		}
	}

	std::string res_data::EncodeURL(const std::string& URL)
	{
		std::string strResult = "";
		for (unsigned int i = 0; i < URL.size(); i++)
		{
			char c = URL[i];
			if (
				('0' <= c && c <= '9') ||
				('a' <= c && c <= 'z') ||
				('A' <= c && c <= 'Z') ||
				c == '/' || c == '.'
				) {
				strResult += c;
			}
			else
			{
				int j = (short int)c;
				if (j < 0)
				{
					j += 256;
				}
				int i1, i0;
				i1 = j / 16;
				i0 = j - i1 * 16;
				strResult += '%';
				strResult += Dec2HexChar(i1);
				strResult += Dec2HexChar(i0);
			}
		}

		return strResult;
	}

	std::string res_data::DecodeURL(const std::string& URL)
	{
		std::string result = "";
		for (unsigned int i = 0; i < URL.size(); i++)
		{
			char c = URL[i];
			if (c != '%')
			{
				result += c;
			}
			else
			{
				char c1 = URL[++i];
				char c0 = URL[++i];
				int num = 0;
				num += HexChar2Dec(c1) * 16 + HexChar2Dec(c0);
				result += char(num);
			}
		}

		return result;
	}
	std::string res_data::json2url(njson& n)
	{
		std::string ret;
		if (n.is_object() && n.size())
		{
			for (auto& [k, v] : n.items())
			{
				if (ret.size())ret.push_back('&');
				ret += k + "=" + toStr(v);
			}
			ret = DecodeURL(ret);
		}
		return ret;
	}
#endif
	// curl_global_cx

	curl_global_cx::curl_global_cx()
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}

	curl_global_cx::~curl_global_cx()
	{
		curl_global_cleanup();
	}

	// curl_m_cx ::public promise_cx

	curl_m_cx::curl_m_cx()
	{
		insert1(this, &curl_m_cx::cm_init, &curl_m_cx::pools, &curl_m_cx::get_info, &curl_m_cx::cleanup);
	}

	curl_m_cx ::~curl_m_cx()
	{
	}
	void curl_m_cx::set(std::vector<std::string> url)
	{
		for (auto& it : url)
		{
			urls.push_back(res_data(it));
		}
	}
	res_data* curl_m_cx::send_get(std::string url)
	{
		method = "GET";
		urls.push_back(res_data(url));
		return &urls[urls.size() - 1];
	}
	res_data* curl_m_cx::send_post(std::string url, njson* ndata)
	{
		method = "POST";
		urls.push_back(res_data(url, ndata));
		return &urls[urls.size() - 1];
	}

	res_data* curl_m_cx::get_req(int idx)
	{
		if (idx > urls.size() || idx < 0)return nullptr;
		return &urls[idx];
	}
	std::string* curl_m_cx::get_body(int idx)
	{
		if (idx > urls.size() || idx < 0)return nullptr;
		return &urls[idx].body;
	}
	njson* curl_m_cx::get_nbody(int idx)
	{
		if (idx > urls.size() || idx < 0)return nullptr;
		return &urls[idx]._nbody;
	}
	njson curl_m_cx::get_header(int idx, njson* out_header)
	{
		if (idx > urls.size() || idx < 0)return nullptr;
		auto ph = &urls[idx];
		if (out_header)*out_header = ph->_header;
		return ph->_header_lower;
	}

	bool curl_m_cx::cm_init()
	{
		cm = curl_multi_init();
		if (method.empty())
		{
			method = "GET";
		}
		for (auto& u : urls) {
			init_easy(cm, &u, u.url, method);
		}
		return true;
	}
	bool curl_m_cx::pools()
	{
		int still_running = 0, msgs_left = 0;
		curl_multi_perform(cm, &still_running);
		int idx = 0;
		curl_waitfd* cw = nullptr;
		unsigned int extras = 0;
		max_wait_msecs = 30 * 1000; /* Wait max. 30 seconds */
		do {
			int numfds = 0;
			int res = curl_multi_wait(cm, cw, extras, max_wait_msecs, &numfds);
			if (res != CURLM_OK) {
				fprintf(stderr, "error: curl_multi_wait() returned %d\n", res);
				//return EXIT_FAILURE;
				break;
			}
			/*
			 if(!numfds) {
				fprintf(stderr, "error: curl_multi_wait() numfds=%d\n", numfds);
				return EXIT_FAILURE;
			 }
			*/
			get_curl_info(cm, &msgs_left);

		} while (0);
		if (still_running)
		{
			inc_idx(-1);
		}
		else
		{
			printf("");
		}
		return true;
	}
	bool curl_m_cx::get_info()
	{
		int  msgs_left = 0;
		return true;
	}
	bool curl_m_cx::cleanup()
	{
		curl_multi_cleanup(cm);
		return true;
	}


	void curl_m_cx::get_curl_info(CURLM* cm, int* msgs_left)
	{
		CURLMsg* msg = nullptr;
		while ((msg = curl_multi_info_read(cm, msgs_left))) {
			if (msg->msg == CURLMSG_DONE) {
				auto eh = msg->easy_handle;

				auto return_code = msg->data.result;
				if (return_code != CURLE_OK) {
					fprintf(stderr, "CURL error code: %d\n", msg->data.result);
					continue;
				}

				// Get HTTP status code
				int http_status_code = 0;
				res_data* rp = 0;
				curl_easy_getinfo(eh, CURLINFO_RESPONSE_CODE, &http_status_code);
				curl_easy_getinfo(eh, CURLINFO_PRIVATE, &rp);

				if (http_status_code == 200) {
					printf("200 OK for %s\n", rp->url.c_str());
				}
				else {
					fprintf(stderr, "GET of %s returned http status code %d\n", rp->url.c_str(), http_status_code);
				}

				curl_multi_remove_handle(cm, eh);
				if (rp->mimeptr)
					curl_mime_free((curl_mime*)rp->mimeptr);
				curl_easy_cleanup(eh);
				rp->mk_header();
			}
			else {
				fprintf(stderr, "error: after curl_multi_info_read(), CURLMsg=%d\n", msg->msg);
			}
		}
	}
	void curl_m_cx::init_easy(CURLM* cm, res_data* r, const std::string& url, const std::string& method)
	{
		assert(method.size());
		CURL* curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, r);

		//抓取头信息，回调函数   
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, r);

		curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		curl_easy_setopt(curl, CURLOPT_PRIVATE, r);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
		r->mk(method == "GET");
		// 设置头
		struct curl_slist* headers = NULL;
		for (auto& it : r->mkheaders)
		{
			headers = curl_slist_append(headers, it.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
#if 1
		// 设置formdata
		curl_mime* mime = nullptr;
		if (r->formdata.size())
		{
			curl_mimepart* part;
			mime = curl_mime_init(curl);
			for (auto& [k, v] : r->formdata)
			{
				part = curl_mime_addpart(mime);
				curl_mime_name(part, k.c_str());
				curl_mime_data(part, v.c_str(), CURL_ZERO_TERMINATED);
			}
			curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
			r->mimeptr = mime;
		}
#endif
		// 设置post数据
		if (method != "GET" && r->cdata)
		{
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, r->cdata);
			if (r->clen > 0)
				curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, r->clen);
		}
		if (cm)
		{
			curl_multi_add_handle(cm, curl);
		}
		else {
			auto res = curl_easy_perform(curl);
			curl_mime_free(mime);
			curl_easy_cleanup(curl);
		}
	}
	size_t curl_m_cx::cb(char* d, size_t n, size_t l, void* p)
	{
		auto c = (res_data*)p;
		auto ot = (std::string*) & c->body;
		auto s = n * l;
		if (ot)
		{
			ot->append(d, s);
		}
		return s;
	}
	size_t curl_m_cx::header_cb(const char* ptr, size_t size, size_t nmemb, void* p)
	{
		auto c = (res_data*)p;
		auto h = &c->header;
		auto s = size * nmemb;
		if (h)
			h->push_back({ ptr,s });
		return s;
	}
}
#endif
// !curl
