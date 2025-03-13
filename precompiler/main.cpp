#include <easylogging++.h>

#include <core/langdef.hpp>
#include <io/file.hpp>
#include <systems/render/ShaderProgram.hpp>

using namespace atom;
using namespace atom::engine::systems::render;

int main(int argc, char* argv[]) {
    // deal with input args
    if (argc == 1) {
        LOG(INFO) << "Please input argument!";
    }
    else if (argc == 2) {
        std::filesystem::path directory = argv[1];
        if (directory.is_absolute()) {}
        else if (directory.is_relative()) {}
    }
    else {
        LOG(INFO) << "Please input correct argument!";
    }

    // get shader files
    // vector<std::string> files;

    // precompile
}
