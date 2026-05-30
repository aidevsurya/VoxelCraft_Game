#ifndef PLAYER_H
#define PLAYER_H

#include "../include/raylib.h"
#include "world.h"

class Player {
private:
    Vector3 velocity;
    Vector2 cameraAngle; 
    
    float moveSpeed;
    float jumpForce;
    float gravity;
    float mouseSensitivity;

    float playerWidth;
    float playerHeight;

    Model axeModel;
    bool hasWeaponModel;
    float swingTimer;
    bool isSwinging;

    float maxReachDistance;

    // TARGET HIGHLIGHT VARIABLES
    bool hasTargetBlock;
    Vector3 targetBlockPos;

    bool CheckCollisionAt(Vector3 pos, World* world);
    void HandleBlockInteraction(Camera3D* camera, World* world);

public:
    Vector3 position;
    bool isGrounded;

    Player();
    ~Player() = default;

    void Init(Vector3 spawnPos);
    void Update(float dt, Camera3D* camera, World* world);
    void DrawWeapon(); 
    void DrawTargetHighlight(); // Renders the selection box around highlighted blocks
    void Unload();
};

#endif // PLAYER_H