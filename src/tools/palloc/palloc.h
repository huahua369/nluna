#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#ifndef msize
#ifdef _WIN32
#define msize _msize
#else
#define msize malloc_usable_size
#endif // _WIN32
#endif // !msize
	typedef struct pool_t pool_t;
	typedef struct pool_cleanup_t pool_cleanup_t;
	//struct pool_t;
	//struct pool_cleanup_t;
	typedef struct mem_dev_info_s
	{
		uint32_t  pagesize;
		uint32_t  pagesize_shift;
		uint32_t  cacheline_size;
		uint32_t  ncpu;
		uint32_t  allocation_granularity;
	}mem_dev_info_t;

	// 自定义分配器
	typedef struct alloc_info_s {
		void* ctx;
		int64_t count;
		int64_t alloc_size;
		mem_dev_info_t mdi;
		void* (*palloc)(void* ctx, size_t size);
		void (*pfree)(void* ctx, void* p);
		// 可选
		void* (*prealloc)(void* ctx, void* p, size_t size);

	}alloc_info_t;

	typedef struct palloc_s palloc_t;
	struct palloc_s {
		alloc_info_t _dev;
		pool_t* _user_p;

		pool_t* (*create_pool)(palloc_t* ctx, size_t size);
		void (*destroy_pool)(pool_t* pool);
		void (*reset_pool)(pool_t* pool);

		void* (*palloc)(pool_t* pool, size_t size);
		void* (*pnalloc)(pool_t* pool, size_t size);
		void* (*pcalloc)(pool_t* pool, size_t count, size_t size);
		void* (*prealloc)(pool_t* pool, void* ptr, size_t size);
		void* (*pmemalign)(pool_t* pool, size_t size, size_t alignment);
		int (*pfree)(pool_t* pool, void* p);

		pool_cleanup_t* (*pool_cleanup_add)(pool_t* p, size_t size);
		void (*pool_run_cleanup_file)(pool_t* p, uint64_t fd);
		void (*pool_cleanup_file)(void* data);
		void (*pool_delete_file)(void* data);
	};
	int  palloc_init(palloc_t* p, alloc_info_t* a);
	void palloc_close(palloc_t* p);
	void get_mem_info(mem_dev_info_t*);

#ifdef __cplusplus
}//!e"c"
#include <unordered_map>
#include <utility>
#include <mutex>
#include <atomic>

class spin_mutex {
	std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
	spin_mutex() = default;
	spin_mutex(const spin_mutex&) = delete;
	spin_mutex& operator= (const spin_mutex&) = delete;
	void lock() {
		while (flag.test_and_set(std::memory_order_acquire))
			;
	}
	void unlock() {
		flag.clear(std::memory_order_release);
	}
};
template<class _Mtx>
class auto_lock
{
public:
	auto_lock(_Mtx* k) :lk(k)
	{
		if (lk)
		{
			lk->lock();
		}
	}

	~auto_lock()
	{
		if (lk)
		{
			lk->unlock();
		}
	}

private:
	_Mtx* lk = nullptr;
};

class auto_ptr
{
public:
	auto_ptr()
	{
	}

	~auto_ptr()
	{
	}
public:
	template<class T, bool isinit = true, typename... Args>
	static T* new_ptr(alloc_info_t* alloc, int n, Args... args)
	{
		assert(alloc);
		T* p = nullptr;
		if (alloc)
		{
			p = (T*)alloc->palloc(alloc->ctx, sizeof(T) * n);
			if (isinit)
			{
				auto p1 = p;
				for (int i = 0; i < n; i++)
					init_ptr(p1++, args...);
			}
		}
		return p;
	}
	template<class T, typename... Args>
	static T* init_ptr(T* p, Args... args)
	{
		if (p)
		{
			new(p)T(args...);
		}
		return p;
	}

	template<class T>
	static void free_ptr(T* p, alloc_info_t* alloc)
	{
		assert(alloc);
		if (p)
		{
			p->~T();
			if (alloc)
				alloc->pfree(alloc->ctx, p);
		}
	}
	template<class T>
	static void free_ptrs(T* p, int n, alloc_info_t* alloc)
	{
		assert(alloc);
		if (p)
		{
			auto t = p;
			for (size_t i = 0; i < n; i++)
			{
				t->~T(); t++;
			}
			if (alloc)
				alloc->pfree(alloc->ctx, p);
		}
	}
private:

};
template<class _T, bool isinit = true>
class auto_release
{
public:
	template<typename... Args>
	auto_release(_T* p1 = nullptr, Args... args) :p(p1)
	{
		if (isinit && p)
		{
			new(p)_T(args...);
		}
	}

