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
 * @file ecl_yaw_controller.cpp
 * Implementation of a simple orthogonal coordinated turn yaw PID controller.
 *
 * Authors and acknowledgements in header.
 */

#define LOG_TAG "ecl_yaw"

#include <ulog/log.h>
#include "ecl_yaw_controller.h"
#include <float.h>
#include <geo/geo.h>
#include <mathlib/mathlib.h>

float ECL_YawController::control_attitude(const float dt, const ECL_ControlData &ctl_data) {
    /* Do not calculate control signal with bad inputs */
    if (!(PX4_ISFINITE(ctl_data.roll) &&
          PX4_ISFINITE(ctl_data.pitch) &&
          PX4_ISFINITE(ctl_data.euler_pitch_rate_setpoint) &&
          PX4_ISFINITE(ctl_data.airspeed_constrained))) {
        return _body_rate_setpoint;
    }

    float constrained_roll;
    bool  inverted = false;

    /* roll is used as feedforward term and inverted flight needs to be considered */
    if (fabsf(ctl_data.roll) < math::radians(90.0f)) {
        /* not inverted, but numerically still potentially close to infinity */
        constrained_roll = math::constrain(ctl_data.roll, math::radians(-80.0f), math::radians(80.0f));

    } else {
        inverted = true;

        // inverted flight, constrain on the two extremes of -pi..+pi to avoid infinity
        // note: the ranges are extended by 10 deg here to avoid numeric resolution effects
        if (ctl_data.roll > 0.0f) {
            /* right hemisphere */
            constrained_roll = math::constrain(ctl_data.roll, math::radians(100.0f), math::radians(180.0f));

        } else {
            /* left hemisphere */
            constrained_roll = math::constrain(ctl_data.roll, math::radians(-180.0f), math::radians(-100.0f));
        }
    }

    constrained_roll = math::constrain(constrained_roll, -fabsf(ctl_data.roll_setpoint), fabsf(ctl_data.roll_setpoint));

    if (!inverted) {
        /* Calculate desired yaw rate from coordinated turn constraint / (no side forces) */
        _euler_rate_setpoint = tanf(constrained_roll) * cosf(ctl_data.pitch) * CONSTANTS_ONE_G / ctl_data.airspeed_constrained;

        /* Transform setpoint to body angular rates (jacobian) */
        const float yaw_body_rate_setpoint_raw = -sinf(ctl_data.roll) * ctl_data.euler_pitch_rate_setpoint +
                                                 cosf(ctl_data.roll) * cosf(ctl_data.pitch) * _euler_rate_setpoint;
        _body_rate_setpoint = math::constrain(yaw_body_rate_setpoint_raw, -_max_rate, _max_rate);
    }

    if (!PX4_ISFINITE(_body_rate_setpoint)) {
        PX4_WARN("yaw rate sepoint not finite");
        _body_rate_setpoint = 0.0f;
    }

    return _body_rate_setpoint;
}
