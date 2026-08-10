// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/Name.h"
#include "EventCallbackFunction.h"
#include "EventSubscriber.h"
#include <unordered_map>



struct EventSubscriber;



//----------------------------------------------------------------------------------------------------------------------
// THE Event System
//
extern class EventSystem* g_eventSystem;



//----------------------------------------------------------------------------------------------------------------------
struct EventSystemConfig
{
    
};



//----------------------------------------------------------------------------------------------------------------------
class EventSystem : public EngineSubsystem
{
public:

    EventSystem(EventSystemConfig config);
    virtual ~EventSystem() override;

    virtual void Shutdown() override;

    // Returns the number of subscribers that responded to the FireEvent call
    int FireEvent(Name eventName) const;
    int FireEvent(Name eventName, NamedProperties& args) const;

    bool IsEventBound(Name eventName) const;
	bool IsFunctionBound(Name eventName, EventCallbackFunction callbackFunc) const;

    template<typename T_Object, typename T_Method>
	bool IsMethodBound(Name eventName, T_Object* object, T_Method method) const;

    void SubscribeFunction(Name eventName, EventCallbackFunction callbackFunc);
    void UnsubscribeFunction(Name eventName, EventCallbackFunction callbackFunc);

    template<typename T_Object, typename T_Method>
    void SubscribeMethod(Name eventName, T_Object* object, T_Method method);
     
    template<typename T_Object, typename T_Method>
    void UnsubscribeMethod(Name eventName, T_Object* object, T_Method method);

    Strings GetAllEventNames() const;

protected:

    EventSystemConfig const m_config;

    std::unordered_map<Name, std::vector<EventSubscriber*>> m_events;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename T_Object, typename T_Method>
bool EventSystem::IsMethodBound(Name eventName, T_Object* object, T_Method method) const
{
    auto it = m_events.find(eventName);
    if (it == m_events.end())
    {
        return false;
    }

    std::vector<EventSubscriber*> const& subList = it->second;
    for (EventSubscriber* const& sub : subList)
	{
        ASSERT_OR_DIE(sub != nullptr, "EventSystem::IsMethodBound() - EventSubscriber is null");
		if (sub->DoesObjectMatch(object) && sub->DoesFunctionMatch(&method))
		{
			return true;
		}
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventSystem::SubscribeMethod(Name eventName, T_Object* object, T_Method method)
{
    auto& subList = m_events[eventName];
	if (IsMethodBound(eventName, object, method))
	{
		ERROR_AND_DIE(StringUtils::StringF("EventSystem::SubscribeMethod() - Double binding to event '%s'", eventName.ToCStr()));
		return;
	}
	auto sub = new EventSubscriberMethod<T_Object>(object, method);
    subList.push_back(sub);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventSystem::UnsubscribeMethod(Name eventName, T_Object* object, T_Method method)
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
        ASSERT_OR_DIE(sub != nullptr, "EventSystem::UnsubscribeMethod() - EventSubscriber is null");
        if (sub->DoesObjectMatch(object) && sub->DoesFunctionMatch(&method))
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
