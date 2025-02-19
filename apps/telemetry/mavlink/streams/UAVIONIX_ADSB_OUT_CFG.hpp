/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef UAVIONIX_ADSB_OUT_CFG_HPP
#define UAVIONIX_ADSB_OUT_CFG_HPP

#include <uORB/topics/parameter_update.h>

class MavlinkStreamUavionixADSBOutCfg : public ModuleParams, public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamUavionixADSBOutCfg(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "UAVIONIX_ADSB_OUT_CFG";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_UAVIONIX_ADSB_OUT_CFG;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    bool const_rate() override {
        return true;
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_UAVIONIX_ADSB_OUT_CFG_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamUavionixADSBOutCfg(Mavlink *mavlink) :
        ModuleParams(nullptr), MavlinkStream(mavlink) {
        param_t param_fw_airspd_stall = param_find("FW_AIRSPD_STALL");

        if (param_fw_airspd_stall != PARAM_INVALID) {
            float fw_airspd_stall;

            if (param_get(param_fw_airspd_stall, &fw_airspd_stall) == PX4_OK) {
                _stall_speed = static_cast<uint16_t>(fw_airspd_stall * 100.0f); // convert [m/s] to [cm/s]
            }
        }

        int32_t callsign_part1 = _adsb_callsign_part1.get();
        int32_t callsign_part2 = _adsb_callsign_part2.get();

        bool success = false;

        if (copy_and_check_callsign(_callsign, callsign_part1)) {
            if (copy_and_check_callsign(_callsign + sizeof(int32_t), callsign_part2)) {
                success = true;
            }
        }

        if (success) {
            _callsign[sizeof(_callsign) - 1] = '\0';

        } else {
            _callsign[0] = '\0';
            events::send(events::ID("uavionix_adsb_out_cfg_unsupported_callsign_format"),
                         events::Log::Critical, "Unsupported callsign format");
        }
    }

    DEFINE_PARAMETERS(
        (ParamInt<params_id::ADSB_ICAO_ID>)_adsb_icao,
        (ParamInt<params_id::ADSB_LEN_WIDTH>)_adsb_len_width,
        (ParamInt<params_id::ADSB_EMIT_TYPE>)_adsb_emit_type,
        (ParamInt<params_id::ADSB_GPS_OFF_LAT>)_adsb_gps_offset_lat,
        (ParamInt<params_id::ADSB_GPS_OFF_LON>)_adsb_gps_offset_lon,
        (ParamInt<params_id::ADSB_CALLSIGN_1>)_adsb_callsign_part1,
        (ParamInt<params_id::ADSB_CALLSIGN_2>)_adsb_callsign_part2);

    uint16_t _stall_speed{0}; // [cm/s]
    char     _callsign[9];

    bool copy_and_check_callsign(char *ptr_callsign, int32_t part) {
        for (uint8_t i = 0; i < sizeof(int32_t); ++i) {
            char character = static_cast<char>(part >> (i * 8));

            if ((character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9') || character == ' ' || character == '\0') {
                ptr_callsign[sizeof(int32_t) - i - 1] = character;

            } else {
                return false;
            }
        }

        return true;
    }

    bool send() override {
        // Required update for static message is 0.1 [Hz]
        mavlink_uavionix_adsb_out_cfg_t cfg_msg = {
            .ICAO         = static_cast<uint32_t>(_adsb_icao.get()),
            .stallSpeed   = _stall_speed,
            .callsign     = {'\0'},
            .emitterType  = static_cast<uint8_t>(_adsb_emit_type.get()),
            .aircraftSize = static_cast<uint8_t>(_adsb_len_width.get()),
            .gpsOffsetLat = static_cast<uint8_t>(_adsb_gps_offset_lat.get()),
            .gpsOffsetLon = static_cast<uint8_t>(_adsb_gps_offset_lon.get()),
            .rfSelect     = UAVIONIX_ADSB_OUT_RF_SELECT_TX_ENABLED};

        static_assert(sizeof(cfg_msg.callsign) == sizeof(_callsign), "Size mismatch");
        rt_memcpy(cfg_msg.callsign, _callsign, sizeof(cfg_msg.callsign));

        mavlink_msg_uavionix_adsb_out_cfg_send_struct(_mavlink->get_channel(), &cfg_msg);

        return true;
    }
};

#endif // UAVIONIX_ADSB_OUT_CFG_HPP
