#pragma once
#include <set>
#include <map>
#include <vector>
namespace hz
{
	template <typename T>
	class recycle_bin
	{
	public:
		typedef struct {
			T* p = 0;
			int n = 0;
		}value_t;
	private:
		// 分配中的，回收中的
		std::map<T*, int> _alloc;
		std::multimap<int, T*> _rcb;
		LockS _lock;
	public:
		recycle_bin()
		{
		}

		~recycle_bin()
		{
			clear(true);
		}

		T* get(int n = 1)
		{
			T* p = nullptr;
			if (n > 0)
			{
				if (_rcb.size())
				{
					LOCK_W(_lock);
					auto it = _rcb.find(n);
					if (it != _rcb.end())
					{
						p = it->second;
						_rcb.erase(it);
					}
				}
				if (!p)
				{
					p = new T[n];
					if (p)
					{
						LOCK_W(_lock);
						_alloc.insert(std::pair<T*, int>(p, n));
					}
				}
			}
			return p;
		}
		void gc(T* p)
		{
			if (p)
			{
				LOCK_W(_lock);
				auto it = _alloc.find(p);
				if (it != _alloc.end())
				{
					_rcb.insert(std::pair<int, T*>(it->second, p));
				}
			}
		}
		void clear(bool isfree)
		{
			LOCK_W(_lock);
			if (isfree)
			{
				for (auto& [k, v] : _alloc)
				{
					if (k)
					{
						delete[] k;
					}
				}
				_rcb.clear();
				_alloc.clear();
			}
			else {
				for (auto& [k, v] : _alloc)
				{
					_rcb.insert(std::pair<int, T*>(v, k));
				}
			}
		}
	private:

	};
}
//!hz
