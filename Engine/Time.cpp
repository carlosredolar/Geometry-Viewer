#include "Time.h"

rClock Time::realClock;
rClock Time::gameClock;

int Time::frameCount = 0;

void Time::Init()
{
	gameClock.timeScale = 0.0f;
	gameClock.started = false;
	
	realClock.timer.Start();
	realClock.started = true;
}


rClock::rClock()
{
	timeScale = 1.0f;
	paused = false;
}

void rClock::Start()
{
	paused = false;
	started = true;
	timeScale = 1.0f;
	timer.Start();
}

void rClock::Stop()
{
	paused = false;
	started = false;
}

void rClock::Pause()
{
	paused = true;
	timeScale = 0.0f;
	timer.Stop();
}

void rClock::Resume()
{
	paused = false;
	timeScale = 1.0f;
	timer.Resume();
}

void rClock::Reset()
{
	timeScale = 1.0f;
	paused = false;
}

void rClock::Step()
{
	dt = (float)deltaTimer.Read() / 1000 * timeScale;
	deltaTimer.Start();
}

float rClock::timeSinceStartup()
{
	if (started)
		return timer.ReadSec();
	else
		return 0.0f;
}

float rClock::deltaTime()
{
	return dt * timeScale;
}
