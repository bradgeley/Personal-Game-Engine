// Bradley Christensen - 2022-2023
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
int EventSystem::FireEvent(std::string const& name)
{
    NamedProperties emptyArgs;
    return FireEvent(name, emptyArgs);
}



//----------------------------------------------------------------------------------------------------------------------
int EventSystem::FireEvent(std::string const& name, NamedProperties& args)
{
    std::string lowerName = StringUtils::GetToLower(name);
    auto it = m_events.find(lowerName);
    if (it != m_events.end())
    {
        std::vector<EventSubscriber*> const& subList = it->second;
        for (EventSubscriber* const& sub : subList)
        {
            sub->Execute(args);
        }

        return (int) subList.size();
    }

    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSystem::IsEventBound(std::string const& name) const
{
    std::string lowerName = StringUtils::GetToLower(name);
    return (m_events.find(lowerName) != m_events.end());
}



//----------------------------------------------------------------------------------------------------------------------
void EventSystem::SubscribeFunction(std::string const& eventName, EventCallbackFunction callbackFunc)
{
    std::string lowerName = StringUtils::GetToLower(eventName);
    auto& subList = m_events[lowerName];
    subList.emplace_back(new EventSubscriberFunction(callbackFunc));
}



//----------------------------------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeFunction(std::string const& eventName, EventCallbackFunction callbackFunc)
{
    std::string lowerName = StringUtils::GetToLower(eventName);
    auto& subList = m_events[lowerName];
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
        result.emplace_back(pair.first);
    }
    return result;
}
