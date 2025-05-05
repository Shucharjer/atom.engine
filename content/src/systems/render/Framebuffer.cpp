#include "systems/render/Framebuffer.hpp"
#include <stdexcept>
#include <easylogging++.h>
#include <core/langdef.hpp>
#include "application/KeyboardInput.hpp"
#include "pchs/graphics.hpp"
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

struct FramebufferAttorney {
    [[nodiscard]] static auto& attachments(const Framebuffer* framebuffer) noexcept {
        return framebuffer->m_Attachments;
    }
};

ColorAttachment::ColorAttachment() { glGenTextures(1, &m_Id); }

ColorAttachment::ColorAttachment(GLuint width, GLuint height) : m_Width(width), m_Height(height) {
    glGenTextures(1, &m_Id);
    glBindTexture(GL_TEXTURE_2D, m_Id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

ColorAttachment::ColorAttachment(ColorAttachment&& that) noexcept
    : m_Id(std::exchange(that.m_Id, 0)), m_Width(that.m_Width), m_Height(that.m_Height),
      m_Buffer(that.m_Buffer) {}

ColorAttachment& ColorAttachment::operator=(ColorAttachment&& that) noexcept {
    if (this != &that) {
        std::swap(m_Id, that.m_Id);
        std::swap(m_Width, that.m_Width);
        std::swap(m_Height, that.m_Height);
        std::swap(m_Buffer, that.m_Buffer);
    }
    return *this;
}

ColorAttachment::~ColorAttachment() noexcept {
    if (m_Id) [[likely]] {
        glDeleteTextures(1, &m_Id);
    }
}

ColorAttachment::operator bool() const noexcept { return m_Id && m_Width; }

math::Vector2 ColorAttachment::size() const noexcept { return { m_Width, m_Height }; }

void ColorAttachment::setSize(GLuint width, GLuint height) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void ColorAttachment::resize(GLuint width, GLuint height) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_Width  = width;
    m_Height = height;
    if (m_Buffer) {
        auto& attachments = FramebufferAttorney::attachments(m_Buffer);
        auto index        = attachments.at(m_Id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, index, GL_TEXTURE_2D, m_Id, 0);
    }
}

Renderbuffer::Renderbuffer() { glGenRenderbuffers(1, &m_Id); }

Renderbuffer::Renderbuffer(GLuint width, GLuint height) : m_Width(width), m_Height(height) {
    glGenRenderbuffers(1, &m_Id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
}

Renderbuffer::Renderbuffer(Renderbuffer&& that) noexcept
    : m_Id(std::exchange(that.m_Id, 0)), m_Width(that.m_Width), m_Height(that.m_Height),
      m_Buffer(std::exchange(that.m_Buffer, nullptr)) {}

Renderbuffer& Renderbuffer::operator=(Renderbuffer&& that) noexcept {
    if (this != &that) [[likely]] {
        std::swap(m_Id, that.m_Id);
        std::swap(m_Width, that.m_Width);
        std::swap(m_Height, that.m_Height);
    }
    return *this;
}

Renderbuffer::~Renderbuffer() noexcept {
    if (m_Id) [[likely]] {
        glDeleteRenderbuffers(1, &m_Id);
        m_Id = 0;
    }
}

Renderbuffer::operator bool() const noexcept { return m_Id; }

math::Vector2 Renderbuffer::size() const noexcept { return { m_Width, m_Height }; }

void Renderbuffer::setSize(GLuint width, GLuint height) {
    glBindRenderbuffer(GL_RENDERBUFFER, m_Id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    m_Width  = width;
    m_Height = height;
}

void Renderbuffer::resize(GLuint width, GLuint height) {
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    m_Width  = width;
    m_Height = height;
    if (m_Buffer) {
        m_Buffer->bind();
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Id
        );
    }
}

Framebuffer::Framebuffer() { glGenFramebuffers(1, &m_Id); }

Framebuffer::Framebuffer(Framebuffer&& that) noexcept : m_Id(std::exchange(that.m_Id, 0)) {}

Framebuffer& Framebuffer::operator=(Framebuffer&& that) noexcept {
    if (this != &that) {
        Framebuffer temp(std::move(that));
        std::swap(m_Id, temp.m_Id);
    }
    return *this;
}

Framebuffer::~Framebuffer() {
    if (m_Id) {
        glDeleteFramebuffers(1, &m_Id);
    }
}

GLuint Framebuffer::get() const noexcept { return m_Id; }

void Framebuffer::bind() const noexcept { glBindFramebuffer(GL_FRAMEBUFFER, m_Id); }

void Framebuffer::unbind() const noexcept { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

GLenum Framebuffer::getStatus() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
    return glCheckFramebufferStatus(GL_FRAMEBUFFER);
}

bool Framebuffer::complete() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::attachColorAttachment(const ColorAttachment& attachment) {
    GLuint attachmentIndex = GL_COLOR_ATTACHMENT0 + m_ColorAttachments++;
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentIndex, GL_TEXTURE_2D, attachment.m_Id, 0);
    m_Attachments.emplace(attachment.m_Id, attachmentIndex);
}

void Framebuffer::attachRenderbuffer(const Renderbuffer& rbo) {

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo.m_Id
    );
}

} // namespace atom::engine::systems::render
