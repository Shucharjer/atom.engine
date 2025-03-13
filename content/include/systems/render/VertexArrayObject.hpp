#pragma once
#include "pch.hpp"

#include "systems/render/BufferObject.hpp"

namespace atom::engine::systems::render {

class VertexArrayObject {
public:
    enum class DataType : GLenum {
        Float = GL_FLOAT,
        Uint  = GL_UNSIGNED_INT,
        Byte  = GL_UNSIGNED_BYTE
    };

    VertexArrayObject();
    VertexArrayObject(VertexArrayObject&& that) noexcept;
    VertexArrayObject& operator=(VertexArrayObject&& that) noexcept;
    VertexArrayObject(const VertexArrayObject&)            = delete;
    VertexArrayObject& operator=(const VertexArrayObject&) = delete;
    ~VertexArrayObject() noexcept;

    [[nodiscard]] GLuint get() const noexcept;

    void addAttribute(
        const GLuint index,
        const GLint size,
        const DataType type,
        const GLsizei stride,
        const std::size_t offset,
        const bool normalized = GL_FALSE
    );

    void addAttribute(const GLuint index, VertexBufferObject& vbo);

    void bind() const noexcept;
    void unbind() const noexcept;

private:
    GLuint m_Id{};
};

} // namespace atom::engine::systems::render
