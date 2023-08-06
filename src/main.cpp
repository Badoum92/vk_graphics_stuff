#include <iostream>

#include "context.h"
#include "device.h"
#include "surface.h"

#include "renderer.h"
#include "path_tracing_renderer.h"

#include "bul/time.h"
#include "bul/window.h"
#include "bul/containers/pool.h"
#include "bul/math/vector.h"
#include "bul/util.h"
#include "bul/log.h"

int main(int, char**)
{
    try
    {
        bul::window::create("Window");
        vk::context::create();
        vk::device::create();
        vk::surface::create();
        auto renderer = Renderer::create();
        // auto renderer = PathTracingRenderer::create();
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

        vk::device::wait_idle();

        renderer.destroy();
        vk::surface::destroy();
        vk::device::destroy();
        vk::context::destroy();
        bul::window::destroy();
    }
    catch (const std::exception& e)
    {
        bul::log_error("Uncaught exception: %s\n", e.what());
    }
}
