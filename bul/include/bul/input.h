#pragma once

#include "bul/containers/enum_array.h"
#include "bul/math/vector.h"

namespace bul
{
enum class mouse_button
{
    mouse_1,
    mouse_2,
    mouse_3,
    mouse_4,
    mouse_5,
    _count
};

// clang-format off
constexpr inline enum_array<mouse_button, const char*> mouse_button_str = {
    "mouse_1",
    "mouse_2",
    "mouse_3",
    "mouse_4",
    "mouse_5"
};
// clang-format on

enum class key
{
#define X(KEY, WIN32_KEY) KEY,
#include "bul/virtual_keys.def"
#undef X
    _count
};

constexpr inline enum_array<key, const char*> keys_str = {
#define X(KEY, WIN32_KEY) #KEY,
#include "bul/virtual_keys.def"
#undef X
};

enum class button_state
{
    down,
    up,
    _count
};

// clang-format off
constexpr inline enum_array<button_state, const char*> button_states_str = {
    "down",
    "up"
};
// clang-format on

void input_new_frame();

extern enum_array<mouse_button, bool> mouse_buttons;
extern vec2i mouse_position_delta;
extern enum_array<key, bool> keys;

bool mouse_button_down(mouse_button button);
bool mouse_button_pressed(mouse_button button);
bool mouse_button_released(mouse_button button);

bool key_down(key key);
bool key_pressed(key key);
bool key_released(key key);
} // namespace bul
