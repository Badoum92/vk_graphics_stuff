#pragma once

#include <string>

class Application
{
public:
    Application(const std::string& name)
        : name_(name)
    {}
    virtual ~Application() = default;

    virtual void update() = 0;
    // virtual void imgui_update() = 0;

    // clang-format off
    // virtual void on_key_event(const Event& event) {}
    // virtual void on_click_event(const Event& event) {}
    // virtual void on_cursor_event(const Event& event) {}
    // clang-format on

protected:
    std::string name_;
};
