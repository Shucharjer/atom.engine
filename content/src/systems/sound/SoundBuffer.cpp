#include "systems/sound/SoundBuffer.hpp"
#include <al.h>

namespace atom::engine::systems::sound {

SoundBuffer::SoundBuffer() noexcept { alGenBuffers(1, &m_Buffer); }

SoundBuffer::~SoundBuffer() noexcept {
    if (m_Buffer) [[likely]] {
        alDeleteBuffers(1, &m_Buffer);
    }
}

void SoundBuffer::setData(
    const Format format, const void* data, const int size, const int sampleRate
) noexcept {
    alBufferData(m_Buffer, static_cast<ALenum>(format), data, size, sampleRate);
}

ALuint SoundBuffer::get() const noexcept { return m_Buffer; }

} // namespace atom::engine::systems::sound
