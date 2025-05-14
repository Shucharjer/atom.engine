#include "RenderSystem.hpp"
#include <cmath>
#include <cstdint>
#include <numbers>
#include <vector>
#include <command.hpp>
#include <ecs.hpp>
#include <queryer.hpp>
#include <ranges/to.hpp>
#include "KeyboardCallback.hpp"
#include "Local.hpp"
#include "MouseCallback.hpp"
#include "ResourcePath.hpp"
#include "application/KeyboardInput.hpp"
#include "application/MouseInput.hpp"
#include "application/Window.hpp"
#include "pchs/graphics.hpp"
#include "pchs/math.hpp"
#include "systems/render/BufferObject.hpp"
#include "systems/render/Camera.hpp"
#include "systems/render/Framebuffer.hpp"
#include "systems/render/Light.hpp"
#include "systems/render/Model.hpp"
#include "systems/render/ShaderProgram.hpp"
#include "systems/render/Transform.hpp"
#include "systems/render/Vertex.hpp"
#include "systems/render/VertexArrayObject.hpp"

using namespace atom::ecs;
using namespace atom::engine;
using namespace atom::engine::math;
using namespace atom::engine::application;
using namespace atom::engine::systems::render;

static ShaderProgram* gShaderProgram;
static ShaderProgram* gCopyShaderProgram;
static ShaderProgram* gInverseShaderProgram;
static ShaderProgram* gGreyShaderProgram;
static ColorAttachment* gColorAttachment;
static Renderbuffer* gRenderbuffer;
static Framebuffer* gFramebuffer;
static VertexArrayObject* gScreenVAO;
static VertexBufferObject* gScreenVBO;

const float gScreenVertices[] = {
    // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    -1.0f, 1.0f,  0.0f, 1.0f, //
    -1.0f, -1.0f, 0.0f, 0.0f, //
    1.0f,  -1.0f, 1.0f, 0.0f, //

    -1.0f, 1.0f,  0.0f, 1.0f, //
    1.0f,  -1.0f, 1.0f, 0.0f, //
    1.0f,  1.0f,  1.0f, 1.0f  //
};

const int maxLightCount = 15;

enum class Postprocess : uint8_t {
    None,
    Inverse,
    Grey,
    _reserve
};

static Postprocess gPostprocess = Postprocess::None;

static auto& hub = hub::instance();
static auto& lib = ::hub.library<Model>();
static auto& tab = ::hub.table<Model>();

static void ReloadShaderProgram(GLFWwindow*) {
    delete gShaderProgram;
    gShaderProgram = new ShaderProgram(VertShaderPath, FragShaderPath);
}

static void SwitchPostProcessing(GLFWwindow*) {
    auto postprocessing = static_cast<uint8_t>(gPostprocess) + 1;
    gPostprocess        = static_cast<Postprocess>(postprocessing);
    if (gPostprocess == Postprocess::_reserve) {
        gPostprocess = Postprocess::None;
    }
}

VertexArrayObject* gSphereVAO;
VertexBufferObject* gSphereVBO;
ElementBufferObject* gSphereEBO;
uint32_t indexCount = 0;

static void createSphere() {

    constexpr auto segments = 64;

    struct Vert {
        Vector3 position;
        Vector3 normal;
        Vector2 texCoords;
    };

    std::pmr::vector<Vert> vertices;

    for (auto i = 0; i < segments; ++i) {
        for (auto j = 0; j < segments; ++j) {
            float x    = (float)i / (float)segments;
            float y    = (float)j / (float)segments;
            float xPos = std::cos(x * 2.0f * std::numbers::pi) * std::sin(y * std::numbers::pi);
            float yPos = std::cos(y * std::numbers::pi);
            float zPos = std::sin(x * 2.0f * std::numbers::pi) * std::sin(y * std::numbers::pi);

            Vert vertex{};
            vertex.position  = { xPos, yPos, zPos };
            vertex.normal    = { xPos, yPos, zPos };
            vertex.texCoords = { x, y };
            vertices.emplace_back(vertex);
        }
    }

    std::pmr::vector<uint32_t> indices;
    for (auto i = 0; i < segments; ++i) {
        for (auto j = 0; j < segments; ++j) {
            auto fir = i * (segments + 1) + j;
            auto sec = fir + segments + 1;

            indices.emplace_back(fir);
            indices.emplace_back(sec);
            indices.emplace_back(fir + 1);
        }
    }

    gSphereVAO = new VertexArrayObject;
    gSphereVBO = new VertexBufferObject(vertices.size() * sizeof(float) * 8);
    gSphereEBO = new ElementBufferObject(indices.size() * sizeof(uint32_t));
    auto vao   = gSphereVAO->get();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal)
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords)
    );
    glEnableVertexAttribArray(2);

    gSphereVAO->bind();
    gSphereVBO->set(vertices.data());
}

