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
    camera->view_proj = camera->proj * camera->view;
    camera->inv_view_proj = camera->inv_view * camera->inv_proj;
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

    camera->pitch = CLAMP(camera->pitch, -math_half_pi + 0.01f, math_half_pi - 0.01f);

    mat4f rotation = mat4_rotation_x(camera->pitch) * mat4_rotation_y(camera->yaw) * mat4_rotation_z(camera->roll);
    camera->right = vec_normalize(vec4f{1, 0, 0, 0} * rotation);
    camera->up = vec_normalize(vec4f{0, 1, 0, 0} * rotation);
    camera->forward = vec_normalize(vec4f{0, 0, -1, 0} * rotation);
}

vec3f camera_world_to_screen(camera const* camera, vec3f _point, vec2u resolution)
{
    vec4f point = {_point.x, _point.y, _point.z, 1.0f};
    point = camera->view_proj * point;
    point /= point.w;

    vec3f ret;
    ret.x = (point.x * 0.5f + 0.5f) * resolution.x;
    ret.y = (point.y * 0.5f + 0.5f) * resolution.y;
    ret.z = point.z;
    return ret;
}