
#ifndef __hlUtil__H_
#define __hlUtil__H_
#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#endif
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <locale>
#include <codecvt>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>  
#include <thread>  
#include <queue>  
#include <functional>
#include <stdio.h>
#include <string.h>
#ifdef ICONV
#include <locale/iconv.hpp>
#endif
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#if 1
#ifdef _HAS_SHARED_MUTEX
#define HSM
#endif
typedef std::shared_mutex LockS;
#define LOCK_R(ls) std::shared_lock<LockS> __locker_(ls)
#define LOCK_W(ls) std::unique_lock<LockS> __locker_(ls)

typedef std::recursive_mutex Lock;
#define LOCK(l) std::lock_guard<Lock> __locker_(l)
#endif
#if 0
//������
Lock lock_n;
//����
LOCK(lock_n);
//д��
LOCK(lock_n);
#endif

namespace hz
{
	class hstring
	{
	public:
		hstring()
		{
		}

		~hstring()
		{
		}

		static bool isNum(const std::string& str)
		{
			std::stringstream sin_(str);
			double d;
			int64_t i64;
			char c;
			if (!(sin_ >> i64))
			{
				return false;
			}
			if (!(sin_ >> d))
			{
				return false;
			}
			if (sin_ >> c)
			{
				return false;
			}
			return true;
		}
		//�ַ����ָ��
		template <typename T>
		static int split_string(T str, T pattern, std::vector<T>* outvs)
		{
			typename T::size_type pos;
			std::vector<T>* result = outvs;
			size_t size = str.size();
			size_t pn = pattern.size();
			size_t i = 0;
			for (; i < size; i++)
			{
				pos = str.find(pattern, i);
				if (pos < size)
				{
					T s = str.substr(i, pos - i + pn);
					result->push_back(s);
					i = pos + pn - 1;
				}
				else
				{
					break;
				}
			}
			if (i < size)
			{
				T s = str.substr(i);
				result->push_back(s);
			}
			return 0;
		}


		//�ַ����ָ��

		template <typename T>
		static std::vector<T> split_aw(T str, const T pattern)
		{
			std::vector<T> result;
			size_t pos;
			str += pattern;//��չ�ַ����Է������
			int64_t size = str.size();
			result.clear();
			int ct = 0;
			for (int64_t i = 0; i < size; i++)
			{
				pos = str.find(pattern, i);
				if (pos < size)
				{
					T s = str.substr(i, pos - i);
					result.push_back(s);
					i = pos + pattern.size() - 1;
					ct++;
				}
			}
			return result;
		}
		static void split(std::string str, const std::string & pattern, std::vector<std::string> & result)
		{
			std::string::size_type pos;
			str += pattern;//��չ�ַ����Է������
			int size = str.size();
			result.clear();
			int ct = 0;
			for (int i = 0; i < size; i++)
			{
				pos = str.find(pattern, i);
				if (pos < size)
				{
					std::string s = str.substr(i, pos - i);
					result.push_back(s);
					i = pos + pattern.size() - 1;
					ct++;
				}
			}
		}

		static std::vector<std::string> split(const std::string & str, const std::string & pattern)
		{
			std::vector<std::string> vs;
			hz::hstring::split(str, pattern, vs);
			return vs;
		}
		// ��ָ��
		static std::vector<std::string> split_m(const std::string & str, const std::string & pattern)
		{
			std::vector<std::string> vs;
			std::string tem;
			for (auto ch : str)
			{
				if (pattern.find(ch) == -1 || pattern.empty())
				{
					tem.push_back(ch);
				}
				else
				{
					vs.push_back(tem);
					tem = "";
				}
			}
			if (tem != "")
			{
				vs.push_back(tem);
			}
			return vs;
		}
		static void forSplit(const std::string & str, const std::string & pattern, std::function<void(const std::string&)> func)
		{
			std::vector<std::string> vs;
			hz::hstring::split(str, pattern, vs);
			if (func)
			{
				for (auto& it : vs) {
					func(it);
				}
			}
		}
		static std::vector<std::vector<std::string>> csv_to_v(const std::string & str)
		{
			std::vector<std::vector<std::string>> ret;
			std::vector<std::string> lines = split(str, "\n");
			ret.resize(lines.size());
			for (size_t i = 0; i < lines.size(); i++)
			{
				if (lines[i] != "" && lines[i] != "\r")
					ret[i] = split(replace(lines[i], "\r", ""), ",");
			}

			return ret;
		}
		// -----------------------------------------------------------------------------------
		// תСд
		static void toLower_(std::string & str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		}
		// ת��д
		static void toUpper_(std::string & str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		}
		// תСд
		static void toLower_(std::wstring & str)
		{
			transform(str.begin(), str.end(), str.begin(), ::towlower);
		}
		// ת��д
		static void toUpper_(std::wstring & str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::towupper);
		}
#if 1
		// תСд
		static std::string toLower(const std::string & str_)
		{
			auto str = str_;
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			return str;
		}
		// ת��д
		static std::string toUpper(const std::string & str_)
		{
			auto str = str_;
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
			return str;
		}
		// תСд
		static std::wstring toLower(const std::wstring & str_)
		{
			auto str = str_;
			std::transform(str.begin(), str.end(), str.begin(), ::towlower);
			return str;
		}
		// ת��д
		static std::wstring toUpper(const std::wstring & str_)
		{
			auto str = str_;
			std::transform(str.begin(), str.end(), str.begin(), ::towupper);
			return str;
		}
#else
		// תСд
		template<typename T>
		static T toLower(T & str_)
		{
			auto str = str_;
			std::transform(str.begin(), str.end(), str.begin(), sizeof(str[0]) == sizeof(char) ? ::tolower : ::towlower);
			return str;
		}
		// ת��д
		template<typename T>
		static T toUpper(T & str_)
		{
			auto str = str_;
			std::transform(str.begin(), str.end(), str.begin(), sizeof(str[0]) == sizeof(char) ? ::toupper : ::towupper);
			return str;
		}
#endif

