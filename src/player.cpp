#include "player.h"
#include "../include/raymath.h"
#include <math.h>

Player::Player() {
    position = { 8.0f, 10.0f, 8.0f }; 
    velocity = { 0.0f, 0.0f, 0.0f };
    cameraAngle = { 0.0f, 0.0f };
    
    moveSpeed = 4.5f;
    jumpForce = 7.0f;  
    gravity = -16.0f;  
    mouseSensitivity = 0.05f;

    playerWidth = 0.6f;  
    playerHeight = 1.8f; 
    isGrounded = false;
    
    hasWeaponModel = false;
    isSwinging = false;
    swingTimer = 0.0f;

    maxReachDistance = 5.0f; 
    hasTargetBlock = false;
    targetBlockPos = { 0.0f, 0.0f, 0.0f };
}

void Player::Init(Vector3 spawnPos) {
    position = spawnPos;
    
    // Explicitly load the model here once during setup, ensuring it allocates correctly in VRAM
    if (FileExists("diamond_axe.glb")) {
        axeModel = LoadModel("diamond_axe.glb");
        if (axeModel.meshCount > 0) {
            if (axeModel.materialCount > 0) {
                SetTextureFilter(axeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture, TEXTURE_FILTER_POINT);
            }
            hasWeaponModel = true;
        }
    }
}

bool Player::CheckCollisionAt(Vector3 pos, World* world) {
    float minX = pos.x - (playerWidth / 2.0f) + 0.01f;
    float maxX = pos.x + (playerWidth / 2.0f) - 0.01f;
    float minY = pos.y - playerHeight + 0.01f; 
    float maxY = pos.y + 0.2f - 0.01f;         
    float minZ = pos.z - (playerWidth / 2.0f) + 0.01f;
    float maxZ = pos.z + (playerWidth / 2.0f) - 0.01f;

    int startX = (int)floorf(minX);
    int endX   = (int)floorf(maxX);
    int startY = (int)floorf(minY);
    int endY   = (int)floorf(maxY);
    int startZ = (int)floorf(minZ);
    int endZ   = (int)floorf(maxZ);

    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            for (int z = startZ; z <= endZ; z++) {
                if (world->IsBlockSolidAt(x, y, z)) return true; 
            }
        }
    }
    return false;
}

void Player::HandleBlockInteraction(Camera3D* camera, World* world) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hasTargetBlock) {
        Vector3 rayDirection = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
        Vector3 currentRayPos = camera->position;
        float stepDist = 0.05f;
        float traveled = 0.0f;

        while (traveled < maxReachDistance) {
            currentRayPos = Vector3Add(currentRayPos, Vector3Scale(rayDirection, stepDist));
            traveled += stepDist;

            int targetX = (int)floorf(currentRayPos.x);
            int targetY = (int)floorf(currentRayPos.y);
            int targetZ = (int)floorf(currentRayPos.z);

            if (world->IsBlockSolidAt(targetX, targetY, targetZ)) {
                Vector3 previousRayPos = Vector3Subtract(currentRayPos, Vector3Scale(rayDirection, stepDist));
                int placeX = (int)floorf(previousRayPos.x);
                int placeY = (int)floorf(previousRayPos.y);
                int placeZ = (int)floorf(previousRayPos.z);

                if (placeX == (int)floorf(position.x) && placeZ == (int)floorf(position.z) &&
                    (placeY == (int)floorf(position.y) || placeY == (int)floorf(position.y - 1.0f))) {
                    return; 
                }

                world->SetBlockAt(placeX, placeY, placeZ, 3); // Place TNT block
                world->Update(Vector3{999.0f, 999.0f, 999.0f}); 
                return;
            }
        }
    }
}

