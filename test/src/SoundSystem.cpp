#include "SoundSystem.hpp"
#include <ranges/to.hpp>
#include "Events.hpp"
#include "Local.hpp"
#include "application/KeyboardInput.hpp"
#include "components/Transform.hpp"
#include "systems/sound/SoundBuffer.hpp"
#include "systems/sound/SoundContext.hpp"
#include "systems/sound/SoundFile.hpp"
#include "systems/sound/SoundListener.hpp"
#include "systems/sound/SoundSource.hpp"

using namespace components;
using namespace atom::engine::application;
using namespace atom::engine::systems::sound;

inline static SoundSource* de_source;
inline static SoundBuffer* de_buffer;
inline static SoundData de_data;

void PlayTheSound(GLFWwindow* window) { de_source->play(); }

void StartupTestSoundSystem(atom::ecs::command& command, atom::ecs::queryer& queryer) {
    auto& context = SoundContext::instance();
    context.makeCurrent();

    command.attach<SoundListener>(gLocalPlayer);

    de_source = new SoundSource;
    de_data   = LoadSoundData("resources/sounds/bricks.mp3");
    de_buffer = new SoundBuffer;
    de_buffer->setData(de_data.format, de_data.data, de_data.size, de_data.sampleRate);
    de_source->bufferData(de_buffer->get());

    auto& keyboard = KeyboardInput::instance();
    keyboard.setPressCallback(GLFW_KEY_Q, PlayTheSound);
}

void UpdateTestSoundSystem(
    atom::ecs::command& command, atom::ecs::queryer& queryer, float deltaTime
) {
    auto& context = SoundContext::instance();

    auto sources = queryer.query_all_of<Transform, SoundSource>();
    for (auto source : sources) {
        auto& src = queryer.get<SoundSource>(source);

        // update position
        auto& transform = queryer.get<Transform>(source);
        src.setPosition(transform.position);

        if (!src.isPlaying()) {
            src.play();
        }
    }

    auto& localListener  = queryer.get<SoundListener>(gLocalPlayer);
    auto& localTransform = queryer.get<Transform>(gLocalPlayer);
    localListener.setPosition(localTransform.position);
}

void ShutdownTestSoundSystem(atom::ecs::command& command, atom::ecs::queryer& queryer) {
    ReleaseSoundData(de_data);
    delete de_buffer;
    delete de_source;
}
