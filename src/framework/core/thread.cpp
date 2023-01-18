#include "stdafx.h"

#include <process.h>

#include "thread.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

Thread::Thread(Runnable* r) : m_pRunnable(r)
{
    if (!m_pRunnable)
    THROW_EX_DETAILS(ThreadException, "Runnable *r failed ");
    this->m_hThread = CreateThread(nullptr, 0, startThreadRunnable, LPVOID(this), CREATE_SUSPENDED, &this->m_wThreadID);
    if (!m_hThread)
    {
        //DWORD err = GetLastError();
        THROW_EX_DETAILS(ThreadException, "_beginthreadex failed");
    }
}

Thread::Thread() : m_pRunnable(nullptr)
{
    this->m_hThread = CreateThread(nullptr, 0, startThread, LPVOID(this), CREATE_SUSPENDED, &this->m_wThreadID);
    if (!m_hThread)
    {
        //DWORD err = GetLastError();
        THROW_EX_DETAILS(ThreadException, "_beginthreadex failed");
    }
}

DWORD WINAPI Thread::startThreadRunnable(LPVOID pVoid)
{
    if (!pVoid)
    THROW_EX(NullPointerException);

    Thread* rThread = static_cast<Thread*>(pVoid);
    rThread->m_lastResult = rThread->m_pRunnable->Run();

    return rThread->m_lastResult;
}

DWORD WINAPI Thread::startThread(LPVOID pVoid)
{
    if (!pVoid)
    THROW_EX(NullPointerException);

    Thread* aThread = static_cast<Thread*>(pVoid);
    aThread->m_lastResult = aThread->Run();

    return aThread->m_lastResult;
}

Thread::~Thread()
{
    if (m_wThreadID != GetCurrentThreadId())
    {
        this->Stop();
        DWORD rc = CloseHandle(m_hThread);
        /*
                if(!rc)
                    THROW_EX_DETAILS(ThreadException, 5003, "CloseHandle failed");			
        */
    }
}

void Thread::Start()
{
    if (!this->m_hThread)
    THROW_EX_DWORD(ThreadException, GetLastError());

    DWORD rc = ResumeThread(m_hThread);
    if (rc == -1)
    {
        THROW_EX_DWORD(ThreadException, GetLastError());
    }
}

void Thread::Join() { WaitForSingleObject(this->m_hThread, INFINITE); }

void Thread::Stop()
{
    WaitForSingleObject(this->m_hThread, 250);
    TerminateThread(this->m_hThread, 0);
}

int Thread::GetCPUCount()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (si.dwNumberOfProcessors > 128)
        return 128;
    return si.dwNumberOfProcessors;
}

/////////////////////////////////////////////////////////

Mutex::Mutex() : m_isLocked(0) { m_hMutex = CreateMutex(nullptr, false, nullptr); }

Mutex::~Mutex()
{
    if (m_hMutex)
        CloseHandle(m_hMutex);
}

void Mutex::Lock(DWORD milli)
{
    WaitForSingleObject(m_hMutex, milli);
    m_isLocked++;
}

void Mutex::Unlock()
{
    m_isLocked--;
    ReleaseMutex(m_hMutex);
}

MutexLocker::~MutexLocker()
{
    m_mutex->Unlock();

    if (m_mutex->IsLocked())
    assert(0);
}