		static std::string replace_trim(std::string & strBase, const std::string & strSrc, const std::string & strDes)
		{
			std::string::size_type pos = 0;
			std::string::size_type srcLen = strSrc.size();
			std::string::size_type desLen = strDes.size();

			pos = strBase.find(strSrc, pos);
			while ((pos != std::string::npos))
			{
				if (pos == 0 || (pos + srcLen == strBase.size()))
				{
					strBase.replace(pos, srcLen, strDes);
				}
				pos = strBase.find(strSrc, (pos + desLen));
			}
			return strBase;
		}
		static void string_replace(std::string & strBase, const std::string & strSrc, const std::string & strDes)
		{
			std::string::size_type pos = 0;
			std::string::size_type srcLen = strSrc.size();
			std::string::size_type desLen = strDes.size();
			pos = strBase.find(strSrc, pos);
			while ((pos != std::string::npos))
			{
				strBase.replace(pos, srcLen, strDes);
				pos = strBase.find(strSrc, (pos + desLen));
			}
		}
		static std::string replace(const std::string & instr, std::string src, std::string dst)
		{
			std::string ret = instr;
			if (ret.size() > 1024 && src.size() > 16)
			{
				Replace(ret, src, dst);
			}
			else
			{
				string_replace(ret, src, dst);
			}
			return ret;
		}

		template<typename Str>
		static Str& replace_all(Str & str, const Str & old_value, const Str & new_value)
		{
			while (true) {
				uint64_t pos(0);
				if ((pos = str.find(old_value)) != Str::npos)
					str.replace(pos, old_value.length(), new_value);
				else break;
			}
			return str;
		}
		template<typename Str>
		static Str& replace_d(Str & str, const Str & old_value, const Str & new_value)
		{
			for (uint64_t pos(0); pos != Str::npos; pos += new_value.length()) {
				if ((pos = str.find(old_value, pos)) != Str::npos)
					str.replace(pos, old_value.length(), new_value);
				else break;
			}
			return str;
		}
		static std::string& replace_ds(std::string & str, const std::string & old_value, const std::string & new_value)
		{
			for (uint64_t pos(0); pos != std::string::npos; pos += new_value.length()) {
				if ((pos = str.find(old_value, pos)) != std::string::npos)
					str.replace(pos, old_value.length(), new_value);
				else break;
			}
			return str;
		}
		// ȥ��ͷβ�ո�
		static std::string trim(const std::string & str, const char* pch = " ")
		{
			auto s = str;
			if (s.empty())
			{
				return s;
			}
			s.erase(0, s.find_first_not_of(pch));
			s.erase(s.find_last_not_of(pch) + 1);
			return s;
		}
		//�����ַ�������
		static size_t find_num(const std::string & s, const std::string & c)
		{
			std::string::size_type pos = 0;
			std::string::size_type srcLen = c.size();
			pos = s.find(c, pos);
			size_t ret = 0;
			while ((pos != std::string::npos))
			{
				ret++;
				pos = s.find(c, pos + srcLen);
			}
			return ret;
		}

