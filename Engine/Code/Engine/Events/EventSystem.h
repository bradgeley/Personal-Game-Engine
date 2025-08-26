// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/Name.h"
#include "EventSubscriber.h"
#include "EventUtils.h"
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
    int FireEvent(Name name);
    int FireEvent(Name name, NamedProperties& args);

    bool IsEventBound(Name name) const;

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
template <typename T_Object, typename T_Method>
void EventSystem::SubscribeMethod(Name eventName, T_Object* object, T_Method method)
{
    auto& subList = m_events[eventName];
    subList.emplace_back(new EventSubscriberMethod(object, method));
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T_Object, typename T_Method>
void EventSystem::UnsubscribeMethod(Name eventName, T_Object* object, T_Method method)
{
    auto& subList = m_events[eventName];
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
