//2018/01/04

#ifndef __json_helper_h__
#define __json_helper_h__
#ifndef MAXUINT64
#define MAXUINT64   ((uint64_t)~((uint64_t)0))
#define MAXINT64    ((int64_t)(MAXUINT64 >> 1))
#define MININT64    ((int64_t)~MAXINT64)
#endif
#include <stdarg.h>
#include <ctype.h>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <vector>
#include <locale>
#include <codecvt>
#include <string>
#include <regex>

//#include "hlUtil.h"
//#include "hl_time.h"

#ifndef NO_JSONTOOL
#ifndef NJSON_H
//#define JSON_NOEXCEPTION
#include "json.hpp"
#if defined( NLOHMANN_JSON_HPP) || defined(INCLUDE_NLOHMANN_JSON_HPP_)
using njson = nlohmann::json;
#define NJSON_H
#endif
#endif

namespace hz
{
#if 1

	/*
	静态文本
#ifdef _STD_STR_
				std::wstring name_w;
				std::string name_a;
#else
				static_str<wchar_t> name_w;
				static_str<char> name_a;
#endif
	*/
	template<class T>
	class static_str
	{
	public:
		static_str()
		{
		}
		static_str(const T* s, size_t len) :str(s), _size(len)
		{
		}
		~static_str()
		{
		}
		static_str& assign(const T* s, size_t len)
		{
			str = s; _size = len;
			return *this;
		}
		std::wstring to_wstr(bool is_swap = false)
		{
			std::wstring ret;
			if (str && _size)
			{
				ret.assign(str, _size);
				if (is_swap)
				{
					char* temp = (char*)ret.data();
					for (int i = 0; i < _size; ++i)
					{
						temp = tt_ushort(temp);
					}
				}
			}
			return ret;
		}
		const char* c_str()
		{
			static std::string s;
			s = to_str();
			return s.c_str();
		}
		std::string to_str()
		{
			std::string ret;
			if (str && _size)
			{
				ret.assign((char*)str, _size);
			}
			return ret;
		}
		operator const T* ()
		{
			return str;
		}
		const T* data()
		{
			return str;
		}
		size_t size()
		{
			return _size;
		}
		static_str& operator= (const char* s)
		{
			assign(s, strlen(s));
			return *this;
		}
		static_str& operator= (const wchar_t* s)
		{
			assign(s, wcslen(s));
			return *this;
		}
		static char* tt_ushort(char* p)
		{
			std::swap(p[0], p[1]);
			return p + 2;
		}
	private:
		const T* str = 0;
		size_t _size = 0;
	};

	typedef static_str<wchar_t> w_string;
	typedef static_str<char> s_string;
	//--------------------------------------------------------------------------------------
	//全角转半角
	static void sbc_to_dbc(char* dst, const char* src)
	{
		char* dbc = dst, * sbc = (char*)src;
		for (; *sbc; ++sbc)
		{
			if ((*sbc & 0xff) == 0xA1 && (*(sbc + 1) & 0xff) == 0xA1)        //全角空格
			{
				*dbc++ = 0x20;
				++sbc;
			}
			else if ((*sbc & 0xff) == 0xA3 && (*(sbc + 1) & 0xff) >= 0xA1 && (*(sbc + 1) & 0xff) <= 0xFE)    //ASCII码中其它可显示字符
				* dbc++ = *++sbc - 0x80;
			else
			{
				if (*sbc < 0)    //如果是中文字符，则拷贝两个字节
					* dbc++ = *sbc++;
				*dbc++ = *sbc;
			}
		}
		*dbc = 0;
	}

	//半角转全角
	static void dbc_to_sbc(char* dst, const char* src)
	{
		char* dbc = (char*)src, * sbc = dst;
		for (; *dbc; ++dbc)
		{
			if ((*dbc & 0xff) == 0x20)    //半角空格
			{
				*sbc++ = 0xA1;
				*sbc++ = 0xA1;
			}
			else if ((*dbc & 0xff) >= 0x21 && (*dbc & 0xff) <= 0x7E)
			{
				*sbc++ = 0xA3;
				*sbc++ = *dbc + 0x80;
			}
			else
			{
				if (*dbc < 0)    //如果是中文字符，则拷贝两个字节
					* sbc++ = *dbc++;
				*sbc++ = *dbc;
			}
		}
		*sbc = 0;
	}
	//--------------------------------------------------------------------------------------
	template<class T>
	inline njson q2n(std::queue<T> q)
	{
		njson ret;
		for (; q.size();)
		{
			auto& it = q.front();
			ret.push_back(it);
			q.pop();
		}
		return ret;
	}
	template<class T>
	inline std::queue<T> n2q(njson n)
	{
		std::queue<T> ret;
		for (auto& it : n)
		{
			ret.push(it);
		}
		return ret;
	}
	template<class T>
	inline std::queue<njson> set2q(std::set<njson> n)
	{
		std::queue<njson> ret;
		for (auto& it : n)
		{
			ret.push(it);
		}
		return ret;
	}

	inline std::set<njson> q2set(std::queue<njson> q)
	{
		std::set<njson> ret;
		for (; q.size();)
		{
			auto& it = q.front();
			ret.insert(it);
			q.pop();
		}
		return ret;
	}
#endif

#ifndef _no_color_
#define UF_COLOR
	union u_col
	{
		unsigned int uc;
		unsigned char u[4];
		struct urgba
		{
			unsigned char r, g, b, a;
		}c;
	};

