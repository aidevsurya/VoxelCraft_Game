#include "../include/raylib.h"
#include "../include/raymath.h"
#include "game_states.h"
#include "main_menu.h"
#include "world.h"
#include "player.h"

int main() {
    InitWindow(100, 100, "VoxelCraft - Physics Integrated Alpha");
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);
    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();
    SetTargetFPS(60);

    SetTextureFilter(GetFontDefault().texture, TEXTURE_FILTER_POINT);

    GameState currentState = STATE_MAIN_MENU;
    
    MainMenu mainMenu;
    World world;
    Player player; // Instantiate our clean modular component class object
    
    Camera3D camera = { 0 };
    mainMenu.Init(screenWidth, screenHeight);
    bool isWorldLoaded = false;

    while (currentState != STATE_EXIT && !WindowShouldClose()) {
        switch (currentState) {
            case STATE_MAIN_MENU: {
                mainMenu.Update(screenWidth, screenHeight);
                mainMenu.Draw(screenWidth, screenHeight);

                if (mainMenu.shouldStartGame) {
                    currentState = STATE_GAMEPLAY;
                    mainMenu.Unload(); 
                } else if (mainMenu.shouldExitGame) {
                    currentState = STATE_EXIT;
                }
            } break;

            case STATE_GAMEPLAY: {
                if (!isWorldLoaded) {
                    world.Init();
                    
                    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
                    camera.fovy = 75.0f;
                    camera.projection = CAMERA_PERSPECTIVE;

                    // Initialize character coordinates safely suspended in the air
                    player.Init(Vector3{ 8.0f, 14.0f, 8.0f });
                    
                    DisableCursor(); 
                    isWorldLoaded = true;
                }

                float dt = GetFrameTime();
                if (dt > 0.1f) dt = 0.1f;

                // Fire the complete self-contained movement input and AABB physics routine loop
                player.Update(dt, &camera, &world);
                world.Update(player.position);

                BeginDrawing();
                    ClearBackground(Color{ 145, 195, 255, 255 }); 

                    BeginMode3D(camera);
                        world.Draw();        
                        player.DrawTargetHighlight(); // FIX: Injects the block selection wireframe highlight!
                        player.DrawWeapon(); 
                    EndMode3D();

                    // Crosshair Overlay
                    DrawRectangle(screenWidth / 2 - 2, screenHeight / 2 - 12, 4, 24, WHITE);
                    DrawRectangle(screenWidth / 2 - 12, screenHeight / 2 - 2, 24, 4, WHITE);

                    // HUD Diagnostics Overlay
                    DrawRectangle(15, 15, 360, 110, Fade(BLACK, 0.45f));
                    DrawText(TextFormat("FPS: %i", GetFPS()), 25, 25, 20, GREEN);
                    DrawText(TextFormat("Pos: X:%.2f Y:%.2f Z:%.2f", player.position.x, player.position.y, player.position.z), 25, 50, 16, WHITE);
                    DrawText(TextFormat("Physics State: %s", player.isGrounded ? "GROUNDED" : "FALLING / AIR"), 25, 75, 14, LIGHTGRAY);
                EndDrawing();
            } break;

            case STATE_EXIT:
                break;
        }
    }

    if (currentState == STATE_MAIN_MENU) mainMenu.Unload();
    if (isWorldLoaded) world.Unload();
    
    CloseWindow();
    return 0;
}