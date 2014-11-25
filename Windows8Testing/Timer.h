#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <iostream>

class Timer
{
protected:
    double m_SecsPerCount;
    double m_DeltaTime;

    __int64 m_BaseTime;
    __int64 m_PauseTime;
    __int64 m_StopTime;
    __int64 m_PrevTime;
    __int64 m_CurrTime;

    bool m_Stopped;
public:
    Timer();
    ~Timer();

    //float Timer()const;
    /* Name: DeltaTime()
    Info: DeltaTime will return the current value of m_DeltaTime member
    */
    float DeltaTime()const;

    /* Name: Reset()
    Info: Reset the timer. This function is to treat the current point in time, when we call this function, as our new base time.
    */
    void Reset();
    /* Name: Start()
    Info: This will start the time when it has been stopped. Not to be used for starting the timer when
    we create our timer object, as that is done during the constructor call. Also if resuming from a stop point
    in time, we calculate m_PauseTime in this function.
    */
    void Start();
    /* Name: Stop()
    Info: Stops the timer at current point in time. Takes current time whens stop was called.
    */
    void Stop();
    /* Name: Tick()
    Info: The timer update function, happens continuously.
    */
    void Tick();
    /* Name: TotalTime()
    Return: Float
    Info: Returns the total time passed since Reset() was last called.
    */
    float TotalTime()const;

};

