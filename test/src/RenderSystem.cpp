#include "RenderSystem.hpp"
#include <cmath>
#include <cstdint>
#include <numbers>
#include <ranges>
#include <vector>
#include <command.hpp>
#include <ecs.hpp>
#include <glm/ext/matrix_transform.hpp>
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
#include "pchs/stbi.hpp"
#include "systems/render/BufferObject.hpp"
#include "systems/render/Camera.hpp"
#include "systems/render/CubeMap.hpp"
#include "systems/render/Framebuffer.hpp"
#include "systems/render/Light.hpp"
#include "systems/render/Model.hpp"
#include "systems/render/ShaderProgram.hpp"
#include "systems/render/Texture.hpp"
#include "systems/render/Transform.hpp"
#include "systems/render/Vertex.hpp"
#include "systems/render/VertexArrayObject.hpp"

using namespace atom;
using namespace atom::utils;
using namespace atom::ecs;
using namespace atom::engine;
using namespace atom::engine::math;
using namespace atom::engine::application;
using namespace atom::engine::systems::render;

static ShaderProgram* sShaderProgram;
static ShaderProgram* sCopyShaderProgram;
static ShaderProgram* sInverseShaderProgram;
static ShaderProgram* sGreyShaderProgram;
static ColorAttachment* sColorAttachment;
static Renderbuffer* sRenderbuffer;
static Framebuffer* sFramebuffer;
static VertexArrayObject* sScreenVAO;
static VertexBufferObject* sScreenVBO;

static VertexArrayObject* sSphereVAO;
static VertexBufferObject* sSphereVBO;
static ElementBufferObject* sSphereEBO;
static uint32_t sSphereIndicesCount = 0;
static ShaderProgram* sSphereShaderProgram;
static Material sSphereMaterial;
static Material sMetalSphereMaterial;
static Material sRubbleSphereMaterial;

static VertexArrayObject* sGroundVAO;
static VertexBufferObject* sGroundVBO;
static ShaderProgram* sGroundShaderProgram;
static Material sGroundMaterial;

static void SwitchMouseInput(GLFWwindow* window) { gEnableMouseInput = !gEnableMouseInput; }

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
    delete sShaderProgram;
    sShaderProgram = new ShaderProgram(VertShaderPath, BlinnPhongPath);
    delete sSphereShaderProgram;
    sSphereShaderProgram = new ShaderProgram(
        "resources/shaders/renderSphere.vert.glsl", "resources/shaders/renderSphere.frag.glsl"
    );
    delete sGroundShaderProgram;
    sGroundShaderProgram = new ShaderProgram(
        "resources/shaders/ground.vert.glsl", "resources/shaders/ground.frag.glsl"
    );
}

static void SwitchPostProcessing(GLFWwindow*) {
    auto postprocessing = static_cast<uint8_t>(gPostprocess) + 1;
    gPostprocess        = static_cast<Postprocess>(postprocessing);
    if (gPostprocess == Postprocess::_reserve) {
        gPostprocess = Postprocess::None;
    }
}

