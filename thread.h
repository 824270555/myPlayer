#ifndef THREAD_H
#define THREAD_H

#include <thread>

class Thread
{
public:
    Thread() {}
    ~Thread() {
        if (m_thread) {
            Thread::Stop();
        }
    }
    void Start() {}
    void Stop() {
        m_abort = 1;
        if (m_thread) {
            m_thread->join();
            delete  m_thread;
            m_thread = nullptr;
        }
    }

    virtual void Run() = 0;
protected:
    int m_abort = 0;
    std::thread *m_thread = nullptr;
};


#endif // THREAD_H