	~auto_release()
	{
		release();
	}
	template<typename... Args>
	void set(_T* p1, Args... args)
	{
		p = p1;
		if (isinit && p)
		{
			new(p)_T(args...);
		}
	}
	void swap(auto_release<_T>& a)
	{
		std::swap(p, a.p);
	}
	void release()
	{
		if (p)
		{
			p->~_T();
			p = nullptr;
		}
	}
private:
	_T* p = nullptr;
};
#if 0
#define pcx_lock_type spin_mutex
#else
#define pcx_lock_type std::recursive_mutex
#endif // 1
class palloc_cx
{
public:

private:
	palloc_t _pac_s = {};
	palloc_t* _pac = nullptr;
	size_t _size = 0;
	typedef void (*destructor_cb) (void* p);
	std::unordered_map<void*, destructor_cb> _gc;
	// 等待释放的对象
	// std::unordered_set<void*> _slaughter;
	char lmutex[sizeof(pcx_lock_type)] = {};
	pcx_lock_type* _lk = nullptr;
	auto_release<pcx_lock_type> arrm;
public:
	palloc_cx() {
	}
	palloc_cx(palloc_t* p) :_pac(p) {

	}

	palloc_cx(int s, alloc_info_t* p) {
		_pac = &_pac_s;
		init(s, p);
	}

	palloc_cx(size_t s) {
		_pac = &_pac_s;
		init(s);
	}