static void createSphere() {
    sSphereShaderProgram = new ShaderProgram(
        "resources/shaders/renderSphere.vert.glsl", "resources/shaders/renderSphere.frag.glsl"
    );

    constexpr auto segments = 64;

    struct Vert {
        Vector3 position;
        Vector3 normal;
        Vector2 texCoords;
    };

    std::pmr::vector<Vert> vertices;

    const auto kTwo = 2.0f;
    for (auto i = 0; i <= segments; ++i) {
        for (auto j = 0; j <= segments; ++j) {
            float x   = (float)i / (float)segments;
            float y   = (float)j / (float)segments;
            auto xPos = static_cast<float>(
                std::cos(x * kTwo * std::numbers::pi) * std::sin(y * std::numbers::pi)
            );
            auto yPos = static_cast<float>(std::cos(y * std::numbers::pi));
            auto zPos = static_cast<float>(
                std::sin(x * kTwo * std::numbers::pi) * std::sin(y * std::numbers::pi)
            );

            Vert vertex{};
            vertex.position  = { xPos, yPos, zPos };
            vertex.normal    = { xPos, yPos, zPos };
            vertex.texCoords = { x, y };
            vertices.emplace_back(vertex);
        }
    }

    std::pmr::vector<uint32_t> indices;
    bool flag = false;
    for (auto y = 0; y < segments; ++y) {
        if (!flag) {
            for (auto x = 0; x <= segments; ++x) {
                indices.emplace_back(y * (segments + 1) + x);
                indices.emplace_back((y + 1) * (segments + 1) + x);
            }
        }
        else {
            for (auto x = segments; x >= 0; --x) {
                indices.emplace_back((y + 1) * (segments + 1) + x);
                indices.emplace_back(y * (segments + 1) + x);
            }
        }
        flag = !flag;
    }

    sSphereIndicesCount = indices.size();

    sSphereVAO = new VertexArrayObject;
    sSphereVBO = new VertexBufferObject(vertices.size() * sizeof(Vert));
    sSphereEBO = new ElementBufferObject(indices.size() * sizeof(uint32_t));
    sSphereVAO->bind();
    sSphereVBO->set(vertices.data());
    sSphereVBO->bind();
    sSphereEBO->set(indices.data());
    sSphereEBO->bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, texCoords)
    );
    glEnableVertexAttribArray(2);

    auto& hub     = hub::instance();
    auto& library = hub.library<Texture>();

    const std::string matTexFolder   = "resources/materials/wood_planks_grey_1k.gltf/textures/";
    sSphereMaterial.baseColorTexture = Texture(matTexFolder + "wood_planks_grey_diff_1k.jpg");
    sSphereMaterial.roughnessTexture = Texture(matTexFolder + "wood_planks_grey_arm_1k.jpg");
    sSphereMaterial.metallicTexture  = Texture(matTexFolder + "wood_planks_grey_arm_1k.jpg");

    resource_handle handle{};
    auto baseColorTexture = sSphereMaterial.baseColorTexture.load();
    handle                = library.install(baseColorTexture);
    sSphereMaterial.baseColorTexture.set_handle(handle);

    auto roughnessTexture = sSphereMaterial.roughnessTexture.load();
    handle                = library.install(roughnessTexture);
    sSphereMaterial.roughnessTexture.set_handle(handle);

    auto metallicTexture = sSphereMaterial.metallicTexture.load();
    handle               = library.install(metallicTexture);
    sSphereMaterial.metallicTexture.set_handle(handle);

    const std::string metalMatTexFolder = "resources/materials/blue_metal_plate_1k.gltf/textures/";
    sMetalSphereMaterial.baseColorTexture =
        Texture(metalMatTexFolder + "blue_metal_plate_diff_1k.jpg");
    sMetalSphereMaterial.roughnessTexture =
        Texture(metalMatTexFolder + "blue_metal_plate_arm_1k.jpg");
    sMetalSphereMaterial.metallicTexture =
        Texture(metalMatTexFolder + "blue_metal_plate_arm_1k.jpg");

    baseColorTexture = sMetalSphereMaterial.baseColorTexture.load();
    handle           = library.install(baseColorTexture);
    sMetalSphereMaterial.baseColorTexture.set_handle(handle);

    roughnessTexture = sMetalSphereMaterial.roughnessTexture.load();
    handle           = library.install(roughnessTexture);
    sMetalSphereMaterial.roughnessTexture.set_handle(handle);

    metallicTexture = sMetalSphereMaterial.metallicTexture.load();
    handle          = library.install(metallicTexture);
    sMetalSphereMaterial.metallicTexture.set_handle(handle);

    const std::string rubMatTexFolder      = "resources/materials/rubble_1k.gltf/textures/";
    sRubbleSphereMaterial.baseColorTexture = Texture(rubMatTexFolder + "rubble_diff_1k.jpg");
    sRubbleSphereMaterial.roughnessTexture = Texture(rubMatTexFolder + "rubble_arm_1k.jpg");
    sRubbleSphereMaterial.metallicTexture  = Texture(rubMatTexFolder + "rubble_arm_1k.jpg");

    baseColorTexture = sRubbleSphereMaterial.baseColorTexture.load();
    handle           = library.install(baseColorTexture);
    sRubbleSphereMaterial.baseColorTexture.set_handle(handle);

    roughnessTexture = sRubbleSphereMaterial.roughnessTexture.load();
    handle           = library.install(roughnessTexture);
    sRubbleSphereMaterial.roughnessTexture.set_handle(handle);

    metallicTexture = sRubbleSphereMaterial.metallicTexture.load();
    handle          = library.install(metallicTexture);
    sRubbleSphereMaterial.metallicTexture.set_handle(handle);
}

