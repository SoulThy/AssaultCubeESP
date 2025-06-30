#pragma once

#include <Windows.h>
#include <vector>
#include "Bypass.h"
#include "MathTypes.h" 

// Represents an enemy entity with base address and position
struct Enemy {
    uintptr_t baseAddress = 0;
    Vector3 position;
};

class Game {
public:
    explicit Game(Bypass* bypass, uintptr_t baseAddress = 0x400000);

    // Update game state by reading memory (players, enemies, etc)
    void Update();

    // Get number of players currently in the game/map
    DWORD GetNumPlayers() const noexcept;

    // Get local player's position in the world
    Vector3 GetLocalPlayerPosition() const noexcept;

    // Get local player's camera angles 
    Vector3 GetLocalPlayerCameraAngles() const noexcept;

    // Get list of enemies currently detected
    const std::vector<Enemy>& GetEnemies() const noexcept;

    // Get bypass attached to game 
    Bypass* GetBypass() const noexcept;

    // Debug: Print game info to console or log
    void PrintGameInfo() const;

private:
    // Helper functions to read game data from memory
    bool ReadNumberOfPlayers();
    bool ReadLocalPlayer();
    bool ReadLocalCameraAngles();
    bool ReadEnemies();

private:
    Bypass* m_bypass = nullptr;

    uintptr_t m_baseAddress = 0;
    Vector3 m_localPlayerPosition{};
    Vector3 m_localPlayerCameraAngles{};
    std::vector<Enemy> m_enemies{};
    DWORD m_numPlayers = 0;
};
