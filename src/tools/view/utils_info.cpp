#if 0
#include <libuv/uv.h>
#include <libuv/uvw/util.hpp>
#include <base/ecc_sv.h>
#include <sem_async_uv.h>
#include <palloc/palloc.h>
#include <base/print_time.h>
#include <base/smq.h>
#endif
#include <base/hlUtil.h>
#include <data/json_helper.h>
#include <base/sem.h>
//#include "comm.h"
#include <view/file.h>
#include <view/mem_pe.h>
#include <iconv.h>
#include "utils_info.h"

namespace hz {

	typedef struct iconv_info_c {
		void* handle = nullptr;
		iconv_t(*iconv_open)(const char* tocode, const char* fromcode);
		size_t(*iconv)(iconv_t cd, char** inbuf, size_t* inbytesleft, char** outbuf, size_t* outbytesleft);
		int (*iconv_close)(iconv_t cd);
		void (*iconvlist)(int (*do_one) (unsigned int namescount, const char* const* names, void* data), void* data);
	}iconv_info_t;
	int do_one_ic(unsigned int namescount, const char* const* names, void* data)
	{
		auto o = (std::set<std::string>*)data;
		if (o && names && namescount)
		{
			for (size_t i = 0; i < namescount; i++)
			{
				o->insert(names[i]);
			}

		}
		return 0;//返回1中断枚举
	}
#ifdef _WIN32
	const char* exts = ".dll";
#else
	const char* exts = ".so";
#endif // _WIN32
	static iconv_info_t ic = {};
	static std::once_flag icof;
	static std::set<std::string> lsic;
	void init_iconv()
	{
		std::call_once(icof, []() {
			std::string dlln = "libiconv";
			auto iconvso = Shared::loadShared(dlln + exts);
			if (!iconvso)return;
			ic.handle = iconvso;
			static const char* ccfn[] = { "libiconv_open","libiconv","libiconv_close","libiconvlist" };
			iconvso->dllsyms(ccfn, (void**)&ic.iconv_open, sizeof(ccfn) / sizeof(char*));
			if (ic.iconvlist)
			{
				ic.iconvlist(do_one_ic, &lsic);
				//for (auto& it : lsic)printf("%s\n", it.c_str());
			}
			});
	}

	std::string str_iconv(const char* str, int len, const char* tocode, const char* fromcode)
	{
		std::string ret;
		init_iconv();
		assert(ic.libiconv);
		if (!ic.iconv || !ic.iconv_open || !ic.iconvlist || !ic.iconv_close)
		{
			return ret;
		}
		char* inbuf = (char*)str;
		size_t ac = len > 0 ? len : strlen(inbuf);
		size_t acc = ac;
		iconv_t cd = ic.iconv_open(tocode, fromcode);
		size_t icd = (size_t)cd;
		assert(icd != -1);
		if (icd == -1)
		{
			return ret;
		}
		size_t k = 0;
		const int bs = 1024;
		char buf[bs] = {};
		for (; 0 < acc;)
		{
			auto ibs = (bs > acc) ? acc : bs;
			size_t il = ibs, ol = bs;
			auto otb = buf;
			k = ic.iconv(cd, &inbuf, (size_t*)&il, &otb, &ol);
			acc -= ibs - il;
			if (bs - ol > 0)
				ret.append(buf, bs - ol);
			else
				break;
		}
		int nErr = 0;
		if (k != 0) {
			nErr = errno;
			switch (nErr)
			{
			case E2BIG:
			{
				printf("errno:E2BGI（OutBuf空间不够）\n");
				break;
			}
			case EILSEQ:
			{
				printf("errno:EILSEQ（InBuf多字节序无效）\n");
				ret.assign(str, len);
				break;
			}
			case EINVAL:
			{
				printf("errno:EINVAL（有残留的字节未转换）\n");
				break;
			}
			default:
				break;
			}
		}
		//printf("%s\n", ret.c_str());
		ic.iconv_close(cd);
		return ret;
	}
	std::string str_iconv(const std::string& str, const char* tocode, const char* fromcode)
	{
		return str_iconv(str.c_str(), str.size(), tocode, fromcode);
	}
	std::set<std::string> get_iconv_list()
	{
		init_iconv();
		return lsic;
	}
	std::string u8_gbk(const std::string& str)
	{
		return str_iconv(str.c_str(), str.size(), "GBK", "UTF-8");
	}
	std::string gbk_u8(const std::string& str)
	{
		return str_iconv(str.c_str(), str.size(), "UTF-8", "GBK");
	}
	const char* get_wp() {
		static std::string _utils_wiv = "UTF-" + std::to_string(sizeof(wchar_t) * 8) + "LE";
		return _utils_wiv.c_str();
	}
	std::wstring u8_w(const std::string& str)
	{

		auto d = str_iconv(str.c_str(), str.size(), get_wp(), "UTF-8");
		return std::wstring((wchar_t*)d.c_str(), d.size() / sizeof(wchar_t));
	}

