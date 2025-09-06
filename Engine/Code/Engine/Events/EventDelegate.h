// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Events/EventCallbackFunction.h"
#include "EventSubscriber.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct EventDelegate
{
    ~EventDelegate();

    int Broadcast(NamedProperties& args) const;
    
    void SubscribeFunction(EventCallbackFunction callbackFunc);
    void UnsubscribeFunction(EventCallbackFunction callbackFunc);

    template<typename T_Object, typename T_Method>
    void SubscribeMethod(T_Object* object, T_Method method);

    template<typename T_Object, typename T_Method>
    void UnsubscribeMethod(T_Object* object, T_Method method);

protected:

    std::vector<EventSubscriber*> m_subs;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventDelegate::SubscribeMethod(T_Object* object, T_Method method)
{
    m_subs.emplace_back(new EventSubscriberMethod<T_Object>(object, method));
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventDelegate::UnsubscribeMethod(T_Object* object, T_Method method)
{
    for (auto it = m_subs.begin(); it != m_subs.end();)
    {
        EventSubscriber*& sub = *it;
        if (sub && sub->DoesObjectMatch(object) && sub->DoesFunctionMatch(&method))
        {
            delete sub;
            sub = nullptr;
            it = m_subs.erase(it);
        }
        else
        {
            it++;
        }
    }
}
