#include "camera.h"

#include "bul/math/math.h"

void camera::compute_view_proj()
{
    bul::mat4f rotation = bul::rotation_x(pitch) * bul::rotation_y(yaw) * bul::rotation_z(roll);
    forward = bul::normalize(rotation * bul::vec4f{0, 0, -1, 0});
    right = bul::normalize(rotation * bul::vec4f{1, 0, 0, 0});
    up = bul::normalize(rotation * bul::vec4f{0, 1, 0, 0});

    view = bul::lookat(position, position + forward, up, &inv_view);
    proj = bul::perspective(fov_y, aspect_ratio, near_plane, far_plane, &inv_proj);
}

void camera::move_forward(float offset)
{
    position += forward * offset;
}

void camera::move_up(float offset)
{
    position += up * offset;
}

void camera::move_right(float offset)
{
    position += right * offset;
}

void camera::rotate(bul::vec3f angles)
{
    if (angles == bul::vec3f{0, 0, 0})
    {
        return;
    }

    pitch += angles.x;
    yaw += angles.y;
    roll += angles.z;

    pitch = bul_clamp(-bul::half_pi + 0.01f, pitch, bul::half_pi - 0.01f);
}
