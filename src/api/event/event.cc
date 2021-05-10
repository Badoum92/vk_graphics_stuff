#include "event.hh"

#include <algorithm>

EventHandler::EventMap EventHandler::on_event;

void EventHandler::register_callback(EventType type, const EventCallbackData& callback_data)
{
    on_event[type].push_back(callback_data);
}

void EventHandler::dispatch(const Event& event)
{
    for (const auto& callback_data : on_event[event.type])
    {
        callback_data.callback(event, callback_data.object);
    }
}
