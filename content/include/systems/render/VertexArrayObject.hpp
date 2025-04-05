#pragma once
#include <ranges>
#include <glad/glad.h>
#include <concepts/type.hpp>
#include "BufferObject.hpp"
#include "reflection.hpp"
#include "systems/render/BufferObject.hpp"
#include "systems/render/Vertex.hpp"

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

    template <utils::concepts::aggregate Ty>
    void addAttribute(const GLuint index, VertexBufferObject& vbo) {
        constexpr auto count = utils::member_count_of<Ty>();
        glBindVertexArray(m_Id);
        vbo.bind();
        // TODO:
        const auto& offsets = utils::offset_array_of<Ty>();
        [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            (SetSingleAttribute<Is, sizeof(utils::member_type_of_t<Is, Ty>), sizeof(Ty)>(
                 index, offsets[Is]
             ),
             ...);
        }(std::make_index_sequence<count>());
    }

    void bind() const noexcept;
    void unbind() const noexcept;

private:
    template <std::size_t Index, GLint Size, GLsizei Stride>
    void SetSingleAttribute(GLuint index, std::size_t offset) {
        const auto currentIndex = index + Index;
        glVertexAttribPointer(
            currentIndex, Size, GL_FLOAT, GL_FALSE, Stride, reinterpret_cast<void*>(offset)
        );
        glEnableVertexAttribArray(currentIndex);
    }

    template <typename T>
    constexpr static GLenum GLtype() {
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

template void VertexArrayObject::addAttribute<Vertex>(GLuint, VertexBufferObject&);

} // namespace atom::engine::systems::render
