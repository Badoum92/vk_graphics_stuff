#pragma once

class Time
{
public:
    static void update();

    static inline double delta_time()
    {
        return delta_time_;
    }

    static inline unsigned fps()
    {
        return 1.0 / delta_time_;
    }

private:
    static double last_frame_;
    static double delta_time_;
};
