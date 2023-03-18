#pragma once

#include <bul/math/matrix.h>
#include <bul/math/vector.h>

class Camera
{
public:
    enum class ProjectionType
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    Camera(const bul::vec3f& pos = {0.0f, 0.0f, 0.0f}, const bul::vec3f& world_up = {0.0f, 1.0f, 0.0f}, float yaw = -90.0f,
           float pitch = 0.0f, float near = 0.001f, float far = 1000.0f);

    void update_vectors();
    const bul::mat4f& get_view();
    const bul::mat4f& get_inv_view();
    void compute_view();
    const bul::mat4f& get_proj();
    const bul::mat4f& get_inv_proj();
    void compute_proj();
    const bul::mat4f& get_view_proj();

    void set_aspect_ratio(float width, float height);
    void set_clip(float near, float far);
    void set_perspective(float fov);
    void set_orthographic(float ortho_size);

    float get_fov() const;
    float get_ortho_size() const;

    void update();

    void set_speed(float speed);
    void set_sensitivity(float sensitivity);
    void set_position(const bul::vec3f& position);

    float get_speed() const;

    const bul::vec3f& get_pos() const;
    const bul::vec3f& get_front() const;
    const bul::vec3f& get_up() const;
    const bul::vec3f& get_right() const;

    bool is_moving() const;

protected:
    void update_pos();
    void update_view();

    bul::vec3f pos_;
    bul::vec3f world_up_;
    float yaw_;
    float pitch_;
    bul::vec3f front_;
    bul::vec3f front_straight_;
    bul::vec3f up_;
    bul::vec3f right_;

    float width_;
    float height_;
    float aspect_ratio_;

    float near_;
    float far_;

    float fov_ = 90.0f;
    float ortho_size_ = 90.0f;

    bul::mat4f view_;
    bul::mat4f inv_view_;
    bul::mat4f proj_;
    bul::mat4f inv_proj_;
    bul::mat4f view_proj_;
    bool recompute_view_ = true;
    bool recompute_proj_ = true;

    ProjectionType proj_type_ = ProjectionType::PERSPECTIVE;

    bul::vec3i dir_{0, 0, 0};

    float speed_ = 50.0f;
    float sensitivity_ = 0.1f;

    float last_x_;
    float last_y_;
};
