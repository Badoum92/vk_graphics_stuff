#include <iostream>

#include "window/window.hh"
#include "vk_context/vk_context.hh"
#include "time/time.hh"

int main(int, char**)
{
    try
    {
        Window::create(1280, 720, "Test Vulkan");
        VkContext::create();

        while (!Window::should_close())
        {
            Time::update();
            Window::poll_events();
            VkContext::draw_frame();
        }

        VkContext::wait_idle();

        VkContext::destroy();
        Window::destroy();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
