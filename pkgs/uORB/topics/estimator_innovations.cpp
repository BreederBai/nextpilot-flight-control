
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file EstimatorInnovations.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/estimator_innovations.h>

constexpr char __orb_estimator_innovations_fields[] = "\x89 timestamp;\x89 timestamp_sample;\x8a[2] gps_hvel;\x8a gps_vvel;\x8a[2] gps_hpos;\x8a gps_vpos;\x8a[2] ev_hvel;\x8a ev_vvel;\x8a[2] ev_hpos;\x8a ev_vpos;\x8a rng_vpos;\x8a baro_vpos;\x8a[2] aux_hvel;\x8a aux_vvel;\x8a[2] flow;\x8a[2] terr_flow;\x8a heading;\x8a[3] mag_field;\x8a[3] gravity;\x8a[2] drag;\x8a airspeed;\x8a beta;\x8a hagl;\x8a hagl_rate;";

ORB_DEFINE(estimator_innovations, struct estimator_innovations_s, 152, __orb_estimator_innovations_fields, static_cast<uint8_t>(ORB_ID::estimator_innovations));
ORB_DEFINE(estimator_innovation_variances, struct estimator_innovations_s, 152, __orb_estimator_innovations_fields, static_cast<uint8_t>(ORB_ID::estimator_innovation_variances));
ORB_DEFINE(estimator_innovation_test_ratios, struct estimator_innovations_s, 152, __orb_estimator_innovations_fields, static_cast<uint8_t>(ORB_ID::estimator_innovation_test_ratios));


void print_message(const orb_metadata *meta, const estimator_innovations_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
