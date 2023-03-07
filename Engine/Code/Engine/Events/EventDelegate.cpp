// Bradley Christensen - 2022-2023
#include "Engine/Events/EventDelegate.h"



//----------------------------------------------------------------------------------------------------------------------
void EventDelegate::Broadcast(NamedProperties& args) const
{
    for (auto& sub : m_subs)
    {
        sub->Execute(args);
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
}
