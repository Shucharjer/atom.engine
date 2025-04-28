#pragma once
#include <memory>
#include <al.h>
#include <alc.h>
#include <ecs.hpp>

namespace atom::engine::systems::sound {

/**
 * @class SoundDevice
 * @brief A basic sound device wrapper for OpenAL. You need to create an instance of this class
 *        before using OpenAL functions.
 * @details This class manages the OpenAL device and provides a simple interface for
 *          creating and destroying the device. Caused by the design of OpenAL, it is not
 *          thread-safe and should be used in a single thread.
 */
class SoundDevice {
    friend class SoundContext;

public:
    SoundDevice(const SoundDevice&)            = delete;
    SoundDevice(SoundDevice&&)                 = delete;
    SoundDevice& operator=(const SoundDevice&) = delete;
    SoundDevice& operator=(SoundDevice&&)      = delete;
    ~SoundDevice() noexcept;

    static SoundDevice& instance() noexcept;

private:
    SoundDevice(const ALchar* deviceName) noexcept;
    ALCdevice* m_Device;
};

} // namespace atom::engine::systems::sound
