#include "Physics.h"
#include <cmath>

void Physics::AddWorldBox(float x, float y, float z,
    float halfW, float halfH, float halfD)
{
    world.push_back({ x, y, z, halfW, halfH, halfD });
}

bool Physics::CheckAABB(const AABB& a, const WorldAABB& b)
{
    return std::fabs(a.x - b.x) < (a.halfW + b.halfW) &&
        std::fabs(a.y - b.y) < (a.halfH + b.halfH) &&
        std::fabs(a.z - b.z) < (a.halfD + b.halfD);
}

void Physics::ResolveCollision(PlayerState& player, const WorldAABB& box)
{
    AABB& a = player.box;

    float overlapX = (a.halfW + box.halfW) - std::fabs(a.x - box.x);
    float overlapY = (a.halfH + box.halfH) - std::fabs(a.y - box.y);
    float overlapZ = (a.halfD + box.halfD) - std::fabs(a.z - box.z);

    // Resolve on the smallest axis
    if (overlapX < overlapY && overlapX < overlapZ)
    {
        if (a.x < box.x) a.x -= overlapX;
        else             a.x += overlapX;
        player.velX = 0;
    }
    else if (overlapY < overlapX && overlapY < overlapZ)
    {
        if (a.y < box.y)
        {
            a.y -= overlapY;
            player.velY = 0;
        }
        else
        {
            a.y += overlapY;
            player.velY = 0;
            player.grounded = true;
        }
    }
    else
    {
        if (a.z < box.z) a.z -= overlapZ;
        else             a.z += overlapZ;
        player.velZ = 0;
    }
}

void Physics::UpdatePlayer(PlayerState& player, float dt)
{
    if (player.dead)
        return;

    player.grounded = false;

    // Gravity
    player.velY -= gravity * dt;

    // Integrate velocity
    player.box.x += player.velX * dt;
    player.box.y += player.velY * dt;
    player.box.z += player.velZ * dt;

    // Check collisions
    for (const auto& box : world)
    {
        if (CheckAABB(player.box, box))
        {
            ResolveCollision(player, box);
        }
    }

    // Death check
    if (player.box.y < deathY)
    {
        player.dead = true;
        player.health = 0;
    }
}
