#ifndef __event__h__
#define __event__h__
#include <functional>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <list>
#include <queue>
#include <chrono>
#include <future>
#include "hlIME.h"
#include "hl_time.h"
#ifndef HZ_BIT
//位左移
#define HZ_BIT(x) (1<<x)
#endif
#ifndef LUNA_MK_UP

//----------------------状态-------------------------------------------------------
#define LUNA_MK_UP 			HZ_BIT(0) //鼠标左键弹起
#define LUNA_MK_DOWN	 	HZ_BIT(1) //按下
#define LUNA_MK_MOVE 		HZ_BIT(2) //移动
#define LUNA_MK_HOVER 		HZ_BIT(3) //进入
#define LUNA_MK_LEAVE 		HZ_BIT(4) //离开
#define LUNA_MK_FOCUS 		HZ_BIT(5) //焦点 
#define LUNA_MK_DISABLE 	HZ_BIT(6) //禁用 
#define LUNA_MK_CONTROL 	HZ_BIT(7) //CTRL键按下
#define LUNA_MK_SHIFT 		HZ_BIT(8) //shift 
#define LUNA_MK_ALT 		HZ_BIT(9) //alt
#define LUNA_MK_MENU 		HZ_BIT(10) //alt菜单
#define LUNA_MK_LBUTTON 	HZ_BIT(11) //鼠标左键 
#define LUNA_MK_RBUTTON 	HZ_BIT(12) //鼠标右键
#define LUNA_MK_CLICK 		HZ_BIT(13) //单击
#define LUNA_MK_DBLCLK 		HZ_BIT(14) //双击
#define LUNA_MK_RDOWN 		HZ_BIT(15) //右键按下
#define LUNA_MK_RUP 		HZ_BIT(15) //右键弹起
#define LUNA_MK_CHANGE 		HZ_BIT(16) //内容改变
#define LUNA_MK_SCROLL		HZ_BIT(17) //滚轮

#define LUNA_KS_IME 		HZ_BIT(18) //输入法
#define LUNA_KS_KEY 		HZ_BIT(19) //键盘 

#define LUNA_KS_IMEKEY      (LUNA_KS_IME|LUNA_KS_KEY)

#define LUNA_MK_DRAG 		HZ_BIT(20) //拖动事件
#define LUNA_MK_MBUTTON		HZ_BIT(21) //中键
#define LUNA_MK_MDOWN 		HZ_BIT(22) //中键按下
#define LUNA_MK_MUP 		HZ_BIT(23) //中键弹起
#define LUNA_MK_OVER 		HZ_BIT(24) //悬停
//-----------------------------------------------------------------------------



/**/

#define LUNA_C_widget 			HZ_BIT(0) 
#define LUNA_C_label			HZ_BIT(1) 
#define LUNA_C_button			HZ_BIT(2) 
#define LUNA_C_edit				HZ_BIT(3) 
#define LUNA_C_imageview		HZ_BIT(4) 
#define LUNA_C_treeview			HZ_BIT(5) 
#define LUNA_C_layout			HZ_BIT(6) 
#define LUNA_C_listview			HZ_BIT(7) 
#define LUNA_C_TableView		HZ_BIT(8) 
#define LUNA_C_Scenes			HZ_BIT(9) 
#define LUNA_C_LayoutView		HZ_BIT(10) 
#define LUNA_C_AutoLayout		HZ_BIT(11) 
// #define LUNA_C_	HZ_BIT(12) 
// #define LUNA_C_		HZ_BIT(13) 
// #define LUNA_C_		HZ_BIT(14) 
// #define LUNA_C_		HZ_BIT(15) 
// #define LUNA_C_		HZ_BIT(15) 
// #define LUNA_C_		HZ_BIT(16) 
// #define LUNA_C_		HZ_BIT(17)


#endif // !LUNA_MK_UP

#ifndef HL_CALLBACK_0
// 基于C ++11新的回调函数指针
//参数个数，静态函数
#define HL_CALLBACK_s0(__selector__, ...)  std::bind(__selector__, ##__VA_ARGS__)
#define HL_CALLBACK_s1(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, ##__VA_ARGS__)
#define HL_CALLBACK_s2(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define HL_CALLBACK_s3(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define HL_CALLBACK_s4(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)
#define HL_CALLBACK_s5(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, ##__VA_ARGS__)
#define HL_CALLBACK_s6(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, ##__VA_ARGS__)
#define HL_CALLBACK_s7(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, ##__VA_ARGS__)
#define HL_CALLBACK_s8(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, ##__VA_ARGS__)
#define HL_CALLBACK_s9(__selector__, ...)  std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, ##__VA_ARGS__)
#define HL_CALLBACK_s10(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, ##__VA_ARGS__)
#define HL_CALLBACK_s11(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, ##__VA_ARGS__)
#define HL_CALLBACK_s12(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, ##__VA_ARGS__)
#define HL_CALLBACK_s13(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, ##__VA_ARGS__)
#define HL_CALLBACK_s14(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, ##__VA_ARGS__)
#define HL_CALLBACK_s15(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, ##__VA_ARGS__)
#define HL_CALLBACK_s16(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, ##__VA_ARGS__)
#define HL_CALLBACK_s17(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, ##__VA_ARGS__)
#define HL_CALLBACK_s18(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18, ##__VA_ARGS__)
#define HL_CALLBACK_s19(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18, std::placeholders::_19, ##__VA_ARGS__)
#define HL_CALLBACK_s20(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18, std::placeholders::_19, std::placeholders::_20, ##__VA_ARGS__)

//普通成员
#define HL_CALLBACK_0(__selector__, __target__, ...)  std::bind(__selector__, __target__, ##__VA_ARGS__)
#define HL_CALLBACK_1(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, ##__VA_ARGS__)
#define HL_CALLBACK_2(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define HL_CALLBACK_3(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define HL_CALLBACK_4(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)
#define HL_CALLBACK_5(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, ##__VA_ARGS__)
#define HL_CALLBACK_6(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, ##__VA_ARGS__)
#define HL_CALLBACK_7(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, ##__VA_ARGS__)
#define HL_CALLBACK_8(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, ##__VA_ARGS__)
#define HL_CALLBACK_9(__selector__, __target__, ...)  std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, ##__VA_ARGS__)
#define HL_CALLBACK_10(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, ##__VA_ARGS__)
#define HL_CALLBACK_11(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, ##__VA_ARGS__)
#define HL_CALLBACK_12(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, ##__VA_ARGS__)
#define HL_CALLBACK_13(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, ##__VA_ARGS__)
#define HL_CALLBACK_14(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, ##__VA_ARGS__)
#define HL_CALLBACK_15(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, ##__VA_ARGS__)
#define HL_CALLBACK_16(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, ##__VA_ARGS__)
#define HL_CALLBACK_17(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, ##__VA_ARGS__)
#define HL_CALLBACK_18(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18, ##__VA_ARGS__)
#define HL_CALLBACK_19(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18, std::placeholders::_19, ##__VA_ARGS__)
#define HL_CALLBACK_20(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18, std::placeholders::_19, std::placeholders::_20, ##__VA_ARGS__)
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif 
/*
事件类
继承写public hz::EventD<std::string>或public hz::EventD<int>都行


*/
namespace hz {
	class Node;

