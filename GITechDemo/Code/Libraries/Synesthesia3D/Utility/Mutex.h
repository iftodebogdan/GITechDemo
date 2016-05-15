/**
 *	@file		Mutex.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2016 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *	@copyright
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	@copyright
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	@copyright
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#define WIN_USE_CRITICAL_SECTION (1)	/**< @brief Enables the use of critical sections on Windows, instead of regular mutexes. */

#if defined(LINUX)

	//Headers
	#include <pthread.h>

	//Data types
	typdef pthread_mutex_t MUTEX;

	#ifndef EBUSY
	#define EBUSY 16 // resource busy
	#endif

	//Macros
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

	//Macros
	#define MUTEX_INIT(mutex)		(NULL != ((mutex) = CreateMutex (0, FALSE, 0)))
	#define MUTEX_DESTROY(mutex)	(0 != CloseHandle((mutex)))
	#define MUTEX_LOCK(mutex)		(WAIT_FAILED == WaitForSingleObject ((mutex), INFINITE))
	#define MUTEX_UNLOCK(mutex)		(0 == ReleaseMutex ((mutex)))
	#define MUTEX_TRYLOCK(mutex)	(WAIT_TIMEOUT == WaitForSingleObject((mutex), IGNORE))

#elif defined(_WINDOWS) && (WIN_USE_CRITICAL_SECTION == 1)

	//Headers
	#include <Windows.h>

	//Data types
	typedef CRITICAL_SECTION MUTEX;	/**< @brief Platform independent mutex */

	//Macros
	#define MUTEX_INIT(mutex)		(InitializeCriticalSection(&mutex))	/**< @brief Initialized the mutex. */
	#define MUTEX_DESTROY(mutex)	(DeleteCriticalSection(&mutex))	/**< @brief Destroys the mutex. */
	#define MUTEX_LOCK(mutex)		(EnterCriticalSection(&mutex))	/**< @brief Attempts to lock the mutex (blocking). */
	#define MUTEX_UNLOCK(mutex)		(LeaveCriticalSection(&mutex))	/**< @brief Unlocks the mutex. */
	#define MUTEX_TRYLOCK(mutex)	(0 != TryEnterCriticalSection(&mutex))	/**< @brief Attempts to lock the mutex (non-blocking). */

#else

	//Data types
	#define MUTEX Please_define_either_WINDOWS_or_LINUX_in_sourcecode

	//Macros
	#define MUTEX_INIT(mutex)		MUTEX
	#define MUTEX_DESTROY(mutex)	MUTEX
	#define MUTEX_LOCK(mutex)		MUTEX
	#define MUTEX_UNLOCK(mutex)		MUTEX
	#define MUTEX_TRYLOCK(mutex)	MUTEX

#endif

#endif // MUTEX_H_
