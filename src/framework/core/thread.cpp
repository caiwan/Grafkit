#include "stdafx.h"

#include "thread.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

Thread::Thread(Runnable *r) : m_pRunnable(r) {
	if(!m_pRunnable)
		THROW_EX_DETAILS(ThreadException, "Runnable *r failed ");
	//m_hThread = (HANDLE)_beginthreadex(nullptr,0,Thread::startThreadRunnable, (LPVOID)this, CREATE_SUSPENDED, &m_wThreadID);
	this->m_hThread = CreateThread(nullptr, 0, startThreadRunnable, (LPVOID)this, CREATE_SUSPENDED, &this->m_wThreadID);
	if(!m_hThread)
		THROW_EX_DETAILS(ThreadException, "_beginthreadex failed");
}

Thread::Thread() : m_pRunnable(nullptr) {
	//m_hThread = (HANDLE)_beginthreadex(nullptr,0,Thread::startThread, (LPVOID)this, CREATE_SUSPENDED, &m_wThreadID);
	this->m_hThread = CreateThread(nullptr, 0, startThread, (LPVOID)this, CREATE_SUSPENDED, &this->m_wThreadID);
	if(!m_hThread)
		THROW_EX_DETAILS(ThreadException, "_beginthreadex failed");
}

DWORD WINAPI Thread::startThreadRunnable(LPVOID pVoid) {
	if (!pVoid) 
		THROW_EX(NullPointerException);

	Thread* rThread = static_cast<Thread*>(pVoid);
	rThread->m_lastResult = rThread->m_pRunnable->Run();
	
	ExitThread(rThread->m_lastResult);
	return 0;
}

DWORD WINAPI Thread::startThread(LPVOID pVoid) {
	if (!pVoid)
		THROW_EX(NullPointerException);

	Thread* aThread = static_cast<Thread*>(pVoid);
	//aThread->status = TS_run;
	aThread->m_lastResult = aThread->Run();

	ExitThread(aThread->m_lastResult);
	return 0;
}

Thread::~Thread() {
	if(m_wThreadID != GetCurrentThreadId()) {
		this->Stop();
		DWORD rc = CloseHandle(m_hThread);
/*
		if(!rc)
			THROW_EX_DETAILS(ThreadException, 5003, "CloseHandle failed");			
*/
	}
}

void Thread::Start() {
	//assert(m_hThread);
	if(!this->m_hThread)
		THROW_EX_DWORD(ThreadException, GetLastError());

	DWORD rc = ResumeThread(m_hThread);
	if(rc == -1){
		THROW_EX_DWORD(ThreadException, GetLastError());
	}
}

void Thread::Join() {
	WaitForSingleObject(this->m_hThread, INFINITE);
}

void Thread::Stop(){
	WaitForSingleObject(this->m_hThread, 250);
	TerminateThread(this->m_hThread, 0);
}

int Thread::GetCPUCount(){
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	if (si.dwNumberOfProcessors > 128) return 128;
	return si.dwNumberOfProcessors;
}

/////////////////////////////////////////////////////////

Mutex::Mutex() : m_isLocked(0)
{
	m_hMutex = CreateMutex(nullptr, true, nullptr);
}

Mutex::~Mutex()
{
	if (m_hMutex)
		CloseHandle(m_hMutex);
}

void Mutex::Lock() {
	WaitForSingleObject(m_hMutex, INFINITE);
	m_isLocked++;
}

void Mutex::Unlock() {
	m_isLocked--;
	ReleaseMutex(m_hMutex);
}

