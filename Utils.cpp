#include "Utils.h"
#include <iostream>
#include <Windows.h>

bool Utils::Error(const std::string& message) {
    std::cerr << message << ". GetLastError = " << GetLastError() << std::endl;
    system("pause");
    return false;
}
