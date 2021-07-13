#pragma once

class GameSettings {
public:
protected:
    float WindowWidth = 1600.0f;
    float WindowHeight = 900.0f;
    float WindowAspectRatio = WindowWidth / WindowHeight;
    float Fov = 70.0f;
    bool InvertMouseY = false;
    bool InvertMouseX = false;
    bool vsync = false;
private:
};
