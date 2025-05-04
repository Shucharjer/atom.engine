#pragma once
#include <glad/glad.h>
#include <concepts/type.hpp>
#include "BufferObject.hpp"
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

    void addAttribute(const GLuint index, VertexBufferObject& vbo, GLint size);

    void addAttributeForVertices(const GLuint index, VertexBufferObject& vbo);

    void bind() const noexcept;
    void unbind() const noexcept;

private:
    template <typename T>
    consteval static GLenum GLtype() {
        if constexpr (std::is_same_v<T, float>)
            return GL_FLOAT;
        else if constexpr (std::is_same_v<T, int>)
            return GL_INT;
        else if constexpr (std::is_same_v<T, unsigned int>)
            return GL_UNSIGNED_INT;
        else
            static_assert(false, "Unsupported array attribute type");
    }

    GLuint m_Id{};
};

} // namespace atom::engine::systems::render
