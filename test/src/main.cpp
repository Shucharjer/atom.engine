#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btAlignedObjectArray.h>
#include <LinearMath/btTransform.h>
#include <btBulletDynamicsCommon.h>
#include <easylogging++.h>
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <winscard.h>
#include <command.hpp>
#include <core/langdef.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <queryer.hpp>
#include <world.hpp>
#include "KeyboardCallback.hpp"
#include "Local.hpp"
#include "application/KeyboardInput.hpp"
#include "application/Panel.hpp"
#include "application/Window.hpp"
#include "asset.hpp"
#include "io/file.hpp"
#include "pchs/math.hpp"
#include "platform/path.hpp"
#include "systems/render/BufferObject.hpp"
#include "systems/render/Camera.hpp"
#include "systems/render/Framebuffer.hpp"
#include "systems/render/Model.hpp"
#include "systems/render/ShaderProgram.hpp"
#include "systems/render/Transform.hpp"
#include "systems/render/VertexArrayObject.hpp"

using namespace atom::ecs;
using namespace atom::engine;
using namespace atom::engine::application;
using namespace atom::engine::platform;
using namespace atom::engine::systems::render;

ATOM_FORCE_INLINE static void CheckWindowConfig(WindowConfig& config) { config.name = "Aimer"; }

ATOM_FORCE_INLINE static WindowConfig CheckWindowConfig(WindowConfig&& config) {
    WindowConfig cfg{ std::move(config) };
    cfg.name = "Aimer";
    return config;
}

class ConsolePanel;

ConsolePanel* pConsole;

class ConsolePanel final : public Panel {
public:
    ConsolePanel() : Panel("Console", false, ImGuiWindowFlags_NoSavedSettings) {
        auto& inst = KeyboardInput::instance();
        inst.setPressCallback(GLFW_KEY_W, SwitchConsoleVisibility);
    }

    static void SwitchConsoleVisibility() {
        bool display = pConsole->getDisplay();
        pConsole->setDisplay(!display);
    }

private:
    void onBegin() override {}
    void layout(
        float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
    ) override {}
};

class MainPanel final : public Panel {
public:
    MainPanel()
        : Panel(
              "Main",
              true,
              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar |
                  ImGuiWindowFlags_NoResize
          ) {}

private:
    void onBegin() override {
        auto* mainViewport = ImGui::GetWindowViewport();
        ImGui::SetNextWindowViewport(mainViewport->ID);
        ImGui::SetNextWindowPos(mainViewport->Pos);
        ImGui::SetNextWindowSize(mainViewport->Size);
    }

    void layout(
        float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
    ) override {
        constexpr auto kStart = "Start";
        auto buttonSize       = ImGui::CalcTextSize(kStart);
        auto& style           = ImGui::GetStyle();
        buttonSize.x += style.FramePadding.x * 2;
        buttonSize.y += style.FramePadding.y * 2;

        auto context = ImGui::GetCurrentContext();

        auto windowPos = ImGui::GetWindowPos();
    }
};

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

auto& hub = hub::instance();
auto& lib = ::hub.library<Model>();
auto& tab = ::hub.table<Model>();
ShaderProgram* gShaderProgram;
ShaderProgram* gCopyShaderProgram;
ShaderProgram* gInverseShaderProgram;
ShaderProgram* gGreyShaderProgram;

enum class Postprocess : uint8_t {
    None,
    Inverse,
    Grey,
    _reserve
};

Postprocess gPostprocess = Postprocess::None;

static void ReloadShaderProgram() {
    delete gShaderProgram;
    gShaderProgram = new ShaderProgram(VertShaderPath, FragShaderPath);
}

static void SwitchPostProcessing() {
    auto postprocessing = static_cast<uint8_t>(gPostprocess) + 1;
    gPostprocess        = static_cast<Postprocess>(postprocessing);
    if (gPostprocess == Postprocess::_reserve) {
        gPostprocess = Postprocess::None;
    }
}

ColorAttachment* gColorAttachment;
Renderbuffer* gRenderbuffer;
Framebuffer* gFramebuffer;
VertexArrayObject* gScreenVAO;
VertexBufferObject* gScreenVBO;

