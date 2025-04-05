#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

namespace atom {
namespace ecs {}
namespace engine {
using namespace ecs;
namespace math {
using Vector2    = glm::vec2;
using Vector3    = glm::vec3;
using Vector4    = glm::vec4;
using Mat4       = glm::mat4;
using Quaternion = glm::quat;
} // namespace math
} // namespace engine
} // namespace atom

extern template struct glm::vec<2, float>;
extern template struct glm::vec<3, float>;
extern template struct glm::vec<4, float>;
extern template struct glm::mat<2, 2, float>;
extern template struct glm::mat<3, 3, float>;
extern template struct glm::mat<4, 4, float>;
extern template struct glm::qua<float>;
