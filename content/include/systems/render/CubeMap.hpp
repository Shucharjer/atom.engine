#pragma once

namespace atom::engine::systems::render {

class CubeMap {
public:
    CubeMap();
    CubeMap(CubeMap&& that) noexcept;
    CubeMap& operator=(CubeMap&& that) noexcept;
    ~CubeMap() noexcept;

    CubeMap(const CubeMap&)            = delete;
    CubeMap& operator=(const CubeMap&) = delete;

private:
};

} // namespace atom::engine::systems::render
