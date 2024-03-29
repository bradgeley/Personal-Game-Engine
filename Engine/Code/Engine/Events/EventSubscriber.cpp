﻿// Bradley Christensen - 2022-2023
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
    return m_callbackFunc == *reinterpret_cast<EventCallbackFunction const*>(functionAddress);
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSubscriberFunction::Execute(NamedProperties& args)
{
    return m_callbackFunc(args);
}
