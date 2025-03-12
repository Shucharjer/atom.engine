#pragma once

#include <filesystem>
#include <sstream>
#include <string>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <easylogging++.h>
#include <imgui.h>
#include <tinyfiledialogs.h>
#include <containers.hpp>
#include <ecs.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Only use this header file in cpp
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

#include <core/langdef.hpp>
#include <meta.hpp>

#include <schedule.hpp>
#include <world.hpp>

namespace atom::engine {
using namespace ecs;
namespace math {
using Vector3    = glm::vec3;
using Vector4    = glm::vec4;
using Mat4       = glm::mat4;
using Quaternion = glm::quat;
} // namespace math
} // namespace atom::engine

namespace engine = atom::engine;

template class std::basic_string<char>;
