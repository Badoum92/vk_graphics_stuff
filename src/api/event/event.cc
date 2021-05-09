#include "event.hh"

#include <algorithm>

EventDispatcher::EventMap EventDispatcher::on_event;

void EventDispatcher::add_callback(EventType type, const EventCallback& callback)
{
    on_event[type].push_back(callback);
}

void EventDispatcher::dispatch(const Event& event)
{
    for (const auto& callback : on_event[event.type])
    {
        callback(event);
    }
}
