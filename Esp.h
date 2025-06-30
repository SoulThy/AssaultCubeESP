#pragma once

#include <Windows.h>
#include "Bypass.h"
#include "Game.h"

// Constants for the ESP window
constexpr wchar_t ESP_WINDOW_CLASS_NAME[] = L"EspWindowClass";
constexpr wchar_t ESP_WINDOW_NAME[] = L"AssaultCube ESP";

class Esp {
public:
    explicit Esp(Game* game);
    ~Esp();

    // Process Windows messages for the ESP window
    void ProcessMessages();

    // Update ESP window position/size and trigger redraw
    void RefreshWindow();

private:
    // Window procedure callback — static as required by WinAPI
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Draw the overlay: background, borders, and dynamic boxes
    void DrawOverlay(HDC hdc, HWND hwnd);

    // Project 3D world coordinates to 2D screen coordinates
    bool WorldToScreen(const Vector3& target, Vector3& screen, const float(&mvpMatrix)[16], int width, int height) const;

    // Fetch the current Model-View-Projection matrix from memory
    bool GetMVPMatrix(float mvpMatrix[16]) const;

    // Initialize the layered transparent ESP window
    bool InitializeWindow();

    // Get the current size and position of the target game window
    void FetchTargetWindowSize();

private:
    HWND m_hwnd = nullptr;             // Handle to the ESP overlay window
    Game* m_game = nullptr;

    // Target window's position and size (updated every refresh)
    LONG m_targetX = 0;
    LONG m_targetY = 0;
    LONG m_targetWidth = 0;
    LONG m_targetHeight = 0;

    // Constants for projection (could eventually be configurable)
    static constexpr float FOV = 90.0f;
    static constexpr float PI = 3.14159265359f;

    // For drawing
    HPEN m_hRedPen = nullptr;
    HPEN m_hYellowPen = nullptr;
    HBRUSH m_hBackgroundBrush = nullptr;
    HBRUSH m_hNoBrush = nullptr;
};
