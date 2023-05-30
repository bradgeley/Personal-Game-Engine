// Bradley Christensen - 2023
#include "CPhysics.h"



//----------------------------------------------------------------------------------------------------------------------
Component* CPhysics::DeepCopy() const
{
    auto copy = new CPhysics;
    *copy = *this;
    return copy;
}
