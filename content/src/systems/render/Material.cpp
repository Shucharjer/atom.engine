#include "systems/render/Material.hpp"
#include <easylogging++.h>
#include <asset.hpp>
#include "pchs/graphics.hpp"
#include "systems/render/ShaderProgram.hpp"
#include "systems/render/Texture.hpp"

namespace atom::engine::systems::render {

static void TryApplyTexture(
    Texture& texture,
    ShaderProgram& shaderProgram,
    library<Texture>& library,
    const std::string& name,
    uint8_t& index
) {
    if (!texture) {
        return;
    }

    auto proxy = library.fetch(texture.get_handle());

#ifdef ATOM_OPT_SHOW_TEXTURE_ACTIVE
    LOG(DEBUG) << std::format("active {}: {}", name, index);
#endif

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, proxy->m_Id);
    shaderProgram.setInt(name, index);
    ++index;
}

void Material::apply(ShaderProgram& shaderProgram) {
    shaderProgram.setVec4("material.baseColorFactor", baseColorFactor);
    shaderProgram.setFloat("material.metallicFactor", metallicFactor);
    shaderProgram.setFloat("material.roughnessFactor", roughnessFactor);
    shaderProgram.setVec3("material.emissiveFactor", emissiveFactor);
    auto& hub     = hub::instance();
    auto& library = hub.library<Texture>();
    uint8_t index{};
    TryApplyTexture(baseColorTexture, shaderProgram, library, "material.baseColorTexture", index);
    TryApplyTexture(occlusionTexture, shaderProgram, library, "material.occlusionTexture", index);
    TryApplyTexture(metallicTexture, shaderProgram, library, "material.metallicTexture", index);
    TryApplyTexture(roughnessTexture, shaderProgram, library, "material.roughnessTexture", index);
    TryApplyTexture(emissionTexture, shaderProgram, library, "material.emissionTexture", index);
}

} // namespace atom::engine::systems::render
