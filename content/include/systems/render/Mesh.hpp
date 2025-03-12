#pragma once
#include <cstdint>
#include <containers.hpp>
#include <pch.hpp>
#include "Material.hpp"
#include "systems/render/Material.hpp"
#include "systems/render/Vertex.hpp"


namespace atom::engine::systems::render {

enum class CullingMode : uint8_t {
    Back,
    None,
    Front
};

struct Mesh {
    bool visibility     = true;
    CullingMode culling = CullingMode::Back;
    atom::vector<Vertex> vertices;
    atom::vector<uint32_t> indices;
    atom::vector<Material> materials;
};

} // namespace atom::engine::systems::render
