#include "window.hh"

static void framebuffer_size_callback(GLFWwindow*, int, int);
// static void cursor_pos_callback(GLFWwindow*, double, double);
// static void mouse_button_callback(GLFWwindow*, int, int, int);
static void key_callback(GLFWwindow*, int, int, int, int);

int Window::width_{0};
int Window::height_{0};
GLFWwindow* Window::handle_{nullptr};

void Window::create(int width, int height, const std::string& title)
{
    if (handle_)
    {
        terminate();
    }

    glfwInit();

    width_ = width;
    height_ = height;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    handle_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!handle_)
    {
        return;
    }

    glfwSetFramebufferSizeCallback(handle_, framebuffer_size_callback);
    // glfwSetCursorPosCallback(handle_, cursor_pos_callback);
    // glfwSetMouseButtonCallback(handle_, mouse_button_callback);
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


void Window::resize(int width, int height)
{
    width_ = width;
    height_ = height;
    glfwSetWindowSize(handle_, width, height);
}

int Window::width()
{
    return width_;
}

int Window::height()
{
    return height_;
}

float Window::aspect_ratio()
{
    return static_cast<float>(width_) / static_cast<float>(height_);
}

void Window::set_size(int width, int height)
{
    width_ = width;
    height_ = height;
}

void Window::update()
{
    glfwPollEvents();
}

void Window::close()
{
    glfwSetWindowShouldClose(handle_, GLFW_TRUE);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(handle_) != 0;
}

/* GLFW callbacks */

static void framebuffer_size_callback(GLFWwindow* w, int width, int height)
{
    Window::set_size(width, height);
}

/* static void cursor_pos_callback(GLFWwindow*, double x_pos, double y_pos)
{
    Event event(EventType::CURSOR_POS);
    event.data.cursor_pos.x = x_pos;
    event.data.cursor_pos.y = y_pos;
    EventDispatcher::dispatch(event);
}

static void mouse_button_callback(GLFWwindow*, int button, int action, int mods)
{
    Event event(EventType::MOUSE_BUTTON);
    event.data.mouse_button.button = button;
    event.data.mouse_button.action = action;
    event.data.mouse_button.mods = mods;
    EventDispatcher::dispatch(event);
} */

static void key_callback(GLFWwindow*, int key, int, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        Window::close();
    }
    // Event event(EventType::KEY);
    // event.data.key.key = key;
    // event.data.key.action = action;
    // event.data.key.mods = mods;
    // EventDispatcher::dispatch(event);
}
