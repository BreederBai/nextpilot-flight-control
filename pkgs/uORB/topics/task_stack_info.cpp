
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file TaskStackInfo.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/task_stack_info.h>

constexpr char __orb_task_stack_info_fields[] = "\x89 timestamp;\x87 stack_free;\x8d[24] task_name;\x86[6] _padding0;";

ORB_DEFINE(task_stack_info, struct task_stack_info_s, 34, __orb_task_stack_info_fields, static_cast<uint8_t>(ORB_ID::task_stack_info));


void print_message(const orb_metadata *meta, const task_stack_info_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
