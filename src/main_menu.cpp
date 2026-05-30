#include "main_menu.h"
#include <math.h>

MainMenu::MainMenu() {
    time = 0.0f;
    scrollOffset = 0.0f;
    useTextureBg = false;
    shouldStartGame = false;
    shouldExitGame = false;

    btnStartBg = Color{ 100, 100, 100, 255 };
    btnExitBg  = Color{ 100, 100, 100, 255 };
    btnStartTextCol = Color{ 224, 224, 224, 255 };
    btnExitTextCol  = Color{ 224, 224, 224, 255 };
}

void MainMenu::Init(int screenWidth, int screenHeight) {
    // Load panoramic menu assets
    panorama = LoadTexture("panorama.png");
    useTextureBg = (panorama.id > 0);
    if (useTextureBg) {
        SetTextureFilter(panorama, TEXTURE_FILTER_POINT);
    }

    // Proportional uniform dynamic scaling factor 
    float minScale = ((float)screenWidth / 1920.0f < (float)screenHeight / 1080.0f) 
                     ? (float)screenWidth / 1920.0f 
                     : (float)screenHeight / 1080.0f;

    // Initialize blocky retro sky cloud particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].position = { (float)GetRandomValue(0, screenWidth), (float)GetRandomValue(0, screenHeight) };
        particles[i].speedY = (float)GetRandomValue(-15, -5) / 10.0f;
        particles[i].size = (float)GetRandomValue(12, 28) * minScale;
        
        if (i % 2 == 0) {
            particles[i].color = Fade(Color{ 255, 255, 255, 255 }, 0.12f);
        } else {
            particles[i].color = Fade(Color{ 130, 180, 230, 255 }, 0.18f);
        }
    }
}

void MainMenu::Update(int screenWidth, int screenHeight) {
    float dt = GetFrameTime();
    time += dt;
    scrollOffset -= 45.0f * dt; // Match frame rate shifts perfectly

    Vector2 mousePos = GetMousePosition();
    
    // Recalculate layout scales automatically on structural sizes
    float minScale = ((float)screenWidth / 1920.0f < (float)screenHeight / 1080.0f) 
                     ? (float)screenWidth / 1920.0f 
                     : (float)screenHeight / 1080.0f;

    float btnWidth = 400.0f * minScale;
    float btnHeight = 60.0f * minScale;
    float btnX = (float)screenWidth / 2.0f - (btnWidth / 2.0f);
    
    Rectangle btnStart = { btnX, (float)screenHeight * 0.55f, btnWidth, btnHeight };
    Rectangle btnExit  = { btnX, (float)screenHeight * 0.63f + 12.0f * minScale, btnWidth, btnHeight };

    // Reset button design fallback matrices
    btnStartBg = Color{ 100, 100, 100, 255 };
    btnExitBg  = Color{ 100, 100, 100, 255 };
    btnStartTextCol = Color{ 224, 224, 224, 255 };
    btnExitTextCol  = Color{ 224, 224, 224, 255 };

    // Click/Hover Collisions
    if (CheckCollisionPointRec(mousePos, btnStart)) {
        btnStartBg = Color{ 140, 140, 220, 255 };
        btnStartTextCol = Color{ 255, 255, 160, 255 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) shouldStartGame = true;
    }
    
    if (CheckCollisionPointRec(mousePos, btnExit)) {
        btnExitBg = Color{ 140, 140, 220, 255 };
        btnExitTextCol = Color{ 255, 255, 160, 255 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) shouldExitGame = true;
    }

    // Animate structural floating retro cloud items
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].position.y += particles[i].speedY;
        if (particles[i].position.y < -40) {
            particles[i].position.y = (float)screenHeight + 40;
            particles[i].position.x = (float)GetRandomValue(0, screenWidth);
        }
    }
}

