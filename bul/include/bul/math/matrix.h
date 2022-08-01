#pragma once

#include "bul/math/vector.h"

namespace bul
{
struct mat4f
{
    float data[16];

    mat4f();
    explicit mat4f(float val);
    explicit mat4f(const float values[16]);

    mat4f(const mat4f& other);

    mat4f& operator=(const mat4f& other);

    static mat4f identity();

    void operator+=(const mat4f& other);
    void operator-=(const mat4f& other);
    void operator*=(float val);

    bool operator==(const mat4f& other) const;

    float& operator[](size_t i);
    float operator[](size_t i) const;
};

mat4f operator+(const mat4f& a,const mat4f& b);
mat4f operator-(const mat4f& a,const mat4f& b);
mat4f operator*(const mat4f& a,const mat4f& b);
mat4f operator*(const mat4f& a, float f);
vec4f operator*(const mat4f& a, vec4f v);
} // namespace bul
