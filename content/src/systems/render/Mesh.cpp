#include "systems/render/Mesh.hpp"
#include <asset.hpp>
#include "pchs/graphics.hpp"

#include "systems/render/ShaderProgram.hpp"
#include "systems/render/Texture.hpp"

namespace atom::engine::systems::render {

Mesh::Mesh(Mesh&& that) noexcept
    : visibility(that.visibility), culling(that.culling), vao(std::move(that.vao)),
      vbo(std::move(that.vbo)), vertices(std::move(that.vertices)),
      indices(std::move(that.indices)), materials(std::move(that.materials)) {}

static void TryActiveTexture(
    hub& hub,
    library<Texture>& library,
    const ShaderProgram& program,
    const Texture& texture,
    uint16_t& i
) {
    if (texture) {
        auto handle = texture.get_handle();
        auto proxy  = library.fetch(handle);
        glActiveTexture(GL_TEXTURE0 + i++);
        glBindTexture(GL_TEXTURE_2D, proxy->m_Id);
    }
}

void Mesh::draw(const ShaderProgram& program) const noexcept {

    auto& hub     = hub::instance();
    auto& library = hub.library<Texture>();
    if (!materials.empty()) {
        auto& material = materials[0];
        uint16_t i     = 0;
        TryActiveTexture(hub, library, program, material.baseColorTexture, i);
        TryActiveTexture(hub, library, program, material.ambientTexture, i);
    }

    vao.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    vao.unbind();
    glActiveTexture(0);
}

} // namespace atom::engine::systems::render
