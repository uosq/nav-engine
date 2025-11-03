#pragma once

#include "../Vector2/vector2.h"
#include "../Screen/screen.h"

class Camera {
    public:
    static Camera& GetInstance() {
        static Camera instance;
        return instance;
    };

    void SetScale(float scale) {
        this->scale = scale;
    }

    Vector2 GetPosition() {
        return this->position;
    }

    void SetPosition(Vector2 position) {
        this->position = position;
    }

    void SetScreen(Vector2 position) {
        screen = position;
    }

    Vector2 GetScreen() {
        return screen;
    }

    Vector2 WorldToScreen(Vector2 worldPosition) {
        Vector2 screenCenter = Screen::GetInstance().GetSize() / 2.0;
        return (worldPosition - position) * scale + screenCenter;
    }

    Vector2 ScreenToWorld(Vector2 screenPosition) {
        Vector2 screenCenter = Screen::GetInstance().GetSize() / 2.0;
        return (screenPosition - screenCenter) / scale + position;
    }

    float GetScaleOnScreen(float scaleThis) {
        return scale * scaleThis;
    }

    float GetScale() {
        return this->scale;
    }

    protected:
    float scale = 1;
    Vector2 position;

private:
    Camera() {};
    Vector2 screen;
};