		static size_t Replace(std::string & instr, std::string src, std::string dst, std::string * temp = nullptr)
		{
			size_t slen = src.length();
			size_t dlen = dst.length();
			std::string temp1;
			std::string& str = (temp ? *temp : temp1);
			str.clear();
			char* is = (char*)instr.c_str();
			size_t n = 0;
			char* ret = sunday_search_num(is, 0, src.c_str(), &n, [&is, dst, &str, slen](char* pos, char* b) {
				std::string tm(is, pos);
				str += tm;
				str += dst;
				is = pos + slen;
				return 0;
			});
			if (n)
			{
				instr = str + is;
			}
			return n;
		}
		static void Substitute(char* pInput, char* pOutput, char* pSrc, char* pDst)
		{
			char* pi, * po, * p;
			int nSrcLen, nDstLen, nLen;

			// ָ�������ַ������ζ�ָ��.   
			pi = pInput;
			// ָ������ַ������ζ�ָ��.   
			po = pOutput;
			// ���㱻�滻�����滻���ĳ���.   
			nSrcLen = strlen(pSrc);
			nDstLen = strlen(pDst);

			// ����piָ���ַ����е�һ�γ����滻����λ��,������ָ��(�Ҳ����򷵻�null).   
			p = strstr(pi, pSrc);
			if (p)
			{
				// �ҵ�.   
				while (p)
				{
					// ���㱻�滻��ǰ���ַ����ĳ���.   
					nLen = (int)(p - pi);
					// ���Ƶ�����ַ���.   
					memcpy(po, pi, nLen);
					memcpy(po + nLen, pDst, nDstLen);
					// �������滻��.   
					pi = p + nSrcLen;
					// ����ָ���������ָ��λ��.   
					po = po + nLen + nDstLen;
					// ��������.   
					p = strstr(pi, pSrc);
				}
				// ����ʣ���ַ���.   
				strcpy(po, pi);
			}
			else
			{
				// û���ҵ���ԭ������.   
				strcpy(po, pi);
			}
		}
		static std::string sub_str(const std::string & src, const std::string & first, const std::string & second, size_t * ot = 0)
		{
			std::string ret;
			if (src.size() && first.size() && second.size())
			{
				auto pos = src.find(first);
				if (pos != std::string::npos)
				{
					pos += first.size();
					auto pos1 = src.find(second, pos);
					if (pos1 == std::string::npos)
						pos1 = src.size();
					ret = src.substr(pos, pos1 - pos);
					if (ot) { *ot = pos; }
				}
			}
			return ret;
		}
		static std::string sub_str(const std::string & src, const std::string & first, size_t len, size_t * ot = 0)
		{
			std::string ret;
			if (src.size() && first.size())
			{
				auto pos = src.find(first);
				if (pos != std::string::npos)
				{
					if (len == 0)
					{
						ret = src.substr(0, pos);
					}
					else
					{
						pos += first.size();
						if (len == std::string::npos)
							len = src.size() - pos;
						ret = src.substr(pos, len);
					}
					if (ot) { *ot = pos; }
				}
			}
			return ret;
		}