void MainMenu::Draw(int screenWidth, int screenHeight) {
    float minScale = ((float)screenWidth / 1920.0f < (float)screenHeight / 1080.0f) 
                     ? (float)screenWidth / 1920.0f 
                     : (float)screenHeight / 1080.0f;

    BeginDrawing();
        // Render background layers (Panorama or plain solid gradients fallback)
        if (useTextureBg) {
            float scaledWidth = (float)panorama.width * ((float)screenHeight / (float)panorama.height);
            if (scrollOffset <= -scaledWidth) scrollOffset = 0.0f;
            
            DrawTexturePro(panorama, Rectangle{ 0, 0, (float)panorama.width, (float)panorama.height }, Rectangle{ scrollOffset, 0, scaledWidth, (float)screenHeight }, Vector2{ 0, 0 }, 0.0f, WHITE);
            DrawTexturePro(panorama, Rectangle{ 0, 0, (float)panorama.width, (float)panorama.height }, Rectangle{ scrollOffset + scaledWidth, 0, scaledWidth, (float)screenHeight }, Vector2{ 0, 0 }, 0.0f, WHITE);
            DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.1f), Fade(BLACK, 0.4f));
        } else {
            DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{ 116, 172, 247, 255 }, Color{ 66, 106, 179, 255 });
        }

        // Draw floating background cloud items
        for (int i = 0; i < MAX_PARTICLES; i++) {
            DrawRectangle((int)particles[i].position.x, (int)particles[i].position.y, (int)particles[i].size, (int)particles[i].size, particles[i].color);
        }

        // --- THE BOUNCING VOXEL ANIMATION ---
        float bounce = fabsf(sinf(time * 4.0f)) * ((float)screenHeight * 0.08f);
        float squash = 1.0f + sinf(time * 8.0f) * 0.10f;
        float wobble = sinf(time * 10.0f) * 12.0f;
        float blockSize = 120.0f * minScale;
        Vector2 blockPos = { (float)screenWidth / 2.0f, ((float)screenHeight * 0.33f) - bounce };
        
        // Voxel soft drop shadow layout
        DrawEllipse(screenWidth / 2, (int)((float)screenHeight * 0.33f + blockSize / 2.0f), (int)((65.0f * minScale) - (bounce * 0.15f)), (int)(10 * minScale), Fade(BLACK, 0.35f));

        // Sharp Dirt Core (Brown)
        DrawRectanglePro(Rectangle{ blockPos.x, blockPos.y, blockSize * squash, blockSize / squash }, Vector2{ (blockSize * squash) / 2.0f, (blockSize / squash) / 2.0f }, wobble, Color{ 134, 91, 62, 255 });
        // Sharp Grass Cap (Green Top)
        float grassHeight = 26.0f * minScale;
        DrawRectanglePro(Rectangle{ blockPos.x, blockPos.y - ((blockSize / squash) / 2.0f) + (grassHeight / 2.0f), blockSize * squash, grassHeight }, Vector2{ (blockSize * squash) / 2.0f, grassHeight / 2.0f }, wobble, Color{ 90, 190, 70, 255 });
        
        // Pulsing splash text
        int splashSize = (int)(22.0f * minScale);
        const char* splashText = "STITCHING 3 TEXTURES LIVE!";
        float splashScale = 1.0f + sinf(time * 6.0f) * 0.08f;
        DrawTextPro(GetFontDefault(), splashText, Vector2{ (float)screenWidth / 2.0f, (float)screenHeight * 0.22f }, Vector2{ (float)MeasureText(splashText, splashSize) / 2.0f, (float)splashSize / 2.0f }, sinf(time * 3.0f) * 5.0f, splashSize * splashScale, 2, YELLOW);

        // Header Title (Drop Shadow + Front Text)
        int titleSize = (int)(65.0f * minScale);
        DrawText("MINECRAFT: RAYLIB", screenWidth / 2 - MeasureText("MINECRAFT: RAYLIB", titleSize) / 2 + 5, (int)(screenHeight * 0.08f) + 5, titleSize, Color{ 40, 40, 40, 255 });
        DrawText("MINECRAFT: RAYLIB", screenWidth / 2 - MeasureText("MINECRAFT: RAYLIB", titleSize) / 2, (int)(screenHeight * 0.08f), titleSize, WHITE);
        
        // Render beveled button interface elements
        float btnWidth = 400.0f * minScale;
        float btnHeight = 60.0f * minScale;
        float btnX = (float)screenWidth / 2.0f - (btnWidth / 2.0f);
        Rectangle btnStart = { btnX, (float)screenHeight * 0.55f, btnWidth, btnHeight };
        Rectangle btnExit  = { btnX, (float)screenHeight * 0.63f + 12.0f * minScale, btnWidth, btnHeight };

        DrawRectangleRec(btnStart, btnStartBg);
        DrawRectangleLinesEx(btnStart, 3 * minScale, Color{ 50, 50, 50, 255 });
        DrawText("Play Game", (int)(btnStart.x + (btnStart.width - MeasureText("Play Game", (int)(24 * minScale))) / 2), (int)(btnStart.y + (btnStart.height - 24 * minScale) / 2), (int)(24 * minScale), btnStartTextCol);

        DrawRectangleRec(btnExit, btnExitBg);
        DrawRectangleLinesEx(btnExit, 3 * minScale, Color{ 50, 50, 50, 255 });
        DrawText("Quit", (int)(btnExit.x + (btnExit.width - MeasureText("Quit", (int)(24 * minScale))) / 2), (int)(btnExit.y + (btnExit.height - 24 * minScale) / 2), (int)(24 * minScale), btnExitTextCol);
    EndDrawing();
}

void MainMenu::Unload() {
    if (useTextureBg) {
        UnloadTexture(panorama);
        useTextureBg = false;
    }
}