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

#include "input.h"
#include "input_rc.h"
#include <cstdint>

#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/gimbal_device_attitude_status.h>
#include <uORB/topics/gimbal_manager_information.h>
#include <uORB/topics/gimbal_manager_status.h>
#include <uORB/topics/gimbal_manager_set_attitude.h>
#include <uORB/topics/gimbal_manager_set_manual_control.h>
#include <uORB/topics/position_setpoint_triplet.h>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_roi.h>
#include <geo/geo.h>

namespace gimbal {

class InputMavlinkROI : public InputBase {
public:
    InputMavlinkROI() = delete;
    explicit InputMavlinkROI(Parameters &parameters);
    virtual ~InputMavlinkROI();

    void         print_status() const override;
    UpdateResult update(unsigned int timeout_ms, ControlData &control_data, bool already_active) override;
    int          initialize() override;

private:
    void _read_control_data_from_position_setpoint_sub(ControlData &control_data);

    int     _vehicle_roi_sub               = -1;
    int     _position_setpoint_triplet_sub = -1;
    uint8_t _cur_roi_mode{vehicle_roi_s::ROI_NONE};
};

class InputMavlinkCmdMount : public InputBase {
public:
    InputMavlinkCmdMount() = delete;
    explicit InputMavlinkCmdMount(Parameters &parameters);
    virtual ~InputMavlinkCmdMount();

    void         print_status() const override;
    UpdateResult update(unsigned int timeout_ms, ControlData &control_data, bool already_active) override;
    int          initialize() override;

private:
    UpdateResult _process_command(ControlData &control_data, const vehicle_command_s &vehicle_command);
    void         _ack_vehicle_command(const vehicle_command_s &cmd);

    int _vehicle_command_sub = -1;
};

class InputMavlinkGimbalV2 : public InputBase {
public:
    InputMavlinkGimbalV2() = delete;
    explicit InputMavlinkGimbalV2(Parameters &parameters);
    virtual ~InputMavlinkGimbalV2();

    UpdateResult update(unsigned int timeout_ms, ControlData &control_data, bool already_active) override;
    int          initialize() override;
    void         print_status() const override;

private:
    UpdateResult _process_set_attitude(ControlData &control_data, const gimbal_manager_set_attitude_s &set_attitude);
    UpdateResult _process_vehicle_roi(ControlData &control_data, const vehicle_roi_s &vehicle_roi);
    UpdateResult _process_position_setpoint_triplet(ControlData                       &control_data,
                                                    const position_setpoint_triplet_s &position_setpoint_triplet);
    UpdateResult _process_command(ControlData &control_data, const vehicle_command_s &vehicle_command);
    UpdateResult _process_set_manual_control(ControlData                               &control_data,
                                             const gimbal_manager_set_manual_control_s &set_manual_control);
    void         _set_control_data_from_set_attitude(ControlData &control_data, const uint32_t flags, const matrix::Quatf &q,
                                                     const matrix::Vector3f &angular_velocity);
    void         _ack_vehicle_command(const vehicle_command_s &cmd, uint8_t result);
    void         _stream_gimbal_manager_information(const ControlData &control_data);
    void         _stream_gimbal_manager_status(const ControlData &control_data);
    void         _read_control_data_from_position_setpoint_sub(ControlData &control_data);

    int _vehicle_roi_sub                       = -1;
    int _gimbal_manager_set_attitude_sub       = -1;
    int _gimbal_manager_set_manual_control_sub = -1;
    int _position_setpoint_triplet_sub         = -1;
    int _vehicle_command_sub                   = -1;

    uORB::Subscription                              _gimbal_device_attitude_status_sub{ORB_ID(gimbal_device_attitude_status)};
    uORB::Publication<gimbal_manager_information_s> _gimbal_manager_info_pub{ORB_ID(gimbal_manager_information)};
    uORB::Publication<gimbal_manager_status_s>      _gimbal_manager_status_pub{ORB_ID(gimbal_manager_status)};
    uint8_t                                         _cur_roi_mode = vehicle_roi_s::ROI_NONE;

    uint8_t _last_device_compid = 0;
};

} /* namespace gimbal */
