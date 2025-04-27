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

    void setOrientation(const float* orientation) const noexcept;
    void setOrientation(
        const float forwardX,
        const float forwardY,
        const float forwardZ,
        const float upX = 0.f,
        const float upY = 1.f,
        const float upZ = 0.f
    ) const noexcept;
    void setOrientation(const math::Vector3& forward, const math::Vector3& up = { 0.f, 1.f, 0.f })
        const noexcept;

    /**
     * @brief Get the Orientation of this source.
     *
     * @return std::pair<math::Vector3, math::Vector3>
     */
    [[nodiscard]] std::pair<math::Vector3, math::Vector3> getOrientation() const noexcept;

    void bufferData(ecs::resource_handle buffer) noexcept;

    void play() const noexcept;
    void pause() const noexcept;
    void stop() const noexcept;

private:
    ALuint m_Source;
};
} // namespace atom::engine::systems::sound
