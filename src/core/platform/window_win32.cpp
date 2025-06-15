#include "core/window.h"

#include <Windows.h>
#include <windowsx.h>
#include <stdlib.h>

#include "core/core.h"
#include "core/log.h"
#include "core/input.h"

#include "imgui/imgui_impl_win32.h"

static _input_state input_state;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static int key_to_win32_key[KEY_COUNT] = {
#define X(KEY, WIN32_KEY) WIN32_KEY,
#include "core/virtual_keys.def"
#undef X
};

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void window_create(window* window, const char* title, vec2i size)
{
    window->should_close = false;
    window->resized = false;
    window->is_cursor_visible = true;
    window->size = size;

    WNDCLASS wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "Win32 Window Class";
    wc.cbWndExtra = (DWLP_USER + 1) * sizeof(void*);
    RegisterClassA(&wc);

    RECT rect = {0, 0, size.x, size.y};
    AdjustWindowRectEx(&rect, WS_BORDER | WS_OVERLAPPEDWINDOW, false, 0);
    size.x = rect.right - rect.left;
    size.y = rect.bottom - rect.top;
    window->handle = CreateWindowEx(0, wc.lpszClassName, title, WS_BORDER | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                    CW_USEDEFAULT, size.x, size.y, nullptr, nullptr, wc.hInstance, nullptr);

    ENSURE(window->handle != nullptr);
    SetWindowLongPtrA((HWND)window->handle, DWLP_USER, (uint64_t)window);
    ShowWindow((HWND)window->handle, SW_SHOW);

    RAWINPUTDEVICE rid = {0x01, 0x02, RIDEV_REMOVE, nullptr};
    ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
}

void window_destroy(window* window)
{
    DestroyWindow((HWND)window->handle);
    window->handle = nullptr;
    window->size = {0, 0};
}

void window_set_title(window* window, const char* title)
{
    SetWindowTextA((HWND)window->handle, title);
}

float window_aspect_ratio(const window* window)
{
    return (float)window->size.x / (float)window->size.y;
}

void window_show_cursor(window* window, bool show)
{
    window->is_cursor_visible = show;

    RECT rect;
    GetClientRect((HWND)window->handle, &rect);
    ClientToScreen((HWND)window->handle, (POINT*)&rect.left);
    ClientToScreen((HWND)window->handle, (POINT*)&rect.right);

    if (!show)
    {
        window->visible_cursor_position = window->cursor_position;
        window->cursor_position = window->invisible_cursor_position;

        rect.left++;
        rect.right--;
        rect.top++;
        rect.bottom--;

        ClipCursor(&rect);
        SetCursor(nullptr);

        RAWINPUTDEVICE rid = {0x01, 0x02, 0, (HWND)window->handle};
        ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
    }
    else
    {
        window->invisible_cursor_position = window->cursor_position;
        window->cursor_position = window->visible_cursor_position;

        ClipCursor(nullptr);
        SetCursorPos(rect.left + window->cursor_position.x, rect.top + window->cursor_position.y);
        SetCursor(LoadCursorA(nullptr, IDC_ARROW));

        RAWINPUTDEVICE rid = {0x01, 0x02, RIDEV_REMOVE, nullptr};
        ENSURE(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
    }
}

void window_poll_events()
{
    input_state = input_new_frame();
    MSG msg = {};
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
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

static KEY handle_key_msg(WPARAM wParam, LPARAM)
{
    for (int i = 0; i < KEY_COUNT; ++i)
    {
        int win32_key = key_to_win32_key[i];
        if ((int)wParam == win32_key)
        {
            return (KEY)i;
        }
    }
    return KEY_COUNT;
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
    window* window = (::window*)GetWindowLongPtr(hwnd, DWLP_USER);
    if (window && window->is_cursor_visible && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
        // Window events

    case WM_DESTROY: {
        window->should_close = true;
        PostQuitMessage(0);
        return 0;
    }

    case WM_CLOSE: {
        window->should_close = true;
        return 0;
    }

    case WM_SIZE: {
        window->size = {LOWORD(lParam), HIWORD(lParam)};
        window->resized = true;
        return 0;
    }

    case WM_ACTIVATE: {
        if (wParam != WA_INACTIVE && !window->is_cursor_visible)
        {
            window_show_cursor(window, window->is_cursor_visible);
        }
    }

        // Key events

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP: {
        wParam = handle_left_right_special_keys(wParam, lParam);
        KEY key = handle_key_msg(wParam, lParam);
        if (key != KEY_COUNT)
        {
            if (key == KEY_L_CTRL && is_right_alt())
            {
                key = KEY_R_ALT;
            }
            input_state.keys[key] = !(HIWORD(lParam) & KF_UP);
        }
        return 0;
    }

        // Mouse events

    case WM_MOUSEMOVE: {
        if (window->is_cursor_visible)
        {
            vec2i new_cursor_position = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            input_state.mouse_position_delta->x = new_cursor_position.x - window->cursor_position.x;
            input_state.mouse_position_delta->y = new_cursor_position.y - window->cursor_position.y;
            *input_state.mouse_position = new_cursor_position;
            window->cursor_position = new_cursor_position;
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK: {
        input_state.mouse_buttons[MOUSE_BUTTON_1] = true;
        return 0;
    }
    case WM_LBUTTONUP: {
        input_state.mouse_buttons[MOUSE_BUTTON_1] = false;
        return 0;
    }
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK: {
        input_state.mouse_buttons[MOUSE_BUTTON_2] = true;
        return 0;
    }
    case WM_RBUTTONUP: {
        input_state.mouse_buttons[MOUSE_BUTTON_2] = false;
        return 0;
    }
    case WM_MBUTTONDOWN: {
        input_state.mouse_buttons[MOUSE_BUTTON_3] = true;
        return 0;
    }
    case WM_MBUTTONUP: {
        input_state.mouse_buttons[MOUSE_BUTTON_3] = false;
        return 0;
    }
    case WM_XBUTTONDOWN: {
        if (lParam & MK_XBUTTON1)
        {
            input_state.mouse_buttons[MOUSE_BUTTON_4] = true;
        }
        else
        {
            input_state.mouse_buttons[MOUSE_BUTTON_5] = true;
        }
        return 0;
    }
    case WM_XBUTTONUP: {
        if (lParam & MK_XBUTTON1)
        {
            input_state.mouse_buttons[MOUSE_BUTTON_4] = false;
        }
        else
        {
            input_state.mouse_buttons[MOUSE_BUTTON_5] = false;
        }
        return 0;
    }

        // Raw input

    case WM_INPUT: {
        UINT size = sizeof(RAWINPUT);
        static RAWINPUT raw_input;
        ENSURE(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw_input, &size, sizeof(RAWINPUTHEADER)));
        if (raw_input.header.dwType == RIM_TYPEMOUSE)
        {
            input_state.mouse_position_delta->x = raw_input.data.mouse.lLastX;
            input_state.mouse_position_delta->y = raw_input.data.mouse.lLastY;
        }
        window->cursor_position += *input_state.mouse_position_delta;
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
