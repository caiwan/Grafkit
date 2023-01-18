#include "../../framework.h"
#include "thread.h"
#include "../../debug/exceptions.h"

using namespace FWcore;

Thread::Thread(Runnable *r) : runnable(r) {
	if(!runnable)
		throw new ExceptionEX(5000, "Runnable *r failed ");
	//hThread = (HANDLE)_beginthreadex(NULL,0,Thread::startThreadRunnable, (LPVOID)this, CREATE_SUSPENDED, &wThreadID);
	this->hThread = CreateThread(NULL, 0, Thread::startThreadRunnable, (LPVOID)this, CREATE_SUSPENDED, &this->wThreadID);
	if(!hThread)
		throw new ExceptionEX(5001, "_beginthreadex failed");
}

Thread::Thread() : runnable(NULL) {
	//hThread = (HANDLE)_beginthreadex(NULL,0,Thread::startThread, (LPVOID)this, CREATE_SUSPENDED, &wThreadID);
	this->hThread = CreateThread(NULL, 0, Thread::startThread, (LPVOID)this, CREATE_SUSPENDED, &this->wThreadID);
	if(!hThread)
		throw new ExceptionEX(5002, "_beginthreadex failed");
}

DWORD WINAPI Thread::startThreadRunnable(LPVOID pVoid) {
	if (!pVoid) 
		throw new EXCEPT_NullPointerException(); //ExceptionEX(5003, "Nullpointer error");

	Thread* rThread = static_cast<Thread*>(pVoid);
	rThread->result = rThread->runnable->run();
	
	ExitThread(rThread->result);
	return 0;
}

DWORD WINAPI Thread::startThread(LPVOID pVoid) {
	if (!pVoid) 
		throw new EXCEPT_NullPointerException(); //ExceptionEX(5003, "Nullpointer error");

	Thread* aThread = static_cast<Thread*>(pVoid);
	//aThread->status = TS_run;
	aThread->result = aThread->run();

	ExitThread(aThread->result);
	return 0;
}

Thread::~Thread() {
	if(wThreadID != GetCurrentThreadId()) {
		this->stop();
		DWORD rc = CloseHandle(hThread);
/*
		if(!rc)
			throw new ExceptionEX(5003, "CloseHandle failed");			
*/
	}
}

void Thread::start() {
	//assert(hThread);
	if(!this->hThread)
		throw new ExceptionEX(5004, "ResumeThread failed !hThread");

	DWORD rc = ResumeThread(hThread);
	if(rc == -1){
		FWcore::writeLog(true, MSG_warn, "Last error %d", GetLastError());
		throw new ExceptionEX(5005, "ResumeThread failed !rc");
	}
}

void Thread::join() {
	WaitForSingleObject(this->hThread, INFINITE);
}

void Thread::stop(){
	WaitForSingleObject(this->hThread, 250);
	TerminateThread(this->hThread, 0);
}

void Thread::recreate(){
	this->stop();
	DWORD rc = CloseHandle(hThread);
	hThread = NULL;
	if (!runnable){
		this->hThread = CreateThread(NULL, 0, Thread::startThread, (LPVOID)this, CREATE_SUSPENDED, &this->wThreadID);
	}else{
		this->hThread = CreateThread(NULL, 0, Thread::startThreadRunnable, (LPVOID)this, CREATE_SUSPENDED, &this->wThreadID);
	}
	if (!hThread) throw new ExceptionEX(5001, "_beginthreadex failed");
}

int Thread::getNumberOfCPUs(){
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	if (si.dwNumberOfProcessors > 128) return 128;
	return si.dwNumberOfProcessors;
}

/////////////////////////////////////////////////////////

Semaphore::Semaphore(){
	this->lock = 0;
	this->num = 0;
}

void Semaphore::reset(){
	this->lock = 0;
	this->num = 0;
}

int Semaphore::getNext(){
	if (!this->lock++){
		int res = num;
		num ++;
		lock = 0;
		return res;
	} else {
		while (this->lock);
		return this->getNext();
	}
}