
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file ActuatorServosTrim.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/actuator_servos_trim.h>

constexpr char __orb_actuator_servos_trim_fields[] = "\x89 timestamp;\x8a[8] trim;";

ORB_DEFINE(actuator_servos_trim, struct actuator_servos_trim_s, 40, __orb_actuator_servos_trim_fields, static_cast<uint8_t>(ORB_ID::actuator_servos_trim));


void print_message(const orb_metadata *meta, const actuator_servos_trim_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
