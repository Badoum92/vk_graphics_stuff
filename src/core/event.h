#pragma once

#include "core/input.h"

enum EVENT_TYPE
{
    EVENT_TYPE_KEY,
    EVENT_TYPE_MOUSE_BUTTON,
    EVENT_TYPE_MOUSE_MOVE,
    EVENT_TYPE_COUNT
};

constexpr const char* event_types_str[EVENT_TYPE_COUNT] = {"Key", "MouseButton", "MouseMove"};

struct event
{
    struct _key
    {
        KEY key;
        BUTTON_STATE state;
    };

    struct _mouse_button
    {
        MOUSE_BUTTON button;
        BUTTON_STATE state;
    };

    EVENT_TYPE type;
    union
    {
        _key key;
        _mouse_button mouse_button;
        vec2i mouse_move;
    };
};

inline event event_create(KEY key, BUTTON_STATE button_state)
{
    return {.type = EVENT_TYPE_KEY, .key = {key, button_state}};
}

inline event event_create(MOUSE_BUTTON mouse_button, BUTTON_STATE button_state)
{
    return {.type = EVENT_TYPE_MOUSE_BUTTON, .mouse_button = {mouse_button, button_state}};
}

inline event event_create(vec2i move)
{
    return {.type = EVENT_TYPE_MOUSE_MOVE, .mouse_move = {move.x, move.y}};
}