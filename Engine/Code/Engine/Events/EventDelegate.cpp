// Bradley Christensen - 2022-2026
#include "Engine/Events/EventDelegate.h"



//----------------------------------------------------------------------------------------------------------------------
EventDelegate::~EventDelegate()
{
	for (EventSubscriber*& sub : m_subs)
	{
		delete sub;
		sub = nullptr;
	}
	m_subs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
int EventDelegate::Broadcast(NamedProperties& args) const
{
	int numExecuted = 0;
	bool consumed = false;
    for (EventSubscriber* const& sub : m_subs)
    {
		ASSERT_OR_DIE(sub != nullptr, "EventDelegate::Broadcast() - EventSubscriber is null");
        consumed = sub->Execute(args);
		numExecuted++;
    	if (consumed)
    	{
			break;
    	}
    }
	return numExecuted;
}



//----------------------------------------------------------------------------------------------------------------------
void EventDelegate::SubscribeFunction(EventCallbackFunction callbackFunc)
{
    m_subs.emplace_back(new EventSubscriberFunction(callbackFunc));
}



//----------------------------------------------------------------------------------------------------------------------
void EventDelegate::UnsubscribeFunction(EventCallbackFunction callbackFunc)
{
	for (auto it = m_subs.begin(); it != m_subs.end(); it++)
	{
		EventSubscriber*& sub = *it;
		ASSERT_OR_DIE(sub != nullptr, "EventDelegate::UnsubscribeFunction() - EventSubscriber is null");
		if (sub->DoesObjectMatch(nullptr) && sub->DoesFunctionMatch((void const*)callbackFunc))
		{
			delete sub;
			sub = nullptr;
			it = m_subs.erase(it);
			return;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool EventDelegate::IsFunctionBound(EventCallbackFunction callbackFunc) const
{
	for (EventSubscriber* const& sub : m_subs)
	{
		ASSERT_OR_DIE(sub != nullptr, "EventDelegate::IsFunctionBound() - EventSubscriber is null");
		if (sub->DoesObjectMatch(nullptr) && sub->DoesFunctionMatch((void const*) callbackFunc))
		{
			return true;
		}
	}
	return false;
}
