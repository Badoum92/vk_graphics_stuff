#pragma once

#include <string>
#include <memory>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window() = delete;

    static void create(uint32_t width, uint32_t height, const std::string& title);
    static void destroy();

    static void resize(uint32_t width, uint32_t height);
    static uint32_t width();
    static uint32_t height();
    static float aspect_ratio();
    static void set_size(uint32_t width, uint32_t height);

    static void poll_events();
    static void wait_events();
    static void close();
    static bool should_close();

    static void set_title(const std::string& title);

    static inline GLFWwindow* handle()
    {
        return handle_;
    }

private:
    static uint32_t width_;
    static uint32_t height_;
    static bool resized_;
    static GLFWwindow* handle_;
};
