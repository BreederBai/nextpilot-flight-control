/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef ODOMETRY_HPP
#define ODOMETRY_HPP

#include <uORB/topics/vehicle_odometry.h>

class MavlinkStreamOdometry : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamOdometry(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "ODOMETRY";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_ODOMETRY;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _vehicle_odometry_sub.advertised() ? MAVLINK_MSG_ID_ODOMETRY_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamOdometry(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _vehicle_odometry_sub{ORB_ID(vehicle_odometry)};

    bool send() override {
        vehicle_odometry_s odom;

        if (_vehicle_odometry_sub.update(&odom)) {
            mavlink_odometry_t msg{};
            msg.time_usec = odom.timestamp_sample;

            // set the frame_id according to the local frame of the data
            switch (odom.pose_frame) {
            case vehicle_odometry_s::POSE_FRAME_NED:
                msg.frame_id = MAV_FRAME_LOCAL_NED;
                break;

            case vehicle_odometry_s::POSE_FRAME_FRD:
                msg.frame_id = MAV_FRAME_LOCAL_FRD;
                break;
            }

            switch (odom.velocity_frame) {
            case vehicle_odometry_s::VELOCITY_FRAME_NED:
                msg.child_frame_id = MAV_FRAME_LOCAL_NED;
                break;

            case vehicle_odometry_s::VELOCITY_FRAME_FRD:
                msg.child_frame_id = MAV_FRAME_LOCAL_FRD;
                break;

            case vehicle_odometry_s::VELOCITY_FRAME_BODY_FRD:
                msg.child_frame_id = MAV_FRAME_BODY_FRD;
                break;
            }

            msg.x = odom.position[0];
            msg.y = odom.position[1];
            msg.z = odom.position[2];

            msg.q[0] = odom.q[0];
            msg.q[1] = odom.q[1];
            msg.q[2] = odom.q[2];
            msg.q[3] = odom.q[3];

            msg.vx = odom.velocity[0];
            msg.vy = odom.velocity[1];
            msg.vz = odom.velocity[2];

            // Current body rates
            msg.rollspeed  = odom.angular_velocity[0];
            msg.pitchspeed = odom.angular_velocity[1];
            msg.yawspeed   = odom.angular_velocity[2];

            // pose_covariance
            //  Row-major representation of a 6x6 pose cross-covariance matrix upper right triangle
            //  (states: x, y, z, roll, pitch, yaw; first six entries are the first ROW, next five entries are the second ROW, etc.)
            for (auto &pc : msg.pose_covariance) {
                pc = NAN;
            }

            msg.pose_covariance[0]  = odom.position_variance[0]; // X  row 0, col 0
            msg.pose_covariance[6]  = odom.position_variance[1]; // Y  row 1, col 1
            msg.pose_covariance[11] = odom.position_variance[2]; // Z  row 2, col 2

            msg.pose_covariance[15] = odom.orientation_variance[0]; // R  row 3, col 3
            msg.pose_covariance[18] = odom.orientation_variance[1]; // P  row 4, col 4
            msg.pose_covariance[20] = odom.orientation_variance[2]; // Y  row 5, col 5

            // velocity_covariance
            //  Row-major representation of a 6x6 velocity cross-covariance matrix upper right triangle
            //  (states: vx, vy, vz, rollspeed, pitchspeed, yawspeed; first six entries are the first ROW, next five entries are the second ROW, etc.)
            for (auto &vc : msg.velocity_covariance) {
                vc = NAN;
            }

            msg.velocity_covariance[0]  = odom.velocity_variance[0]; // X  row 0, col 0
            msg.velocity_covariance[6]  = odom.velocity_variance[1]; // Y  row 1, col 1
            msg.velocity_covariance[11] = odom.velocity_variance[2]; // Z  row 2, col 2

            msg.reset_counter = odom.reset_counter;

            // source: PX4 estimator
            msg.estimator_type = MAV_ESTIMATOR_TYPE_AUTOPILOT;

            msg.quality = odom.quality;

            mavlink_msg_odometry_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // ODOMETRY_HPP
