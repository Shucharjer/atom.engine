#include "systems/render/CubeMap.hpp"
#include <array>
#include <string>
#include <utility>
#include <stb_image.h>
#include "pchs/graphics.hpp"
#include "systems/render/Texture.hpp"

using namespace atom::engine;
using namespace atom::engine::systems::render;

static const float kCubeMapVertices[] = {
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

    1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f
};

CubeMap::CubeMap() = default;

CubeMap::CubeMap(const std::string& path) : m_Path(path) {}

CubeMap::CubeMap(std::string&& path) noexcept : m_Path(std::move(path)) {}

CubeMap::CubeMap(CubeMap&& that) noexcept
    : m_Handle(std::exchange(that.m_Handle, 0)), m_Path(std::move(that.m_Path)) {}

CubeMap& CubeMap::operator=(CubeMap&& that) noexcept {
    if (this != &that) {
        CubeMap temp(std::move(that));
        std::swap(m_Handle, temp.m_Handle);
        std::swap(m_Path, temp.m_Path);
    }
    return *this;
}

CubeMap::~CubeMap() noexcept = default;

static void ReadTexture2D(const std::string& path, GLenum target) {
    int width{}, height{}, channels{};
    auto* bytes = stbi_load(path.c_str(), &width, &height, &channels, 0);

    // NOTE: not corrected to srgb.
    GLint internalFormat{}, format{};
    switch (channels) {
    case 3:
        internalFormat = GL_RGB;
        format         = GL_RGB;
    case 4:
    default:
        internalFormat = GL_RGBA;
        format         = GL_RGBA;
    }
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, bytes);
    stbi_image_free(bytes);
}

auto CubeMap::load() -> shared_ptr<CubeMap::Proxy> {
    auto proxy = std::make_shared<CubeMap::Proxy>();
    auto& VAO  = proxy->vao;
    auto& VBO  = proxy->vbo;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeMapVertices), &kCubeMapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glGenTextures(1, &proxy->texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, proxy->texture);
    const auto kCubeMapFaces = 6;

    const std::array<std::string, kCubeMapFaces> faces = { m_Path + "_right", m_Path + "_left",
                                                           m_Path + "_top",   m_Path + "_bottom",
                                                           m_Path + "_front", m_Path + "_back" };

    for (auto i = 0; i < kCubeMapFaces; ++i) {
        ReadTexture2D(faces[i], GL_TEXTURE_CUBE_MAP_NEGATIVE_X + i);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    return proxy;
}

void CubeMap::unload() noexcept {
    //
}
