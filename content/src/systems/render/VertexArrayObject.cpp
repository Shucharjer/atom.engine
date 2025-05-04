#include "systems/render/VertexArrayObject.hpp"
#include <easylogging++.h>
#include "pchs/graphics.hpp"

#include "systems/render/BufferObject.hpp"
#include "systems/render/Vertex.hpp"

using namespace atom::engine::systems::render;

VertexArrayObject::VertexArrayObject() { glGenVertexArrays(1, &m_Id); }

VertexArrayObject::VertexArrayObject(VertexArrayObject&& that) noexcept
    : m_Id(std::exchange(m_Id, 0)) {}

VertexArrayObject& VertexArrayObject::operator=(VertexArrayObject&& that) noexcept {
    if (this != &that) {
        VertexArrayObject temp(std::move(that));
        std::swap(m_Id, temp.m_Id);
    }
    return *this;
}

VertexArrayObject::~VertexArrayObject() noexcept {
    if (m_Id) {
        glDeleteVertexArrays(1, &m_Id);
    }
}

GLuint VertexArrayObject::get() const noexcept { return m_Id; }

void VertexArrayObject::addAttribute(
    const GLuint index,
    const GLint size,
    const DataType type,
    const GLsizei stride,
    const std::size_t offset,
    const bool normalized
) {
    glBindVertexArray(m_Id);
    glVertexAttribPointer(
        index, size, static_cast<GLenum>(type), normalized, stride, reinterpret_cast<void*>(offset)
    );
    glEnableVertexAttribArray(index);
}

void VertexArrayObject::addAttribute(const GLuint index, VertexBufferObject& vbo, GLint size) {
    glBindVertexArray(m_Id);
    vbo.bind();
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
}

void VertexArrayObject::addAttributeForVertices(const GLuint index, VertexBufferObject& vbo) {
    glBindVertexArray(m_Id);
    vbo.bind();
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(
        index + 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal)
    );
    glEnableVertexAttribArray(index + 1);
    glVertexAttribPointer(
        index + 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords)
    );
    glEnableVertexAttribArray(index + 2);
}

void VertexArrayObject::bind() const noexcept {
    if (m_Id) [[likely]] {
        glBindVertexArray(m_Id);
    }
    else [[unlikely]] {
        LOG(INFO) << "try bind to a empty vertex array object";
    }
}

void VertexArrayObject::unbind() const noexcept { glBindVertexArray(0); }
