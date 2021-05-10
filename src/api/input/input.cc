#include "input.hh"

#include "window/window.hh"

static bool cursor_enabled_ = false;

bool Input::is_key_pressed(int key)
{
    return glfwGetKey(Window::handle(), key) == GLFW_PRESS;
}

bool Input::is_key_repeated(int key)
{
    return glfwGetKey(Window::handle(), key) == GLFW_REPEAT;
}

bool Input::is_key_released(int key)
{
    return glfwGetKey(Window::handle(), key) == GLFW_RELEASE;
}

bool Input::is_button_pressed(int button)
{
    return glfwGetMouseButton(Window::handle(), button) == GLFW_PRESS;
}

bool Input::is_button_released(int button)
{
    return glfwGetMouseButton(Window::handle(), button) == GLFW_RELEASE;
}

bool Input::cursor_enabled()
{
    return cursor_enabled_;
}

void Input::show_cursor(bool show)
{
    cursor_enabled_ = show;
    int enabled = show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
    glfwSetInputMode(Window::handle(), GLFW_CURSOR, enabled);
}

std::pair<double, double> Input::get_cursor_pos()
{
    std::pair<double, double> pos;
    glfwGetCursorPos(Window::handle(), &pos.first, &pos.second);
    return pos;
}
