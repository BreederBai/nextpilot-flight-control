
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file SensorGps.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#define SENSOR_GPS_JAMMING_STATE_UNKNOWN 0
#define SENSOR_GPS_JAMMING_STATE_OK 1
#define SENSOR_GPS_JAMMING_STATE_WARNING 2
#define SENSOR_GPS_JAMMING_STATE_CRITICAL 3
#define SENSOR_GPS_SPOOFING_STATE_UNKNOWN 0
#define SENSOR_GPS_SPOOFING_STATE_NONE 1
#define SENSOR_GPS_SPOOFING_STATE_INDICATED 2
#define SENSOR_GPS_SPOOFING_STATE_MULTIPLE 3
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT sensor_gps_s {
#else
struct sensor_gps_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint64_t timestamp_sample;
	uint64_t time_utc_usec;
	uint32_t device_id;
	int32_t lat;
	int32_t lon;
	int32_t alt;
	int32_t alt_ellipsoid;
	float s_variance_m_s;
	float c_variance_rad;
	float eph;
	float epv;
	float hdop;
	float vdop;
	int32_t noise_per_ms;
	int32_t jamming_indicator;
	float vel_m_s;
	float vel_n_m_s;
	float vel_e_m_s;
	float vel_d_m_s;
	float cog_rad;
	int32_t timestamp_time_relative;
	float heading;
	float heading_offset;
	float heading_accuracy;
	float rtcm_injection_rate;
	uint16_t automatic_gain_control;
	uint8_t fix_type;
	uint8_t jamming_state;
	uint8_t spoofing_state;
	bool vel_ned_valid;
	uint8_t satellites_used;
	uint8_t selected_rtcm_instance;
	uint8_t _padding0[4]; // required for logger

#ifdef __cplusplus
	static constexpr uint8_t JAMMING_STATE_UNKNOWN = 0;
	static constexpr uint8_t JAMMING_STATE_OK = 1;
	static constexpr uint8_t JAMMING_STATE_WARNING = 2;
	static constexpr uint8_t JAMMING_STATE_CRITICAL = 3;
	static constexpr uint8_t SPOOFING_STATE_UNKNOWN = 0;
	static constexpr uint8_t SPOOFING_STATE_NONE = 1;
	static constexpr uint8_t SPOOFING_STATE_INDICATED = 2;
	static constexpr uint8_t SPOOFING_STATE_MULTIPLE = 3;
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using SensorGps = sensor_gps_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(sensor_gps);
ORB_DECLARE(vehicle_gps_position);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const sensor_gps_s& message);
#endif // __cplusplus
