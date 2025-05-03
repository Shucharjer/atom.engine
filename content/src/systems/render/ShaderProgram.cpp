#include "systems/render/ShaderProgram.hpp"
//
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <vector>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <core/langdef.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <reflection.hpp>
#include <schedule.hpp>
#include <thread/thread_pool.hpp>
#include "io/file.hpp"
#include "pchs/math.hpp"

using namespace atom;
using namespace atom::ecs;
using namespace atom::engine;
using namespace atom::engine::systems::render;
using namespace atom::engine::io;

namespace fs = std::filesystem;

const static auto kBufSize = 512;

fs::path p(std::string_view("jiaosdjioasd"));

constexpr static std::streamsize kShaderInfoSize        = sizeof(ShaderInfo);
constexpr static std::streamsize kShaderProgramInfoSize = sizeof(ShaderInfo);

ATOM_FORCE_INLINE static ShaderInfo MakeShaderInfo() noexcept {
    return ShaderInfo{ .infoSize = kShaderInfoSize };
}

ATOM_FORCE_INLINE static ShaderProgramInfo MakeShaderProgramInfo() noexcept {
    return ShaderProgramInfo{ .infoSize = kShaderProgramInfoSize };
}

ATOM_FORCE_INLINE std::string_view ShaderTypeString(const ShaderType type) {
    using enum ShaderType;
    switch (type) {
    case VertexShader:
        return "vertex shader";
        break;
    case FragmentShader:
        return "fragment shader";
        break;
    default:
        return "unknown shader";
        break;
    }
}

ATOM_FORCE_INLINE void CheckShaderCompileStatus(uint32_t shader, const ShaderType type) {
    int success = {};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) [[unlikely]] {
        std::array<GLchar, kBufSize> infoLog{};
        glGetShaderInfoLog(shader, kBufSize, nullptr, infoLog.data());
        LOG(ERROR) << ShaderTypeString(type) << " compile failed: " << infoLog.data();
    }
}

Shader::Shader(const fs::path& path, const ShaderType type) {
    // TODO: verify shader type

    if (!Exists(path)) [[unlikely]] {
        LOG(INFO) << ShaderTypeString(type) << " does not exist, path: {" << path << "}";
        return;
    }

    std::ifstream file(path);
    if (!file.is_open()) [[unlikely]] {
        LOG(INFO) << "Could open file at {" << path << "}";
        return;
    }

    std::stringstream stream;
    std::string source;
    const char* this_source = {};
    try {
        stream << file.rdbuf();
        file.close();
        source      = stream.str();
        this_source = source.c_str();
    }
    catch (std::ifstream::failure failure) {
        LOG(ERROR) << "" << failure.what();
    }

    m_Shader = glCreateShader(static_cast<GLenum>(type));
    glShaderSource(m_Shader, 1, &this_source, nullptr);
    glCompileShader(m_Shader);

    CheckShaderCompileStatus(m_Shader, type);
}

Shader::~Shader() { glDeleteShader(m_Shader); }

auto Shader::get() const noexcept -> uint32_t { return m_Shader; }

ATOM_FORCE_INLINE static void CheckShaderProgramLinkStatus(uint32_t program) {
    int success = {};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        std::array<GLchar, kBufSize> infoLog{};
        glGetProgramInfoLog(program, kBufSize, nullptr, infoLog.data());
        LOG(ERROR) << "shader program link failed: " << infoLog.data();
    }
}

ATOM_RELEASE_INLINE static void LoadShaderProgramFromPrecompiled(
    const fs::path& path, uint32_t& program
) {
    std::ifstream stream(path, std::ios::in | std::ios::binary);
    if (stream.is_open()) {
        stream.seekg(0, std::ios::end);
        std::streamsize length = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::pmr::vector<GLubyte> binary(length);
        stream.read(reinterpret_cast<char*>(binary.data()), length);
        stream.close();

        ShaderProgramInfo info = *(ShaderProgramInfo*)binary.data();

        program = glCreateProgram();
        glProgramBinary(
            program,
            info.binaryFormat,
            binary.data() + info.infoSize,
            static_cast<GLsizei>(length - kShaderProgramInfoSize)
        );
        CheckShaderProgramLinkStatus(program);
    }
    else {
        LOG(INFO) << "couldn't open precompiled shader program: {" << path << "}";
    }
}

