#pragma once

#include "../Vector2/vector2.h"
#include "../Screen/screen.h"

class Camera {
    public:
    static Camera& GetInstance() {
        static Camera instance;
        return instance;
    };

    void SetZoom(float zoom) {
        this->zoom = zoom;
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
        return (worldPosition - position) * zoom + screenCenter;
    }

    Vector2 ScreenToWorld(Vector2 screenPosition) {
        Vector2 screenCenter = Screen::GetInstance().GetSize() / 2.0;
        return (screenPosition - screenCenter) / zoom + position;
    }

    float GetZoomOnScreen(float scale) {
        return zoom * scale;
    }

    float GetZoom() {
        return this->zoom;
    }

    protected:
    float zoom = 1;
    Vector2 position;

private:
    Camera() {};
    Vector2 screen;
};