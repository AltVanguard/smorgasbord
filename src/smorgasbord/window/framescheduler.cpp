#include "framescheduler.hpp"

Smorgasbord::SyncStatus Smorgasbord::FrameScheduler::GetSyncStatus()
{
	switch (syncStatus)
	{
	case SyncStatus::ScheduleNextFrame: // Schedule next frame
	{
		switch (frameScheduleMode)
		{
		case FrameScheduleMode::EveryFrame:
			syncStatus = SyncStatus::ReadyToDraw;
			break;
			
		case FrameScheduleMode::OnDemand:
			if (nextScheduledFrame != 0.0)
			{
				syncStatus = SyncStatus::ReadyToDraw;
				nextScheduledFrame = 0.0;
			}
			break;
			
		case FrameScheduleMode::FadeOnDemand:
			if (nextScheduledFrame < -0.5)
			{
				syncStatus = SyncStatus::ReadyToDraw;
				invalidateTimer.Reset();
				invalidateTimer.Start();
			}
			
			if (invalidateTimer.IsRunning())
			{
				double elapsed = invalidateTimer.GetSeconds();
				
				if (elapsed > frameScheduleFadeTime)
				{
					nextScheduledFrame = 0.0;
					invalidateTimer.Stop();
				}
				
				if (elapsed >= nextScheduledFrame
						&& invalidateTimer.IsRunning())
				{
					syncStatus = SyncStatus::ReadyToDraw;
					
					double arg = glm::max(
						0.0,
						(elapsed / frameScheduleFadeTime) * 1.2 - 0.2);
					arg = arg * arg * arg;
					nextScheduledFrame = elapsed + arg;
				}
			}
			break;
		}
		
		if (syncStatus != SyncStatus::ReadyToDraw)
		{
			SDL_Delay(10);
		}
		
		break;
	}
		
	default:
		break;
	}
	
	return syncStatus;
}

void Smorgasbord::FrameScheduler::Signal(Smorgasbord::SyncStatus syncStatus)
{
	switch (syncStatus)
	{
	case SyncStatus::DrawInProgress:
	case SyncStatus::ScheduleNextFrame:
		this->syncStatus = syncStatus;
		break;
		
	default:
		LogE("Wrong signal");
		break;
	}
}
