#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <functional>
#include <vector>
#include <queue>
#include "sem.h"

namespace hz {
	// 高精度计时
	class mp_timer
	{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
	public:
		mp_timer() : m_begin(std::chrono::high_resolution_clock::now())
		{
		}

		~mp_timer()
		{
		}

		void reset() { m_begin = std::chrono::high_resolution_clock::now(); }

		//输出毫秒
		uint64_t elapsed() const
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
		//微秒
		uint64_t elapsed_micro() const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}
	private:

	};
	// 多线程列队
	template<class T>
	class a_queue
	{
	private:
		std::queue<T> q;
		std::recursive_mutex _mtx;
		sem_d _sig1;
		sem_d* _sig = nullptr;
	public:
		a_queue() :_sig(&_sig1)
		{
		}

		~a_queue()
		{
		}
		void set_sem(sem_d* s)
		{
			if (s)
				_sig = s;
		}
		sem_d* get_sem()
		{
			return _sig;
		}
	public:
		size_t size() { return q.size(); }
		void post()
		{
			_sig->post();
		}
		void push1(const T& t)
		{
			auto_lock ak(&_mtx);
			q.push(t);
		}
		void push(const T& t)
		{
			{
				auto_lock ak(&_mtx);
				q.push(t);
			}
			_sig->post();
		}
		T pop()
		{
			T t = {};
			if (q.size())
			{
				auto_lock ak(&_mtx);
				if (q.size())
				{
					t = q.front();
					q.pop();
				}
			}
			return t;
		}
		//template<class Tq>
		//void pop2aq(Tq* oq)
		//{
		//	if (q.size())
		//	{
		//		auto_lock ak(&_mtx);
		//		for (; q.size();)
		//		{
		//			auto& t = q.front();
		//			//t->set_tp(oq);
		//			oq->push1(t);
		//			q.pop();
		//		}
		//	}
		//}
		template<class Tq>
		void pop2q(Tq* oq)
		{
			if (q.size())
			{
				auto_lock ak(&_mtx);
				for (; q.size();)
				{
					auto& t = q.front();
					oq->push(t);
					q.pop();
				}
			}
		}
		T get_wait_one(int64_t n)
		{
			T task = {};
			if (n > 0)
			{
				auto ik = _sig->wait_try();
				if (ik != 0)return task;
			}
			else
			{
				_sig->wait();
			}
			{
				task = pop();
			}
			return task;
		}
	};

	/*
		todo 异步任务
		串联、并联
		set_next为true则继续执行下一下任务
	*/
	class promise_cx
	{
	public:
		using fun_t = std::function<bool()>;
		using funv_t = std::function<void()>;
		using vec_t = std::vector<fun_t>;
	private:
		// 等待数量
		std::atomic_int _await_count = 0;
		size_t idx = 0;
		vec_t cbs;
		promise_cx* _prev = nullptr;
		a_queue<promise_cx*> _child;
		void* _tp = nullptr;
		sem_d* _sem = nullptr;
		funv_t _done_func;
		// 是否进行下一步
		bool is_next = true;

	public:
		promise_cx();
		template<typename... Args>
		promise_cx(Args... args)
		{
			auto n = sizeof...(args);
			cbs = { args... };
		}
		virtual ~promise_cx();
		void wait();
		void insert_child(promise_cx*);
		void post2child();
	public:
		void set_tp(void* tp) { _tp = tp; }
		// 设置等待上一个promise执行完再执行自己的
		void set_prev(promise_cx*);
		// 判断是否执行下一个
		bool is() { return is_next; }
		void set_next();
		void set_nonext() { is_next = false; }
		// 计数
		int inc_ac(int n) {
			return _await_count += n;
		}
		void set_ac(int n) { _await_count = n; }
		int  get_ac() { return _await_count; }

		void inc_idx(int ic)
		{
			idx += ic;
		}
		// 判断是否全部执行完
		bool is_done();
		// 异常直接重置跳出所有
		void clear();
		/*
			插入异步任务方法有
			then（插入到最后，支持普通全局函数、静态函数、以及lambda表达式、类成员函数）
			insert（插入到最后，普通全局函数、静态函数、以及lambda表达式）
			insert1（插入到最后，支持类成员函数）
			insert2（插入到倒数第一前面，只支持类成员函数）
		*/
		// 添加异步任务
		template<typename... Args>
		void insert(Args... args)
		{
			auto n = sizeof...(args);
			vec_t m = { args... };
			if (m.size())
			{
				cbs.insert(cbs.end(), m.begin(), m.end());
			}
		}
		// 添加批量类成员函数
		template<typename Self, typename ...Args>
		void insert1(Self* t, Args ...args)
		{
			auto n = sizeof...(args);
			fun_t m[] = { (printarg(t,args))... };
			if (n)
			{
				cbs.insert(cbs.end(), m, m + n);
			}
		}
		// 插入到倒数第二
		template<typename Self, typename ...Args>
		void insert2(Self* t, Args ...args)
		{
			auto n = sizeof...(args);
			fun_t m[] = { (printarg(t,args))... };
			if (n)
			{
				cbs.insert(--cbs.end(), m, m + n);
			}
		}

		/*
		* 支持普通全局函数、静态函数、以及lambda表达式
		* this->add_task([=](){});
		*/
		template<typename Function, typename... Args>
		promise_cx& then(const Function& func, Args... args)
		{
			fun_t task = [&func, args...](){ return func(args...); };
			cbs.emplace_back(std::move(task));
			return *this;
		}
		/*
		·类成员函数
		·this->then( this,&Test::func, str, i);
		*/
		template<typename Function, typename Self, typename ...Args>
		promise_cx& then(Self* self, Function func, Args ...args)
		{
			fun_t task = [func, self, args...](){
				return (self->*func)(args...);
			};
			cbs.emplace_back(std::move(task));
			return *this;
		}
		template<typename Function, typename... Args>
		promise_cx& set_done_cb(Function func, Args... args)
		{
			auto n = sizeof...(args);
			_done_func = [func, args...](){ func(args...); };
			return *this;
		}
		// 异步线程执行
		bool call();
	private:
		template<class Self, class T>
		fun_t printarg(Self* self, T func)
		{
			fun_t task = [func, self]() {
				return (self->*func)();
			};
			return task;
		}
	public:

	};
	// 用于初始化和自动析构
	class auto_bcb
	{
	public:
		std::function<void()> _cb[2];
		bool isc = false;
	public:
		auto_bcb()
		{
		}

		auto_bcb(auto_bcb& t) :isc(true)
		{
			_cb[0] = t._cb[0];
			_cb[1] = t._cb[1];
			if (_cb[0])
			{
				_cb[0]();
			}
		}

		~auto_bcb()
		{
			if (isc && _cb[1])
			{
				_cb[1]();
			}
		}
		void set(auto_bcb& t)
		{
			isc = true;
			_cb[0] = t._cb[0];
			_cb[1] = t._cb[1];
			if (_cb[0])
			{
				_cb[0]();
			}
		}
	private:

	};
	// 创建执行线程池
	class promise_run
	{
	private:
		// 共享列队
		void* cvqptr = nullptr;
		// 线程、执行列队
		void* _thr_ctx = nullptr;
		auto_bcb _ab;
		size_t tn = 0;

	public:
		promise_run(int thr_num = 0, auto_bcb* acb = nullptr);

		virtual ~promise_run();

	public:
		void push(promise_cx* p);
		// 添加不post
		void push1(promise_cx* p);
		void post(bool is_all = false);

		size_t run(int ms);
		// 使用线程池执行
		size_t run_thr();

		static void sleep(int s);
	public:
		// 异步测试
		static void test();
	};
}
//!hz
