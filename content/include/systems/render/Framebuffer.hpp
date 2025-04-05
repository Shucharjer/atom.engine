#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <containers.hpp>

namespace atom::engine::systems::render {

class Framebuffer {
public:
    enum class AttachmentType : std::uint8_t {
        Color,
        Depth,
        Stencil,
        DepthStencil
    };

    Framebuffer();
    Framebuffer(Framebuffer&&) noexcept;
    Framebuffer& operator=(Framebuffer&&) noexcept;
    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    ~Framebuffer();

    void attach(AttachmentType type, GLenum internalFormat, GLenum format, GLenum dataType);
    void bind(GLenum target = GL_FRAMEBUFFER) const noexcept;
    void unbind() const noexcept;
    void resize(std::uint32_t width, std::uint32_t height);
    [[nodiscard]] bool checkStatus() const noexcept;
    [[nodiscard]] GLuint getColorTexture(const AttachmentType type = AttachmentType::Color) const;

    /**
     * @brief Add texture attachment.
     *
     * @param type Type of attachment.
     * @param internalFormat Internal format, like GL_RGB32F, GL_RGB16F, GL_RGBA...
     * @param format Format, such as GL_RGB, GL_RGBA
     * @param dataType Type of data, such as GL_FLOAT or GL_UNSIGNED_BYTE
     */
    void addTextureAttachment(
        const AttachmentType type, GLenum internalFormat, GLenum format, GLenum dataType
    );

    /**
     * @brief Add renderbuffer.
     *
     * @param type Type of attachment.
     * @param internalFormat Internal format, such as GL_DEPTH24_STENCIL8
     */
    void addRenderbuffer(const AttachmentType type, GLenum internalFormat);

private:
    void attachTexture(GLuint texture, GLenum target, const AttachmentType type);
    void attachRenderBuffer(GLuint rbo, const AttachmentType type);

    GLuint m_Id{};
    GLuint m_Rbo{};
    std::uint32_t m_Width{};
    std::uint32_t m_Height{};
    std::uint32_t m_Samples{};
    std::uint32_t m_ColorAttachments{};
    atom::map<AttachmentType, GLuint> m_Textures;
};

struct OutputToFramebuffer {
    bool value = false;
};

} // namespace atom::engine::systems::render
