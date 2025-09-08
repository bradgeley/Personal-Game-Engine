// Bradley Christensen - 2022-2025
#include "SpriteAnimationGroup.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SpriteAnimationGroup::LoadFromXml(void const* xmlElement)
{
	XmlElement const* element = static_cast<XmlElement const*>(xmlElement);
	ASSERT_OR_DIE(element != nullptr, "SpriteAnimationDef::LoadFromXml - xmlElement is null");
	if (!element)
	{
		return;
	}

	m_name = XmlUtils::ParseXmlAttribute(*element, "name", Name::Invalid);

	XmlElement const* animationDefElem = element->FirstChildElement("SpriteAnimation");
	while (animationDefElem != nullptr)
	{
		size_t newIndex = m_animationDefs.size();
		m_animationDefs.emplace_back(SpriteAnimationDef());
		m_animationDefs[newIndex].LoadFromXml(animationDefElem);
		animationDefElem = animationDefElem->NextSiblingElement("SpriteAnimation");
	}
}



//----------------------------------------------------------------------------------------------------------------------
Name SpriteAnimationGroup::GetName() const
{
	return m_name;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<SpriteAnimationDef> const& SpriteAnimationGroup::GetAnimationDefs() const
{
	return m_animationDefs;
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimationDef const* SpriteAnimationGroup::GetBestAnimForDir(Vec2 const& direction, Vec2 const& tiebreakerDir) const
{
	float maxDotProduct = -1.f;
	SpriteAnimationDef const* bestAnim = nullptr;
	for (int i = 0; i < (int)m_animationDefs.size(); i++)
	{
		SpriteAnimationDef const& animDef = m_animationDefs[i];
		float dotProduct = direction.Dot(animDef.GetDirection());
		if (bestAnim && MathUtils::IsNearlyEqual(dotProduct, maxDotProduct))
		{
			float bestAnimDotProduct = tiebreakerDir.Dot(bestAnim->GetDirection());
			float animDefDotProduct = tiebreakerDir.Dot(animDef.GetDirection());
			if (animDefDotProduct > bestAnimDotProduct)
			{
				bestAnim = &animDef;
			}
		}
		else if (dotProduct > maxDotProduct)
		{
			maxDotProduct = dotProduct;
			bestAnim = &animDef;
		}
	}
	return bestAnim;
}
