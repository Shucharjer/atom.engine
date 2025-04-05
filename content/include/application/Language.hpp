#pragma once
#include <fstream>
#include <easylogging++.h>
#include <core/langdef.hpp>
#include <nlohmann/json.hpp>

namespace atom::engine::application {

inline auto& GetLanguage() {
    static nlohmann::json lang;
    return lang;
}

inline void SetToDefaultLanguage() {
    auto& lang = GetLanguage();
    lang       = R"(
        {
            "docker": {
            "self": "docker",
            "project": {
                "self": "Project",
                "OpenProject": "Open project",
                "saveProject": "Save project",
                "saveAs": "Save as"
            }
        }
    }
    )";
}

inline void LoadLanguageFile(const std::string& path) {
    auto& lang = GetLanguage();

    std::ifstream stream(path);

    if (stream.is_open()) {
        try {
            stream >> lang;
        }
        catch (...) {
            LOG(ERROR) << "Failed to parse the language file.";
        }
    }
}

} // namespace atom::engine::application
