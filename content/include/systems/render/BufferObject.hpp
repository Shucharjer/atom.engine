#pragma once
#include <easylogging++.h>
#include "pch.hpp"

namespace atom::engine::systems::render {

template <typename Buffer>
class BufferObject {
public:
    constexpr static auto buffer_type = Buffer::buffer_type;

    BufferObject(const std::size_t size, const GLenum usage = GL_STATIC_DRAW);
    BufferObject(BufferObject&& that) noexcept;
    BufferObject& operator=(BufferObject&& that) noexcept;
    ~BufferObject() noexcept;

    BufferObject(const BufferObject&)            = delete;
    BufferObject& operator=(const BufferObject&) = delete;

    std::size_t size() const noexcept;
    GLuint get() const noexcept;
    void set(void* const data) noexcept;
    void bind(const GLuint bindingPoint = 0) noexcept;
    void update(const std::size_t offset, void* const data, const std::size_t size) noexcept;
    void batchUpdate(const std::vector<std::tuple<std::size_t, void*, std::size_t>>& updates
    ) noexcept;

private:
    GLuint m_Id{};
    std::size_t m_Size;
    GLenum m_Usage;
    mutable GLuint m_CurrentBinding{};
};

class BasicVertexBufferObject {
public:
    constexpr static auto buffer_type = GL_ARRAY_BUFFER;
};

class BasicUniformBufferObject {
public:
    constexpr static auto buffer_type = GL_UNIFORM_BUFFER;
};

template class BufferObject<BasicVertexBufferObject>;
template class BufferObject<BasicUniformBufferObject>;

using VertexBufferObject  = BufferObject<BasicVertexBufferObject>;
using UniformBufferObject = BufferObject<BasicUniformBufferObject>;

template <typename Buffer>
BufferObject<Buffer>::BufferObject(const std::size_t size, const GLenum usage)
    : m_Size(size), m_Usage(usage) {
    glGenBuffers(1, &m_Id);
    glBindBuffer(buffer_type, m_Id);
    glBufferData(buffer_type, size, m_Size, usage);
    glBindBuffer(buffer_type, 0);
}

template <typename Buffer>
BufferObject<Buffer>::BufferObject(BufferObject&& that) noexcept
    : m_Id(std::exchange(that.m_Id, 0)), m_Size(std::exchange(that.m_Size, 0)),
      m_Usage(std::exchange(that.m_Usage, 0)) {}

template <typename Buffer>
BufferObject<Buffer>& BufferObject<Buffer>::operator=(BufferObject&& that) noexcept {
    if (this != &that) [[likely]] {
        BufferObject<Buffer> temp(std::move(that));
        std::swap(m_Id, that.m_Id);
        std::swap(m_Size, that.m_Size);
        std::swap(m_Usage, that.m_Usage);
    }

    return *this;
}

template <typename Buffer>
GLuint BufferObject<Buffer>::get() const noexcept {
    return m_Id;
}

template <typename Buffer>
std::size_t BufferObject<Buffer>::size() const noexcept {
    return m_Size;
}

template <typename Buffer>
void BufferObject<Buffer>::set(void* const data) noexcept {
    glBindBuffer(buffer_type, m_Id);
    glBufferData(buffer_type, m_Size, data, m_Usage);
    glBindBuffer(buffer_type, 0);
}

template <typename Buffer>
void BufferObject<Buffer>::bind(const GLuint bindingPoint) noexcept {
    glBindBufferBase(buffer_type, bindingPoint, m_Id);
    m_CurrentBinding = bindingPoint;
}

template <typename Buffer>
void BufferObject<Buffer>::update(
    const std::size_t offset, void* const data, const std::size_t size
) noexcept {
    glBindBuffer(buffer_type, m_Id);
    glBufferSubData(buffer_type, offset, size, data);
    glBindBuffer(buffer_type, 0);
}

template <typename Buffer>
void BufferObject<Buffer>::batchUpdate(
    const std::vector<std::tuple<std::size_t, void*, std::size_t>>& updates
) noexcept {
    glBindBuffer(buffer_type, m_Id);
    for (const auto& [offset, data, size] : updates) {
        if (offset + size > m_Size) [[unlikely]] {
            LOG(ERROR) << "";
            continue;
        }

        glBufferSubData(buffer_type, offset, size, data);
    }
    glBindBuffer(buffer_type, 0);
}

} // namespace atom::engine::systems::render
