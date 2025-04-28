#include "systems/sound/SoundSource.hpp"
#include <al.h>
#include "pchs/math.hpp"

namespace atom::engine::systems::sound {

SoundSource::SoundSource() : m_Source() {
    alGenSources(1, &m_Source);
    alSourcef(m_Source, AL_PITCH, 1.0f);
    alSourcef(m_Source, AL_GAIN, 1.0f);
    alSource3f(m_Source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(m_Source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
}

SoundSource::SoundSource(SoundSource&& that) noexcept : m_Source(std::exchange(that.m_Source, 0)) {}

SoundSource& SoundSource::operator=(SoundSource&& that) noexcept {
    if (this != &that) {
        m_Source = std::exchange(that.m_Source, 0);
    }
    return *this;
}

SoundSource::~SoundSource() noexcept {
    if (m_Source) {
        alDeleteSources(1, &m_Source);
    }
}

void SoundSource::setDistanceModel(const DistanceModel model) const noexcept {
    alDistanceModel(static_cast<ALenum>(model));
}

void SoundSource::setSpeed(float speed) const noexcept {
    alSourcef(m_Source, AL_SPEED_OF_SOUND, speed);
}

void SoundSource::setPosition(float x, float y, float z) noexcept {
    alSource3f(m_Source, AL_POSITION, x, y, z);
}

void SoundSource::setPosition(const math::Vector3& position) noexcept {
    alSourcefv(m_Source, AL_POSITION, &position.x);
}

math::Vector3 SoundSource::getPosition() const noexcept {
    math::Vector3 position;
    alGetSourcefv(m_Source, AL_POSITION, (float*)&position);
    return position;
}

void SoundSource::setOrientation(const float* orientation) const noexcept {
    alSourcefv(m_Source, AL_ORIENTATION, orientation);
}

void SoundSource::setOrientation(
    const float forwardX,
    const float forwardY,
    const float forwardZ,
    const float upX,
    const float upY,
    const float upZ

) const noexcept {
    const float orientation[] = { forwardX, forwardY, forwardZ, upX, upY, upZ };
    alSourcefv(m_Source, AL_ORIENTATION, static_cast<const float*>(orientation));
}

void SoundSource::setOrientation(const math::Vector3& direction, const math::Vector3& up)
    const noexcept {
    const float orientation[] = { direction.x, direction.y, direction.z, up.x, up.y, up.z };
    alSourcefv(m_Source, AL_ORIENTATION, static_cast<const float*>(orientation));
}

std::pair<math::Vector3, math::Vector3> SoundSource::getOrientation() const noexcept {
    std::pair<math::Vector3, math::Vector3> ori;
    alGetSourcefv(m_Source, AL_ORIENTATION, reinterpret_cast<float*>(&ori));
    return ori;
}

void SoundSource::bufferData(ecs::resource_handle buffer) noexcept {
    if (m_Source) [[likely]] {
        alSourcei(m_Source, AL_BUFFER, static_cast<ALint>(buffer));
    }
}

void SoundSource::play() const noexcept {
    if (m_Source) [[likely]] {
        alSourcePlay(m_Source);
    }
}

void SoundSource::pause() const noexcept {
    if (m_Source) [[likely]] {
        alSourcePause(m_Source);
    }
}

void SoundSource::stop() const noexcept {
    if (m_Source) [[likely]] {
        alSourceStop(m_Source);
    }
}

} // namespace atom::engine::systems::sound
