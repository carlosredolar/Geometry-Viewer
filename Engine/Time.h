#ifndef _TIME_H_
#define _TIME_H_

#include "Timer.h"

struct ClockR
{
	ClockR();
	void Start();
	void Stop();
	void Pause();
	void Resume();
	void Reset();
	void Step();
	//time since startup
	float timeSinceStartup();

	float deltaTime(); //last frame expressed in seconds time
	float timeScale; //scale at which time is passing
	bool paused;
	bool started;
	float dt;
	Timer timer;
	Timer deltaTimer;
};

class Time {
public:
	static void Init();
	static int frameCount; //frames since game start

public:
	static ClockR realClock;
	static ClockR gameClock;
};

#endif