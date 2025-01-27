
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file RadioStatus.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT radio_status_s {
#else
struct radio_status_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint16_t rxerrors;
	uint16_t fix;
	uint8_t rssi;
	uint8_t remote_rssi;
	uint8_t txbuf;
	uint8_t noise;
	uint8_t remote_noise;
	uint8_t _padding0[7]; // required for logger

#ifdef __cplusplus
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using RadioStatus = radio_status_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(radio_status);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const radio_status_s& message);
#endif // __cplusplus