		static std::vector<std::string> split_str(const std::string & src, const std::string & first, const std::string & sub = "\n")
		{
			std::vector<std::string> ret;
			auto lines = split(src, sub);
			for (auto& it : lines)
			{
				auto p = it.find(first);
				if (p != -1)
				{
					ret.push_back(it);
				}
			}
			return ret;
		}
		/**************************************************************************
		* Sunday�㷨�����ַ�����Ԥ����ʱ���Ǧ�(m + |��|)��ʱ�临�Ӷ��Ǧ�(n/m),
		* O(mn)������һ������±�KMP��BMҪ��.
		**************************************************************************/
		static int sunday_searchv(const char* txt, const char* pat, int offset[])
		{
			int i, j, n, m, c = 0;
			n = strlen(txt);
			m = strlen(pat);
			int  next[256];// = new int[256];
			for (i = 0; i < 256; i++)
				next[i] = m + 1;
			for (i = 0; i < m; i++)
				next[(unsigned char)pat[i]] = m - i;
			for (i = 0, j = 0; i <= n - m;) {
				for (j = 0; j < m; j++)
					if (txt[i + j] != pat[j])
						break;
				if (j == m)
					offset[c++] = i;
				i += next[(unsigned char)txt[i + m]];
			}
			return c;
		}
		static int sunday_search(char* srcstr, char* substr, int str_len, int sub_len, std::function<void(int pos)> fun)
		{
			unsigned char* str = (unsigned char*)srcstr, * sub = (unsigned char*)substr;
			int marks[256];
			int i, j, k;
			int ret = -1;
			for (i = 0; i < 256; i++) {
				marks[i] = -1;
			}
			if (str_len == -1)
				str_len = strlen((char*)str);
			if (sub_len == -1)
				sub_len = strlen((char*)sub);
			j = 0;
			for (i = sub_len - 1; i >= 0; i--) {
				if (marks[sub[i]] == -1) {
					marks[sub[i]] = sub_len - i;
					if (++j == 256) break;
				}
			}
			i = 0;    j = str_len - sub_len + 1;
			while (i < j) {
				for (k = 0; k < sub_len; k++) {
					if (str[i + k] != sub[k]) break;
				}
				if (k == sub_len) {
					ret = i;// (str + i);
					if (fun)
						fun(i);
					break;
				}
				k = marks[str[i + sub_len]];
				(k == -1) ? i = i + sub_len + 1 : i = i + k;
			}
			return ret;
		}
		static int sunday_search(const char* srcstr, const char* substr, int str_len, int sub_len)
		{
			unsigned char* str = (unsigned char*)srcstr, * sub = (unsigned char*)substr;
			int marks[256];
			int i, j, k;
			int ret = -1;
			for (i = 0; i < 256; i++) {
				marks[i] = -1;
			}
			if (str_len < 0)
				str_len = strlen((char*)str);
			if (sub_len < 0)
				sub_len = strlen((char*)sub);
			j = 0;
			for (i = sub_len - 1; i >= 0; i--)
			{
				if (marks[sub[i]] == -1)
				{
					marks[sub[i]] = sub_len - i;
					if (++j == 256) break;
				}
			}
			i = 0;    j = str_len - sub_len + 1;
			while (i < j)
			{
				for (k = 0; k < sub_len; k++)
				{
					if (str[i + k] != sub[k]) break;
				}
				if (k == sub_len) {
					ret = i;// (str + i);
					break;
				}
				k = marks[str[i + sub_len]];
				//(k == -1) ? i = i + sub_len + 1 : i = i + k;
				i += (k == -1) ? sub_len + 1 : k;
			}
			return ret;
		}
		static size_t ReplaceS(std::string & instr, std::string src, std::string dst, std::string * temp = nullptr)
		{
			size_t slen = src.length();
			size_t dlen = dst.length();
			std::string temp1;
			std::string& str = (temp ? *temp : temp1);
			str.clear();
			char* is = (char*)instr.c_str();
			size_t n = 0, pos = 0;
			for (; pos != -1;)
			{
				pos = sunday_search(is, src.c_str(), instr.length(), src.length());
				if (pos != -1)
				{
					str.append(is, pos);
					//str+= std::string(is, pos - is);
				}
			}
			char* ret = sunday_search_num(is, 0, src.c_str(), &n, [&is, dst, &str, slen](char* pos, char* b) {
				str += std::string(is, pos - is);
				str += dst;
				is = pos + slen;
				return 0;
			});
			if (n)
			{
				instr = str;
				instr += is;
			}
			return n;
		}
		static void* mymemcpy(void* dst, const void* src, size_t num)
		{
			if ((dst == NULL) && (src == NULL))return dst;
			size_t wordnum = num / sizeof(size_t);//�����ж��ٸ�32λ����4�ֽڿ���  
			size_t slice = num % sizeof(size_t);//ʣ��İ��ֽڿ���  
			size_t * pintsrc = (size_t*)src;
			size_t * pintdst = (size_t*)dst;
			while (wordnum--) * pintdst++ = *pintsrc++;
			while (slice--) * ((char*)pintdst++) = *((char*)pintsrc++);
			return dst;
		}
		//�����ַ�������
		static char* sunday_search_num(const char* str, int lens, const char* sub, size_t * num = nullptr, std::function<int(char* pos, char* begin)> func = nullptr)
		{
			char* s = (char*)str;
			char* rt = NULL;
			size_t t = -1;
			int i = 0, len = lens ? lens : strlen((char*)str), slenb = strlen((char*)sub);
			int js = 0;
			size_t dlen = 0;
			for (; (s - (char*)str) < len;)
			{
				dlen = len - (s - str);
				t = sunday_search(s, sub, dlen, slenb); //strstr(s, (char*)sub);//
				if (t != -1) {
					t += (size_t)s;
					s = (char*)t + slenb + 1;
					++i;
					if (func)
					{
						if (func((char*)t, rt) == -1)
							break;;
					}
					rt = (char*)t;
				}
				else
				{
					rt = s;
					break;
				}
			}
			auto isk = (s - (char*)str);
			if (num)
				* num = i;
			return rt;
		}
	private:
		static bool IsTextUTF8(char* str, uint64_t length)
		{
			unsigned long nBytes = 0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�  
			auto tns = nBytes;
			unsigned char chr;
			bool bAllAscii = true; //���ȫ������ASCII, ˵������UTF-8  
			bool isu = true;
			for (int i = 0; i < length; ++i)
			{
				chr = *(str + i);
				if ((chr & 0x80) != 0) // �ж��Ƿ�ASCII����,�������,˵���п�����UTF-8,ASCII��7λ����,����һ���ֽڴ�,���λ���Ϊ0,o0xxxxxxx  
					bAllAscii = false;
				if (nBytes == 0) //�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���  
				{
					if (chr >= 0x80)
					{
						if (chr >= 0xFC && chr <= 0xFD)
							nBytes = 6;
						else if (chr >= 0xF8)
							nBytes = 5;
						else if (chr >= 0xF0)
							nBytes = 4;
						else if (chr >= 0xE0)
							nBytes = 3;
						//else if (chr >= 0xC0)
							//nBytes = 2;
						else
							return false;
						tns = nBytes;
						nBytes--;
					}
				}
				else //���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx  
				{
					if ((chr & 0xC0) != 0x80)
						return false;

					nBytes--;
				}
			}
			if (nBytes > 0) //Υ������  
				return false;
			if (bAllAscii) //���ȫ������ASCII, ˵������UTF-8  
				return false;

			return true;
		}
	public:

		static bool IsTextUTF8(const char* str)
		{
			size_t len = strlen(str);
			return (len > 2) && (beUtf8(str) /*|| IsTextUTF8((char*)str, len)*/);
		}

		static size_t countGBK(const char* str)
		{
			//assert(str != NULL);
			intptr_t len = (intptr_t)strlen(str);
			size_t counter = 0;
			unsigned char head = 0x80;
			unsigned char firstChar, secondChar;

			for (intptr_t i = 0; i < len - 1; ++i)
			{
				firstChar = (unsigned char)str[i];
				if (!(firstChar & head))continue;
				secondChar = (unsigned char)str[i];
				if (firstChar >= 161 && firstChar <= 247 && secondChar >= 161 && secondChar <= 254)
				{
					counter += 2;
					++i;
				}
			}
			return counter;
		}

		static size_t countUTF8(const char* str)
		{
			//assert(str != NULL);
			size_t len = (size_t)strlen(str);
			size_t counter = 0;
			unsigned char head = 0x80;
			unsigned char firstChar;
			for (size_t i = 0; i < len; ++i)
			{
				firstChar = (unsigned char)str[i];
				if (!(firstChar & head))continue;
				unsigned char tmpHead = head;
				size_t wordLen = 0, tPos = 0;
				while (firstChar & tmpHead)
				{
					++wordLen;
					tmpHead >>= 1;
				}
				if (wordLen <= 1)continue; //utf8��С����Ϊ2
				wordLen--;
				if (wordLen + i >= len)break;
				for (tPos = 1; tPos <= wordLen; ++tPos)
				{
					unsigned char secondChar = (unsigned char)str[i + tPos];
					if (!(secondChar & head))break;
				}
				if (tPos > wordLen)
				{
					counter += wordLen + 1;
					i += wordLen;
				}
			}
			return counter;
		}

		static bool beUtf8(const char* str)
		{
			size_t len = strlen(str);
			size_t iGBK = countGBK(str);
			size_t iUTF8 = countUTF8(str);
			if (iUTF8 % 3)
			{
				return false;
			}
			if (iUTF8 == iGBK)
			{
				return iUTF8 == 0 || IsTextUTF8((char*)str, len);
			}
			if (iUTF8 > 0)return true;
			return false;
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

			std::vector<char> tem(len);
			::WideCharToMultiByte(CP_ACP, 0, buf, -1, &tem[0], len, NULL, NULL);

			return &tem[0];
		}
		static void UnicodeToAnsi(const wchar_t* buf, char* tem, int tlen)
		{
			ZeroMemory(tem, tlen);
			int len = ::WideCharToMultiByte(CP_ACP, 0, buf, -1, NULL, 0, NULL, NULL);
			if (len == 0)
			{
				return;
			}
			if (len > tlen)
				len = tlen;
			::WideCharToMultiByte(CP_ACP, 0, buf, -1, tem, len, NULL, NULL);

		}
		static std::wstring Utf8ToUnicode(const char* buf)
		{
			int len = ::MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
			if (len == 0) return L"";

			std::vector<wchar_t> unicode(len);
			::MultiByteToWideChar(CP_UTF8, 0, buf, -1, &unicode[0], len);
			unicode.push_back(0);
			return unicode.data();
		}

		static std::string UnicodeToUtf8(const wchar_t* buf)
		{
			int len = ::WideCharToMultiByte(CP_UTF8, 0, buf, -1, NULL, 0, NULL, NULL);
			if (len == 0) return "";

			std::vector<char> utf8(len);
			::WideCharToMultiByte(CP_UTF8, 0, buf, -1, &utf8[0], len, NULL, NULL);
			utf8.push_back(0);
			return utf8.data();
		}
		static std::string AnsiToUtf8(const char* buf)
		{
			return UnicodeToUtf8(AnsiToUnicode(buf).c_str());
		}

		static std::string Utf8ToAnsi(const char* buf)
		{
			return UnicodeToAnsi(Utf8ToUnicode(buf).c_str());
		}
#else
#endif
	public:

		template <typename T>
		static void deleteAllMark(T & s, const T & mark)
		{
			unsigned int nSize = mark.size();
			while (1)
			{
				unsigned int pos = s.find(mark);
				if (pos == T::npos || pos == -1)
				{
					return;
				}
				s.erase(pos, nSize);
			}
		}
	private:

		};
	//!hstring