ATOM_RELEASE_INLINE static void LoadShaderProgramFromSource(
    const fs::path& vsh, const fs::path& fsh, uint32_t& program
) {
    Shader vertShader{ vsh, ShaderType::VertexShader };
    Shader fragShader{ fsh, ShaderType::FragmentShader };
    program = glCreateProgram();
    glAttachShader(program, vertShader.get());
    glAttachShader(program, fragShader.get());
    glLinkProgram(program);
    CheckShaderProgramLinkStatus(program);
}

ShaderProgram::ShaderProgram(const fs::path& precompiled)
    : m_Program(), m_Path(precompiled.string()) {
    if (Exists(precompiled)) {
        LoadShaderProgramFromPrecompiled(precompiled, m_Program);
    }
    else {
        LOG(INFO) << "precompiled shader program not exist: {" << precompiled << "}";
    }
}

ShaderProgram::ShaderProgram(const fs::path& vsh, const fs::path& fsh)
    : m_Program(), m_VertPath(vsh.string()), m_FragPath(fsh.string()) {
    LoadShaderProgramFromSource(vsh, fsh, m_Program);
}

ShaderProgram::ShaderProgram(const fs::path& precompiled, const fs::path& vsh, const fs::path& fsh)
    : m_Program(), m_Path(precompiled.string()), m_VertPath(vsh.string()),
      m_FragPath(fsh.string()) {
    if (Exists(precompiled)) {
        LoadShaderProgramFromPrecompiled(precompiled, m_Program);
    }
    else {
        LOG(INFO) << "precompiled shader program not exist, path: {" << precompiled << "}";
        LoadShaderProgramFromSource(vsh, fsh, m_Program);
    }
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(m_Program); }

[[nodiscard]] auto ShaderProgram::path() const noexcept -> std::string_view { return m_Path; }
[[nodiscard]] constexpr auto ShaderProgram::type() const noexcept -> asset_type {
    return asset_type::shader_program;
}

void ShaderProgram::use() const noexcept { glUseProgram(m_Program); }

ATOM_FORCE_INLINE int UniformLocation(unsigned int program, const std::string& name) {
    return glGetUniformLocation(program, name.c_str());
}

void ShaderProgram::setInt(const std::string& name, int val) const {
    glUniform1i(UniformLocation(m_Program, name), val);
}

void ShaderProgram::setFloat(const std::string& name, float val) const {
    glUniform1f(UniformLocation(m_Program, name), val);
}

void ShaderProgram::setVec3(const std::string& name, const math::Vector3& vec) const {
    glUniform3fv(UniformLocation(m_Program, name), 1, glm::value_ptr(vec));
}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(UniformLocation(m_Program, name), x, y, z);
}

void ShaderProgram::setMat4(const std::string& name, const math::Mat4& mat) const {
    glUniformMatrix4fv(UniformLocation(m_Program, name), 1, GL_FALSE, glm::value_ptr(mat));
}

bool ShaderProgram::SaveAsBinary(const std::filesystem::path& path) const {
    GLint length{};
    glGetProgramiv(m_Program, GL_PROGRAM_BINARY_LENGTH, &length);

    std::pmr::vector<GLubyte> binary(kShaderProgramInfoSize + length);
    auto info = MakeShaderProgramInfo();
    glGetProgramBinary(
        m_Program, length, nullptr, &info.binaryFormat, binary.data() + kShaderProgramInfoSize
    );
    *reinterpret_cast<ShaderProgramInfo*>(binary.data()) = info;
    if (path.begin() == path.end()) {
        return io::WriteAsBinaryFile(m_Path, binary.data(), kShaderProgramInfoSize + length);
    }
    else {
        return io::WriteAsBinaryFile(path, binary.data(), length);
    }
}

std::future<bool> ShaderProgram::AsyncSaveAsBinary(const std::filesystem::path& path) const {
    auto& thread_pool = ecs::scheduler::thread_pool();
    return thread_pool.enqueue(
        [this](const std::filesystem::path& path) { return SaveAsBinary(path); }, path
    );
}
