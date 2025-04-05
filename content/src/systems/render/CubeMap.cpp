#include "systems/render/CubeMap.hpp"
#include <utility>

using namespace atom::engine;
using namespace atom::engine::systems::render;

CubeMap::CubeMap() = default;

CubeMap::CubeMap(CubeMap&& that) noexcept {}

CubeMap& CubeMap::operator=(CubeMap&& that) noexcept {
    if (this != &that) {
        CubeMap temp(std::move(that));
    }
    return *this;
}

CubeMap::~CubeMap() noexcept = default;
