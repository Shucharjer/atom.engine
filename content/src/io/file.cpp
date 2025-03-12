#include "io/file.hpp"
#include <easylogging++.h>
#include <schedule.hpp>
#include <filesystem>

namespace fs = std::filesystem;

namespace atom::engine::io {

/*! @cond TURN_OFF_DOXYGEN */
namespace internal {

bool WriteAsBinaryFile(const std::filesystem::path &path, const char *buf, std::streamsize len) {
    CheckFileFolder(path);

    std::ofstream stream(path, std::ios::out | std::ios::binary);
    if (stream.is_open()) [[likely]] {
        stream.write(buf, len);
        stream.close();
        return true;
    }
    else [[unlikely]] {
        LOG(ERROR) << "";
        return false;
    }
}

std::future<bool> AsyncWriteAsBinaryFile(const std::filesystem::path& path, const char* buf, std::streamsize len) {
    auto& thread_pool = ecs::scheduler::thread_pool();
    return thread_pool.enqueue(WriteAsBinaryFile, path, buf, len);
}

}
/*! @endcond */

std::filesystem::path CurrentPath() {
    return fs::current_path();
}

bool Exists(const std::filesystem::path& path) {
    namespace fs = std::filesystem;
    return fs::exists(path);
}

void CheckFolder(const std::filesystem::path &path) {
    internal::CheckFolder(path);
}

void CheckFileFolder(const std::filesystem::path& path) {
    internal::CheckFileFolder(path);
}

}