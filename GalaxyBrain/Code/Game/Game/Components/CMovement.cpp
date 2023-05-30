// Bradley Christensen - 2023
#include "CMovement.h"



//----------------------------------------------------------------------------------------------------------------------
Component* CMovement::DeepCopy() const
{
    auto copy = new CMovement;
    *copy = *this;
    return copy;
}