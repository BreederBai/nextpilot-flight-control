
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file VehicleConstraints.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/vehicle_constraints.h>

constexpr char __orb_vehicle_constraints_fields[] = "\x89 timestamp;\x8a speed_up;\x8a speed_down;\x8c want_takeoff;\x86[7] _padding0;";

ORB_DEFINE(vehicle_constraints, struct vehicle_constraints_s, 17, __orb_vehicle_constraints_fields, static_cast<uint8_t>(ORB_ID::vehicle_constraints));


void print_message(const orb_metadata *meta, const vehicle_constraints_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
