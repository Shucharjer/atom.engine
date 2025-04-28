#include "systems/sound/SoundDevice.hpp"
#include <alc.h>
#include <easylogging++.h>
#include <asset.hpp>

namespace atom::engine::systems::sound {

SoundDevice::SoundDevice(const ALchar* deviceName) noexcept : m_Device(alcOpenDevice(deviceName)) {
    if (!m_Device) {
        LOG(ERROR) << "Failed to open sound device: " << deviceName;
        return;
    }
}

SoundDevice& SoundDevice::instance() noexcept {
    static SoundDevice instance(nullptr);
    return instance;
}

SoundDevice::~SoundDevice() noexcept {
    if (m_Device) {
        alcCloseDevice(m_Device);
    }
}

} // namespace atom::engine::systems::sound
