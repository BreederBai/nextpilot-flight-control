
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file InputRc.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/input_rc.h>

constexpr char __orb_input_rc_fields[] = "\x89 timestamp;\x89 timestamp_last_signal;\x84 rssi;\x8a rssi_dbm;\x87 rc_lost_frame_count;\x87 rc_total_frame_count;\x87 rc_ppm_frame_length;\x87[18] values;\x86 channel_count;\x8c rc_failsafe;\x8c rc_lost;\x86 input_source;\x82 link_quality;\x86[1] _padding0;";

ORB_DEFINE(input_rc, struct input_rc_s, 71, __orb_input_rc_fields, static_cast<uint8_t>(ORB_ID::input_rc));


void print_message(const orb_metadata *meta, const input_rc_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
