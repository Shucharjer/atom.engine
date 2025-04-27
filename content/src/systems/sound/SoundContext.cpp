#include "systems/sound/SoundContext.hpp"
#include <alc.h>
#include <easylogging++.h>
#include "systems/sound/SoundDevice.hpp"

namespace atom::engine::systems::sound {

SoundContext::SoundContext(SoundDevice& device) {}

SoundContext& SoundContext::instance() noexcept {
    auto& device = SoundDevice::instance();
    static SoundContext instance(device);
    instance.m_Context = alcCreateContext(device.m_Device, nullptr);
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