void Player::Update(float dt, Camera3D* camera, World* world) {
    // 1. Mouse Look Tracking
    Vector2 mouseDelta = GetMouseDelta();
    cameraAngle.x -= mouseDelta.x * mouseSensitivity * dt;
    cameraAngle.y -= mouseDelta.y * mouseSensitivity * dt;

    if (cameraAngle.y > 89.0f * DEG2RAD)  cameraAngle.y = 89.0f * DEG2RAD;
    if (cameraAngle.y < -89.0f * DEG2RAD) cameraAngle.y = -89.0f * DEG2RAD;

    Vector3 forward = { sinf(cameraAngle.x), 0.0f, cosf(cameraAngle.x) };
    Vector3 right = { cosf(cameraAngle.x), 0.0f, -sinf(cameraAngle.x) };

    // 2. Keyboard Input Layout (Inverted Strafe Configuration Kept Intact)
    Vector3 moveDirection = { 0, 0, 0 };
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) moveDirection = Vector3Add(moveDirection, forward);
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) moveDirection = Vector3Subtract(moveDirection, forward);
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) moveDirection = Vector3Add(moveDirection, right);      
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveDirection = Vector3Subtract(moveDirection, right); 

    if (Vector3Length(moveDirection) > 0.0f) moveDirection = Vector3Normalize(moveDirection);

    velocity.x = moveDirection.x * moveSpeed;
    velocity.z = moveDirection.z * moveSpeed;
    velocity.y += gravity * dt;

    // 3. Jump Physics Execution
    if (isGrounded && IsKeyPressed(KEY_SPACE)) {
        velocity.y = jumpForce;
        isGrounded = false;
    }

    // 4. CONTINUOUS TARGET POINTER RAYCAST (Highlights targeted blocks on every frame)
    hasTargetBlock = false;
    Vector3 rayDirection = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
    Vector3 checkRayPos = camera->position;
    float rayStep = 0.05f;
    float distCheck = 0.0f;

    while (distCheck < maxReachDistance) {
        checkRayPos = Vector3Add(checkRayPos, Vector3Scale(rayDirection, rayStep));
        distCheck += rayStep;

        int checkX = (int)floorf(checkRayPos.x);
        int checkY = (int)floorf(checkRayPos.y);
        int checkZ = (int)floorf(checkRayPos.z);

        if (world->IsBlockSolidAt(checkX, checkY, checkZ)) {
            hasTargetBlock = true;
            targetBlockPos = { (float)checkX, (float)checkY, (float)checkZ };
            break;
        }
    }

    // 5. Input Action Trigger for Axe Swing
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isSwinging) {
        isSwinging = true;
        swingTimer = 0.0f;
    }

    if (isSwinging) {
        float oldTimer = swingTimer;
        swingTimer += dt * 5.5f; 
        
        // Block destruction synchronization pass: triggers exactly at the mid-point of the axe swing
        if (oldTimer < 0.5f && swingTimer >= 0.5f && hasTargetBlock) {
            world->SetBlockAt((int)targetBlockPos.x, (int)targetBlockPos.y, (int)targetBlockPos.z, 0); // Destroy!
            world->Update(Vector3{999.0f, 999.0f, 999.0f}); 
        }

        if (swingTimer > 1.0f) {
            isSwinging = false;
            swingTimer = 0.0f;
        }
    }

    // 6. Traditional Axis-Separated AABB Movement Physics
    Vector3 nextPos = position;
    nextPos.x += velocity.x * dt;
    if (CheckCollisionAt(nextPos, world)) velocity.x = 0.0f; else position.x = nextPos.x;

    nextPos = position;
    nextPos.z += velocity.z * dt;
    if (CheckCollisionAt(nextPos, world)) velocity.z = 0.0f; else position.z = nextPos.z;

    isGrounded = false;
    nextPos = position;
    nextPos.y += velocity.y * dt;
    if (CheckCollisionAt(nextPos, world)) {
        if (velocity.y < 0.0f) isGrounded = true;
        velocity.y = 0.0f;
    } else {
        position.y = nextPos.y;
    }

    if (position.y < -30.0f) {
        position = { 8.0f, 10.0f, 8.0f };
        velocity = { 0.0f, 0.0f, 0.0f };
    }

    // 7. Camera Tracking Configuration
    camera->position = position;
    Vector3 targetOffset = { sinf(cameraAngle.x) * cosf(cameraAngle.y), sinf(cameraAngle.y), cosf(cameraAngle.x) * cosf(cameraAngle.y) };
    camera->target = Vector3Add(camera->position, targetOffset);

    // 8. Right-Hand Weapon Transform Placement
    if (hasWeaponModel) {
        float bobbing = (Vector3Length(Vector3{velocity.x, 0, velocity.z}) > 0.1f && isGrounded) ? sinf(GetTime() * 12.0f) * 0.02f : 0.0f;
        
        Vector3 weaponPos = Vector3Add(camera->position, Vector3Scale(right, 0.30f)); 
        weaponPos = Vector3Add(weaponPos, Vector3Scale(forward, 0.50f));
        weaponPos.y += -0.28f + bobbing;

        float swingAngle = isSwinging ? sinf(swingTimer * PI) * 55.0f : 0.0f;

        axeModel.transform = MatrixIdentity();
        axeModel.transform = MatrixMultiply(axeModel.transform, MatrixScale(0.40f, 0.40f, 0.40f)); 
        axeModel.transform = MatrixMultiply(axeModel.transform, MatrixRotateX((15.0f + swingAngle) * DEG2RAD));
        axeModel.transform = MatrixMultiply(axeModel.transform, MatrixRotateY(-cameraAngle.x + (35.0f - swingAngle) * DEG2RAD));
        axeModel.transform = MatrixMultiply(axeModel.transform, MatrixTranslate(weaponPos.x, weaponPos.y, weaponPos.z));
    }

    HandleBlockInteraction(camera, world);
}

void Player::DrawWeapon() {
    if (hasWeaponModel) {
        DrawModel(axeModel, Vector3{0,0,0}, 1.0f, WHITE);
    }
}

// Renders the Minecraft-style selection block highlight bounding shape overlay
void Player::DrawTargetHighlight() {
    if (hasTargetBlock) {
        // Shift spatial center offsets from floor layout to center point vectors
        Vector3 centerPos = { targetBlockPos.x + 0.5f, targetBlockPos.y + 0.5f, targetBlockPos.z + 0.5f };
        // Draw a clean wireframe cube right around the target block boundaries
        DrawCubeWires(centerPos, 1.02f, 1.02f, 1.02f, BLACK);
    }
}

void Player::Unload() {
    if (hasWeaponModel) UnloadModel(axeModel);
}