static void renderSphere() {
    gSphereVAO->bind();
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, nullptr);
}

static void destroySphere() {
    delete gSphereEBO;
    delete gSphereVBO;
    delete gSphereVAO;
}

void StartupTestRenderSystem(command& command, queryer& queryer) {

    // initialize camera
    command.attach<Transform, Camera>(gLocalPlayer, Transform{}, Camera{});
    auto& camera = queryer.get<Camera>(gLocalPlayer);
    camera.rotate(0, glm::vec3(0.0F, 1.0F, 0.0F));
    camera.position = Vector3(0.0F, 9.0F, 38.0F);
    // camera.position = math::Vector3(0.0F, (2 << 2) + (2 << 2), (2 << 2) + (2 << 2));
    gCamera = &camera;

    // create entitys
    // create model
    auto entity = command.spawn<Model, Transform>(NanosuitPath, Transform{});
    auto& model = queryer.get<Model>(entity);
    auto proxy  = model.load();
    auto handle = lib.install(std::move(proxy));
    model.set_handle(handle);
    tab.emplace(model.path(), handle);
    // create lights
    auto dlEntity = command.spawn<DirectionalLight>();
    auto& dLight  = queryer.get<DirectionalLight>(dlEntity);
    dLight.color  = { 1.0F, 1.0F, 0.0F };

    auto pointEntity     = command.spawn<PointLight>();
    auto& pointLight     = queryer.get<PointLight>(pointEntity);
    pointLight.color     = { 1.0F, 0.0F, 0.0F };
    pointLight.position  = { 0.0F, 0.0F, 4.0F };
    pointLight.intensity = 3.0F;

    auto pointEntity2     = command.spawn<PointLight>();
    auto& pointLight2     = queryer.get<PointLight>(pointEntity2);
    pointLight2.position  = { 4.0F, 0.0F, 0.0F };
    pointLight2.color     = { 0.0F, 0.0F, 1.0F };
    pointLight2.intensity = 5.0F;

    // initialize shader
    gShaderProgram        = new ShaderProgram(VertShaderPath, FragShaderPath);
    gCopyShaderProgram    = new ShaderProgram(SimplyCopyVertShaderPath, SimplyCopyFragShaderPath);
    gInverseShaderProgram = new ShaderProgram(SimplyCopyVertShaderPath, InverseFragShaderPath);
    gGreyShaderProgram    = new ShaderProgram(SimplyCopyVertShaderPath, GreyFragShaderPath);

    // initialize keyboard and mouse input
    auto& keyboard = KeyboardInput::instance();
    keyboard.setPressCallback(GLFW_KEY_O, &ReloadShaderProgram);
    keyboard.setPressCallback(GLFW_KEY_K, &SwitchPostProcessing);
    // move camera
    keyboard.setPressCallback(GLFW_KEY_W, &MoveForward);
    keyboard.setPressCallback(GLFW_KEY_S, &MoveBackward);
    keyboard.setPressCallback(GLFW_KEY_A, &MoveLeft);
    keyboard.setPressCallback(GLFW_KEY_D, &MoveRight);
    keyboard.setPressCallback(GLFW_KEY_SPACE, &MoveJump);
    keyboard.setPressCallback(GLFW_KEY_LEFT_SHIFT, &MoveDown);
    // rotation camera
    keyboard.setPressCallback(GLFW_KEY_UP, &RotationUp);
    keyboard.setPressCallback(GLFW_KEY_DOWN, &RotationDown);
    keyboard.setPressCallback(GLFW_KEY_LEFT, &RotationLeft);
    keyboard.setPressCallback(GLFW_KEY_RIGHT, &RotationRight);
    keyboard.setPressCallback(GLFW_KEY_X, &RotationX);

    auto& mouse = MouseInput::instance();
    mouse.bind<&MouseCallback>();

    // initialize framebuffer
    gColorAttachment = new ColorAttachment(kDefaultWidth, kDefaultHeight);
    gRenderbuffer    = new Renderbuffer(kDefaultWidth, kDefaultHeight);
    gFramebuffer     = new Framebuffer();
    gFramebuffer->bind();
    gFramebuffer->attachColorAttachment(*gColorAttachment);
    gFramebuffer->attachRenderbuffer(*gRenderbuffer);
    if (auto status = gFramebuffer->getStatus(); status == GL_FRAMEBUFFER_COMPLETE) {
        LOG(DEBUG) << "framebuffer completed";
    }
    else {
        LOG(ERROR) << "framebuffer not completed: " << status;
    }
    gFramebuffer->unbind();

    gScreenVAO = new VertexArrayObject();
    gScreenVBO = new VertexBufferObject(sizeof(gScreenVertices));
    gScreenVBO->set(static_cast<const float*>(gScreenVertices));
    gScreenVBO->bind();
    gScreenVAO->addAttributeForVertices2D(0, *gScreenVBO);

    // createSphere();
}

