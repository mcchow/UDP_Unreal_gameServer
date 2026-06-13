#include "GameServer.h"
#include "PlayerState.h"
#include "Physics.h"

#include <enet/enet.h>
#include <iostream>
#include <unordered_map>
#include <chrono>

using namespace std;

static const int SERVER_PORT = 1234;
static const int MAX_CLIENTS = 32;
static const int CHANNELS = 2;
static const int TICK_RATE = 60; // 60 updates per second

// ---------------------------------------------
// Global server state
// ---------------------------------------------
unordered_map<ENetPeer*, PlayerState> players;
Physics physics;

// Example spawn point
static const float SPAWN_X = 0.0f;
static const float SPAWN_Y = 200.0f;
static const float SPAWN_Z = 0.0f;

// ---------------------------------------------
// Handle incoming packets (input, actions, etc.)
// ---------------------------------------------
void HandlePacket(ENetPeer* peer, ENetPacket* packet)
{
    // TODO: parse your input packet format
    // For now, just print size
    cout << "Received packet (" << packet->dataLength << " bytes)" << endl;

    // Example: interpret first byte as movement direction
    if (packet->dataLength >= 1)
    {
        uint8_t move = packet->data[0];

        PlayerState& p = players[peer];

        // Simple input example:
        // 1 = move left, 2 = move right, 3 = jump
        if (move == 1) p.velX = -physics.moveSpeed;
        else if (move == 2) p.velX = physics.moveSpeed;
        else p.velX = 0;

        if (move == 3 && p.grounded)
            p.velY = physics.jumpVelocity;
    }
}

// ---------------------------------------------
// Broadcast player states to all clients
// ---------------------------------------------
void BroadcastState(ENetHost* server)
{
    // TODO: pack your PlayerState into a packet
    // For now, send a placeholder
    const char* msg = "state";
    ENetPacket* packet = enet_packet_create(msg, strlen(msg) + 1, ENET_PACKET_FLAG_UNSEQUENCED);

    enet_host_broadcast(server, 0, packet);
}

// ---------------------------------------------
// Main
// ---------------------------------------------
int main()
{
    if (enet_initialize() != 0)
    {
        cout << "Failed to initialize ENet" << endl;
        return EXIT_FAILURE;
    }

    cout << "ENet initialized!" << endl;

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = SERVER_PORT;

    ENetHost* server = enet_host_create(
        &address,
        MAX_CLIENTS,
        CHANNELS,
        0, 0
    );

    if (!server)
    {
        cout << "Failed to create ENet server" << endl;
        enet_deinitialize();
        return EXIT_FAILURE;
    }

    cout << "Server started on port " << SERVER_PORT << endl;

    // ---------------------------------------------
    // Setup world collision (example)
    // ---------------------------------------------
    physics.AddWorldBox(0, 0, 0, 500, 10, 500); // big ground

    // ---------------------------------------------
    // Main loop
    // ---------------------------------------------
    ENetEvent event;

    const double tickInterval = 1.0 / TICK_RATE;
    auto lastTick = chrono::high_resolution_clock::now();

    while (true)
    {
        // ---------------------------------------------
        // Handle ENet events
        // ---------------------------------------------
        while (enet_host_service(server, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                cout << "Client connected!" << endl;

                PlayerState newPlayer;
                newPlayer.id = players.size();
                newPlayer.box = { SPAWN_X, SPAWN_Y, SPAWN_Z, 30, 50, 30 };

                players[event.peer] = newPlayer;
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
                HandlePacket(event.peer, event.packet);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                cout << "Client disconnected" << endl;
                players.erase(event.peer);
                break;

            default:
                break;
            }
        }

        // ---------------------------------------------
        // Tick update (physics + game logic)
        // ---------------------------------------------
        auto now = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration<double>(now - lastTick).count();

        if (elapsed >= tickInterval)
        {
            lastTick = now;

            // Update all players
            for (auto& [peer, player] : players)
            {
                physics.UpdatePlayer(player, (float)tickInterval);

                // Respawn if dead
                if (player.dead)
                    player.Respawn(SPAWN_X, SPAWN_Y, SPAWN_Z);
            }

            // Send updated state to all clients
            BroadcastState(server);
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    return 0;
}