	static bool is_alpha(unsigned int c)
	{
		return (c & 0xFF000000);
	}
	inline unsigned int set_alpha(unsigned int c, unsigned int a)
	{
		u_col* t = (u_col*)& c;
		t->c.a = a;
		return c;
	}
	inline unsigned int multiply_alpha(unsigned int c, unsigned int a)
	{
		u_col* t = (u_col*)& c;
		double af = a / 255.0;
		t->c.r *= af;
		t->c.g *= af;
		t->c.b *= af;
		t->c.a = a;
		return c;
	}
	inline unsigned int set_alpha_f(unsigned int c, double af)
	{
		unsigned int a = af * 255;
		u_col* t = (u_col*)& c;
		t->c.a = a;
		return c;
	}
	inline unsigned int set_alpha_xf(unsigned int c, double af)
	{
		//unsigned int a = af * 255;
		u_col* t = (u_col*)& c;
		t->c.a *= af;
		return c;
	}
	inline unsigned int get_alpha(unsigned int c)
	{
		u_col* t = (u_col*)& c;
		return  t->c.a;
	}
	inline double get_alpha_f(unsigned int c)
	{
		u_col* t = (u_col*)& c;
		return  t->c.a / 255.0;
	}
	// 灰度 r=g=b  a
	inline unsigned int to_uint(glm::vec2 col)
	{
		u_col t;
		t.u[0] = col.x * 255;
		t.u[1] = col.x * 255;
		t.u[2] = col.x * 255;
		t.u[3] = col.y * 255;
		return t.uc;
	}
	// rgb
	inline unsigned int to_uint(glm::vec3 col)
	{
		u_col t;
		t.u[0] = col.x * 255;
		t.u[1] = col.y * 255;
		t.u[2] = col.z * 255;
		t.u[3] = 1 * 255;
		return t.uc;
	}
	// rgb
	inline unsigned int to_uint(glm::ivec3 col)
	{
		u_col t;
		t.u[0] = col.x;
		t.u[1] = col.y;
		t.u[2] = col.z;
		t.u[3] = 1 * 255;
		return t.uc;
	}
	// rgba
	inline unsigned int to_uint(glm::vec4 col)
	{
		u_col t;
		t.u[0] = col.x * 255;
		t.u[1] = col.y * 255;
		t.u[2] = col.z * 255;
		t.u[3] = col.w * 255;
		return t.uc;
	}
	// rgba
	inline unsigned int to_uint(float* col)
	{
		u_col t;
		t.u[0] = col[0] * 255;
		t.u[1] = col[1] * 255;
		t.u[2] = col[2] * 255;
		t.u[3] = col[3] * 255;
		return t.uc;
	}
	inline glm::vec4 to_c4(unsigned int c)
	{
		u_col* t = (u_col*)& c;
		glm::vec4 fc;
		float* f = &fc.x;
		for (int i = 0; i < 4; i++)
		{
			*f++ = t->u[i] / 255.0;
		}
		return  fc;
	}
	inline unsigned int multiply_color(unsigned int c, unsigned int c1)
	{
		glm::vec4 v1 = to_c4(c), v2 = to_c4(c1);
		float a1 = v1.w;
		float a2 = v2.w - (a1 * v2.w) / 256;
		float a = a1 + a2;
		glm::vec4 v;
		v.x = (a1 * v1.x + a2 * v2.x);
		v.y = (a1 * v1.y + a2 * v2.y);
		v.z = (a1 * v1.z + a2 * v2.z);
		v.w = (a + 1) * v2.w;
		return to_uint(v);
	}
	inline bool is_range(const glm::ivec2& v, int64_t n, bool is = true)
	{
		return is ? n >= v.x && n <= v.y : n > v.x && n < v.y;
	}
	inline bool is_range(const glm::ivec3& v, int64_t n, bool is = true)
	{
		return is ? n >= v.x && n <= v.y : n > v.x && n < v.y;
	}
	inline bool is_range(const glm::ivec4& v, int64_t n, bool is = true)
	{
		return is ? n >= v.x && n <= v.y : n > v.x && n < v.y;
	}

#endif
#if 1
	/*
	utf8操作
	*/
#ifndef UTF8_FIRST
#define UTF8_ASCII(b) (((unsigned char)(b)>=0x00)&&((unsigned char)(b)<=0x7F))
#define UTF8_FIRST(b) (((unsigned char)(b)>=0xC0)&&((unsigned char)(b)<=0xFD))
#define UTF8_OTHER(b) (((unsigned char)(b)>=0x80)&&((unsigned char)(b)<=0xBF))
#endif // UTF8_FIRST
	//根据首字节,获取utf8字符后续所占字节数
	static int get_utf8_last_num(unsigned char ch)
	{
		static unsigned char t[] = {
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,
			0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
			0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
			0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
			0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
			0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03, 0x04,0x04,0x04,0x04, 0x05,0x05,0,0 };
		return t[ch];
	}
	static int64_t get_utf8_count(const char* buffer, int64_t len)
	{
		const char* p = 0, *pend = buffer + len;
		int64_t count = 0;
		if (!buffer || len <= 0)
		{
			return 0;
		}
		for (p = buffer; p < pend; p++)
		{
			if (UTF8_ASCII(*p) || (UTF8_FIRST(*p)))
			{
				count++;
				p += get_utf8_last_num(*p);
			}
		}
		return count;
	}
	// 获取一个utf8字符串开始位置
	static const char* get_utf8_first(const char* str)
	{
		const char* p = str;
		for (; *p && !(UTF8_ASCII(*p) || (UTF8_FIRST(*p))); p++);
		return p;
	}
	// 获取第n个字符的位置
	static const char* utf8_char_pos(int64_t pos, const char* buffer, int64_t len)
	{
		const char* p = 0, *pend = buffer + len;
		int64_t count = 0;
		if (!buffer || len <= 0)
		{
			return 0;
		}
		for (p = buffer; p < pend; p++)
		{
			if (UTF8_ASCII(*p) || (UTF8_FIRST(*p)))
			{
				count++;
				if (count > pos)
				{
					break;
				}
			}
		}
		return p;
	}

	static std::array<char, 8> utf8(int c) {
		std::array<char, 8> seq;
		int n = 0;
		if (c < 0x80) n = 1;
		else if (c < 0x800) n = 2;
		else if (c < 0x10000) n = 3;
		else if (c < 0x200000) n = 4;
		else if (c < 0x4000000) n = 5;
		else if (c <= 0x7fffffff) n = 6;
		seq[n] = '\0';
		switch (n) {
		case 6: seq[5] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0x4000000;
		case 5: seq[4] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0x200000;
		case 4: seq[3] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0x10000;
		case 3: seq[2] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0x800;
		case 2: seq[1] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0xc0;
		case 1: seq[0] = c;
		}
		return seq;
	}
#endif
#if 1
	inline bool isse(int ch)
	{
		return ch == 0 || (ch > 0 && isspace(ch));
	}
	inline static bool is_json(const char* str, size_t len)
	{
		bool ret = false;
		char* te = (char*)str + len - 1;
		char* t = (char*)str;
		if (str && len > 1)
		{
			do
			{
				while (isse(*t))
				{
					t++;
				}
				if (*t != '[' && *t != '{')
				{
					break;
				}
				while (isse(*te))
				{
					te--;
				}
				if ((*t == '[' && *te == ']') || (*t == '{' && *te == '}'))
				{
					ret = true;
				}
			} while (0);
		}
		return ret;
	}
	static uint64_t toUInt(const njson& v, uint64_t de = 0)
	{
		uint64_t ret = de;
		if (v.is_number())
		{
			ret = v.get<uint64_t>();
		}
		else if (!v.is_null())
		{
			ret = std::atoll(hstring::trim(v.dump(), "\"").c_str());
		}
		return ret;
	}
	static int64_t toInt(const njson& v, int64_t de = 0)
	{
		int64_t ret = de;
		if (v.is_number())
		{
			ret = v.get<int64_t>();
		}
		else if (!v.is_null())
		{
			ret = std::atoll(hstring::trim(v.dump(), "\"").c_str());
		}
		return ret;
	}
	static double toDouble(const njson& v, double de = 0)
	{
		double ret = de;
		if (v.is_number())
		{
			ret = v.get<double>();
		}
		else if (!v.is_null())
		{
			ret = std::atof(hstring::trim(v.dump(), "\"").c_str());
		}
		return ret;
	}
#define toFloat toDouble

