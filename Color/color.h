#pragma once

class Color {
    public:
    int r, g, b, a;
    Color();
    Color(int r, int g, int b, int a);
};

inline Color::Color() {
    r = 0.0;
    g = 0.0;
    b = 0.0;
    a = 255.0;
};

inline Color::Color(int r, int g, int b, int a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
};