
#include "hlUtil.h"
#include <palloc/palloc.h>
//#include <base/thread_pool.h>
#include "promise_cx.h"
#define B_SEM_H

namespace hz {
#ifndef __thread_h__

	static uint64_t get_tid()
	{
		auto stid = std::this_thread::get_id();
		int k = sizeof(stid);
		uint64_t ret = 0;
		memcpy(&ret, &stid, k);
		return ret;
	}

#ifdef _WIN32
	//
// Usage: SetThreadName (-1, "MainThread");
//
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // must be 0x1000
		LPCSTR szName; // pointer to name (in user addr space)
		DWORD dwThreadID; // thread ID (-1=caller thread)
		DWORD dwFlags; // reserved for future use, must be zero
	} THREADNAME_INFO;

	static void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = szThreadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;

		__try
		{
			RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_CONTINUE_EXECUTION)
		{
		}
	}
#endif // _WIN32
	static void set_thr_name(const char* name, unsigned int tid)
	{
#ifndef _WIN32
		prctl(PR_SET_NAME, name);
#else
		SetThreadName(tid, name);
#endif
	}
	// 设置当前线程名
	static void set_thr_name_cur(const char* name)
	{
		set_thr_name(name, get_tid());
	}
#endif // !__thread_h__

	promise_cx::promise_cx()
	{
	}

	promise_cx::~promise_cx()
	{
		if (_sem)
		{
			delete _sem;
			_sem = nullptr;
		}
	}

	void promise_cx::wait()
	{
		if (!_sem)
		{
			_sem = new sem_d();
		}
		if (_sem)
		{
			_sem->wait();
		}
	}

	void promise_cx::set_prev(promise_cx* p)
	{
		_prev = p;
	}

	void promise_cx::insert_child(promise_cx* c)
	{
		if (c)
		{
			c->set_prev(this);
			_child.push(c);
		}
	}

	void promise_cx::post2child()
	{
		auto t = (promise_run*)_tp;
		if (t && _child.size())
		{
			_child.pop2q(t);
			t->post();
		}
		if (_sem)_sem->post();
	}

	void promise_cx::set_next() {
		is_next = true;
		if (is_done())
		{
			return;
		}
	}

	bool promise_cx::is_done() {
		return idx == cbs.size();
	}

	// 重置
	void promise_cx::clear()
	{
		idx = 0;
		cbs.clear();
		is_next = false;
		set_ac(0);
	}

	// 异步线程执行
	bool promise_cx::call()
	{
		bool ret = true;
		// is_next==true则继续执行
		if (_prev && !_prev->is_done())return true;
		for (; is_next && idx < cbs.size();)
		{
			auto& cb = cbs[idx++];
			if (cb)
			{
				is_next = false;
				set_ac(1);
				is_next = cb();
				// 每次执行一个
				break;
			}
		}
		if (is_done())
		{
			post2child();
			if (_done_func)
				_done_func();
			ret = false;
		}
		return ret;
	}
	// --------------------------------------------------------------------------------

	class task_pr
	{
	public:
		std::thread thr;
		std::queue<promise_cx*> qs;
		a_queue<promise_cx*>* cvq = nullptr;
		void* _tp = nullptr;
		bool _start = true;
	public:
		task_pr()
		{
		}

		~task_pr()
		{
			stop();
			if (thr.joinable())
				thr.join();
		}
		void stop()
		{
			_start = false;
		}
		bool is_start()
		{
			return _start;
		}
		size_t run(int ms)
		{
			std::queue<promise_cx*> t;
			size_t c = 0;
			do {
				if (cvq) {
					auto p = cvq->get_wait_one(qs.size());
					if (p)
					{
						qs.push(p);
					}
				}
				bool is = false;
				for (; qs.size();)
				{
					auto p = qs.front();
					qs.pop();
					if (p)
					{
						if (p->call())
							t.push(p);
					}
					if (cvq->size())
					{
						auto tt = cvq->pop();
						if (tt)
						{
							qs.push(tt);
						}
					}
				}
				for (; t.size();)
				{
					auto p = t.front();
					qs.push(p);
					t.pop();
				}
				if (ms > 0)
				{
					promise_run::sleep(ms);
				}
				c = qs.size();
			} while (c > 0 && ms > 0 && _start);
			return c;
		}
	private:

	};

