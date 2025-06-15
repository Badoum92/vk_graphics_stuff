#include "camera.h"

#include "core/math/math.h"

camera camera::create()
{
    camera camera;
    camera.right = WORLD_RIGHT;
    camera.up = WORLD_UP;
    camera.forward = WORLD_FORWARD;
    return camera;
}

void camera::compute_view_proj()
{
    view = mat4_lookat(position, position + forward, up, &inv_view);
    proj = mat4_perspective(fov_y, aspect_ratio, near_plane, far_plane, &inv_proj);
}

void camera::rotate(vec3f angles)
{
    if (angles == vec3f{0, 0, 0})
    {
        return;
    }

    pitch += angles.x;
    yaw += angles.y;
    roll += angles.z;

    pitch = math_clamp(-math_half_pi + 0.01f, pitch, math_half_pi - 0.01f);

    mat4f rotation = mat4_rotation_x(pitch) * mat4_rotation_y(yaw) * mat4_rotation_z(roll);
    right = vec_normalize(rotation * vec4f{1, 0, 0, 0});
    up = vec_normalize(rotation * vec4f{0, 1, 0, 0});
    forward = vec_normalize(rotation * vec4f{0, 0, -1, 0});
}
