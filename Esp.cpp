#include "Esp.h"
#include "Utils.h"
#include "Offsets.h"
#include <Windows.h>
#include <iostream>

Esp::Esp(Game* game)
    : m_hwnd(nullptr), m_game(game), m_targetHeight(0), m_targetWidth(0), m_targetX(0), m_targetY(0)
{
    if (!InitializeWindow()) {
        Utils::Error("Could not initialize window for ESP");
    }
    m_hRedPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    m_hYellowPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
    m_hBackgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
    m_hNoBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
}

Esp::~Esp()
{
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    DeleteObject(m_hRedPen);
    DeleteObject(m_hYellowPen);
    DeleteObject(m_hBackgroundBrush);

    UnregisterClass(ESP_WINDOW_CLASS_NAME, GetModuleHandle(nullptr));
}

void Esp::ProcessMessages()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // nullptr to process all messages
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Esp::RefreshWindow()
{
    if (!m_hwnd) return;

    RECT oldRect = { m_targetX, m_targetY, m_targetX + m_targetWidth, m_targetY + m_targetHeight };

    FetchTargetWindowSize();

    RECT newRect = { m_targetX, m_targetY, m_targetX + m_targetWidth, m_targetY + m_targetHeight };

    if (memcmp(&oldRect, &newRect, sizeof(RECT)) != 0) {
        SetWindowPos(
            m_hwnd,
            HWND_TOPMOST,
            m_targetX,
            m_targetY,
            m_targetWidth,
            m_targetHeight,
            SWP_NOACTIVATE | SWP_SHOWWINDOW
        );
    }

    InvalidateRect(m_hwnd, nullptr, TRUE);
    UpdateWindow(m_hwnd);
}

bool Esp::InitializeWindow()
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = Esp::WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = ESP_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!GetClassInfo(wc.hInstance, wc.lpszClassName, &wc)) {
        // Class not registered yet, fill again before registering
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = nullptr;
        wc.lpszMenuName = nullptr;

        if (!RegisterClass(&wc)) {
            return Utils::Error("Failed to register ESP window class");
        }
    }

    FetchTargetWindowSize();

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        ESP_WINDOW_CLASS_NAME,
        ESP_WINDOW_NAME,
        WS_POPUP,
        m_targetX,
        m_targetY,
        m_targetWidth,
        m_targetHeight,
        nullptr,
        nullptr,
        wc.hInstance,
        this
    );

    if (!hwnd) {
        return Utils::Error("Error creating ESP window");
    }

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(hwnd, SW_SHOW);

    m_hwnd = hwnd;

    return true;
}

void Esp::FetchTargetWindowSize()
{
    HWND targetHwnd = m_game->GetBypass()->GetGameWindowHandle();
    if (targetHwnd) {
        RECT clientRect;
        if (GetClientRect(targetHwnd, &clientRect)) {
            POINT topLeft = { clientRect.left, clientRect.top };
            ClientToScreen(targetHwnd, &topLeft);

            m_targetX = topLeft.x;
            m_targetY = topLeft.y;
            m_targetWidth = clientRect.right - clientRect.left;
            m_targetHeight = clientRect.bottom - clientRect.top;
        }
    }
}

LRESULT CALLBACK Esp::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Esp* espInstance = reinterpret_cast<Esp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg)
    {
    case WM_CREATE:
    {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }
    return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (espInstance) {
            espInstance->DrawOverlay(hdc, hwnd);
        }

        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Esp::DrawOverlay(HDC hdc, HWND hwnd)
{
    if (!m_game) return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    // Clear background
    FillRect(hdc, &rect, m_hBackgroundBrush);

    // Select red pen and no brush for border rectangle
    HPEN hOldPen = (HPEN)SelectObject(hdc, m_hRedPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, m_hNoBrush);

    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

    float mvpMatrix[16];
    if (!GetMVPMatrix(mvpMatrix)) {
        // Restore GDI objects before return
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        return;
    }

    int screenWidth = rect.right - rect.left;
    int screenHeight = rect.bottom - rect.top;

    const auto& enemies = m_game->GetEnemies();

    for (const Enemy& enemy : enemies)
    {
        Vector3 head = enemy.position;
        head.z += 4.5f;

        Vector3 foot = enemy.position;

        Vector3 screenHead, screenFoot;
        if (WorldToScreen(head, screenHead, mvpMatrix, screenWidth, screenHeight) &&
            WorldToScreen(foot, screenFoot, mvpMatrix, screenWidth, screenHeight))
        {
            int top = static_cast<int>(screenHead.y);
            int bottom = static_cast<int>(screenFoot.y);
            int height = bottom - top;

            if (height <= 5 || height > screenHeight) continue;

            int width = height / 2;
            int centerX = static_cast<int>(screenFoot.x);

            int left = centerX - (width / 2);
            int right = centerX + (width / 2);

            HPEN hOldBoxPen = (HPEN)SelectObject(hdc, m_hYellowPen);

            Rectangle(hdc, left, top, right, bottom);

            SelectObject(hdc, hOldBoxPen);
        }
    }

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
}


bool Esp::WorldToScreen(const Vector3& target, Vector3& screen, const float(&mvpMatrix)[16], int width, int height) const
{
    float clipX = mvpMatrix[0] * target.x + mvpMatrix[4] * target.y + mvpMatrix[8] * target.z + mvpMatrix[12];
    float clipY = mvpMatrix[1] * target.x + mvpMatrix[5] * target.y + mvpMatrix[9] * target.z + mvpMatrix[13];
    float clipZ = mvpMatrix[2] * target.x + mvpMatrix[6] * target.y + mvpMatrix[10] * target.z + mvpMatrix[14];
    float clipW = mvpMatrix[3] * target.x + mvpMatrix[7] * target.y + mvpMatrix[11] * target.z + mvpMatrix[15];

    if (clipW < 0.1f) return false;

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    screen.x = (width / 2.0f) * (ndcX + 1.0f);
    screen.y = (height / 2.0f) * (1.0f - ndcY);
    screen.z = clipZ / clipW;

    return true;
}

bool Esp::GetMVPMatrix(float mvpMatrix[16]) const
{
    return m_game->GetBypass()->Read(Offsets::Rendering::MvpMatrixOffset, mvpMatrix, sizeof(float) * 16);
}