	/*
	Timer返回两个时间差
	reset函数重置

	void fun()
	{
	cout << ”hello word” << endl;
	}
	int main()
	{
	Timer t; //开始计时
	fun();
	cout << t.elapsed() << endl; //打印fun函数耗时多少毫秒
	cout << t.elapsed_micro() << endl; //打印微秒
	cout << t.elapsed_nano() << endl; //打印纳秒
	cout << t.elapsed_seconds() << endl; //打印秒
	cout << t.elapsed_minutes() << endl; //打印分钟
	cout << t.elapsed_hours() << endl; //打印小时
	}
	*/
#ifndef __hl_time__h__
	class Timer
	{
	public:
		Timer() : m_begin(std::chrono::high_resolution_clock::now()) {}
		void reset() { m_begin = std::chrono::high_resolution_clock::now(); }
		//默认输出毫秒
		int64_t elapsed() const
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//微秒
		int64_t elapsed_micro() const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//纳秒
		int64_t elapsed_nano() const
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//秒
		int64_t elapsed_seconds() const
		{
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//分
		int64_t elapsed_minutes() const
		{
			return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//时
		int64_t elapsed_hours() const
		{
			return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
	};
#endif
	//-----------------------------------------------------------------------------



	typedef std::function<void(void* ptr, const char* data, size_t size)>					EVENT_FUN;

	typedef struct _EVENT_STRUCT
	{
		EVENT_FUN func;
		void* userdata = nullptr;
		_EVENT_STRUCT() {}
		_EVENT_STRUCT(EVENT_FUN f, void* u = nullptr) : func(f), userdata(u)
		{}
	}EVENT_STRUCT;

	//-----------------------------------------------------------------------------
	class Event
	{
	public:
		//挂起s毫秒
		static void sleep(int s)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(s));
		}

		static std::string toStringPtr(void* ptr)
		{
			std::string s;
			s.resize(sizeof(void*));
			*((void**)&s[0]) = ptr;
			return s;
		}
		static void* toPtr(std::string s)
		{
			return *((void**)&s[0]);
		}
		/*
		int i = 123;
		std::string s = hz::Event::toStringInt(i);
		int istr = hz::Event::toInt(s);
		*/
		static std::string toStringInt(int d)
		{
			std::string s;
			s.resize(sizeof(int));
			*((int*)&s[0]) = d;
			return s;
		}
		static int toInt(std::string s)
		{
			return *((int*)&s[0]);
		}
		static size_t waitObject(size_t* ret)
		{
			if (ret)
			{
				while (*ret == 0)
				{
					sleep(1);
				}
			}
			return *ret;
		}

		//发消息到指定接收器
		/*static void send(EventD *e, T en, std::string data)
		{
		if (e)
		e->send(en, data);
		}*/

		static unsigned int pthreadSelf()
		{
#ifdef _WIN32
			return	GetCurrentThreadId();
#else
			return (unsigned int)pthread_self();;
#endif
		}
		static unsigned int pthreadID()
		{
#ifdef _WIN32
			return	GetCurrentThreadId();
#else
			return (unsigned int)pthread_self();;
#endif
		}
	public:
		Event()
		{
		}

		~Event()
		{
		}

	private:

	};
	//---------EventDispatcher--------------------------------------------------------------------
	template <typename T>
	class EventD :public Res
	{
	public:
		unsigned int thread_id = 0;
	public:
		typedef std::vector<EVENT_STRUCT>							EVENT_LIST;				//执行列表
		typedef std::multimap<T, EVENT_STRUCT>						EVENT_CALLBACK_MAP;     //消息映射表声明
		class ENdata
		{
		public:
			ENdata(T en, std::string data) :_en(en), _data(data)
			{
			}
			T _en;
			std::string _data;
		};

		//注册消息（en为消息类型，data方便判断对象），同一个消息类型同一个对象只有一个回调函数
		void on(T en, void* data, std::function<void(void* ptr, const char* data, size_t size)> func)
		{
			RegMessage(en, func, data ? data : this);
		}
		//删除消息处理
		void removeOn(T en, void* data = nullptr)
		{
			delMessage(en, data ? data : this);
		}
		//发消息
		void send(T en, std::string data)
		{
			LOCK(_lock_en);
			_en_list.push(new ENdata(en, data));
		}
		//执行消息回调，可以在独立线程
		void CallEvent()
		{
			if (!_en_list.empty())
			{
				//LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);
				LOCK(_lock_en);
				if (_en_list.empty())
					return;
				ENdata* p = _en_list.front();
				CallEventList(p->_en, p->_data.c_str(), p->_data.length());
				delete p;
				_en_list.pop();
			}
		}
		unsigned int Tid()
		{
			return thread_id;
		}
		void setTid(unsigned int id)
		{
			thread_id = id;
		}

	private:
		void delMessage(T msg, void* userdata)      //用户删除消息处理函数
		{
			LOCK(_lock_del);
			_del_list.push_back(std::pair<T, void*>(msg, userdata));
		}
		void RegMessage(T msg, EVENT_FUN func, void* userdata)      //注册消息处理函数
		{
			LOCK(_lock_en);
			bool isfind = true;
			auto range = _event_func_list.equal_range(msg);   //查找消息回调函数multimap
			if (range.first != range.second)
			{
				for (auto multi_it = range.first; multi_it != range.second; ++multi_it)
				{
					if (userdata == multi_it->second.userdata)
					{
						multi_it->second.func = func;
						isfind = false;
					}
				}
			}
			if (isfind)
				_event_func_list.insert(std::pair<T, EVENT_STRUCT>(msg, EVENT_STRUCT(func, userdata)));

		}
		int DelMessage(T msg, void* userdata)      //内部删除消息处理函数
		{
			int re = 0;
			auto range = _event_func_list.equal_range(msg);   //查找消息回调函数multimap
			for (auto multi_it = range.first; multi_it != range.second;)
			{
				if (userdata == multi_it->second.userdata)
				{
					_event_func_list.erase(multi_it++);
					++re;
				}
				else
				{
					++multi_it;
				}
			}
			return re;
		}
		void CallEventList(T msg, const char* data, size_t size)
		{
			pushEventList();
			EVENT_LIST* lt = getEventList();              //临时执行列表
			if (!lt->empty())
				lt->clear();
			auto range = _event_func_list.equal_range(msg);   //查找消息回调函数
			for (auto multi_it = range.first; multi_it != range.second; ++multi_it)
			{
				lt->push_back(multi_it->second);
			}
			for (auto it : (*lt))
			{
				if (it.func)
					it.func(it.userdata, data, size);
			}
			popEventList();
			if (!_del_list.empty())		//删除监听
			{
				LOCK(_lock_del);
				for (auto it : _del_list)
				{
					DelMessage(it.first, it.second);
				}
			}
		}
		//-----------------------------------------------------------------------------
		void initStackList()
		{
			while (!_stack_list.empty())
			{
				_stack_list.pop();
			}
			EVENT_LIST elst;
			_stack_list.push(elst);
		}

