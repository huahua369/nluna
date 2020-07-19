#ifndef __Singleton__
#define __Singleton__
#include <cstdlib>
#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <condition_variable> 
#include <memory>
#include <atomic>
#include <type_traits>
#include <functional>
#include <string>
#include <list>
#include <set>
#ifndef _WIN32
#include <sys/prctl.h>
#endif // _WIN32

namespace hz {
	/// For Example:
	/// 
	/// class A : public Singleton<A>
	/// {...}
	/// 

		//¹ÒÆðsºÁÃë
	static void sleep(int s)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(s));
	}

	template <typename T>
	class Singleton
	{
	private:
		static T* me;
		static std::once_flag flag;
	public:
		inline static T* s();
		inline static T* getInstance()
		{
			return s();
		}
		static void destroy();
		virtual ~Singleton() {}
	};

	template <typename T>
	T* Singleton<T>::me = nullptr;
	template <typename T>
	std::once_flag Singleton<T>::flag;

	template <typename T>
	T* Singleton<T>::s()
	{
		if (nullptr == me)
		{
			//LOCK(Singleton<T>::lock);
			std::call_once(flag, [&]() {
				me = new T();
				//std::atexit(destroy);
				});
		}
		return me;
	}

	template <typename T>
	void Singleton<T>::destroy()
	{
		if (nullptr != me)
		{
			delete me;
			me = nullptr;
		}
	}


}//!hz
#endif //__Singleton__