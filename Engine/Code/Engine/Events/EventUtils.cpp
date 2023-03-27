// Bradley Christensen - 2022-2023
#include "Engine/Events/EventUtils.h"

#include "EventSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction callbackFunc)
{
    g_eventSystem->SubscribeFunction(eventName, callbackFunc);
}



//----------------------------------------------------------------------------------------------------------------------
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction callbackFunc)
{
    g_eventSystem->UnsubscribeFunction(eventName, callbackFunc);
}



//----------------------------------------------------------------------------------------------------------------------
int FireEvent(std::string const& eventName, NamedProperties& args)
{
    return g_eventSystem->FireEvent(eventName, args);
}



//----------------------------------------------------------------------------------------------------------------------
int FireEvent(std::string const& eventName)
{
    return g_eventSystem->FireEvent(eventName);
}
