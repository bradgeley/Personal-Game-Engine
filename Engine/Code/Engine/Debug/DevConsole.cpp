// Bradley Christensen - 2023
#include "Engine/Debug/DevConsole.h"
#include "DebugDrawUtils.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/Image.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Dev Console
//
DevConsole* g_devConsole = nullptr;



//----------------------------------------------------------------------------------------------------------------------
DevConsole::DevConsole(DevConsoleConfig const& config) : m_config(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
class LoadDevConsoleBackgroundImageJob : public Job
{
public:
    
    LoadDevConsoleBackgroundImageJob(DevConsole* console, std::string const& path) : m_console(console), m_path(path) {}
    
    void Execute() override
    {
        m_texture = new Texture();
        if (m_image.LoadFromFile(m_path.data()))
        {
            g_devConsole->LogSuccess(StringF("Loaded image: %s", m_path.data()));
        }
        else
        {
            g_devConsole->LogError(StringF("Failed to load image: %s", m_path.data()));
        }
    }
    
    void Complete() override
    {
        // Can only create textures in sync with the main thread
        m_texture->CreateFromImage(m_image);
        m_console->AddBackgroundImage(m_texture); 
    }

    Image m_image;
    Texture* m_texture = nullptr;
    DevConsole* m_console;
    std::string m_path;
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
    m_inputLine.m_commandEntered.SubscribeMethod(this, &DevConsole::HandleCommandEntered);
    m_log.SetNumLines(m_config.m_logNumLines);

    m_camera.SetOrthoBounds(Vec3::ZeroVector, Vec3(g_window->GetAspect(), 1.f, 1.f));

    for (auto& backgroundImage : m_config.m_backgroundImages)
    {
        std::string path = "Data/Images/" + backgroundImage;
        LoadDevConsoleBackgroundImageJob* job = new LoadDevConsoleBackgroundImageJob(this, path);
        m_backgroundImageLoadingJobs.emplace_back(g_jobSystem->PostJob(job));
    }

    // Randomize the starting background image
    PickNextBackgroundImage();
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
    m_openCloseAnimationFraction = ClampF(m_openCloseAnimationFraction, 0.f, 1.f);

    UpdateBackgroundImage(deltaSeconds);

    m_inputLine.Update(deltaSeconds);
} 



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Render() const
{
    if (m_openCloseAnimationFraction == 1.f)
    {
        return;
    }

    float devConsoleOffset = SmoothStart3(m_openCloseAnimationFraction);

    // Sets camera and renderer pipeline state
    g_renderer->BeginWindow(g_window);
    g_renderer->BeginCamera(m_camera);

    // Translate by the animation fraction
    Mat44 modelMatrix;
    modelMatrix.Append(Mat44::CreateTranslation(0.f, devConsoleOffset));
    g_renderer->SetModelMatrix(modelMatrix);

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
    
    m_inputLine.m_commandEntered.UnsubscribeMethod(this, &DevConsole::HandleCommandEntered);

    for (auto& bgdTex : m_backgroundImages)
    {
        bgdTex->ReleaseResources();
        delete bgdTex;
        bgdTex = nullptr;
    }
    m_backgroundImages.clear();
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::Clear(NamedProperties& args)
{
    UNUSED(args)
    m_log.Clear();
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::Help(NamedProperties& args)
{
    m_helpDelegate.Broadcast(args);
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::AddLine(std::string const& line, Rgba8 const& tint)
{
    std::unique_lock lock(m_devConsoleMutex);
    m_log.AddLine({ line, tint });
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::AddBackgroundImage(Texture* backgroundImage)
{
    std::unique_lock lock(m_devConsoleMutex);
    m_backgroundImages.emplace_back(backgroundImage);
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
        m_isShowing = !m_isShowing;
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
bool DevConsole::HandleMouseButtonDown(NamedProperties& args)
{
    UNUSED(args)
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleMouseButtonUp(NamedProperties& args)
{
    UNUSED(args)
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
bool DevConsole::HandleCommandEntered(NamedProperties& args)
{
    std::string command = args.Get<std::string>("Command", "");
    Strings commandWords = SplitStringOnDelimeter(command, ' ');

    // Add to command history
    m_commandHistory.AddCommand(command);

    // Fire the event
    std::string eventName = GetToLower(commandWords[0]);
    int numResponders = FireEvent(eventName);
    
    if (numResponders == 0)
    {
        LogWarning(StringF("No response to event: %s", eventName.data()));
    }
    
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::UpdateBackgroundImage(float deltaSeconds)
{
    // Try to complete jobs
    if (!m_backgroundImageLoadingJobs.empty())
    {
        g_jobSystem->CompleteJobs(m_backgroundImageLoadingJobs, false);
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
    VertexBuffer backgroundVbo;
    auto& backgroundVerts = backgroundVbo.GetMutableVerts();
    AABB2 backgroundBox = m_camera.GetOrthoBounds2D();
    AddVertsForAABB2(backgroundVerts, backgroundBox, m_config.m_backgroundTint);
    g_renderer->DrawVertexBuffer(&backgroundVbo);
    
    if (m_backgroundImages.empty())
    {
        return;
    }

    Texture* currentBkg = m_backgroundImages[m_currentBackgroundImageIndex];
    float alpha = GetBackgroundImageAlpha();
    
    VertexBuffer imageVBO;    
    auto& imageVerts = imageVBO.GetMutableVerts();
    auto imageDims = currentBkg->GetDimensions();
    float imageAspect = imageDims.GetAspect();
    AABB2 imageBox = backgroundBox;
    
    float windowAspect = g_window->GetAspect();
    if (imageAspect < windowAspect)
    {
        imageBox.mins.x += (windowAspect - imageAspect);
        AABB2 fillerBox = backgroundBox;
        fillerBox.maxs.x = imageBox.mins.x;
        AddVertsForAABB2(imageVerts, fillerBox, Rgba8(255,255,255,(uint8_t) (25.f * alpha)), AABB2(0.f, 0.f, 0.f, 1.f));
    }
    else
    {
        // high aspect images don't look nearly as good (lame)
        imageBox.maxs.y += (windowAspect - imageAspect);
        AABB2 fillerBox = backgroundBox;
        fillerBox.mins.y = imageBox.maxs.y;
        AddVertsForAABB2(imageVerts, fillerBox, Rgba8(255,255,255,(uint8_t) (25.f * alpha)), AABB2(0.f, 1.f, 1.f, 1.f));
    }
    
    AddVertsForAABB2(imageVerts, imageBox, Rgba8(255,255,255,(uint8_t) (25.f * alpha)));
    g_renderer->BindTexture(currentBkg);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(&imageVBO);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::DrawText() const
{
    float relativeLineThickness = m_config.m_inputLineThickness / (float) g_window->GetHeight();
    Vec2 inputLineMins = m_camera.GetOrthoBounds2D().mins;
    Vec2 inputLineMaxs = Vec2(m_camera.GetOrthoDimensions().x, relativeLineThickness);
    AABB2 inputLineBox(inputLineMins, inputLineMaxs);
    m_inputLine.RenderToBox(inputLineBox);

    Vec2 logMins = inputLineBox.mins + Vec2(0.f, relativeLineThickness);
    Vec2 logMaxs = m_camera.GetOrthoBounds2D().maxs;
    AABB2 logBox(logMins, logMaxs);
    m_log.RenderToBox(logBox);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::DrawCommandHistory() const
{
    float relativeLineThickness = m_config.m_inputLineThickness / (float) g_window->GetHeight();
    AABB2 inputLineBox(m_camera.GetOrthoBounds2D().mins, m_camera.GetOrthoBounds2D().mins + Vec2(1.f, relativeLineThickness));
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
std::string DevConsole::GuessCommandInput(std::string const& input) const
{
    int bestMatching = 0;
    std::string bestGuess = "";
    
    int numInputChars = (int) input.size();

    std::string inputLower = GetToLower(input);
    
    Strings allEvents = g_eventSystem->GetAllEventNames();
    for (auto& event : allEvents)
    {
        int numEventChars = (int) event.size();
        
        int numMatchingChars = 0;
        for (int i = 0; i < Min(numEventChars, numInputChars); ++i)
        {
            if (inputLower[i] == event[i])
            {
                ++numMatchingChars;
            }
            if (numMatchingChars > bestMatching)
            {
                bestMatching = numMatchingChars;
                bestGuess = event;
            }
            if (inputLower[i] != event[i])
            {
                break;
            }
        }
    }
    
    return bestGuess;
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
    m_commandHistory.SaveTo(m_config.m_commandHistoryFilePath);
}
