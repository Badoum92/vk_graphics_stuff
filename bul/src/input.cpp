#include "bul/input.h"

#include "bul/containers/enum_array.h"

#include <string.h>

namespace bul
{
enum_array<mouse_button, bool> mouse_buttons = {false};
static enum_array<mouse_button, bool> mouse_buttons_prev = {false};

enum_array<key, bool> keys = {false};
static enum_array<key, bool> keys_prev = {false};

vec2i mouse_position_delta = {0, 0};

void input_new_frame()
{
    memcpy(mouse_buttons_prev.data, mouse_buttons.data, mouse_buttons.SIZE);
    memcpy(keys_prev.data, keys.data, keys.SIZE);
    mouse_position_delta = {0, 0};
}

bool mouse_button_down(mouse_button button)
{
    return mouse_buttons[button];
}
bool mouse_button_pressed(mouse_button button)
{
    return mouse_buttons[button] && !mouse_buttons_prev[button];
}
bool mouse_button_released(mouse_button button)
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
