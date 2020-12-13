#include "Time.h"

ClockR Time::realClock;
ClockR Time::gameClock;

int Time::frameCount = 0;

void Time::Init()
{
	gameClock.timeScale = 0.0f;
	gameClock.started = false;
	
	realClock.timer.Start();
	realClock.started = true;
}


ClockR::ClockR()
{
	timeScale = 1.0f;
	paused = false;
}

void ClockR::Start()
{
	paused = false;
	started = true;
	timeScale = 1.0f;
	timer.Start();
}

void ClockR::Stop()
{
	paused = false;
	started = false;
}

void ClockR::Pause()
{
	paused = true;
	timeScale = 0.0f;
	timer.Stop();
}

void ClockR::Resume()
{
	paused = false;
	timeScale = 1.0f;
	timer.Resume();
}

void ClockR::Reset()
{
	timeScale = 1.0f;
	paused = false;
}

void ClockR::Step()
{
	dt = (float)deltaTimer.Read() / 1000 * timeScale;
	deltaTimer.Start();
}

float ClockR::timeSinceStartup()
{
	if (started)
		return timer.ReadSec();
	else
		return 0.0f;
}

float ClockR::deltaTime()
{
	return dt * timeScale;
}
