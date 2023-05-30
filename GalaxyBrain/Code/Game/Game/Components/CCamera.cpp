// Bradley Christensen - 2023
#include "CCamera.h"



//----------------------------------------------------------------------------------------------------------------------
Component* CCamera::DeepCopy() const
{
    auto copy = new CCamera;
    *copy = *this;
    return copy;
}
