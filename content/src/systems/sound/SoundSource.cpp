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

void SoundSource::setDirection(const float* direction) const noexcept {
    alSourcefv(m_Source, AL_DIRECTION, direction);
}

void SoundSource::setDirection(
    const float x, const float y, const float z

) const noexcept {
    const float temp[] = { x, y, z };
    alSourcefv(m_Source, AL_ORIENTATION, static_cast<const float*>(temp));
}

void SoundSource::setDirection(const math::Vector3& direction) const noexcept {
    const float temp[] = { direction.x, direction.y, direction.z };
    alSourcefv(m_Source, AL_ORIENTATION, static_cast<const float*>(temp));
}

void SoundSource::bufferData(ALuint buffer) noexcept {
    if (m_Source) [[likely]] {
        alSourcei(m_Source, AL_BUFFER, buffer);
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

bool SoundSource::isPlaying() const noexcept {
    ALint state{};
    alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

bool SoundSource::isPaused() const noexcept {
    ALint state{};
    alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
    return state == AL_PAUSED;
}

bool SoundSource::isStopped() const noexcept {
    ALint state{};
    alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
    return state == AL_STOPPED;
}
} // namespace atom::engine::systems::sound