static void renderSphere() {
    glDrawElements(
        GL_TRIANGLE_STRIP, static_cast<GLsizei>(sSphereIndicesCount), GL_UNSIGNED_INT, nullptr
    );
}

static void destroySphere() {
    delete sSphereShaderProgram;
    delete sSphereEBO;
    delete sSphereVBO;
    delete sSphereVAO;
    auto& hub     = hub::instance();
    auto& library = hub.library<Texture>();
    resource_handle handle{};
    handle = sSphereMaterial.baseColorTexture.get_handle();
    library.uninstall(handle);
    handle = sSphereMaterial.roughnessTexture.get_handle();
    library.uninstall(handle);
    handle = sSphereMaterial.metallicTexture.get_handle();
    library.uninstall(handle);
    handle = sMetalSphereMaterial.baseColorTexture.get_handle();
    library.uninstall(handle);
    handle = sMetalSphereMaterial.roughnessTexture.get_handle();
    library.uninstall(handle);
    handle = sMetalSphereMaterial.metallicTexture.get_handle();
    library.uninstall(handle);
    handle = sRubbleSphereMaterial.baseColorTexture.get_handle();
    library.uninstall(handle);
    handle = sRubbleSphereMaterial.roughnessTexture.get_handle();
    library.uninstall(handle);
    handle = sRubbleSphereMaterial.metallicTexture.get_handle();
    library.uninstall(handle);
}

static void createGround() {
    sGroundShaderProgram = new ShaderProgram(
        "resources/shaders/ground.vert.glsl", "resources/shaders/ground.frag.glsl"
    );

    struct Vert {
        math::Vector3 position;
        math::Vector3 normal;
        math::Vector2 texCoords;
    };

    const Vert groundVertices[] = {
        Vert{  { -1000.f, 0.f, 1000.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } }, // 左上
        Vert{  { 1000.f, 0.f, -1000.f }, { 0.f, 1.f, 0.f }, { 1.f, 0.f } }, // 右下
        Vert{ { -1000.f, 0.f, -1000.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } }, // 左下

        Vert{  { -1000.f, 0.f, 1000.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } }, // 左上
        Vert{   { 1000.f, 0.f, 1000.f }, { 0.f, 1.f, 0.f }, { 1.f, 1.f } }, // 右上
        Vert{  { 1000.f, 0.f, -1000.f }, { 0.f, 1.f, 0.f }, { 1.f, 0.f } }  // 右下
    };

    sGroundVAO = new VertexArrayObject;
    sGroundVBO = new VertexBufferObject(sizeof(groundVertices));
    sGroundVAO->bind();
    sGroundVBO->set(static_cast<const Vert*>(groundVertices));
    sGroundVBO->bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, texCoords)
    );
    glEnableVertexAttribArray(2);

    auto& hub     = hub::instance();
    auto& library = hub.library<Texture>();

    const std::string matTexFolder   = "resources/materials/laminate_floor_02_4k.gltf/textures/";
    sGroundMaterial.baseColorTexture = Texture(matTexFolder + "laminate_floor_02_diff_4k.jpg");
    sGroundMaterial.roughnessTexture = Texture(matTexFolder + "laminate_floor_02_rough_4k.jpg");
    sGroundMaterial.normalTexture    = Texture(matTexFolder + "laminate_floor_02_nor_gl_4k.jpg");

    resource_handle handle{};
    auto baseColorTexture = sGroundMaterial.baseColorTexture.load();
    handle                = library.install(baseColorTexture);
    sGroundMaterial.baseColorTexture.set_handle(handle);

    auto roughnessTexture = sGroundMaterial.roughnessTexture.load();
    handle                = library.install(roughnessTexture);
    sGroundMaterial.roughnessTexture.set_handle(handle);

    auto normalTexture = sGroundMaterial.normalTexture.load();
    handle             = library.install(normalTexture);
    sGroundMaterial.normalTexture.set_handle(handle);
}

