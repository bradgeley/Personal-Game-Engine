﻿// Bradley Christensen - 2023
#include "CRender.h"



//----------------------------------------------------------------------------------------------------------------------
Component* CRender::DeepCopy() const
{
    auto copy = new CRender;
    *copy = *this;
    return copy;
}
