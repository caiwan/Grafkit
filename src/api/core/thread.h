#pragma once

#include "../stdafx.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include <iostream>
#include <memory>
#include <process.h>

namespace Grafkit{

	class Thread;

	/**
		Runnable interface
	*/
	class Runnable {
	friend Thread;
		
	public:
		virtual int run() = 0;
		virtual ~Runnable(){}
	};
	
	
	/**
		Java-like thread management
		Thread interface
	*/	
	class Thread{
	public:
		//Thread(std::auto_ptr<Runnable> run);
		Thread(Runnable *run);
		Thread();
			
		virtual ~Thread();

		void start();
		void stop();
		void join();

		///@todo ez helyett valami mas megoldas kell 
		void recreate();

		static int getCPUCount();


	private:
		HANDLE m_hThread;
		DWORD m_wThreadID;

		Runnable *m_pRunnable;

		Thread(const Thread&);
		const Thread& operator=(const Thread&);

		// stores return value from run()
		int m_lastResult;

		virtual int run() { return 0; }

		static DWORD WINAPI startThreadRunnable(LPVOID pVoid);
		static DWORD WINAPI startThread(LPVOID pVoid);
	};

	/**
		threadsafe sorszam huzass
	*/
	class Semaphore{
	public:
		Semaphore();
		~Semaphore();

		UINT getNext();
		void reset();

	private:
		UINT m_count;
		HANDLE m_hMutex;
	};

}

DEFINE_EXCEPTION(ThreadException, 0, "Thread Exception");
