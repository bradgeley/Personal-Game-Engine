// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class AIMovementFlags : uint8_t
{
    IsWiggly = 1 << 0,
};



//----------------------------------------------------------------------------------------------------------------------
struct CAIController
{
public:

    CAIController() = default;
    CAIController(void const* xmlElement);

    bool GetIsMovementWiggly() const;
    void SetIsMovementWiggly(bool isWiggly);

public:

	uint8_t m_aiMovementFlags = 0;
};