// Bradley Christensen - 2022-2025
#include "Engine/Debug/DevConsole.h"
#include "DebugDrawUtils.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Image/Image.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Window/Window.h"
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(AUDIO_SYSTEM_ENABLED)
#include "Engine/Audio/AudioSystem.h"
#endif



//----------------------------------------------------------------------------------------------------------------------
// THE Dev Console
//
DevConsole* g_devConsole = nullptr;



//----------------------------------------------------------------------------------------------------------------------
DevConsole::DevConsole(DevConsoleConfig const& config) : EngineSubsystem("DevConsole"), m_config(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{
	g_devConsole = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
class LoadDevConsoleBackgroundImageJob : public Job
{
public:
    
    LoadDevConsoleBackgroundImageJob(DevConsole* console, std::string const& path) : m_console(console), m_path(path) {}
    
    void Execute() override
    {
        m_texture = g_renderer->MakeTexture();
        m_image = g_assetManager->LoadSynchronous<Image>(m_path);
    }
    
    bool Complete() override
    {
        // Can only create textures in sync with the main thread
        Texture* texture = g_renderer->GetTexture(m_texture);
		Image* image = g_assetManager->Get<Image>(m_image);
        texture->CreateFromImage(*image);
        m_console->AddBackgroundImage(m_texture);
        return true;
    }

    AssetID m_image         = INVALID_ASSET_ID;
    TextureID m_texture     = RendererUtils::InvalidID;
    DevConsole* m_console   = nullptr;
    Name m_path             = Name::s_invalidName;
};



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
    LoadCommandHistory();
    
    g_window->m_charInputEvent.SubscribeMethod(this, &DevConsole::HandleChar);
    g_window->m_keyDownEvent.SubscribeMethod(this, &DevConsole::HandleKeyDown);
    g_window->m_keyUpEvent.SubscribeMethod(this, &DevConsole::HandleKeyUp);
    g_window->m_mouseButtonDownEvent.SubscribeMethod(this, &DevConsole::HandleMouseButtonDown);
    g_window->m_mouseButtonUpEvent.SubscribeMethod(this, &DevConsole::HandleMouseButtonUp);
    g_window->m_mouseWheelEvent.SubscribeMethod(this, &DevConsole::HandleMouseWheel);

    g_eventSystem->SubscribeMethod("clear", this, &DevConsole::Clear);
    g_eventSystem->SubscribeMethod("help", this, &DevConsole::Help);
    
    m_inputLine.SetOutputLog(&m_log);
    m_inputLine.m_commandEntered.SubscribeMethod(this, &DevConsole::OnCommandEnteredEvent);
    m_log.SetNumLines(m_config.m_logNumLines);

    m_camera = new Camera(Vec3::ZeroVector, Vec3(g_window->GetAspect(), 1.f, 1.f));
    g_window->m_windowSizeChanged.SubscribeMethod(this, &DevConsole::WindowSizeChanged);

    for (auto& backgroundImage : m_config.m_backgroundImages)
    {
        std::string path = "Data/Images/" + backgroundImage;
        LoadDevConsoleBackgroundImageJob* job = new LoadDevConsoleBackgroundImageJob(this, path);
        job->SetPriority(-99999); // Extremely low (negative) prio
        m_backgroundImageLoadingJobs.emplace_back(g_jobSystem->PostLoadingJob(job));
    }

    // Randomize the starting background image
    PickNextBackgroundImage();

    m_vbo = g_renderer->MakeVertexBuffer();

    m_log.Startup();
    m_commandHistory.Startup();
    m_inputLine.Startup();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
    m_shiftState.OnNextFrame();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Update(float deltaSeconds)
{
    float animSeconds = m_config.m_openCloseAnimationSpeed * deltaSeconds;
    m_openCloseAnimationFraction += (m_isShowing ? -animSeconds : animSeconds);
    m_openCloseAnimationFraction = MathUtils::Clamp(m_openCloseAnimationFraction, 0.f, 1.f);

    UpdateBackgroundImage(deltaSeconds);

    m_inputLine.Update(deltaSeconds);
} 



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Render() const
{
    float devConsoleOffset = MathUtils::SmoothStart3(m_openCloseAnimationFraction);

    // Sets camera and renderer pipeline state
    g_renderer->BeginCameraAndWindow(m_camera, g_window);

    // Translate by the animation fraction
    Mat44 modelMatrix = Mat44::CreateTranslation3D(0.f, devConsoleOffset);
    g_renderer->SetModelMatrix(modelMatrix);

    DrawTab();

    if (m_openCloseAnimationFraction == 1.f)
    {
        return;
    }

    DrawBackground();
    DrawText();

    if (m_commandHistory.IsActive())
    {
        DrawCommandHistory();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{
    SaveCommandHistory();
    
    g_window->m_charInputEvent.UnsubscribeMethod(this, &DevConsole::HandleChar);
    g_window->m_keyDownEvent.UnsubscribeMethod(this, &DevConsole::HandleKeyDown);
    g_window->m_keyUpEvent.UnsubscribeMethod(this, &DevConsole::HandleKeyUp);
    g_window->m_mouseButtonDownEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseButtonDown);
    g_window->m_mouseButtonUpEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseButtonUp);
    g_window->m_mouseWheelEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseWheel);

    g_eventSystem->UnsubscribeMethod("clear", this, &DevConsole::Clear);
    
    m_inputLine.m_commandEntered.UnsubscribeMethod(this, &DevConsole::OnCommandEnteredEvent);

    g_window->m_windowSizeChanged.UnsubscribeMethod(this, &DevConsole::WindowSizeChanged);

    for (TextureID bgdTex : m_backgroundImages)
    {
        g_renderer->ReleaseTexture(bgdTex);
    }
    m_backgroundImages.clear();

    delete m_camera;
    m_camera = nullptr;

    g_renderer->ReleaseVertexBuffer(m_vbo);
    m_log.Shutdown();
    m_commandHistory.Shutdown();
    m_inputLine.Shutdown();
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::Open()
{
    m_isShowing = true;
    #if defined(AUDIO_SYSTEM_ENABLED)
        if (g_audioSystem && !m_config.m_openSoundFilePath.empty())
        {
            SoundID id = g_audioSystem->PlaySoundFromFile(m_config.m_openSoundFilePath.c_str());
            if (id == AudioSystem::s_invalidSoundID)
            {
                LogErrorF("Dev console opening sound file invalid: %s", m_config.m_openSoundFilePath.c_str());
            }
        }
    #endif
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::Close()
{
    m_isShowing = false;
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::ToggleOpen()
{
    if (m_isShowing)
    {
        Close();
    }
    else
    {
        Open();
    }
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::Clear(NamedProperties&)
{
    m_log.Clear();
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::Help(NamedProperties& args)
{
    AddLine("Commands:");
    for (DevConsoleCommandInfo const& m_commandInfo : m_commandInfos)
    {
        AddLine(m_commandInfo.ToString(), Rgba8::Yellow);
    }
    AddLine("Events:");
    for (std::string const& event : g_eventSystem->GetAllEventNames())
    {
        AddLine(event, Rgba8::DarkGreen);
    }
    m_helpDelegate.Broadcast(args);
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::AddLine(std::string const& line, Rgba8 const& tint)
{
    DevConsoleLine devConsoleLine(line, tint);
    std::unique_lock lock(m_devConsoleMutex);
    m_log.AddLine(devConsoleLine);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::AddMultiLine(std::string const& line, Rgba8 const& tint)
{
    Strings lines = StringUtils::SplitStringOnDelimiter(line, '\n');
    
    for (auto& splitLine : lines)
    {
        if (splitLine == "")
        {
            continue;
        }
        DevConsoleLine devConsoleLine(splitLine, tint);
    
        std::unique_lock lock(m_devConsoleMutex);
        m_log.AddLine(devConsoleLine);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::AddBackgroundImage(TextureID backgroundImage)
{
    std::unique_lock lock(m_devConsoleMutex);
    m_backgroundImages.emplace_back(backgroundImage);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::AddDevConsoleCommandInfo(DevConsoleCommandInfo const& info)
{
    m_commandInfos.emplace_back(info);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::AddDevConsoleCommandInfo(Name eventName, Name argName, DevConsoleArgType argType)
{
    m_commandInfos.emplace_back(eventName, argName, argType);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::RemoveDevConsoleCommandInfo(Name commandName)
{
    for (auto it = m_commandInfos.begin(); it != m_commandInfos.end();)
    {
        DevConsoleCommandInfo& commandInfo = *it;
        if (commandInfo.m_commandName == commandName)
        {
            m_commandInfos.erase(it);
            break;
        }
        else
        {
            ++it;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
DevConsoleCommandInfo const* DevConsole::GetDevConsoleCommandInfo(Name commandName) const
{
    for (DevConsoleCommandInfo const& info : m_commandInfos)
    {
        if (info.m_commandName == commandName)
        {
            return &info;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
std::string DevConsole::GuessCommandInput(std::string const& input) const
{
    int bestMatching = 0;
    std::string bestValidGuess = "";

    int numInputChars = (int) input.size();

    Strings allEvents = g_eventSystem->GetAllEventNames();
    for (std::string const& eventName : allEvents)
    {
        int numMatchingChars = 0;
        for (int i = 0; i < numInputChars; ++i)
        {
            char inputChar = StringUtils::ToLowerChar(input[i]);
            char eventChar = StringUtils::ToLowerChar(eventName[i]);

            if (inputChar != eventChar)
            {
                break;
            }
            else
            {
                ++numMatchingChars;
            }
        }

        if (numMatchingChars == numInputChars && numMatchingChars > bestMatching)
        {
            bestMatching = numMatchingChars;
            bestValidGuess = eventName;
        }
    }

    if (!bestValidGuess.empty())
    {
        for (int i = 0; i < MathUtils::Min((int) input.size(), (int) bestValidGuess.size()); ++i)
        {
            bestValidGuess[i] = input[i];
        }
    }

    return bestValidGuess;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::LogSuccess(std::string const& line)
{
    AddLine(line, m_config.m_successTint);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::LogWarning(std::string const& line)
{
    AddLine(line, m_config.m_warningTint);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::LogError(std::string const& line)
{
    AddLine(line, m_config.m_errorTint);
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleChar(NamedProperties& args)
{
    int character = args.Get("Char", -1);
    if (character == '`')
    {
        return true;
    }

    if (m_isShowing)
    {
        if (m_commandHistory.IsActive())
        {
            m_commandHistory.Close();
        }
        
        // Only let you type these characters
        if (character >= 32 && character <= 126)
        {
            m_inputLine.InputChar((uint8_t) character);
        }
    }
    
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleKeyDown(NamedProperties& args)
{
    int key = args.Get("Key", -1);
    if (key == (uint8_t) KeyCode::Tilde)
    {
        ToggleOpen();
        return true;
    }

    if (key == (uint8_t) KeyCode::Escape)
    {
        Close();
        return true;
    }

    if (!m_isShowing)
    {
        return false;
    }
    
    if (key == (uint8_t) KeyCode::Shift)
    {
        m_shiftState.Press();
    }
    else if (key == (uint8_t) KeyCode::Enter)
    {
        m_inputLine.Enter();
        m_commandHistory.Close();
    }
    else if (key == (uint8_t) KeyCode::Delete)
    {
        m_inputLine.Delete();
    }
    else if (key == (uint8_t) KeyCode::Backspace)
    {
        m_inputLine.Backspace();
    }
    else if (key == (uint8_t) KeyCode::Left)
    {
        m_inputLine.MoveCaret(-1, m_shiftState.IsPressed());
    }
    else if (key == (uint8_t) KeyCode::Right)
    {
        m_inputLine.MoveCaret(1, m_shiftState.IsPressed());
    }
    else if (key == (uint8_t) KeyCode::Up)
    {
        m_commandHistory.ArrowUp();
        m_inputLine.SetLine(m_commandHistory.GetSelectedCommand());
        m_inputLine.MoveCaretToEndOfLine();
    }
    else if (key == (uint8_t) KeyCode::Down)
    {
        m_commandHistory.ArrowDown();
        m_inputLine.SetLine(m_commandHistory.GetSelectedCommand());
        m_inputLine.MoveCaretToEndOfLine();
    }
    else if (key == (uint8_t) KeyCode::Tab)
    {
        std::string guess = GuessCommandInput(m_inputLine.GetLine());
        if (!guess.empty())
        {
            m_inputLine.SetLine(guess);
            m_inputLine.MoveCaretToEndOfLine();
        }
    }
    
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleKeyUp(NamedProperties& args)
{
    int key = args.Get("Key", -1);
    if (key == (uint8_t) KeyCode::Shift)
    {
        m_shiftState.Release();
    }
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleMouseButtonDown(NamedProperties&)
{
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleMouseButtonUp(NamedProperties&)
{
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleMouseWheel(NamedProperties& args)
{
    int wheelChange = args.Get("WheelChange", 0);
    if (m_isShowing)
    {
        m_log.Scroll(wheelChange);
    }
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::OnCommandEnteredEvent(NamedProperties& args)
{
    std::string command = args.Get<std::string>("Command", "");
    Strings commandFragments = StringUtils::SplitStringOnDelimiter(command, ' ');

    // Remove empty fragments due to too much whitespace
    for (auto it = commandFragments.begin(); it != commandFragments.end();)
    {
        if (it->empty())
        {
            it = commandFragments.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Fire the event
    std::string eventNameString = commandFragments[0];
    StringUtils::TrimWhitespace(eventNameString);
    Name eventName = eventNameString;

    if (!g_eventSystem->IsEventBound(eventName))
    {
        g_devConsole->LogWarning("No events bound to that command.");
        return true;
    }

	// Add to command history (only if bound to an event)
    m_commandHistory.AddCommand(command);

    DevConsoleCommandInfo const* info = GetDevConsoleCommandInfo(eventName);

    NamedProperties eventProperties;

    // Start at arg index 1 because index 0 is the command name
    for (int i = 1; i < (int) commandFragments.size(); ++i)
    {
        auto& fragment = commandFragments[i];
        Strings keyValue = StringUtils::SplitStringOnDelimiter(fragment, '=');

        if (keyValue.size() == 1 || keyValue.size() == 2)
        {
            Name argName = Name::s_invalidName;
            std::string argValue;
            
            if (keyValue.size() == 1)
            {
                // User only put the value, assume there is a devConsoleInfo that explains what it should mean
                if (!info)
                {
                    g_devConsole->LogError("Arg name not specified, but DevConsoleCommandInfo does not exist for this command.");
                    continue;
                }
                int argIndex = i - 1; // -1 accounts for the command name
                if (argIndex >= (int) info->m_argNames.size())
                {
                    g_devConsole->LogError("Unexpected number of args found in DevConsoleCommandInfo for this command.");
                    continue;
                }
                argName = info->m_argNames[argIndex];
                argValue = keyValue[0];
            }
            else if (keyValue.size() == 2)
            {
                argName = keyValue[0];
                argValue = keyValue[1];
            }

            if (info)
            {
                DevConsoleArgType type = info->GetArgType(argName);
                switch (type)
                {
                    case DevConsoleArgType::Float:
                        eventProperties.Set(argName, StringUtils::StringToFloat(argValue));
                        break;
                    case DevConsoleArgType::Int:
                        eventProperties.Set(argName, StringUtils::StringToInt(argValue));
                        break;
                    case DevConsoleArgType::Bool:
                        eventProperties.Set(argName, StringUtils::StringToBool(argValue));
                        break;
                    default:
                        eventProperties.Set(argName, argValue);
                        break;
                }
            }
            else
            {
                eventProperties.Set(argName, argValue);
            }
        }
        else
        {
            LogErrorF("Invalid arg: %s", keyValue[0].c_str());
        }
    }

    int numResponders = EventUtils::FireEvent(eventName, eventProperties);
    
    if (numResponders > 0)
    {
        LogSuccessF("%i response(s) to event: %s", numResponders, eventName.ToCStr());
    }
    else
    {
        LogWarning(StringUtils::StringF("No response to event: %s", eventName.ToCStr()));
    }
    
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::WindowSizeChanged(NamedProperties&)
{
    m_camera->SetOrthoBounds(Vec3::ZeroVector, Vec3(g_window->GetAspect(), 1.f, 1.f));
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::UpdateBackgroundImage(float deltaSeconds)
{
    // Try to complete bgi loading jobs
    if (!m_backgroundImageLoadingJobs.empty())
    {
        // max 1 completion per frame
        if (g_jobSystem->CompleteJob(m_backgroundImageLoadingJobs[m_backgroundImageLoadingJobs.size() - 1], false))
        {
            m_backgroundImageLoadingJobs.erase(m_backgroundImageLoadingJobs.end() - 1);
        }
    }

    if (m_backgroundImages.size() <= 1)
    {
        return;
    }
    
    m_backgroundAnimationSeconds += deltaSeconds;
    
    switch (m_transitionState)
    {
    case EDevConsoleBGIState::Sustaining:
        if (m_backgroundAnimationSeconds > m_config.m_backgroundImageSustainSeconds)
        {
            m_backgroundAnimationSeconds -= m_config.m_backgroundImageSustainSeconds;
            m_transitionState = EDevConsoleBGIState::FadingOut;
        }
        break;
    case EDevConsoleBGIState::FadingIn:
        if (m_backgroundAnimationSeconds > m_config.m_backgroundImageFadeSeconds)
        {
            m_backgroundAnimationSeconds -= m_config.m_backgroundImageFadeSeconds;
            m_transitionState = EDevConsoleBGIState::Sustaining;
        }
        break;
    case EDevConsoleBGIState::FadingOut:
        if (m_backgroundAnimationSeconds > m_config.m_backgroundImageFadeSeconds)
        {
            m_backgroundAnimationSeconds -= m_config.m_backgroundImageFadeSeconds;
            PickNextBackgroundImage();
            m_transitionState = EDevConsoleBGIState::FadingIn;
        }
        break;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::DrawBackground() const
{
    AABB2 backgroundBox = m_camera->GetOrthoBounds2D();

    VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
    vbo.ClearVerts();

    VertexUtils::AddVertsForAABB2(vbo, backgroundBox, m_config.m_backgroundTint);
    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(m_vbo);

    // Reusing the same vbo for 2 draw calls - todo: make more efficient
    vbo.ClearVerts();
    
    if (m_backgroundImages.empty())
    {
        return;
    }

    TextureID currentBkg = m_backgroundImages[m_currentBackgroundImageIndex];
    Texture* texture = g_renderer->GetTexture(currentBkg);

    float alpha = GetBackgroundImageAlpha();
    
    auto imageDims = texture->GetDimensions();
    float imageAspect = imageDims.GetAspect();
    AABB2 imageBox = backgroundBox;
    
    float windowAspect = g_window->GetAspect();
    if (imageAspect < windowAspect)
    {
        imageBox.mins.x += (windowAspect - imageAspect);
        AABB2 fillerBox = backgroundBox;
        fillerBox.maxs.x = imageBox.mins.x;
        VertexUtils::AddVertsForAABB2(vbo, fillerBox, Rgba8(255,255,255,(uint8_t) (25.f * alpha)), AABB2(0.f, 0.f, 0.f, 1.f));
    }
    else
    {
        // high aspect images don't look nearly as good (lame)
        imageBox.maxs.y += (windowAspect - imageAspect);
        AABB2 fillerBox = backgroundBox;
        fillerBox.mins.y = imageBox.maxs.y;
        VertexUtils::AddVertsForAABB2(vbo, fillerBox, Rgba8(255,255,255,(uint8_t) (25.f * alpha)), AABB2(0.f, 1.f, 1.f, 1.f));
    }
    
    VertexUtils::AddVertsForAABB2(vbo, imageBox, Rgba8(255,255,255,(uint8_t) (25.f * alpha)));
    g_renderer->BindTexture(currentBkg);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::DrawTab() const
{
    if (m_openCloseAnimationFraction == 0.f)
    {
        return;
    }

    float tabSize = m_config.m_devConsoleTabSize;
    if (tabSize <= 0.f)
    {
        return;
    }

    AABB2 backgroundBox = m_camera->GetOrthoBounds2D();
    AABB2 tabDims;
    tabDims.maxs = backgroundBox.GetBottomRight();
    tabDims.mins = tabDims.maxs - tabSize * Vec2(0.07f, 0.01f);

    VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
    vbo.ClearVerts();

    VertexUtils::AddVertsForAABB2(vbo, tabDims, m_config.m_backgroundTint);
    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(vbo);

    vbo.ClearVerts();

    auto font = g_renderer->GetDefaultFont();
    font->AddVertsForAlignedText2D(vbo, tabDims.GetCenter(), Vec2::ZeroVector, tabDims.GetHeight(), "DevConsole (~)", Rgba8::White);
    font->SetRendererState();
    g_renderer->DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::DrawText() const
{
    float relativeLineThickness = m_config.m_inputLineThickness / (float) g_window->GetHeight();
    Vec2 inputLineMins = m_camera->GetOrthoBounds2D().mins;
    Vec2 inputLineMaxs = Vec2(m_camera->GetOrthoDimensions().x, relativeLineThickness);
    AABB2 inputLineBox(inputLineMins, inputLineMaxs);
    m_inputLine.RenderToBox(inputLineBox);

    Vec2 logMins = inputLineBox.mins + Vec2(0.f, relativeLineThickness);
    Vec2 logMaxs = m_camera->GetOrthoBounds2D().maxs;
    AABB2 logBox(logMins, logMaxs);
    m_log.RenderToBox(logBox);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::DrawCommandHistory() const
{
    float relativeLineThickness = m_config.m_inputLineThickness / (float) g_window->GetHeight();
    AABB2 inputLineBox(m_camera->GetOrthoBounds2D().mins, m_camera->GetOrthoBounds2D().mins + Vec2(1.f, relativeLineThickness));
    AABB2 historyBox(inputLineBox.GetTopLeft(), inputLineBox.maxs + Vec2(0.f, 0.3f));
    historyBox.Squeeze(0.005f);
    
    m_commandHistory.RenderToBox(historyBox);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::PickNextBackgroundImage()
{
    if (m_backgroundImages.size() <= 1)
    {
        return;
    }
    
    int startingIndex = m_currentBackgroundImageIndex;
    while (m_currentBackgroundImageIndex == startingIndex)
    {
        m_currentBackgroundImageIndex = g_rng->GetRandomIntInRange(0, (int) m_backgroundImages.size() - 1);
    }
}



//----------------------------------------------------------------------------------------------------------------------
float DevConsole::GetBackgroundImageAlpha() const
{
    float alpha = 1.f;
    
    switch (m_transitionState)
    {
    case EDevConsoleBGIState::FadingIn:
        alpha = m_backgroundAnimationSeconds / m_config.m_backgroundImageFadeSeconds;
        break;
    case EDevConsoleBGIState::FadingOut:
        alpha = (m_config.m_backgroundImageFadeSeconds - m_backgroundAnimationSeconds) / m_config.m_backgroundImageFadeSeconds;
        break;
    default:
        break;
    }

    if (m_backgroundImages.size() == 1)
    {
        // Hammer over animation stuff if there's only one image available
        alpha = 1.f;
    }

    return alpha;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::LoadCommandHistory()
{
    if (m_commandHistory.LoadFrom(m_config.m_commandHistoryFilePath))
    {
        LogSuccess("Loaded command history.");
    }
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::SaveCommandHistory() const
{
    ASSERT_OR_DIE(m_commandHistory.SaveTo(m_config.m_commandHistoryFilePath), "Failed to save dev console command history");
}
