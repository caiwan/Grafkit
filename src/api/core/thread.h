#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include <iostream>
#include <memory>
//#include <cassert>
#include <process.h>

/**
	\ingroup core_utils
	@{
		\defgroup core_thread Threading
		@{
			@brief Java-like threading support
		@}
	@}
	
	\ingroup core_thread
	@{
*/
namespace FWcore{

	class Thread;

	/**
		@brief Runnable interface
	*/
	class Runnable {
		friend Thread;
		
		public:
			virtual int run() = 0;
			virtual ~Runnable(){}
	};
	
	/**
		Threadpool
		
		@todo feature: +threadpool
	*/
	class ThreadPool{
	
	};
		
	/**
		@brief Thread interface
	*/	
	class Thread /*: public Runnable*/{
		private:
			HANDLE hThread;
			DWORD wThreadID;

			Runnable *runnable;

			Thread(const Thread&);
			const Thread& operator=(const Thread&);

			// stores return value from run()
			int result;
			
			virtual int run() {return 0;} 

			static DWORD WINAPI startThreadRunnable(LPVOID pVoid);
			static DWORD WINAPI startThread(LPVOID pVoid);

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

			static int getNumberOfCPUs();
	};

	/**
		threadsafe sorszam huzass
	*/
	class Semaphore{
		private:
			int num;
			int lock;
		public:
			Semaphore();
			//~Semaphore();
			int getNext();	///< kovetkezo sorszamot kihuzza
			void reset();	///< visszaallitja a szamlalot nullara
	};
///@}
};