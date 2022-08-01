#pragma once

#include "bul/input.h"

namespace bul
{
enum class EventType
{
    MouseClick,
    Key,
    MouseMove,
    Count
};

// clang-format off
constexpr inline EnumArray<EventType, const char*> event_types_str = {
    "MouseClick",
    "Key",
    "MouseMove"
};
// clang-format on

struct Event
{
    struct MouseClick_
    {
        MouseButton button;
        ButtonState state;
    };

    struct Key_
    {
        Key key;
        ButtonState state;
    };

    struct MouseMove_
    {
        int x;
        int y;
    };

    EventType type;
    union
    {
        MouseClick_ mouse_click;
        Key_ key;
        MouseMove_ mouse_move;
    };

    Event(MouseButton button, ButtonState state)
    {
        type = EventType::MouseClick;
        mouse_click = {.button = button, .state = state};
    }

    Event(Key k, ButtonState state)
    {
        type = EventType::Key;
        key = {k, state};
    }

    Event(int x, int y)
    {
        type = EventType::MouseMove;
        mouse_move = {x, y};
    }
};
} // namespace bul
