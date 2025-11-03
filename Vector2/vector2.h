#pragma once

#include <math.h>

class Vector2 {
    public:
    ~Vector2() = default;
    float x = 0.0f, y = 0.0f;

    Vector2() {
        this->x = 0.0f;
        this->y = 0.0f;
    }

    Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    void Normalize() {
        float length = std::sqrtf(this->x * this->x + this->y * this->y);
        if (length <= 0.0001f) /* Too small :( */
            return;

        this->x /= length;
        this->y /= length;
    }

    float Length() {
        return std::sqrtf(this->x * this->x + this->y * this->y);
    }
    
    float Dot(Vector2 vec) {
        return (x * vec.x + y * vec.y);
    }

    /* Gets the angle between 2 vectors |
    source.Angle(to); */
    float Angle(Vector2 to){
        return std::acos(Dot(to) / (Length() * to.Length()));
    }

    Vector2& operator + (Vector2 vec) {
        this->x += vec.x;
        this->y += vec.y;
        return *this;
    };

    Vector2& operator - (Vector2 vec) {
        this->x -= vec.x;
        this->y -= vec.y;
        return *this;
    };

    Vector2& operator * (float scalar) {
        this->x *= scalar;
        this->y *= scalar;
        return *this;
    };

    Vector2& operator += (Vector2 vec) {
        this->x += vec.x;
        this->y += vec.y;
        return *this;
    };

    Vector2& operator -= (Vector2 vec) {
        this->x -= vec.x;
        this->y -= vec.y;
        return *this;
    };

    Vector2& operator *= (float scalar) {
        this->x *= scalar;
        this->y *= scalar;
        return *this;
    }

    Vector2& operator / (float scalar) {
        this->x /= scalar;
        this->y /= scalar;
        return *this;
    };

    Vector2& operator /= (float scalar) {
        this->x /= scalar;
        this->y /= scalar;
        return *this;
    };
};