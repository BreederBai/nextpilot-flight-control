/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file TargetEstimator.hpp
 *
 * @author Alessandro Simovic <potaito-dev@protonmail.com>
 */

#pragma once

#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>

#include <uORB/topics/follow_target.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/follow_target_estimator.h>
#include <uORB/topics/parameter_update.h>

#include <hrtimer.h>
#include <mathlib/math/filter/AlphaFilter.hpp>
#include <geo/geo.h>
#include <mathlib/mathlib.h>
#include <module/module_params.hpp>
#include <module/module_usage.h>
#include <workq/WorkItemScheduled.hpp>

static constexpr float GPS_MESSAGE_STALE_TIMEOUT_MS        = 3000.0f; // Duration after which the connection to the target is considered lost
static constexpr float MINIMUM_TIME_BETWEEN_POS_FUSIONS_MS = 500.0f;
static constexpr float MINIMUM_TIME_BETWEEN_VEL_FUSIONS_MS = 100.0f;
static constexpr float ACCELERATION_SATURATION             = 20.0f; // 2*g
static constexpr float MINIMUM_SPEED_FOR_TARGET_MOVING     = 0.1f;  // speed threshold above which the target is considered to be moving

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::workq;
using namespace nextpilot::param;

struct filter_gains_s {
    // Position fusion gains
    float G_p;
    float H_p;
    float K_p;

    // Velocity fusion gains
    float G_v;
    float H_v;

    // Estimator position / velocity update gains
    float responsiveness;
};

struct filter_states_s {
    matrix::Vector3f pos_ned_est{}; // target's position in NED frame
    matrix::Vector3f vel_ned_est{}; // target's velocity in NED frame
    matrix::Vector3f acc_ned_est{}; // target's acceleration in NED frame

    /**
     * Check if all state are finite
     *
     * @return true if all state are finite, or false as soon as any state is NAN
     */
    bool is_finite() {
        return pos_ned_est.isAllFinite() && vel_ned_est.isAllFinite() && acc_ned_est.isAllFinite();
    }

    /**
     * Limits the acceleration state to some sane value to prevent unrealistic
     * spikes in the acceleration, which could cause severely unexpected behaviour in the drone
     * that is tracking the target
     *
     * @param saturation [m/s^2] limit to use for acceleration saturation
     */
    void saturate_acceleration(float saturation) {
        if (acc_ned_est.norm_squared() > saturation * saturation) {
            acc_ned_est = acc_ned_est.unit_or_zero() * saturation;
        }
    }
};

class TargetEstimator : public ModuleParams, public WorkItemScheduled {
public:
    TargetEstimator();
    ~TargetEstimator() override;

    bool Start();
    void Stop();

protected:
    void Run() override;

    /**
     * Process new measurement data and iterate filter to current time
     */
    void update();

    /**
     * Check for parameter changes and update them if needed.
     *
     * @param force for a parameter update
     */
    void parameters_update(bool force = false);

    /**
     * Recompute filter gains
     *
     * @param filter_gains filter gains that will be updated based on current PX4 parameters
     */
    void update_filter_gains(filter_gains_s &filter_gains) const;

    /**
     * Checks whether a sensor measurement should be fused or not
     *
     *
     * @param current_measurement the most recent measurement that has not yet been fused
     * @param previous_measurement the previous measurement that was accepted and therefore fused
     * @param last_fusion_timestamp last timestamp of when this sensor data was fused
     * @param min_delta_t minimum amount of time that needs to pass between two sensor fusions
     */
    bool measurement_can_be_fused(const matrix::Vector3f &current_measurement, const matrix::Vector3f &previous_measurement,
                                  uint64_t last_fusion_timestamp, float min_delta_t) const;

    /**
     * Perform filter update with new follow_target data
     *
     * @param follow_target GPS data last received from target
     */
    void measurement_update(follow_target_s follow_target);

    /**
     * Perform prediction step based on simple position-velocity-acceleration model of a point mass
     * Can be called at a much higher frequency than measurement data is being received.
     *
     * @param deltatime time [s] since the last prediction or measurement update
     */
    void prediction_update(float deltatime);

    /**
     * Get current LAT/LON/ALT estimate of target
     *
     * @return Current position estimate of target as latitude / longitude / altitude vector
     */
    matrix::Vector3<double> get_lat_lon_alt_est() const;

    /**
     * Check if last received data from target is too old
     *
     * @param timeout_duration_ms timeout in [ms] to use for this check
     */
    bool is_stale(const float timeout_duration_ms) const;

    /**
     * Reset all filter states causing it to completely forget the old filter state
     */
    void reset();

    filter_gains_s  _filter_gains;
    filter_states_s _filter_states;

    MapProjection            _reference_position{};
    vehicle_local_position_s _vehicle_local_position{};

    hrt_abstime _last_iteration_timestamp{0};
    hrt_abstime _last_filter_reset_timestamp{0};
    hrt_abstime _last_position_fusion_timestamp{0};
    hrt_abstime _last_velocity_fusion_timestamp{0};
    hrt_abstime _last_follow_target_timestamp{0};

    // Pos/vel from previous measurement update. Required for filtering duplicate messages
    matrix::Vector3f _pos_measurement_old{};
    matrix::Vector3f _vel_measurement_old{};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::FLW_TGT_RS>)_param_flw_tgt_rs)

    // Subscriptions
    uORB::SubscriptionCallbackWorkItem                _follow_target_sub{this, ORB_ID(follow_target)};
    uORB::SubscriptionCallbackWorkItem                _vehicle_local_position_sub{this, ORB_ID(vehicle_local_position)};
    uORB::PublicationMulti<follow_target_estimator_s> _follow_target_estimator_pub{ORB_ID(follow_target_estimator)};
    uORB::SubscriptionInterval                        _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    // Debugging
    uint64_t _prediction_count{0};
    uint64_t _fusion_count{0};
};
