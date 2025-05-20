#pragma once
#include <al.h>
#include <ecs.hpp>

namespace atom::engine::systems::sound {

enum class SoundFormat : uint16_t {
    Mono8    = AL_FORMAT_MONO8,
    Mono16   = AL_FORMAT_MONO16,
    Stereo8  = AL_FORMAT_STEREO8,
    Stereo16 = AL_FORMAT_STEREO16,
};

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

    void setData(
        const SoundFormat format, const void* data, const int size, const int sampleRate
    ) noexcept;

    [[nodiscard]] ALuint get() const noexcept;

private:
    ALuint m_Buffer{};
};

struct SoundData {
    SoundFormat format;
    void* data;
    int size;
    int sampleRate;
};

[[nodiscard]] SoundData LoadSoundData(const std::string& path);
void ReleaseSoundData(SoundData& data) noexcept;

} // namespace atom::engine::systems::sound
