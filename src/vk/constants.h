#pragma once

#include <stdint.h>

namespace vk
{
constexpr uint32_t max_color_attachments = 4;
constexpr uint32_t max_frames_in_flight = 3;
constexpr uint32_t max_graphics_states = 1;
constexpr uint32_t max_binless_descriptors = 1024;
} // namespace vk