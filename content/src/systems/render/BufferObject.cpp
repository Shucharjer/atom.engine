#include "systems/render/BufferObject.hpp"
#include "pchs/graphics.hpp"

namespace atom::engine::systems::render {
template class BufferObject<BasicVertexBufferObject>;
template class BufferObject<BasicUniformBufferObject>;
template class BufferObject<BasicElementBufferObject>;
} // namespace atom::engine::systems::render
