// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Core/Name.h"
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
class SpriteAnimationDef
{
	friend class SpriteAnimation;

public:

	SpriteAnimationDef() = default;

	void LoadFromXml(void const* xmlElement);
	void Init(SpriteAnimationType type, std::vector<int> const& frames, float secondsPerFrame);

	Name GetName() const;
	SpriteAnimation MakeAnimInstance(int startingFrameIndex = 0, int startingDirection = 1) const;

protected:

	Name m_name;
	float m_secondsPerFrame = 0.f;
	std::vector<int> m_frames;
	SpriteAnimationType m_type = SpriteAnimationType::Looping;
};



//----------------------------------------------------------------------------------------------------------------------
// One instance of a sprite animation
//
class SpriteAnimation
{
public:

	SpriteAnimation() = default;
	SpriteAnimation(SpriteAnimationDef const& def, int startingFrame = 0, int startingDirection = 1);

	bool IsValid() const;
	void ChangeDef(SpriteAnimationDef const& newDef, bool restart = false);
	void Update(float deltaSeconds);
	int GetCurrentSpriteIndex() const;	// Which sprite index in the sprite sheet is currently being displayed
	int GetCurrentFrameIndex() const;	// Which frame index in the animation definition is currently being displayed
	int GetCurrentDirection() const;
	float GetDuration() const;
	float GetSecondsPerFrame() const;
	void SetSpeedMultiplier(float speedMultiplier);

protected:

	SpriteAnimationDef const* m_def = nullptr;

	int m_currentDirection		= 1;	// 1 for forward, -1 for backward
	int m_currentFrame			= 0;
	float m_speedMultiplier		= 1.f;
	float m_t					= 0.f;	// 0-1, representes the interpolation between the current frame and the next frame
};