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
 * @file ControlAllocator.hpp
 *
 * Control allocator.
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#pragma once

#include <ActuatorEffectiveness.hpp>
#include <ActuatorEffectivenessMultirotor.hpp>
#include <ActuatorEffectivenessStandardVTOL.hpp>
#include <ActuatorEffectivenessTiltrotorVTOL.hpp>
#include <ActuatorEffectivenessTailsitterVTOL.hpp>
#include <ActuatorEffectivenessRoverAckermann.hpp>
#include <ActuatorEffectivenessRoverDifferential.hpp>
#include <ActuatorEffectivenessFixedWing.hpp>
#include <ActuatorEffectivenessMCTilt.hpp>
#include <ActuatorEffectivenessCustom.hpp>
#include <ActuatorEffectivenessUUV.hpp>
#include <ActuatorEffectivenessHelicopter.hpp>
#include <ControlAllocation.hpp>
#include <ControlAllocationPseudoInverse.hpp>
#include <ControlAllocationSequentialDesaturation.hpp>
#include <matrix/matrix/math.hpp>
#include <mathlib/mathlib.h>
#include <perf/perf_counter.h>
#include <module_command.hpp>
#include <module/module_params.hpp>
#include <WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/actuator_motors.h>
#include <uORB/topics/actuator_servos.h>
#include <uORB/topics/actuator_servos_trim.h>
#include <uORB/topics/control_allocator_status.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_torque_setpoint.h>
#include <uORB/topics/vehicle_thrust_setpoint.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/failure_detector_status.h>

using namespace nextpilot::param;
using namespace nextpilot::workq;

class ControlAllocator : public ModuleCommand<ControlAllocator>, public ModuleParams, public WorkItemScheduled {
public:
    static constexpr int NUM_ACTUATORS = ControlAllocation::NUM_ACTUATORS;
    static constexpr int NUM_AXES      = ControlAllocation::NUM_AXES;

    static constexpr int MAX_NUM_MOTORS = actuator_motors_s::NUM_CONTROLS;
    static constexpr int MAX_NUM_SERVOS = actuator_servos_s::NUM_CONTROLS;

    using ActuatorVector = ActuatorEffectiveness::ActuatorVector;

    ControlAllocator();

    virtual ~ControlAllocator();

    /** @see ModuleCommand */
    static ControlAllocator *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    void Run() override;

    int init() override;

private:
    struct ParamHandles {
        param_t slew_rate_motors[MAX_NUM_MOTORS];
        param_t slew_rate_servos[MAX_NUM_SERVOS];
    };

    struct Params {
        float slew_rate_motors[MAX_NUM_MOTORS];
        float slew_rate_servos[MAX_NUM_SERVOS];
    };

    /**
     * initialize some vectors/matrices from parameters
     */
    void parameters_updated();

    void update_allocation_method(bool force);
    bool update_effectiveness_source();

    void update_effectiveness_matrix_if_needed(EffectivenessUpdateReason reason);

    void check_for_motor_failures();

    void publish_control_allocator_status(int matrix_index);

    void publish_actuator_controls();

    AllocationMethod   _allocation_method_id{AllocationMethod::NONE};
    ControlAllocation *_control_allocation[ActuatorEffectiveness::MAX_NUM_MATRICES]{}; ///< class for control allocation calculations
    int                _num_control_allocation{0};
    hrt_abstime        _last_effectiveness_update{0};

    enum class EffectivenessSource {
        NONE                 = -1,
        MULTIROTOR           = 0,
        FIXED_WING           = 1,
        STANDARD_VTOL        = 2,
        TILTROTOR_VTOL       = 3,
        TAILSITTER_VTOL      = 4,
        ROVER_ACKERMANN      = 5,
        ROVER_DIFFERENTIAL   = 6,
        MOTORS_6DOF          = 7,
        MULTIROTOR_WITH_TILT = 8,
        CUSTOM               = 9,
        HELICOPTER           = 10,
    };

    enum class FailureMode {
        IGNORE                     = 0,
        REMOVE_FIRST_FAILING_MOTOR = 1,
    };

    EffectivenessSource    _effectiveness_source_id{EffectivenessSource::NONE};
    ActuatorEffectiveness *_actuator_effectiveness{nullptr}; ///< class providing actuator effectiveness

    uint8_t _control_allocation_selection_indexes[NUM_ACTUATORS * ActuatorEffectiveness::MAX_NUM_MATRICES]{};
    int     _num_actuators[(int)ActuatorType::COUNT]{};

    // Inputs
    uORB::SubscriptionCallbackWorkItem _vehicle_torque_setpoint_sub{this, ORB_ID(vehicle_torque_setpoint)}; /**< vehicle torque setpoint subscription */
    uORB::SubscriptionCallbackWorkItem _vehicle_thrust_setpoint_sub{this, ORB_ID(vehicle_thrust_setpoint)}; /**< vehicle thrust setpoint subscription */

    uORB::Subscription _vehicle_torque_setpoint1_sub{ORB_ID(vehicle_torque_setpoint), 1};                   /**< vehicle torque setpoint subscription (2. instance) */
    uORB::Subscription _vehicle_thrust_setpoint1_sub{ORB_ID(vehicle_thrust_setpoint), 1};                   /**< vehicle thrust setpoint subscription (2. instance) */

    // Outputs
    uORB::PublicationMulti<control_allocator_status_s> _control_allocator_status_pub[2]{ORB_ID(control_allocator_status), ORB_ID(control_allocator_status)};

    uORB::Publication<actuator_motors_s>      _actuator_motors_pub{ORB_ID(actuator_motors)};
    uORB::Publication<actuator_servos_s>      _actuator_servos_pub{ORB_ID(actuator_servos)};
    uORB::Publication<actuator_servos_trim_s> _actuator_servos_trim_pub{ORB_ID(actuator_servos_trim)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
    uORB::Subscription _failure_detector_status_sub{ORB_ID(failure_detector_status)};

    matrix::Vector3f _torque_sp;
    matrix::Vector3f _thrust_sp;

    // Reflects motor failures that are currently handled, not motor failures that are reported.
    // For example, the system might report two motor failures, but only the first one is handled by CA
    uint16_t _handled_motor_failure_bitmask{0};

    perf_counter_t _loop_perf; /**< loop duration performance counter */

    bool        _armed{false};
    hrt_abstime _last_run{0};
    hrt_abstime _timestamp_sample{0};
    hrt_abstime _last_status_pub{0};

    ParamHandles _param_handles{};
    Params       _params{};
    bool         _has_slew_rate{false};

    DEFINE_PARAMETERS(
        (ParamInt<params_id::CA_AIRFRAME>)_param_ca_airframe,
        (ParamInt<params_id::CA_METHOD>)_param_ca_method,
        (ParamInt<params_id::CA_FAILURE_MODE>)_param_ca_failure_mode,
        (ParamInt<params_id::CA_R_REV>)_param_r_rev)
};
