#pragma once

#include "core/exceptions.h"

namespace Grafkit
{
    class Thread;

    /**
    Java-like Runnable interface
    */
    class Runnable
    {
        friend Thread;

    public:
        virtual int Run() = 0;

        virtual ~Runnable() {
        }
    };


    /**
        Java-like thread management
    */
    class Thread
    {
    public:
        //Thread(std::auto_ptr<Runnable> run);
        explicit Thread(Runnable* run);
        Thread();

        virtual ~Thread();

        void Start();
        void Stop();
        void Join();

        static int GetCPUCount();

    protected:
        virtual int Run() { return 0; }
        Runnable* m_pRunnable;

    private:
        HANDLE m_hThread;
        DWORD m_wThreadID;

        Thread(const Thread&) = delete;
        const Thread& operator=(const Thread&) = delete;

        // stores return value from run()
        int m_lastResult;


        static DWORD WINAPI startThreadRunnable(LPVOID pVoid);
        static DWORD WINAPI startThread(LPVOID pVoid);
    };

    /**
    QT-like Mutex class
    */
    class Mutex
    {
    public:
        Mutex();
        ~Mutex();

        void Lock(DWORD milli = INFINITE);
        void Unlock();

        bool IsLocked() const { return m_isLocked != 0; }

    private:
        int m_isLocked;
        HANDLE m_hMutex;
    };

    // Qt-inspired context/mutex locker
    class MutexLocker
    {
    public:
        explicit MutexLocker(Mutex& mutex, DWORD milli = INFINITE) : m_mutex(&mutex) { m_mutex->Lock(milli); }
        explicit MutexLocker(Mutex* mutex, DWORD milli = INFINITE) : m_mutex(mutex) { m_mutex->Lock(milli); }

        ~MutexLocker();

        Mutex* GetMutex() const { return m_mutex; }

    private:
        Mutex* m_mutex;
    };
}

DEFINE_EXCEPTION(ThreadException, 0, "Thread Exception");
