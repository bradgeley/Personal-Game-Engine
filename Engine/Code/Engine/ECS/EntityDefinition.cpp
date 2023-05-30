// Bradley Christensen - 2023
#include "EntityDefinition.h"
#include "Component.h"



//----------------------------------------------------------------------------------------------------------------------
void EntityDefinition::Cleanup()
{
    for (auto& c : m_components)
    {
        delete c;
    }
    m_components.clear();
}
