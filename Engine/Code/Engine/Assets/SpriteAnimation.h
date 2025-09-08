// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Core/Name.h"
#include "Engine/Math/Vec2.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
enum class SpriteAnimationType
{
	None,
	Loop,
	Once,
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
	Vec2 const& GetDirection() const;
	SpriteAnimation MakeAnimInstance(int startingFrameIndex = 0, int startingDirection = 1) const;

protected:

	Name m_name;											// "name" in xml.
	float m_secondsPerFrame = 0.f;							// "secondsPerFrame" in xml. How long each frame lasts in seconds
	Vec2 m_direction = Vec2::ZeroVector;					// "dir" in xml. Direction associated with this animation, e.g. NSEW
	std::vector<int> m_frames;								// "frames" in xml. List of sprite indices in the sprite sheet that make up this animation
	SpriteAnimationType m_type = SpriteAnimationType::Loop;	// "type" in xml. How the animation plays (loop, once, pingpong, single frame)
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
	SpriteAnimationDef const& GetDef() const;
	void ChangeDef(SpriteAnimationDef const& newDef, bool restart = false);
	void Update(float deltaSeconds);
	int GetCurrentSpriteIndex() const;	// Which sprite index in the sprite sheet is currently being displayed
	int GetCurrentFrameIndex() const;	// Which frame index in the animation definition is currently being displayed
	int GetCurrentDirection() const;
	float GetDuration() const;
	float GetSecondsPerFrame() const;
	bool IsFinished() const;			// Returns true if the animation is finished (only applies to LoopOnce animations)
	bool IsOnLastFrame() const;			
	void SetSpeedMultiplier(float speedMultiplier);

protected:

	SpriteAnimationDef const* m_def = nullptr;

	int m_currentDirection		= 1;	// 1 for forward, -1 for backward
	int m_currentFrame			= 0;
	float m_speedMultiplier		= 1.f;
	float m_t					= 0.f;	// 0-1, representes the interpolation between the current frame and the next frame
};