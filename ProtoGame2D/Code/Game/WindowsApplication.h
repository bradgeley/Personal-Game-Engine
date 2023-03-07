// Bradley Christensen - 2022-2023
#pragma once



extern class WindowsApplication* g_theApp;



struct NamedProperties;
class Engine;
class Game;



//----------------------------------------------------------------------------------------------------------------------
// Windows Application
//
// The layer between Windows and the game. Contains an Engine, an ECS which can be configured
// based on the game, and a Game, which houses the ECS for the game.
//
class WindowsApplication
{
public:

    void Startup();
    void Run();
    void Shutdown();

    bool HandleQuit(NamedProperties& args);
    bool IsQuitting() const;
    
private:

    bool m_isQuitting = false;

    Engine* m_engine = nullptr;
    Game* m_game = nullptr;
};


