/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef POSITION_TARGET_GLOBAL_INT_HPP
#define POSITION_TARGET_GLOBAL_INT_HPP

#include <uORB/topics/position_setpoint_triplet.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_local_position_setpoint.h>

class MavlinkStreamPositionTargetGlobalInt : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamPositionTargetGlobalInt(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "POSITION_TARGET_GLOBAL_INT";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_POSITION_TARGET_GLOBAL_INT;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _pos_sp_triplet_sub.advertised() ? MAVLINK_MSG_ID_POSITION_TARGET_GLOBAL_INT_LEN +
                                                      MAVLINK_NUM_NON_PAYLOAD_BYTES :
                                                  0;
    }

private:
    explicit MavlinkStreamPositionTargetGlobalInt(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _control_mode_sub{ORB_ID(vehicle_control_mode)};
    uORB::Subscription _lpos_sp_sub{ORB_ID(vehicle_local_position_setpoint)};
    uORB::Subscription _pos_sp_triplet_sub{ORB_ID(position_setpoint_triplet)};

    bool send() override {
        vehicle_control_mode_s control_mode{};
        _control_mode_sub.copy(&control_mode);

        if (control_mode.flag_control_position_enabled) {
            position_setpoint_triplet_s pos_sp_triplet{};
            _pos_sp_triplet_sub.copy(&pos_sp_triplet);

            if (pos_sp_triplet.timestamp > 0 && pos_sp_triplet.current.valid && PX4_ISFINITE(pos_sp_triplet.current.lat) && PX4_ISFINITE(pos_sp_triplet.current.lon)) {
                mavlink_position_target_global_int_t msg{};

                msg.time_boot_ms     = hrt_absolute_time() / 1000;
                msg.coordinate_frame = MAV_FRAME_GLOBAL_INT;
                msg.lat_int          = pos_sp_triplet.current.lat * 1e7;
                msg.lon_int          = pos_sp_triplet.current.lon * 1e7;
                msg.alt              = pos_sp_triplet.current.alt;

                vehicle_local_position_setpoint_s lpos_sp;

                if (_lpos_sp_sub.copy(&lpos_sp) && (lpos_sp.timestamp > 0)) {
                    // velocity
                    msg.vx = lpos_sp.vx;
                    msg.vy = lpos_sp.vy;
                    msg.vz = lpos_sp.vz;

                    // acceleration
                    msg.afx = lpos_sp.acceleration[0];
                    msg.afy = lpos_sp.acceleration[1];
                    msg.afz = lpos_sp.acceleration[2];

                    // yaw
                    msg.yaw      = lpos_sp.yaw;
                    msg.yaw_rate = lpos_sp.yawspeed;
                }

                mavlink_msg_position_target_global_int_send_struct(_mavlink->get_channel(), &msg);

                return true;
            }
        }

        return false;
    }
};

#endif // POSITION_TARGET_GLOBAL_INT_HPP
