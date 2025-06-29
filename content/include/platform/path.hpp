#pragma once
#include <string>
#include <easylogging++.h>
#include "core/langdef.hpp"

#if defined(_WIN32) || defined(_WIN64)
    #include <ShlObj.h>
    #include <Windows.h>

#else
    #include <pwd.h>
    #include <unistd.h>
#endif

namespace atom::engine::platform {

#if defined(_WIN32) || defined(_WIN64)
constexpr auto sep = '\\';
#else
constexpr auto sep = '/';
#endif

ATOM_FORCE_INLINE std::string GetUserHomeDirectroy() {
    std::string homeDir;
#if defined(_WIN32) || defined(_WIN64)
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path))) {
        homeDir = path;
    }
    else {
        LOG(ERROR) << "Faield to get user home directory.";
    }
#else
    const char* home;

    if ((home = getenv("HOME")) == 0) {
        home = getpwuid(getuid())->pw_dir;
    }

    if (home) {
        homeDir = home;
    }
    else {
        LOG(ERROR) << "Faield to get user home directory.";
    }
#endif
    return homeDir;
}

} // namespace atom::engine::platform
