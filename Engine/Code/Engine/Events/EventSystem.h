// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "EventSubscriber.h"
#include "EventUtils.h"
#include "Engine/Core/StringUtils.h"
#include <unordered_map>
#include <string>




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

    virtual void Shutdown() override;

    // Returns the number of subscribers that responded to the FireEvent call
    int FireEvent(std::string const& name);
    int FireEvent(std::string const& name, NamedProperties& args);

    bool IsEventBound(std::string const& name) const;

    void SubscribeFunction(std::string const& eventName, EventCallbackFunction callbackFunc);
    void UnsubscribeFunction(std::string const& eventName, EventCallbackFunction callbackFunc);

    template<typename T_Object, typename T_Method>
    void SubscribeMethod(std::string const& eventName, T_Object* object, T_Method method);

    template<typename T_Object, typename T_Method>
    void UnsubscribeMethod(std::string const& eventName, T_Object* object, T_Method method);

    Strings GetAllEventNames() const;

protected:

    EventSystemConfig const m_config;

    std::unordered_map<std::string, std::vector<EventSubscriber*>> m_events;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventSystem::SubscribeMethod(std::string const& eventName, T_Object* object, T_Method method)
{
    std::string lowerName = StringUtils::GetToLower(eventName);
    auto& subList = m_events[lowerName];
    subList.emplace_back(new EventSubscriberMethod(object, method));
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventSystem::UnsubscribeMethod(std::string const& eventName, T_Object* object, T_Method method)
{
    std::string lowerName = StringUtils::GetToLower(eventName);
    auto& subList = m_events[lowerName];
    for (auto it = subList.begin(); it != subList.end();)
    {
        auto& sub = *it;
        if (sub->DoesObjectMatch(object) && sub->DoesFunctionMatch(&method))
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
