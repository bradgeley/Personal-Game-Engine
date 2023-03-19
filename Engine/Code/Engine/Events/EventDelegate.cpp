// Bradley Christensen - 2022-2023
#include "Engine/Events/EventDelegate.h"



//----------------------------------------------------------------------------------------------------------------------
void EventDelegate::Broadcast(NamedProperties& args) const
{
	bool consumed = false;
    for (auto& sub : m_subs)
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
	for (int i = 0; i < (int) m_subs.size(); ++i)
	{
		auto& sub = m_subs[i];
		if (sub->DoesObjectMatch(nullptr) && sub->DoesFunctionMatch((void const*)callbackFunc))
		{
			m_subs.erase(m_subs.begin() + i);
			return;
		}
	}
}
