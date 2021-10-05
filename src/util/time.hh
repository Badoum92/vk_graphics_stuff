#pragma once

#include <GLFW/glfw3.h>

class Time
{
public:
    static inline void update()
    {
        double current_frame = glfwGetTime();
        delta_time_ = current_frame - last_frame_;
        last_frame_ = current_frame;
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
    inline static double last_frame_ = 0;
    inline static double delta_time_ = 0;
};
