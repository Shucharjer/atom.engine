#include <queryer.hpp>
#include "components/Transform.hpp"
#include "systems/sound/SoundContext.hpp"
#include "systems/sound/SoundListener.hpp"
#include "systems/sound/SoundSource.hpp"
#include "systems/systems.hpp"

using namespace atom::ecs;
using namespace atom::engine::systems::sound;

using namespace components;

namespace atom::engine::systems {

void StartupSoundSystem(command& command, queryer& querier) {
    auto& context = SoundContext::instance();
}

void UpdateSoundSystem(command& command, queryer& querier, const float deltaTime) {
    auto sources   = querier.query_all_of<SoundSource, Transform>();
    auto listeners = querier.query_all_of<SoundListener, Transform>();

    for (auto source : sources) {
        auto& src       = querier.get<SoundSource>(source);
        const auto& tsf = querier.get<Transform>(source);

        src.setPosition(tsf.position);
    }

    for (auto listener : listeners) {
        const auto& lsn = querier.get<SoundListener>(listener);
        const auto& tsf = querier.get<Transform>(listener);

        lsn.setPosition(tsf.position);
    }
}

void ShutdownSoundSystem(command& command, queryer& querier) {
    auto sources   = querier.query_any_of<SoundSource>();
    auto listeners = querier.query_any_of<SoundListener>();

    for (auto source : sources) {
        //
        auto& com = querier.get<SoundSource>(source);
        com.stop();
    }

    for (auto listener : listeners) {
        //
        auto& com = querier.get<SoundListener>(listener);
    }
}

} // namespace atom::engine::systems