#ifdef ICONV
	class stru {
	public:
		static std::string AnsiToUtf8(const char* buf)
		{
			int len = strlen(buf);
			size_t outlen = len * 3;
			std::vector<char> cdst;
			cdst.resize(len * 3);
			bool code = code_convert("gbk", "utf8", (char*)buf, len, cdst.data(), &outlen);
			std::string strUtf8;
			if (code)
			{
				cdst[outlen] = 0;
				strUtf8 = cdst.data();
			}
			return strUtf8;
		}

		static std::string Utf8ToAnsi0(const char* buf)
		{
			std::vector<char> cname;
			int len = strlen(buf);
			size_t outlen = len;
			std::vector<char> cdst;
			cdst.resize(len + 1);
			bool code = code_convert("utf-8", "gbk", (char*)buf, len, cdst.data(), &outlen);
			std::string strGBK;
			if (code)
			{
				strGBK = cdst.data();
			}
			return strGBK;
		}
		static std::wstring Utf8ToUnicode(const char* buf)
		{
			int len = strlen(buf);
			size_t outlen = len * 2;
			std::vector<wchar_t> cdst;
			cdst.resize(len + 1);
			bool code = code_convert("utf8", "wchart", (char*)buf, len, (char*)cdst.data(), &outlen);
			std::wstring str;
			if (code)
			{
				cdst[len] = 0;
				str = (wchar_t*)cdst.data();
			}
			return str;
		}
		static std::wstring AnsiToUnicode(const char* buf)
		{
			int len = strlen(buf);
			size_t outlen = len * 2;
			std::vector<wchar_t> cdst;
			cdst.resize(len + 1);
			bool code = code_convert("gbk", "wchart", (char*)buf, len, (char*)cdst.data(), &outlen);
			std::wstring str;
			if (code)
			{
				cdst[len] = 0;
				str = (wchar_t*)cdst.data();
			}
			return str;
		}

		static std::string UnicodeToAnsi(const wchar_t* buf)
		{
			int len = wcslen(buf);
			std::vector<wchar_t> cdst;
			size_t outlen = len * 2;
			cdst.resize(outlen + 1);
			bool code = code_convert("wchart", "gbk", (char*)buf, len * 2, (char*)cdst.data(), &outlen);
			std::string strGBK;
			if (code)
			{
				for (auto it : cdst)
				{
					if (it < 0x00ff)
						strGBK.push_back(it);
					else {
						char c[8] = { 0 };
						*((wchar_t*)c) = it;
						strGBK.push_back(c[1]);
						strGBK.push_back(c[0]);

					}
				}
			}
			return strGBK;
		}
		static std::string UnicodeToUtf80(const wchar_t* buf)
		{
			int len = wcslen(buf);
			std::vector<char> cdst;
			size_t outlen = len * 3;
			cdst.resize(outlen + 1);
			bool code = code_convert("wchart", "utf8", (char*)buf, len * 2, cdst.data(), &outlen);
			std::string strGBK;
			if (code)
			{
				cdst[outlen] = 0;
				strGBK = cdst.data();
			}
			return strGBK;
		}

		static bool code_convert(const char* from_charset, const char* to_charset, char* inbuf, size_t inlen, char* outbuf, size_t* outlen)
		{
			iconv_t cd;
			char** pin = &inbuf;
			char** pout = &outbuf;
			cd = iconv_open(to_charset, from_charset);
			if (cd == 0)
			{
				return false;
			}
			memset(outbuf, 0, *outlen);
			int convert = iconv(cd, pin, &inlen, pout, outlen);
			if (convert == -1)
			{
				iconv_close(cd);
				return false;
			}
			iconv_close(cd);
			return true;
		}
	};
