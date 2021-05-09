#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

#include <GLFW/glfw3.h>

enum class EventType
{
    CURSOR_POS,
    MOUSE_BUTTON,
    KEY
};

struct Event
{
    Event(EventType t)
        : type(t)
    {}

    EventType type;

    union
    {
        struct
        {
            float x;
            float y;
        } cursor_pos;

        struct
        {
            int button;
            int action;
            int mods;
        } mouse_button;

        struct
        {
            int key;
            int action;
            int mods;
        } key;
    } data;

    inline double pos_x() const
    {
        return data.cursor_pos.x;
    }

    inline double pos_y() const
    {
        return data.cursor_pos.y;
    }

    inline int mouse_button() const
    {
        return data.mouse_button.button;
    }

    inline int mouse_action() const
    {
        return data.mouse_button.action;
    }

    inline int mouse_mods() const
    {
        return data.mouse_button.mods;
    }

    inline int key() const
    {
        return data.key.key;
    }

    inline int key_action() const
    {
        return data.key.action;
    }

    inline int key_mods() const
    {
        return data.key.mods;
    }
};

class EventDispatcher
{
public:
    using EventCallback = std::function<void(const Event&)>;
    using EventMap = std::unordered_map<EventType, std::vector<EventCallback>>;

    static void add_callback(EventType type, const EventCallback& callback);
    static void dispatch(const Event& event);

private:
    static EventMap on_event;
};

#define BIND_EVENT_METHOD(M) std::bind(&M, this, std::placeholders::_1)
