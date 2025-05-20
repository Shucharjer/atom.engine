#include "systems/sound/SoundFile.hpp"
#include <al.h>
#include <sndfile.h>
#include <pchs/logger.hpp>
#include <schedule.hpp>

namespace atom::engine::systems::sound {

SoundFile::SoundFile() = default;

SoundFile::SoundFile(const std::string& path) : m_Path(path) {}

SoundFile::SoundFile(std::string&& path) noexcept : m_Path(std::move(path)) {}

SoundFile::SoundFile(const SoundFile& that) : m_Path(that.m_Path) {}

SoundFile& SoundFile::operator=(const SoundFile& that) {
    if (this != &that) {
        m_Path = that.m_Path;
    }
    return *this;
}

SoundFile::SoundFile(SoundFile&& that) noexcept : m_Path(std::move(that.m_Path)) {}

SoundFile& SoundFile::operator=(SoundFile&& that) noexcept {
    if (this != &that) {
        m_Path = std::move(that.m_Path);
    }
    return *this;
}

SoundFile::~SoundFile() = default;

auto SoundFile::get_handle() const noexcept -> ecs::resource_handle { return m_Handle; }

void SoundFile::set_handle(const ecs::resource_handle handle) noexcept { m_Handle = handle; }

auto SoundFile::load() const -> std::shared_ptr<proxy_type> {
    auto proxy = std::make_shared<proxy_type>();
    SF_INFO info{};
    SNDFILE* file = sf_open(m_Path.c_str(), SFM_READ, &info);
    if (!file) {
        LOG(ERROR) << "[Sound] Failed to open sound file: " << m_Path;
        return nullptr;
    }

    LOG(INFO) << "[Sound] SF_INFO: { channels: " << info.channels << ", format: " << info.format
              << ", frames: " << info.frames << ", samplerate: " << info.samplerate
              << ", sections: " << info.sections << ", seekable: " << info.seekable << " }";

    std::pmr::vector<short> temp(info.frames * info.channels);
    auto size        = temp.size();
    sf_count_t count = sf_readf_short(file, temp.data(), temp.size());
    sf_close(file);
    using Format = SoundFormat;
    proxy->buffer.setData(
        info.channels == 1 ? Format::Stereo8 : Format::Stereo16,
        temp.data(),
        sizeof(short) * count * info.channels,
        info.samplerate
    );
    return proxy;
}

} // namespace atom::engine::systems::sound
