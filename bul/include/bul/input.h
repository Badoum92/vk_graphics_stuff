#pragma once

#include "bul/containers/enum_array.h"

namespace bul
{
enum class MouseButton
{
    Mouse_1,
    Mouse_2,
    Mouse_3,
    Mouse_4,
    Mouse_5,
    Count
};

// clang-format off
constexpr inline EnumArray<MouseButton, const char*> mouse_button_str = {
    "Mouse_1",
    "Mouse_2",
    "Mouse_3",
    "Mouse_4",
    "Mouse_5"
};
// clang-format on

enum class Key
{
#define X(KEY, WIN32_KEY) KEY,
#include "bul/virtual_keys.def"
#undef X
    Count
};

constexpr inline EnumArray<Key, const char*> keys_str = {
#define X(KEY, WIN32_KEY) #KEY,
#include "bul/virtual_keys.def"
#undef X
};

enum class ButtonState
{
    Down,
    Up,
    Count
};

// clang-format off
constexpr inline EnumArray<ButtonState, const char*> button_states_str = {
    "Down",
    "Up"
};
// clang-format on

namespace input::_private
{
void new_frame();
}

bool button_down(MouseButton button);
bool button_pressed(MouseButton button);
bool button_released(MouseButton button);

bool key_down(Key key);
bool key_pressed(Key key);
bool key_released(Key key);
} // namespace bul
