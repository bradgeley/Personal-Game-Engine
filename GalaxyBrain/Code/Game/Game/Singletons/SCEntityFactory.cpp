// Bradley Christensen - 2023
#include "SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
EntityDef const* SCEntityFactory::GetEntityDef(std::string const& name) const
{
    auto it = m_entityDefinitions.find(name);
    if (it != m_entityDefinitions.end())
    {
        return it->second;
    }
    return nullptr;
}
