#include "framescheduler.hpp"

#include <glm/glm.hpp>

constexpr double fadeTime = 5.0;
constexpr double fadeExponent = 3.0;

bool Smorgasbord::FrameScheduler::CanDraw()
{
	bool canDraw = true;
	switch (frameScheduleMode)
	{
	case FrameScheduleMode::EveryFrame:
		canDraw = true;
		break;
		
	case FrameScheduleMode::OnDemand:
		canDraw = nextScheduledFrame != 0.0;
		if (nextScheduledFrame != 0.0)
		{
			nextScheduledFrame = 0.0;
		}
		break;
		
	case FrameScheduleMode::FadeOnDemand:
		canDraw = false;
		if (nextScheduledFrame < -0.5)
		{
			canDraw = true;
			invalidateTimer.Reset();
			invalidateTimer.Start();
		}
		
		if (invalidateTimer.IsRunning())
		{
			double elapsed = invalidateTimer.GetSeconds();
			
			if (elapsed > fadeTime)
			{
				nextScheduledFrame = 0.0;
				invalidateTimer.Stop();
			}
			
			if (elapsed >= nextScheduledFrame
					&& invalidateTimer.IsRunning())
			{
				canDraw = true;
				
				double arg = glm::max(
					0.0,
					(elapsed / fadeTime) * 1.2 - 0.2);
				arg = glm::pow(arg, fadeExponent);
				nextScheduledFrame = elapsed + arg;
			}
		}
		break;
	}

	return canDraw;
}
