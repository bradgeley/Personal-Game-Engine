// Bradley Christensen - 2022-2026
#include "CPlaceable.h"
#include "Tile.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CPlaceable::CPlaceable(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	m_dims = XmlUtils::ParseXmlAttribute(elem, "dims", IntVec2::OneVector);
	bool canPlaceOnPathTiles = XmlUtils::ParseXmlAttribute(elem, "onPath", false);
	bool canPlaceOnSolidTiles = XmlUtils::ParseXmlAttribute(elem, "onSolid", false);

	m_tileTagQuery.m_queryOp = TagQueryOp::DoesNotHaveAny;
	if (!canPlaceOnPathTiles)
	{
		m_tileTagQuery.m_tagsToQuery |= (uint8_t) TileTag::IsPath;
	}
	if (!canPlaceOnSolidTiles)
	{
		m_tileTagQuery.m_tagsToQuery |= (uint8_t) TileTag::Solid;
	}
}
