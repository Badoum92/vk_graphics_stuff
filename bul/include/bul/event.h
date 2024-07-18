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
        _mouse_click mouse_click_event;
        _key key_event;
        _mouse_move mouse_move_event;
    };

    event(mouse_button button, button_state state)
    {
        type = event_type::mouse_click;
        mouse_click_event = {button, state};
    }

    event(key key, button_state state)
    {
        type = event_type::key;
        key_event = {key, state};
    }

    event(int x, int y)
    {
        type = event_type::mouse_move;
        mouse_move_event = {x, y};
    }
};
} // namespace bul