#endif


	template<class T>
	T BinarySearch(T k, std::vector<T> & a, int& len)//k���� ,  a�Ѿ�����͵��ߵ����� ��len���鳤��
	{
		int high = len > 0 ? len : a.size(), low = 0;
		int& m = len, alen = len;
		if (alen == 1)
			return a[0];
		while (high >= low)
		{
			m = (high + low) / 2;
			T pm = a[m];
			if (m >= alen - 1 || m < 1)
				break;
			pm = (k - a[m]);
			// T cm = (a[m] - a[m - 1]) / 2; && ((k - a[m])>(a[m + 1] - a[m]) / 2)

			if (k < a[m])
			{
				if (k > a[m - 1])
				{
					//len = m;
					return ((abs(a[m] - k)) / 2) < (abs(k - a[m - 1]) / 2) ? a[m] : a[--m];
				}
				high = m - 1;
			}
			else if ((k > a[m]))
			{
				if (k < a[m + 1])
				{
					//len = m;
					return (abs(k - a[m]) / 2) < (abs(k - a[m + 1]) / 2) ? a[m] : a[++m];
				}
				low = m + 1;
			}
			else if (k == a[m])
			{
				return a[m];
			}
		}
		if (m >= alen && alen > 1)
			m = alen - 1;
		if (m == alen - 1)
			--m;
		if (m < 0)
		{
			m = 0;
		}
		else if (k < a[m])
		{
			if (m > 0)
			{
				return (((abs(a[m] - k)) / 2) < (abs(k - a[m - 1]) / 2)) ? a[m] : a[--m];
			}
		}
		else if (k > a[m])
		{
			auto m1 = m + 1;
			if (m1 < alen)
			{
				return (abs(k - a[m]) / 2) < (abs(k - a[m1]) / 2) ? a[m] : a[++m];
			}
		}
		return a[m];
	}
	//-----------------------------------------------------------------------------
	template<class T>
	T BinarySearch(T k, std::vector<T> & a, int& len, T er)//k���� ,  a�Ѿ�����͵��ߵ����� ��len���鳤��
	{
		int64_t m = 0, left = 0, right = a.size();
		while (left <= right)
		{
			m = (left + right) >> 1;
			if (m == left && !(left < right))
			{
				break;
			}
			m = std::min((int64_t)a.size() - 1, m);
			if ((a[m] > k) > er)
				right = m - 1;
			else
				left = m + 1;
			if (abs(a[m] < k) < er)
				return a[m];
		}
		m = std::min((int64_t)a.size() - 1, m);
		return a[m];
	}
	//k��Ҫ������ֵ ,  a�����������, is_less=true����ѡС��k��ֵ,false��ѡ�����
	template<class T>
	T binary_search_vague(T k, std::vector<T> & a, int64_t * idx, bool is_less = true)
	{
		std::map<T, int64_t> ers;
		int64_t m = 0, left = 0, right = a.size();
		while (left <= right)
		{
			m = (left + right) >> 1;
			if (m == left && !(left < right))
				break;
			m = std::min((int64_t)a.size() - 1, m);
			if ((a[m] > k))
				right = m - 1;
			else
				left = m + 1;
			if (a[m] == k)
				break;
		}
		auto mless = [&](T x, int64_t idxm) {
			if (is_less) { if (x >= 0) { ers[x] = idxm; } }
			else { ers[abs(x)] = idxm; }
		};
		if (m >= a.size())
			m = a.size() - 1;
		mless(k - a[m], m);
		if (m > 0)
		{
			mless(k - a[m - 1], m - 1);
		}
		if (m + 1 < a.size() - 1)
		{
			mless(k - a[m + 1], m + 1);
		}
		if (ers.size())
			m = ers.begin()->second;
		if (idx)* idx = m;
		return a[m];
	}
	//-----------------------------------------------------------------------------
	class Util
	{
	public:
		Util()
		{
		}

		~Util()
		{
		}
		inline static int h_atoi(const char* str, int* out = 0)
		{
			const char* t = str;
			int value = 0;
			bool b_plus = true; //�жϷ���

			while (*str) //���˷���
			{
				if (*str == '+')
				{
					str++;
					break;
				}
				if (*str == '-')
				{
					b_plus = false;
					str++;
					break;
				}
				if ((*str == '.' || (*str >= '0' && *str <= '9')))
				{
					break;
				}
				str++;
			}

			//while (*str > 0x2f && *str < 0x40)
			while (*str >= '0' && *str <= '9')
			{
				value = (value * 10) + (*str - '0');
				str++;
			}
			if (!b_plus)
				value = -value;
			if (out)
			{
				*out = str - t;
			}
			return value;
		}
		//------------�ַ�������ת���ɸ�����-----------------------------------------------------------------
		static char* strTofloatv(const char* str, float* f, int num)//�ַ���������λ�ã�ת���ĸ��������سɹ�ת��������
		{
			char* st = (char*)str;
			int i = 0;
			while (*st && i < num)
			{
				if ((st[0] == 0x2d) || st[0] > 0x2f && st[0] < 0x40)
				{
					f[i] = atof(st);
					++i;
					st += st[0] == '-' ? 10 : 9;
				}
				else st++;
			}
			return st;
		}

		static char* strTointv(const char* str, int* f, int num)//�ַ���������λ�ã�ת���ĸ��������سɹ�ת��������
		{
			char* st = (char*)str;
			int i = 0;
			while (*st && i < num)
			{
				if ((st[0] == 0x2d) || st[0] > 0x2f && st[0] < 0x40)
				{
					f[i] = atoi(st);
					++i;
					st += st[0] == '-' ? 10 : 9;
				}
				else st++;
			}
			return st;
		}
		static std::wstring AtoW(const std::string & text)
		{
			return (hz::hstring::IsTextUTF8(text.c_str())) ?
				utf8_to_wstring(text) :
				ansi_to_wstring(text);
		}


		// convert UTF-8 string to wstring  
		static std::wstring utf8_to_wstring(const std::string & str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.from_bytes(str);
		}
		static std::string utf8_to_ansi(const std::string & str)
		{
			return wstring_to_ansi(utf8_to_wstring(str));
		}
		// convert wstring to UTF-8 string  
		static std::string wstring_to_utf8(const std::wstring & str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.to_bytes(str);
		}

		static std::string wstring_to_ansi(const std::wstring & src)
		{
			std::string str;
			std::locale sys_locale("");

			const wchar_t* data_from = src.c_str();
			const wchar_t* data_from_end = src.c_str() + src.size();
			const wchar_t* data_from_next = 0;

			int wchar_size = 4;
			std::vector<char> tem;
			tem.resize((src.size() + 1) * wchar_size);
			char* data_to = (char*)tem.data();
			char* data_to_end = data_to + (src.size() + 1) * wchar_size;
			char* data_to_next = 0;

			memset(data_to, 0, (src.size() + 1) * wchar_size);

			typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
			mbstate_t out_state = { 0 };
			auto result = std::use_facet<convert_facet>(sys_locale).out(
				out_state, data_from, data_from_end, data_from_next,
				data_to, data_to_end, data_to_next);
			if (result == convert_facet::ok)
			{
				str = data_to;
				return str;
			}
			return str;
		}

		static std::wstring ansi_to_wstring(const std::string & src)
		{
			std::wstring wstr;
			std::locale sys_locale("");
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
			if (result == convert_facet::ok)
			{
				wstr = data_to;
				return wstr;
			}
			return wstr;
		}
		static std::string ansi_to_utf8(const std::string & src)
		{
			return wstring_to_utf8(ansi_to_wstring(src));
		}
#ifdef _WIN32
#ifndef _Clipboard_
#define _Clipboard_
		//-----------------------------------------------------------------------------
		static void setClipboard(const char* text, int len)
		{
			//�򿪼��а�
			if (OpenClipboard(0) & len > 0)
			{
				HGLOBAL hClip;
				TCHAR* pBuf = nullptr;
				if (text == "")
				{
					CloseClipboard();
					return;
				}
				//��ռ��а�����
				EmptyClipboard();
				//������ȫ���ڴ�ռ�
				hClip = GlobalAlloc(GHND, len + 1);
				//��סȫ���ڴ�ռ�
				pBuf = (TCHAR*)GlobalLock(hClip);
				//������д��ȫ���ڴ�ռ�
				memcpy(pBuf, text, len);
				//���ռ��е�����д����а�
#ifndef UNICODE
				SetClipboardData(CF_TEXT, hClip);         //��������
#else
				SetClipboardData(CF_UNICODETEXT, hClip);         //��������
#endif
																 //����ȫ���ڴ�ռ�
				GlobalUnlock(hClip);         //����
											 //�ͷ�ȫ���ڴ�ռ�
				GlobalFree(hClip);
				//�رռ��а�
				CloseClipboard();
			}
		}
		static void setClipboardW(const std::wstring & text)
		{
			//�򿪼��а�
			if (!text.empty() && OpenClipboard(0))
			{
				HGLOBAL hClip;
				TCHAR* pBuf = nullptr;
				if (text.empty())
				{
					CloseClipboard();
					return;
				}
				//��ռ��а�����
				EmptyClipboard();
				//������ȫ���ڴ�ռ�
				size_t len = (text.length() + 1) * sizeof(wchar_t);
				hClip = GlobalAlloc(GHND, len);
				//��סȫ���ڴ�ռ�
				pBuf = (TCHAR*)GlobalLock(hClip);
				//������д��ȫ���ڴ�ռ�
				memcpy(pBuf, text.c_str(), len);
				//���ռ��е�����д����а�
				SetClipboardData(CF_UNICODETEXT, hClip);         //��������
																 //����ȫ���ڴ�ռ�
				GlobalUnlock(hClip);         //����
											 //�ͷ�ȫ���ڴ�ռ�
				GlobalFree(hClip);
				//�رռ��а�
				CloseClipboard();
			}
		}
		static const char* getClipboard(int& type)
		{
			char* t = 0;
			if (IsClipboardFormatAvailable(CF_UNICODETEXT))
				type = CF_UNICODETEXT;
			else if (IsClipboardFormatAvailable(CF_TEXT))
				type = CF_TEXT;

			if ((type == CF_UNICODETEXT || type == CF_TEXT) && OpenClipboard(0))
			{
				HGLOBAL hMem = GetClipboardData(type);
				t = (char*)GlobalLock(hMem);
				GlobalUnlock(hMem);
				CloseClipboard();
			}
			return t;
		}
#endif
#endif
		// ����
/*
		template<class _InIt1,
			class _InIt2,
			class _OutIt>
			static void n_push_back(_InIt1 _First1, _InIt2 _Last1, _OutIt _Dest)
		{
			for (; _UFirst1 != _ULast1 && _UFirst2 != _ULast2; ++_UDest)
			{
			}
		}*/

	private:

	};
	//!util



}//!hz


#endif // !__hlUtil__H_