static void renderGround() { glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(6)); }

static void destroyGround() {
    auto& hub     = hub::instance();
    auto& library = hub.library<Texture>();

    delete sGroundShaderProgram;
    delete sGroundVAO;
    delete sGroundVBO;

    resource_handle handle{};
    handle = sGroundMaterial.baseColorTexture.get_handle();
    library.uninstall(handle);
    handle = sGroundMaterial.roughnessTexture.get_handle();
    library.uninstall(handle);
    handle = sGroundMaterial.normalTexture.get_handle();
    library.uninstall(handle);
}

static CubeMap* pCubeMap;

void StartupTestRenderSystem(command& command, queryer& queryer) {

    // initialize camera
    command.attach<Transform, Camera>(gLocalPlayer, Transform{}, Camera{});
    auto& camera = queryer.get<Camera>(gLocalPlayer);
    camera.rotate(0, glm::vec3(0.0F, 1.0F, 0.0F));
    camera.position = Vector3(0.0F, 9.0F, 38.0F);
    gCamera         = &camera;

    // create entitys
    // create model
    auto entity = command.spawn<Model, Transform>(NanosuitPath, Transform{});
    auto& model = queryer.get<Model>(entity);
    auto proxy  = model.load();
    auto handle = lib.install(std::move(proxy));
    model.set_handle(handle);
    tab.emplace(model.path(), handle);
    // create lights
    auto dlEntity    = command.spawn<DirectionalLight>();
    auto& dLight     = queryer.get<DirectionalLight>(dlEntity);
    dLight.color     = { 1.0F, 1.0F, 0.0F };
    dLight.direction = { 0.8f, -0.6f, 0.f };

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
    sShaderProgram        = new ShaderProgram(VertShaderPath, BlinnPhongPath);
    sCopyShaderProgram    = new ShaderProgram(SimplyCopyVertShaderPath, SimplyCopyFragShaderPath);
    sInverseShaderProgram = new ShaderProgram(SimplyCopyVertShaderPath, InverseFragShaderPath);
    sGreyShaderProgram    = new ShaderProgram(SimplyCopyVertShaderPath, GreyFragShaderPath);

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

    keyboard.setPressCallback(GLFW_KEY_ESCAPE, &SwitchMouseInput);

    auto& mouse = MouseInput::instance();
    mouse.bind<&MouseCallback>();

    // initialize framebuffer
    sColorAttachment = new ColorAttachment(kDefaultWidth, kDefaultHeight);
    sRenderbuffer    = new Renderbuffer(kDefaultWidth, kDefaultHeight);
    sFramebuffer     = new Framebuffer();
    sFramebuffer->bind();
    sFramebuffer->attachColorAttachment(*sColorAttachment);
    sFramebuffer->attachRenderbuffer(*sRenderbuffer);
    if (auto status = sFramebuffer->getStatus(); status == GL_FRAMEBUFFER_COMPLETE) {
        LOG(DEBUG) << "framebuffer completed";
    }
    else {
        LOG(ERROR) << "framebuffer not completed: " << status;
    }
    sFramebuffer->unbind();

    sScreenVAO = new VertexArrayObject();
    sScreenVBO = new VertexBufferObject(sizeof(gScreenVertices));
    sScreenVBO->set(static_cast<const float*>(gScreenVertices));
    sScreenVBO->bind();
    sScreenVAO->addAttributeForVertices2D(0, *sScreenVBO);

    createGround();
    createSphere();

    pCubeMap = new CubeMap();
}

