#include "systems/sound/SoundListener.hpp"
#include <al.h>
#include <alc.h>
#include "pchs/math.hpp"

namespace atom::engine::systems::sound {

SoundListener::SoundListener() {}

void SoundListener::setPosition(const float* position) const noexcept {
    alListenerfv(AL_POSITION, position);
}

void SoundListener::setPosition(const math::Vector3& position) const noexcept {
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void SoundListener::setPosition(const float x, const float y, const float z) const noexcept {
    alListener3f(AL_POSITION, x, y, z);
}

void SoundListener::setOrientation(const float* orientation) const noexcept {
    alListenerfv(AL_ORIENTATION, orientation);
}

void SoundListener::setOrientation(const math::Vector3& forward, const math::Vector3& up)
    const noexcept {
    const math::Vector3 temp[] = { forward, up };
    alListenerfv(AL_ORIENTATION, reinterpret_cast<const float*>(&temp));
}

void SoundListener::setOrientation(
    const float forwardX,
    const float forwardY,
    const float forwardZ,
    const float upX,
    const float upY,
    const float upZ
) const noexcept {
    const float temp[] = { forwardX, forwardY, forwardZ, upX, upY, upZ };
    alListenerfv(AL_ORIENTATION, static_cast<const float*>(temp));
}

std::pair<math::Vector3, math::Vector3> SoundListener::getOrientation() const noexcept {
    std::pair<math::Vector3, math::Vector3> temp;
    alGetListenerfv(AL_ORIENTATION, reinterpret_cast<float*>(&temp));
    return temp;
}

} // namespace atom::engine::systems::sound
