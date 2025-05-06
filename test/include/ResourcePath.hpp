#pragma once
#include "io/file.hpp"
#include "platform/path.hpp"

using namespace atom::engine::platform;

const auto CurrentPath     = atom::engine::io::CurrentPath();
const auto ResourceFolder  = CurrentPath / "resources";
const auto ModelsFolder    = ResourceFolder / "models";
const auto LoraPath        = ModelsFolder.string() + sep + "诺菈_by_幻塔" + sep + "诺菈.pmx";
const auto MikuPath        = ModelsFolder.string() + sep + "miku" + sep + "model.pmx";
const auto NanosuitPath    = ModelsFolder.string() + sep + "nanosuit" + sep + "nanosuit.obj";
const auto CubePath        = ModelsFolder.string() + sep + "Cube.fbx";
const auto IllustriousPath = ModelsFolder.string() + sep + "illustrious" + sep + "illustrious.pmx";

const auto ShaderPath               = ResourceFolder / "shaders";
const auto VertShaderPath           = ShaderPath / "example.vert.glsl";
const auto FragShaderPath           = ShaderPath / "example.frag.glsl";
const auto PostprocessShaderPath    = ShaderPath / "post";
const auto SimplyCopyVertShaderPath = PostprocessShaderPath / "copy.vert.glsl";
const auto SimplyCopyFragShaderPath = PostprocessShaderPath / "copy.frag.glsl";
const auto InverseFragShaderPath    = PostprocessShaderPath / "inverse.frag.glsl";
const auto GreyFragShaderPath       = PostprocessShaderPath / "grey.frag.glsl";
