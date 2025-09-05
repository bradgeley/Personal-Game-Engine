// Bradley Christensen - 2022-2025
#include "Engine/Events/EventSystem.h"
#include "EventSubscriber.h"
#include "Engine/DataStructures/NamedProperties.h"



//----------------------------------------------------------------------------------------------------------------------
EventSystem* g_eventSystem = nullptr;



//----------------------------------------------------------------------------------------------------------------------
EventSystem::EventSystem(EventSystemConfig config) : EngineSubsystem("EventSystem"), m_config(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
EventSystem::~EventSystem()
{
	g_eventSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void EventSystem::Shutdown()
{
    for (auto pair : m_events)
    {
        std::vector<EventSubscriber*>& subs = pair.second;
        for (EventSubscriber*& sub : subs)
        {
            delete sub;
        }
        subs.clear();
    }
    m_events.clear();
}



//----------------------------------------------------------------------------------------------------------------------
int EventSystem::FireEvent(Name name)
{
    NamedProperties emptyArgs;
    return FireEvent(name, emptyArgs);
}



//----------------------------------------------------------------------------------------------------------------------
int EventSystem::FireEvent(Name name, NamedProperties& args)
{
    auto it = m_events.find(name);
    if (it != m_events.end())
    {
        int numExecuted = 0;
        std::vector<EventSubscriber*> const& subList = it->second;
        for (EventSubscriber* const& sub : subList)
        {
            bool consumed = sub->Execute(args);
            numExecuted++;
            if (consumed) 
            {
                break;
            }
        }

        return numExecuted;
    }

    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSystem::IsEventBound(Name name) const
{
    return (m_events.find(name) != m_events.end());
}



//----------------------------------------------------------------------------------------------------------------------
void EventSystem::SubscribeFunction(Name eventName, EventCallbackFunction callbackFunc)
{
    auto& subList = m_events[eventName];
    subList.emplace_back(new EventSubscriberFunction(callbackFunc));
}



//----------------------------------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeFunction(Name eventName, EventCallbackFunction callbackFunc)
{
    auto& subList = m_events[eventName];
    for (auto it = subList.begin(); it != subList.end();)
    {
        auto& sub = *it;
        if (sub->DoesFunctionMatch((void const*) callbackFunc))
        {
            delete sub;
            it = subList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
Strings EventSystem::GetAllEventNames() const
{
    Strings result;
    for (auto& pair : m_events)
    {
        result.emplace_back(pair.first.ToString());
    }
    return result;
}