	static bool toBool(const njson& v, bool def = false)
	{
		bool ret = def;
		if (v.is_boolean())
		{
			ret = v.get<bool>();
		}
		else
		{
			ret = hstring::trim(v.dump(), "\"") != "null";
		}
		return ret;
	}
	static std::string toStr(const njson& v, const std::string& des = "")
	{
		std::string ret = des;
		if (v.is_null())
		{
			//ret = "";
		}
		else if (v.is_string())
		{
			ret = v.get<std::string>();
		}
		else
		{
			ret = hstring::trim(v.dump(), "\"");
		}
		return ret;
	}
	static std::string doubleToString(double price, int n = 0)
	{
		auto res = n > 0 ? std::_Floating_to_string(("%." + std::to_string(n) + "f").c_str(), price) : std::to_string(price);
		const std::string format("$1");
		try {
			std::regex r("(\\d*)\\.0{6}|");
			std::regex r2("(\\d*\\.{1}0*[^0]+)0*");
			res = std::regex_replace(res, r2, format);
			res = std::regex_replace(res, r, format);
		}
		catch (const std::exception& e) {
			return res;
		}
		return res;
	}
	static std::string toStr(double price, int n = 0)
	{
		return n > 0 ? std::_Floating_to_string(("%." + std::to_string(n) + "f").c_str(), price) : std::to_string(price);
	}
	static uint64_t toHex(const njson& v, uint64_t d = 0)
	{
		uint64_t ret = d;
		if (v.is_string())
		{
			std::string buf = toStr(v);
			char* str = 0;
			//buf.resize(10);
			ret = std::strtoll(buf.c_str(), &str, buf.find("0x") == 0 ? 16 : 10);
		}
		else if (v.is_number())
		{
			ret = toUInt(v);
		}
		return ret;
	}
	static unsigned int toColor(const njson& v, unsigned int d = 0)
	{
		return toHex(v, d);
	}

	static std::string ptHex(const std::string& str, char ch = 'x', char step = '\0')
	{
		char spch[8] = { '%','0','2',ch, step, 0 };
		std::string chBuffer;
		char chEach[10];
		int nCount;
		memset(chEach, 0, 10);
		unsigned char* d = (unsigned char*)str.c_str();
		auto len = str.size();
		for (nCount = 0; nCount < len && d[nCount]>0; nCount++)
		{
			sprintf(chEach, spch, d[nCount]);
			chBuffer += chEach;
		}
		return chBuffer;
	}

#if (defined(GLM_VERSION))
	static glm::vec2 toVec2(const njson& v, double d = -1)
	{
		std::vector<double> rv = { d, d };
		if (v.is_array() && v.size() > 2)
		{
			std::vector<double> trv = v;
			rv.swap(trv);
		}
		return { rv[0],rv[1] };
	}
	static glm::vec3 toVec3(const njson& v, double d = -1)
	{
		std::vector<double> rv = { d, d ,d };
		if (v.is_array() && v.size() > 3)
		{
			std::vector<double> trv = v;
			rv.swap(trv);
		}
		return { rv[0],rv[1] ,rv[2] };
	}
	static glm::vec4 toVec4(const njson& v, double d = -1)
	{
		std::vector<double> rv = { d, d,d,d };
		if (v.is_array() && v.size() > 4)
		{
			std::vector<double> trv = v;
			rv.swap(trv);
		}
		return { rv[0],rv[1],rv[2],rv[3] };
	}
	static njson v2to(const glm::vec2& v)
	{
		std::vector<double> rv = { v.x,v.y };
		return rv;
	}
	static njson v3to(const glm::vec3& v)
	{
		std::vector<double> rv = { v.x,v.y ,v.z };
		return rv;
	}
	static njson v4to(const glm::vec4& v)
	{
		std::vector<double> rv = { v.x,v.y ,v.z,v.w };
		return rv;
	}
#endif
	template <typename T>
	static T* toPtr(const njson& v)
	{
#if 0
		std::string buf = toStr(v);
		unsigned long long ret = 0;
		char* str = 0;
		buf.resize(18);
		ret = std::strtoll(buf.c_str(), &str, buf.find("0x") == 0 ? 16 : 10);
		return (T*)ret;
#else
		return (T*)toHex(v);
#endif
	}

	template <typename T>
	static std::string fromPtr(T* pv)
	{
		std::string buf = "0x";
		buf.resize(sizeof(T*) * 4);
		_ui64toa_s((uint64_t)pv, (char*)(buf.data() + 2), buf.size() - 2, 16);
		return buf.c_str();
	}

#endif

	//------------------------------------------------------------------------------------------------------------------
	class jsonT
	{
	public:
#ifndef _GLIBCXX_USE_C99_STDLIB
		static unsigned long Stoul(const std::string& _Str, size_t* _Idx = 0, int _Base = 10)
		{	// convert string to unsigned long
			const char* _Ptr = _Str.c_str();
			char* _Eptr;
			errno = 0;
			unsigned long _Ans = strtoul(_Ptr, &_Eptr, _Base);

			//if (_Ptr == _Eptr)
			//	_Xinvalid_argument("invalid stoul argument");
			//if (errno == ERANGE)
			//	_Xout_of_range("stoul argument out of range");
			if (_Idx != 0)
				* _Idx = (size_t)(_Eptr - _Ptr);
			return (_Ans);
		}
#else
#define Stoul std::stoul
#endif
	public:
		jsonT()
		{

		}

