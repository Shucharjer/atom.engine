#include "systems/render/Texture.hpp"
#include <memory>
#include <easylogging++.h>
#include <asset.hpp>
#include "pchs/logger.hpp"
#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace atom::ecs;
using namespace atom::engine::systems::render;

Texture::Texture() : m_Handle(), m_Type() {}
Texture::Texture(const std::string& path) : m_Handle(), m_Type(), m_Path(path) {}
Texture::Texture(std::string&& path) noexcept : m_Handle(), m_Type(), m_Path(std::move(path)) {}

Texture::Texture(const Texture& that) = default;

Texture::Texture(Texture&& that) noexcept
    : m_Handle(that.m_Handle), m_Type(that.m_Type), m_Path(std::move(that.m_Path)) {}

Texture& Texture::operator=(const Texture& that) {
    if (this == &that) [[unlikely]] {
        return *this;
    }

    // if (m_Handle == that.m_Handle) [[unlikely]] {
    //     return *this;
    // }

    // reload(m_Handle, m_Path, m_Type);

    m_Handle = that.m_Handle;
    m_Type   = that.m_Type;
    m_Path   = that.m_Path;

    return *this;
}

Texture& Texture::operator=(Texture&& that) noexcept {
    if (this == &that) [[unlikely]] {
        return *this;
    }

    if (m_Path == that.m_Path) [[unlikely]] {
        return *this;
    }

    // unload();

    m_Handle = std::exchange(that.m_Handle, 0);
    m_Type   = that.m_Type;
    m_Path   = std::move(that.m_Path);

    return *this;
}

Texture::~Texture() noexcept = default;

[[nodiscard]] const std::string& Texture::path() const noexcept { return m_Path; }
[[nodiscard]] constexpr auto Texture::type() const noexcept -> asset_type {
    return ecs::asset_type::texture;
}

auto Texture::load(bool correction, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter)
    const -> shared_ptr<Texture::Proxy> {
    auto proxy = std::make_shared<Texture::Proxy>();
    glGenTextures(1, &proxy->m_Id);

    int width{}, height{}, channels{};
    GLubyte* data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);
    if (data) [[likely]] {
        GLint sFormat{};
        GLenum format{};

        if (correction) {
            switch (channels) {
            case 3:
                format  = GL_RGB;
                sFormat = GL_SRGB;
                break;
            case 4:
                format  = GL_RGBA;
                sFormat = GL_SRGB_ALPHA;
                break;
            default:
                LOG(INFO) << "Unexpected channel count: " << channels << ", path: {" << m_Path
                          << "}";
                break;
            }
        }
        else {
            switch (channels) {
            case 3:
                sFormat = GL_RGB;
                format  = GL_RGB;
                break;
            case 4:
                sFormat = GL_RGBA;
                format  = GL_RGBA;
                break;
            default:
                LOG(INFO) << "Unexpected channel count: " << channels << ", path: {" << m_Path
                          << "}";
                break;
            }
        }

        glBindTexture(GL_TEXTURE_2D, proxy->m_Id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexImage2D(GL_TEXTURE_2D, 0, sFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        LOG(INFO) << "Couldn't load texture: {" << m_Path << "}";
    }
    stbi_image_free(data);

    return proxy;
}

void Texture::unload() noexcept {
    if (m_Handle) {
        auto& hub   = hub::instance();
        auto& table = hub.table<Texture>();
        if (table.count(m_Path) == 1) {
            auto& library = hub.library<Texture>();
            auto proxy    = library.fetch(m_Handle);
            glDeleteTextures(1, &proxy->m_Id);
            library.uninstall(m_Handle);
        }
        table.erase(m_Path);
        m_Handle = 0;
    }
}

void Texture::reload(const resource_handle handle, const key_type& key, const TextureType type) {
    auto& hub   = hub::instance();
    auto& table = hub.table<Texture>();

    if (m_Handle) {
        auto& library = hub.library<Texture>();
        if (table.count(m_Path) == 1) {
            auto proxy = library.fetch(m_Handle);
            library.uninstall(m_Handle);
        }
        table.erase(m_Path);
        m_Handle = 0;
    }

    if (handle) {
        table.emplace(key, handle);
    }

    m_Handle = handle;
    m_Path   = key;
    m_Type   = type;
}

TextureType Texture::getType() const noexcept { return m_Type; }
void Texture::setType(const TextureType type) noexcept { m_Type = type; }

Texture::operator bool() const noexcept { return m_Handle; }
