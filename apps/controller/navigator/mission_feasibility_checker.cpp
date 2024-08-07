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
 * @file mission_feasibility_checker.cpp
 * Provides checks if mission is feasible given the navigation capabilities
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 * @author Thomas Gubler <thomasgubler@student.ethz.ch>
 * @author Sander Smeets <sander@droneslab.com>
 * @author Nuno Marques <nuno.marques@dronesolutions.io>
 */

#include "mission_feasibility_checker.h"
#include "MissionFeasibility/FeasibilityChecker.hpp"
#include "mission_block.h"
#include "navigator.h"
// #include <drivers/drv_pwm_output.h>
#include <geo/geo.h>
#include <mathlib/mathlib.h>
#include <ulog/mavlink_log.h>
#include <uORB/Subscription.hpp>
#include <events/events.h>

bool MissionFeasibilityChecker::checkMissionFeasible(const mission_s &mission) {
    // Reset warning flag
    _navigator->get_mission_result()->warning = false;

    // first check if we have a valid position
    const bool home_valid     = _navigator->home_global_position_valid();
    const bool home_alt_valid = _navigator->home_alt_valid();

    // trivial case: A mission with length zero cannot be valid
    if ((int)mission.count <= 0) {
        return false;
    }

    if (!home_alt_valid) {
        mavlink_log_info(_navigator->get_mavlink_log_pub(), "Not yet ready for mission, no position lock.\t");
        // events::send(events::ID("navigator_mis_no_pos_lock"), events::Log::Info, "Not yet ready for mission, no position lock");
        return false;
    }

    bool failed = false;

    for (size_t i = 0; i < mission.count; i++) {
        struct mission_item_s missionitem = {};
        const ssize_t         len         = sizeof(struct mission_item_s);

        if (dm_read((dm_item_t)mission.dataman_id, i, &missionitem, len) != len) {
            _navigator->get_mission_result()->warning = true;
            /* not supposed to happen unless the datamanager can't access the SD card, etc. */
            return false;
        }

        if (!_feasibility_checker.processNextItem(missionitem, i, mission.count)) {
            failed = true;
            break;
        }
    }

    failed |= _feasibility_checker.someCheckFailed();

    failed |= !checkGeofence(mission, _navigator->get_home_position()->alt, home_valid);

    _navigator->get_mission_result()->warning = failed;

    return !failed;
}

bool MissionFeasibilityChecker::checkGeofence(const mission_s &mission, float home_alt, bool home_valid) {
    if (_navigator->get_geofence().isHomeRequired() && !home_valid) {
        mavlink_log_critical(_navigator->get_mavlink_log_pub(), "Geofence requires valid home position\t");
        // events::send(events::ID("navigator_mis_geofence_no_home"), {events::Log::Error, events::LogInternal::Info},
        // "Geofence requires a valid home position");
        return false;
    }

    /* Check if all mission items are inside the geofence (if we have a valid geofence) */
    if (_navigator->get_geofence().valid()) {
        for (size_t i = 0; i < mission.count; i++) {
            struct mission_item_s missionitem = {};
            const ssize_t         len         = sizeof(missionitem);

            if (dm_read((dm_item_t)mission.dataman_id, i, &missionitem, len) != len) {
                /* not supposed to happen unless the datamanager can't access the SD card, etc. */
                return false;
            }

            if (missionitem.altitude_is_relative && !home_valid) {
                mavlink_log_critical(_navigator->get_mavlink_log_pub(), "Geofence requires valid home position\t");
                // events::send(events::ID("navigator_mis_geofence_no_home2"), {events::Log::Error, events::LogInternal::Info},
                // "Geofence requires a valid home position");
                return false;
            }

            // Geofence function checks against home altitude amsl
            missionitem.altitude = missionitem.altitude_is_relative ? missionitem.altitude + home_alt : missionitem.altitude;

            if (MissionBlock::item_contains_position(missionitem) && !_navigator->get_geofence().check(missionitem)) {
                mavlink_log_critical(_navigator->get_mavlink_log_pub(), "Geofence violation for waypoint %zu\t", i + 1);
                // events::send<int16_t>(events::ID("navigator_mis_geofence_violation"), {events::Log::Error, events::LogInternal::Info},
                // "Geofence violation for waypoint {1}",
                // i + 1);
                return false;
            }
        }
    }

    return true;
}
