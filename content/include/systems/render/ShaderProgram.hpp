#pragma once
#include <filesystem>
#include <future>
#include <string>
#include <glad/glad.h>
#include <asset.hpp>
#include "pchs/math.hpp"

namespace atom::engine::systems::render {

enum class ShaderType : GLenum {
    VertexShader   = GL_VERTEX_SHADER,
    FragmentShader = GL_FRAGMENT_SHADER
};

struct ShaderInfo {
    std::streamsize infoSize;
    GLenum binaryFormat;
};

struct ShaderProgramInfo {
    std::streamsize infoSize;
    GLenum binaryFormat;
};

class Shader {
public:
    explicit Shader(const std::filesystem::path& path, const ShaderType type);
    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(Shader&&)      = delete;
    ~Shader();

    auto get() const noexcept -> uint32_t;

private:
    GLuint m_Shader;
};

class ShaderProgram {
public:
    /**
     * @brief Construct a new Shader Program object from precompiled file.
     *
     * @param precompiled Path to the precompiled shader program file.
     * @param binaryFormat Binary format of the file.
     */
    ShaderProgram(const std::filesystem::path& precompiled);
    /**
     * @brief Construct a new Shader Program object from source.
     *
     * @param vsh The file path of the vertex shader.
     * @param fsh The file path of the fragment shader.
     */
    ShaderProgram(const std::filesystem::path& vsh, const std::filesystem::path& fsh);
    /**
     * @brief Construct a new Shader Program object.
     *
     * @param precompiled
     * @param vsh
     * @param fsh
     */
    ShaderProgram(
        const std::filesystem::path& precompiled,
        const std::filesystem::path& vsh,
        const std::filesystem::path& fsh
    );

    ShaderProgram(const ShaderProgram&)                = delete;
    ShaderProgram& operator=(const ShaderProgram&)     = delete;
    ShaderProgram(ShaderProgram&&) noexcept            = delete;
    ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

    ~ShaderProgram();

    [[nodiscard]] auto path() const noexcept -> std::string_view;
    [[nodiscard]] constexpr auto type() const noexcept -> ecs::asset_type;

    void use() const noexcept;

    void setFloat(const std::string& name, float val) const;
    void setVec3(const std::string& name, const math::Vector3& vec) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMat4(const std::string& name, const math::Mat4& mat) const;

    /**
     * @brief Save this shader program as binary file.
     *
     * @param path
     */
    bool SaveAsBinary(const std::filesystem::path& path) const;
    std::future<bool> AsyncSaveAsBinary(const std::filesystem::path& path) const;

private:
    GLuint m_Program;
    std::string m_Path;
    std::string m_VertPath;
    std::string m_FragPath;
};

} // namespace atom::engine::systems::render
