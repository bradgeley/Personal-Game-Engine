// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/Asset.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
enum class SpriteAnimationType
{
	None,
	Looping,
	PingPong,
	SingleFrame
};



//----------------------------------------------------------------------------------------------------------------------
// Shared defintion for a sprite animation
//
class SpriteAnimationDef : public IAsset
{
	friend class SpriteAnimation;

public:

	SpriteAnimationDef() = default;

	void Init(SpriteAnimationType type, std::vector<int> const& frames, float secondsPerFrame);

	SpriteAnimation MakeAnimInstance(int startingFrame = 0, int startingDirection = 1) const;

protected:

	SpriteAnimationType m_type = SpriteAnimationType::Looping;
	std::vector<int> m_frames;
	float m_secondsPerFrame = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
// One instance of a sprite animation
//
class SpriteAnimation
{
public:

	SpriteAnimation(SpriteAnimationDef const& def, int startingFrame = 0, int startingDirection = 1);

	void Update(float deltaSeconds);
	int GetCurrentFrame() const;

protected:

	SpriteAnimationDef const* m_def = nullptr;

	int m_currentDirection = 1; // 1 for forward, -1 for backward
	int m_currentFrame = 0;
	float m_timeAccumulated = 0.f;
};