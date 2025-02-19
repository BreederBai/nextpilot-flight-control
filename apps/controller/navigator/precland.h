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
 * @file precland.h
 *
 * Helper class to do precision landing with a landing target
 *
 * @author Nicolas de Palezieux (Sunflower Labs) <ndepal@gmail.com>
 */

#pragma once

#include <matrix/math.hpp>
#include <geo/geo.h>
#include <module/module_params.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/landing_target_pose.h>
#include "navigator_mode.h"
#include "mission_block.h"

using namespace nextpilot::param;
using namespace nextpilot;

enum class PrecLandState {
    Start,              // Starting state
    HorizontalApproach, // Positioning over landing target while maintaining altitude
    DescendAboveTarget, // Stay over landing target while descending
    FinalApproach,      // Final landing approach, even without landing target
    Search,             // Search for landing target
    Fallback,           // Fallback landing method
    Done                // Done landing
};

enum class PrecLandMode {
    Opportunistic = 1, // only do precision landing if landing target visible at the beginning
    Required      = 2  // try to find landing target if not visible at the beginning
};

class PrecLand : public MissionBlock, public ModuleParams {
public:
    PrecLand(Navigator *navigator);
    ~PrecLand() override = default;

    void on_activation() override;
    void on_active() override;
    void on_inactivation() override;

    void set_mode(PrecLandMode mode) {
        _mode = mode;
    }

    PrecLandMode get_mode() {
        return _mode;
    }

    bool is_activated() {
        return _is_activated;
    }

private:
    void updateParams() override;

    // run the control loop for each state
    void run_state_start();
    void run_state_horizontal_approach();
    void run_state_descend_above_target();
    void run_state_final_approach();
    void run_state_search();
    void run_state_fallback();

    // attempt to switch to a different state. Returns true if state change was successful, false otherwise
    bool switch_to_state_start();
    bool switch_to_state_horizontal_approach();
    bool switch_to_state_descend_above_target();
    bool switch_to_state_final_approach();
    bool switch_to_state_search();
    bool switch_to_state_fallback();
    bool switch_to_state_done();

    void print_state_switch_message(const char *state_name);

    // check if a given state could be changed into. Return true if possible to transition to state, false otherwise
    bool check_state_conditions(PrecLandState state);
    void slewrate(float &sp_x, float &sp_y);

    landing_target_pose_s _target_pose{}; /**< precision landing target position */

    uORB::Subscription _target_pose_sub{ORB_ID(landing_target_pose)};
    bool               _target_pose_valid{false};   /**< whether we have received a landing target position message */
    bool               _target_pose_updated{false}; /**< wether the landing target position message is updated */

    MapProjection _map_ref{};                       /**< class for local/global projections */

    uint64_t _state_start_time{0};                  /**< time when we entered current state */
    uint64_t _last_slewrate_time{0};                /**< time when we last limited setpoint changes */
    uint64_t _target_acquired_time{0};              /**< time when we first saw the landing target during search */
    uint64_t _point_reached_time{0};                /**< time when we reached a setpoint */

    int   _search_cnt{0};                           /**< counter of how many times we had to search for the landing target */
    float _approach_alt{0.0f};                      /**< altitude at which to stay during horizontal approach */

    matrix::Vector2f _sp_pev;
    matrix::Vector2f _sp_pev_prev;

    PrecLandState _state{PrecLandState::Start};

    PrecLandMode _mode{PrecLandMode::Opportunistic};

    bool _is_activated{false}; /**< indicates if precland is activated */

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::PLD_BTOUT>)_param_pld_btout,
        (ParamFloat<params_id::PLD_HACC_RAD>)_param_pld_hacc_rad,
        (ParamFloat<params_id::PLD_FAPPR_ALT>)_param_pld_fappr_alt,
        (ParamFloat<params_id::PLD_SRCH_ALT>)_param_pld_srch_alt,
        (ParamFloat<params_id::PLD_SRCH_TOUT>)_param_pld_srch_tout,
        (ParamInt<params_id::PLD_MAX_SRCH>)_param_pld_max_srch)

    // non-navigator parameters
    param_t _handle_param_acceleration_hor{PARAM_INVALID};
    param_t _handle_param_xy_vel_cruise{PARAM_INVALID};
    float   _param_acceleration_hor{0.0f};
    float   _param_xy_vel_cruise{0.0f};
};
