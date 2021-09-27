#include "window.hh"

#include "event.hh"

static void framebuffer_size_callback(GLFWwindow*, int, int);
static void cursor_pos_callback(GLFWwindow*, double, double);
static void mouse_button_callback(GLFWwindow*, int, int, int);
static void key_callback(GLFWwindow*, int, int, int, int);

uint32_t Window::width_{0};
uint32_t Window::height_{0};
bool Window::resized_{false};
GLFWwindow* Window::handle_{nullptr};

void Window::create(uint32_t width, uint32_t height, const std::string& title)
{
    if (handle_)
    {
        terminate();
    }

    glfwInit();

    width_ = width;
    height_ = height;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    handle_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!handle_)
    {
        return;
    }

    glfwSetInputMode(handle_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(handle_, framebuffer_size_callback);
    glfwSetCursorPosCallback(handle_, cursor_pos_callback);
    glfwSetMouseButtonCallback(handle_, mouse_button_callback);
    glfwSetKeyCallback(handle_, key_callback);
}

void Window::destroy()
{
    glfwDestroyWindow(handle_);
    glfwTerminate();
    handle_ = nullptr;
    width_ = 0;
    height_ = 0;
}

void Window::resize(uint32_t width, uint32_t height)
{
    width_ = width;
    height_ = height;
    glfwSetWindowSize(handle_, width, height);
}

uint32_t Window::width()
{
    return width_;
}

uint32_t Window::height()
{
    return height_;
}

float Window::aspect_ratio()
{
    return static_cast<float>(width_) / static_cast<float>(height_);
}

void Window::set_size(uint32_t width, uint32_t height)
{
    width_ = width;
    height_ = height;
}

void Window::poll_events()
{
    glfwPollEvents();
}

void Window::wait_events()
{
    glfwWaitEvents();
}

void Window::close()
{
    glfwSetWindowShouldClose(handle_, GLFW_TRUE);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(handle_) != 0;
}

void Window::set_title(const std::string& title)
{
    glfwSetWindowTitle(handle_, title.c_str());
}

/* GLFW callbacks */

static void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    Window::set_size(width, height);
}

static void cursor_pos_callback(GLFWwindow*, double x_pos, double y_pos)
{
    Event event(EventType::CURSOR_POS);
    event.data.cursor_pos.x = x_pos;
    event.data.cursor_pos.y = y_pos;
    EventHandler::dispatch(event);
}

static void mouse_button_callback(GLFWwindow*, int button, int action, int mods)
{
    Event event(EventType::MOUSE_BUTTON);
    event.data.mouse_button.button = button;
    event.data.mouse_button.action = action;
    event.data.mouse_button.mods = mods;
    EventHandler::dispatch(event);
}

static void key_callback(GLFWwindow*, int key, int, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        Window::close();
    }

    Event event(EventType::KEY);
    event.data.key.key = key;
    event.data.key.action = action;
    event.data.key.mods = mods;
    EventHandler::dispatch(event);
}
