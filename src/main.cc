#include <iostream>

#include "window.hh"
#include "time.hh"

#include "context.hh"
#include "device.hh"
#include "surface.hh"

#include "renderer.hh"
#include "path_tracing_renderer.hh"

#include "bul/window.h"
#include "bul/containers/pool.h"
#include "bul/math/vector.h"
#include "bul/util.h"

/* std::string to_string(const bul::Event& e)
{
    std::string s = bul::event_types_str[e.type];
    s += " ";
    switch (e.type)
    {
    case bul::EventType::MouseClick:
        s += bul::mouse_button_str[e.mouse_click.button];
        s += " ";
        s += bul::button_states_str[e.mouse_click.state];
        break;
    case bul::EventType::Key:
        s += bul::keys_str[e.key.key];
        s += " ";
        s += bul::button_states_str[e.key.state];
        break;
    case bul::EventType::MouseMove:
        s += "{";
        s += std::to_string(e.mouse_move.x);
        s += ", ";
        s += std::to_string(e.mouse_move.y);
        s += "}";
    default:
        return s;
    }
    return s;
} */

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
            Time::update();
            const auto& events = bul::window::poll_events();
            if (bul::key_pressed(bul::Key::Escape))
            {
                bul::window::close();
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
