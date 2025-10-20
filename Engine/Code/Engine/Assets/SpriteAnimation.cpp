// Bradley Christensen - 2022-2025
#include "SpriteAnimation.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SpriteAnimationDef::LoadFromXml(void const* xmlElement)
{
	XmlElement const* element = static_cast<XmlElement const*>(xmlElement);
	ASSERT_OR_DIE(element != nullptr, "SpriteAnimationDef::LoadFromXml - xmlElement cannot be null");

	m_name = XmlUtils::ParseXmlAttribute(*element, "name", Name::Invalid);
	ASSERT_OR_DIE(m_name.IsValid(), "SpriteAnimationDef::LoadFromXml - Invalid name");

	m_direction = XmlUtils::ParseXmlAttribute(*element, "dir", Vec2::ZeroVector);
    if (!m_direction.IsZero())
    {
		m_direction.Normalize();
    }

	std::string typeStr = StringUtils::GetToLower(XmlUtils::ParseXmlAttribute(*element, "type", "Loop"));
    if (typeStr == "loop")
    {
        m_type = SpriteAnimationType::Loop;
    }
    else if (typeStr == "once")
    {
        m_type = SpriteAnimationType::Once;
    }
    else if (typeStr == "pingpong")
    {
        m_type = SpriteAnimationType::PingPong;
    }
    else if (typeStr == "singleframe")
    {
        m_type = SpriteAnimationType::SingleFrame;
    }
    else
    {
        ERROR_AND_DIE("SpriteAnimationDef::LoadFromXml - Invalid animation type");
	}

	m_secondsPerFrame = XmlUtils::ParseXmlAttribute(*element, "secondsPerFrame", 1.f);

	// Frames can be specified as a comma-separated list of integers or ranges of integers (e.g. "0,1,2,5-10")
	std::string framesStr = XmlUtils::ParseXmlAttribute(*element, "frames", "");
	Strings frameRangeTokens = StringUtils::SplitStringOnDelimiter(framesStr, ',');

    for (std::string const& range : frameRangeTokens)
    {
		Strings frameTokens = StringUtils::SplitStringOnDelimiter(range, '-');
        if (frameTokens.size() == 2)
        {
			// This is a range, e.g. "5-10"
			int frameMin = StringUtils::StringToInt(frameTokens[0]);
			int frameMax = StringUtils::StringToInt(frameTokens[1]);
            for (int i = frameMin; i <= frameMax; i++)
            {
                m_frames.push_back(i);
			}
        }
		else if (frameTokens.size() == 1)
        {
			// This is just a single frame csv, e.g. "3"
			int frame = StringUtils::StringToInt(frameTokens[0]);
			m_frames.push_back(frame);
        }
        else
        {
			ERROR_AND_DIE("SpriteAnimationDef::LoadFromXml - Invalid frame range format");
        }
    }
}



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
Name SpriteAnimationDef::GetName() const
{
    return m_name;
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimationType SpriteAnimationDef::GetType() const
{
    return m_type;
}



//----------------------------------------------------------------------------------------------------------------------
Name SpriteAnimationDef::GetAnimGroupName() const
{
    return m_groupName;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 const& SpriteAnimationDef::GetDirection() const
{
    return m_direction;
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimation SpriteAnimationDef::MakeAnimInstance(int startingFrameIndex /*= 0*/, int startingDirection /*= 1*/) const
{
	SpriteAnimation animInstance(*this, startingFrameIndex, startingDirection);
    return animInstance;
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimation::SpriteAnimation(SpriteAnimationDef const& def, int startingFrame /*= 0*/, int startingDirection /*= 1*/) : m_def(&def), m_currentDirection(startingDirection), m_currentFrame(startingFrame)
{
	ASSERT_OR_DIE(m_currentFrame >= 0 && m_currentFrame < (int) m_def->m_frames.size(), "SpriteAnimation::SpriteAnimation - startingFrame must be within the range of defined frames");
}



//----------------------------------------------------------------------------------------------------------------------
bool SpriteAnimation::IsValid() const
{
    return m_def != nullptr && !m_def->m_frames.empty();
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimationDef const& SpriteAnimation::GetDef() const
{
    return *m_def;
}



//----------------------------------------------------------------------------------------------------------------------
void SpriteAnimation::ChangeDef(SpriteAnimationDef const& newDef, bool restart /*= false*/)
{
	// todo: handle the case where the new definition has a different number of frames than the old one, and adjust the current frame accordingly
    m_def = &newDef;
    if (restart)
    {
		m_currentFrame = 0;
		m_t = 0.f;
    }
    else
    {
	    m_currentFrame = MathUtils::Clamp(m_currentFrame, 0, (int) m_def->m_frames.size() - 1);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SpriteAnimation::Update(float deltaSeconds)
{
    if (m_def->m_type == SpriteAnimationType::SingleFrame || m_def->m_frames.size() <= 1)
    {
        // Single frame animations do not update
        m_t += deltaSeconds / GetSecondsPerFrame();
        m_t = MathUtils::Clamp01(m_t);
        return;
	}

    if (m_def->m_type == SpriteAnimationType::Once && IsFinished())
    {
        // LoopOnce animations that are finished do not update
        return;
	}

    float secondsPerFrame = GetSecondsPerFrame();
    if (MathUtils::IsNearlyZero(secondsPerFrame))
    {
        // Animation either stopped or infinite speed
        return;
    }

	float duration = m_def->m_frames.size() * secondsPerFrame;
    deltaSeconds = MathUtils::Clamp(deltaSeconds, 0.f, duration);

    m_t += deltaSeconds / secondsPerFrame;

    while (m_t > 1.f)
    {
		// Because we clamp duration, should at most loop N times where N is the number of frames in the animation
        m_t -= 1.f;

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

            m_currentFrame = m_currentFrame + m_currentDirection;
        }
        else if (m_def->m_type == SpriteAnimationType::Loop)
        {
            if (m_currentDirection == 1)
            {
                m_currentFrame = MathUtils::IncrementIntInRange(m_currentFrame, 0, (int) m_def->m_frames.size() - 1, true);
            }
            else
            {
                m_currentFrame = MathUtils::DecrementIntInRange(m_currentFrame, 0, (int) m_def->m_frames.size() - 1, true);
            }
		}
        else if (m_def->m_type == SpriteAnimationType::Once)
        {
            if (IsOnLastFrame())
            {
                m_t = 1.f;
            }
            if (m_currentDirection == 1)
            {
                m_currentFrame = MathUtils::IncrementIntInRange(m_currentFrame, 0, (int) m_def->m_frames.size() - 1, false);
            }
            else
            {
                m_currentFrame = MathUtils::DecrementIntInRange(m_currentFrame, 0, (int) m_def->m_frames.size() - 1, false);
            }
		}
    }
}



//----------------------------------------------------------------------------------------------------------------------
int SpriteAnimation::GetCurrentSpriteIndex() const
{
    return m_def->m_frames[m_currentFrame];
}



//----------------------------------------------------------------------------------------------------------------------
int SpriteAnimation::GetCurrentFrameIndex() const
{
    return m_currentFrame;
}



//----------------------------------------------------------------------------------------------------------------------
int SpriteAnimation::GetCurrentDirection() const
{
    return m_currentDirection;
}



//----------------------------------------------------------------------------------------------------------------------
float SpriteAnimation::GetDuration() const
{
    return m_def->m_frames.size() * GetSecondsPerFrame();
}



//----------------------------------------------------------------------------------------------------------------------
float SpriteAnimation::GetSecondsPerFrame() const
{
    if (MathUtils::IsNearlyZero(m_speedMultiplier))
    {
        return 0.f;
    }
    return m_def->m_secondsPerFrame / m_speedMultiplier;
}



//----------------------------------------------------------------------------------------------------------------------
bool SpriteAnimation::IsFinished() const
{
    if (m_currentDirection == 1)
    {
        return (m_currentFrame == m_def->m_frames.size() - 1) && MathUtils::IsNearlyEqual(m_t, 1.f);
    }
    else
    {
        return (m_currentFrame == 0) && MathUtils::IsNearlyEqual(m_t, 1.f);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool SpriteAnimation::IsOnLastFrame() const
{
    if (m_currentDirection == 1)
    {
        return m_currentFrame == (int) m_def->m_frames.size() - 1;
    }
    else
    {
        return m_currentFrame == 0;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SpriteAnimation::SetSpeedMultiplier(float speedMultiplier)
{
    m_speedMultiplier = speedMultiplier;
}