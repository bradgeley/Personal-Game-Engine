// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



class VertexBuffer;
class Texture;



//----------------------------------------------------------------------------------------------------------------------
class SRender : public System
{
public:

    SRender(std::string const& name = "Render") : System(name) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

    static VertexBuffer* CreateOrGetVbo(std::string const& name);
    static Texture* CreateOrGetTexture(std::string const& name);
};
