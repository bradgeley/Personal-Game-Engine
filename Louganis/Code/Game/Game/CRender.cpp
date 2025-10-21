﻿// Bradley Christensen - 2022-2025
#include "CRender.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
CRender::CRender(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_renderRadius = XmlUtils::ParseXmlAttribute(elem, "scale", m_renderRadius);
    m_renderOffset = XmlUtils::ParseXmlAttribute(elem, "offset", m_renderOffset);

	bool rotateSprite = XmlUtils::ParseXmlAttribute(elem, "rotateSprite", false);
	SetRotateSprite(rotateSprite);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 CRender::GetRenderPosition() const
{
	return m_pos + (m_renderOffset * m_renderRadius * 0.5f);
}



//----------------------------------------------------------------------------------------------------------------------
float CRender::GetRenderOrientation() const
{
	if (GetRotateSprite())
	{
		return m_orientation;
	}
	else
	{
		return 0.f;
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool CRender::GetRotateSprite() const
{
	return (m_renderFlags & static_cast<uint8_t>(RenderFlags::RotateSprite)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool CRender::GetIsInCameraView() const
{
	return (m_renderFlags & static_cast<uint8_t>(RenderFlags::IsInCameraView)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void CRender::SetRotateSprite(bool rotate)
{
	if (rotate)
	{
		m_renderFlags |= static_cast<uint8_t>(RenderFlags::RotateSprite);
	}
	else
	{
		m_renderFlags &= ~static_cast<uint8_t>(RenderFlags::RotateSprite);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void CRender::SetIsInCameraView(bool isInView)
{
	if (isInView)
	{
		m_renderFlags |= static_cast<uint8_t>(RenderFlags::IsInCameraView);
	}
	else
	{
		m_renderFlags &= ~static_cast<uint8_t>(RenderFlags::IsInCameraView);
	}
}
