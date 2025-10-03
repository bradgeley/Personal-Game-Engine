// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/AssetID.h"
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Input/KeyButtonState.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Renderer/Rgba8.h"
#include "DevConsoleCommandHistory.h"
#include "DevConsoleCommandInfo.h"
#include "DevConsoleInput.h"
#include "DevConsoleLog.h"
#include <vector>
#include <string>
#include <mutex>



class Camera;
class Texture;
struct JobID;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
// THE Dev Console
//
extern class DevConsole* g_devConsole;



//----------------------------------------------------------------------------------------------------------------------
struct DevConsoleConfig
{
	std::string m_commandHistoryFilePath = "Temp/Debug/DevConsoleCommandHistory.txt";
	std::string m_openSoundFilePath;
	
	// All visual toggles for dev console
	float m_inputLineThickness				= 30.f;
	float m_logNumLines						= 40.5f;
	float m_openCloseAnimationSpeed			= 2.f;
	float m_backgroundImageSustainSeconds	= 30.f;
	float m_backgroundImageFadeSeconds		= 1.f;
	Rgba8 m_backgroundTint					= Rgba8(0,0,0,225);
	Rgba8 m_errorTint						= Rgba8::Red;
	Rgba8 m_warningTint						= Rgba8::Yellow;
	Rgba8 m_successTint						= Rgba8::Green;
	float m_devConsoleTabSize				= 1.f; // 0 to disable
	std::vector<std::string> m_backgroundImages;
};



//----------------------------------------------------------------------------------------------------------------------
// Dev Console
//
// Debug interface for calling functions in the game and engine.
//
// Register as a subsystem after the Window, but before the InputSystem
//
class DevConsole : public EngineSubsystem
{
public:

	DevConsole(DevConsoleConfig const& config);
	virtual ~DevConsole() override;

	DevConsoleConfig const& GetConfig() const;

	void Startup() override;
	void BeginFrame() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

	bool Open();
	bool Close();
	bool ToggleOpen();

	bool Clear(NamedProperties& args);
	bool Help(NamedProperties& args);

	void AddLine(std::string const& line, Rgba8 const& tint = Rgba8::LightBlue);
	void AddMultiLine(std::string const& line, Rgba8 const& tint = Rgba8::LightBlue);
	void AddBackgroundImage(TextureID backgroundImage);

	void AddDevConsoleCommandInfo(DevConsoleCommandInfo const& info);
	void AddDevConsoleCommandInfo(Name eventName, Name argName, DevConsoleArgType argType);
	void RemoveDevConsoleCommandInfo(Name commandName);
	DevConsoleCommandInfo const* GetDevConsoleCommandInfo(Name commandName) const;
	std::string GuessCommandInput(std::string const& input) const;
	std::vector<std::string> GetTopInputGuesses(std::string const& input, int maxGuesses = 10) const;
	
	void LogSuccess(std::string const& line);
	void LogWarning(std::string const& line);
	void LogError(std::string const& line);

	template<typename ...Args>
	void LogF(Rgba8 tint, char const* format, Args... args);
	template<typename ...Args>
	void LogSuccessF(char const* format, Args... args);
	template<typename ...Args>
	void LogWarningF(char const* format, Args... args);
	template<typename ...Args>
	void LogErrorF(char const* format, Args... args);

protected:

	// Event Handlers
	// - steals input from the game if the dev console is registered as a subsystem BEFORE InputSystem
	bool HandleChar(NamedProperties& args);
	bool HandleKeyDown(NamedProperties& args);
	bool HandleKeyUp(NamedProperties& args);
	bool HandleMouseButtonDown(NamedProperties& args);
	bool HandleMouseButtonUp(NamedProperties& args);
	bool HandleMouseWheel(NamedProperties& args);
	bool OnCommandEnteredEvent(NamedProperties& args);
	bool WindowSizeChanged(NamedProperties& args);

private:

	void UpdateBackgroundImage(float deltaSeconds);
	void DrawBackground() const;
	void DrawTab() const;
	void DrawText() const;
	void DrawCommandHistory() const;
	void PickNextBackgroundImage();
	float GetBackgroundImageAlpha() const;
	
	void LoadCommandHistory();
	void SaveCommandHistory() const;

private:

	enum class EDevConsoleBGIState // BGI: Background Image
	{
		FadingIn,
		FadingOut,
		Sustaining,
	};

public:

	EventDelegate m_helpDelegate;
	
protected:
	
	DevConsoleConfig const m_config;

	std::mutex m_devConsoleMutex;
	DevConsoleInput m_inputLine;
	DevConsoleLog m_log;
	DevConsoleCommandHistory m_commandHistory;
	std::vector<DevConsoleCommandInfo> m_commandInfos;

	// DevConsole completely steals window events, so we need to track our own key button states Otherwise, we'd have
	// to have another way to disable the input system ONLY for the game but not DevConsole, which is awkward. Yay for events...
	// todo: fix this
	KeyButtonState m_shiftState; 
	KeyButtonState m_ctrlState; 

	bool m_isShowing = false;
	Camera* m_camera = nullptr;

private:

	// Open/Close animation state. If 1.f, dev console is "closed"
	float m_openCloseAnimationFraction = 1.f;
	
	// Background image state
	EDevConsoleBGIState m_transitionState = EDevConsoleBGIState::Sustaining;
	int m_currentBackgroundImageIndex = 0;
	float m_backgroundAnimationSeconds = 0.f;
	std::vector<AssetID> m_backgroundImages;

	// Vertex Buffers
	VertexBufferID m_vbo = RendererUtils::InvalidID;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename ...Args>
inline void DevConsole::LogF(Rgba8 tint, char const* format, Args ...args)
{
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), format, args...);
	AddLine(buffer, tint);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename ... Args>
void DevConsole::LogSuccessF(char const* format, Args... args)
{
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), format, args...);
	AddLine(buffer, m_config.m_successTint);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename ... Args>
void DevConsole::LogWarningF(char const* format, Args... args)
{
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), format, args...);
	AddLine(buffer, m_config.m_warningTint);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename ... Args>
void DevConsole::LogErrorF(char const* format, Args... args)
{
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), format, args...);
	AddLine(buffer, m_config.m_errorTint);
}