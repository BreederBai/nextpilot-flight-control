
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file CameraTrigger.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#define CAMERA_TRIGGER_ORB_QUEUE_LENGTH 2
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT camera_trigger_s {
#else
struct camera_trigger_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint64_t timestamp_utc;
	uint32_t seq;
	bool feedback;
	uint8_t _padding0[3]; // required for logger

#ifdef __cplusplus
	static constexpr uint32_t ORB_QUEUE_LENGTH = 2;
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using CameraTrigger = camera_trigger_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(camera_trigger);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const camera_trigger_s& message);
#endif // __cplusplus
