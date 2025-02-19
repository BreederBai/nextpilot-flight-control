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
 * @file payload_deliverer.h
 * @author Junwoo Hwang (junwoo091400@gmail.com)
 */
#include "gripper.h"

#include <hrtimer.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <workq/WorkItemScheduled.hpp>


#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionCallback.hpp>

#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_command_ack.h>
#include <uORB/topics/parameter_update.h>

using namespace time_literals;

extern "C" __EXPORT int payload_deliverer_main(int argc, char *argv[]);

// If cached gripper action is set to this value, it means we aren't running any vehicle command
static constexpr int8_t GRIPPER_ACTION_NONE = -1;

/**
 * @brief Payload Deliverer Module
 *
 * Activates a winch / gripper when the DO_WINCH or DO_GRIPPER vehicle command is received,
 * after which the vehicle_command_ack command gets sent upon successful confirmation
 * of the payload deployment.
 *
 * The module runs based on having a subscription callback mechanism to the 'vehicle_command' topic
 * as it only needs to run when we receive the relevant vehicle command.
 *
 * This module communicates with the Navigator which handles publishing such vehicle commands.
 */
class PayloadDeliverer : public ModuleCommand<PayloadDeliverer>, public ModuleParams, public px4::WorkItemScheduled {
public:
    PayloadDeliverer();

    /** @see ModuleCommand **/
    static int task_spawn(int argc, char *argv[]);
    static int custom_command(int argc, char *argv[]);
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand. Override "status" output when invoked via Commandline, to give detailed status **/
    int print_status() override;

    // Initializes the module
    bool init();

    // Gripper test commands provoked by custom_command() via CLI interface
    void gripper_test();
    void gripper_open();
    void gripper_close();

private:
    /**
     * @brief Main Run function that runs when subscription callback is triggered
     */
    void Run() override;

    /**
     * @brief Called to update the parameters and initialize gripper instance if necessary
     */
    void parameter_update();

    /**
     * @brief Initialize or deinitialize gripper instance based on parameter settings
     *
     * Depending on `PD_GRIPPER_EN` and the state of `_gripper` instance, this function will
     * either try to initialize or de-initialize the gripper appropriately.
     */
    void configure_gripper();

    /**
     * @brief Update gipper instance's state and send vehicle command ack
     *
     * This updates the gripper instance to check if the gripper has reached the desired state.
     * And if so, it sends a vehicle command ack to the navigator.
     *
     * If the gripper instance isn't valid (i.e. not initialized), it doesn't do anything
     */
    void gripper_update(const hrt_abstime &now);

    /**
     * @brief Commands the payload delivery mechanism based on the received vehicle command
     *
     * Also handle vehicle command acknowledgement once the delivery is confirmed from the mechanism
     */
    void handle_vehicle_command(const hrt_abstime &now, const vehicle_command_s *vehicle_command = nullptr);

    /**
     * @brief Send DO_GRIPPER vehicle command with specified gripper action
     *
     * @param gripper_command GRIPPER_ACTION_GRAB or GRIPPER_ACTION_RELEASE
     */
    bool send_gripper_vehicle_command(const int32_t gripper_action);

    /**
     * @brief Send ack response to DO_GRIPPER vehicle command with specified parameters
     *
     * For the case of VEHICLE_CMD_RESULT_IN_PROGRESS, progress percentage parameter will be filled out
     */
    bool send_gripper_vehicle_command_ack(const hrt_abstime now, const uint8_t command_result, const uint8_t target_system,
                                          const uint8_t target_component);

    Gripper _gripper;

    // Cached values of the currently running vehicle command for the gripper action
    // used for conflicting vehicle commands & successful vehicle command acknowledgements
    int8_t  _cur_vcmd_gripper_action{GRIPPER_ACTION_NONE};
    uint8_t _cur_vcmd_target_system{0};
    uint8_t _cur_vcmd_target_component{0};

    // Subscription
    uORB::SubscriptionCallbackWorkItem _vehicle_command_sub{this, ORB_ID(vehicle_command)};
    uORB::SubscriptionInterval         _parameter_update_sub{ORB_ID(parameter_update), 1_s}; // subscription limited to 1 Hz updates

    // Publications
    uORB::Publication<vehicle_command_ack_s> _vehicle_command_ack_pub{ORB_ID(vehicle_command_ack)};
    uORB::Publication<vehicle_command_s>     _vehicle_command_pub{ORB_ID(vehicle_command)};

    DEFINE_PARAMETERS(
        (ParamFloat<px4::params::PD_GRIPPER_TO>)_param_gripper_timeout_s,
        (ParamInt<px4::params::PD_GRIPPER_TYPE>)_param_gripper_type,
        (ParamBool<px4::params::PD_GRIPPER_EN>)_param_gripper_enable)
};