const float gScreenVertices[] = {
    // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    -1.0f, 1.0f,  0.0f, 1.0f, //
    -1.0f, -1.0f, 0.0f, 0.0f, //
    1.0f,  -1.0f, 1.0f, 0.0f, //

    -1.0f, 1.0f,  0.0f, 1.0f, //
    1.0f,  -1.0f, 1.0f, 0.0f, //
    1.0f,  1.0f,  1.0f, 1.0f  //
};

static void StartupSys(command& command, queryer& queryer) {
    gLocalPlayer    = command.spawn<Transform, Camera>(Transform{}, Camera{});
    auto& camera    = queryer.get<Camera>(gLocalPlayer);
    camera.position = math::Vector3(0.0F, -4.0F, 13.0F);
    camera.rotate(0, glm::vec3(0.0F, 1.0F, 0.0F));
    gWorld  = queryer.current_world();
    gCamera = &camera;

    auto entity = command.spawn<Model, Transform>(NanosuitPath, Transform{});
    auto& model = queryer.get<Model>(entity);
    auto proxy  = model.load();
    auto handle = lib.install(std::move(proxy));
    model.set_handle(handle);
    tab.emplace(model.path(), handle);

    gShaderProgram        = new ShaderProgram(VertShaderPath, FragShaderPath);
    gCopyShaderProgram    = new ShaderProgram(SimplyCopyVertShaderPath, SimplyCopyFragShaderPath);
    gInverseShaderProgram = new ShaderProgram(SimplyCopyVertShaderPath, InverseFragShaderPath);
    gGreyShaderProgram    = new ShaderProgram(SimplyCopyVertShaderPath, GreyFragShaderPath);

    auto& localCamera    = queryer.get<Camera>(gLocalPlayer);
    localCamera.position = math::Vector3(0.0F, (2 << 2) + (2 << 2), (2 << 2) + (2 << 2));

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
}

static void UpdateSys(command& command, queryer& queryer, float deltaTime) {
    gDeltaTime      = deltaTime;
    auto& transform = queryer.get<Transform>(gLocalPlayer);
    auto& camera    = queryer.get<Camera>(gLocalPlayer);

    const auto view = camera.view();
    const auto proj = camera.proj();
    // gFramebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    gShaderProgram->use();
    gShaderProgram->setMat4("view", view);
    gShaderProgram->setMat4("proj", proj);
    gShaderProgram->setVec3("viewPos", camera.position);

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

    // NOTE: the coordination starts at the left top corner in opengl, but the screen coordnation
    // starts at the left buttom corner. If you want to get a readable picture, you should reverse
    // these bytes.

    // std::pmr::vector<std::byte> temp(1280 * 960 * 4);
    // glReadPixels(0, 0, 1280, 960, GL_RGBA, GL_UNSIGNED_BYTE, temp.data());
    // stbi_write_png("frame.png", 1280, 960, 4, temp.data(), 0);

    // gFramebuffer->unbind();

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
    // gScreenVAO->bind();
    // gColorAttachment->bind();
    // glDisable(GL_DEPTH_TEST);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // glEnable(GL_DEPTH_TEST);
}

static void ShutdownSys(command& command, queryer& queryer) {
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
}

static void StartupPhysicsSystem(command& command, queryer& queryer) {
    // command.add<btDefaultCollisionConfiguration>();
    // auto* collisionConfiguration = queryer.find<btDefaultCollisionConfiguration>();
    // command.add<btCollisionDispatcher>(collisionConfiguration);
    // auto* dispatcher                  = queryer.find<btCollisionDispatcher>();
    // btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    // command.add<btBroadphaseInterface>(broadphase);
    // command.add<btSequentialImpulseConstraintSolver>();
    // auto* solver = queryer.find<btSequentialImpulseConstraintSolver>();
    // command.add<btSequentialImpulseConstraintSolver>(solver);
    // command.add<btDiscreteDynamicsWorld>(dispatcher, broadphase, solver, collisionConfiguration);
    // auto* dynamicWorld = queryer.find<btDiscreteDynamicsWorld>();
    // dynamicWorld->setGravity(btVector3(0, -10, 0));
    // command.add<btAlignedObjectArray<btCollisionShape*>>(btAlignedObjectArray<btCollisionShape*>{});
}

