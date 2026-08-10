// Bradley Christensen - 2022-2026
#pragma once
#include "GameState.h"
#include "SCRunData.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Assets/AssetID.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class RunState : public GameState
{
public:

	RunState();

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

	bool MissionOver(NamedProperties& props);

public:

	Camera m_camera;
	VertexBufferID m_untexturedVerts = RendererUtils::InvalidID;
	VertexBufferID m_textVerts = RendererUtils::InvalidID;
	AssetID m_fontID = AssetID::Invalid;
	SCRunData m_runData;
};