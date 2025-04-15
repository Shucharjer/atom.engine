#pragma once
#include <exception>
#include <filesystem>
#include <fstream>
#include <future>
#include <ios>
#include <string>
#include <easylogging++.h>
#include <core/langdef.hpp>
#include <schedule.hpp>
#include <thread/thread_pool.hpp>

namespace atom::engine::io {

// declrations

/*! @cond TURN_OFF_DOXYGEN */
namespace internal {
ATOM_FORCE_INLINE void CheckFolder(const std::filesystem::path& path);
ATOM_FORCE_INLINE void CheckFileFolder(const std::filesystem::path& path);
} // namespace internal
/*! @endcond */

[[nodiscard]] std::filesystem::path CurrentPath();
bool Exists(const std::filesystem::path& path);
void CheckFolder(const std::filesystem::path& path);
void CheckFileFolder(const std::filesystem::path& path);

template <typename Content>
bool WriteToFile(const std::filesystem::path& path, Content&& content);

template <typename Content>
std::future<bool> AsyncWriteToFile(const std::filesystem::path& path, Content&& content);

template <typename Ty>
bool WriteAsBinaryFile(const std::filesystem::path& path, const Ty* buf, std::streamsize len);

template <typename Ty>
std::future<bool> AsyncWriteAsBinaryFile(
    const std::filesystem::path& path, const Ty* buf, std::streamsize len
);

// definations

/**
 * @brief Write content to file at the path.
 *
 * If the path is not exist, it will create.
 */
template <typename Content>
bool WriteToFile(const std::filesystem::path& path, Content&& content) {
    internal::CheckFileFolder(path);

    std::ofstream stream(path);
    if (stream.is_open()) {
        try {
            stream << std::forward<Content>(content);
        }
        catch (std::exception& e) {
            LOG(ERROR) << "Exception occured when writing to file at {" << path
                       << "}: " << e.what();
        }
        return true;
    }
    else {
        LOG(ERROR) << "Failed to open file at {" << path << "}";
        return false;
    }
}

template <typename Content>
ATOM_FORCE_INLINE std::future<bool> AsyncWriteToFile(
    const std::filesystem::path& path, Content&& content
) {
    auto& thread_pool = ecs::scheduler::thread_pool();
    return thread_pool.enqueue(WriteToFile, path, std::forward<Content>(content));
}

/*! @cond TURN_OFF_DOXYGEN */
namespace internal {
bool WriteAsBinaryFile(const std::filesystem::path& path, const char* buf, std::streamsize len);
std::future<bool> AsyncWriteAsBinaryFile(
    const std::filesystem::path& path, const char* buf, std::streamsize len
);
} // namespace internal
/*! @endcond */

template <>
inline bool WriteAsBinaryFile(
    const std::filesystem::path& path, const char* buf, std::streamsize len
) {
    return internal::WriteAsBinaryFile(path, buf, len);
}

template <typename Ty>
bool WriteAsBinaryFile(const std::filesystem::path& path, const Ty* buf, std::streamsize len) {
    return internal::WriteAsBinaryFile(path, reinterpret_cast<const char*>(buf), len);
}

template <>
inline std::future<bool> AsyncWriteAsBinaryFile(
    const std::filesystem::path& path, const char* buf, std::streamsize len
) {
    return internal::AsyncWriteAsBinaryFile(path, buf, len);
}

template <typename Ty>
std::future<bool> AsyncWriteAsBinaryFile(
    const std::filesystem::path& path, const Ty* buf, std::streamsize len
) {
    return internal::AsyncWriteAsBinaryFile(path, reinterpret_cast<const char*>(buf), len);
}

/*! @cond TURN_OFF_DOXYGEN */
namespace internal {

ATOM_FORCE_INLINE void CheckFolder(const std::filesystem::path& path) {
    namespace fs = std::filesystem;

    if (!fs::exists(path)) {
        LOG(INFO) << "Directory Path {" << path << "} is not exist, now creating it";
        if (!fs::create_directories(path)) {
            LOG(ERROR) << "Failed to create the path!";
        }
    }
}

ATOM_FORCE_INLINE void CheckFolder(const std::string& path) {
    namespace fs     = std::filesystem;
    fs::path dirPath = { path };
    CheckFolder(dirPath);
}

ATOM_FORCE_INLINE void CheckFileFolder(const std::filesystem::path& path) {
    CheckFolder(path.parent_path());
}

} // namespace internal
/*! @endcond */

} // namespace atom::engine::io
