// Bradley Christensen - 2022-2025
#pragma once
#include <vector>



struct ClockInternalData;



//----------------------------------------------------------------------------------------------------------------------
class Clock
{
public:

	Clock();
	Clock(Clock const& other) = delete;
	explicit Clock(Clock* parentClock);
	~Clock();

	// Only call on parent-most clocks
	void Update();

	double GetCurrentTimeSeconds() const;
	float GetCurrentTimeSecondsF() const;

	double GetDeltaSeconds() const;
	float GetDeltaSecondsF() const;

	void SetTimeDilation(double timeDilation);
	void GetTimeDilation(double& out_timeDilation) const;

	double GetLocalTimeDilation() const;
	float GetLocalTimeDilationF() const;

	void AddChildClock(Clock* childClock);
	void RemoveChildClock(Clock* childClock);

protected:

	void UpdateInternal();

protected:

	Clock* m_parentClock = nullptr;
	std::vector<Clock*> m_childClocks;

	double m_deltaSeconds = 0.0;
	double m_currentTimeSeconds = 0.0;
	double m_timeDilation = 1.0;

private:

	ClockInternalData* m_internalData = nullptr; // Only for parent clocks, keeps std::chrono in cpp
};

