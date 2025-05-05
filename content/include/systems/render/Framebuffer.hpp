#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <containers.hpp>
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

class Framebuffer;
struct FramebufferAttorney;

class ColorAttachment {
    friend Framebuffer;

public:
    ColorAttachment();
    ColorAttachment(GLuint width, GLuint height);
    ColorAttachment(ColorAttachment&&) noexcept;
    ColorAttachment& operator=(ColorAttachment&&) noexcept;
    ColorAttachment(const ColorAttachment&)            = delete;
    ColorAttachment& operator=(const ColorAttachment&) = delete;
    ~ColorAttachment() noexcept;

    [[nodiscard]] operator bool() const noexcept;

    [[nodiscard]] math::Vector2 size() const noexcept;

    void setSize(GLuint width, GLuint height);

    void resize(GLuint width, GLuint height);

    void bind();

private:
    GLuint m_Id{};
    GLuint m_Width{};
    GLuint m_Height{};
    Framebuffer* m_Buffer{};
};

/**
 * @class Renderbuffer
 * @brief Renderbuffer Object, RBO, usually used to attach into framebuffer as depth stencil
 * attachment.
 *
 */
class Renderbuffer {
    friend Framebuffer;

public:
    Renderbuffer();
    Renderbuffer(GLuint width, GLuint height);
    Renderbuffer(Renderbuffer&&) noexcept;
    Renderbuffer& operator=(Renderbuffer&&) noexcept;
    Renderbuffer(const Renderbuffer&)            = delete;
    Renderbuffer& operator=(const Renderbuffer&) = delete;
    ~Renderbuffer() noexcept;

    [[nodiscard]] operator bool() const noexcept;

    [[nodiscard]] math::Vector2 size() const noexcept;

    void setSize(GLuint width, GLuint height);

    void resize(GLuint width, GLuint height);

private:
    GLuint m_Id{};
    GLuint m_Width{};
    GLuint m_Height{};
    Framebuffer* m_Buffer{};
};

/**
 * @class Framebuffer
 * @brief A buffer stored in GPU memory that can be used as a render target.
 * A complete framebuffer consists of at least one buffer (color, depth or stencil), at least one
 * color attachment.
 */
class Framebuffer {
    friend FramebufferAttorney;

public:
    Framebuffer();
    Framebuffer(Framebuffer&&) noexcept;
    Framebuffer& operator=(Framebuffer&&) noexcept;
    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    ~Framebuffer();

    [[nodiscard]] GLuint get() const noexcept;

    /**
     * @brief Bind this framebuffer as the using one.
     */
    void bind() const noexcept;

    /**
     * @brief Unbind the framebuffer, which means binding to the default framebuffer.
     */
    void unbind() const noexcept;

    /**
     * @brief Get the framebuffer status.
     * @warning You should bind this framebuffer before calling this function.
     * @return GLenum GL_FRAMEBUFFER_COMPLETE or GL_FRAMEBUFFER_INCOMPLETE_*
     */
    [[nodiscard]] GLenum getStatus() const noexcept;

    /**
     * @brief Whether this framebuffer is complete.
     * @warning You should bind this framebuffer before calling this function.
     */
    [[nodiscard]] bool complete() const noexcept;

    /**
     * @brief Attach a color attachment.
     * @warning You should bind this framebuffer before calling this function.
     */
    void attachColorAttachment(const ColorAttachment& attachment);

    /**
     * @brief Attach a renderbuffer as depth & stencil attachment.
     * @warning You should bind this framebuffer before calling this function.
     */
    void attachRenderbuffer(const Renderbuffer& rbo);

private:
    GLuint m_Id{};
    std::uint32_t m_Width{};
    std::uint32_t m_Height{};
    std::uint32_t m_Samples{};
    std::uint32_t m_ColorAttachments{};
    atom::map<GLuint, GLuint> m_Attachments;
};

struct OutputToFramebuffer {
    bool value = false;
};

} // namespace atom::engine::systems::render
