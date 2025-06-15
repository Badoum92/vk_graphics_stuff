#include "core/input.h"

#include <string.h>

static bool keys[KEY_COUNT] = {false};
static bool keys_prev[KEY_COUNT] = {false};

static bool mouse_buttons[MOUSE_BUTTON_COUNT] = {false};
static bool mouse_buttons_prev[MOUSE_BUTTON_COUNT] = {false};

static vec2i mouse_position = {0, 0};
static vec2i mouse_position_delta = {0, 0};

_input_state input_new_frame()
{
    memcpy(keys_prev, keys, KEY_COUNT);
    memcpy(mouse_buttons_prev, mouse_buttons, MOUSE_BUTTON_COUNT);
    mouse_position = {0, 0};
    mouse_position_delta = {0, 0};
    return {keys, mouse_buttons, &mouse_position, &mouse_position_delta};
}

bool is_mouse_down(MOUSE_BUTTON button)
{
    return mouse_buttons[button];
}
bool is_mouse_pressed(MOUSE_BUTTON button)
{
    return mouse_buttons[button] && !mouse_buttons_prev[button];
}
bool is_mouse_released(MOUSE_BUTTON button)
{
    return !mouse_buttons[button] && mouse_buttons_prev[button];
}

bool is_key_down(KEY key)
{
    return keys[key];
}
bool is_key_pressed(KEY key)
{
    return keys[key] && !keys_prev[key];
}
bool is_key_released(KEY key)
{
    return !keys[key] && keys_prev[key];
}

vec2i mouse_get_position()
{
    return mouse_position;
}

vec2i mouse_get_delta()
{
    return mouse_position_delta;
}
