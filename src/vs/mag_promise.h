#pragma once

#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <queue>

namespace hz {

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

	/*
		todo 异步任务
		串联、并联
		is_next为true则继续执行下一下任务
	*/
	class promise_cx
	{
	public:
		using vec_t = std::vector<std::function<void(bool* is_done)>>;
	private:
		bool is_next = true;
		size_t idx = 0;
		vec_t cbs;
	public:
		promise_cx()
		{
		}
		template<typename... Args>
		promise_cx(Args... args);
		virtual ~promise_cx();
		bool is() { return is_next; }
		bool isdone();
		// 重置
		void clear();

		template<typename... Args>
		void insert(Args... args);

		// 添加异步任务
		promise_cx& then(std::function<void(bool* is_done)> cb);
		// 异步线程执行
		void call();
	public:

	};
	class promise_run
	{
	private:
		std::queue<promise_cx*> rs;
		std::queue<promise_cx*> ws;

	public:
		promise_run()
		{
		}

		virtual ~promise_run()
		{
		}
	public:
		void push(promise_cx* p);
		size_t run(int ms);

		static void sleep(int s);
	public:
		// 异步测试
		static void test();
	};
}
//!hz