void UpdateTestRenderSystem(
    atom::ecs::command& command, atom::ecs::queryer& queryer, float deltaTime
) {
    gDeltaTime      = deltaTime;
    auto& transform = queryer.get<Transform>(gLocalPlayer);
    auto& camera    = queryer.get<Camera>(gLocalPlayer);

    const auto view = camera.view();
    const auto proj = camera.proj();
    // gFramebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_STENCIL_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    gShaderProgram->use();
    gShaderProgram->setMat4("view", view);
    gShaderProgram->setMat4("proj", proj);
    gShaderProgram->setVec3("viewPos", camera.position);

    // apply light of first directional light
    auto lights = queryer.query_any_of<DirectionalLight>();
    if (!lights.empty()) {
        auto& lightComp = queryer.get<DirectionalLight>(lights.front());
        gShaderProgram->setVec3("dirLight.direction", lightComp.direction);
        gShaderProgram->setVec4(
            "dirLight.color", math::Vector4(lightComp.color, lightComp.intensity)
        );
        // rotation the directional light
        lightComp.rotate(90.0 * gDeltaTime, math::Vector3(0.0F, 1.0F, 0.0F));
    }

    // apply point lights
    auto pointLights       = queryer.query_any_of<PointLight>();
    auto pointLightVectors = atom::utils::ranges::to<std::vector>(pointLights);
    auto pointLightCount   = pointLightVectors.size();
    std::string tempStr    = "";
    pointLightCount        = pointLightCount > maxLightCount ? maxLightCount : pointLightCount;
    gShaderProgram->setInt("pointLightCount", pointLightCount);
    for (auto i = 0; i < pointLightCount; ++i) {
        auto light = queryer.get<PointLight>(pointLightVectors[i]);
        tempStr    = "pointLights[" + std::to_string(i) + "].position";
        gShaderProgram->setVec3(tempStr, light.position);
        tempStr = "pointLights[" + std::to_string(i) + "].color";
        gShaderProgram->setVec3(tempStr, light.color);
        tempStr = "pointLights[" + std::to_string(i) + "].intensity";
        gShaderProgram->setFloat(tempStr, light.intensity);
        tempStr = "pointLights[" + std::to_string(i) + "].constant";
        gShaderProgram->setFloat(tempStr, light.constant);
        tempStr = "pointLights[" + std::to_string(i) + "].linearValue";
        gShaderProgram->setFloat(tempStr, light.linear);
        tempStr = "pointLights[" + std::to_string(i) + "].quadratic";
        gShaderProgram->setFloat(tempStr, light.quadratic);
    }

    auto entities = queryer.query_all_of<Model, Transform>();
    for (const auto entity : entities) {
        auto& model            = queryer.get<Model>(entity);
        auto& transform        = queryer.get<Transform>(entity);
        math::Mat4 modelMatrix = transform.toMatrix();
        gShaderProgram->setMat4("model", modelMatrix);
        auto handle = model.get_handle();
        auto proxy  = lib.fetch(handle);
        model.draw(lib, *gShaderProgram);
    }

    // renderSphere();

    // NOTE: the coordination starts at the left top corner in opengl, but the screen coordnation
    // starts at the left buttom corner. If you want to get a readable picture, you should reverse
    // these bytes.

    // std::pmr::vector<std::byte> temp(1280 * 960 * 4);
    // glReadPixels(0, 0, 1280, 960, GL_RGBA, GL_UNSIGNED_BYTE, temp.data());
    // stbi_write_png("frame.png", 1280, 960, 4, temp.data(), 0);

    // glDisable(GL_BLEND);
    // gFramebuffer->unbind();

    // gScreenVAO->bind();
    // glActiveTexture(GL_TEXTURE0);
    // gColorAttachment->bind();
    // switch (gPostprocess) {
    //     using enum Postprocess;
    // case Inverse: {
    //     gInverseShaderProgram->use();
    //     gInverseShaderProgram->setInt("screenTexture", 0);
    // } break;
    // case None:
    // default: {
    //     gCopyShaderProgram->use();
    //     gInverseShaderProgram->setInt("screenTexture", 0);
    // } break;
    // }
    // glDisable(GL_DEPTH_TEST);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // glEnable(GL_DEPTH_TEST);
}

void ShutdownTestRenderSystem(command& command, queryer& queryer) {
    auto entities = queryer.query_any_of<Model>();
    for (const auto entity : entities) {
        auto& model = queryer.get<Model>(entity);
        auto handle = model.get_handle();
        LOG(INFO) << "unloading handle: [" << handle << "]";
        model.unload();
    }

    delete gShaderProgram;
    delete gColorAttachment;
    delete gRenderbuffer;
    delete gFramebuffer;
    delete gCopyShaderProgram;
    delete gInverseShaderProgram;
    delete gGreyShaderProgram;

    // destroySphere();
}
