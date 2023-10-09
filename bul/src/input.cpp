#include "bul/input.h"

#include "bul/containers/enum_array.h"

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
    memcpy(mouse_buttons_prev.data, mouse_buttons.data, mouse_buttons.SIZE);
    memcpy(keys_prev.data, keys.data, keys.SIZE);
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

bool key_down(Key key)
{
    return keys[key];
}
bool key_pressed(Key key)
{
    return keys[key] && !keys_prev[key];
}
bool key_released(Key key)
{
    return !keys[key] && keys_prev[key];
}
} // namespace bul
