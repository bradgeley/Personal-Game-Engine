// Bradley Christensen - 2022-2025
#pragma once
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
class Clock
{
	friend struct ClockInternalData;

public:

	Clock();
	Clock(Clock const& other) = delete;
	explicit Clock(Clock* parentClock);
	~Clock();

	// Only call on parent-most clocks
	void Update();
	void Update(double deltaSeconds);

	double GetDeltaSeconds() const;
	float GetDeltaSecondsF() const;

	double GetCurrentTimeSeconds() const;
	float GetCurrentTimeSecondsF() const;

	double GetLocalTimeDilation() const;
	float GetLocalTimeDilationF() const;

	void SetLocalTimeDilation(double timeDilation);
	void GetAggregateTimeDilation(double& out_timeDilation) const;

	void SetPaused(bool isPaused);
	void TogglePaused();
	bool IsPaused() const;

	Clock* GetParentClock() const;
	Clock const* GetParentmostClock() const;
	std::vector<Clock*>& GetChildrenClocks();
	std::vector<Clock*> const& GetChildrenClocks() const;
	ClockInternalData* GetInternalData() const;
	bool AttachChildClock(Clock* childClock);
	bool Reparent(Clock* newParent);
	bool DetachChildClock(Clock* childClock);
	bool AttachToParent(Clock* parent);
	bool DetachFromParent();

protected:

	Clock* m_parentClock = nullptr;
	std::vector<Clock*> m_childClocks;

	double	m_deltaSeconds			= 0.0;
	double	m_currentTimeSeconds	= 0.0;
	double	m_timeDilation			= 1.0;
	bool	m_isPaused				= false;

private:

	ClockInternalData* m_internalData = nullptr; // Only for parent clocks, keeps std::chrono in cpp
};

