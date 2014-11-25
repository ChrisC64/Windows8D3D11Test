#include "Timer.h"


Timer::Timer() : m_SecsPerCount(0),
m_DeltaTime(0),
m_BaseTime(0),
m_CurrTime(0),
m_PauseTime(0),
m_PrevTime(0),
m_StopTime(0),
m_Stopped(false)
{
    // Begin timer at creation
    __int64 cntsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
    m_SecsPerCount = 1.0 / (double)cntsPerSec;
}


Timer::~Timer()
{
}


void Timer::Tick()
{
    if (m_Stopped)
    {
        m_DeltaTime = 0.0;
        return;
    }

    // Get the current time
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    m_CurrTime = currTime;

    // Find the Delta tie
    m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecsPerCount;

    // Prepare for next time frame
    m_PrevTime = m_CurrTime;

    // If delta time is converted to a negative number due to some reason, set it back to a positive value
    if (m_DeltaTime < 0.0)
    {
        m_DeltaTime = 0.0;
    }
    return;
}

float Timer::DeltaTime()const
{
    return(float)m_DeltaTime;
}

void Timer::Reset()
{
    // Grab current time 
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

    // Set base and current time members to the time we just received, reset other members to default values
    m_BaseTime = currTime;
    m_CurrTime = currTime;
    //m_PrevTime = currTime;
    m_StopTime = 0;
    m_Stopped = false;
}

void Timer::Stop()
{
    // If Stopped, do nothing
    if (m_Stopped)
    {
        return;
    }

    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    // Set current time to stop time
    m_StopTime = currTime;
    m_Stopped = false;
}

void Timer::Start()
{
    // Only call function if we are stopped, to start the timer up again, otherwise do nothing.
    // Calculate how long we were paused, only if we were stopped
    if (m_Stopped)
    {
        __int64 startTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

        // Accumulate the Pause Time
        m_PauseTime += (startTime - m_StopTime);

        // Start timer back up 
        m_PrevTime = startTime;

        // No longer remained stopped
        m_StopTime = 0;
        m_Stopped = false;
    }

    return;
}

float Timer::TotalTime()const
{
    // Check if we are currently stopped
    if (m_Stopped)
    {
        return(float)(((m_StopTime - m_PauseTime) - m_BaseTime) * m_SecsPerCount);
    }
    else
    {
        return(float)(((m_CurrTime - m_PauseTime) - m_BaseTime) * m_SecsPerCount);
    }
}