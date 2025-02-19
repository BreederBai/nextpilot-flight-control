/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include "FlightTask.hpp"
#include "FlightTasks_generated.hpp"
#include <hrtimer.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <workq/WorkItemScheduled.hpp>
#include <perf/perf_counter.h>

#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>
#include <uORB/topics/landing_gear.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/takeoff_status.h>
#include <uORB/topics/trajectory_setpoint.h>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_land_detected.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_local_position_setpoint.h>
#include <uORB/topics/vehicle_status.h>

#include <new>

enum class FlightTaskError : int {
    NoError          = 0,
    InvalidTask      = -1,
    ActivationFailed = -2
};

class FlightModeManager : public ModuleCommand<FlightModeManager>, public ModuleParams, public WorkItem {
public:
    FlightModeManager();
    ~FlightModeManager() override;

    /** @see ModuleCommand */
    static FlightModeManager *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    int init() override;

private:
    void Run() override;
    void updateParams() override;
    void start_flight_task();
    void handleCommand();
    void generateTrajectorySetpoint(const float dt, const vehicle_local_position_s &vehicle_local_position);
    void limitAltitude(trajectory_setpoint_s &setpoint, const vehicle_local_position_s &vehicle_local_position);

    /**
     * Switch to a specific task (for normal usage)
     * @param task index to switch to
     * @return 0 on success, <0 on error
     */
    FlightTaskError switchTask(FlightTaskIndex new_task_index);
    FlightTaskError switchTask(int new_task_index);

    /**
     * Call this method to get the description of a task error.
     */
    const char *errorToString(const FlightTaskError error);

    /**
     * Check if any task is active
     * @return true if a task is active, false if not
     */
    bool isAnyTaskActive() const {
        return _current_task.task;
    }

    void tryApplyCommandIfAny();

    // generated
    int _initTask(FlightTaskIndex task_index);

    /**
     * Union with all existing tasks: we use it to make sure that only the memory of the largest existing
     * task is needed, and to avoid using dynamic memory allocations.
     */
    TaskUnion _task_union; /**< storage for the currently active task */

    struct flight_task_t {
        FlightTask     *task{nullptr};
        FlightTaskIndex index{FlightTaskIndex::None};
    } _current_task{};

    int8_t  _old_landing_gear_position{landing_gear_s::GEAR_KEEP};
    uint8_t _takeoff_state{takeoff_status_s::TAKEOFF_STATE_UNINITIALIZED};

    bool _no_matching_task_error_printed{false};

    perf_counter_t _loop_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")}; ///< loop duration performance counter
    hrt_abstime    _time_stamp_last_loop{0};                                  ///< time stamp of last loop iteration

    vehicle_command_s _current_command{};
    bool              _command_failed{false};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription                              _takeoff_status_sub{ORB_ID(takeoff_status)};
    uORB::Subscription                              _vehicle_attitude_setpoint_sub{ORB_ID(vehicle_attitude_setpoint)};
    uORB::Subscription                              _vehicle_command_sub{ORB_ID(vehicle_command)};
    uORB::SubscriptionData<home_position_s>         _home_position_sub{ORB_ID(home_position)};
    uORB::SubscriptionData<vehicle_control_mode_s>  _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
    uORB::SubscriptionData<vehicle_land_detected_s> _vehicle_land_detected_sub{ORB_ID(vehicle_land_detected)};
    uORB::SubscriptionCallbackWorkItem              _vehicle_local_position_sub{this, ORB_ID(vehicle_local_position)};

    uORB::SubscriptionData<vehicle_status_s> _vehicle_status_sub{ORB_ID(vehicle_status)};

    uORB::Publication<landing_gear_s>        _landing_gear_pub{ORB_ID(landing_gear)};
    uORB::Publication<trajectory_setpoint_s> _trajectory_setpoint_pub{ORB_ID(trajectory_setpoint)};
    uORB::Publication<vehicle_constraints_s> _vehicle_constraints_pub{ORB_ID(vehicle_constraints)};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::LNDMC_ALT_MAX>)_param_lndmc_alt_max,
        (ParamInt<params_id::MPC_POS_MODE>)_param_mpc_pos_mode);
};
