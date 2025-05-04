#pragma once
#include <asset.hpp>
#include <core/langdef.hpp>
#include "pchs/graphics.hpp"
#include "pchs/math.hpp"
#include "systems/render/Texture.hpp"

namespace atom::engine::systems::render {

constexpr float kDefaultAmbientFactor = 0.2F;

/**
 * @class Material Describes a sets of properties that affects how a surface interacts with light.
 * @brief A aggregate of all the properties that are used to render a surface.
 */
struct Material {
    math::Vector4 baseColorFactor{ 1.0F };
    float metallicFactor{ 1.0F };
    float roughnessFactor{ 1.0F };
    float ambientFactor = kDefaultAmbientFactor;
    float _padding;
    math::Vector3 emissiveFactor{ 0.0F };
    float _padding_;
    math::Vector3 ambientLight{ 1.0F };

    Texture baseColorTexture;
    Texture ambientTexture;
    Texture diffuseTexture;
    Texture specularTexture;
    Texture emissiveTexture;
    Texture metallicTexture;
    Texture roughnessTexture;
    Texture occlusionTexture;
    Texture normalTexture;

    std::string name;
};

} // namespace atom::engine::systems::render
