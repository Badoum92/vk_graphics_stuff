#include "time.hh"

#include <GLFW/glfw3.h>

float Time::last_frame_ = 0.0f;
float Time::delta_time_ = 0.0f;
float Time::avg_ = 0.0f;
float Time::second_timer_ = 0.0f;
bool Time::second_ = false;
unsigned Time::fps_ = 0;
unsigned Time::fps_acc_ = 0;
float Time::total_ = 0.0f;

void Time::update()
{
    float current_frame = glfwGetTime();
    delta_time_ = current_frame - last_frame_;
    last_frame_ = current_frame;

    total_ += delta_time_;
    second_timer_ += delta_time_;
    ++fps_acc_;
    if (second_timer_ >= 1.0f)
    {
        second_timer_ -= 1.0f;
        second_ = true;
        fps_ = fps_acc_;
        fps_acc_ = 0;
    }
    else
    {
        second_ = false;
    }

    avg_ = (avg_ * 0.9f) + (delta_time_ * 0.1f);
}
