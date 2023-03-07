// Bradley Christensen - 2022-2023
#include "Engine/Events/EventSubscriber.h"



//----------------------------------------------------------------------------------------------------------------------
EventSubscriberFunction::EventSubscriberFunction(EventCallbackFunction f) : m_callbackFunc(f)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSubscriberFunction::DoesObjectMatch(void const* objectAddress) const
{
    (void) objectAddress;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSubscriberFunction::DoesFunctionMatch(void const* functionAddress) const
{
    return *reinterpret_cast<size_t const*>(&m_callbackFunc) == *reinterpret_cast<size_t*>(&functionAddress);
}



//----------------------------------------------------------------------------------------------------------------------
void EventSubscriberFunction::Execute(NamedProperties& args)
{
    m_callbackFunc(args);
}
