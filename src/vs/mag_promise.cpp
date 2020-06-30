#include "mag_promise.h"

namespace hz {
	template<typename ...Args>
	promise_cx::promise_cx(Args ...args)
	{
		auto n = sizeof...(args);
		cbs = { args... };
	}
	template<typename ...Args>
	void promise_cx::insert(Args ...args)
	{
		auto n = sizeof...(args);
		vec_t m{ args... };
		if (m.size())
		{
			cbs.insert(cbs.end(), m.begin(), m.end());
		}
	}


	promise_cx::~promise_cx()
	{

	}

	bool promise_cx::isdone() {
		return idx > 0 && idx == cbs.size();
	}

	// 重置
	void promise_cx::clear()
	{
		idx = 0;
		cbs.clear();
		is_next = true;
	}



	// 添加异步任务
	promise_cx& promise_cx::then(std::function<void(bool* is_done)> cb)
	{
		cbs.push_back(cb);
		return *this;
	}

	// 异步线程执行
	void promise_cx::call()
	{
		// is_next==true则继续执行
		for (; is_next && idx < cbs.size();)
		{
			auto& cb = cbs[idx++];
			if (cb)
			{
				is_next = false;
				cb(&is_next);
			}
		}
	}
	// --------------------------------------------------------------------------------

	void promise_run::push(promise_cx* p)
	{
		if (p)
		{
			p->is() ? rs.push(p) : ws.push(p);
		}
	}

	size_t promise_run::run(int ms)
	{
		std::queue<promise_cx*> t;
		do {
			for (; rs.size();)
			{
				auto p = rs.front();
				rs.pop();
				if (p)
				{
					p->call();
					if (!p->isdone())
					{
						ws.push(p);
					}
				}
			}
			for (; ws.size();)
			{
				auto p = ws.front();
				ws.pop();
				if (p)
				{
					p->is() ? rs.push(p) : t.push(p);
				}
			}
			if (t.size())
			{
				ws.swap(t);
			}
			if (ms > 0)
			{
				sleep(ms);
			}
		} while (ms > 0);
		return rs.size() + ws.size();
	}

	void promise_run::sleep(int s)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(s));
	}

	// 异步测试

	void promise_run::test()
	{
		bool b = false;
		bool* bp = nullptr;
		promise_cx pe1([](bool* d) { printf("pe1第1步\n"); *d = true; }
		, [](bool* d) { printf("pe1第2步\n"); *d = true; }
		, [](bool* d) { printf("pe1第3步\n"); *d = true; });

		pe1.insert([](bool* d) { printf("pe1第5步\n"); *d = true; }
		, [](bool* d) { printf("pe1第6步\n"); *d = true; });

		promise_cx pe2;
		mp_timer ts;
		{
			pe2.then([&](bool* d) {
				int ems = ts.elapsed();
				printf("pe2第1步:%d ms\n", ems); ts.reset(); bp = d; b = true;
				})
				.then([&](bool* d) {
					int ems = ts.elapsed();
					printf("pe2第2步:%d ms\n", ems); ts.reset(); bp = d; b = true;
					})
					.then([&](bool* d) {
						int ems = ts.elapsed();
						printf("pe2第3步:%d ms\n", ems); bp = d; b = true;
						});
					// 等待另一个异步线程
					std::thread([&]() {
						while (1) {
							sleep(600);
							if (b) {
								if (bp)
									*bp = true;
								b = false;
							}
						}}).detach();
		}
		ts.reset();
		// 执行异步操作
		promise_run pr;
		pr.push(&pe1);
		pr.push(&pe2);
		for (;;)
		{
			//pe1.call();pe2.call();
			pr.run(0);
			sleep(1);
		}
	}

}
//!hz
