#ifndef SMORGASBORD_FRAMESCHEDULER_HPP
#define SMORGASBORD_FRAMESCHEDULER_HPP

#include "smorgasbord/util/log.hpp"
#include "smorgasbord/util/timer.hpp"

#include <iostream>
#include <memory>

/// FrameScheduler is useful if you are running on batteries and you want to
/// hold back rendering to reduce battery consumption

namespace Smorgasbord {

enum class FrameScheduleMode
{
	/// Redraw at every opportunity regardless of invalidation
	EveryFrame,
	/// Only redraw if invalidated
	OnDemand,
	/// Redraw hen invalidated, then progressively increase the pause
	/// between draws and eventually halting until invalidated again
	FadeOnDemand,
};

class FrameScheduler
{
	FrameScheduleMode frameScheduleMode;
	Timer invalidateTimer;
	double nextScheduledFrame = -1.0;
	
public:
	FrameScheduler(
		FrameScheduleMode _scheduleMode = FrameScheduleMode::FadeOnDemand)
		: frameScheduleMode(_scheduleMode)
	{ }
	
	bool CanDraw();
	
	void Invalidate()
	{
		nextScheduledFrame = -1.0;
	}
	
	void SetFrameScheduleMode(FrameScheduleMode newMode)
	{
		frameScheduleMode = newMode;
		Invalidate();
	}
};

}

#endif // SMORGASBORD_FRAMESCHEDULER_HPP
