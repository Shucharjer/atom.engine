#pragma once
#include <glad/glad.h>
//
#include <string>
#include <GLFW/glfw3.h>

namespace asserts {

struct ShaderProgram {
    bool isBinary;
    GLenum binaryFormat;
    std::string path;
};

}