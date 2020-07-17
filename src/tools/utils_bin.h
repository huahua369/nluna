#pragma once
#include <string>


namespace hz {
	static std::string bin2hex(const char* str, int size, bool is_lower)
	{
		std::string ret;
		static const char* cts[] = { "0123456789ABCDEF", "0123456789abcdef" };
		auto ct = cts[is_lower ? 1 : 0];
		if (size > 0 && str)
		{
			ret.reserve((size_t)size * 2);
			for (size_t i = 0; i < size; i++)
			{
				ret.push_back(ct[((unsigned char)str[i]) >> 4]);
				ret.push_back(ct[((unsigned char)str[i]) & 0x0f]);
			}
		}
		return ret;
	}
	static std::string hex2bin(const char* str, int size)
	{
		std::string ret;
		if (size > 0 && str)
		{
			ret.reserve((size_t)size / 2);
			for (size_t i = 0; i < size; i++)
			{
				char c = str[i];
				char o;
				if (c >= '0' && c <= '9')
				{
					o = (c - '0') << 4;
				}
				else if (c >= 'a' && c <= 'f')
				{
					o = (c - 'a' + 10) << 4;
				}
				else if (c >= 'A' && c <= 'F')
				{
					o = (c - 'A' + 10) << 4;
				}
				else {
					break;
				}
				c = str[++i];
				if (c >= '0' && c <= '9')
				{
					o |= (c - '0');
				}
				else if (c >= 'a' && c <= 'f')
				{
					o |= (c - 'a' + 10);
				}
				else if (c >= 'A' && c <= 'F')
				{
					o |= (c - 'A' + 10);
				}
				else {
					break;
				}
				ret.push_back(o);
			}
		}
		return ret;
	}
}
//!hz
