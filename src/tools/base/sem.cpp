
#include "sem.h"
using namespace hz;
#ifdef __has_include


#if (__has_include(<uv0.h>))
#include <uv.h>

class Semaphore :public sem_dev_s {
public:
	Semaphore() noexcept
	{
		if (0 != uv_sem_init(&_sem, 0))
		{
			throw("uv_sem_init");
		}
	}

	~Semaphore() noexcept
	{
		uv_sem_destroy(&_sem);
	}

	void wait() noexcept
	{
		uv_sem_wait(&_sem);
	}

	int wait_try() noexcept
	{
		return uv_sem_trywait(&_sem);
	}

	void post() noexcept
	{
		uv_sem_post(&_sem);
	}
public:
	uv_sem_t _sem = {};
};
#elif (__has_include(<mach/semaphore.h>))
#include <mach/semaphore.h>
#include <mach/mach_init.h>
#include <mach/task.h>

class Semaphore :public sem_dev_s {
public:
	explicit Semaphore() noexcept
		:_Handle(New()) { }

	~Semaphore() { Release(_Handle); }

	void wait() noexcept {
		while (semaphore_wait(_Handle) != KERN_SUCCESS) {}
	}

	void post() noexcept { semaphore_signal(_Handle); }

	int wait_try() {
		// todo wait_try 
		return semaphore_wait_noblock(_Handle);
	}
private:
	static semaphore_t New() noexcept {
		semaphore_t ret;
		semaphore_create(mach_task_self(), &ret, SYNC_POLICY_FIFO, 0);
		return ret;
	}

	static void Release(semaphore_t sem) noexcept {
		semaphore_destroy(mach_task_self(), sem);
	}

	semaphore_t _Handle = {};
};

#elif (__has_include(<windows.h>))
#include <winsock2.h> 
#include <Windows.h>
class Semaphore :public sem_dev_s {
public:
	Semaphore() noexcept
		: _Handle(CreateSemaphore(nullptr, 0, MAXLONG, nullptr)) { }

	~Semaphore() noexcept { CloseHandle(_Handle); }

	void wait() noexcept { WaitForSingleObject(_Handle, INFINITE); }

	void post() noexcept {
		LONG __last;
		ReleaseSemaphore(_Handle, 1, &__last);
	}

	int wait_try() {
		DWORD r = WaitForSingleObject(_Handle, 0);

		if (r == WAIT_OBJECT_0)
			return 0;

		if (r == WAIT_TIMEOUT)
			return WAIT_TIMEOUT;

		//abort();
		return -1; /* Satisfy the compiler. */
	}

private:
	HANDLE _Handle = 0;
};

#elif (__has_include(<semaphore.h>))
#include <semaphore.h>
class Semaphore :public sem_dev_s {
public:
	Semaphore() noexcept {
		sem_init(&_Semaphore, 0, 0);
	}

	~Semaphore() noexcept {
		sem_destroy(&_Semaphore);
	}

	void wait() noexcept {
		sem_wait(&_Semaphore);
	}

	void post() noexcept {
		sem_post(&_Semaphore);
	}
	int wait_try() {
		return sem_trywait(&_Semaphore);
	}
private:
	sem_t _Semaphore;
};
#endif
#else
class Semaphore :public sem_dev_s {
public:
	Semaphore() noexcept {}

	~Semaphore() noexcept {}

	void wait() noexcept {}

	void post() noexcept {}
	int wait_try() {
		return -1;
	}
};
# error "No Adaquate Semaphore Supported to be adapted from"

#endif // __has_include
namespace hz
{
	sem_d::sem_d() noexcept
	{
		_dev = (sem_dev_s*) new Semaphore();
	}
	sem_d::~sem_d() noexcept
	{
		delete _dev;
	}
	void sem_d::wait() noexcept
	{
		_dev->wait();
	}
	void sem_d::wait_f(std::function<bool()> func) noexcept
	{
		if (func)
		{
			while (!func())
				_dev->wait();
		}
		else
		{
			_dev->wait();
		}
	}
	int sem_d::wait_try() noexcept
	{
		return _dev->wait_try();
	}
	void sem_d::post() noexcept
	{
		_dev->post();
	}
}// !hz
