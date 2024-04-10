/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __GETOPT_H__
#define __GETOPT_H__

/**
 * @file getopt.h
 * Thread safe version of getopt
 */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

int getopt(int argc, char *argv[], const char *options, int *myoptind, const char **myoptarg);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __GETOPT_H__
