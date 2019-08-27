#ifndef _RecycleBin_h_
#define _RecycleBin_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <set>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <random>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>  
#include <thread>  
#include <queue>  
#include <functional>
#include <atomic>

#ifndef LOCK_R
#include <shared_mutex>
typedef std::shared_mutex LockS;
#define LOCK_R(ls) std::shared_lock<LockS> __locker_(ls)
#define LOCK_W(ls) std::unique_lock<LockS> __locker_(ls)
#endif // !LOCK_R
namespace hz {

	//------回收站-----------------------------------------------------------------------

	enum class RcType
	{
		RT_PUSH = 0,		 //加入回收
		RT_POP,				 //获取一个
		RT_CLERA,			 //清空回收站
	};


	template <typename T>
	class RecycleBinNew
	{
	private:
		std::set<T*>	_recycler;
		LockS _lock;
	public:
		//template <typename T1>
		static T* makeRC(T* d, RcType rt)
		{
			//字符串回收站
			static RecycleBinNew<T>* rc_str = nullptr;
			static std::once_flag flag;
			if (nullptr == rc_str)
			{
				//LOCK(Singleton<T>::lock);
				std::call_once(flag, [&]() { rc_str = new RecycleBinNew<T>(); });
			}
			T* ret = 0;
			switch (rt)
			{
			case RcType::RT_POP:
				ret = rc_str->getItem(d);
				break;
			case RcType::RT_CLERA:
				rc_str->clear();
				break;
			case RcType::RT_PUSH:
			default:
				if (d)
				{
					//delete d;
					rc_str->push(d);
				}
				break;
			}
			return ret;
		}
	public:
		T* getItem(const T* t = 0)
		{
			T* p = 0;
			if (_recycler.size())
			{
				LOCK_W(_lock);

				if (_recycler.size())
				{
					p = *_recycler.begin();
					_recycler.erase(p);
					if (t)
					{
						*p = *t;
					}
					return p;
				}
			}
			{
				p = t ? new T(*t) : new T();
			}
			return p;
		}
		void push(T* item)
		{
			if (item)
			{
				LOCK_W(_lock);
				_recycler.insert(item);
			}
		}
		void clear()
		{
			LOCK_W(_lock);
			for (; _recycler.size();)
			{
				auto it = *_recycler.begin();
				delete it;
				_recycler.erase(it);
			}
		}
	public:
		RecycleBinNew()
		{
		}

		~RecycleBinNew()
		{
			clear();
		}

	};


}/*!hz*/
#endif /*!_RecycleBin_h_*/