// Bradley Christensen - 2023
#include "CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
Component* CTransform::DeepCopy() const
{
    auto copy = new CTransform;
    *copy = *this;
    return copy;
}
