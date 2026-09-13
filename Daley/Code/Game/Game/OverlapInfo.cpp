// Bradley Christensen - 2022-2026
#include "OverlapInfo.h"



//----------------------------------------------------------------------------------------------------------------------
OverlapInfo::OverlapInfo(EntityID entityA, EntityID entityB)
{
	if (entityA < entityB)
	{
		m_key = (static_cast<uint64_t>(entityA.GetRawData()) << 32) | entityB.GetRawData();
	}
	else
	{
		m_key = (static_cast<uint64_t>(entityB.GetRawData()) << 32) | entityA.GetRawData();
	}
}
