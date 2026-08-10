// Bradley Christensen - 2022-2026
#include "Engine/Events/EventSystem.h"
#include "EventSubscriber.h"
#include "Engine/Core/NamedProperties.h"



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
            ASSERT_OR_DIE(sub != nullptr, "EventSystem::Shutdown() - EventSubscriber is null");
            delete sub;
        }
        subs.clear();
    }
    m_events.clear();
}



//----------------------------------------------------------------------------------------------------------------------
int EventSystem::FireEvent(Name eventName) const
{
    NamedProperties emptyArgs;
    return FireEvent(eventName, emptyArgs);
}



//----------------------------------------------------------------------------------------------------------------------
int EventSystem::FireEvent(Name eventName, NamedProperties& args) const
{
    auto it = m_events.find(eventName);
    if (it == m_events.end())
    {
        return 0;
    }

    int numExecuted = 0;
    std::vector<EventSubscriber*> const& subList = it->second;
    for (EventSubscriber* const& sub : subList)
    {
        ASSERT_OR_DIE(sub != nullptr, "EventSystem::FireEvent() - EventSubscriber is null");
        bool consumed = sub->Execute(args);
        numExecuted++;
        if (consumed) 
        {
            break;
        }
    }

    return numExecuted;
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSystem::IsEventBound(Name eventName) const
{
    auto it = m_events.find(eventName);
    if (it == m_events.end())
    {
        return false;
    }

    std::vector<EventSubscriber*> const& subList = it->second;
    return !subList.empty();
}



//----------------------------------------------------------------------------------------------------------------------
bool EventSystem::IsFunctionBound(Name eventName, EventCallbackFunction callbackFunc) const
{
	auto it = m_events.find(eventName);
	if (it == m_events.end())
	{
		return false;
	}

	std::vector<EventSubscriber*> const& subList = it->second;
	for (EventSubscriber* const& sub : subList)
	{
        ASSERT_OR_DIE(sub != nullptr, "EventSystem::IsFunctionBound() - EventSubscriber is null");
		if (sub->DoesFunctionMatch((void const*)callbackFunc))
		{
			return true;
		}
	}
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void EventSystem::SubscribeFunction(Name eventName, EventCallbackFunction callbackFunc)
{
	if (IsFunctionBound(eventName, callbackFunc))
	{
		ERROR_AND_DIE(StringUtils::StringF("EventSystem::SubscribeFunction() - Double binding to event '%s'", eventName.ToCStr()));
		return;
	}

    std::vector<EventSubscriber*>& subList = m_events[eventName];
    subList.emplace_back(new EventSubscriberFunction(callbackFunc));
}



//----------------------------------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeFunction(Name eventName, EventCallbackFunction callbackFunc)
{
    auto eventIt = m_events.find(eventName);
    if (eventIt == m_events.end())
    {
        return;
    }

    std::vector<EventSubscriber*>& subList = eventIt->second;
    for (auto subIt = subList.begin(); subIt != subList.end();)
    {
        auto& sub = *subIt;
        ASSERT_OR_DIE(sub != nullptr, "EventSystem::UnsubscribeFunction() - EventSubscriber is null");
        if (sub->DoesFunctionMatch((void const*) callbackFunc))
        {
            delete sub;
            subIt = subList.erase(subIt);
        }
        else
        {
            ++subIt;
        }
    }
    if (subList.empty())
    {
		m_events.erase(eventName);
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
