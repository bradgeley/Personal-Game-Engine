// Bradley Christensen - 2022-2026
#include "SCCollision.h"



//----------------------------------------------------------------------------------------------------------------------
void SCCollision::ForEachEnemyInTile(int tileIndex, std::function<bool(EntityID)> const& callback) const
{
	if (m_sortedEnemies.empty())
	{
		return;
	}

	if (m_enemyBucketIndices.empty())
	{
		return;
	}

	int startingIndex = m_enemyBucketIndices[tileIndex];
	if (startingIndex == -1)
	{
		return;
	}

	int endingIndex = (tileIndex + 1 < (int) m_enemyBucketIndices.size()) ? m_enemyBucketIndices[tileIndex + 1] : (int) m_sortedEnemies.size();

	for (int i = startingIndex; i < endingIndex; ++i)
	{
		EntityID entityID = m_sortedEnemies[i];
		if (!callback(entityID))
		{
			break;
		}
	}
}
