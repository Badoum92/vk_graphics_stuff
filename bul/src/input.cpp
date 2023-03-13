#include "bul/input.h"

#include <cstring>

namespace bul
{
EnumArray<MouseButton, bool> mouse_buttons = {false};
static EnumArray<MouseButton, bool> mouse_buttons_prev = {false};

EnumArray<Key, bool> keys = {false};
static EnumArray<Key, bool> keys_prev = {false};

namespace input::_private
{
void new_frame()
{
    memcpy(mouse_buttons_prev.data, mouse_buttons.data, mouse_buttons.SIZE * sizeof(bool));
    memcpy(keys_prev.data, keys.data, keys.SIZE * sizeof(bool));
}
} // namespace _private

bool button_down(MouseButton button)
{
    return mouse_buttons[button];
}
bool button_pressed(MouseButton button)
{
    return mouse_buttons[button] && !mouse_buttons_prev[button];
}
bool button_released(MouseButton button)
{
    return !mouse_buttons[button] && mouse_buttons_prev[button];
}

bool key_down(Key vkey)
{
    return keys[vkey];
}
bool key_pressed(Key vkey)
{
    return keys[vkey] && !keys_prev[vkey];
}
bool key_released(Key vkey)
{
    return !keys[vkey] && keys_prev[vkey];
}
} // namespace bul
