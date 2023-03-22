// Bradley Christensen - 2023
#pragma once
#include "DevConsoleInput.h"
#include "DevConsoleLog.h"
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Rgba8.h"
#include <string>
#include <mutex>

#include "Engine/Input/KeyButtonState.h"



struct JobID;
class Texture;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
// THE Dev Console
//
extern class DevConsole* g_devConsole;



//----------------------------------------------------------------------------------------------------------------------
struct DevConsoleConfig
{
	float m_inputLineThickness = 30.f;
	float m_logNumLines = 40.5f;
	float m_openCloseAnimationSpeed = 2.f;
	float m_backgroundImageSustainSeconds = 20.f;
	float m_backgroundImageFadeSeconds = 1.f;
	Rgba8 m_backgroundTint = Rgba8(0,0,0,225);
	Rgba8 m_errorTint = Rgba8::Red;
	Rgba8 m_warningTint = Rgba8::Yellow;
	Rgba8 m_successTint = Rgba8::Green;
	std::vector<std::string> m_backgroundImages;
};



//----------------------------------------------------------------------------------------------------------------------
// Dev Console
//
// Debug interface for calling functions in the game and engine.
//
class DevConsole : public EngineSubsystem
{
public:

	DevConsole(DevConsoleConfig const& config);
	virtual ~DevConsole() override = default;

	void Startup() override;
	void BeginFrame() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

	bool Clear(NamedProperties& args);

	void AddLine(std::string const& line, Rgba8 const& tint = Rgba8::LightBlue);
	void LogSuccess(std::string const& line);
	void LogWarning(std::string const& line);
	void LogError(std::string const& line);
	void AddBackgroundImage(Texture* backgroundImage);

protected:

	// Event Handlers - steals input from the game if the dev console is registered as a subsystem BEFORE InputSystem
	bool HandleChar(NamedProperties& args);
	bool HandleKeyDown(NamedProperties& args);
	bool HandleKeyUp(NamedProperties& args);
	bool HandleMouseButtonDown(NamedProperties& args);
	bool HandleMouseButtonUp(NamedProperties& args);
	bool HandleMouseWheel(NamedProperties& args);
	bool HandleCommandEntered(NamedProperties& args);

private:

	void UpdateBackgroundImage(float deltaSeconds);
	void DrawBackground() const;
	void DrawText() const;
	void PickNextBackgroundImage();

private:

	enum class EDevConsoleTransitionState
	{
		FadingIn,
		FadingOut,
		Sustaining,
	};
	
protected:
	
	DevConsoleConfig const m_config;

	std::mutex m_devConsoleMutex;
	DevConsoleInput m_inputLine;
	DevConsoleLog m_log;

	// DevConsole completely steals window events, so we need to track our own key button states Otherwise, we'd have
	// to have another way to disable the input system ONLY for the game but not DevConsole, which is awkward.
	KeyButtonState m_shiftState; 

	bool m_isShowing = false;
	Camera m_camera;

private:

	// Open/Close animation state. If 1.f, dev console is "closed"
	float m_openCloseAnimationFraction = 1.f;
	
	// Background image state
	EDevConsoleTransitionState m_transitionState = EDevConsoleTransitionState::Sustaining;
	int m_currentBackgroundImageIndex = 0;
	std::vector<Texture*> m_backgroundImages;
	std::vector<JobID> m_backgroundImageJobs;
	float m_backgroundAnimationSeconds = 0.f;
};





