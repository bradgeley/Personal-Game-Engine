// Bradley Christensen - 2022-2025
#include "SpriteAnimation.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SpriteAnimationDef::Init(SpriteAnimationType type, std::vector<int> const& frames, float secondsPerFrame)
{
	ASSERT_OR_DIE(!frames.empty(), "SpriteAnimation::Init - frames vector cannot be empty");
	ASSERT_OR_DIE(secondsPerFrame > 0.f, "SpriteAnimation::Init - secondsPerFrame cannot be negative");
	m_type = type;
	m_frames = frames;
	m_secondsPerFrame = secondsPerFrame;
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimation SpriteAnimationDef::MakeAnimInstance(int startingFrame /*= 0*/, int startingDirection /*= 1*/) const
{
    return SpriteAnimation(*this);
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimation::SpriteAnimation(SpriteAnimationDef const& def, int startingFrame /*= 0*/, int startingDirection /*= 1*/) : m_def(&def), m_currentDirection(startingDirection), m_currentFrame(startingFrame)
{
	ASSERT_OR_DIE(m_currentFrame >= 0 && m_currentFrame < (int) m_def->m_frames.size(), "SpriteAnimation::SpriteAnimation - startingFrame must be within the range of defined frames");
}



//----------------------------------------------------------------------------------------------------------------------
void SpriteAnimation::Update(float deltaSeconds)
{
    if (m_def->m_type == SpriteAnimationType::SingleFrame)
    {
        // Single frame animations do not update
        return;
	}

    m_timeAccumulated += deltaSeconds;

    if (m_timeAccumulated > m_def->m_secondsPerFrame)
    {
        m_timeAccumulated -= m_def->m_secondsPerFrame;

        if (m_def->m_type == SpriteAnimationType::PingPong)
        {
            if (m_currentFrame == 0)
            {
				m_currentDirection = 1;
            }
            else if (m_currentFrame == (int) m_def->m_frames.size() - 1)
            {
                m_currentDirection = -1;
            }
        }

        m_currentFrame = MathUtils::WrapInteger(m_currentFrame + m_currentDirection, 0, (int) m_def->m_frames.size() - 1);
    }
}

int SpriteAnimation::GetCurrentFrame() const
{
    return m_def->m_frames[m_currentFrame];
}
