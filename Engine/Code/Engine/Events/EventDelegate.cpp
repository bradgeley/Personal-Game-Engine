// Bradley Christensen - 2022-2025
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
void EventDelegate::Broadcast(NamedProperties& args) const
{
	bool consumed = false;
    for (EventSubscriber* const& sub : m_subs)
    {
        consumed = sub->Execute(args);
    	if (consumed)
    	{
    		return;
    	}
    }
}



//----------------------------------------------------------------------------------------------------------------------
void EventDelegate::SubscribeFunction(EventCallbackFunction callbackFunc)
{
    m_subs.emplace_back(new EventSubscriberFunction(callbackFunc));
}



//----------------------------------------------------------------------------------------------------------------------
void EventDelegate::UnsubscribeFunction(EventCallbackFunction callbackFunc)
{
	for (auto it = m_subs.begin(); it != m_subs.end();)
	{
		EventSubscriber*& sub = *it;
		if (sub && sub->DoesObjectMatch(nullptr) && sub->DoesFunctionMatch((void const*)callbackFunc))
		{
			it = m_subs.erase(it);
		}
		else
		{
			it++;
		}
	}
}
