#pragma once
#include <string>
#include <asset.hpp>
#include <command.hpp>
#include <ecs.hpp>
#include "systems/render/Animation.hpp"
#include "systems/render/Mesh.hpp"

namespace atom::engine::systems::render {

class Model {
public:
    explicit Model(const std::string& path);
    explicit Model(std::string&& path) noexcept;
    Model(const Model&);
    Model(Model&&) noexcept;
    Model& operator=(const Model&);
    Model& operator=(Model&&) noexcept;

    /**
     * @brief Destroy the Model object
     * @warning Only destroy objects in it. Please manage proxy manually.
     */
    ~Model() noexcept;

    [[nodiscard]] const std::string& path() const noexcept;
    [[nodiscard]] constexpr auto type() const noexcept -> ecs::asset_type;

    struct Proxy {
        bool visibility = true;
        math::Mat4 boneMatrix;

        atom::vector<Mesh> meshes;
        atom::vector<Animation> animations;
    };

    using proxy_type = Proxy;
    using key_type   = std::string;

    [[nodiscard]] auto load() const -> shared_ptr<Model::Proxy>;
    void unload() noexcept;
    void reload(const key_type& key, const resource_handle handle);

    [[nodiscard]] resource_handle get_handle() const noexcept { return m_Handle; }

    void set_handle(const resource_handle handle) noexcept { m_Handle = handle; }

    REFL_MEMBERS(Model, m_Path)

    void draw(library<Model>& library, ShaderProgram& program);

private:
    resource_handle m_Handle;
    std::string m_Path;
};

} // namespace atom::engine::systems::render