		~jsonT()
		{
		}
		static std::string format(const char* strc, size_t len = 0, bool isN = false)
		{
			std::set<char> jch = { '{', '}', '[', ']', ',', ':', '\"','\'','\n' ,'=',';',' ' };
			std::string o;
			unsigned char* str = (unsigned char*)strc;
			unsigned char* strb = str;
			size_t slen = len > 0 ? len : strlen(strc);
			if (!str)
			{
				return o;
			}
			char lastch = 0;
			size_t lastpos = 0;
			for (; *str && (str - strb) <= slen; str++)
			{
				//判断是否为空字符
				if (isspace((unsigned char)* str))
				{
					o.push_back((*str != '\t') ? *str : ' ');
					continue;
				}
				//判断是否为json边界符号
				if (jch.find(*str) != jch.end())
				{
					char s = *str;
					unsigned char ch = *str;
					if (*str == '\"' || (*str == '\''))
					{
						do
						{
							if (ch == '\'')
							{
								ch = '\"';
							}
							o.push_back(ch);
							str++;
							ch = *str;
						} while (str[0] != s);
					}
					if (*str == '=')
					{
						ch = ':';
					}
					if (*str == ';')
					{
						ch = ',';
					}
					if (o.size())
					{
						char ro = lastch;
						if (ch == '}' || ch == ']')
						{
							if (ro == ',')
								o[lastpos] = ' ';
						}
						//if (ch == '{' || ch == '[')
						//{
						//	if (ro != ':' && !(ro == '{' || ro == '['))
						//		o.push_back(':');
						//}
					}
					lastpos = o.size();
					if (ch == '\'')
					{
						ch = '\"';
					}
					o.push_back(ch);
					lastch = ch;
					continue;
				}
				//判断注释
				if (str[0] == '/' && str[1] == '/')
				{
					for (; *str && (*str != '\n'); str++);
					o.push_back(*str);
					continue;
				}
				//判断是否是数字
				if (*str == '-' || *str == '.' || (*str >= '0' && *str <= '9'))
				{
					unsigned char* tem = str;
					bool isnum = false;
					for (; *tem; tem++)
					{
						if (!((!isN && (*tem == '-' || *tem == '+')) || *tem == '.' || *tem == 'e' || *tem == 'E' || (*tem >= '0' && *tem <= '9')))
						{
							if ((jch.find(*tem) != jch.end()))
							{
								isnum = true;
							}
							break;
						}
					}
					if (isnum) {
						size_t len = tem - str;
						std::string tems((char*)str, len);
						str += len - 1;
						o += tems;
						lastch = *o.rbegin(); continue;
					}
				}
				//判断true\false\null
				std::set<std::string> tfn = { "true","false","null","TRUE" ,"FALSE" ,"NULL" };
				bool istfn = false;
				for (auto it : tfn)
				{
					if (memcmp(str, it.c_str(), it.length()) == 0)
					{
						o += it;
						str += it.length() - 1;
						istfn = true;
						lastch = *o.rbegin();
						break;
					}
				}
				if (istfn)
					continue;

				//字符串
				o.push_back('\"');
				unsigned char* tem = str;
				for (; *tem && (jch.find(*tem) == jch.end() && *tem != ' '); tem++);
				size_t len = tem - str;
				std::string tems((char*)str, len);
				str += len - 1;
				o += tems;
				lastch = '\"';
				o.push_back(lastch);
			}
			return o;
		}

		inline static bool is_json(const char* str, size_t len)
		{
			bool ret = false;
			if (str && len > 1)
			{
				while (len > 0 && !str[len - 1])
				{
					len--;
				}
				if (str[0] == '[' && str[len - 1] == ']')
					ret = true;
				if (str[0] == '{' && str[len - 1] == '}')
					ret = true;
			}
			return ret;
		}
		inline static bool is_json(const njson& s)
		{
			auto str = toStr(s);
			return is_json(str.c_str(), str.size());
		}
		static uint64_t getUInt(const njson& v, uint64_t de = 0)
		{
			uint64_t ret = de;
			if (v.is_number())
			{
				ret = v.get<uint64_t>();
			}
			else if (!v.is_null())
			{
				ret = std::atoll(hstring::trim(v.dump(), "\"").c_str());
			}
			return ret;
		}
		static int64_t getInt(const njson& v, int64_t de = 0)
		{
			int64_t ret = de;
			if (v.is_number())
			{
				ret = v.get<int64_t>();
			}
			else if (!v.is_null())
			{
				ret = std::atoll(hstring::trim(v.dump(), "\"").c_str());
			}
			return ret;
		}
		static double getDouble(const njson& v, double de = 0)
		{
			double ret = de;
			if (v.is_number())
			{
				ret = v.get<double>();
			}
			else if (!v.is_null())
			{
				ret = std::atof(hstring::trim(v.dump(), "\"").c_str());
			}
			return ret;
		}
#define getFloat getDouble
		static std::string getStr2(const njson& v)
		{
			std::string ret;
			if (v.is_null())
			{
				ret = "";
			}
			else if (v.is_string())
			{
				ret = v.get<std::string>();
			}
			else
			{
				ret = hstring::trim(v.dump(), "\"");
			}
			return ret;
		}
		static std::string getStr(const njson& v, bool isutf8 = true)
		{
			std::string ret;
			if (v.is_null())
			{
				ret = "";
			}
			else if (v.is_string())
			{
				ret = v.get<std::string>();
			}
			else
			{
				ret = hstring::trim(v.dump(), "\"");
			}
			if (!isutf8)
				return AtoA(ret);
			return ret;// AtoUTF8(ret);
		}
		static unsigned int getColor(const njson& v, unsigned int d = 0)
		{
			std::string buf = getStr(v);
			unsigned long long ret = d;
			char* str = 0;
			buf.resize(10);
			ret = std::strtoll(buf.c_str(), &str, buf.find("0x") == 0 ? 16 : 10);
			return ret;
		}
#if (defined(GLM_VERSION))
		static glm::vec2 getVec2(const njson& v)
		{
			std::vector<double> rv = v;
			return { rv[0],rv[1] };
		}
		static glm::vec3 getVec3(const njson& v)
		{
			std::vector<double> rv = v;
			return { rv[0],rv[1] ,rv[2] };
		}
		static glm::vec4 getVec4(const njson& v)
		{
			std::vector<double> rv = v;
			return { rv[0],rv[1],rv[2],rv[3] };
		}
#endif
		template <typename T>
		static T* getPtr(const njson& v)
		{
			std::string buf = getStr(v);
			unsigned long long ret = 0;
			char* str = 0;
			buf.resize(18);
			ret = std::strtoll(buf.c_str(), &str, buf.find("0x") == 0 ? 16 : 10);
			return (T*)ret;
		}

#if 1

		/*
		 * 把数组转换成多维对象表
		 * arr 数组对象
		 * k string或数组
		 *
		 */
		static njson array2map(const njson & arr, njson k)
		{
			if (k.is_string())
			{
				std::string ks = jsonT::getStr(k);
				k = njson::array();
				k.push_back(ks);
			}
			njson ret;
			for (auto it : arr)
			{
				std::string key;
				njson* dn = &ret;
				for (auto kt : k)
				{
					dn = &(*dn)[jsonT::getStr(it[jsonT::getStr(kt)])];
				}
				dn->push_back(it);
			}
			return ret;
		}
		/*
		 * 枚举对象或数组
		 */
		static void map_oa(const njson& data, std::function<void(njson k, njson v)> func)
		{
			if (data.is_array() || data.is_string())
			{
				for (auto it : data)
				{
					func({}, it);
				}
			}
			else if (data.is_object())
			{
				for (auto it = data.begin(); it != data.end(); ++it)
				{
					func(it.key(), it.value());
				}
			}
		}

