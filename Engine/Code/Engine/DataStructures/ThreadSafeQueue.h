// Bradley Christensen - 2022-2023
#pragma once
#include <atomic>
#include <condition_variable>
#include <optional>
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
// Thread Safe Queue
//
// Thread safe wrapper around std::queue
//
template<typename T>
class ThreadSafeQueue : protected std::queue<T>
{
public:

    bool IsEmpty() const;
    int Count() const;
    void Push(T const& obj);
    std::optional<T> Pop();
    
    std::mutex mutable      m_lock;
    
private:

    std::atomic<bool>       m_isQuitting = false;
    std::condition_variable m_condVar;
};




//----------------------------------------------------------------------------------------------------------------------
// Template function definitions
//
template<typename T>
bool ThreadSafeQueue<T>::IsEmpty() const
{
    return Count() == 0;
}



template <typename T>
int ThreadSafeQueue<T>::Count() const
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    int count = (int) std::queue<T>::size();
    return count;
}



template <typename T>
void ThreadSafeQueue<T>::Push(T const& obj)
{
    std::unique_lock<std::mutex> uniqueLock(m_lock);
    std::queue<T>::emplace(obj);
    m_condVar.notify_one();
}



template <typename T>
std::optional<T> ThreadSafeQueue<T>::Pop()
{
    std::optional<T> result = std::nullopt;
    std::unique_lock uniqueLock(m_lock);
    while (std::queue<T>::empty() && !m_isQuitting)
    {
        m_condVar.wait(uniqueLock);
    }
    if (!std::queue<T>::empty())
    {
        result = std::queue<T>::front();
        std::queue<T>::pop();
    }

    return result;
}
