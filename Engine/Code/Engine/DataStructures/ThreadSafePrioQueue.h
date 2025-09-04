// Bradley Christensen - 2022-2025
#pragma once
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <vector>



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

    typedef typename std::vector<T*>::iterator iterator;
    typedef typename std::vector<T*>::const_iterator const_iterator;

    // Lock() before iterating!
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    iterator erase(iterator where);
    
private:

    std::atomic<bool>       m_isQuitting = false;
    
    mutable std::mutex      m_lock;
    std::condition_variable m_condVar;
    std::vector<T*>         m_heap;
};




//----------------------------------------------------------------------------------------------------------------------
// Template function definitions
//
template<typename T>
bool ThreadSafePrioQueue<T>::IsEmpty() const
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    return m_heap.empty();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int ThreadSafePrioQueue<T>::Count() const
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    return (int) m_heap.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void ThreadSafePrioQueue<T>::Push(T* obj)
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    m_heap.push_back(obj);
	std::push_heap(m_heap.begin(), m_heap.end());
    uniqueLock.unlock(); // supposedly faster and still safe to unlock before notifying?
    m_condVar.notify_one();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T* ThreadSafePrioQueue<T>::Pop(bool blocking)
{
    T* result = nullptr;
    
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    
    while (m_heap.empty() && !m_isQuitting)
    {
        if (blocking)
        {
            m_condVar.wait(uniqueLock);
        }
        else break;
    }
    
    if (!m_heap.empty())
    {
        result = m_heap.front();
		std::pop_heap(m_heap.begin(), m_heap.end());
        m_heap.pop_back();
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



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename ThreadSafePrioQueue<T>::iterator ThreadSafePrioQueue<T>::begin()
{
    return m_heap.begin();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename ThreadSafePrioQueue<T>::iterator ThreadSafePrioQueue<T>::end()
{
    return m_heap.end();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename ThreadSafePrioQueue<T>::const_iterator ThreadSafePrioQueue<T>::begin() const
{
    return m_heap.begin();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename ThreadSafePrioQueue<T>::const_iterator ThreadSafePrioQueue<T>::end() const
{
    return m_heap.end();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename ThreadSafePrioQueue<T>::iterator ThreadSafePrioQueue<T>::erase(typename ThreadSafePrioQueue<T>::iterator where)
{
    if (where == m_heap.end())
    {
        return m_heap.end();
    }
    std::iter_swap(where, m_heap.end() - 1);
    m_heap.pop_back();
    std::make_heap(m_heap.begin(), m_heap.end());
    return where;
}