static void UpdatePhysicsSystem(command& command, queryer& queryer, float deltaTime) {
    // auto* collisionConfiguration = queryer.find<btDefaultCollisionConfiguration>();
    // auto* dispatcher             = queryer.find<btCollisionDispatcher>();
    // auto* broadphase             = queryer.find<btBroadphaseInterface>();
    // auto* solver                 = queryer.find<btSequentialImpulseConstraintSolver>();
    // auto* dynamicWorld           = queryer.find<btDiscreteDynamicsWorld>();

    // LOG(DEBUG) << "*dynamicWorld: " << *dynamicWorld;

    // LOG(DEBUG) << "stepSimulation: " << deltaTime;
    // (dynamicWorld)->stepSimulation(deltaTime, 10);

    // LOG(DEBUG) << "numCollisionObjects: " << (dynamicWorld)->getNumCollisionObjects();
    // for (auto i = (dynamicWorld)->getNumCollisionObjects() - 1; i >= 0; --i) {
    //     btCollisionObject* obj = (dynamicWorld)->getCollisionObjectArray()[i];
    //     btRigidBody* body      = btRigidBody::upcast(obj);
    //     btTransform trans;
    //     if (body && body->getMotionState()) {
    //         body->getMotionState()->getWorldTransform(trans);
    //     }
    //     else {
    //         trans = obj->getWorldTransform();
    //     }
    // }
}

static void ShutdownPhysicsSystem(command& command, queryer& queryer) {
    auto* collisionConfiguration = queryer.find<btDefaultCollisionConfiguration>();
    auto* dispatcher             = queryer.find<btCollisionDispatcher>();
    auto* broadphase             = queryer.find<btBroadphaseInterface>();
    auto* solver                 = queryer.find<btSequentialImpulseConstraintSolver>();
    auto* dynamicWorld           = queryer.find<btDiscreteDynamicsWorld>();
    auto* collisionShapes        = queryer.find<btAlignedObjectArray<btCollisionShape*>>();

    for (auto i = (dynamicWorld)->getNumCollisionObjects() - 1; i >= 0; --i) {
        btCollisionObject* obj = (dynamicWorld)->getCollisionObjectArray()[i];
        btRigidBody* body      = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        (dynamicWorld)->removeCollisionObject(obj);
        delete obj;
    }

    for (auto i = 0; i < collisionShapes->size(); ++i) {
        btCollisionShape* shape = (*collisionShapes)[i];
        (*collisionShapes)[i]   = 0;
        delete shape;
    }

    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
    delete solver;
    delete dynamicWorld;
}

class ApplicationWindow : public Window {
public:
    ApplicationWindow(WindowConfig config) : Window(CheckWindowConfig(std::move(config))) {
        auto& factory = PanelFactory::instance();
        // emplace("Main", factory.make<MainPanel>());
        // pConsole = factory.make<ConsolePanel>();
        // emplace("Console", pConsole);
        auto world = createWorld("main world");
        world->add_startup(StartupSys, atom::ecs::late_main_thread);
        world->add_update(UpdateSys, atom::ecs::late_main_thread);
        world->add_shutdown(ShutdownSys, atom::ecs::late_main_thread);

        // world->add_startup(StartupPhysicsSystem, atom::ecs::late_main_thread);
        // world->add_update(UpdatePhysicsSystem, atom::ecs::late_main_thread);
        // world->add_shutdown(ShutdownPhysicsSystem, atom::ecs::late_main_thread);
    }

private:
};

const std::string kConfigPath =
    GetUserHomeDirectroy() + sep + ".atom" + sep + "proj" + sep + "aimer.json";

int main(int argc, char* argv[]) {
    auto config = LoadWindowConfig(kConfigPath);
    ApplicationWindow window{ std::move(config) };
    SetupWindow(window);
    SaveWindowConfig(kConfigPath, window.config());

    return 0;
}
