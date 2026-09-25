// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <array>
#include <cstdint>



struct EntityDebugContext;



//----------------------------------------------------------------------------------------------------------------------
// Flags that determine which ability features are currently enabled
// 
// When adding new: 
// - Update GetAllFlagNames()
//
enum class AbilityFlag : uint8_t
{
	Hit,
	Splash,
	Puddle,
	Count,
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityFlags
{
	AbilityFlags() = default;
	AbilityFlags(void const* xmlElement);

	bool HasFlag(AbilityFlag flag) const;

	inline bool CanHit() const { return HasFlag(AbilityFlag::Hit); }
	inline bool CanSplash() const { return HasFlag(AbilityFlag::Splash); }
	inline bool CanPuddle() const { return HasFlag(AbilityFlag::Puddle); }

	void AppendDebugString(EntityDebugContext& debugContext) const;

	static std::array<Name, static_cast<size_t>(AbilityFlag::Count)> GetAllFlagNames();

	uint8_t m_flags = 0;
};