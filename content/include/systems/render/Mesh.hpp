#pragma once
#include <cstdint>
#include <containers.hpp>
#include "Material.hpp"
#include "VertexArrayObject.hpp"
#include "pchs/std.hpp"
#include "systems/render/BufferObject.hpp"
#include "systems/render/Material.hpp"
#include "systems/render/ShaderProgram.hpp"
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
    VertexArrayObject vao;
    VertexBufferObject vbo;
    atom::vector<Vertex> vertices;
    atom::vector<uint32_t> indices;
    atom::vector<Material> materials;

    Mesh()                       = default;
    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;

    void draw(const ShaderProgram& program) const noexcept;
};

} // namespace atom::engine::systems::render
