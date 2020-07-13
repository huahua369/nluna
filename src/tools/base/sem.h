#pragma once
#include <functional>
namespace hz
{
	class sem_dev_s
	{
	public:
		sem_dev_s() {}
		virtual~sem_dev_s() {}

		virtual void wait() = 0;

		virtual int wait_try() = 0;

		virtual void post() = 0;
	};

	class sem_d
	{
	public:
		sem_d()noexcept;

		virtual ~sem_d()noexcept;
		void wait()noexcept;
		void wait_f(std::function<bool()> func)noexcept;
		int	 wait_try() noexcept;
		void post()noexcept;
	private:
		sem_dev_s* _dev = nullptr;
	};// !sem_t

}// !hz
