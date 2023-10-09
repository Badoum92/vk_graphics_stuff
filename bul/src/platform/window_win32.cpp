#include "bul/window.h"

#include <Windows.h>
#include <windowsx.h>

#include "bul/bul.h"
#include "bul/util.h"
#include "bul/input.h"

namespace bul::window
{
static std::vector<Event> events;
static vec2u size_ = {0, 0};
static HWND handle_ = nullptr;
static bool should_close_ = false;
static bool resized_ = false;

static vec2i cursor_pos_ = {0, 0};
static bool cursor_visible_ = true;
static bul::vec2i save_visible_cursor_pos_ = cursor_pos_;
static bul::vec2i save_invisible_cursor_pos_ = cursor_pos_;

static RAWINPUT* raw_input_ = nullptr;
static UINT raw_input_size_ = 0;

// clang-format off
static EnumArray<Key, int> key_to_win32_key = {
#define X(KEY, WIN32_KEY) WIN32_KEY,
#include "bul/virtual_keys.def"
#undef X
};
// clang-format on

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void create(const std::string_view title, vec2u size)
{
    destroy();

    size_ = size;

    WNDCLASS wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "Win32 Window Class";
    RegisterClass(&wc);

    RECT rect = {0, 0, static_cast<LONG>(size_.x), static_cast<LONG>(size_.y)};
    AdjustWindowRectEx(&rect, WS_BORDER | WS_OVERLAPPEDWINDOW, false, 0);
    size_.x = uint32_t(rect.right - rect.left);
    size_.y = uint32_t(rect.bottom - rect.top);
    handle_ = CreateWindowEx(0, wc.lpszClassName, title.data(), WS_BORDER | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                             CW_USEDEFAULT, size_.x, size_.y, nullptr, nullptr, wc.hInstance, nullptr);

    ASSERT(handle_ != nullptr);
    should_close_ = false;
    ShowWindow(handle_, SW_SHOW);
    resized_ = false;

    RAWINPUTDEVICE rid = {0x01, 0x02, RIDEV_REMOVE, nullptr};
    ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
}

void destroy()
{
    free(raw_input_);
}

void close()
{
    DestroyWindow(handle_);
    size_ = {0, 0};
    handle_ = nullptr;
}

bool should_close()
{
    return should_close_;
}

void* handle()
{
    return handle_;
}

vec2u size()
{
    return size_;
}

bool resized()
{
    bool ret = resized_;
    resized_ = false;
    return ret;
}

float aspect_ratio()
{
    return static_cast<float>(size_.x) / static_cast<float>(size_.y);
}

vec2i cursor_pos()
{
    return cursor_pos_;
}

void show_cursor(bool show)
{
    cursor_visible_ = show;

    RECT rect;
    GetClientRect(static_cast<HWND>(window::handle()), &rect);
    ClientToScreen(static_cast<HWND>(window::handle()), reinterpret_cast<POINT*>(&rect.left));
    ClientToScreen(static_cast<HWND>(window::handle()), reinterpret_cast<POINT*>(&rect.right));

    if (!cursor_visible_)
    {
        save_visible_cursor_pos_ = cursor_pos_;
        cursor_pos_ = save_invisible_cursor_pos_;

        ClipCursor(&rect);
        SetCursor(nullptr);

        RAWINPUTDEVICE rid = {0x01, 0x02, 0, static_cast<HWND>(window::handle())};
        ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
    }
    else
    {
        save_invisible_cursor_pos_ = cursor_pos_;
        cursor_pos_ = save_visible_cursor_pos_;

        ClipCursor(nullptr);
        SetCursorPos(rect.left + cursor_pos_.x, rect.top + cursor_pos_.y);
        SetCursor(LoadCursor(nullptr, IDC_ARROW));

        RAWINPUTDEVICE rid = {0x01, 0x02, RIDEV_REMOVE, nullptr};
        ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
    }
}

bool cursor_visible()
{
    return cursor_visible_;
}

const std::vector<Event>& poll_events()
{
    events.clear();
    input::_private::new_frame();
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return events;
}

static WPARAM handle_left_right_special_keys(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case VK_SHIFT:
        return MapVirtualKey(HIWORD(lParam) & 0xff, MAPVK_VSC_TO_VK_EX);
    case VK_CONTROL:
        return HIWORD(lParam) & KF_EXTENDED ? VK_RCONTROL : VK_LCONTROL;
    case VK_MENU:
        return HIWORD(lParam) & KF_EXTENDED ? VK_RMENU : VK_LMENU;
    }
    return wParam;
}

static Key handle_key_msg(WPARAM wParam, LPARAM)
{
    int count = to_underlying(Key::Count);
    for (int i = 0; i < count; ++i)
    {
        Key key = to_enum<Key>(i);
        int win32_key = key_to_win32_key[key];
        if (static_cast<int>(wParam) == win32_key)
        {
            return key;
        }
    }
    return Key::Count;
}

