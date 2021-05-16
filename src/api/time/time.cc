#include "time.hh"

#include <GLFW/glfw3.h>

double Time::last_frame_ = 0.0f;
double Time::delta_time_ = 0.0f;

void Time::update()
{
    double current_frame = glfwGetTime();
    delta_time_ = current_frame - last_frame_;
    last_frame_ = current_frame;
}
