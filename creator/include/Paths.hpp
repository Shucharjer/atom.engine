#pragma once
#include <string>
#include "platform/path.hpp"

namespace atom::creator {

const static inline std::string kHomeDirectory = engine::platform::GetUserHomeDirectroy();

const static inline std::string kAtomDirectory = kHomeDirectory + engine::platform::sep + ".atom";

const static inline std::string kAtomLocalizationDirectory =
    kAtomDirectory + engine::platform::sep + "localization";

const static inline std::string kAtomLocalizationFile = kAtomLocalizationDirectory + ".json";

const static inline std::string kAtomProjectsDirectory =
    kAtomDirectory + engine::platform::sep + "projects";

} // namespace atom::creator
