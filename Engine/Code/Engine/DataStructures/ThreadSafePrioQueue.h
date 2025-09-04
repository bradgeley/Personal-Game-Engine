// Bradley Christensen - 2022-2025
#pragma once
#include <atomic>
#include <condition_variable>
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
// Thread Safe Priority Queue
//
// Thread safe wrapper around std::priority_queue
//
template<typename T>
class ThreadSafePrioQueue
{
public:

    bool IsEmpty() const;
    int Count() const;
    void Push(T* obj);
    T* Pop(bool blocking = true);
    void Quit();
    void Lock();
    void Unlock();
    
private:

    std::atomic<bool>       m_isQuitting = false;
    
    mutable std::mutex      m_lock;
    std::condition_variable m_condVar;
    std::priority_queue<T*> m_queue;
};




//----------------------------------------------------------------------------------------------------------------------
// Template function definitions
//
template<typename T>
bool ThreadSafePrioQueue<T>::IsEmpty() const
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    return m_queue.empty();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int ThreadSafePrioQueue<T>::Count() const
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    return (int) m_queue.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafePrioQueue<T>::Push(T* obj)
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    m_queue.push(obj);
    uniqueLock.unlock(); // supposedly faster and still safe to unlock before notifying?
    m_condVar.notify_one();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T* ThreadSafePrioQueue<T>::Pop(bool blocking)
{
    T* result = nullptr;
    
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    
    while (m_queue.empty() && !m_isQuitting)
    {
        if (blocking)
        {
            m_condVar.wait(uniqueLock);
        }
        else break;
    }
    
    if (!m_queue.empty())
    {
        result = m_queue.top();
        m_queue.pop();
    }
 
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafePrioQueue<T>::Quit()
{
    // Must lock the mutex here for m_isQuitting otherwise very rarely a thread will be reading m_isQuitting right as
    // the value is getting set, then it waits on the cond var after the notify goes off. We want the value to
    // be set to true before any threads can read it so that they all wake up and see true at the same time. The threads
    // that were not waiting will see true and not wait, and the ones who are will catch the notify then see true.
    m_lock.lock();
    m_isQuitting = true;
    m_lock.unlock();
    
    m_condVar.notify_all();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafePrioQueue<T>::Lock()
{
    m_lock.lock();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafePrioQueue<T>::Unlock()
{
    m_lock.unlock();
}
