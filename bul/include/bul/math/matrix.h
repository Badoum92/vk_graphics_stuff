#pragma once

#include "bul/math/vector.h"

namespace bul
{
struct mat4f
{
    union
    {
        float data[16];
        vec4f cols[4];
    };

    mat4f();
    explicit mat4f(float val);
    explicit mat4f(const float (&values)[16]);

    mat4f(const mat4f& other);

    mat4f& operator=(const mat4f& other);

    static mat4f identity();

    void operator+=(const mat4f& other);
    void operator-=(const mat4f& other);
    void operator*=(float val);

    bool operator==(const mat4f& other) const;

    vec4f& operator[](size_t i);
    vec4f operator[](size_t i) const;
};

mat4f operator+(const mat4f& a,const mat4f& b);
mat4f operator-(const mat4f& a,const mat4f& b);
mat4f operator*(const mat4f& a,const mat4f& b);
mat4f operator*(const mat4f& a, float f);
vec4f operator*(const mat4f& a, vec4f v);

mat4f inverse(const mat4f& m);
mat4f lookat(bul::vec3f pos, bul::vec3f target, bul::vec3f up, mat4f* inv = nullptr);
mat4f perspective(float fov, float aspect_ratio, float near, float far, mat4f* inv = nullptr);
} // namespace bul
