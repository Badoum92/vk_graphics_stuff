#pragma once

#include "bul/math/matrix.h"
#include "bul/math/vector.h"

struct camera
{
    static constexpr bul::vec3f WORLD_RIGHT = {1, 0, 0};
    static constexpr bul::vec3f WORLD_UP = {0, 1, 0};
    static constexpr bul::vec3f WORLD_FORWARD = {0, 0, -1};

    static camera create();

    void compute_view_proj();
    void rotate(bul::vec3f angles);

    bul::vec3f position;
    bul::vec3f right;
    bul::vec3f up;
    bul::vec3f forward;

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

    bul::mat4f view;
    bul::mat4f inv_view;
    bul::mat4f proj;
    bul::mat4f inv_proj;
};
