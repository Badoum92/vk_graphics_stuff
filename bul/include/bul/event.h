#pragma once

#include "bul/input.h"

namespace bul
{
enum class event_type
{
    mouse_click,
    key,
    mouse_move,
    _count
};

// clang-format off
constexpr inline enum_array<event_type, const char*> event_types_str = {
    "MouseClick",
    "Key",
    "MouseMove"
};
// clang-format on

struct event
{
    struct _mouse_click
    {
        mouse_button button;
        button_state state;
    };

    struct _key
    {
        key key;
        button_state state;
    };

    struct _mouse_move
    {
        int x;
        int y;
    };

    event_type type;
    union
    {
        _mouse_click mouse_click;
        _key key;
        _mouse_move mouse_move;
    };

    event(mouse_button button, button_state state)
    {
        type = event_type::mouse_click;
        mouse_click = {.button = button, .state = state};
    }

    event(bul::key key_, bul::button_state state)
    {
        type = event_type::key;
        key = {key_, state};
    }

    event(int x, int y)
    {
        type = event_type::mouse_move;
        mouse_move = {x, y};
    }
};
} // namespace bul