		static njson map(njson& arr, std::function<njson(njson&)> func)
		{
			// todo 实现map
			njson ret = njson::array();
			if (!func)
			{
				return arr;
			}
			for (auto it = arr.begin(); it != arr.end(); ++it)
			{
				ret.push_back(func(*it));
			}
			return ret;
		}
		static std::string get_keys(const njson& data, std::string s = "`")
		{
			std::string lstr;
			jsonT::map_oa(data, [=, &lstr](njson k, njson v) {
				if (lstr.size())
				{
					lstr += ",";
				}
				lstr += s + jsonT::getStr(k) + s;
				});
			return lstr;
		}
		static std::string get_values(const njson& data, std::string s = "'", std::vector<std::string> rang = { "(",")" })
		{
			std::string values;
			for (auto& it : data)
			{
				std::string its = jsonT::join(it, ",", nullptr, true);
				if (values.size())
				{
					values += ",";
				}
				values += rang[0] + hstring::replace(its, "\"", s) + rang[1];
			}
			return values;
		}
		inline static std::string get_value(const njson& data, std::string s = "'", const std::string& ts = "null")
		{
			if (data.is_null())
			{
				return ts;
			}
			std::string its = data.dump();
			std::string values;
			values += hstring::replace(its, "\"", s);
			values.erase(
				remove_if(values.begin(), values.end(), [=](int c) {return (c < 0 || c == s[0] || c == '_' || c == '-' || c == ',' || c == '.' || c == ':' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']') ? 0 : ispunct((char)c); }),// static_cast<int(*)(int)>(&ispunct)),
				values.end());
			if (values.size() != 21)
			{
				values.erase(
					remove_if(values.begin(), values.end(), [](int c) {return c > 0 ? isspace((char)c) : 0; }),
					values.end());
			}
			if (values == "''")
			{
				values = ts;
			}
			return values;
		}
		inline static std::string get_between(const std::string& key, njson first, njson second, const std::string& ts)
		{
			std::string ret = "(" + key + " BETWEEN ";
			auto vf = get_value(first);
			auto vs = get_value(second);
			if (vf == "" || vf == "''")
			{
				return "";
			}
			if (vs == "" || vs == "''")
			{
				vs = ts;
			}
			ret += vf + " and " + vs + ") ";
			return ret;
		}
		static njson to_json_arrsy(const njson& v)
		{
			njson vals;
			if (!v.is_array())
			{
				vals.push_back(v);
			}
			else
			{
				vals = v;
			}
			return vals;
		}

		static void print_json(const njson& n, int idxs)
		{
			std::string str;
			if (idxs > 0)
			{
				str = n.dump(idxs);
			}
			else
			{
				str = n.dump();
			}
			str = AtoA(str);
			printf("%s\n", str.c_str());
		}
#endif

		static bool isNum(const char* str, int64_t len = MAXINT64)
		{
			//判断是否是数字
			bool isnum = false;
			if (*str == '-' || *str == '+' || *str == '.' || (*str >= '0' && *str <= '9'))
			{
				unsigned char* tem = (unsigned char*)str;
				isnum = true;
				int d = 0;
				for (; *tem && len--; tem++)
				{
					if (*tem == '.')
					{
						d++;
					}
					if (!((*tem == '.') || (*tem >= '0' && *tem <= '9') || *tem == 'e' || *tem == 'E' || (*tem == 'f')) || d > 1)
					{
						isnum = false;
						break;
					}
				}
			}
			return isnum;
		}
		// 字符串转json value
		static njson makeValue(const std::string& str)
		{
			double d;
			int64_t i64;
			i64 = std::atoll(str.c_str());
			d = std::atof(str.c_str());
			auto is = std::to_string(i64);
			bool k = isNum(str.c_str());
			size_t ic = 0;
			auto pos = str.find('.');
			if (k && pos != -1)
			{
				return d;
				//ic = str.size() - pos - 1;
				//std::string fmt = "%.";
				//fmt += std::to_string(ic) + "lf";
				//if (std::_Floating_to_string(fmt.c_str(), d) == str)
				//{
				//	return d;
				//}
			}
			if (k || is == str)
			{
				return i64;
			}
			return str;
		}

		static bool getBool(const njson& v, bool def = false)
		{
			bool ret = def;
			if (v.is_boolean())
			{
				ret = v.get<bool>();
			}
			else
			{
				ret = hstring::trim(v.dump(), "\"") != "null";
			}
			return ret;
		}
		static std::string join(const njson& a, const std::string& subs, std::function<std::string(njson it, size_t idx)> func = nullptr, bool is_dump = false)
		{
			std::string ret;
			size_t i = 0;
			if (!func)
			{
				func = [=](njson it, size_t) {return (is_dump ? it.dump() : getStr(it)); };
			}
			for (auto it : a)
			{
				ret += (i > 0 ? subs : "") + func(it, i); i++;
			}
			return ret;
		}
		static njson split(std::string str, const std::string& pattern)
		{
			njson ret;
			std::string::size_type pos;
			str += pattern;//扩展字符串以方便操作
			int size = str.size();
			int ct = 0;
			for (int i = 0; i < size; i++)
			{
				pos = str.find(pattern, i);
				if (pos < size)
				{
					std::string s = str.substr(i, pos - i);
					ret.push_back(s);
					i = pos + pattern.size() - 1;
					ct++;
				}
			}
			return ret;
		}
		static njson set_union(const njson& first, const njson& second)//并集) intersection(交集)difference差集
		{
			njson ret, tf = first;
			ret = tf;
			if (first.is_null())
			{
				return second;
			}
			else if (second.is_null())
			{
				return first;
			}
			if (tf.is_array() && second.is_array())
			{
				ret = njson::array();
				njson a = njson::array(), ts = second;
				njson::array_t* f = tf.get<njson::array_t*>();
				njson::array_t* s = ts.get<njson::array_t*>();
				njson::array_t* aa = a.get<njson::array_t*>();
				aa->resize(tf.size() + second.size());
				std::sort(f->begin(), f->end());
				std::sort(s->begin(), s->end());
				std::set_union(f->begin(), f->end(), s->begin(), s->end(), aa->begin());
				for (auto it = aa->rbegin(); it != aa->rend(); it = aa->rbegin())
				{
					if (it->is_null())
					{
						aa->pop_back();
					}
					else
					{
						break;
					}
				}
				ret = a;
			}
			else if (first.is_object() && second.is_object())
			{
				for (auto& [k, v] : second.items())
				{
					ret[k] = v;
				}
			}
			return ret;
		}



		// 获取范围内的日期

