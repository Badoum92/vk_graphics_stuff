#pragma once

#include <string>
#include <memory>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window() = delete;

    static void create(int width, int height, const std::string& title);
    static void destroy();

    static void resize(int width, int height);
    static int width();
    static int height();
    static float aspect_ratio();
    static void set_size(int width, int height);

    static void update();
    static void close();
    static bool should_close();

    static inline GLFWwindow* handle()
    {
        return handle_;
    }

private:
    static int width_;
    static int height_;
    static GLFWwindow* handle_;
};
