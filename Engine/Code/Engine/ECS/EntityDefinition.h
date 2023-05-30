// Bradley Christensen - 2023
#pragma once
#include <vcruntime_typeinfo.h>
#include <vector>



struct Component;



//----------------------------------------------------------------------------------------------------------------------
// One of each component that has been registered to the ecs, treated as a definition for how to instantiate an entity.
//
struct EntityDefinition
{
    void Cleanup();
    
    template<typename T>
    T* GetComponentDef();
    
    std::vector<Component*> m_components;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T* EntityDefinition::GetComponentDef()
{
    for (auto& c : m_components)
    {
        if (!c)
        {
            continue;
        }
        
        if (typeid(*c).hash_code() == typeid(T).hash_code())
        {
            return static_cast<T*>(c);
        }
    }
    return nullptr;
}