		template <typename T>
		static njson getDateRang(T& d, bool isStr = true)
		{
			if (d[0] == "" || d[1] == "")
			{
				return njson();
			}
			Date start(d[0].substr(0, 10));
			Date ends(d[1].substr(0, 10));
			njson ret;
			ret.push_back(start.c_str());
			for (; ;) {
				start.setDay(start.getDate() + 1);
				if (Date::dateDiff(start, ends, 'd') >= 0) {
					ret.push_back(start.c_str());
				}
				else {
					break;
				}
			}
			//ret = ret.map((it: any) = > (moment(it).format('YYYY-MM-DD')));
			if (isStr)
			{
				ret = join(ret, ",");
			}
			return ret;
		}
#if 0
		// 获取指定年假期（2018，['1/1,2/15-21,4/5-7,4/29-30,5/1,6/16-18,9/22-24,10/1-7']）
		getLeaveDays: (year: string, leave : any[]) = > {
			const curretYear = year.split('-')[0];
			let ret : any = {};
			const days : any = {};
			for (const kt of leave) {
				kt.split(',').map((it: any) = > {
					const m = it.split('/').map((mt: any) = > (mt.length < 2 ? `0${mt
				}` : mt));
					const d = m[1].split('-').map((mt: any) = > (mt.length < 2 ? `0${mt
			}` : mt));
					let rd = [`${curretYear
		}-${ m[0] }-${ d[0] }`];
					if (d.length > 1) {
						rd = utils.getDateRang([`${curretYear
					}-${ m[0] }-${ d[0] }`, `${curretYear
	}-${ m[0] }-${ d[1] }`], false);
}
rd.map((rt: any) = > { ret[rt] = 1; });
	});
	}
	let all = 0;
	for (const ot of Object.keys(ret)) {
		let ms = ot.split('-');
		if (ms.length < 2)
			continue;
		const m = ms[1];
		// 统计每个月多少天法定假期
		days[m] = (days[m] || 0) + ret[ot];
		all++;
	}
	// 本年总数
	ret.count = all;
	// 每月天数
	ret.days = days;
	return ret;
		}
#endif
		// ---------------------------------------------------------------------------------------------------------------------------------------------------------
		// convert UTF-8 string to wstring  
		static std::wstring utf8_to_wstring(const std::string& str)
		{
			std::wstring ret;
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			try
			{
				ret = myconv.from_bytes(str);
			}
			catch (const std::exception& e)
			{
				ret = ansi_to_wstring(str);
			}
			return ret;
		}
		static std::string utf8_to_ansi(const std::string& str)
		{
			return wstring_to_ansi(utf8_to_wstring(str));
		}
		// convert wstring to UTF-8 string  
		static std::string wstring_to_utf8(const std::wstring& str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.to_bytes(str);
		}

		static std::string wstring_to_ansi0(const std::wstring& src)
		{
			std::string str;
			setlocale(LC_ALL, "");     //设置本地默认Locale
			int64_t s = 100, inc = 100;
			auto ws = src;
			for (; s > 0; inc += 100)
			{
				ws = src.substr(inc, 100);
				s = wcstombs(nullptr, ws.c_str(), 0);
			}
			if (s > 0)
			{
				std::vector<char> tem;
				tem.resize(s);
				int64_t nRet = wcstombs(tem.data(), src.c_str(), src.size());
				if (nRet <= 0)
				{
					printf("转换失败\n");
				}
				else
				{
					tem.push_back(0);
					printf("转换成功%lld字符\n", nRet);
					printf("%s\n", tem.data());
					str = tem.data();
				}
			}
			return str;
		}

		static std::wstring ansi_to_wstring0(const std::string& src)
		{
			std::wstring wstr;
			auto s = mbstowcs(nullptr, src.c_str(), src.size());
			if (s > 0)
			{
				std::vector<wchar_t> tem;
				tem.resize(s);
				int64_t nRet = mbstowcs(tem.data(), src.c_str(), src.size());
				if (nRet <= 0)
				{
					printf("转换失败\n");
				}
				else
				{
					tem.push_back(0);
					printf("转换成功%lld字符\n", nRet);
					wprintf(L"%ls\n", tem.data());
					wstr = tem.data();
				}
			}
			return wstr;
		}
		// todo 部分转换失败
		static std::string wstring_to_ansi(const std::wstring& src)
		{
			std::string str;
			static std::locale sys_locale("");

			const wchar_t* data_from = src.c_str();
			const wchar_t* data_from_end = src.c_str() + src.size();
			const wchar_t* data_from_next = 0;

			int wchar_size = 4;
			std::vector<char> tem;
			tem.resize((src.size() + 1) * wchar_size);
			char* data_to = (char*)tem.data();
			char* data_to_end = data_to + tem.size();// (src.size() + 1) * wchar_size;
			char* data_to_next = 0;

			memset(data_to, 0, tem.size());

			typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
			mbstate_t out_state = { 0 };
			auto result = std::use_facet<convert_facet>(sys_locale).out(
				out_state, data_from, data_from_end, data_from_next,
				data_to, data_to_end, data_to_next);
			str = data_to;
			if (result == convert_facet::ok)
			{
				return str;
			}
			return str;
		}

		static std::wstring ansi_to_wstring(const std::string& src)
		{
			std::wstring wstr;
			static std::locale sys_locale("");
			const char* data_from = src.c_str();
			const char* data_from_end = src.c_str() + src.size();
			const char* data_from_next = 0;

			std::vector<wchar_t> tem;
			tem.resize(src.size() + 1);
			wchar_t* data_to = (wchar_t*)tem.data();
			wchar_t* data_to_end = data_to + src.size() + 1;
			wchar_t* data_to_next = 0;

			wmemset(data_to, 0, src.size() + 1);

			typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
			mbstate_t in_state = { 0 };
			auto result = std::use_facet<convert_facet>(sys_locale).in(
				in_state, data_from, data_from_end, data_from_next,
				data_to, data_to_end, data_to_next);

			wstr = data_to;
			if (result == convert_facet::ok)
			{
				return wstr;
			}
			return wstr;
		}
		static std::string ansi_to_utf8(const std::string& src)
		{
			return wstring_to_utf8(ansi_to_wstring(src));
		}

		static std::wstring AtoW(const std::string& str)
		{
			if (hz::hstring::IsTextUTF8(str.c_str()))
			{
				std::string a = str.substr(0, 5);
				int ic = deleteBom(a, false);
				a = str.substr(ic);
				return hz::jsonT::utf8_to_wstring(a);
			}
			else
				return hz::jsonT::ansi_to_wstring(str);
		}

		static std::string AtoA(const std::string& str)
		{
			if (hz::hstring::IsTextUTF8(str.c_str()))
				return hz::jsonT::utf8_to_ansi(str);
			else
				return str;
		}
		static std::string AtoUTF8(const std::string& str)
		{
			if (hz::hstring::IsTextUTF8(str.c_str()))
				return str;
			else
				return hz::jsonT::ansi_to_utf8(str);
		}

		static char exchange(char c)
		{
			if (c <= 'Z' && c >= 'A')
				c = tolower(c);
			else if (c >= 'a' && c <= 'z')
				c = toupper(c);
			return c;
		}
		static void Aexchange(std::string& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), exchange);  //大小写切换
		}
		static void Atolower(std::string& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::tolower); //将大写的都转换成小写
		}
		static void Atoupper(std::string& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::toupper); //将小写的都转换成大写
		}

		static std::string Aexchange(const std::string& cstr)
		{
			std::string str = cstr;
			std::transform(str.begin(), str.end(), str.begin(), exchange);  //大小写切换
			return str;
		}
		static std::string Atolower(const std::string& cstr)
		{
			std::string str = cstr;
			std::transform(str.begin(), str.end(), str.begin(), ::tolower); //将大写的都转换成小写
			return str;
		}
		static std::string Atoupper(const std::string& cstr)
		{
			std::string str = cstr;
			std::transform(str.begin(), str.end(), str.begin(), ::toupper); //将小写的都转换成大写
			return str;
		}

		static int getUtf8Count(const char* s)
		{
			int i = 0, j = 0;
			while (s[i])
			{
				if ((s[i] & 0xc0) != 0x80) j++;
				i++;
			}
			return j;
		}

		static int getUtf8Word(const char* s, int wantedNum, int& offset)
		{
			int i = 0, j = 0;

			int readedNum = 0;
			int isReach = 0;
			while (s[i])
			{
				if ((s[i] & 0xc0) != 0x80)
				{
					if (isReach)
					{
						break;
					}
					++j;
					readedNum = j;
					if (j == wantedNum)
					{
						isReach = 1;
					}

				}
				++i;
			}
			offset = i;
			return readedNum;
		}
