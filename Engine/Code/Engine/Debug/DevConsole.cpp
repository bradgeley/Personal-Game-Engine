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
class LoadTextureJob : public Job
{
public:
    
    LoadTextureJob(DevConsole* console, std::string const& path) : m_console(console), m_path(path) {}
    
    void Execute() override
    {
        m_texture = new Texture();
        if (m_image.LoadFromFile(m_path.data()))
        {
            // TODO: Calling renderer functions from another thread is NOT safe
        }
    }
    
    bool NeedsComplete() override { return true; }
    
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
    g_window->m_charInputEvent.SubscribeMethod(this, &DevConsole::HandleChar);
    g_window->m_keyDownEvent.SubscribeMethod(this, &DevConsole::HandleKeyDown);
    g_window->m_keyUpEvent.SubscribeMethod(this, &DevConsole::HandleKeyUp);
    g_window->m_mouseButtonDownEvent.SubscribeMethod(this, &DevConsole::HandleMouseButtonDown);
    g_window->m_mouseButtonUpEvent.SubscribeMethod(this, &DevConsole::HandleMouseButtonUp);
    g_window->m_mouseWheelEvent.SubscribeMethod(this, &DevConsole::HandleMouseWheel);
    
    m_inputLine.SetOutputLog(&m_log);
    m_inputLine.m_commandEntered.SubscribeMethod(this, &DevConsole::HandleCommandEntered);
    m_log.SetNumLines(m_config.m_logNumLines);

    for (auto& backgroundImage : m_config.m_backgroundImages)
    {
        std::string path = "Data/Images/" + backgroundImage;
        LoadTextureJob* job = new LoadTextureJob(this, path);
        m_backgroundImageJobs.emplace_back(g_jobSystem->PostJob(job));
    }

    // Randomize the starting background image
    PickNextBackgroundImage();
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
    g_renderer->BeginCamera(m_camera);

    // Translate by the animation fraction
    Mat44 modelMatrix;
    modelMatrix.Append(Mat44::CreateTranslation(0.f, devConsoleOffset));
    g_renderer->SetModelMatrix(modelMatrix);

    DrawBackground();
    DrawText();
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{
    g_window->m_charInputEvent.UnsubscribeMethod(this, &DevConsole::HandleChar);
    g_window->m_keyDownEvent.UnsubscribeMethod(this, &DevConsole::HandleKeyDown);
    g_window->m_keyUpEvent.UnsubscribeMethod(this, &DevConsole::HandleKeyUp);
    g_window->m_mouseButtonDownEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseButtonDown);
    g_window->m_mouseButtonUpEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseButtonUp);
    g_window->m_mouseWheelEvent.UnsubscribeMethod(this, &DevConsole::HandleMouseWheel);
    
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
void DevConsole::AddLine(std::string const& line, Rgba8 const& tint)
{
    m_log.AddLine({ line, tint });
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::AddBackgroundImage(Texture* backgroundImage)
{
    m_backgroundImages.emplace_back(backgroundImage);
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
    if (key == (uint8_t) KeyCode::Enter)
    {
        m_inputLine.Enter();
    }
    else if (key == (uint8_t) KeyCode::Delete)
    {
        m_inputLine.Delete();
    }
    else if (key == (uint8_t) KeyCode::Backspace)
    {
        m_inputLine.Backspace();
    }
    
    return m_isShowing;
}



//----------------------------------------------------------------------------------------------------------------------
bool DevConsole::HandleKeyUp(NamedProperties& args)
{
    UNUSED(args)
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
    UNUSED(args)
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsole::UpdateBackgroundImage(float deltaSeconds)
{
    // Try to complete jobs
    g_jobSystem->CompleteJobs(m_backgroundImageJobs);
    
    m_backgroundAnimationSeconds += deltaSeconds;
    
    switch (m_transitionState)
    {
    case EDevConsoleTransitionState::Sustaining:
        if (m_backgroundAnimationSeconds > m_config.m_backgroundImageSustainSeconds)
        {
            m_backgroundAnimationSeconds -= m_config.m_backgroundImageSustainSeconds;
            m_transitionState = EDevConsoleTransitionState::FadingOut;
        }
        break;
    case EDevConsoleTransitionState::FadingIn:
        if (m_backgroundAnimationSeconds > m_config.m_backgroundImageFadeSeconds)
        {
            m_backgroundAnimationSeconds -= m_config.m_backgroundImageFadeSeconds;
            m_transitionState = EDevConsoleTransitionState::Sustaining;
        }
        break;
    case EDevConsoleTransitionState::FadingOut:
        if (m_backgroundAnimationSeconds > m_config.m_backgroundImageFadeSeconds)
        {
            m_backgroundAnimationSeconds -= m_config.m_backgroundImageFadeSeconds;
            PickNextBackgroundImage();
            m_transitionState = EDevConsoleTransitionState::FadingIn;
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
    float alpha = 1.f;
    switch (m_transitionState)
    {
    case EDevConsoleTransitionState::FadingIn:
        alpha = m_backgroundAnimationSeconds / m_config.m_backgroundImageFadeSeconds;
        break;
    case EDevConsoleTransitionState::FadingOut:
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
    

    
    VertexBuffer imageVBO;    
    auto& imageVerts = imageVBO.GetMutableVerts();
    auto imageDims = currentBkg->GetDimensions();
    float imageAspect = imageDims.GetAspect();
    AABB2 imageBox = backgroundBox;
    if (imageAspect < 1.f)
    {
        imageBox.mins.x += (1.f - imageAspect);
        AABB2 fillerBox = backgroundBox;
        fillerBox.maxs.x = imageBox.mins.x;
        AddVertsForAABB2(imageVerts, fillerBox, Rgba8(255,255,255,(uint8_t) (25.f * alpha)), AABB2(0.f, 0.f, 0.f, 1.f));
    }
    else
    {
        // high aspect images don't look nearly as good (lame)
        imageBox.maxs.y += (1.f - imageAspect);
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
    AABB2 inputLineBox(m_camera.GetOrthoBounds2D().mins, m_camera.GetOrthoBounds2D().mins + Vec2(1.f, relativeLineThickness));
    m_inputLine.RenderToBox(inputLineBox);

    Vec2 logMins = inputLineBox.mins + Vec2(0.f, relativeLineThickness);
    Vec2 logMaxs = m_camera.GetOrthoBounds2D().maxs;
    AABB2 logBox(logMins, logMaxs);
    m_log.RenderToBox(logBox);
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
