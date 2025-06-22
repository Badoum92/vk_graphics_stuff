#include "camera.h"

#include "core/math/math.h"

camera camera_create()
{
    camera camera;
    camera.right = WORLD_RIGHT;
    camera.up = WORLD_UP;
    camera.forward = WORLD_FORWARD;
    return camera;
}

void camera_compute_view_proj(camera* camera)
{
    camera->view = mat4_lookat(camera->position, camera->position + camera->forward, camera->up, &camera->inv_view);
    camera->proj =
        mat4_perspective(camera->fov_y, camera->aspect_ratio, camera->near_plane, camera->far_plane, &camera->inv_proj);
}

void camera_rotate(camera* camera, vec3f angles)
{
    if (angles == vec3f{0, 0, 0})
    {
        return;
    }

    camera->pitch += angles.x;
    camera->yaw += angles.y;
    camera->roll += angles.z;

    camera->pitch = math_clamp(-math_half_pi + 0.01f, camera->pitch, math_half_pi - 0.01f);

    mat4f rotation = mat4_rotation_x(camera->pitch) * mat4_rotation_y(camera->yaw) * mat4_rotation_z(camera->roll);
    camera->right = vec_normalize(rotation * vec4f{1, 0, 0, 0});
    camera->up = vec_normalize(rotation * vec4f{0, 1, 0, 0});
    camera->forward = vec_normalize(rotation * vec4f{0, 0, -1, 0});
}
