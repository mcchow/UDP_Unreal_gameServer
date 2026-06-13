cmake_minimum_required(VERSION 3.20)
project(GameServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GameServer
    GameServer.cpp
    GameServer.h
  "PlayerState.h" "Physics.h" "World.h" "Packet.h" "Physics.cpp" "World.cpp")

# ENet is added via add_subdirectory, so just link it
target_link_libraries(GameServer PRIVATE enet)
target_include_directories(GameServer PRIVATE
    ${CMAKE_SOURCE_DIR}/external/enet/include
)