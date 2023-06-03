#include "camera.h"

#include <cmath>
#include <algorithm>
#include <bul/window.h>
#include <bul/time.h>
#include <bul/math/math.h>

#undef near
#undef far

Camera::Camera(const bul::vec3f& pos, const bul::vec3f& world_up, float yaw, float pitch, float near, float far)
{
    pos_ = pos;
    world_up_ = world_up;
    yaw_ = yaw;
    pitch_ = pitch;
    near_ = near;
    far_ = far;
    update_vectors();

    width_ = bul::window::size().x;
    height_ = bul::window::size().y;
    aspect_ratio_ = bul::window::aspect_ratio();
}

void Camera::update_vectors()
{
    bul::vec3f dir;
    dir.x = cos(bul::radians(yaw_)) * cos(bul::radians(pitch_));
    dir.y = sin(bul::radians(pitch_));
    dir.z = sin(bul::radians(yaw_)) * cos(bul::radians(pitch_));
    front_ = bul::normalize(dir);
    right_ = bul::normalize(bul::cross(front_, world_up_));
    up_ = bul::normalize(bul::cross(right_, front_));
    front_straight_ = bul::normalize(bul::cross(world_up_, right_));
}

const bul::mat4f& Camera::get_view()
{
    if (recompute_view_)
    {
        compute_view();
        recompute_view_ = false;
    }
    return view_;
}

const bul::mat4f& Camera::get_inv_view()
{
    if (recompute_view_)
    {
        compute_view();
        recompute_view_ = false;
    }
    return inv_view_;
}

void Camera::compute_view()
{
    view_ = bul::lookat(pos_, pos_ + front_, up_, &inv_view_);
}

const bul::mat4f& Camera::get_proj()
{
    if (recompute_proj_)
    {
        compute_proj();
        recompute_proj_ = false;
    }
    return proj_;
}

const bul::mat4f& Camera::get_inv_proj()
{
    if (recompute_proj_)
    {
        compute_proj();
        recompute_proj_ = false;
    }
    return inv_proj_;
}

void Camera::compute_proj()
{
    if (proj_type_ == ProjectionType::PERSPECTIVE)
    {
        float fov = bul::radians(fov_);
        proj_ = bul::perspective(fov, aspect_ratio_, far_, near_, &inv_proj_);
    }
    else if (proj_type_ == ProjectionType::ORTHOGRAPHIC)
    {
        // float v = ortho_size_ * 0.5f;
        // float h = ortho_size_ * aspect_ratio_ * 0.5f;
        // proj_ = bul::ortho(-h, h, -v, v, near_, far_); // FIXME
    }
}

const bul::mat4f& Camera::get_view_proj()
{
    view_proj_ = get_proj() * get_view();
    return view_proj_;
}

void Camera::set_aspect_ratio(float width, float height)
{
    width_ = width;
    height_ = height;
    aspect_ratio_ = width_ / height_;
    recompute_proj_ = true;
}

void Camera::set_clip(float near, float far)
{
    near_ = near;
    far_ = far;
    recompute_proj_ = true;
}

void Camera::set_perspective(float fov)
{
    fov_ = fov;
    proj_type_ = ProjectionType::PERSPECTIVE;
    recompute_proj_ = true;
}

void Camera::set_orthographic(float ortho_size)
{
    ortho_size_ = ortho_size;
    proj_type_ = ProjectionType::ORTHOGRAPHIC;
    recompute_proj_ = true;
}

float Camera::get_fov() const
{
    return fov_;
}

float Camera::get_ortho_size() const
{
    return ortho_size_;
}

void Camera::update_pos()
{
    dir_ = bul::vec3i(0);
    if (bul::key_down(bul::Key::Z))
        dir_.z += 1;
    if (bul::key_down(bul::Key::S))
        dir_.z -= 1;
    if (bul::key_down(bul::Key::Q))
        dir_.x -= 1;
    if (bul::key_down(bul::Key::D))
        dir_.x += 1;
    if (bul::key_down(bul::Key::Space))
        dir_.y += 1;
    if (bul::key_down(bul::Key::C))
        dir_.y -= 1;

    if (dir_ == bul::vec3i(0))
        return;

    auto dir = bul::normalize(bul::vec3f{(float)dir_.x, (float)dir_.y, (float)dir_.z});
    dir *= speed_ * bul::time::delta_s();
    pos_ += right_ * dir.x;
    pos_ += world_up_ * dir.y;
    pos_ += front_straight_ * dir.z;
    recompute_view_ = true;
}

void Camera::update_view()
{
    static bul::vec2i last_pos = bul::window::cursor_pos();
    if (bul::window::cursor_visible())
        return;

    bul::vec2i pos = bul::window::cursor_pos();
    if (last_pos == pos)
        return;

    float x_offset = pos.x - last_pos.x;
    float y_offset = last_pos.y - pos.y;
    last_pos.x = pos.x;
    last_pos.y = pos.y;

    x_offset *= sensitivity_;
    y_offset *= sensitivity_;

    yaw_ += x_offset;
    pitch_ += y_offset;

    pitch_ = std::min(pitch_, 89.0f);
    pitch_ = std::max(pitch_, -89.0f);

    update_vectors();
    recompute_view_ = true;
}

void Camera::update()
{
    update_pos();
    update_view();
}

void Camera::set_speed(float speed)
{
    speed_ = speed;
}

void Camera::set_sensitivity(float sensitivity)
{
    sensitivity_ = sensitivity;
}

void Camera::set_position(const bul::vec3f& position)
{
    pos_ = position;
}

float Camera::get_speed() const
{
    return speed_;
}

const bul::vec3f& Camera::get_pos() const
{
    return pos_;
}

const bul::vec3f& Camera::get_front() const
{
    return front_;
}

const bul::vec3f& Camera::get_up() const
{
    return up_;
}

const bul::vec3f& Camera::get_right() const
{
    return right_;
}

bool Camera::is_moving() const
{
    return recompute_view_;
}
