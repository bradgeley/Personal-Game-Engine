// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Events/EventUtils.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
struct EventSubscriber
{
    virtual ~EventSubscriber() = default;
    virtual void Execute(NamedProperties& args) = 0;
    virtual bool DoesObjectMatch(void const* objectAddress) const = 0;
    virtual bool DoesFunctionMatch(void const* functionAddress) const = 0;
};



//----------------------------------------------------------------------------------------------------------------------
// Function
//
// Simple subscription, just a function with no object
//
struct EventSubscriberFunction : public EventSubscriber
{
    explicit EventSubscriberFunction(EventCallbackFunction f);
    
    virtual bool DoesObjectMatch(void const* objectAddress) const override;
    virtual bool DoesFunctionMatch(void const* functionAddress) const override;
    virtual void Execute(NamedProperties& args) override;

    EventCallbackFunction m_callbackFunc;
};



//----------------------------------------------------------------------------------------------------------------------
// Method
//
// Subscription with both an object and a member function
//
template<typename T>
struct EventSubscriberMethod : public EventSubscriber
{
    typedef bool (T::*EventCallbackMethod)(NamedProperties& args);

    explicit EventSubscriberMethod(T* object, EventCallbackMethod method);

    virtual bool DoesObjectMatch(void const* object) const override;
    virtual bool DoesFunctionMatch(void const* functionAddress) const override;
    virtual void Execute(NamedProperties& args) override;

    T* m_object;
    EventCallbackMethod m_method;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
EventSubscriberMethod<T>::EventSubscriberMethod(T* object, EventCallbackMethod method) : m_object(object), m_method(method)
{
        
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool EventSubscriberMethod<T>::DoesObjectMatch(void const* object) const
{
    return m_object == object;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool EventSubscriberMethod<T>::DoesFunctionMatch(void const* functionAddress) const
{
    return m_method == *reinterpret_cast<EventCallbackMethod const*>(functionAddress);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void EventSubscriberMethod<T>::Execute(NamedProperties& args)
{
    T& object = *m_object;
    (object.*m_method)(args);
}




