// Bradley Christensen - 2022-2026
#pragma once
#include "GameState.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Assets/AssetID.h"



//----------------------------------------------------------------------------------------------------------------------
class MainMenuState : public GameState
{
public:

	MainMenuState();

	virtual void Enter(NamedProperties const& props) override;
	virtual void Exit(NamedProperties const& props) override;
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

	Camera m_camera;
	VertexBufferID m_untexturedVerts = RendererUtils::InvalidID;
	VertexBufferID m_textVerts = RendererUtils::InvalidID;
	AssetID m_fontID = AssetID::Invalid;
};