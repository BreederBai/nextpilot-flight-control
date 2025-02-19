/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef OPEN_DRONE_ID_BASIC_ID_HPP
#define OPEN_DRONE_ID_BASIC_ID_HPP

#include <uORB/topics/vehicle_status.h>

class MavlinkStreamOpenDroneIdBasicId : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamOpenDroneIdBasicId(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "OPEN_DRONE_ID_BASIC_ID";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_OPEN_DRONE_ID_BASIC_ID;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _vehicle_status_sub.advertised() ? MAVLINK_MSG_ID_OPEN_DRONE_ID_BASIC_ID_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamOpenDroneIdBasicId(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

    MAV_ODID_UA_TYPE odidTypeForMavType(uint8_t system_type) {
        switch (system_type) {
        case MAV_TYPE_GENERIC:
            return MAV_ODID_UA_TYPE_OTHER;

        case MAV_TYPE_FIXED_WING:
            return MAV_ODID_UA_TYPE_AEROPLANE;

        case MAV_TYPE_QUADROTOR:
            return MAV_ODID_UA_TYPE_HELICOPTER_OR_MULTIROTOR;

        case MAV_TYPE_COAXIAL:
            return MAV_ODID_UA_TYPE_HELICOPTER_OR_MULTIROTOR;

        case MAV_TYPE_HELICOPTER:
            return MAV_ODID_UA_TYPE_HELICOPTER_OR_MULTIROTOR;

        case MAV_TYPE_ANTENNA_TRACKER:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_GCS:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_AIRSHIP:
            return MAV_ODID_UA_TYPE_AIRSHIP;

        case MAV_TYPE_FREE_BALLOON:
            return MAV_ODID_UA_TYPE_FREE_BALLOON;

        case MAV_TYPE_ROCKET:
            return MAV_ODID_UA_TYPE_ROCKET;

        case MAV_TYPE_GROUND_ROVER:
            return MAV_ODID_UA_TYPE_GROUND_OBSTACLE;

        case MAV_TYPE_SURFACE_BOAT:
            return MAV_ODID_UA_TYPE_OTHER;

        case MAV_TYPE_SUBMARINE:
            return MAV_ODID_UA_TYPE_OTHER;

        case MAV_TYPE_HEXAROTOR:
            return MAV_ODID_UA_TYPE_HELICOPTER_OR_MULTIROTOR;

        case MAV_TYPE_OCTOROTOR:
            return MAV_ODID_UA_TYPE_HELICOPTER_OR_MULTIROTOR;

        case MAV_TYPE_TRICOPTER:
            return MAV_ODID_UA_TYPE_HELICOPTER_OR_MULTIROTOR;

        case MAV_TYPE_FLAPPING_WING:
            return MAV_ODID_UA_TYPE_ORNITHOPTER;

        case MAV_TYPE_KITE:
            return MAV_ODID_UA_TYPE_KITE;

        case MAV_TYPE_ONBOARD_CONTROLLER:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_VTOL_TAILSITTER_DUOROTOR:
            return MAV_ODID_UA_TYPE_HYBRID_LIFT;

        case MAV_TYPE_VTOL_TAILSITTER_QUADROTOR:
            return MAV_ODID_UA_TYPE_HYBRID_LIFT;

        case MAV_TYPE_VTOL_TILTROTOR:
            return MAV_ODID_UA_TYPE_HYBRID_LIFT;

        case MAV_TYPE_VTOL_FIXEDROTOR:
            return MAV_ODID_UA_TYPE_HYBRID_LIFT;

        case MAV_TYPE_VTOL_TAILSITTER:
            return MAV_ODID_UA_TYPE_HYBRID_LIFT;

        case MAV_TYPE_VTOL_TILTWING:
            return MAV_ODID_UA_TYPE_HYBRID_LIFT;

        case MAV_TYPE_VTOL_RESERVED5:
            return MAV_ODID_UA_TYPE_HYBRID_LIFT;

        case MAV_TYPE_GIMBAL:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_ADSB:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_PARAFOIL:
            return MAV_ODID_UA_TYPE_FREE_FALL_PARACHUTE;

        case MAV_TYPE_DODECAROTOR:
            return MAV_ODID_UA_TYPE_HELICOPTER_OR_MULTIROTOR;

        case MAV_TYPE_CAMERA:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_CHARGING_STATION:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_FLARM:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_SERVO:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_ODID:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_DECAROTOR:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_BATTERY:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_PARACHUTE:
            return MAV_ODID_UA_TYPE_FREE_FALL_PARACHUTE;

        case MAV_TYPE_LOG:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_OSD:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_IMU:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_GPS:
            return MAV_ODID_UA_TYPE_NONE;

        case MAV_TYPE_WINCH:
            return MAV_ODID_UA_TYPE_NONE;

        default:
            return MAV_ODID_UA_TYPE_OTHER;
        }
    }

    bool send() override {
        vehicle_status_s vehicle_status;

        if (_vehicle_status_sub.update(&vehicle_status)) {
            mavlink_open_drone_id_basic_id_t msg{};

            msg.target_system    = 0; // 0 for broadcast
            msg.target_component = 0; // 0 for broadcast
            // msg.id_or_mac // Only used for drone ID data received from other UAs.

            // id_type: MAV_ODID_ID_TYPE
            msg.id_type = MAV_ODID_ID_TYPE_SERIAL_NUMBER;

            // ua_type: MAV_ODID_UA_TYPE
            msg.ua_type = odidTypeForMavType(vehicle_status.system_type);

            // uas_id: UAS (Unmanned Aircraft System) ID following the format specified by id_type
            // TODO: MAV_ODID_ID_TYPE_SERIAL_NUMBER needs to be ANSI/CTA-2063 format
            // board_get_guid_formated((char *)(msg.uas_id), sizeof(msg.uas_id));

            mavlink_msg_open_drone_id_basic_id_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // OPEN_DRONE_ID_BASIC_ID_HPP
