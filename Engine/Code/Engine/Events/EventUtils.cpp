// Bradley Christensen - 2022-2025
#include "Engine/Events/EventUtils.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void EventUtils::SubscribeEventCallbackFunction(Name eventName, EventCallbackFunction callbackFunc)
{
    ASSERT_OR_DIE(g_eventSystem != nullptr, "Event system does not exist");
    if (g_eventSystem)
    {
        g_eventSystem->SubscribeFunction(eventName, callbackFunc);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void EventUtils::UnsubscribeEventCallbackFunction(Name eventName, EventCallbackFunction callbackFunc)
{
    ASSERT_OR_DIE(g_eventSystem != nullptr, "Event system does not exist");
    if (g_eventSystem)
    {
        g_eventSystem->UnsubscribeFunction(eventName, callbackFunc);
    }
}



//----------------------------------------------------------------------------------------------------------------------
int EventUtils::FireEvent(Name eventName, NamedProperties& args)
{
    ASSERT_OR_DIE(g_eventSystem != nullptr, "Event system does not exist");
    if (g_eventSystem)
    {
        return g_eventSystem->FireEvent(eventName, args);
    }
    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
int EventUtils::FireEvent(Name eventName)
{
    ASSERT_OR_DIE(g_eventSystem != nullptr, "Event system does not exist");
    if (g_eventSystem)
    {
        return g_eventSystem->FireEvent(eventName);
    }
    return 0;
}
