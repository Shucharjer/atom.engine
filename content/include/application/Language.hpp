#pragma once
#include <fstream>
#include <string_view>
#include <core/langdef.hpp>
#include <nlohmann/json.hpp>
#include "io/file.hpp"

namespace atom::engine::application {

auto& GetLanguage() {
    static nlohmann::json lang;
    return lang;
}

void LoadLanguageFile(const std::string& path) {
    auto& lang = GetLanguage();

    std::ifstream stream(path);

    if (stream.is_open()) {
        try {
            stream >> lang;
        }
        catch (...) {
            // Log err
        }
    }
}

} // namespace atom::engine::application
