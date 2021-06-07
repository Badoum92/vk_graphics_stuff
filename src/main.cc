#include <iostream>

#include <memory>

#include "window/window.hh"
#include "time/time.hh"
#include "vk_context/vk_context.hh"

#include "test_app/test_app.hh"
#include "voxel_app/voxel_app.hh"

int main(int, char**)
{
    try
    {
        Window::create(1280, 720, "Test Vulkan");
        VkContext::create();

        {
            // TestApp app;
            VoxelApp app;
            while (!Window::should_close())
            {
                Time::update();
                Window::poll_events();
                app.update();
            }
            VkContext::wait_idle();
        }
        VkContext::destroy();
        Window::destroy();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
