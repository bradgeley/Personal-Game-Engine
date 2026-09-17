// Bradley Christensen - 2022-2026
#include "SpawnInfo.h"



//----------------------------------------------------------------------------------------------------------------------
bool SpawnInfo::AddSpawnTag(Name tag)
{
	if (m_numSpawnTags >= MAX_SPAWN_TAGS)
	{
		return false;
	}
    m_spawnTags[m_numSpawnTags] = tag;
    ++m_numSpawnTags;
    return true;
}
