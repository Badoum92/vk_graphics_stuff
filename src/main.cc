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

#include "bul/uuid.h"

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

void print_uuid(bul::UUID uuid)
{
    auto data = reinterpret_cast<const uint8_t*>(&uuid);
    size_t i = 0;
    for (; i < 4; ++i)
    {
        char a = (data[i] & 0xf0) >> 4;
        char b = (data[i] & 0x0f);
        printf("%x%x", a, b);
    }
    printf("-");
    for (; i < 6; ++i)
    {
        char a = (data[i] & 0xf0) >> 4;
        char b = (data[i] & 0x0f);
        printf("%x%x", a, b);
    }
    printf("-");
    for (; i < 8; ++i)
    {
        char a = (data[i] & 0xf0) >> 4;
        char b = (data[i] & 0x0f);
        printf("%x%x", a, b);
    }
    printf("-");
    for (; i < 10; ++i)
    {
        char a = (data[i] & 0xf0) >> 4;
        char b = (data[i] & 0x0f);
        printf("%x%x", a, b);
    }
    printf("-");
    for (; i < 16; ++i)
    {
        char a = (data[i] & 0xf0) >> 4;
        char b = (data[i] & 0x0f);
        printf("%x%x", a, b);
    }
    printf("\n");
}

int main(int, char**)
{
    const char* a = "123e4567-e89b-12d3-a456-426614174000";
    bul::UUID u{a};
    print_uuid(u);
    return 0;

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
