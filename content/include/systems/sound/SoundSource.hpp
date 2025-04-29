#pragma once
#include <al.h>
#include <ecs.hpp>
#include "pchs/math.hpp"

namespace atom::engine::systems::sound {

class SoundSource {
public:
    SoundSource();
    SoundSource(const SoundSource&) = delete;
    SoundSource(SoundSource&&) noexcept;
    SoundSource& operator=(const SoundSource&) = delete;
    SoundSource& operator=(SoundSource&&) noexcept;
    ~SoundSource() noexcept;

    /**
     * @brief Distance attenuation model.
     * @note Its inner default value is AL_INVERSE_DISTANCE_CLAMPED
     *
     * The model by which sources attenuate with distance.
     *
     * @details The distance model determines how the sound attenuates with distance.
     * None     - No distance attenuation.
     * Inverse  - Doubling the distance halves the source gain.
     * Linear   - Linear gain scaling between the reference and max distances.
     * Exponent - Exponential gain dropoff.
     *
     * Clamped variations work like the non-clamped counterparts, except the
     * distance calculated is clamped between the reference and max distances.
     */
    enum class DistanceModel : uint16_t {
        None                    = AL_NONE,
        InverseDistance         = AL_INVERSE_DISTANCE,
        InverseDistanceClamped  = AL_INVERSE_DISTANCE_CLAMPED,
        LinearDistance          = AL_LINEAR_DISTANCE,
        LinearDistanceClamped   = AL_LINEAR_DISTANCE_CLAMPED,
        ExponentDistance        = AL_EXPONENT_DISTANCE,
        ExponentDistanceClamped = AL_EXPONENT_DISTANCE_CLAMPED,
    };

    void setDistanceModel(const DistanceModel model) const noexcept;

    /**
     * @brief Set speed of sound spread in units per second.
     * @note Default: 343.3
     * @details Its range is [0.0001, ]
     *
     * The speed at which sound waves are assumed to travel, when calculating the
     * doppler effect from source and listener velocities.
     */
    void setSpeed(float speed) const noexcept;

    /**
     * @brief Update the position of the sound source.
     *
     * @details This function would call global function `alSource3f` to update the position of the
     *          sound source, and it would not change member variable in this class.
     */
    void setPosition(float x, float y, float z) noexcept;

    /**
     * @brief Get the Position of this source
     */
    [[nodiscard]] math::Vector3 getPosition() const noexcept;

    /**
     * @copydoc setPosition
     */
    void setPosition(const math::Vector3& position) noexcept;

    void setDirection(const float x, const float y, const float z) const noexcept;
    void setDirection(const math::Vector3& direction) const noexcept;
    void setDirection(const float* direction) const noexcept;

    void bufferData(ecs::resource_handle buffer) noexcept;

    void play() const noexcept;
    void pause() const noexcept;
    void stop() const noexcept;

private:
    ALuint m_Source;
};
} // namespace atom::engine::systems::sound
