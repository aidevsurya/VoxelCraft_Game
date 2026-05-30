#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "../include/raylib.h"

// Re-encapsulating our menu layout structures inside our module space
struct MenuCloudParticle {
    Vector2 position;
    float speedY;
    float size;
    Color color;
};

class MainMenu {
private:
    static const int MAX_PARTICLES = 30;
    MenuCloudParticle particles[MAX_PARTICLES];
    
    Texture2D panorama;
    bool useTextureBg;
    
    float time;
    float scrollOffset;
    
    // UI Interaction Tracking Color Buffers
    Color btnStartBg;
    Color btnExitBg;
    Color btnStartTextCol;
    Color btnExitTextCol;

public:
    bool shouldStartGame;
    bool shouldExitGame;

    MainMenu();
    ~MainMenu() = default;

    void Init(int screenWidth, int screenHeight);
    void Update(int screenWidth, int screenHeight);
    void Draw(int screenWidth, int screenHeight);
    void Unload();
};

#endif // MAIN_MENU_H