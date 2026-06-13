#pragma once
#include "PlayerState.h"
#include <vector>

// 3D world collision box
struct WorldAABB
{
    float x, y, z;        // center
    float halfW, halfH, halfD; // half extents
};

class Physics
{
public:
    Physics() = default;

    // Add static world geometry
    void AddWorldBox(float x, float y, float z,
        float halfW, float halfH, float halfD);

    // Update a single player's physics
    void UpdatePlayer(PlayerState& player, float dt);

    // Physics constants
    float gravity = 2000.0f;
    float moveSpeed = 600.0f;
    float jumpVelocity = 800.0f;

    float deathY = -500.0f; // fall off map

private:
    std::vector<WorldAABB> world;

    bool CheckAABB(const AABB& a, const WorldAABB& b);
    void ResolveCollision(PlayerState& player, const WorldAABB& box);
};
