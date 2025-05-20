#pragma once
#include <al.h>
#include <alc.h>
#include "systems/sound/SoundDevice.hpp"

namespace atom::engine::systems::sound {
struct SoundContextAttorney;
/**
 * @class SoundContext
 * @brief A basic class for OpenAL context management. You need to create an instance of this class
 *        before using OpenAL functions.
 * @details
 *
 */
class SoundContext {
    friend SoundContextAttorney;

public:
    SoundContext(SoundContext&&) noexcept;
    SoundContext& operator=(SoundContext&&)      = delete;
    SoundContext(const SoundContext&)            = delete;
    SoundContext& operator=(const SoundContext&) = delete;
    ~SoundContext();

    static SoundContext& instance() noexcept;

    void makeCurrent() const noexcept;

private:
    SoundContext(SoundDevice& dev1ce);
    ALCcontext* m_Context{ nullptr };
};
} // namespace atom::engine::systems::sound
