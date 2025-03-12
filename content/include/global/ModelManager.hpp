#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <auxiliary/singleton.hpp>

const float kDefaultRetireTime = 1000000000.F;

class ModelManager : public ::atom::utils::singleton<ModelManager> {
    friend class ::atom::utils::singleton<ModelManager>;
    struct Model;

    struct ModelInfo {
        float lastUsedTime;
        Model* model;
    };

public:
    Model* use(std::string_view name) {
        if (m_Models.contains(name)) {}
    }

    void retire() noexcept {
        auto cond = [](const auto& pair) { return false; };
    }

private:
    ModelManager()     = default;
    float m_RetireTime = kDefaultRetireTime;
    std::unordered_map<std::string_view, ModelInfo> m_Models;
};
