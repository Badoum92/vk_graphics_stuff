#pragma once

#include "core/math/vector.h"

enum KEY
{
#define X(KEY, WIN32_KEY) KEY_##KEY,
#include "core/virtual_keys.def"
#undef X
    KEY_COUNT
};

constexpr const char* keys_str[KEY_COUNT] = {
#define X(KEY, WIN32_KEY) #KEY,
#include "core/virtual_keys.def"
#undef X
};

enum MOUSE_BUTTON
{
    MOUSE_BUTTON_1,
    MOUSE_BUTTON_2,
    MOUSE_BUTTON_3,
    MOUSE_BUTTON_4,
    MOUSE_BUTTON_5,
    MOUSE_BUTTON_COUNT
};

constexpr const char* mouse_button_str[MOUSE_BUTTON_COUNT] = {"mouse_1", "mouse_2", "mouse_3", "mouse_4", "mouse_5"};

enum BUTTON_STATE
{
    BUTTON_STATE_DOWN,
    BUTTON_STATE_UP,
    BUTTON_STATE_COUNT
};

constexpr const char* button_states_str[BUTTON_STATE_COUNT] = {"down", "up"};

struct _input_state
{
    bool* keys;
    bool* mouse_buttons;
    vec2i* mouse_position;
    vec2i* mouse_position_delta;
};

_input_state input_new_frame();

bool is_mouse_down(MOUSE_BUTTON button);
bool is_mouse_pressed(MOUSE_BUTTON button);
bool is_mouse_released(MOUSE_BUTTON button);

bool is_key_down(KEY key);
bool is_key_pressed(KEY key);
bool is_key_released(KEY key);

vec2i input_get_mouse_position();
vec2i input_get_mouse_delta();

void input_show_cursor(bool show);
bool input_is_cursor_visible();