	~palloc_cx()
	{
		free_pac();
		if (_lk)
		{
			_lk = nullptr;
		}
	}
	void free_pac()
	{
		free_obj(!_size);
		auto_lock alk(_lk);
		if (_pac && _size)
		{
			if (_pac->_user_p)
				_pac->destroy_pool(_pac->_user_p);
			palloc_close(_pac);
		}
	}
	void free_obj(bool isf)
	{
		if (_pac)
		{
			auto_lock alk(_lk);
			if (isf)
			{
				for (auto& [k, v] : _gc)
				{
					v(k);
					pfree((size_t*)k - 1);
				}
			}
			else
			{
				for (auto& [k, v] : _gc)
				{
					v(k);
				}
			}
			_gc.clear();
		}
	}
public:
	void swap(palloc_cx& right)
	{
		std::swap(*this, right);
	}
	void set_lock(bool is)
	{
		if (is)
		{
			_lk = (pcx_lock_type*)lmutex;
			arrm.set(_lk);
		}
		else
		{
			_lk = nullptr;
		}
	}
	void init(palloc_t* p) {
		if (p && p != _pac)
		{
			free_pac();
			_pac = p;
			_size = 0;
		}
	}
	void init(size_t s, alloc_info_t* a = nullptr)
	{
		if (s > 0)
		{
			if (_pac && _size)
			{
				free_pac();
			}
			{
				_pac = &_pac_s;
				palloc_init(_pac, a);
				if (_pac)
				{
					_pac->_user_p = _pac->create_pool(_pac, s);
					_size = s;
					set_lock(true);
				}
			}
		}
		else
		{
			throw u8"palloc_c init size不能为0";
		}
	}
	palloc_t* get()
	{
		return _pac;
	}
	alloc_info_t* get_alloc()
	{
		return &_pac->_dev;
	}
	int64_t acount()
	{
		return _pac->_dev.count;
	}
	int64_t alloc_size()
	{
		return _pac->_dev.alloc_size;
	}

public:
	// 重置内存池
	void reset_pool()
	{
		auto_lock alk(_lk);
		free_obj(!_size);
		if (_size)
		{
			_pac->reset_pool(_pac->_user_p);
		}
	}
	// 直接用分配器申请内存
	template<class _T>
	_T* malloc(size_t s)
	{
		return (_T*)_pac->_dev.palloc(_pac->_dev.ctx, s);
	}
	template<class _T>
	_T* ocalloc(size_t n)
	{
		return (_T*)calloc(n, sizeof(_T));
	}
	void* calloc(size_t n, size_t s)
	{
		auto as = n * s;
		void* p = nullptr;
		if (as > 0)
		{
			p = (char*)_pac->_dev.palloc(_pac->_dev.ctx, as);
			if (p)
			{
				size_t* c = (size_t*)p;
				size_t ls = as % sizeof(size_t);
				size_t ct = as / sizeof(size_t);
				for (size_t i = 0; i < ct; i++, c++)
				{
					if (*c)
					{
						*c = 0;
					}
				}
				if (ls)
					memset(c, 0, ls);
			}
		}
		return p;
	}
	void* realloc(void* p, size_t s)
	{
		return _pac && _pac->_dev.prealloc ? _pac->_dev.prealloc(_pac->_dev.ctx, p, s) : nullptr;
	}
	void free(void* p)
	{
		_pac->_dev.pfree(_pac->_dev.ctx, p);
	}
public:
	void* palloc(size_t s)
	{
		auto_lock alk(_lk);
		return _pac->palloc(_pac->_user_p, s);
	}
	void* prealloc(void* ptr, size_t s)
	{
		auto_lock alk(_lk);
		return _pac->prealloc(_pac->_user_p, ptr, s);
	}
	// 申请内存并清空
	void* pcalloc(size_t c, size_t s = 1)
	{
		auto_lock alk(_lk);
		return _pac->pcalloc(_pac->_user_p, c, s);
	}
	void pfree(void* p)
	{
		auto_lock alk(_lk);
		_pac->pfree(_pac->_user_p, p);
	}
	// onew申请无需执行析构函数的对象,调用pfree即可
	template <class _T, bool isc = true>
	_T* onew(size_t n = 1)
	{
		if (n < 1)n = 1;
		_T* p = (_T*)palloc(sizeof(_T) * n);
		auto t = p;
		if (isc)
		{
			for (size_t i = 0; i < n; i++, t++)
			{
				new(t)_T();
			}
		}
		return p;
	}
	template <class _T>
	_T* onew(size_t n, const _T& c)
	{
		_T* p = (_T*)palloc(sizeof(_T) * n);
		if (p)
		{
			for (size_t i = 0; i < n; i++)
			{
				*(p + i) = c;
			}
		}
		return p;
	}
	// -----pnew带析构函数的对象----------------------------------------------------------------
	template <class _T, typename... Args>
	_T* pnew(size_t n, Args... args)
	{
		int64_t un = n;
		if (un < 0) return nullptr;
		_T* p = (_T*)palloc(sizeof(_T) * n + sizeof(size_t));
		if (p)
		{
			*(size_t*)p = n;
			p = (_T*)((char*)p + sizeof(size_t));
			{
				auto_lock alk(_lk);
				_gc[p] = (destructor_cb)&gc_destructor<_T>;
			}
			for (size_t i = 0; i < n; i++)
			{
				new(p + i)_T(args...);
			}
		}
		return p;
	}
	template <class _T, typename... Args>
	_T* pnew1(Args... args)
	{
		return pnew<_T>(1, args...);
	}
	template <class _T>
	void pdelete(_T* p)
	{
		//给开辟的n个对象调用析构函数	
		if (!p)return;
#if 1
		auto_lock alk(_lk);
		char* top = (char*)p;
		//top -= sizeof(size_t);
		auto it = _gc.find(top);
		if (it != _gc.end())
		{
			it->second(top);
			pfree((size_t*)p - 1);
			_gc.erase(it);
		}
#else
		gc_destructor(p);
#endif
		}

	template <class _T>
	static void gc_destructor(_T* p)
	{
		size_t n = *((size_t*)p - 1);
		for (size_t i = 0; i < n; i++)
		{
			(p + i)->~_T();
		}
	}
private:
};
#else
	typedef struct palloc_cxs {
		palloc_t* _pac;
		size_t _size;
	}palloc_cx;
#endif // __cplusplus

#if 0

	class A
	{
	public:
		A()
		{
			printf("A()\n");
		}

		virtual ~A()
		{
			printf("~A()\n");
		}

	private:

	};
	class B :public A
	{
	public:
		B(int a) :_a(a)
		{
			printf("B()\n");
		}

		~B()
		{
			printf("~B()%d\n", _a);
		}

	private:
		int _a = 0;
	};

	void test_obj()
	{
		palloc_cx pcx(4096);
		printf("\x1b[34;1m%s\x1b[0m\n", "a");
		auto a = pcx.pnew<A>(2);
		printf("\x1b[34;1m%s\x1b[0m\n", "b");
		auto b = pcx.pnew<B>(2, 6);
		auto b1 = pcx.pnew1<B>(8);
		printf("\x1b[32;1m%s\x1b[0m\n", "自动析构");
	}

#endif // 0
