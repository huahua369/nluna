#ifndef __print_time_h__
#define __print_time_h__
#include <string>
#include <chrono>
#include <vector>
#include <set>

class print_time
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _begin;
	std::string _str;
public:
	print_time(std::string str = "") :_begin(std::chrono::high_resolution_clock::now()), _str(str)
	{

	}
	~print_time()
	{
		double aa = elapsed_micro();
		aa *= 0.001;
		printf("time:\t\x1b[32;1m%lf\x1b[0mms\t(\x1b[33m%s\x1b[0m)\t\n", aa, _str.c_str());
	}
	void set_str(const std::string& s)
	{
		_str = s;
	}
	int64_t elapsed() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}
	//微秒
	int64_t elapsed_micro() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}
private:

};
namespace ts {
	static const char* CharsCheck(const char* str)
	{
		while ((*str >= 'a' && *str <= 'z') ||
			(*str >= 'A' && *str <= 'Z') ||
			(*str >= '0' && *str <= '9') ||
			*str == '_')
		{
			str++;
		}
		return str;
	}
	static size_t getemail(const char* str, std::set<std::string>& out)
	{
		std::set<std::string>& ks = out;
		std::string key;
		int cuc = 0;
		const char* t1 = nullptr;
		auto tb = str; size_t inc = 0;
		for (; *tb; tb++)
		{
			auto t = CharsCheck(tb);
			if (t != tb)
			{
				if (t1)
				{
					if (t[0] == '@' || (key.size() > 0 && t[0] == '.'))
					{
						key += std::string(t1, t);
						cuc++;
						key.push_back(t[0]);
					}
					else {
						if (cuc > 1)
						{
							key += std::string(t1, t);
							ks.insert(key); key = ""; cuc = 0;
							inc++;
						}
					}
					tb = t;
				}
			}
			t1 = t + 1;
		}
		return inc;
	}
}
#endif // !__print_time_h__
#if 0
// 使用花括号自动计算本块代码执行时间
{
	print_time pt("get_extent_str");
	auto c = ts::getemail("<gf dfg> af_fds@gfdgf.com</gf>a@123.com");
	std::vector<std::string> vs;
	std::set<std::string> es;
	int ainc = 0;
	for (auto& it : vs)
		ainc += ts::getemail(it.c_str(), es);
	printf("总数：%d\t实际:%d\n", ainc, (int)es.size());
	for (auto& it : es)
	{
		printf("%s\n", it.c_str());
	}
}
#endif
