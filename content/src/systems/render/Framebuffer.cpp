#include "systems/render/Framebuffer.hpp"
#include <stdexcept>
#include <easylogging++.h>
#include <core/langdef.hpp>
#include "pchs/graphics.hpp"

using namespace atom::engine::systems::render;

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

void Framebuffer::attachTexture(GLuint texture, GLenum target, const AttachmentType type) {
    GLenum attachment{};
    using atype = AttachmentType;
    switch (type) {
    case atype::Color:
        attachment = GL_COLOR_ATTACHMENT0 + m_ColorAttachments++;
        break;
    case atype::Depth:
        attachment = GL_DEPTH_ATTACHMENT;
        break;
    case atype::Stencil:
        attachment = GL_STENCIL_ATTACHMENT;
        break;
    case atype::DepthStencil:
        attachment = GL_DEPTH_STENCIL_ATTACHMENT;
        break;
    default:
        LOG(ERROR) << "Invalid texture attachment type.";
        break;
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, texture, 0);
}

void Framebuffer::attachRenderBuffer(GLuint rbo, const AttachmentType type) {
    GLenum attachment{};
    using atype = AttachmentType;
    switch (type) {
    case atype::Depth:
        attachment = GL_DEPTH_ATTACHMENT;
        break;
    case atype::Stencil:
        attachment = GL_STENCIL_ATTACHMENT;
        break;
    case atype::DepthStencil:
        attachment = GL_DEPTH_STENCIL_ATTACHMENT;
        break;
    default:
        throw std::invalid_argument("Invalid renderbuffer attachment type");
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
}

void Framebuffer::attach(
    AttachmentType type, GLenum internalFormat, GLenum format, GLenum dataType
) {
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    if (m_Samples) {
        glRenderbufferStorageMultisample(
            GL_RENDERBUFFER, m_Samples, internalFormat, m_Width, m_Height
        );
    }
    else {
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_Width, m_Height);
    }
    attachRenderBuffer(rbo, type);
}

void Framebuffer::bind(GLenum target) const noexcept {
    glBindFramebuffer(target, m_Id);
    if (target == GL_DRAW_FRAMEBUFFER) {
        std::vector<GLenum> buffers(m_ColorAttachments, GL_COLOR_ATTACHMENT0);
        for (auto i = 0; i < m_ColorAttachments; ++i) {
            buffers[i] += i;
        }
        glDrawBuffers((GLsizei)buffers.size(), buffers.data());
    }
}

void Framebuffer::unbind() const noexcept { glBindFramebuffer(GL_FRAMEBUFFER, m_Id); }

void Framebuffer::resize(std::uint32_t width, std::uint32_t height) {
    m_Width  = width;
    m_Height = height;

    // TODO: regenerate objects...
}

bool Framebuffer::checkStatus() const noexcept {
    bind();
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

GLuint Framebuffer::getColorTexture(const AttachmentType type) const { return m_Textures.at(type); }
