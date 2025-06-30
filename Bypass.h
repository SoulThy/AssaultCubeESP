#pragma once
#include <Windows.h>

class Bypass {
public:
    Bypass() noexcept;
    ~Bypass() noexcept;

    // Attach to a process by its process ID
    bool Attach(DWORD pid);

    // Attach to a process by its window name (title)
    bool AttachToWindow(const char* windowName);

    // Read memory from the target process
    bool Read(uintptr_t address, void* buffer, SIZE_T size, SIZE_T* bytesRead = nullptr) const;

    // Write memory to the target process
    bool Write(uintptr_t address, const void* buffer, SIZE_T size, SIZE_T* bytesWritten = nullptr) const;

    DWORD GetProcessId() const noexcept;
    HWND GetGameWindowHandle() const noexcept;

private:
    HANDLE m_processHandle = nullptr;
    DWORD m_processId = 0;
    HWND m_gameWindowHandle = nullptr;
};