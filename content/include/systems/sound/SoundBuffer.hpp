#pragma once
#include <al.h>
#include <ecs.hpp>

namespace atom::engine::systems::sound {

/**
 * @class SoundBuffer.
 * @brief A buffer object contains sound data, which is used to play sound by a sound source.
 *        One or more sound files should be loaded into a buffer object, but a sound source could
 * use only one buffer object at a time.
 */
class SoundBuffer {
public:
    SoundBuffer() noexcept;
    SoundBuffer(const SoundBuffer&)            = delete;
    SoundBuffer(SoundBuffer&&)                 = delete;
    SoundBuffer& operator=(const SoundBuffer&) = delete;
    SoundBuffer& operator=(SoundBuffer&&)      = delete;
    ~SoundBuffer() noexcept;

    enum class Format : uint16_t {
        Mono8    = AL_FORMAT_MONO8,
        Mono16   = AL_FORMAT_MONO16,
        Stereo8  = AL_FORMAT_STEREO8,
        Stereo16 = AL_FORMAT_STEREO16,
    };

    void setData(
        const Format format, const void* data, const int size, const int sampleRate
    ) noexcept;

    [[nodiscard]] ALuint get() const noexcept;

private:
    ALuint m_Buffer{};
};

} // namespace atom::engine::systems::sound
