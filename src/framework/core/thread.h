#pragma once

#include "../stdafx.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <iostream>
#include <memory>
#include <process.h>

namespace Grafkit {

	class Thread;

	/**
	Java-like Runnable interface
	*/
	class Runnable {
		friend Thread;

	public:
		virtual int Run() = 0;
		virtual ~Runnable() {}
	};


	/**
		Java-like thread management
	*/
	class Thread {
	public:
		//Thread(std::auto_ptr<Runnable> run);
		Thread(Runnable *run);
		Thread();

		virtual ~Thread();

		void Start();
		void Stop();
		void Join();

		static int GetCPUCount();

	protected:
		virtual int Run() { return 0; }

	protected:
		Runnable *m_pRunnable;

	private:
		HANDLE m_hThread;
		DWORD m_wThreadID;

		Thread(const Thread&);
		const Thread& operator=(const Thread&);

		// stores return value from run()
		int m_lastResult;


		static DWORD WINAPI startThreadRunnable(LPVOID pVoid);
		static DWORD WINAPI startThread(LPVOID pVoid);
	};

	/**
	QT-like Mutex class
	*/
	class Mutex {
	public:
		Mutex();
		~Mutex();

		void Lock();
		void Unlock();

		bool IsLocked() { return m_isLocked != 0; }

	private:
		int m_isLocked;
		HANDLE m_hMutex;
	};

	// Qt-inspired context/mutex locker
	class MutexLocker {
	public:
		MutexLocker(Mutex& mutex) : m_mutex(&mutex) { m_mutex->Lock(); }
		MutexLocker(Mutex* mutex) : m_mutex(mutex) { m_mutex->Lock(); }
		~MutexLocker() { m_mutex->Unlock(); }

		Mutex * GetMutex() { return m_mutex; }

	private:
		Mutex * m_mutex;
	};

}

DEFINE_EXCEPTION(ThreadException, 0, "Thread Exception");
