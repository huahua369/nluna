#ifndef __utils_info_h__
#define __utils_info_h__
/*
 通用工具结构

*/
namespace hz
{
	// -----------------------------------------------------------------------------------------

	std::string str_iconv(const char* str, int len, const char* tocode, const char* fromcode);
	std::string str_iconv(const std::string& str, const char* tocode, const char* fromcode);
	// 获取支持的编码列表
	std::set<std::string> get_iconv_list();
	// utf8转gbk
	std::string u8_gbk(const std::string& str);
	// gbk转utf8
	std::string gbk_u8(const std::string& str);
	// 以下utf16/32都是LE
	// utf8转utf16/32
	std::wstring u8_w(const std::string& str);
	std::string w_u8(const std::wstring& str);
	std::string w_gbk(const std::wstring& str);
	std::wstring gbk_w(const std::string& str);

	std::string w2a(const std::wstring& str, bool isu8);
	std::wstring a2w(const std::string& str, bool isu8);
	// utf8转utf16
	std::u16string u8_u16(const std::string& str);
	std::u16string gbk_u16(const std::string& str);
	std::u32string u8_u32(const std::string& str);
	std::u32string gbk_u32(const std::string& str);

	//std::vector<std::string>
	template <typename T>
	std::string join(const T& v, const std::string& sub)
	{
		std::string ret;
		size_t i = 0;
		for (auto& it : v)
		{
			if (i > 0)ret += sub;
			i++;
			ret += it;
		}
		return ret;
	}
	std::string join(const njson& v, const std::string& sub);
	// 可读json或cbor
	njson read_json(std::string fn);
	// 保存json到文件，-1则保存成cbor
	void save_json(const njson& n, std::string fn, int indent_cbor = 0);
}
// !hz
#endif // __utils_info_h__
