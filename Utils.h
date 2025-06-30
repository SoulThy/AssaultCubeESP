#pragma once

#include <string>

namespace Utils {

    // Logs an error message and optionally performs additional error handling.
    // Returns false to allow usage like: `if (!Utils::Error("msg")) { ... }`
    bool Error(const std::string& message);

}
