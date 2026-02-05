// Bradley Christensen - 2022-2025
#include "SInput.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SInput::Startup()
{
    AddReadDependencies<InputSystem>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInput::Run(SystemContext const& context)
{

}
