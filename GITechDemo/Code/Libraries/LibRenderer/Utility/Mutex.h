#ifndef MUTEX_H_
#define MUTEX_H_

#define WIN_USE_CRITICAL_SECTION (1)

#if defined(LINUX)

	//Headers
	#include <pthread.h>

	//Data types
	typdef pthread_mutex_t MUTEX;

	#ifndef EBUSY
	#define EBUSY 16 // resource busy
	#endif

	//Macroes
	#define MUTEX_INIT(mutex)		(0 == pthread_mutex_init (&(mutex), NULL))
	#define MUTEX_DESTROY(mutex)	(0 == pthread_mutex_destroy(&(mutex)))
	#define MUTEX_LOCK(mutex)		(0 == pthread_mutex_lock (&(mutex)))
	#define MUTEX_UNLOCK(mutex)		(0 == pthread_mutex_unlock (&(mutex)))
	#define MUTEX_TRYLOCK(mutex)	(EBUSY == pthread_mutex_trylock (&(mutex)))

#elif defined(_WINDOWS) && (WIN_USE_CRITICAL_SECTION == 0)

	//Headers
	#include <Windows.h>
	#include <process.h>

	//Data types
	typedef HANDLE MUTEX;

	//Macroes
	#define MUTEX_INIT(mutex)		(NULL != ((mutex) = CreateMutex (0, FALSE, 0)))
	#define MUTEX_DESTROY(mutex)	(0 != CloseHandle((mutex)))
	#define MUTEX_LOCK(mutex)		(WAIT_FAILED == WaitForSingleObject ((mutex), INFINITE))
	#define MUTEX_UNLOCK(mutex)		(0 == ReleaseMutex ((mutex)))
	#define MUTEX_TRYLOCK(mutex)	(WAIT_TIMEOUT == WaitForSingleObject((mutex), IGNORE))

#elif defined(_WINDOWS) && (WIN_USE_CRITICAL_SECTION == 1)

	//Headers
	#include <Windows.h>

	//Data types
	typedef CRITICAL_SECTION MUTEX;

	//Macroes
	#define MUTEX_INIT(mutex)		(InitializeCriticalSection(&mutex))
	#define MUTEX_DESTROY(mutex)	(DeleteCriticalSection(&mutex))
	#define MUTEX_LOCK(mutex)		(EnterCriticalSection(&mutex))
	#define MUTEX_UNLOCK(mutex)		(LeaveCriticalSection(&mutex))
	#define MUTEX_TRYLOCK(mutex)	(0 != TryEnterCriticalSection(&mutex))

#else

	//Data types
	#define MUTEX Please_define_either_WINDOWS_or_LINUX_in_sourcecode

#endif

#endif // MUTEX_H_