#include "systems/render/Mesh.hpp"
#include <format>
#include <asset.hpp>
#include "pchs/graphics.hpp"

#include "systems/render/ShaderProgram.hpp"
#include "systems/render/Texture.hpp"

namespace atom::engine::systems::render {

Mesh::Mesh(Mesh&& that) noexcept
    : visibility(that.visibility), culling(that.culling), vao(std::move(that.vao)),
      vbo(std::move(that.vbo)), vertices(std::move(that.vertices)),
      indices(std::move(that.indices)), materials(std::move(that.materials)) {}

Mesh& Mesh::operator=(Mesh&& that) noexcept {
    if (this != &that) {
        visibility = that.visibility;
        culling    = that.culling;
        vao        = std::move(that.vao);
        vbo        = std::move(that.vbo);
        vertices   = std::move(that.vertices);
        indices    = std::move(that.indices);
        materials  = std::move(that.materials);
    }
    return *this;
}

static inline void TryActiveTexture(
    library<Texture>& library,
    const ShaderProgram& program,
    const Texture& texture,
    const char* textureType,
    uint16_t& i
) {
    if (texture) {
        auto handle = texture.get_handle();
        auto proxy  = library.fetch(handle);
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, proxy->m_Id);
        program.setInt(textureType, i);
#if defined(ATOM_OPT_SHOW_TEXTURE_ACTIVE)
        LOG(DEBUG) << std::format("active {} at {}: {}", textureType, texture.path(), i);
#endif
        ++i;
    }
}

void Mesh::draw(const ShaderProgram& program) const noexcept {
    auto& hub     = hub::instance();
    auto& library = hub.library<Texture>();
    if (!materials.empty()) {
        auto& material = materials[0];
        uint16_t i     = 0;
        TryActiveTexture(
            library, program, material.baseColorTexture, "material.baseColorTexture", i
        );
        TryActiveTexture(library, program, material.ambientTexture, "material.ambientTexture", i);

        TryActiveTexture(library, program, material.diffuseTexture, "material.diffuseTexture", i);
        TryActiveTexture(library, program, material.specularTexture, "material.specularTexture", i);
        TryActiveTexture(library, program, material.emissionTexture, "material.emissionTexture", i);
        TryActiveTexture(library, program, material.metallicTexture, "material.metallicTexture", i);
        TryActiveTexture(
            library, program, material.roughnessTexture, "material.roughnessTexture", i
        );
        TryActiveTexture(
            library, program, material.occlusionTexture, "material.occlusionTexture", i
        );
        TryActiveTexture(library, program, material.normalTexture, "material.normalTexture", i);
        program.setVec4("material.baseColorFactor", material.baseColorFactor);
        program.setFloat("material.metallicFactor", material.metallicFactor);
        program.setFloat("material.roughnessFactor", material.roughnessFactor);
        program.setFloat("material.ambientFactor", material.ambientFactor);
        program.setVec3("material.emissiveFactor", material.emissiveFactor);
        program.setVec3("material.ambientLight", material.ambientLight);
    }

    vao.bind();
    // glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    vao.unbind();
}

} // namespace atom::engine::systems::render
