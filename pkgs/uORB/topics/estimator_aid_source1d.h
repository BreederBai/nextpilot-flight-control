
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file EstimatorAidSource1d.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT estimator_aid_source1d_s {
#else
struct estimator_aid_source1d_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint64_t timestamp_sample;
	uint64_t time_last_fuse;
	uint32_t device_id;
	float observation;
	float observation_variance;
	float innovation;
	float innovation_variance;
	float test_ratio;
	uint8_t estimator_instance;
	bool fusion_enabled;
	bool innovation_rejected;
	bool fused;
	uint8_t _padding0[4]; // required for logger

#ifdef __cplusplus
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using EstimatorAidSource1d = estimator_aid_source1d_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(estimator_aid_src_baro_hgt);
ORB_DECLARE(estimator_aid_src_ev_hgt);
ORB_DECLARE(estimator_aid_src_gnss_hgt);
ORB_DECLARE(estimator_aid_src_rng_hgt);
ORB_DECLARE(estimator_aid_src_airspeed);
ORB_DECLARE(estimator_aid_src_sideslip);
ORB_DECLARE(estimator_aid_src_fake_hgt);
ORB_DECLARE(estimator_aid_src_mag_heading);
ORB_DECLARE(estimator_aid_src_gnss_yaw);
ORB_DECLARE(estimator_aid_src_ev_yaw);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const estimator_aid_source1d_s& message);
#endif // __cplusplus
