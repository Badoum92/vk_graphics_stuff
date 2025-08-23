#pragma once

#include "core/math/matrix.h"
#include "core/math/vector.h"

static constexpr vec3f WORLD_RIGHT = {1, 0, 0};
static constexpr vec3f WORLD_UP = {0, 1, 0};
static constexpr vec3f WORLD_FORWARD = {0, 0, -1};

struct camera
{
    vec3f position;
    vec3f right;
    vec3f up;
    vec3f forward;

    float pitch;
    float yaw;
    float roll;
    union
    {
        float fov_y;
        float ortho_size;
    };
    float aspect_ratio;
    float near_plane;
    float far_plane;

    mat4f view;
    mat4f inv_view;
    mat4f proj;
    mat4f inv_proj;
    mat4f view_proj;
    mat4f inv_view_proj;
};

camera camera_create();
void camera_compute_view_proj(camera* camera);
void camera_rotate(camera* camera, vec3f angles);
vec3f camera_world_to_screen(camera const* camera, vec3f point, vec2u resolution);