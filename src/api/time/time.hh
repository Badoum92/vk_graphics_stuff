#pragma once

class Time
{
public:
    static void update();

    static inline float delta_time()
    {
        return delta_time_;
    }

    static inline unsigned avg_fps()
    {
        return 1.0f / avg_;
    }

    static inline unsigned fps()
    {
        return fps_;
    }

    static inline bool is_second()
    {
        return second_;
    }

    static inline float total()
    {
        return total_;
    }

private:
    static float last_frame_;
    static float delta_time_;
    static float avg_;
    static float second_timer_;
    static bool second_;
    static unsigned fps_;
    static unsigned fps_acc_;
    static float total_;
};
