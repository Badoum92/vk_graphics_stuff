#include "camera.hh"

#include <cmath>
#include <algorithm>
#include <bul/window.h>

#include "time.hh"

#undef near
#undef far

Camera::Camera(const glm::vec3& pos, const glm::vec3& world_up, float yaw, float pitch, float near, float far)
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
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    dir.y = sin(glm::radians(pitch_));
    dir.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(dir);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
    front_straight_ = glm::normalize(glm::cross(world_up_, right_));
}

const glm::mat4& Camera::get_view()
{
    if (recompute_view_)
    {
        compute_view();
        recompute_view_ = false;
    }
    return view_;
}

const glm::mat4& Camera::get_inv_view()
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
    view_ = glm::lookAt(pos_, pos_ + front_, up_);
    inv_view_ = glm::inverse(view_);
}

const glm::mat4& Camera::get_proj()
{
    if (recompute_proj_)
    {
        compute_proj();
        recompute_proj_ = false;
    }
    return proj_;
}

const glm::mat4& Camera::get_inv_proj()
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
        float fov = glm::radians(fov_);
        proj_ = glm::perspective(fov, aspect_ratio_, far_, near_);
    }
    else if (proj_type_ == ProjectionType::ORTHOGRAPHIC)
    {
        float v = ortho_size_ * 0.5f;
        float h = ortho_size_ * aspect_ratio_ * 0.5f;
        proj_ = glm::ortho(-h, h, -v, v, near_, far_);
    }
    proj_[1][1] *= -1;
    inv_proj_ = glm::inverse(proj_);
}

const glm::mat4& Camera::get_view_proj()
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
    dir_ = glm::ivec3(0);
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

    if (dir_ == glm::ivec3(0))
        return;

    auto dir = glm::normalize(glm::vec3{dir_});
    dir *= speed_ * Time::delta_time();
    pos_ += dir.x * right_;
    pos_ += dir.y * world_up_;
    pos_ += dir.z * front_straight_;
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

void Camera::set_position(const glm::vec3& position)
{
    pos_ = position;
}

float Camera::get_speed() const
{
    return speed_;
}

const glm::vec3& Camera::get_pos() const
{
    return pos_;
}

const glm::vec3& Camera::get_front() const
{
    return front_;
}

const glm::vec3& Camera::get_up() const
{
    return up_;
}

const glm::vec3& Camera::get_right() const
{
    return right_;
}

bool Camera::is_moving() const
{
    return recompute_view_;
}
