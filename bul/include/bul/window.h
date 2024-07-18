#pragma once

#include "bul/event.h"
#include "bul/math/vector.h"

namespace bul
{
struct window
{
    static void create(window* window, const char* title, vec2i size);
    void destroy();

    static void poll_events();

    void set_title(const char* title);
    float aspect_ratio() const;
    void show_cursor(bool show);

    bool should_close;
    bool resized;
    bool is_cursor_visible;
    vec2i size;
    vec2i cursor_position;
    vec2i visible_cursor_position;
    vec2i invisible_cursor_position;
    vec2i delta_cursor;
    const char* title;

#if defined(_WIN32)
    void* handle;
    void* raw_input;
    uint32_t raw_input_size;
#else
#error window not implemented for this platform
#endif
};
} // namespace bul