#define sig ((sem_d*)_sig)
#define tp ((task_pr*)_thr_ctx)
#define cqt ((a_queue<promise_cx*>*)cvqptr)

	promise_run::promise_run(int thr_num, auto_bcb* acb)
	{
		if (acb)
			_ab = *acb;
		auto cq = new a_queue<promise_cx*>();
		cvqptr = cq;
		if (thr_num > 0) {
#ifdef B_SEM_H
			auto tps = new task_pr[thr_num];
			for (size_t i = 0; i < thr_num; i++)
			{
				tps[i].cvq = cq;
				tps[i]._tp = this;
				std::thread a([=](task_pr* tpr, int idx) {
					set_thr_name_cur(("promise run " + std::to_string(idx)).c_str());
					auto p = &tpr[idx];
					auto_bcb acbt;
					if (acb)
						acbt.set(_ab);
					while (p && p->is_start())
					{
						p->run(1);
					}
					return;
					}, tps, i);
				tps[i].thr.swap(a);
			}
			_thr_ctx = tps;
			tn = thr_num;
#endif // B_SEM_H
		}
		if (!_thr_ctx)
		{
			_thr_ctx = new task_pr[1];
			tp->cvq = cq;
			tp->_tp = this;
		}
	}

	promise_run::~promise_run()
	{
		if (_thr_ctx)
		{
			auto np = tp;
			for (size_t i = 0; i < tn; i++)
			{
				np->stop(); np++;
			}
			if (cqt)
			{
				// 发通知，tn为线程数量
				for (size_t i = 0; i < tn; i++)
					cqt->post();
			}

			delete[] tp;
			_thr_ctx = nullptr;
		}
		if (cqt)
		{
			delete cqt;
			cvqptr = nullptr;
		}
	}


	void promise_run::push(promise_cx* p)
	{
		if (p)
		{
			p->set_tp(this);
			cqt->push(p);
		}
	}
	void promise_run::push1(promise_cx* p)
	{
		if (p)
		{
			p->set_tp(this);
			cqt->push1(p);
		}
	}

	void promise_run::post(bool is_all)
	{
		cqt->post();
		if (is_all)
		{
			for (int i = 1; i < tn; i++)
				cqt->post();
		}
	}


	size_t promise_run::run(int ms)
	{
		return tp && !tn ? tp->run(ms) : 0;
	}
	size_t promise_run::run_thr()
	{
		return size_t();
	}

	void promise_run::sleep(int s)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(s));
	}
	void print_tid()
	{
		auto pid = get_tid();
		printf("pid: %llu\t", pid);
	}

	/*
		todo 异步测试
		插入异步任务方法有
		then（插入到最后，支持普通全局函数、静态函数、以及lambda表达式、类成员函数）
		insert（插入到最后，普通全局函数、静态函数、以及lambda表达式）
		insert1（插入到最后，支持类成员函数）
		insert2（插入到倒数第一前面，只支持类成员函数）
	*/
	class test_task :public promise_cx
	{
	public:
		int k = 0;
	public:
		test_task()
		{
			insert1(this, &test_task::ts0, &test_task::ts1, &test_task::ts2);
		}

		~test_task()
		{
		}
	public:
		bool ts0()
		{
			k += 1; print_tid();
			printf("test_task第0步%d\n", k);
			// 设置d=true则完成本块任务
			//也可以多线程调用set_next();完成本任务，比如在libuv回调函数调用
			set_next();
			return false;
		}
		bool ts1()
		{
			k += 2; print_tid();
			printf("test_task第1步%d\n", k);
			return true;
		}
		bool ts2()
		{
			k += 3; print_tid();
			printf("test_task第2步%d\n", k);
			return true;
		}
	};

	void promise_run::test()
	{
		promise_run pr(3), pr1;

		auto pf10 = &pr;
		auto pf1 = &pr1;
		auto lf1 = [=](promise_cx* a) {pf1->push(a); };
		std::function<void(promise_cx*)>f1, f2 = lf1;
		void* vf8[8] = {};
		void* vf18[8] = {};
		memcpy(vf8, &f1, sizeof(void*) * 8);
		void* v1[8] = {};
		memcpy(v1, &lf1, sizeof(void*) * 8);
		memcpy(vf18, &f2, sizeof(void*) * 8);
		int ks[] = { sizeof(lf1),sizeof(std::function<void()>) };
		bool b = false;
		promise_cx pe1([]() { print_tid(); printf("pe1第1步\n"); return true; }
		, []() {  print_tid(); printf("pe1第2步\n"); return true; }
		, []() { print_tid(); printf("pe1第3步\n"); return true; });

		pe1.insert([]() { print_tid(); printf("pe1第5步\n"); return true; }
		, []() { print_tid(); printf("pe1第6步\n"); return true; });

		test_task tk;
		test_task tk1;
		promise_cx pe2;
		mp_timer ts;
		bool* bp = nullptr;
		{
			tk.insert_child(&pe2);
			pe2.then([&]() {
				int ems = ts.elapsed(); print_tid();
				printf("pe2第1步:%d ms\n", ems);
				ts.reset();
				b = true;
				return false;
				})
				.then([&]() {
					int ems = ts.elapsed(); print_tid();
					printf("pe2第2步:%d ms\n", ems); ts.reset();
					b = true;
					return false;
					})
					.then([&]() {
						int ems = ts.elapsed();
						print_tid();
						printf("pe2第3步:%d ms\n", ems);
						b = true;
						return false;
						});
					// 等待另一个异步线程
					std::thread([&]() {
						while (1) {
							sleep(600);
							if (b) {
								pe2.set_next();
								b = false;
							}
						}}).detach();
		}
		ts.reset();
		// 执行异步操作
		{
			pr.push(&pe1);
			pr.push(&pe2);
			pr.push(&tk);
			pr.push(&tk1);
			int inc = 1;
			for (;;)
			{
				//pe1.call();pe2.call();
				pr.run(1);
				if (inc == 0)
				{
					break;
				}
				sleep(1);
			}
		}
		for (;;)
			sleep(1);
	}

}
//!hz