#ifdef _WIN32
		static std::wstring AnsiToUnicode(const char* buf)
		{
			int len = ::MultiByteToWideChar(CP_ACP, 0, buf, -1, NULL, 0);
			if (len == 0) return L"";

			std::vector<wchar_t> unicode(len);
			::MultiByteToWideChar(CP_ACP, 0, buf, -1, &unicode[0], len);

			return &unicode[0];
		}

		static std::string UnicodeToAnsi(const wchar_t* buf)
		{
			int len = ::WideCharToMultiByte(CP_ACP, 0, buf, -1, NULL, 0, NULL, NULL);
			if (len == 0) return "";

			std::vector<char> utf8(len);
			::WideCharToMultiByte(CP_ACP, 0, buf, -1, &utf8[0], len, NULL, NULL);

			return &utf8[0];
		}

		static std::wstring Utf8ToUnicode(const char* buf)
		{
			int len = ::MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
			if (len == 0) return L"";

			std::vector<wchar_t> unicode(len);
			::MultiByteToWideChar(CP_UTF8, 0, buf, -1, &unicode[0], len);

			return &unicode[0];
		}

		static std::string UnicodeToUtf8(const wchar_t* buf)
		{
			int len = ::WideCharToMultiByte(CP_UTF8, 0, buf, -1, NULL, 0, NULL, NULL);
			if (len == 0) return "";

			std::vector<char> utf8(len);
			::WideCharToMultiByte(CP_UTF8, 0, buf, -1, &utf8[0], len, NULL, NULL);

			return &utf8[0];
		}
		static std::string AnsiToUtf8(const char* buf)
		{
			return UnicodeToUtf8(AnsiToUnicode(buf).c_str());
		}
		static std::string Utf8ToAnsi(std::string ustr)
		{
			deleteBom(ustr);
			return UnicodeToAnsi(Utf8ToUnicode(ustr.c_str()).c_str());
		}
