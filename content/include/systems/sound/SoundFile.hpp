#pragma once
#include <string>
#include <al.h>
#include <asset.hpp>
#include <ecs.hpp>
#include "systems/sound/SoundBuffer.hpp"

namespace atom::engine::systems::sound {

class SoundFile {
public:
    SoundFile();
    SoundFile(const std::string& path);
    SoundFile(std::string&& path) noexcept;
    SoundFile(const SoundFile& that);
    SoundFile& operator=(const SoundFile& that);
    SoundFile(SoundFile&& that) noexcept;
    SoundFile& operator=(SoundFile&& that) noexcept;
    ~SoundFile();

    using key_type = std::string;

    struct Proxy {
        SoundBuffer buffer;
    };

    using proxy_type = Proxy;

    [[nodiscard]] auto get_handle() const noexcept -> ecs::resource_handle;

    void set_handle(const ecs::resource_handle handle) noexcept;

    constexpr auto type() -> ecs::asset_type { return ecs::asset_type::sound; }

    [[nodiscard]] auto load() const -> std::shared_ptr<proxy_type>;

    void unload() noexcept;

    operator bool() const noexcept { return !m_Path.empty(); }

private:
    ecs::resource_handle m_Handle{};
    std::string m_Path;
};

} // namespace atom::engine::systems::sound
