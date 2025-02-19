/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GPS_RAW_INT_HPP
#define GPS_RAW_INT_HPP

#include <uORB/topics/sensor_gps.h>

using namespace time_literals;

class MavlinkStreamGPSRawInt : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGPSRawInt(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GPS_RAW_INT";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GPS_RAW_INT;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _sensor_gps_sub.advertised() ? (MAVLINK_MSG_ID_GPS_RAW_INT_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) : 0;
    }

private:
    explicit MavlinkStreamGPSRawInt(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription           _sensor_gps_sub{ORB_ID(sensor_gps), 0};
    hrt_abstime                  _last_send_ts{};
    static constexpr hrt_abstime kNoGpsSendInterval{1_s};

    bool send() override {
        sensor_gps_s          gps;
        mavlink_gps_raw_int_t msg{};
        hrt_abstime           now{};

        if (_sensor_gps_sub.update(&gps)) {
            msg.time_usec = gps.timestamp;
            msg.fix_type  = gps.fix_type;
            msg.lat       = gps.lat;
            msg.lon       = gps.lon;
            msg.alt       = gps.alt;
            msg.eph       = gps.hdop * 100; // GPS HDOP horizontal dilution of position (unitless)
            msg.epv       = gps.vdop * 100; // GPS VDOP vertical dilution of position (unitless)

            if (PX4_ISFINITE(gps.vel_m_s) && (fabsf(gps.vel_m_s) >= 0.f)) {
                msg.vel = gps.vel_m_s * 100.f; // cm/s

            } else {
                msg.vel = UINT16_MAX; // If unknown, set to: UINT16_MAX
            }

            msg.cog                = math::degrees(matrix::wrap_2pi(gps.cog_rad)) * 1e2f;
            msg.satellites_visible = gps.satellites_used;
            msg.alt_ellipsoid      = gps.alt_ellipsoid;
            msg.h_acc              = gps.eph * 1e3f;            // position uncertainty in mm
            msg.v_acc              = gps.epv * 1e3f;            // altitude uncertainty in mm
            msg.vel_acc            = gps.s_variance_m_s * 1e3f; // speed uncertainty in mm

            if (PX4_ISFINITE(gps.heading)) {
                if (fabsf(gps.heading) < FLT_EPSILON) {
                    msg.yaw = 36000; // Use 36000 for north.

                } else {
                    msg.yaw = math::degrees(matrix::wrap_2pi(gps.heading)) * 100.0f; // centidegrees
                }

                if (PX4_ISFINITE(gps.heading_accuracy)) {
                    msg.hdg_acc = math::degrees(gps.heading_accuracy) * 1e5f; // Heading / track uncertainty in degE5
                }
            }

            mavlink_msg_gps_raw_int_send_struct(_mavlink->get_channel(), &msg);
            _last_send_ts = gps.timestamp;

            return true;

        } else if (_last_send_ts != 0 && (now = hrt_absolute_time()) > _last_send_ts + kNoGpsSendInterval) {
            msg.fix_type           = GPS_FIX_TYPE_NO_GPS;
            msg.eph                = UINT16_MAX;
            msg.epv                = UINT16_MAX;
            msg.vel                = UINT16_MAX;
            msg.cog                = UINT16_MAX;
            msg.satellites_visible = UINT8_MAX;
            mavlink_msg_gps_raw_int_send_struct(_mavlink->get_channel(), &msg);
            _last_send_ts = now;

            return true;
        }

        return false;
    }
};

#endif // GPS_RAW_INT_HPP
