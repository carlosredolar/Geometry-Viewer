#include "Time.h"

Clock Time::realClock;
Clock Time::gameClock;

int Time::frameCount = 0;

void Time::Init()
{
	gameClock.timeScale = 0.0f;
	gameClock.started = false;
	
	realClock.timer.Start();
	realClock.started = true;
}


Clock::Clock()
{
	timeScale = 1.0f;
	paused = false;
}

void Clock::Start()
{
	paused = false;
	started = true;
	timeScale = 1.0f;
	timer.Start();
}

void Clock::Stop()
{
	paused = false;
	started = false;
}

void Clock::Pause()
{
	paused = true;
	timeScale = 0.0f;
	timer.Stop();
}

void Clock::Resume()
{
	paused = false;
	timeScale = 1.0f;
	timer.Resume();
}

void Clock::Reset()
{
	timeScale = 1.0f;
	paused = false;
}

void Clock::Step()
{
	dt = (float)deltaTimer.Read() / 1000 * timeScale;
	deltaTimer.Start();
}

float Clock::timeSinceStartup()
{
	if (started)
		return timer.ReadSec();
	else
		return 0.0f;
}

float Clock::deltaTime()
{
	return dt * timeScale;
}
