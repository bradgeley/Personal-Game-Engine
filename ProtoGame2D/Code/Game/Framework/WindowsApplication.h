// Bradley Christensen - 2022
#pragma once



class Engine;
class Game;



extern class WindowsApplication* g_theApp;



//----------------------------------------------------------------------------------------------------------------------
// Windows Application
//
// The layer between Windows and the game. Contains
class WindowsApplication
{
public:
    
    WindowsApplication();

    void Startup();
    void Run();
    void Shutdown();

    void Quit();
    bool IsQuitting() const;
    
private:

    bool m_isQuitting = false;

    Engine* m_engine = nullptr;
    Game* m_game = nullptr;
};


