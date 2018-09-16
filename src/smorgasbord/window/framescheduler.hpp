#ifndef SMORGASBORD_FRAMESCHEDULER_HPP
#define SMORGASBORD_FRAMESCHEDULER_HPP

#include "smorgasbord/util/log.hpp"
#include "smorgasbord/util/timer.hpp"

#include <GL/glew.h>
#include <SDL.h>
#include <glm/glm.hpp>

#include <iostream>
#include <memory>

using namespace std;
using namespace glm;

namespace Smorgasbord {

enum class FrameScheduleMode
{
	EveryFrame,
	OnDemand,
	FadeOnDemand,
};

enum class SyncStatus
{
	ReadyToDraw,
	DrawInProgress,
	ReadyToSwap,
	ScheduleNextFrame,
};

class FrameScheduler
{
	static constexpr double frameScheduleFadeTime = 4.0;
	
	SyncStatus syncStatus = SyncStatus::ReadyToDraw;
	FrameScheduleMode frameScheduleMode;
	Timer invalidateTimer;
	double nextScheduledFrame = 0.0;
	
public:
	FrameScheduler(
		FrameScheduleMode _scheduleMode = FrameScheduleMode::FadeOnDemand)
		: frameScheduleMode(_scheduleMode)
	{ }
	
	SyncStatus GetSyncStatus();
	void Signal(SyncStatus syncStatus);
	
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

class FrameTimeCounter
{
	double accTime = 0;
	
	Timer frametimeDisplayTimer;
	int runs = 0;
	
public:
	void FinishFrame(double elapsedTime)
	{
		if (!frametimeDisplayTimer.IsRunning())
		{
			frametimeDisplayTimer.Start();
		}
		
		// Print frame time
		
		accTime += elapsedTime;
		runs++;
		if (frametimeDisplayTimer.GetSeconds() > 1.0 || runs >= 10000)
		{
			cout << (accTime / runs)
				<< "ms (" << (runs / (accTime)) << "fps)" << endl;
			accTime = 0;
			runs = 0;
			frametimeDisplayTimer.SetSeconds(0);
		}
	}
};

class GL4TimeQuery
{
	bool isInited = false;
	GLuint query = 0;
	GLuint64 elapsedTime = 0;
	
public:
	void Begin()
	{
		if (!isInited)
		{
			glGenQueries(1, &query);
			isInited = true;
		}
		
		glBeginQuery(GL_TIME_ELAPSED, query);
	}
	
	void End()
	{
		glEndQuery(GL_TIME_ELAPSED);
	}
	
	bool IsReady()
	{
		GLint queryDone = GL_FALSE;
		glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &queryDone);
		return queryDone == GL_TRUE;
	}
	
	double GetElapsedTime()
	{
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
		return elapsedTime / 1000000.0;
	}
};

}

#endif // SMORGASBORD_FRAMESCHEDULER_HPP
