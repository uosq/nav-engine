#pragma once

#include <cmath>

class Vector2 {
public:
    float x = 0.0f;
    float y = 0.0f;

    Vector2() = default;
    Vector2(float x, float y) : x(x), y(y) {}
    ~Vector2() = default;

    void Normalize() {
        float length = Length();
        if (length <= 0.0001f)
            return;
        x /= length;
        y /= length;
    }

    float Length() const {
        return std::sqrtf(x * x + y * y);
    }

    float Dot(const Vector2& vec) const {
        return x * vec.x + y * vec.y;
    }

    float Angle(const Vector2& to) const {
        return std::acos(Dot(to) / (Length() * to.Length()));
    }

    void Lerp(const Vector2& to, float time) {
        x = x + (to.x - x) * time;
        y = y + (to.y - y) * time;
    }

    // Non-mutating arithmetic
    Vector2 operator+(const Vector2& vec) const {
        return Vector2(x + vec.x, y + vec.y);
    }

    Vector2 operator-(const Vector2& vec) const {
        return Vector2(x - vec.x, y - vec.y);
    }

    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    // Compound assignment (mutating)
    Vector2& operator+=(const Vector2& vec) {
        x += vec.x;
        y += vec.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& vec) {
        x -= vec.x;
        y -= vec.y;
        return *this;
    }

    Vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    bool operator==(const Vector2& v) const {
        return x == v.x && y == v.y;
    }

    bool operator!=(const Vector2& v) const {
        return !(*this == v);
    }

};
