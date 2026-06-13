#pragma once
#include <cstdint>

// ---------------------------------------------
// 3D Axis-Aligned Bounding Box (AABB)
// ---------------------------------------------
struct AABB
{
    float x, y, z;              // center position
    float halfW, halfH, halfD;  // half extents (width, height, depth)
};

// ---------------------------------------------
// Player State (Authoritative Server Representation)
// ---------------------------------------------
struct PlayerState
{
    uint8_t id = 0;         // Assigned by server

    AABB box;               // Collision box

    // Velocity in 3D space
    float velX = 0.0f;
    float velY = 0.0f;
    float velZ = 0.0f;

    bool grounded = false;  // True if standing on ground
    bool dead = false;      // True if fell off map or killed
    int health = 100;       // Simple health system

    // Reset player to a spawn point
    void Respawn(float x, float y, float z)
    {
        box.x = x;
        box.y = y;
        box.z = z;

        velX = velY = velZ = 0.0f;
        grounded = false;
        dead = false;
        health = 100;
    }
};