#endif
		static int deleteBom(std::string& str, bool isDel = true)
		{
			//UTF-8
			char utf8[] = { (char)0xEF ,(char)0xBB ,(char)0xBF };
			//UTF-16（大端序）
			char utf16d[] = { (char)0xFE ,(char)0xFF };
			//UTF-16（小端序）
			char utf16[] = { (char)0xFF ,(char)0xFE };
			//UTF-32（大端序）
			char utf32d[] = { (char)0x00 ,(char)0x00 ,(char)0xFE ,(char)0xFF };
			//UTF-32（小端序）
			char utf32[] = { (char)0xFF ,(char)0xFE ,(char)0x00 ,(char)0x00 };

			int bomlen[] = { 3,2,2,4,4 };
			char* bom[] = { utf8,utf16d,utf16,utf32d,utf32 };
			int u = -1;
			for (int i = 0; i < 5; ++i)
			{
				if (0 == memcmp(str.c_str(), bom[i], bomlen[i]))
				{
					u = i;
				}
			}
			int ret = 0;
			if (u >= 0)
			{
				if (isDel)
					str.replace(0, bomlen[u], "");
				ret = bomlen[u];
			}
			return ret;
		}
		//-----------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------


		//----数据操作-----------------------------------------------------------------------------------------
		/*
				std::string str = jsonT::formatbin("%d%d%f%lf", width, height, 0.2f, 0.3);
				int n[2] = { 0 };
				float f = 0.0f;
				double lf = 0.0;
				jsonT::sscanf(str.c_str(), "%d%d%f%lf", n, &n[1], &f, &lf);
		*/
		template <typename T>
		static inline void copytostr(std::string& s, T n)
		{
			char buf[256];
			memcpy(buf, &n, sizeof(T));
			s.append((char*)buf, sizeof(T));
		}
		template <typename T>
		static inline char* copytoptr(char* d, T* n, size_t len = 1)
		{
			if (len < 1)
				len = strlen(d);
			memcpy(n, d, sizeof(T) * len);
			return d + sizeof(T) * len;
		}
		template <typename t>
		static t vaarg(char* ap)
		{
			if (sizeof(t) > sizeof(int64_t) || (sizeof(t) & (sizeof(t) - 1)) != 0)
			{
				return **(t * *)((ap += sizeof(int64_t)) - sizeof(int64_t));
			}
			else {
				return *(t*)((ap += sizeof(int64_t)) - sizeof(int64_t));
			}
		}
		static std::string formatbin(char* format, ...)
		{
			std::string rs;
			va_list arg;
			va_start(arg, format);

			while (*format)
			{
				char ret = *format;
				if (ret == '%')
				{
					char q = *++format;
					if (q == 'l')
						format++;
					char d = *format;
					switch (d)
					{
					case 'c':
					{
						char ch = va_arg(arg, char);
						rs.push_back(ch);
						break;
					}
					case 's':
					{
						char* pc = va_arg(arg, char*);
						int64_t len = strlen(pc);
						copytostr(rs, len);
						rs.append(pc);
						break;
					}
					case 'u':
					case 'd':
					case 'i':
					{
						if (q == 'l')
						{
							int64_t n = va_arg(arg, int64_t);
							copytostr(rs, n);
						}
						else {
							int n = va_arg(arg, int);
							copytostr(rs, n);
						}
						break;
					}
					case 'p':
					{
						size_t* n = va_arg(arg, size_t*);
						copytostr(rs, n);
						break;
					}
					case 'f':
					{
						if (q == 'l')
						{
							double n = va_arg(arg, double);
							copytostr(rs, n);
						}
						else {
							float n = (float)va_arg(arg, double);
							copytostr(rs, n);
						}
						break;
					}
					default:
						break;
					}
				}
				else
				{
					//putchar(*format);
				}
				format++;
			}
			va_end(arg);
			return rs;
		}
		static size_t sscanf(const char* data, char* fmt, ...)
		{
			va_list arg;
			char* dp = (char*)data;
			va_start(arg, fmt);

			while (*fmt)
			{
				char ret = *fmt;
				if (ret == '%')
				{
					char q = *++fmt;
					if (q == 'l')
						fmt++;
					char d = *fmt;
					switch (d)
					{
					case 'c':
					{
						char* ch = va_arg(arg, char*);
						dp = copytoptr(dp, ch, 1);

						break;
					}
					case 's':
					{
						char* ch = va_arg(arg, char*);
						int64_t len = *(int64_t*)dp;
						dp += sizeof(int64_t);
						dp = copytoptr(dp, ch, (size_t)len);

						break;
					}
					case 'u':
					case 'd':
					case 'i':
					{
						if (q == 'l')
						{
							int64_t* n = va_arg(arg, int64_t*);
							dp = copytoptr(dp, n);

						}
						else {
							int* n = va_arg(arg, int*);
							dp = copytoptr(dp, n);
						}
						break;
					}
					case 'p':
					{
						size_t* n = va_arg(arg, size_t*);
						dp = copytoptr(dp, n);

						break;
					}
					case 'f':
					{
						if (q == 'l')
						{
							double* n = va_arg(arg, double*);
							dp = copytoptr(dp, n);
						}
						else {
							float* n = va_arg(arg, float*);
							dp = copytoptr(dp, n);
						}
						break;
					}
					default:
						break;
					}
				}
				else
				{
					//putchar(*format);
				}
				fmt++;
			}
			va_end(arg);
			return (size_t)dp;
		}
		/**/
	private:
	};
	class Wheres
	{
	private:
		std::string _data, _tb;
		njson _where;
	public:
		Wheres()
		{
		}
		~Wheres()
		{
		}
		static std::string mjkey(std::string row, std::string k)
		{
			return "SUBSTRING(" + row + "-> '$." + k + "',2, CHAR_LENGTH(" + row + "->'$." + k + "')-2)";
		}
		void clear()
		{
			_data = "";
			_where = njson::array();
		}
		bool empty()
		{
			return _where.empty();
		}
		void set_tb(const std::string& tb)
		{
			_tb = tb;
		}
	public:
		void push(std::string n, bool is_and = true)
		{
			_where.push_back({ n, is_and ? "AND" : "OR" });
		}
		void push_json(std::string r, njson d, bool is_and = true)
		{
			if (d.is_null())
			{
				return;
			}
			jsonT::map_oa(d, [&](njson k, njson v)
				{
					auto vstr = jsonT::get_value(v);
					if (vstr != "null")
					{
						//std::string qs = r + "->'$." + jsonT::getStr(k) + "' =" + vstr;
						std::string qs = mjkey(r, jsonT::getStr(k)) + " =" + vstr;
						push(qs, is_and);
					}
				});
		}
		// r->k的值不能是数组、对象。
		// k in [a]
		void push_json_in(std::string r, njson k, njson a, bool is_and = true)
		{
			if (!k.is_array())
			{
				k = njson::array({ k });
			}
			if (a.is_null())
			{
				return;
			}
			if (!a.is_array())
			{
				a = njson::array({ a });
			}
			auto kin = jsonT::join(k, " or ", [=](njson kt, size_t) {
				auto ins = jsonT::join(a, ",", [=](njson it, size_t) {
					return jsonT::get_value(it);
					});
				return ("JSON_CONTAINS(JSON_ARRAY(" + ins + "), JSON_EXTRACT(`" + r + "`, '$." + toStr(kt) + "'))");
				});
			push("(" + kin + ")");
		}
		// r->k的值是数组。
		// a in [k]
		void push_json_ink(std::string r, njson k, njson a, bool is_and = true)
		{
			if (!k.is_array())
			{
				k = njson::array({ k });
			}
			if (a.is_null())
			{
				return;
			}
			if (!a.is_array())
			{
				a = njson::array({ a });
			}
			auto kin = jsonT::join(k, " or ", [=](njson kt, size_t) {
				auto kstr = "`" + r + "` -> '$." + toStr(kt) + "'";
				auto ins = jsonT::join(a, " or ", [=](njson it, size_t) {
					return "JSON_CONTAINS(" + kstr + ",JSON_ARRAY(" + jsonT::get_value(it) + "))";
					});
				return "(" + ins + ")";
				});
			push("(" + kin + ")");
		}
		void push_json_like(std::string r, njson k, njson a, bool is_and = true)
		{
			if (!k.is_array())
			{
				k = njson::array({ k });
			}
			if (a.is_null())
			{
				return;
			}
			if (!a.is_array())
			{
				a = njson::array({ a });
			}
			auto kin = jsonT::join(k, " or ", [=](njson kt, size_t) {
				auto ins = jsonT::join(a, " or ", [=](njson it, size_t) {
					return ("(" + r + "->'$." + toStr(kt) + "' LIKE '%" + toStr(it) + "%')");
					});
				return("(" + ins + ")");
				});
			push("(" + kin + ")");
		}
		// column_name, value, operator:'='
		void push_o(njson arr, bool is_and = true)
		{
			if (!arr.is_array() || arr.size() < 2 || arr[1].is_null())
			{
				return;
			}
			if (arr.size() == 2)
			{
				arr.push_back("=");
			}
			auto ls = "(`" + jsonT::getStr(arr[0]) + "` " + jsonT::getStr(arr[2]) + " " + jsonT::get_value(arr[1], "'") + ")";
			_where.push_back({ ls, is_and ? "AND" : "OR" });
		}
		void push_in(std::string key, njson a, bool is_and = true)
		{
			if (key.find('-') == std::string::npos)
			{
				key = "`" + key + "`";
			}
			auto ls = key + " IN(" + jsonT::join(a, ",", [](njson it, size_t idx) {
				return jsonT::get_value(it);
				}) + ")";
			_where.push_back({ ls, is_and ? "AND" : "OR" });
		}
		// 表名，ts为当参数缺少时默认值
		void push_between(std::string key, njson first, njson second, const std::string& ts, bool is_and = true)
		{
			if (first.is_null())
			{
				return;
			}
			if (key.find('-') == std::string::npos)
			{
				key = "`" + key + "`";
			}
			std::string fs = jsonT::get_value(first, "'", ""), ss = jsonT::get_value(second, "'", ts);
			if (fs != "")
			{
				auto ls = "(" + key + " BETWEEN " + fs + " AND " + ss + ")";
				_where.push_back({ ls, is_and ? "AND" : "OR" });
			}
		}
		std::string str(bool isw = true)
		{
			make();
			if (_data == "")
			{
				isw = false;
			}
			return isw ? " WHERE " + _data : _data;
		}
	private:
		void make()
		{
			_data = jsonT::join(_where, " ", [](njson it, size_t idx) {
				std::string ret;
				if (idx > 0)
				{
					ret = " " + jsonT::getStr(it[1]) + " ";
				}
				ret += jsonT::getStr(it[0]);
				return ret;
				});
			_data = hstring::replace_all<std::string>(_data, "  ", " ");
		}

	};

}
#endif

/*
njson n = njson::parse(str);//解析字符串，中文要utf8编码
std::string s=n.dump();//全部输出1行
auto s1= n.dump(2);//缩进2个空格
*/


//#include "xml2json.h"

#endif // __json_helper_h__