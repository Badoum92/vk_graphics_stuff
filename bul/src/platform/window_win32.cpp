#include "bul/window.h"

#include <Windows.h>
#include <windowsx.h>
#include <stdlib.h>

#include "bul/bul.h"
#include "bul/input.h"
#include "bul/containers/enum_array.h"

namespace bul
{
// clang-format off
static enum_array<key, int> key_to_win32_key = {
#define X(KEY, WIN32_KEY) WIN32_KEY,
#include "bul/virtual_keys.def"
#undef X
};
// clang-format on

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void window::create(window* window, const char* title, vec2i size)
{
    ASSERT(window != nullptr);

    window->should_close = false;
    window->resized = false;
    window->is_cursor_visible = true;
    window->size = size;

    WNDCLASS wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "Win32 Window Class";
    RegisterClass(&wc);

    RECT rect = {0, 0, size.x, size.y};
    AdjustWindowRectEx(&rect, WS_BORDER | WS_OVERLAPPEDWINDOW, false, 0);
    size.x = rect.right - rect.left;
    size.y = rect.bottom - rect.top;
    window->handle = CreateWindowEx(0, wc.lpszClassName, title, WS_BORDER | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                    CW_USEDEFAULT, size.x, size.y, nullptr, nullptr, wc.hInstance, nullptr);

    ENSURE(window->handle != nullptr);
    SetProp((HWND)window->handle, "bul", window);
    ShowWindow((HWND)window->handle, SW_SHOW);

    RAWINPUTDEVICE rid = {0x01, 0x02, RIDEV_REMOVE, nullptr};
    ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
    window->raw_input = nullptr;
    window->raw_input_size = 0;
}

void window::destroy()
{
    DestroyWindow((HWND)handle);
    handle = nullptr;
    size = {0, 0};
    free(raw_input);
    raw_input = nullptr;
    raw_input_size = 0;
}

void window::set_title(const char* title_)
{
    SetWindowText((HWND)handle, title_);
}

float window::aspect_ratio() const
{
    return (float)size.x / (float)size.y;
}

void window::show_cursor(bool show)
{
    is_cursor_visible = show;

    RECT rect;
    GetClientRect((HWND)handle, &rect);
    ClientToScreen((HWND)handle, (POINT*)&rect.left);
    ClientToScreen((HWND)handle, (POINT*)&rect.right);

    if (!show)
    {
        visible_cursor_position = cursor_position;
        cursor_position = invisible_cursor_position;

        ClipCursor(&rect);
        SetCursor(nullptr);

        RAWINPUTDEVICE rid = {0x01, 0x02, 0, (HWND)handle};
        ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
    }
    else
    {
        invisible_cursor_position = cursor_position;
        cursor_position = visible_cursor_position;

        ClipCursor(nullptr);
        SetCursorPos(rect.left + cursor_position.x, rect.top + cursor_position.y);
        SetCursor(LoadCursor(nullptr, IDC_ARROW));

        RAWINPUTDEVICE rid = {0x01, 0x02, RIDEV_REMOVE, nullptr};
        ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
    }
}

void window::poll_events()
{
    input_new_frame();
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
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

static key handle_key_msg(WPARAM wParam, LPARAM)
{
    int count = (int)key::_count;
    for (int i = 0; i < count; ++i)
    {
        key key = (bul::key)i;
        int win32_key = key_to_win32_key[key];
        if ((int)wParam == win32_key)
        {
            return key;
        }
    }
    return key::_count;
}

static bool is_right_alt(MSG m)
{
    return (m.message == WM_KEYDOWN || m.message == WM_SYSKEYDOWN || m.message == WM_KEYUP || m.message == WM_SYSKEYUP)
        && m.wParam == VK_MENU && (HIWORD(m.lParam) & KF_EXTENDED);
}

static bool is_right_alt()
{
    MSG next;
    DWORD time = GetMessageTime();
    if (PeekMessage(&next, nullptr, 0, 0, PM_NOREMOVE) && is_right_alt(next) && next.time == time)
    {
        PeekMessage(&next, nullptr, 0, 0, PM_REMOVE);
        return true;
    }
    return false;
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    window* w = (window*)GetProp(hwnd, "bul");

    switch (uMsg)
    {
        // Window events

    case WM_DESTROY: {
        w->should_close = true;
        PostQuitMessage(0);
        return 0;
    }

    case WM_CLOSE: {
        w->should_close = true;
        return 0;
    }

    case WM_SIZE: {
        w->size = {LOWORD(lParam), HIWORD(lParam)};
        w->resized = true;
        return 0;
    }

        // Key events

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP: {
        wParam = handle_left_right_special_keys(wParam, lParam);
        key key = handle_key_msg(wParam, lParam);
        if (key != bul::key::_count)
        {
            if (key == bul::key::l_ctrl && is_right_alt())
            {
                key = bul::key::r_alt;
            }
            keys[key] = !(HIWORD(lParam) & KF_UP);
            // events.emplace_back(key, HIWORD(lParam) & KF_UP ? button_state::up : button_state::down);
        }
        return 0;
    }

        // Mouse events

    case WM_MOUSEMOVE: {
        if (w->is_cursor_visible)
        {
            bul::vec2i new_cursor_position = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            mouse_position_delta.x = new_cursor_position.x - w->cursor_position.x;
            mouse_position_delta.y = new_cursor_position.y - w->cursor_position.y;
            w->cursor_position = new_cursor_position;
            // events.emplace_back(cursor_pos_.x, cursor_pos_.y);
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK: {
        bul::mouse_buttons[mouse_button::mouse_1] = true;
        // events.emplace_back(mouse_button::mouse_1, button_state::down);
        return 0;
    }
    case WM_LBUTTONUP: {
        bul::mouse_buttons[mouse_button::mouse_1] = false;
        // events.emplace_back(mouse_button::mouse_1, button_state::up);
        return 0;
    }
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK: {
        bul::mouse_buttons[mouse_button::mouse_2] = true;
        // events.emplace_back(mouse_button::mouse_2, button_state::down);
        return 0;
    }
    case WM_RBUTTONUP: {
        bul::mouse_buttons[mouse_button::mouse_2] = false;
        // events.emplace_back(mouse_button::mouse_2, button_state::up);
        return 0;
    }
    case WM_MBUTTONDOWN: {
        bul::mouse_buttons[mouse_button::mouse_3] = true;
        // events.emplace_back(mouse_button::mouse_3, button_state::down);
        return 0;
    }
    case WM_MBUTTONUP: {
        bul::mouse_buttons[mouse_button::mouse_3] = false;
        // events.emplace_back(mouse_button::mouse_3, button_state::up);
        return 0;
    }
    case WM_XBUTTONDOWN: {
        if (lParam & MK_XBUTTON1)
        {
            bul::mouse_buttons[mouse_button::mouse_4] = true;
            // events.emplace_back(mouse_button::mouse_4, button_state::down);
        }
        else
        {
            bul::mouse_buttons[mouse_button::mouse_5] = true;
            // events.emplace_back(mouse_button::mouse_5, button_state::down);
        }
        return 0;
    }
    case WM_XBUTTONUP: {
        if (lParam & MK_XBUTTON1)
        {
            bul::mouse_buttons[mouse_button::mouse_4] = false;
            // events.emplace_back(mouse_button::mouse_4, button_state::up);
        }
        else
        {
            bul::mouse_buttons[mouse_button::mouse_5] = false;
            // events.emplace_back(mouse_button::mouse_5, button_state::up);
        }
        return 0;
    }

        // Raw input

    case WM_INPUT: {
        UINT size = 0;
        HRAWINPUT raw_input_handle = (HRAWINPUT)lParam;
        RAWINPUT* raw_input;

        GetRawInputData(raw_input_handle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
        if (size > w->raw_input_size)
        {
            w->raw_input_size = size;
            w->raw_input = (RAWINPUT*)realloc(w->raw_input, size);
            ENSURE(w->raw_input != nullptr);
        }

        size = w->raw_input_size;
        raw_input = (RAWINPUT*)w->raw_input;
        ENSURE(GetRawInputData(raw_input_handle, RID_INPUT, raw_input, &size, sizeof(RAWINPUTHEADER)) != UINT32_MAX);

        if (raw_input->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
        {
            mouse_position_delta.x = raw_input->data.mouse.lLastX - w->cursor_position.x;
            mouse_position_delta.y = raw_input->data.mouse.lLastY - w->cursor_position.y;
        }
        else
        {
            mouse_position_delta.x = raw_input->data.mouse.lLastX;
            mouse_position_delta.y = raw_input->data.mouse.lLastY;
        }
        w->cursor_position += mouse_position_delta;
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
} // namespace bul
