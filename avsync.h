#ifndef AVSYNC_H
#define AVSYNC_H
#include <ctime>
#include <chrono>
#include <cmath>

using namespace std::chrono;

class AVSync
{
public:
    AVSync();

    void InitClock() {
        SetClock(NAN);
    }

    double GetClock() {
        double time = GetMicroseconds() / 1000000.0;//us->s
        return m_pts_drift + time;
    }

    void SetClockAt(double pts, double time) {
        m_pts = pts;
        m_pts_drift = pts - time;
    }

    void SetClock(double pts) {
        double time = GetMicroseconds() / 1000000.0;//us->s
        SetClockAt(pts, time);
    }

    time_t GetMicroseconds() {
        system_clock::time_point time_point_new = system_clock::now();
        system_clock::duration duration = time_point_new.time_since_epoch();
        time_t us = duration_cast<microseconds>(duration).count();
        return us;
    }

    double m_pts = 0;
    double m_pts_drift = 0;
};

#endif // AVSYNC_H
