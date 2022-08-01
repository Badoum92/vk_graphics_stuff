#pragma once

#include <bul/timer.h>

class Time
{
public:
    static inline void update()
    {
        delta_time_ = timer_.elapsed_s();
    }

    static inline double delta_time()
    {
        return delta_time_;
    }

    static inline unsigned fps()
    {
        return 1.0 / delta_time_;
    }

private:
    inline static bul::Timer timer_;
    inline static double delta_time_;
};
