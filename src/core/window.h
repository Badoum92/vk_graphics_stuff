#pragma once

#include "core/math/vector.h"

struct window
{
    bool should_close;
    bool resized;
    vec2i size;
    const char* title;

#if defined(_WIN32)
    void* handle;
#else
#error window not implemented for this platform
#endif
};

window* window_get_main_window();

void window_create(window* window, const char* title, vec2i size);
void window_destroy(window* window);

void window_set_title(window* window, const char* title);
float window_aspect_ratio(const window* window);

void window_poll_events();
