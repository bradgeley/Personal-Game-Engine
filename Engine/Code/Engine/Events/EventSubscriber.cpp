// Bradley Christensen - 2022-2025
#include "Engine/Events/EventSubscriber.h"



//----------------------------------------------------------------------------------------------------------------------
EventSubscriberFunction::EventSubscriberFunction(EventCallbackFunction f) : m_callbackFunc(f)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSubscriberFunction::DoesObjectMatch(void const*) const
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSubscriberFunction::DoesFunctionMatch(void const* functionAddress) const
{
    return m_callbackFunc == *reinterpret_cast<EventCallbackFunction const*>(functionAddress);
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSubscriberFunction::Execute(NamedProperties& args)
{
    return m_callbackFunc(args);
}
