#pragma once

#include <string_view>
#include <vector>

#include "bul/event.h"
#include "bul/math/vector.h"

namespace bul::window
{
void create(const std::string_view title, vec2u size = {1280, 720});
void destroy();
void close();
bool should_close();
void* handle();

vec2u size();
bool resized();
float aspect_ratio();

vec2i cursor_pos();
void show_cursor(bool show);
bool cursor_visible();

const std::vector<Event>& poll_events();
} // namespace bul::window
