/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __NEXTPILOT_PX4_H__
#define __NEXTPILOT_PX4_H__

#include <rtdbg.h>

#define PX4_INFO     LOG_I
#define PX4_PANIC    LOG_I
#define PX4_ERR      LOG_E
#define PX4_WARN     LOG_W
#define PX4_DEBUG    LOG_D
#define PX4_INFO_RAW LOG_RAW
#define PX4_LOG_NAMED_COND(name, cond, fmt, ...) \
    do {                                         \
        if (cond) {                              \
            LOG_I(fmt, ##__VA_ARGS__);           \
        }                                        \
    } while (0);

#define PX4_ERROR (-1)
#define PX4_OK    0

#ifndef ERROR
// #define ERROR (-1)
#endif // ERROR

#ifndef OK
#   define OK 0
#endif // OK

#define px4_usleep usleep

#ifndef PX4_ISFINITE
#   define PX4_ISFINITE rt_isfinite
#endif // PX4_ISFINITE

#define PX4_STORAGEDIR ""
#define PX4_ROOTFSDIR  ""

#endif // __NEXTPILOT_PX4_H__
