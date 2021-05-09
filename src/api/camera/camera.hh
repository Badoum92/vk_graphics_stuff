#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
    enum class ProjectionType
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    Camera(const glm::vec3& pos = {0.0f, 0.0f, 0.0f}, const glm::vec3& world_up = {0.0f, 1.0f, 0.0f}, float yaw = 90.0f,
           float pitch = 0.0f, float near = 0.1f, float far = 1000.0f);

    void update_vectors();
    const glm::mat4& get_view();
    const glm::mat4& get_inv_view();
    void compute_view();
    const glm::mat4& get_proj();
    const glm::mat4& get_inv_proj();
    void compute_proj();
    const glm::mat4& get_view_proj();

    void set_aspect_ratio(float width, float height);
    void set_clip(float near, float far);
    void set_perspective(float fov);
    void set_orthographic(float ortho_size);

    void on_update();
    void on_key_event(int key, int action);
    void on_mouse_moved(float x, float y);

    void set_last_x_y(float x, float y);
    void set_speed(float speed);
    void set_sensitivity(float sensitivity);

    const glm::vec3& get_pos() const;

protected:
    glm::vec3 pos_;
    glm::vec3 world_up_;
    float yaw_ = 90.0f;
    float pitch_ = 0.0f;
    glm::vec3 front_;
    glm::vec3 front_straight_;
    glm::vec3 up_;
    glm::vec3 right_;

    float width_;
    float height_;
    float aspect_ratio_;

    float near_;
    float far_;

    float fov_ = 90.0f;
    float ortho_size_ = 10.0f;

    glm::mat4 view_;
    glm::mat4 inv_view_;
    glm::mat4 proj_;
    glm::mat4 inv_proj_;
    glm::mat4 view_proj_;
    bool recompute_view_ = true;
    bool recompute_proj_ = true;

    ProjectionType proj_type_ = ProjectionType::PERSPECTIVE;

    glm::ivec3 dir_{0, 0, 0};

    float speed_ = 75.0f;
    float sensitivity_ = 0.1f;

    float last_x_;
    float last_y_;
};
