#pragma once

#include <Windows.h>
#include <vector>

// Namespace for all game offsets to avoid name collisions
namespace Offsets {

    namespace Player {
        // Base address offset for player (relative to module base address)
        constexpr DWORD PlayerBase = 0x017E0A8;

        // Pointer chains to reach player head position
        const std::vector<DWORD> PositionHeadOffsets = { 0x4, 0x8, 0xC };

        // Pointer chains to reach player feet position
        const std::vector<DWORD> PositionFeetOffsets = { 0x28, 0x2C, 0x30 };

        // Pointer chains to reach player camera angles
        const std::vector<DWORD> CameraOffsets = { 0x34, 0x38, 0x3C };
    }

    namespace Enemy {
        // Base address offset where all enemies are stored
        constexpr DWORD EnemyBase = 0x018AC04;
    }

    namespace Map {
        // Address storing number of players currently in map
        constexpr DWORD NumPlayersInMap = 0x18AC0C;
    }

    namespace Rendering {
        // Offset for the MVP (Model-View-Projection) matrix in memory
        constexpr DWORD MvpMatrixOffset = 0x57DFD0;
    }

}
