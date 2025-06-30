#include "Game.h"
#include "Offsets.h"
#include "Utils.h"
#include <iostream>

Game::Game(Bypass* bypass, uintptr_t baseAddress /*= 0x400000*/)
    : m_baseAddress(baseAddress),
    m_localPlayerPosition{},
    m_localPlayerCameraAngles{},
    m_numPlayers(0),
    m_bypass(bypass)
{
}

void Game::Update()
{
    if (ReadNumberOfPlayers()) {
        ReadLocalPlayer();
        ReadLocalCameraAngles();
        ReadEnemies();
    }
}

DWORD Game::GetNumPlayers() const noexcept
{
    return m_numPlayers;
}

Vector3 Game::GetLocalPlayerPosition() const noexcept
{
    return m_localPlayerPosition;
}

Vector3 Game::GetLocalPlayerCameraAngles() const noexcept
{
    return m_localPlayerCameraAngles;
}

const std::vector<Enemy>& Game::GetEnemies() const noexcept
{
    return m_enemies;
}

Bypass* Game::GetBypass() const noexcept
{
    return m_bypass;
}

void Game::PrintGameInfo() const
{
    std::cout << "==== Game Info ====" << std::endl;
    std::cout << "Number of Players: " << GetNumPlayers() << std::endl;

    Vector3 pos = GetLocalPlayerPosition();
    Vector3 camera = GetLocalPlayerCameraAngles();
    std::cout << "Local Player Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    std::cout << "Local Player Camera: (" << camera.x << ", " << camera.y << ", " << camera.z << ")" << std::endl;

    const auto& enemies = GetEnemies();
    std::cout << "Enemies (" << enemies.size() << "):" << std::endl;

    for (size_t i = 0; i < enemies.size(); ++i) {
        const auto& enemy = enemies[i];
        std::cout << "  Enemy " << i << ": Position("
            << enemy.position.x << ", "
            << enemy.position.y << ", "
            << enemy.position.z << ")" << std::endl;
    }
    std::cout << "===================" << std::endl;
}

bool Game::ReadNumberOfPlayers()
{
    if (!m_bypass->Read(m_baseAddress + Offsets::Map::NumPlayersInMap, &m_numPlayers, sizeof(DWORD))) {
        return Utils::Error("Could not read number of players in map.");
    }
    return true;
}

bool Game::ReadLocalPlayer()
{
    uintptr_t addressPlayerBase = 0;
    if (!m_bypass->Read(m_baseAddress + Offsets::Player::PlayerBase, &addressPlayerBase, sizeof(uintptr_t))) {
        return Utils::Error("Could not read pointer to player base.");
    }

    if (!m_bypass->Read(addressPlayerBase + Offsets::Player::PositionFeetOffsets[0], &m_localPlayerPosition, sizeof(Vector3))) {
        return Utils::Error("Could not read (x,y,z) of player.");
    }

    return true;
}

bool Game::ReadLocalCameraAngles()
{
    uintptr_t addressPlayerBase = 0;
    if (!m_bypass->Read(m_baseAddress + Offsets::Player::PlayerBase, &addressPlayerBase, sizeof(uintptr_t))) {
        return Utils::Error("Could not read pointer to player base.");
    }

    Vector3 a;
    if (!m_bypass->Read(addressPlayerBase + Offsets::Player::CameraOffsets[0], &m_localPlayerCameraAngles, sizeof(Vector3))) {
        return Utils::Error("Could not read (yaw,pitch,roll) of player.");
    }

    return true;
}

bool Game::ReadEnemies()
{
    if (m_numPlayers <= 1) {
        m_enemies.clear();
        return true;
    }

    DWORD numEnemies = m_numPlayers - 1; // exclude local player

    uintptr_t addressEnemiesBase = 0;
    if (!m_bypass->Read(m_baseAddress + Offsets::Enemy::EnemyBase, &addressEnemiesBase, sizeof(uintptr_t))) {
        return Utils::Error("Could not read base address of enemies.");
    }

    // Skip local player (myself)
    addressEnemiesBase += sizeof(uintptr_t);

    std::vector<uintptr_t> enemyAddresses(numEnemies);
    if (!m_bypass->Read(addressEnemiesBase, enemyAddresses.data(), sizeof(uintptr_t) * numEnemies)) {
        return Utils::Error("Could not read pointers to enemies info.");
    }

    m_enemies.clear();
    m_enemies.resize(numEnemies);

    for (size_t i = 0; i < numEnemies; ++i) {
        m_enemies[i].baseAddress = enemyAddresses[i];
        if (!m_bypass->Read(m_enemies[i].baseAddress + Offsets::Player::PositionFeetOffsets[0], &m_enemies[i].position, sizeof(Vector3))) {
            return Utils::Error("Could not read enemy position.");
        }
    }

    return true;
}
