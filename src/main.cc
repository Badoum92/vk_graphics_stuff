#include <iostream>

#include "window.hh"

#include "context.hh"
#include "device.hh"
#include "surface.hh"

#include "renderer.hh"
#include "path_tracing_renderer.hh"

#include "bul/time.h"
#include "bul/window.h"
#include "bul/containers/pool.h"
#include "bul/math/vector.h"
#include "bul/util.h"

int main(int, char**)
{
    try
    {
        bul::window::create("Window");
        auto context = vk::Context::create();
        auto device = vk::Device::create(context);
        auto surface = vk::Surface::create(context, device);
        // auto renderer = Renderer::create(context, device, surface);
        auto renderer = PathTracingRenderer::create(context, device, surface);
        renderer.init();

        while (!bul::window::should_close())
        {
            bul::time::update();
            bul::window::poll_events();

            if (bul::key_pressed(bul::Key::Escape))
            {
                bul::window::close();
            }

            if (bul::key_pressed(bul::Key::LAlt))
            {
                bul::window::show_cursor(!bul::window::cursor_visible());
            }

            renderer.render();
        }

        device.wait_idle();

        renderer.destroy();
        surface.destroy(context, device);
        device.destroy();
        context.destroy();
        bul::window::destroy();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Uncaught exception: " << e.what() << "\n";
    }
}
