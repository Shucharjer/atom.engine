#include "systems/render/BufferObject.hpp"
#include "pchs/graphics.hpp"

namespace atom::engine::systems::render {
template class BufferObject<BasicVertexBufferObject>;
template class BufferObject<BasicUniformBufferObject>;
} // namespace atom::engine::systems::render
