#pragma once
#include "pchs/graphics.hpp"
#include "pchs/logger.hpp"

namespace atom::engine::systems::render {

template <typename Buffer>
class BufferObject {
public:
    constexpr static auto buffer_type = Buffer::buffer_type;

    BufferObject() noexcept;
    BufferObject(const std::size_t size, const GLenum usage = GL_STATIC_DRAW);
    BufferObject(BufferObject&& that) noexcept;
    BufferObject& operator=(BufferObject&& that) noexcept;
    ~BufferObject() noexcept;

    BufferObject(const BufferObject&)            = delete;
    BufferObject& operator=(const BufferObject&) = delete;

    std::size_t size() const noexcept;
    GLuint get() const noexcept;
    void set(const void* const data) noexcept;
    void bind();
    void bindBase(const GLuint bindingPoint = 0) noexcept;
    void update(const std::size_t offset, void* const data, const std::size_t size) noexcept;
    void batchUpdate(const std::vector<std::tuple<std::size_t, void*, std::size_t>>& updates
    ) noexcept;

    void create(const std::size_t size, const GLenum usage = GL_STATIC_DRAW);
    void destroy();

    operator bool() const noexcept;

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

extern template class BufferObject<BasicVertexBufferObject>;
extern template class BufferObject<BasicUniformBufferObject>;

using VertexBufferObject  = BufferObject<BasicVertexBufferObject>;
using UniformBufferObject = BufferObject<BasicUniformBufferObject>;

template <typename Buffer>
BufferObject<Buffer>::BufferObject() noexcept : m_Size(), m_Usage() {}

template <typename Buffer>
BufferObject<Buffer>::BufferObject(const std::size_t size, const GLenum usage)
    : m_Size(size), m_Usage(usage) {
    glGenBuffers(1, &m_Id);
    glBindBuffer(buffer_type, m_Id);
    glBufferData(buffer_type, size, nullptr, usage);
    glBindBuffer(buffer_type, 0);
}

template <typename Buffer>
BufferObject<Buffer>::BufferObject(BufferObject&& that) noexcept
    : m_Id(std::exchange(that.m_Id, 0)), m_Size(std::exchange(that.m_Size, 0)),
      m_Usage(std::exchange(that.m_Usage, 0)), m_CurrentBinding(that.m_CurrentBinding) {}

template <typename Buffer>
BufferObject<Buffer>& BufferObject<Buffer>::operator=(BufferObject&& that) noexcept {
    if (this != &that) [[likely]] {
        BufferObject<Buffer> temp(std::move(that));
        std::swap(m_Id, temp.m_Id);
        std::swap(m_Size, temp.m_Size);
        std::swap(m_Usage, temp.m_Usage);
        std::swap(m_CurrentBinding, temp.m_CurrentBinding);
    }

    return *this;
}

template <typename Buffer>
BufferObject<Buffer>::~BufferObject() noexcept {
    if (m_Id) {
        glDeleteBuffers(1, &m_Id);
        m_Id = 0;
    }
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
void BufferObject<Buffer>::set(const void* const data) noexcept {
    glBindBuffer(buffer_type, m_Id);
    glBufferData(buffer_type, m_Size, data, m_Usage);
    glBindBuffer(buffer_type, 0);
}

template <typename Buffer>
void BufferObject<Buffer>::bind() {
    glBindBuffer(buffer_type, m_Id);
}

template <typename Buffer>
void BufferObject<Buffer>::bindBase(const GLuint bindingPoint) noexcept {
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

template <typename Buffer>
void BufferObject<Buffer>::create(std::size_t size, const GLenum usage) {
    if (!m_Id) {
        glGenBuffers(1, &m_Id);
        glBindBuffer(buffer_type, m_Id);
        glBufferData(buffer_type, size, nullptr, usage);
        glBindBuffer(buffer_type, 0);
        m_Size           = size;
        m_Usage          = usage;
        m_CurrentBinding = 0;
    }
}

template <typename Buffer>
void BufferObject<Buffer>::destroy() {
    if (m_Id) [[likely]] {
        glDeleteBuffers(1, &m_Id);
    }
}

template <typename Buffer>
BufferObject<Buffer>::operator bool() const noexcept {
    return m_Id;
}

} // namespace atom::engine::systems::render
