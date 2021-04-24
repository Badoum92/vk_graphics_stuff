#include "surface.hh"

#include <GLFW/glfw3.h>

#include "window/window.hh"
#include "vk_context/vk_context.hh"

void Surface::create()
{
    VK_CHECK(glfwCreateWindowSurface(VkContext::instance.handle(), Window::handle(), nullptr, &handle_));
}

void Surface::destroy()
{
    vkDestroySurfaceKHR(VkContext::instance, handle_, nullptr);
}
