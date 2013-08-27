/*
 * Timer.cpp
 * Implementation for Timer class.
 *
 * See header file for more info.
 */

#define _USE_32BIT_TIME_T

#include <sys/timeb.h>
#include "timer.h"

Timer::Timer()
{
	m_MilliTimer = getMilliCount();
	m_MilliCount = getMilliCount();
	m_DeltaMilliseconds = getMilliSpan( m_MilliCount );
}

int Timer::getMilliCount()
{
  // Something like GetTickCount but portable
  // It rolls over every ~ 12.1 days (0x100000/24/60/60)
  // Use GetMilliSpan to correct for rollover
  timeb tb;
  ftime( &tb );
  int nCount = tb.millitm + ( tb.time & 0xfffff ) * 1000;
  return nCount;
}

int Timer::getMilliSpan( int nTimeStart )
{
  int nSpan = getMilliCount() - nTimeStart;
  if ( nSpan < 0 )
    nSpan += 0x100000 * 1000;
  return nSpan;
}

int Timer::getElapsed()
{
    return getMilliSpan( m_MilliTimer );
}

int Timer::getDelta()
{
	m_DeltaMilliseconds = getMilliSpan( m_MilliCount );
	if( m_DeltaMilliseconds > 0 ) m_MilliCount = getMilliCount();
	return m_DeltaMilliseconds;
}
