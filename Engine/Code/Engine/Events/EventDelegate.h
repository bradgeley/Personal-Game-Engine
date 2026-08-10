// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/ErrorUtils.h"
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

	bool IsFunctionBound(EventCallbackFunction callbackFunc) const;

	template<typename T_Object, typename T_Method>
	bool IsMethodBound(T_Object* object, T_Method method) const;

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
    for (auto it = m_subs.begin(); it != m_subs.end(); it++)
    {
        EventSubscriber*& sub = *it;
        ASSERT_OR_DIE(sub != nullptr, "EventDelegate::UnsubscribeMethod() - EventSubscriber is null");
        if (sub->DoesObjectMatch(object) && sub->DoesFunctionMatch(&method))
        {
            delete sub;
            sub = nullptr;
            it = m_subs.erase(it);
            return; // Only allow single binding from the same source
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T_Object, typename T_Method>
bool EventDelegate::IsMethodBound(T_Object* object, T_Method method) const
{
    for (auto it = m_subs.begin(); it != m_subs.end(); it++)
    {
        EventSubscriber* const& sub = *it;
		ASSERT_OR_DIE(sub != nullptr, "EventDelegate::IsMethodBound() - Found null subscriber in list");
        if (sub->DoesObjectMatch(object) && sub->DoesFunctionMatch(&method))
        {
            return true;
        }
    }
    return false;
}
