#include "systems/sound/SoundBuffer.hpp"
#include <vector>
#include <al.h>
#include <easylogging++.h>
#include <sndfile.h>

namespace atom::engine::systems::sound {

SoundBuffer::SoundBuffer() noexcept { alGenBuffers(1, &m_Buffer); }

SoundBuffer::~SoundBuffer() noexcept {
    if (m_Buffer) [[likely]] {
        alDeleteBuffers(1, &m_Buffer);
    }
}

void SoundBuffer::setData(
    const SoundFormat format, const void* data, const int size, const int sampleRate
) noexcept {
    alBufferData(m_Buffer, static_cast<ALenum>(format), data, size, sampleRate);
}

ALuint SoundBuffer::get() const noexcept { return m_Buffer; }

SoundData LoadSoundData(const std::string& path) {
    SoundData data{};
    SF_INFO sf_info{};
    SNDFILE* file = sf_open(path.c_str(), SFM_READ, &sf_info);
    if (!file) {
        LOG(ERROR) << "[Sound] Failed to open sound file: " << path;
        return data;
    }

    data.format     = sf_info.channels == 1 ? SoundFormat::Stereo8 : SoundFormat::Stereo16;
    data.sampleRate = sf_info.samplerate;
    data.size       = sf_info.frames * sf_info.channels;
    data.data       = new short[data.size];
    sf_read_short(file, (short*)data.data, data.size);
    sf_close(file);
    return data;
}

void ReleaseSoundData(SoundData& data) noexcept {
    if (data.data) {
        delete[] static_cast<short*>(data.data);
        data.data = nullptr;
    }
}

} // namespace atom::engine::systems::sound
