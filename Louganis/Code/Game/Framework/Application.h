// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// THE App
//
extern class Application* g_app;



struct NamedProperties;
class Game;



//----------------------------------------------------------------------------------------------------------------------
// Application
//
// The layer between the platform and the game. Contains an Engine, an ECS which can be configured
// based on the game, and a Game, which houses the ECS for the game.
//
class Application
{
public:

    virtual void Startup();
    virtual void Run();
    virtual void Shutdown();

    virtual void Quit();
    virtual bool HandleQuit(NamedProperties& args);
    virtual bool IsQuitting() const;
    
private:

    bool m_isQuitting = false;

    Game* m_game = nullptr;
};