template <std::ranges::range Rng>
requires std::same_as<std::ranges::range_value_t<Rng>, DirectionalLight*> ||
         std::same_as<std::ranges::range_value_t<Rng>, const DirectionalLight*>
void ApplyDirLights(ShaderProgram& shaderProgram, const Rng& dirLights) {
    if (!dirLights.empty()) {
        auto light = dirLights.front();
        shaderProgram.setVec3("dirLight.direction", light->direction);
        shaderProgram.setVec4("dirLight.color", math::Vector4(light->color, light->intensity));
    }
}

template <std::ranges::range Rng>
requires std::same_as<std::ranges::range_value_t<Rng>, PointLight*> ||
         std::same_as<std::ranges::range_value_t<Rng>, const PointLight*>
void ApplyPointLights(ShaderProgram& shaderProgram, const Rng& pointLights) {
    auto count           = pointLights.size();
    auto pointLightCount = count > maxLightCount ? maxLightCount : count;
    shaderProgram.setInt("pointLightCount", pointLightCount);
    const char* prefix = "pointLights[";
    std::string tempStr(magic_32, 0);
    for (auto i = 0; i < pointLightCount; ++i) {
        auto light = pointLights[i];
        tempStr    = prefix + std::to_string(i) + "].position";
        shaderProgram.setVec3(tempStr, light->position);
        tempStr = prefix + std::to_string(i) + "].color";
        shaderProgram.setVec3(tempStr, light->color);
        tempStr = prefix + std::to_string(i) + "].intensity";
        shaderProgram.setFloat(tempStr, light->intensity);
        tempStr = prefix + std::to_string(i) + "].constant";
        shaderProgram.setFloat(tempStr, light->constant);
        tempStr = prefix + std::to_string(i) + "].linearValue";
        shaderProgram.setFloat(tempStr, light->linear);
        tempStr = prefix + std::to_string(i) + "].quadratic";
        shaderProgram.setFloat(tempStr, light->quadratic);
    }
}

static void renderScene() {}

