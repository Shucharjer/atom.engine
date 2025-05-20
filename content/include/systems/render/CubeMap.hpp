#pragma once
#include <asset.hpp>
#include <ecs.hpp>
#include <reflection.hpp>
#include "pchs/graphics.hpp"

namespace atom::engine::systems::render {

class CubeMap {
public:
    CubeMap();
    CubeMap(const std::string& path);
    CubeMap(std::string&& path) noexcept;
    CubeMap(CubeMap&& that) noexcept;
    CubeMap& operator=(CubeMap&& that) noexcept;
    ~CubeMap() noexcept;

    CubeMap(const CubeMap&)            = delete;
    CubeMap& operator=(const CubeMap&) = delete;

    struct Proxy {
        GLuint vao;
        GLuint vbo;
        GLuint texture;
    };

    using proxy_type = Proxy;
    using key_type   = std::string;

    [[nodiscard]] auto load() -> shared_ptr<CubeMap::Proxy>;
    void unload() noexcept;

    [[nodiscard]] ecs::resource_handle get_handle() const noexcept { return m_Handle; };

    void set_handle(const ecs::resource_handle handle) noexcept { m_Handle = handle; };

    REFL_MEMBERS(CubeMap, m_Path);

private:
    ecs::resource_handle m_Handle{};
    std::string m_Path;
};

} // namespace atom::engine::systems::render
