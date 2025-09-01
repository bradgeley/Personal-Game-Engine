// Bradley Christensen - 2022-2025
#pragma once
#include <atomic>
#include <condition_variable>
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
// Thread Safe Queue
//
// Thread safe wrapper around std::deque
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
    void Lock();
    void Unlock();

    typedef typename std::deque<T*>::iterator iterator;
    typedef typename std::deque<T*>::const_iterator const_iterator;

    // Lock() before iterating!
    iterator begin() { return m_queue.begin(); }
    iterator end() { return m_queue.end(); }
    const_iterator begin() const { return m_queue.begin(); }
    const_iterator end() const { return m_queue.end(); }
    iterator erase(const_iterator where) { return m_queue.erase(where); }
    
private:

    std::atomic<bool>       m_isQuitting = false;
    
    mutable std::mutex      m_lock;
    std::condition_variable m_condVar;
    std::deque<T*>          m_queue;
};




//----------------------------------------------------------------------------------------------------------------------
// Template function definitions
//
template<typename T>
bool ThreadSafeQueue<T>::IsEmpty() const
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    return m_queue.empty();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int ThreadSafeQueue<T>::Count() const
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    return (int) m_queue.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafeQueue<T>::Push(T* obj)
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    m_queue.emplace_back(obj);
    uniqueLock.unlock(); // supposedly faster and still safe to unlock before notifying?
    m_condVar.notify_one();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T* ThreadSafeQueue<T>::Pop(bool blocking)
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
        result = m_queue.front();
        m_queue.pop_front();
    }
 
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafeQueue<T>::Quit()
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
void ThreadSafeQueue<T>::Lock()
{
    m_lock.lock();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafeQueue<T>::Unlock()
{
    m_lock.unlock();
}
