
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file TuneControl.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#define TUNE_CONTROL_TUNE_ID_STOP 0
#define TUNE_CONTROL_TUNE_ID_STARTUP 1
#define TUNE_CONTROL_TUNE_ID_ERROR 2
#define TUNE_CONTROL_TUNE_ID_NOTIFY_POSITIVE 3
#define TUNE_CONTROL_TUNE_ID_NOTIFY_NEUTRAL 4
#define TUNE_CONTROL_TUNE_ID_NOTIFY_NEGATIVE 5
#define TUNE_CONTROL_TUNE_ID_ARMING_WARNING 6
#define TUNE_CONTROL_TUNE_ID_BATTERY_WARNING_SLOW 7
#define TUNE_CONTROL_TUNE_ID_BATTERY_WARNING_FAST 8
#define TUNE_CONTROL_TUNE_ID_GPS_WARNING 9
#define TUNE_CONTROL_TUNE_ID_ARMING_FAILURE 10
#define TUNE_CONTROL_TUNE_ID_PARACHUTE_RELEASE 11
#define TUNE_CONTROL_TUNE_ID_SINGLE_BEEP 12
#define TUNE_CONTROL_TUNE_ID_HOME_SET 13
#define TUNE_CONTROL_TUNE_ID_SD_INIT 14
#define TUNE_CONTROL_TUNE_ID_SD_ERROR 15
#define TUNE_CONTROL_TUNE_ID_PROG_PX4IO 16
#define TUNE_CONTROL_TUNE_ID_PROG_PX4IO_OK 17
#define TUNE_CONTROL_TUNE_ID_PROG_PX4IO_ERR 18
#define TUNE_CONTROL_TUNE_ID_POWER_OFF 19
#define TUNE_CONTROL_NUMBER_OF_TUNES 20
#define TUNE_CONTROL_VOLUME_LEVEL_MIN 0
#define TUNE_CONTROL_VOLUME_LEVEL_DEFAULT 20
#define TUNE_CONTROL_VOLUME_LEVEL_MAX 100
#define TUNE_CONTROL_ORB_QUEUE_LENGTH 4
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT tune_control_s {
#else
struct tune_control_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint32_t duration;
	uint32_t silence;
	uint16_t frequency;
	uint8_t tune_id;
	bool tune_override;
	uint8_t volume;
	uint8_t _padding0[3]; // required for logger

#ifdef __cplusplus
	static constexpr uint8_t TUNE_ID_STOP = 0;
	static constexpr uint8_t TUNE_ID_STARTUP = 1;
	static constexpr uint8_t TUNE_ID_ERROR = 2;
	static constexpr uint8_t TUNE_ID_NOTIFY_POSITIVE = 3;
	static constexpr uint8_t TUNE_ID_NOTIFY_NEUTRAL = 4;
	static constexpr uint8_t TUNE_ID_NOTIFY_NEGATIVE = 5;
	static constexpr uint8_t TUNE_ID_ARMING_WARNING = 6;
	static constexpr uint8_t TUNE_ID_BATTERY_WARNING_SLOW = 7;
	static constexpr uint8_t TUNE_ID_BATTERY_WARNING_FAST = 8;
	static constexpr uint8_t TUNE_ID_GPS_WARNING = 9;
	static constexpr uint8_t TUNE_ID_ARMING_FAILURE = 10;
	static constexpr uint8_t TUNE_ID_PARACHUTE_RELEASE = 11;
	static constexpr uint8_t TUNE_ID_SINGLE_BEEP = 12;
	static constexpr uint8_t TUNE_ID_HOME_SET = 13;
	static constexpr uint8_t TUNE_ID_SD_INIT = 14;
	static constexpr uint8_t TUNE_ID_SD_ERROR = 15;
	static constexpr uint8_t TUNE_ID_PROG_PX4IO = 16;
	static constexpr uint8_t TUNE_ID_PROG_PX4IO_OK = 17;
	static constexpr uint8_t TUNE_ID_PROG_PX4IO_ERR = 18;
	static constexpr uint8_t TUNE_ID_POWER_OFF = 19;
	static constexpr uint8_t NUMBER_OF_TUNES = 20;
	static constexpr uint8_t VOLUME_LEVEL_MIN = 0;
	static constexpr uint8_t VOLUME_LEVEL_DEFAULT = 20;
	static constexpr uint8_t VOLUME_LEVEL_MAX = 100;
	static constexpr uint8_t ORB_QUEUE_LENGTH = 4;
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using TuneControl = tune_control_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(tune_control);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const tune_control_s& message);
#endif // __cplusplus
