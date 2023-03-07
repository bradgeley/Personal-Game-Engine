// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Events/EventUtils.h"
#include "EventSubscriber.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct EventDelegate
{
    void Broadcast(NamedProperties& args) const;
    
    void SubscribeFunction(EventCallbackFunction callbackFunc);
    void UnsubscribeFunction(EventCallbackFunction callbackFunc);

    template<typename T_Object, typename T_Method>
    void SubscribeMethod(T_Object& object, T_Method method);

    template<typename T_Object, typename T_Method>
    void UnsubscribeMethod(T_Object& object, T_Method method);

protected:

    std::vector<EventSubscriber*> m_subs;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventDelegate::SubscribeMethod(T_Object& object, T_Method method)
{
    m_subs.emplace_back(new EventSubscriberMethod(object, method));
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventDelegate::UnsubscribeMethod(T_Object& object, T_Method method)
{
    for (int i = 0; i < (int) m_subs.size(); ++i)
    {
        auto& sub = m_subs[i];
        if (sub->DoesObjectMatch(object) && sub->DoesFunctionMatch(method))
        {
            m_subs.erase(m_subs.begin() + i);
            return;
        }
    }
}
