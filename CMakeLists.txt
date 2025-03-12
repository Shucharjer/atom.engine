cmake_minimum_required(VERSION 3.19)

if (NOT CMAKE_CXX_STANDARD)
set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_NAMESPACE "Atom::")

project(Engine
    DESCRIPTION "A game engine based on atom.ecs"
    LANGUAGES C CXX
)

if (NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(Engine_DOXY_DIR ${Engine_SOURCE_DIR}/content)

file(GLOB Engine_SRC_FILES
    ${Engine_SOURCE_DIR}/content/src/*.cpp
    ${Engine_SOURCE_DIR}/content/src/*/*.cpp
    ${Engine_SOURCE_DIR}/content/src/*/*/*.cpp
    ${Engine_SOURCE_DIR}/thirdparty/*.cpp
)

include_directories(content/include)

add_library(Engine STATIC ${Engine_SRC_FILES})
add_library(Atom::Engine ALIAS Engine)

target_include_directories(Engine PUBLIC
    $<BUILD_INTERFACE:${Engine_SOURCE_DIR}/content/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Utils

find_package(Utils CONFIG REQUIRED)
if (NOT Utils_FOUND)
    message(FATAL_ERROR "atom.utils not found!")
endif()
target_link_libraries(Engine PUBLIC Atom::Utils)

# Ecs

find_package(Ecs CONFIG REQUIRED)
if (NOT Ecs_FOUND)
    message(FATAL_ERROR "atom.ecs not found!")
endif()
target_link_libraries(Engine PUBLIC Atom::Ecs)

# thridparty

find_package(imgui CONFIG REQUIRED)
find_package(lunasvg CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(Vulkan REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(tinyfiledialogs CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(easyloggingpp easyloggingpp REQUIRED IMPORTED_TARGET)
find_package(assimp CONFIG REQUIRED)

set(THIRDPARTY_DIR "${Engine_SOURCE_DIR}/thirdparty")

target_include_directories(Engine PUBLIC ${OPENGL_INCLUDE_DIR})

target_link_libraries(Engine PUBLIC
    glfw 
    glad::glad
    glm::glm
    OpenGL::GL
    ${OPENGL_LIBRARIES}
    Vulkan::Vulkan
    imgui::imgui
    Freetype::Freetype
    lunasvg::lunasvg
    OpenAL::OpenAL
    tinyfiledialogs::tinyfiledialogs
    ${LUA_LIBRARIES}
    sol2
    PkgConfig::easyloggingpp
    assimp::assimp
    ${BULLET_LIBRARIES}
    Bullet3Dynamics
)

add_compile_definitions(AUTO_INITIALIZE_EASYLOGGINGPP)
add_compile_definitions(ELPP_THREAD_SAFE)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.16")
    target_precompile_headers(Engine PRIVATE
        ${Engine_SOURCE_DIR}/content/include/pch.hpp
    )
endif()

include(JoinPaths)

set(Engine_PKGCONFIG ${CMAKE_CURRENT_BINARY_DIR}/engine.pc)

join_paths(Engine_PKGCONFIG_INCLUDEDIR "\${prefix}" "CMAKE_INSTALL_INCLUDEDIR")

configure_file(
    ${Engine_SOURCE_DIR}/cmake/in/engine.pc.in
    ${Engine_PKGCONFIG}
    @ONLY
)

install(
    FILES ${Engine_PKGCONFIG}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

include(CMakePackageConfigHelpers)

install(
    TARGETS Engine
    EXPORT EngineTargets
    LIBRARY DESTINATION ${INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${INSTALL_LIBDIR}
    RUNTIME DESTINATION ${INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_package_config_file(
    ${Engine_SOURCE_DIR}/cmake/in/EngineConfig.cmake.in
    EngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/Engine/cmake
)

export(
    EXPORT EngineTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/EngineTargets.cmake
    NAMESPACE Atom::
)

install(
    EXPORT EngineTargets
    FILE EngineTargets.cmake
    NAMESPACE Atom::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Engine/cmake
)

install(
    FILES ${PROJECT_BINARY_DIR}/EngineConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Engine/cmake
)

install(
    DIRECTORY
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)

export(PACKAGE Engine)

option(BUILD_ENGINE_DOCS OFF)
if (BUILD_ENGINE_DOCS)
    find_package(Doxygen REQUIRED dot)
    if (DOXYGEN_FOUND)
        include(FetchContent)

        FetchContent_Declare(
            doxygen-awesome-css
            GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css
            GIT_TAG main
            GIT_SHALLOW 1
        )

        FetchContent_GetProperties(doxygen-awesome-css)

        if(NOT doxygen-awesome-css_POPULATED)
            FetchContent_Populate(doxygen-awesome-css)
            set(doxygen-awesome-css_INCLUDE_DIR ${doxygen-awesome-css_SOURCE_DIR})
        endif()

        set(DOXY_SOURCE_DIRECTORY ${Engine_DOXY_DIR})
        set(DOXY_OUTPUT_DIRECTROY ${CMAKE_CURRENT_BINARY_DIR}/docs)
        set(DOXY_CSS_DIRECTROY ${doxygen-awesome-css_INCLUDE_DIR})

        configure_file(doxy.in doxy.cfg @ONLY)

        add_custom_target(
            documents
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxy.cfg
            WORKING_DIRECTORY ${Utils_SOURCE_DIR}
            VERBATIM
        )

    else()
        message(FATAL_ERROR "Required to build documents but doxygen is not found!")
    endif()
endif()

option(ATOM_BUILD_CREATOR OFF)
set(ATOM_BUILD_CREATOR ON)

if (ATOM_BUILD_CREATOR)
    file(GLOB CREATOR_SRC_FILES
        ${Engine_SOURCE_DIR}/creator/src/*.cpp
        ${Engine_SOURCE_DIR}/creator/src/*/*.cpp
    )

    add_executable(creator ${CREATOR_SRC_FILES})

    target_include_directories(creator PRIVATE 
        $<BUILD_INTERFACE:${Engine_SOURCE_DIR}/creator//include>
    )

    target_link_libraries(creator PRIVATE Atom::Engine)
endif()

option(BUILD_Engine_TEST OFF)
set(BUILD_Engine_TEST ON)

if (BUILD_Engine_TEST)

    file(GLOB TEST_SRC_FILES
        ${Engine_SOURCE_DIR}/test/*.cpp
    )

    add_executable(test ${TEST_SRC_FILES})

    target_link_libraries(test PRIVATE
        Atom::Engine
    )

endif()

message(WARNING "----------------------------------------------------------------\nYou might meet error when compiling sol2, see https://github.com/ThePhD/sol2/issues/1657#event-16282465360\nPlease change the content around line 2194 to as described in the link")