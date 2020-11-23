#ifndef __TIMER_H__
#define __TIMER_H__

class Timer {

public:

	// Constructor
	Timer();

	void Start();
	void Stop();

	float Read();

private:

	bool	running;
	float	started_at;
	float	stopped_at;
};

#endif //__TIMER_H__