	std::string w_u8(const std::wstring& str)
	{
		return str_iconv((char*)str.c_str(), str.size() * sizeof(wchar_t), "UTF-8", get_wp());
	}
	std::string w_gbk(const std::wstring& str)
	{
		return str_iconv((char*)str.c_str(), str.size() * sizeof(wchar_t), "GBK", get_wp());
	}
	std::wstring gbk_w(const std::string& str)
	{
		auto d = str_iconv(str.c_str(), str.size(), get_wp(), "GBK");
		return std::wstring((wchar_t*)d.c_str(), d.size() / sizeof(wchar_t));
	}
	std::string w2a(const std::wstring& str, bool isu8)
	{
		return str_iconv((char*)str.c_str(), str.size() * sizeof(wchar_t), isu8 ? "UTF-8" : "GBK", get_wp());
	}
	std::wstring a2w(const std::string& str, bool isu8)
	{
		auto d = str_iconv(str.c_str(), str.size(), get_wp(), isu8 ? "UTF-8" : "GBK");
		return std::wstring((wchar_t*)d.c_str(), d.size() / sizeof(wchar_t));
	}
	std::u16string u8_u16(const std::string& str)
	{
		auto d = str_iconv(str.c_str(), str.size(), "UTF-16LE", "UTF-8");
		return std::u16string((char16_t*)d.c_str(), d.size() / sizeof(char16_t));
	}
	std::u16string gbk_u16(const std::string& str)
	{
		auto d = str_iconv(str.c_str(), str.size(), "UTF-16LE", "GBK");
		return std::u16string((char16_t*)d.c_str(), d.size() / sizeof(char16_t));
	}
	std::u32string u8_u32(const std::string& str)
	{
		auto d = str_iconv(str.c_str(), str.size(), "UTF-32LE", "UTF-8");
		return std::u32string((char32_t*)d.c_str(), d.size() / sizeof(char32_t));
	}
	std::u32string gbk_u32(const std::string& str)
	{
		auto d = str_iconv(str.c_str(), str.size(), "UTF-32LE", "GBK");
		return std::u32string((char32_t*)d.c_str(), d.size() / sizeof(char32_t));
	}
	//----------------------------------------------------------------------------------------------
	std::string join(const njson& v, const std::string& sub)
	{
		std::string ret;
		size_t i = 0;
		for (auto& it : v)
		{
			if (i > 0)ret += sub;
			i++;
			ret += toStr(it);
		}
		return ret;
	}
	njson read_json(std::string fn)
	{
		if (fn[1] != ':' && fn[0] != '/')
		{
			fn = hz::File::getAP(fn);
		}
		auto v = hz::File::read_binary_file(fn);
		njson ret;
		if (v.size())
		{
			try
			{
				int bs = boms(v.data());
				if (hz::is_json(v.data() + bs, v.size() - bs))
				{
					ret = njson::parse(v.begin() + bs, v.end());
				}
				else
				{
					ret = njson::from_cbor(v.begin() + bs, v.end());
				}
			}
			catch (const std::exception& e)
			{
				printf("parse json:\t%s\n", e.what());
			}
		}
		return ret;
	}
	// 保存json到文件，-1则保存成cbor
	void save_json(const njson& n, std::string fn, int indent_cbor)
	{
		if (fn[1] != ':' && fn[0] != '/')
		{
			fn = hz::File::getAP(fn);
		}
		if (indent_cbor < 0)
		{
			auto s = njson::to_cbor(n);
			if (s.size())
				hz::File::save_binary_file(fn, (char*)s.data(), s.size());
		}
		else
		{
			std::string s;
			if (indent_cbor <= 0)
			{
				s = n.dump();
			}
			else
			{
				s = n.dump(indent_cbor);
			}
			if (s.size())
				hz::File::save_binary_file(fn, (char*)s.data(), s.size());
		}
	}
}
//!hz
