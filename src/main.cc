#include <iostream>

#include "window.hh"
#include "time.hh"

#include "context.hh"
#include "device.hh"
#include "surface.hh"

#include "renderer.hh"
#include "path_tracing_renderer.hh"

int main(int, char**)
{
    try
    {
        Window::create(1280, 720, "Test Vulkan");
        auto context = vk::Context::create();
        auto device = vk::Device::create(context);
        auto surface = vk::Surface::create(context, device);
        // auto renderer = Renderer::create(context, device, surface);
        auto renderer = PathTracingRenderer::create(context, device, surface);
        renderer.init();

        while (!Window::should_close())
        {
            Time::update();
            Window::poll_events();
            renderer.render();
        }

        device.wait_idle();

        renderer.destroy();
        surface.destroy(context, device);
        device.destroy();
        context.destroy();
        Window::destroy();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Uncaught exception: " << e.what() << "\n";
    }
}
