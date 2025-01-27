
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file HoverThrustEstimate.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/hover_thrust_estimate.h>

constexpr char __orb_hover_thrust_estimate_fields[] = "\x89 timestamp;\x89 timestamp_sample;\x8a hover_thrust;\x8a hover_thrust_var;\x8a accel_innov;\x8a accel_innov_var;\x8a accel_innov_test_ratio;\x8a accel_noise_var;\x8c valid;\x86[7] _padding0;";

ORB_DEFINE(hover_thrust_estimate, struct hover_thrust_estimate_s, 41, __orb_hover_thrust_estimate_fields, static_cast<uint8_t>(ORB_ID::hover_thrust_estimate));


void print_message(const orb_metadata *meta, const hover_thrust_estimate_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
