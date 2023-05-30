// Bradley Christensen - 2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct Component
{
    virtual ~Component() = default;
    virtual Component* DeepCopy() const { return nullptr; }
};
