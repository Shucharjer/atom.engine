#pragma once
#include "systems/render/Model.hpp"

namespace components {

struct Renderable {
    bool visible           = true;
    systems::render::Model* model = nullptr;
};

} // namespace components
