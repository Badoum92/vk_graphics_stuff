#include "camera.h"

#include "bul/math/math.h"

void camera::compute_view_proj()
{
    bul::mat4f rotation = bul::rotation_x(pitch) * bul::rotation_y(yaw) * bul::rotation_z(roll);
    right = bul::normalize(rotation * bul::vec4f{1, 0, 0, 0});
    up = bul::normalize(rotation * bul::vec4f{0, 1, 0, 0});
    forward = bul::normalize(rotation * bul::vec4f{0, 0, -1, 0});

    view = bul::lookat(position, position + forward, up, &inv_view);
    proj = bul::perspective(fov_y, aspect_ratio, near_plane, far_plane, &inv_proj);
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
