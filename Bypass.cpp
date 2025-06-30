#include "Bypass.h"
#include "Utils.h"

Bypass::Bypass() noexcept
    : m_processHandle(nullptr), m_processId(0), m_gameWindowHandle(nullptr)
{
}

Bypass::~Bypass() noexcept
{
    if (m_processHandle != nullptr) {
        CloseHandle(m_processHandle);
        m_processHandle = nullptr;
    }
}

bool Bypass::Attach(DWORD pid)
{
    m_processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
    if (m_processHandle != nullptr) {
        m_processId = pid;
        return true;
    }
    return false;
}

bool Bypass::AttachToWindow(const char* windowName)
{
    HWND hwnd = FindWindowA(nullptr, windowName);
    if (hwnd == nullptr) {
        return Utils::Error("Error: Could not find the AssaultCube window. Make sure the game is running.");
    }

    GetWindowThreadProcessId(hwnd, &m_processId);
    m_gameWindowHandle = hwnd;

    return Attach(m_processId);
}

bool Bypass::Read(uintptr_t address, void* buffer, SIZE_T size, SIZE_T* bytesRead) const
{
    return ReadProcessMemory(m_processHandle, reinterpret_cast<LPCVOID>(address), buffer, size, bytesRead) != 0;
}

bool Bypass::Write(uintptr_t address, const void* buffer, SIZE_T size, SIZE_T* bytesWritten) const
{
    return WriteProcessMemory(m_processHandle, reinterpret_cast<LPVOID>(address), buffer, size, bytesWritten) != 0;
}

DWORD Bypass::GetProcessId() const noexcept
{
    return m_processId;
}

HWND Bypass::GetGameWindowHandle() const noexcept
{
    return m_gameWindowHandle;
}
