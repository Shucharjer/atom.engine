#include "systems/render/Mesh.hpp"
#include "pchs/graphics.hpp"

#include "systems/render/ShaderProgram.hpp"

namespace atom::engine::systems::render {

Mesh::Mesh(Mesh&& that) noexcept
    : visibility(that.visibility), culling(that.culling), vao(std::move(that.vao)),
      vbo(std::move(that.vbo)), vertices(std::move(that.vertices)),
      indices(std::move(that.indices)), materials(std::move(that.materials)) {}

void Mesh::draw(const ShaderProgram& program) const noexcept {

    vao.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    vao.unbind();
    glActiveTexture(0);
}

} // namespace atom::engine::systems::render
