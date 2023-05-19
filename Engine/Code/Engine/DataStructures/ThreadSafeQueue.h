// Bradley Christensen - 2022-2023
#pragma once
#include <atomic>
#include <condition_variable>
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
// Thread Safe Queue
//
// Thread safe wrapper around std::queue
//
template<typename T>
class ThreadSafeQueue
{
public:

    bool IsEmpty() const;
    int Count() const;
    void Push(T* obj);
    T* Pop(bool blocking = true);
    void Quit();

    
private:

    std::atomic<bool>       m_isQuitting = false;
    
    std::mutex mutable      m_lock;
    std::condition_variable m_condVar;
    std::queue<T*>          m_queue;
};




//----------------------------------------------------------------------------------------------------------------------
// Template function definitions
//
template<typename T>
bool ThreadSafeQueue<T>::IsEmpty() const
{
    std::unique_lock uniqueLock(m_lock);
    return m_queue.empty();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int ThreadSafeQueue<T>::Count() const
{
    std::unique_lock uniqueLock(m_lock);
    return (int) m_queue.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafeQueue<T>::Push(T* obj)
{
    std::unique_lock uniqueLock(m_lock);
    m_queue.push(obj);
    uniqueLock.unlock(); // supposedly faster and still safe to unlock before notifying?
    m_condVar.notify_one();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T* ThreadSafeQueue<T>::Pop(bool blocking)
{
    T* result = nullptr;
    
    std::unique_lock uniqueLock(m_lock);
    
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
        result = m_queue.front();
        m_queue.pop();
    }
 
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafeQueue<T>::Quit()
{
    m_lock.lock();
    m_isQuitting = true;
    m_lock.unlock();
    
    m_condVar.notify_all();
}
