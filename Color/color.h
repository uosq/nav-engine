#pragma once

class Color {
    public:
    float r, g, b, a;
    Color();
    Color(float r, float g, float b, float a);
};

inline Color::Color() {
    r = 0.0;
    g = 0.0;
    b = 0.0;
    a = 255.0;
};

inline Color::Color(float r, float g, float b, float a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
};