static bool is_right_alt(MSG m)
{
    return (m.message == WM_KEYDOWN || m.message == WM_SYSKEYDOWN || m.message == WM_KEYUP || m.message == WM_SYSKEYUP)
        && m.wParam == VK_MENU && (HIWORD(m.lParam) & KF_EXTENDED);
}

static bool is_right_alt()
{
    MSG next;
    LONG time = GetMessageTime();
    if (PeekMessage(&next, nullptr, 0, 0, PM_NOREMOVE) && is_right_alt(next) && next.time == time)
    {
        PeekMessage(&next, nullptr, 0, 0, PM_REMOVE);
        return true;
    }
    return false;
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        // Window events

    case WM_DESTROY: {
        should_close_ = true;
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE: {
        size_ = {LOWORD(lParam), HIWORD(lParam)};
        resized_ = true;
        return 0;
    }

        // Key events

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP: {
        wParam = handle_left_right_special_keys(wParam, lParam);
        Key key = handle_key_msg(wParam, lParam);
        if (key != Key::Count)
        {
            if (key == Key::LCtrl && is_right_alt())
            {
                key = Key::RAlt;
            }
            keys[key] = !(HIWORD(lParam) & KF_UP);
            events.emplace_back(key, HIWORD(lParam) & KF_UP ? ButtonState::Up : ButtonState::Down);
        }
        return 0;
    }

        // Mouse events

    case WM_MOUSEMOVE: {
        if (cursor_visible())
        {
            cursor_pos_ = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            events.emplace_back(cursor_pos_.x, cursor_pos_.y);
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK: {
        bul::mouse_buttons[MouseButton::Mouse_1] = true;
        events.emplace_back(MouseButton::Mouse_1, ButtonState::Down);
        return 0;
    }
    case WM_LBUTTONUP: {
        bul::mouse_buttons[MouseButton::Mouse_1] = false;
        events.emplace_back(MouseButton::Mouse_1, ButtonState::Up);
        return 0;
    }
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK: {
        bul::mouse_buttons[MouseButton::Mouse_2] = true;
        events.emplace_back(MouseButton::Mouse_2, ButtonState::Down);
        return 0;
    }
    case WM_RBUTTONUP: {
        bul::mouse_buttons[MouseButton::Mouse_2] = false;
        events.emplace_back(MouseButton::Mouse_2, ButtonState::Up);
        return 0;
    }
    case WM_MBUTTONDOWN: {
        bul::mouse_buttons[MouseButton::Mouse_3] = true;
        events.emplace_back(MouseButton::Mouse_3, ButtonState::Down);
        return 0;
    }
    case WM_MBUTTONUP: {
        bul::mouse_buttons[MouseButton::Mouse_3] = false;
        events.emplace_back(MouseButton::Mouse_3, ButtonState::Up);
        return 0;
    }
    case WM_XBUTTONDOWN: {
        if (lParam & MK_XBUTTON1)
        {
            bul::mouse_buttons[MouseButton::Mouse_4] = true;
            events.emplace_back(MouseButton::Mouse_4, ButtonState::Down);
        }
        else
        {
            bul::mouse_buttons[MouseButton::Mouse_5] = true;
            events.emplace_back(MouseButton::Mouse_5, ButtonState::Down);
        }
        return 0;
    }
    case WM_XBUTTONUP: {
        if (lParam & MK_XBUTTON1)
        {
            bul::mouse_buttons[MouseButton::Mouse_4] = false;
            events.emplace_back(MouseButton::Mouse_4, ButtonState::Up);
        }
        else
        {
            bul::mouse_buttons[MouseButton::Mouse_5] = false;
            events.emplace_back(MouseButton::Mouse_5, ButtonState::Up);
        }
        return 0;
    }

        // Raw input

    case WM_INPUT: {
        UINT size = 0;
        HRAWINPUT ri = reinterpret_cast<HRAWINPUT>(lParam);

        GetRawInputData(ri, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
        if (size > raw_input_size_)
        {
            raw_input_size_ = size;
            raw_input_ = static_cast<RAWINPUT*>(realloc(raw_input_, size));
        }

        size = raw_input_size_;
        ENSURE(GetRawInputData(ri, RID_INPUT, raw_input_, &size, sizeof(RAWINPUTHEADER)) != UINT(-1));
        ENSURE(GetRawInputData(ri, RID_INPUT, raw_input_, &size, sizeof(RAWINPUTHEADER)) != UINT(-1));

        bul::vec2i delta;
        if (raw_input_->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
        {
            delta.x = raw_input_->data.mouse.lLastX - cursor_pos_.x;
            delta.y = raw_input_->data.mouse.lLastY - cursor_pos_.y;
        }
        else
        {
            delta.x = raw_input_->data.mouse.lLastX;
            delta.y = raw_input_->data.mouse.lLastY;
        }
        cursor_pos_ += delta;
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
} // namespace bul::window
