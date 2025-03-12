#pragma once
#include <string>
#include <containers.hpp>
#include <ecs.hpp>

namespace atom::engine::systems::render {

enum class TextureType : uint8_t {
    Unset,
    BaseColor,
    Ambient,
    Diffuse,
    Specular,
    Emissive,
    Normal,
    Metalness,
    Height,
    Roughness,
    Unknown
};

class Texture {
public:
    Texture();
    explicit Texture(const std::string& path);
    explicit Texture(std::string&& path) noexcept;
    Texture(const Texture&);
    Texture(Texture&&) noexcept;
    Texture& operator=(const Texture&);
    Texture& operator=(Texture&&) noexcept;

    /**
     * @brief Destroy the Texture object
     * @warning Only destroy objects in it. Please manage proxy manually.
     */
    ~Texture() noexcept;

    [[nodiscard]] const std::string& path() const noexcept;
    [[nodiscard]] constexpr auto type() const noexcept -> asset_type;

    struct Proxy {
        uint32_t m_Id;
    };

    using proxy_type = Proxy;
    using key_type   = std::string;

    [[nodiscard]] resource_handle get_handle() const noexcept { return m_Handle; }
    void set_handle(const resource_handle handle) noexcept { m_Handle = handle; }

    [[nodiscard]] auto load(
        bool correction = false,
        GLint wrapS     = GL_REPEAT,
        GLint wrapT     = GL_REPEAT,
        GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
        GLint magFilter = GL_LINEAR_MIPMAP_LINEAR
    ) const -> shared_ptr<Texture::Proxy>;
    void unload() noexcept;
    void reload(const resource_handle handle, const key_type& key, const TextureType type);

    [[nodiscard]] TextureType getType() const noexcept;
    void setType(const TextureType type) noexcept;

private:
    resource_handle m_Handle;
    TextureType m_Type;
    std::string m_Path;
};

} // namespace atom::engine::systems::render