		void popEventList()
		{
			_stack_list.pop();
		}
		void pushEventList()
		{
			_stack_list.push(_stack_list.top());
		}
		EVENT_LIST* getEventList()
		{
			return &_stack_list.top();
		}
		size_t getEventListSize()
		{
			return _stack_list.size();
		}
		//-----------------------------------------------------------------------------

	protected:
		EventD()
		{
			initStackList();
		}
	public:
		~EventD()
		{
			_event_func_list.clear();
		}
		//-----------------------------------------------------------------------------

	private:
		EVENT_CALLBACK_MAP           _event_func_list;            //消息回调函数列表

		std::queue<ENdata*>			 _en_list;

		std::stack<EVENT_LIST>		 _stack_list;
		std::list<std::pair<T, void*>>  _del_list;				//删除表
		Lock						 _lock_del, _lock_en;
	};


#define Event_Send(e,en,data) hz::Event::send((EventD*)e,en,data)

	//----数据操作-----------------------------------------------------------------------------------------
#if 0
	template <typename T>
	inline void copydata(std::string& s, T n)
	{
		char buf[256];
		memcpy(buf, &n, sizeof(T));
		s.append((char*)buf, sizeof(T));
	}
	std::string format(char* format, ...)
	{
		std::string rs;
		va_list arg;
		va_start(arg, format);

		while (*format)
		{
			char ret = *format;
			if (ret == '%')
			{
				switch (*++format)
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
					while (*pc)
					{
						rs.push_back(*pc);
						pc++;
					}
					break;
				}

				case 'i':
				{
					int n = va_arg(arg, int);
					copydata(rs, n);
					break;
				}
				case 'p':
				{
					void* n = va_arg(arg, void*);
					copydata(rs, n);
					break;
				}
				case 'f':
				{
					float n = va_arg(arg, float);
					copydata(rs, n);
					break;
				}
				case 'd':
				{
					double n = va_arg(arg, double);
					copydata(rs, n);
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
#endif
#if 0
	/* -*- linux-c -*- ------------------------------------------------------- *
	*
	* Copyright (C) 1991, 1992 Linus Torvalds
	* Copyright 2007 rPath, Inc. - All Rights Reserved
	*
	* This file is part of the Linux kernel, and is made available under
	* the terms of the GNU General Public License version 2.
	*
	* ----------------------------------------------------------------------- */

	/*
	* Oh, it's a waste of space, but oh-so-yummy for debugging. This
	* version of printf() does not include 64-bit support. "Live with
	* it."
	*
	*/

	//#include "boot.h"

	static int skip_atoi(const char** s)
	{
		int i = 0;

		while (isdigit(**s))
			i = i * 10 + *((*s)++) - '0';
		return i;
	}

#define ZEROPAD    1        /* pad with zero */
#define SIGN    2        /* unsigned/signed long */
#define PLUS    4        /* show plus */
#define SPACE    8        /* space if plus */
#define LEFT    16        /* left justified */
#define SMALL    32        /* Must be 32 == 0x20 */
#define SPECIAL    64        /* 0x */

	static inline int __do_div(long& n, int base)
	{
		int __res;
		__res = ((unsigned long)n) % (unsigned)base;
		n = ((unsigned long)n) / (unsigned)base;
		return __res;
	}

	static char* number(char* str, long num, int base, int size, int precision,
		int type)
	{
		/* we are called with base 8, 10 or 16, only, thus don't need "G..." */
		static const char digits[20] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

		char tmp[66];
		char c, sign, locase;
		int i;

		/* locase = 0 or 0x20. ORing digits or letters with 'locase'
		* produces same digits or (maybe lowercased) letters */
		locase = (type & SMALL);
		if (type & LEFT)
			type &= ~ZEROPAD;
		if (base < 2 || base > 36)
			return NULL;
		c = (type & ZEROPAD) ? '0' : ' ';
		sign = 0;
		if (type & SIGN) {
			if (num < 0) {
				sign = '-';
				num = -num;
				size--;
			}
			else if (type & PLUS) {
				sign = '+';
				size--;
			}
			else if (type & SPACE) {
				sign = ' ';
				size--;
			}
		}
		if (type & SPECIAL) {
			if (base == 16)
				size -= 2;
			else if (base == 8)
				size--;
		}
		i = 0;
		if (num == 0)
			tmp[i++] = '0';
		else
			while (num != 0)
				tmp[i++] = (digits[__do_div(num, base)] | locase);
		if (i > precision)
			precision = i;
		size -= precision;
		if (!(type & (ZEROPAD + LEFT)))
			while (size-- > 0)
				*str++ = ' ';
		if (sign)
			*str++ = sign;
		if (type & SPECIAL) {
			if (base == 8)
				*str++ = '0';
			else if (base == 16) {
				*str++ = '0';
				*str++ = ('X' | locase);
			}
		}
		if (!(type & LEFT))
			while (size-- > 0)
				*str++ = c;
		while (i < precision--)
			*str++ = '0';
		while (i-- > 0)
			*str++ = tmp[i];
		while (size-- > 0)
			*str++ = ' ';
		return str;
	}
	template <typename T>
	inline void copynum(std::string& s, T n)
	{
		char buf[256];
		memcpy(buf, &n, sizeof(T));
		s.append((char*)buf, sizeof(T));
	}
	std::string vsp(const char* fmt, va_list args)
	{
		std::string str;
		int len;
		unsigned long num;
		int i, base;
		char buff[256] = { 0 };
		const char* s;

		int flags;        /* flags to number() */

		int field_width;    /* width of output field */
		int precision;        /* min. # of digits for integers; max
							  number of chars for from string */
		int qualifier;        /* 'h', 'l', or 'L' for integer fields */

		for (; *fmt; ++fmt) {
			if (*fmt != '%') {
				//*str++ = *fmt;
				continue;
			}

			/* process flags */
			flags = 0;
		repeat:
			++fmt;        /* this also skips first '%' */
			switch (*fmt) {
			case '-':
				flags |= LEFT;
				goto repeat;
			case '+':
				flags |= PLUS;
				goto repeat;
			case ' ':
				flags |= SPACE;
				goto repeat;
			case '#':
				flags |= SPECIAL;
				goto repeat;
			case '0':
				flags |= ZEROPAD;
				goto repeat;
			}

			/* get field width */
			field_width = -1;
			if (isdigit(*fmt))
				field_width = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				field_width = va_arg(args, int);
				if (field_width < 0) {
					field_width = -field_width;
					flags |= LEFT;
				}
			}

			/* get the precision */
			precision = -1;
			if (*fmt == '.') {
				++fmt;
				if (isdigit(*fmt))
					precision = skip_atoi(&fmt);
				else if (*fmt == '*') {
					++fmt;
					/* it's the next argument */
					precision = va_arg(args, int);
				}
				if (precision < 0)
					precision = 0;
			}

			/* get the conversion qualifier */
			qualifier = -1;
			if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
				qualifier = *fmt;
				++fmt;
			}

			/* default base */
			base = 10;

			switch (*fmt) {
			case 'c':
				if (!(flags & LEFT))
					while (--field_width > 0)
						str.push_back(' ');
				str.push_back((unsigned char)va_arg(args, int));
				while (--field_width > 0)
					str.push_back(' ');
				continue;

			case 's':
				s = va_arg(args, char*);
				len = strnlen(s, precision);

				if (!(flags & LEFT))
					while (len < field_width--)
						str.push_back(' ');
				for (i = 0; i < len; ++i)
					str.push_back(*s++);
				while (len < field_width--)
					str.push_back(' ');
				continue;

			case 'p':
			{
				if (field_width == -1) {
					field_width = 2 * sizeof(void*);
					flags |= ZEROPAD;
				}
				void* p = va_arg(args, void*);
				copynum(str, p);
				continue;
			}

			case 'n':
				if (qualifier == 'l') {
					long* ip = va_arg(args, long*);
					//*ip = (str - buf);
				}
				else {
					int* ip = va_arg(args, int*);
					//*ip = (str - buf);
				}
				continue;

			case '%':
				//*str++ = '%';
				continue;

				/* integer number formats - set up the flags and "break" */
			case 'o':
				base = 8;
				copynum(str, va_arg(args, short));
				continue;
			case 'f':
			{
				copynum(str, (qualifier == 'l') ? va_arg(args, double) : va_arg(args, float));

			}continue;
			case 'd':
			case 'i':
				flags |= SIGN;
				copynum(str, (qualifier == 'l') ? va_arg(args, int64_t) : va_arg(args, int));
				continue;
			case 'u':
				copynum(str, (qualifier == 'l') ? va_arg(args, uint64_t) : va_arg(args, unsigned int));
				continue;
			default:
				//*str++ = '%';
				if (*fmt)
				{//*str++ = *fmt;
				}
				else
					--fmt;
				continue;
			}
			if (qualifier == 'l')
				copynum(str, va_arg(args, unsigned long));
		}

		return str;
	}

	std::string format(int type, const char* fmt, ...)
	{
		va_list args;
		int i;
		std::string s;
		char buf[10240] = { 0 };
		va_start(args, fmt);
		if (type == 1)
			s = vsprintf(buf, fmt, args);
		else
			s = vsp(fmt, args);
		va_end(args);
		return s;
	}
#endif
	//-----------------------------------------------------------------------------
	//鼠标事件

#ifndef LUNA_MOUSE_TIME
#define LUNA_MOUSE_TIME 10
#endif

		//-----------------------------------------------------------------------------
		/*
		事件处理
		*/
		//-----------------------------------------------------------------------------
	typedef std::function<void(unsigned int ptState, int x, int y)> onMouse_FUNC;
	typedef std::function<void(const char*, int, bool, int num)>  onDrag_FUNC;
	typedef std::function<void(int key, int scancode, int action, int mods)> onKey_FUNC;
	typedef std::function<void(int mods)> onKeyDown_FUNC;
	typedef std::function<void(void*)> onChange_FUNC;

	typedef std::vector<onMouse_FUNC> v_onMouse_FUNC;
	typedef std::vector<onDrag_FUNC> v_onDrag_FUNC;
	typedef std::vector<onKey_FUNC> v_onKey_FUNC;
	typedef std::vector<onKeyDown_FUNC> v_onKeyDown_FUNC;
	typedef std::vector<onChange_FUNC> v_onChange_FUNC;

	template <typename _Pr>
	class EventList
	{
	public:
		typedef std::vector<_Pr> v_FUNC;
		typedef struct Eventmap
		{
			void* data;
			_Pr func;
			Eventmap(void* d, _Pr f) :data(d), func(f) {}
		}Eventmap;

		std::multimap<unsigned int, Eventmap > _event_func;     //消息表
		Lock _lock;
	public:
		void pushFunc(void* data, unsigned int msg, _Pr func)
		{
			LOCK(_lock);
			_event_func.insert(std::pair<unsigned int, Eventmap>(msg, Eventmap{ data, func }));
		}
		void popFunc(void* data, unsigned int msg)
		{
			LOCK(_lock);
			auto range = _event_func.equal_range(msg);   //查找消息回调函数multimap
			for (auto mit = range.first; mit != range.second;)
			{
				if (data == mit->second.data)
				{
					_event_func.erase(mit++);
				}
				else
				{
					++mit;
				}
			}
		}
		size_t getFunc(unsigned int msg, v_FUNC* lt, std::vector<void*>* d = 0)
		{
			LOCK(_lock);
			lt->clear();
			auto range = _event_func.equal_range(msg);   //查找消息回调函数
			for (auto multi_it = range.first; multi_it != range.second; ++multi_it)
			{
				lt->push_back(multi_it->second.func);
				if (d)
				{
					d->push_back(multi_it->second.data);
				}
			}
			return lt->size();
		}
		size_t size()
		{
			return _event_func.size();
		}
	public:
		EventList() {}
		virtual ~EventList() {}
	private:

	};
#ifndef nLUNA_MK_UP

	class GuiEvent : public Res
	{
	public:
		// 判断鼠标坐标是否包含在元素内
		std::function<bool(const glm::vec2& pt)> is_contains;
		// 获取元素坐标，用来计算保存鼠标按下时，元素的坐标
		std::function<glm::vec3()> get_pos;
		// 如果需要把事件传递给父级则设置此回调函数
		std::function<GuiEvent* ()> get_parent_event;

		// 输入法
		IMEDelegate* _onIme = 0;
		void* _userData = 0;		// 用户存放标识符或指针，非必要
#ifdef _WIN32
		GuiDropTarget* _drop = nullptr;
#endif
#if 1
		unsigned int tem[4] = { 0 };
#endif
		glm::vec2		_firstpos, _lastpos, _mousepos;
		EventList<onMouse_FUNC> _mouse_event;
		//EventList<onDrag_FUNC> _drag_event;
		EventList<onKey_FUNC> _key_event;
		EventList<onChange_FUNC> _change_event;
		EventList<onKeyDown_FUNC> _keyDown_event;
	private:
		unsigned int	_nMouseState = LUNA_MK_UP;			//鼠标状态
		unsigned int	_wKeyState = 0;						//键盘状态		
		bool			_keys[1024];						//按键状态

		int _nWheel = 0;						//滚轮值
		int _maxW = 100, _minW = 0, _step = 1;	//滚轮最大最小、滚动量


		int inc = 0;
		unsigned int m_style = 0;

		Timer_t _t, _over_js;
		bool			_isDblclk = false;
		bool            _isDragTXT = false;       //是否拖动
		int             _isDragEnd = 0;
		//如果要把事件传给父节点则增加一个
		std::set<unsigned int> _passon;
		int _over_ms = 100;


		LockS _lock;
		// 按下坐标，拖动最后坐标
		glm::ivec2 _pos_down;
		glm::ivec2 _pos_end;
		glm::ivec4 _rect;
	public:
		virtual ~GuiEvent()
		{
			IMEDelegate::destroy(_onIme);
#ifdef _WIN32
			Res::destroy(_drop);
			_drop = 0;
#endif
			_onIme = 0;
		}
		static GuiEvent* create(void* userData)//监听对象
		{
			return new GuiEvent(userData);
		}
	protected:
		GuiEvent(void* userData) :_userData(userData)
		{
			initfunc();
		}

		void initfunc()
		{
			setMouseEvent(_userData, LUNA_MK_DOWN, HL_CALLBACK_3(&GuiEvent::onMouseDown, this));
			setMouseEvent(_userData, LUNA_MK_UP, HL_CALLBACK_3(&GuiEvent::onMouseUp, this));
			setMouseEvent(_userData, LUNA_MK_MOVE, HL_CALLBACK_3(&GuiEvent::onMouseMove, this));
			setMouseEvent(_userData, LUNA_MK_HOVER, HL_CALLBACK_3(&GuiEvent::onMouseHover, this));
			setMouseEvent(_userData, LUNA_MK_LEAVE, HL_CALLBACK_3(&GuiEvent::onMouseLeave, this));
			setMouseEvent(_userData, LUNA_MK_SCROLL, HL_CALLBACK_3(&GuiEvent::onMouseScroll, this));

			setMouseEvent(_userData, LUNA_MK_RDOWN, HL_CALLBACK_3(&GuiEvent::onMouseRdown, this));
			setMouseEvent(_userData, LUNA_MK_RBUTTON, HL_CALLBACK_3(&GuiEvent::onMouseRbutton, this));
			//默认继承滚轮事件
			pushPasson(LUNA_MK_SCROLL);
			pushPasson(LUNA_MK_MOVE);
		}
#ifdef _WIN32
		void setImePos(HWND hWnd, POINT* outpos)
		{

		}
#endif
	public:
		void setDblclk(bool isdblclk /*= true*/)
		{
#ifdef _WIN32
			if (isdblclk)
			{
				m_style |= CS_DBLCLKS;
			}
			else
			{
				m_style &= ~CS_DBLCLKS;
			}
#endif
		}
		unsigned int getMouseState()
		{
			return _nMouseState;
		}

		unsigned int getKeyState()
		{
			_wKeyState = 0;

#ifdef _WIN32
			if (::GetKeyState(VK_CONTROL) < 0) _wKeyState |= LUNA_MK_CONTROL;
			if (::GetKeyState(VK_RBUTTON) < 0) _wKeyState |= LUNA_MK_RBUTTON;
			if (::GetKeyState(VK_LBUTTON) < 0) _wKeyState |= LUNA_MK_LBUTTON;
			if (::GetKeyState(VK_MBUTTON) < 0) _wKeyState |= LUNA_MK_MBUTTON;
			if (::GetKeyState(VK_SHIFT) < 0) _wKeyState |= LUNA_MK_SHIFT;
			if (::GetKeyState(VK_MENU) < 0) _wKeyState |= LUNA_MK_ALT;
#endif
			return _wKeyState;
		}

		bool emptyFunc()
		{
			return !(_mouse_event.size() ||
				//_drag_event.size() ||
				_key_event.size() ||
				_change_event.size());
		}
	public:
		// 事件传给父节点则增加一个
		void pushPasson(unsigned int msg)
		{
			_passon.insert(msg);
		}
		void popPasson(unsigned int msg)
		{
			_passon.erase(msg);
		}
		bool isPasson(unsigned int m)
		{
			return (_passon.find(m) != _passon.end());
		}
	public:
		void set_on_dblclk(onMouse_FUNC pefun)                            //设置双击事件
		{
			setMouseEvent(this, LUNA_MK_DBLCLK, pefun);
		}
		void set_on_click(onMouse_FUNC pefun)                            //设置单击事件
		{
			setMouseEvent(this, LUNA_MK_LBUTTON, pefun);
		}
		void set_on_rclick(onMouse_FUNC pefun)                            //设置右击事件
		{
			setMouseEvent(this, LUNA_MK_RBUTTON, pefun);
		}
		void set_on_mouseup(onMouse_FUNC pefun)
		{
			setMouseEvent(this, LUNA_MK_UP, pefun);
		}
		void set_on_mousedown(onMouse_FUNC pefun)
		{
			setMouseEvent(this, LUNA_MK_DOWN, pefun);
		}
		void set_on_scroll(onMouse_FUNC pefun)
		{
			setMouseEvent(this, LUNA_MK_SCROLL, pefun);
		}
		// 鼠标拖动事件，比如文本选择、显示选择框、开始拖动元素
		void set_on_drag(onMouse_FUNC pefun)
		{
			setMouseEvent(this, LUNA_MK_DRAG, pefun);
		}
		void set_on_move(onMouse_FUNC pefun)
		{
			setMouseEvent(this, LUNA_MK_MOVE, pefun);
		}
		// 鼠标进入
		void set_on_hover(onMouse_FUNC pefun)
		{
			setMouseEvent(this, LUNA_MK_HOVER, pefun);
		}
		void set_on_leave(onMouse_FUNC pefun)
		{
			setMouseEvent(this, LUNA_MK_LEAVE, pefun);
		}
		// 鼠标悬停
		void set_on_mouseover(int ms, onMouse_FUNC pefun)
		{
			_over_ms = std::max(std::min(ms, 6 * 1000), 50);
			setMouseEvent(this, LUNA_MK_OVER, pefun);
		}
		void set_on_key_down(unsigned int modes, onKeyDown_FUNC pefun)
		{
			setOnKeyDown(modes, pefun);
		}
		void set_on_change(onChange_FUNC pefun)
		{
			_change_event.pushFunc(this, 0, pefun);
		}
		void set_on_key_press(onKey_FUNC pefun)
		{
			_key_event.pushFunc(this, 0, pefun);
		}
		//设置接收拖放
		void set_on_drop(std::function<void(const char*, size_t, bool, size_t num)> pOnChar, std::function<bool(int x, int y)> pOnOver = nullptr)
		{
			setDropTarget(pOnChar, pOnOver);
		}
		template<class T>
		void set_mouses(void* data, T& s)
		{
			for (auto& [k, v] : s)
			{
				setMouseEvent(data, k, v);
			}
		}
	public:
		void setMouseEvent(void* data, unsigned int mode, onMouse_FUNC pefun)
		{
			if (mode & LUNA_MK_DBLCLK)
			{
				_isDblclk = true;
				_mouse_event.pushFunc(data, mode, HL_CALLBACK_3(&GuiEvent::onMouseDblclk, this));
			}
			_mouse_event.pushFunc(data, mode, pefun);
		}
		void setOnChange(void* data, onChange_FUNC pefun)
		{
			_change_event.pushFunc(data, 0, pefun);
		}
		void setOnKeyPress(void* data, onKey_FUNC pefun)
		{
			_key_event.pushFunc(data, 0, pefun);
		}
		void setOnKeyDown(unsigned int mode, onKeyDown_FUNC pefun)
		{
			_keyDown_event.pushFunc(this, mode, pefun);
		}
		void setOnIme(std::function<void(const char*, size_t, std::wstring* utext)> pOnChar, std::function<void(int* x, int* y)> onSetImePos)
		{
			if (!_onIme)
				_onIme = IMEDelegate::create();
			_onIme->pOnChar = pOnChar;
			_onIme->onSetIme = onSetImePos;
		}
		void setDropTarget(std::function<void(const char*, size_t, bool, size_t num)> pOnChar,
			std::function<bool(int x, int y)> pOnOver)
		{
#ifdef _WIN32
			if (!_drop)
			{
				_drop = GuiDropTarget::create();
			}
			_drop->pOnChar = pOnChar;
			_drop->pOnOver = pOnOver;
#endif
		}
	public:
		glm::vec2 getFirstPos()
		{
			return _firstpos;
		}
		glm::vec2 getMousePos()
		{
			return _mousepos;
		}
	public:
		void enter_over()
		{
			//std::async([]() {
			//	while (true)
			//	{

			//	hz::sleep(1);
			//	}
			//	});
		}
		void call_over(int x, int y)
		{
			int es = _t.elapsed();
			if (es < _over_ms)
			{
				exeMouse(LUNA_MK_OVER, x, y);
			}
		}

		void exeMouse(unsigned int ptState, int x, int y)
		{
			v_onMouse_FUNC lt;
			std::vector<void*> dv;
			_mousepos.x = x;
			_mousepos.y = y;
			if (_mouse_event.getFunc(ptState, &lt, &dv) > 0)
			{
				if ((ptState & LUNA_MK_LBUTTON || ptState & LUNA_MK_RBUTTON) && dv.size())
				{
					if (is_contains)
						if (!(is_contains({ x,y })))
							return;
				}
				//bool isud = (!_userData);
				int i = 0;
				for (auto& it : lt)
				{
					if (ptState & LUNA_MK_SCROLL)
					{
						x = _nWheel;
					}
					//if (isud)
					//	_userData = dv[i++];
					it(ptState, x, y);
				}
				//if (isud)
				{
					//	_userData = 0;
				}
			}
			if (isPasson(ptState))
			{
				auto pev = get_parent_event();
				if (pev)
				{
					pev->exeMouse(ptState, x, y);
				}
			}
		}
		void onMouse(unsigned int ptState, int x, int y)
		{
			unsigned int pts = ptState;
			if (pts & LUNA_MK_UP)
			{
				exeMouse(LUNA_MK_UP, x, y);
				//onMouseUp(ptState, x, y);
				return;
			}/*
			if (pts&LUNA_MK_HOVER)
			{
				onMouseHover(pts,x,y);
			}
			if (pts&LUNA_MK_LEAVE)
			{
				onMouseLeave(pts, x, y);
			}*/
			{
				exeMouse(pts, x, y);
			}
		}
		//void onDrag0(const char* text, int len, bool isFile, int num)
		//{
		//	v_onDrag_FUNC lt;
		//	if (_drag_event.getFunc(0, &lt) > 0)
		//	{
		//		for (auto& it : lt)
		//		{
		//			it(text, len, isFile, num);
		//		}
		//	}
		//}
		void onKey(int key, int scancode, int action, int mods)
		{
			v_onKey_FUNC lt;
			_keys[key] = action;
			if (_key_event.getFunc(0, &lt) > 0)
			{
				for (auto& it : lt)
				{
					it(key, scancode, action, mods);
				}
			}
			if (action)
			{
				onKeyDown(key, mods);
			}
		}
		void onKeyDown(int key, int mods)
		{
			v_onKeyDown_FUNC lt;

			if (_keyDown_event.getFunc(key, &lt) > 0)
			{
				for (auto& it : lt)
				{
					it(mods);
				}
			}
		}


		void onChange(void* data)
		{
			v_onChange_FUNC lt;
			if (_change_event.getFunc(0, &lt) > 0)
			{
				for (auto& it : lt)
				{
					it(data);
				}
			}
		}
	public:

		void onMouseScroll(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;
			//if (!((Widget*)m_userData)->getSelectNode())
			//	return;
			//  case WM_MOUSEWHEEL:    
			if (y < 0)
			{
				//SB_LINEUP
				_nWheel += _step;
				if (_nWheel > _maxW)
				{
					_nWheel = _maxW;
				}
				//// _nMouseState;

			}
			else  if (y > 0)
			{
				//SB_LINEDOWN
				_nWheel -= _step;
				if (_nWheel < _minW)
				{
					_nWheel = _minW;
				}
				//// _nMouseState;
			}
		}
		void set_drag_begin(int x, int y)
		{
			if (get_pos)
			{
				glm::vec3 pos = get_pos();
				LOCK_W(_lock);
				_pos_down = _firstpos = { x - pos.x, y - pos.y };
			}
		}
		void set_drag_end(int x, int y)
		{
			if (get_pos)
			{
				glm::vec3 pos = get_pos();
				LOCK_W(_lock);
				_pos_end = { x - pos.x, y - pos.y };
				re_rect();
			}
		}
		//-----------------------------------------------------------------------------
		void onMouseMove(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;
			_nMouseState |= LUNA_MK_MOVE;
			// _nMouseState;

			if (_nMouseState & LUNA_MK_DOWN)
			{
				glm::vec2 pos = { x - _firstpos.x , y - _firstpos.y };
				set_drag_end(x, y);
				exeMouse(LUNA_MK_DRAG, pos.x, pos.y); //执行拖动处理
			}
#ifdef l_WIN32
			if (((HLNode*)m_userData)->getSelectNode())
			{
				if (e.Type == WM_MOUSEMOVE)
				{
					TRACKMOUSEEVENT MouseEvent = { sizeof(TRACKMOUSEEVENT) };
					MouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
					MouseEvent.hwndTrack = e.hWnd;
					MouseEvent.dwHoverTime = LUNA_MOUSE_TIME;
					TrackMouseEvent(&MouseEvent);
				}
				return;
			}
			//else if (!(_nMouseState & LUNA_MK_DOWN))
			{
				onMouseLeave(e);
			}
#endif
		}
		void onMouseLeave(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;
			/* if (_nMouseState&LUNA_MK_LEAVE && !e.lParam)
				return;
			if (!(_nMouseState &LUNA_MK_MOVE) && !e.lParam)
			return;*/
			if (!(_nMouseState & LUNA_MK_HOVER))
			{
				return;
			}
			_nMouseState &= ~LUNA_MK_HOVER;
			/*if ((_nMouseState&LUNA_MK_DOWN && (e.pSender == (PTR)Widget::getFocusedWidget())))
			{
				return;
			}*/
			_nMouseState &= ~LUNA_MK_MOVE;
			_nMouseState |= LUNA_MK_LEAVE;
			// _nMouseState;
			//执行事件

			_nMouseState &= ~LUNA_MK_DOWN;
			_nMouseState &= ~LUNA_MK_RDOWN;
			_nMouseState |= LUNA_MK_UP;
			_over_js.reset();
		}
		void onMouseHover(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;
			_over_js.reset();
			//			if (!(_nMouseState&LUNA_MK_MOVE) && !e.lParam)
			//				return;
			//			if (e.pSender && ((HLNode*)e.pSender)->getSelectNode())
			{
				_nMouseState &= ~LUNA_MK_LEAVE;
				_nMouseState |= LUNA_MK_HOVER;
				// _nMouseState;
/*
				if (m.pMouseHover)
					m.pMouseHover(e);
				if (pMouseHover)
					pMouseHover(e);
				if (m.pMouse)		//执行优先鼠标事件
					m.pMouse(e);
				if (pMouse)		    //执行鼠标事件
					pMouse(e);*/
			}
		}
		void onMouseDown(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;
			if ((_nMouseState & LUNA_MK_HOVER))
			{
				_nMouseState |= LUNA_MK_DOWN;
				_nMouseState &= ~LUNA_MK_UP;
				//执行事件
				set_drag_begin(x, y);
#ifdef _WIN32
				if (_isDblclk)
				{
					int dct = GetDoubleClickTime();//获取系统双击间隔时间
					int es = _t.elapsed();
					if (es < dct)
					{
						exeMouse(LUNA_MK_DBLCLK, x, y);
						_nMouseState &= ~LUNA_MK_DOWN;
					}
				}
#endif
			}
		}
		void onMouseUp(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;

			_nMouseState |= LUNA_MK_UP;
			set_drag_end(x, y);
			if (_nMouseState & LUNA_MK_DOWN)
			{
#ifdef _DEBUG
				//  cout << "EDIT：" << ((Widget*)e.pSender)->getName() << "释放" << endl;
#endif
				// _nMouseState;

				if (_nMouseState & LUNA_MK_HOVER /*&& ((Node*)_userData)->getHover()*/)
				{
					_nMouseState |= LUNA_MK_CLICK;
					// _nMouseState;
					//执行鼠标单击左键事件
					if (_isDblclk)
						_t.reset();
					exeMouse(LUNA_MK_LBUTTON, x, y);
					_nMouseState &= ~LUNA_MK_CLICK;
					//Widget *pwidget = ((Widget*)e.pSender);

				}

			}
			_nMouseState &= ~LUNA_MK_DOWN;

		}
		void onMouseDblclk(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;
			if (_nMouseState & LUNA_MK_HOVER)
			{
				_nMouseState |= LUNA_MK_DBLCLK;
				// _nMouseState;

				_nMouseState &= ~LUNA_MK_DBLCLK;
			}
		}
		void onMouseRdown(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;
			if (_nMouseState & LUNA_MK_HOVER)
			{
				_nMouseState |= LUNA_MK_RDOWN;
				// _nMouseState;
				exeMouse(LUNA_MK_RBUTTON, x, y);
			}
		}
		void onMouseRbutton(unsigned int ptState, int x, int y)
		{
			if (_nMouseState & LUNA_MK_DISABLE)//禁用鼠标操作
				return;
			if (_nMouseState & LUNA_MK_RDOWN)
			{
				_nMouseState |= LUNA_MK_RBUTTON;

				_nMouseState &= ~LUNA_MK_RDOWN;
				// _nMouseState;
				if (_nMouseState & LUNA_MK_HOVER)
				{
					//执行右键单击
				}

				_nMouseState &= ~LUNA_MK_RBUTTON;
			}
		}

		//-----------------------------------------------------------------------------

		int getScroll()
		{
			return _nWheel;
		}
		void setScroll(int n)
		{
			if (n > _maxW)
			{
				n = _maxW;
			}
			if (n < _minW)
			{
				n = _minW;
			}
			_nWheel = n;
		}
		void setRange(int max, int min, int step)
		{
			_maxW = max;
			_minW = min;
			_step = (step > 0) ? step : 1;
		}
	public:

		// 获取选中的区域 
		glm::ivec4 get_select_rect()
		{
			LOCK_R(_lock);
			return _rect;
		}
	private:

		glm::ivec4 re_rect()
		{
			glm::ivec4& ret = _rect;
			ret.x = std::min(_pos_down.x, _pos_end.x);
			ret.y = std::min(_pos_down.y, _pos_end.y);
			ret.z = std::max(_pos_down.x, _pos_end.x);
			ret.w = std::max(_pos_down.y, _pos_end.y);
			return ret;
		}

	public:

	};

	//-----------------------------------------------------------------------------
	class IMEDispatcher
	{
	public:
		//friend EventDispatcher;
		IMEDispatcher() {}
		~IMEDispatcher() {}
		virtual void dispatchInsertText(const char* text, size_t len, std::wstring* utext)
		{
			LOCK(_lock_ime);
			if (_currenDelegate)
				_currenDelegate->InsertText(text, len, utext);
		}
		virtual void dispatchKey(int key, int scancode, int action, int mods)
		{
			LOCK(_lock_ime);
			if (_curr_event)
			{
				_curr_event->onKey(key, scancode, action, mods);
			}
			// 			if (_currenDelegate)
			// 				_currenDelegate->keyPress(text, len, utext);
		}
		virtual void activeDelegate(GuiEvent* e)//IMEDelegate* delegate)
		{
			LOCK(_lock_ime);
			if (e != _curr_event)
			{
				removeDelegate();
				_curr_event = e;
				if (e->_onIme)
				{
					_currenDelegate = e->_onIme;
					_currenDelegate->ImeEnter();
					if (_currenDelegate->onSetIme)
					{
						int x = 0, y = 0;
						_currenDelegate->onSetIme(&x, &y);
						_imepoint.x = (float)x;
						_imepoint.y = (float)y;
					}
				}
			}
		}
		virtual void removeDelegate()
		{
			LOCK(_lock_ime);
			if (_currenDelegate)
			{
				_currenDelegate->ImeExit();
				_currenDelegate = nullptr;
				_curr_event = nullptr;
			}
		}
#ifdef _WIN32

		// 获取窗口标题
		static std::string get_window_title(HWND hwnd)
		{
			char szWindowTitle[10240] = { 0 };
			::GetWindowTextA(hwnd, szWindowTitle, sizeof(szWindowTitle));
			return szWindowTitle;
		}


		typedef struct
		{
			std::vector<HWND>* hWnds;
			DWORD dwPid;
		}WNDINFO;

		static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
		{
			WNDINFO* pInfo = (WNDINFO*)lParam;
			DWORD dwProcessId = 0;
			GetWindowThreadProcessId(hWnd, &dwProcessId);

			if (dwProcessId == pInfo->dwPid)
			{
				pInfo->hWnds->push_back(hWnd);
				//return FALSE;
			}
			return TRUE;
		}

		static size_t GetHwndByProcessId(DWORD dwProcessId, std::vector<HWND>& ws)
		{
			WNDINFO info = { 0 };
			info.hWnds = &ws;
			info.dwPid = dwProcessId;
			EnumWindows(EnumWindowsProc, (LPARAM)&info);
			return ws.size();
		}

		void hide_cw()
		{
			static std::once_flag flag;
			std::call_once(flag, [&]() {
				std::thread ta([&]() {
					while (true) {
						hide_ime_cw();
						Event::sleep(1);
					}
					});
				ta.detach();
				});
		}

		static void hide_ime_cw()
		{
			DWORD processId = GetCurrentProcessId();//当前进程id
			std::vector<HWND> ws;
			GetHwndByProcessId(processId, ws);

			//printf("windows:%d\n", (int)ws.size());
			int ic = 0;
			for (auto it : ws)
			{
				auto str = get_window_title(it);
				RECT lpRect = { 0 };
				GetWindowRect(it, &lpRect);
				int w = lpRect.right - lpRect.left, h = lpRect.bottom - lpRect.top;
				if (h == 18)
				{
					ShowWindow(it, SW_HIDE);
					//printf("title<%02d><%08p><%d,%d>:%s\n", ic++, it, w, h, str.c_str());
				}
			}
		}

		//输入法处理
		int on_inputLagChange(HWND hWnd, int type, WPARAM wParam, LPARAM lParam, POINT ptMouse)
		{
			if (_isDisable)
				return 0;
			return (type == WM_INPUTLANGCHANGE) ? 1 : 0;
		}
		void get_ime_pos()
		{
			LOCK(_lock_ime);
			if (_currenDelegate && _currenDelegate->onSetIme)
			{
				int x = 0, y = 0;
				_currenDelegate->onSetIme(&x, &y);
				_imepoint.x = (float)x;
				_imepoint.y = (float)y;
			}
		}

		int on_ime_StartComposition(HWND hWnd, int type, WPARAM wParam, LPARAM lParam, POINT ptMouse)
		{
			if (_isDisable)
				return 0;
			get_ime_pos();
			HIMC hIMC = ::ImmGetContext(hWnd);
			COMPOSITIONFORM cf;
			cf.dwStyle = CFS_POINT | CFS_RECT;
			RECT rc = { 0 };
			if (_imepoint.x > 0 || _imepoint.y > 0)
			{
				rc.left = (long)_imepoint.x;
				rc.top = (long)_imepoint.y;
			}
			cf.rcArea.top = cf.rcArea.left = cf.rcArea.right = cf.rcArea.bottom = 0;
			cf.ptCurrentPos.x = rc.left;//输入法坐标
			cf.ptCurrentPos.y = rc.top;

			::ImmSetCompositionWindow(hIMC, &cf);
			//DropCaret();可在此处隐藏光标，在OnImeEndComposition消息处理中再显示光标
			::ImmReleaseContext(hWnd, hIMC);
			//hide_cw();
			_ime_state = WM_IME_STARTCOMPOSITION;
			//printf("ime_Start_pos:%d,%d\n", rc.left, rc.top);
			return 1;
		}

		int on_ime_EndComposition(HWND hWnd, int type, WPARAM wParam, LPARAM lParam, POINT ptMouse)
		{
			if (_isDisable)
				return 0;

			LOCK(_lock_ime);
			get_ime_pos();
			_ime_state = WM_IME_ENDCOMPOSITION;
			return 1;
		}
		int on_ime_Composition(HWND hWnd, int type, WPARAM wParam, LPARAM lParam, POINT ptMouse)
		{
			if (_isDisable)
				return 0;

			if (lParam & GCS_RESULTSTR)
			{
				//get result text
				HIMC hIMC = ::ImmGetContext(hWnd);
				if (hIMC)
				{
					//static wchar_t *wcs = 0;
					static std::wstring wcs;
					LONG bytes = ::ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, 0, 0) + sizeof(wchar_t);
					wcs.clear();
					wcs.resize(bytes);
					bytes = ::ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, (LPVOID)wcs.data(), bytes);
					// Set new position after converted
					POINT pos = ptMouse;
					COMPOSITIONFORM cf;
					cf.dwStyle = CFS_POINT | CFS_RECT;
					cf.ptCurrentPos.x = _imepoint.x;// pos.x;
					cf.ptCurrentPos.y = _imepoint.y; //pos.y;
					cf.rcArea.top = cf.rcArea.left = cf.rcArea.right = cf.rcArea.bottom = 0;
					::ImmSetCompositionWindow(hIMC, &cf);
					::ImmReleaseContext(hWnd, hIMC);

					//printf("ime_Com_pos:%d,%d\n", pos.x, pos.y);
				}
			}
			_ime_state = WM_IME_COMPOSITION;
			return 1;
		}
#endif
	private:
		GuiEvent* _curr_event = 0;
		IMEDelegate* _currenDelegate = 0;
		Lock _lock_ime;
		glm::vec2        _imepoint = { -1, -1 };
		unsigned int     _ime_state = 0;
		bool             _isDisable = false;
	};
#endif

}
#endif//END __event__h__
