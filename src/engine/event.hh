#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

enum class EventType
{
    KEY,
    CURSOR_POS,
    MOUSE_BUTTON
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

template <typename T>
concept HasKeyCallback = requires(T, const Event& event, void* object)
{
    T::key_callback(event, object);
};

template <typename T>
concept HasCursorPosCallback = requires(T, const Event& event, void* object)
{
    T::cursor_pos_callback(event, object);
};

template <typename T>
concept HasMouseButtonCallback = requires(T, const Event& event, void* object)
{
    T::mouse_button_callback(event, object);
};

class EventHandler
{
public:
    struct EventCallbackData
    {
        std::function<void(const Event&, void*)> callback;
        void* object;
    };

    using EventMap = std::unordered_map<EventType, std::vector<EventCallbackData>>;

    static void register_callback(EventType type, const EventCallbackData& callback_data);
    static void dispatch(const Event& event);

    template <HasKeyCallback T>
    static void register_key_callback(T* object)
    {
        EventCallbackData callback_data{&T::key_callback, object};
        register_callback(EventType::KEY, callback_data);
    }

    template <HasCursorPosCallback T>
    static void register_cursor_pos_callback(T* object)
    {
        EventCallbackData callback_data{&T::cursor_pos_callback, object};
        register_callback(EventType::CURSOR_POS, callback_data);
    }

    template <HasMouseButtonCallback T>
    static void register_cursor_pos_callback(T* object)
    {
        EventCallbackData callback_data{&T::mouse_button_callback, object};
        register_callback(EventType::MOUSE_BUTTON, callback_data);
    }

private:
    static EventMap on_event;
};
