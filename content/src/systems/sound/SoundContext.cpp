#include "systems/sound/SoundContext.hpp"
#include <alc.h>
#include <easylogging++.h>
#include "systems/sound/SoundDevice.hpp"

namespace atom::engine::systems::sound {

SoundContext::SoundContext(SoundDevice& device) {}

SoundContext::SoundContext(SoundContext&& that) noexcept
    : m_Context(std::exchange(that.m_Context, nullptr)) {}

struct SoundContextAttorney {
    static inline SoundContext create(SoundDevice& dev1ce) {
        SoundContext context(dev1ce);
        context.m_Context = alcCreateContext(dev1ce.get(), nullptr);
        return context;
    }
};

SoundContext& SoundContext::instance() noexcept {
    static SoundContext instance = SoundContextAttorney::create(SoundDevice::instance());
    return instance;
}

void SoundContext::makeCurrent() const noexcept {
    if (m_Context) [[likely]] {
        alcMakeContextCurrent(m_Context);
    }
    else [[unlikely]] {
        LOG(ERROR) << "[Sound] Failed to make context current: m_Context is null";
    }
}

SoundContext::~SoundContext() {
    if (m_Context) [[likely]] {
        alcDestroyContext(m_Context);
    }
    else [[unlikely]] {
        LOG(ERROR) << "[Sound] Failed to destroy context: m_Context is null";
    }
}

} // namespace atom::engine::systems::sound
