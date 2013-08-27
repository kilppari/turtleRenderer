/*
 * Timer class for counting elapsed milliseconds.
 *
 * Most of the code is straight from:
 * http://www.firstobject.com/getmillicount-milliseconds-portable-c++.htm
 */

#ifndef TIMER_H
#define TIMER_H

class Timer
{

private:
	int m_MilliTimer;
	int m_DeltaMilliseconds;
	int m_MilliCount;

	int getMilliCount();
	int getMilliSpan( int nTimeStart );

public:
	Timer();

    /* Reset timer to current time. */
	void reset() { m_MilliTimer = getMilliCount(); }

    /* Get elapsed time since last reset. */
	int getElapsed();

    /* Wrapper for getMilliCount. */
	int getTickCount() { return getMilliCount(); }

    /* Return elapsed time since previous use of getDelta(). */
	int getDelta();
};

#endif /* TIMER_H */
