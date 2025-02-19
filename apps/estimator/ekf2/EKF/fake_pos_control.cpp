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
 * @file fake_pos_control.cpp
 * Control functions for ekf fake position fusion
 */

#include "ekf.h"

void Ekf::controlFakePosFusion() {
    auto &aid_src = _aid_src_fake_pos;

    // If we aren't doing any aiding, fake position measurements at the last known position to constrain drift
    // During intial tilt aligment, fake position is used to perform a "quasi-stationary" leveling of the EKF
    const bool fake_pos_data_ready = !isHorizontalAidingActive() && isTimedOut(aid_src.time_last_fuse, (uint64_t)2e5); // Fuse fake position at a limited rate

    if (fake_pos_data_ready) {
        Vector2f obs_var;

        if (_control_status.flags.in_air && _control_status.flags.tilt_align) {
            obs_var(0) = obs_var(1) = sq(fmaxf(_params.pos_noaid_noise, _params.gps_pos_noise));

        } else if (!_control_status.flags.in_air && _control_status.flags.vehicle_at_rest) {
            // Accelerate tilt fine alignment by fusing more
            // aggressively when the vehicle is at rest
            obs_var(0) = obs_var(1) = sq(0.01f);

        } else {
            obs_var(0) = obs_var(1) = sq(0.5f);
        }

        const float innov_gate = 3.f;

        updateHorizontalPositionAidSrcStatus(_time_delayed_us, Vector2f(_last_known_pos), obs_var, innov_gate, aid_src);

        const bool continuing_conditions_passing = !isHorizontalAidingActive();
        const bool starting_conditions_passing   = continuing_conditions_passing && _horizontal_deadreckon_time_exceeded;

        if (_control_status.flags.fake_pos) {
            if (continuing_conditions_passing) {
                // always protect against extreme values that could result in a NaN
                aid_src.fusion_enabled = (aid_src.test_ratio[0] < sq(100.0f / innov_gate)) && (aid_src.test_ratio[1] < sq(100.0f / innov_gate));

                fuseHorizontalPosition(aid_src);

                const bool is_fusion_failing = isTimedOut(aid_src.time_last_fuse, (uint64_t)4e5);

                if (is_fusion_failing) {
                    resetFakePosFusion();
                }

            } else {
                stopFakePosFusion();
            }

        } else {
            if (starting_conditions_passing) {
                ECL_INFO("start fake position fusion");
                _control_status.flags.fake_pos = true;
                resetFakePosFusion();

                if (_control_status.flags.tilt_align) {
                    // The fake position fusion is not started for initial alignement
                    _warning_events.flags.stopping_navigation = true;
                    ECL_WARN("stopping navigation");
                }
            }
        }

    } else if (_control_status.flags.fake_pos && isHorizontalAidingActive()) {
        stopFakePosFusion();
    }
}

void Ekf::resetFakePosFusion() {
    ECL_INFO("reset fake position fusion");
    _last_known_pos.xy() = _state.pos.xy();

    resetHorizontalPositionToLastKnown();
    resetHorizontalVelocityToZero();

    _aid_src_fake_pos.time_last_fuse = _time_delayed_us;
}

void Ekf::stopFakePosFusion() {
    if (_control_status.flags.fake_pos) {
        ECL_INFO("stop fake position fusion");
        _control_status.flags.fake_pos = false;

        resetEstimatorAidStatus(_aid_src_fake_pos);
    }
}
