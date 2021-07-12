#pragma once

#include <utility>

namespace Input
{
    bool is_key_pressed(int key);
    bool is_key_repeated(int key);
    bool is_key_released(int key);

    bool is_button_pressed(int button);
    bool is_button_released(int button);

    bool cursor_enabled();
    void show_cursor(bool show);
    std::pair<double, double> get_cursor_pos();
} // namespace Input