void UpdateTestRenderSystem(
    atom::ecs::command& command, atom::ecs::queryer& queryer, float deltaTime
) {
    gDeltaTime      = deltaTime;
    auto& transform = queryer.get<Transform>(gLocalPlayer);
    auto& camera    = queryer.get<Camera>(gLocalPlayer);

    const auto view = camera.view();
    const auto proj = camera.proj();
    sFramebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    sShaderProgram->use();
    sShaderProgram->setMat4("view", view);
    sShaderProgram->setMat4("proj", proj);
    sShaderProgram->setVec3("viewPos", camera.position);

    // apply light of first directional light
    auto dirLights = queryer.query_any_of<DirectionalLight>() |
                     std::views::transform([&queryer](const auto& light) {
                         return &queryer.get<DirectionalLight>(light);
                     }) |
                     ranges::to<std::vector>();
    {
        auto light = dirLights.front();
        // rotation the directional light
        light->rotate(90.0 * gDeltaTime, math::Vector3(0.0F, 1.0F, 0.0F));
    }
    ApplyDirLights(*sShaderProgram, dirLights);

    // apply point lights
    auto pointLights = queryer.query_any_of<PointLight>() |
                       std::views::transform([&queryer](auto entity) {
                           return &queryer.get<PointLight>(entity);
                       }) |
                       ranges::to<std::vector>();
    ApplyPointLights(*sShaderProgram, pointLights);

    auto entities = queryer.query_all_of<Model, Transform>() | ranges::to<std::vector>();
    for (const auto entity : entities) {
        auto& model            = queryer.get<Model>(entity);
        auto& transform        = queryer.get<Transform>(entity);
        math::Mat4 modelMatrix = transform.toMatrix();
        sShaderProgram->setMat4("model", modelMatrix);
        auto handle = model.get_handle();
        auto proxy  = lib.fetch(handle);
        model.draw(lib, *sShaderProgram);
    }

    sSphereShaderProgram->use();
    sSphereShaderProgram->setMat4("proj", proj);
    sSphereShaderProgram->setMat4("view", view);
    sSphereShaderProgram->setVec3("viewPos", camera.position);

    ApplyDirLights(*sSphereShaderProgram, dirLights);
    ApplyPointLights(*sSphereShaderProgram, pointLights);
    sSphereVAO->bind();
    const Mat4 sphereModel = glm::translate(Mat4(1.0), Vector3(0, 8, 10));
    sSphereShaderProgram->setMat4("model", sphereModel);
    sSphereMaterial.apply(*sSphereShaderProgram);
    renderSphere();

    const Mat4 metalSphereModel = glm::translate(Mat4(1.0), Vector3(0, 10, 10));
    sSphereShaderProgram->setMat4("model", metalSphereModel);
    sMetalSphereMaterial.apply(*sSphereShaderProgram);
    renderSphere();

    auto interactives = queryer.query_all_of<Interactivable, Transform>();
    for (auto interactive : interactives) {
        const auto& transform = queryer.get<Transform>(interactive);
        const auto model      = transform.toMatrix();
        sSphereShaderProgram->setMat4("model", model);
        sRubbleSphereMaterial.apply(*sSphereShaderProgram);
        renderSphere();
    }

    sGroundShaderProgram->use();
    sGroundShaderProgram->setMat4("proj", proj);
    sGroundShaderProgram->setMat4("view", view);
    sGroundShaderProgram->setVec3("viewPos", camera.position);
    ApplyDirLights(*sGroundShaderProgram, dirLights);
    ApplyPointLights(*sGroundShaderProgram, pointLights);
    sGroundVAO->bind();
    sGroundMaterial.apply(*sGroundShaderProgram);
    renderGround();

    // NOTE: the coordination starts at the left top corner in opengl, but the screen coordnation
    // starts at the left buttom corner. If you want to get a readable picture, you should reverse
    // these bytes.

    // std::pmr::vector<std::byte> temp(1280 * 960 * 4);
    // glReadPixels(0, 0, 1280, 960, GL_RGBA, GL_UNSIGNED_BYTE, temp.data());
    // stbi_write_png("frame.png", 1280, 960, 4, temp.data(), 0);

    // glDisable(GL_BLEND);
    sFramebuffer->unbind();

    sScreenVAO->bind();
    glActiveTexture(GL_TEXTURE0);
    sColorAttachment->bind();
    switch (gPostprocess) {
        using enum Postprocess;
    case Inverse: {
        sInverseShaderProgram->use();
        sInverseShaderProgram->setInt("screenTexture", 0);
    } break;
    case None:
    default: {
        sCopyShaderProgram->use();
        sInverseShaderProgram->setInt("screenTexture", 0);
    } break;
    }
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

void ShutdownTestRenderSystem(command& command, queryer& queryer) {
    auto entities = queryer.query_any_of<Model>();
    for (const auto entity : entities) {
        auto& model = queryer.get<Model>(entity);
        auto handle = model.get_handle();
        LOG(INFO) << "unloading handle: [" << handle << "]";
        model.unload();
    }

    delete sShaderProgram;
    delete sColorAttachment;
    delete sRenderbuffer;
    delete sFramebuffer;
    delete sCopyShaderProgram;
    delete sInverseShaderProgram;
    delete sGreyShaderProgram;

    destroySphere();
    destroyGround();
}
