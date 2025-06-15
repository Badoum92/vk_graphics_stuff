#pragma once

#include "core/math/vector.h"

struct window
{
    /*static void create(window* window, const char* title, vec2i size);
    void destroy();

    static void poll_events();

    void set_title(const char* title);
    float aspect_ratio() const;
    void show_cursor(bool show);*/

    bool should_close;
    bool resized;
    bool is_cursor_visible;
    vec2i size;
    vec2i cursor_position;
    vec2i visible_cursor_position;
    vec2i invisible_cursor_position;
    const char* title;

#if defined(_WIN32)
    void* handle;
#else
#error window not implemented for this platform
#endif
};

void window_create(window* window, const char* title, vec2i size);
void window_destroy(window* window);

void window_set_title(window* window, const char* title);
float window_aspect_ratio(const window* window);
void window_show_cursor(window* window, bool show);

void window_poll_events();
