#include "bul/input.h"

#include "bul/containers/enum_array.h"

#include <cstring>

namespace bul
{
enum_array<mouse_button, bool> mouse_buttons = {false};
static enum_array<mouse_button, bool> mouse_buttons_prev = {false};

enum_array<key, bool> keys = {false};
static enum_array<key, bool> keys_prev = {false};

namespace input::_private
{
void new_frame()
{
    memcpy(mouse_buttons_prev.data, mouse_buttons.data, mouse_buttons.SIZE);
    memcpy(keys_prev.data, keys.data, keys.SIZE);
}
} // namespace _private

bool button_down(mouse_button button)
{
    return mouse_buttons[button];
}
bool button_pressed(mouse_button button)
{
    return mouse_buttons[button] && !mouse_buttons_prev[button];
}
bool button_released(mouse_button button)
{
    return !mouse_buttons[button] && mouse_buttons_prev[button];
}

bool key_down(key key)
{
    return keys[key];
}
bool key_pressed(key key)
{
    return keys[key] && !keys_prev[key];
}
bool key_released(key key)
{
    return !keys[key] && keys_prev[key];
}
} // namespace bul
