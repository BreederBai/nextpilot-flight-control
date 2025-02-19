/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/
#define LOG_TAG "sdcard_check"
#define LOG_LVL LOG_LVL_INFO


#include "sdcardCheck.hpp"
#include <dirent.h>
#include <string.h>

#ifdef __PX4_DARWIN
#   include <sys/param.h>
#   include <sys/mount.h>
#elif defined(PX4_STORAGEDIR)
#   include <sys/statfs.h>
#endif

void SdCardChecks::checkAndReport(const Context &context, Report &reporter) {
#ifdef PX4_STORAGEDIR

    if (_param_com_arm_sdcard.get() > 0) {
        struct statfs statfs_buf;

        if (!_sdcard_detected && statfs(PX4_STORAGEDIR, &statfs_buf) == 0) {
            // on NuttX we get a data block count f_blocks and byte count per block f_bsize if an SD card is inserted
            _sdcard_detected = (statfs_buf.f_blocks > 0) && (statfs_buf.f_bsize > 0);
        }

        if (!_sdcard_detected) {
            NavModes affected_modes{NavModes::None};

            if (_param_com_arm_sdcard.get() == 2) {
                // disallow arming without sd card
                affected_modes = NavModes::All;
            }

            /* EVENT
             * @description
             * Insert an SD Card to the autopilot and reboot the system.
             *
             * <profile name="dev">
             * This check can be configured via <param>COM_ARM_SDCARD</param> parameter.
             * </profile>
             */
            reporter.armingCheckFailure(affected_modes, health_component_t::system,
                                        events::ID("check_missing_fmu_sdcard"),
                                        events::Log::Error, "Missing FMU SD Card");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Missing FMU SD Card");
            }
        }
    }

#   ifdef __PX4_NUTTX
    // Check for hardfault files

    if (!_hardfault_checked_once && _param_com_arm_hardfault_check.get()) {
        _hardfault_checked_once = true;

        DIR *dp = opendir(PX4_STORAGEDIR);

        if (dp != nullptr) {
            struct dirent *result;

            while ((result = readdir(dp)) && !_hardfault_file_present) {
                // Check for pattern fault_*.log
                if (strncmp("fault_", result->d_name, 6) == 0 && strcmp(result->d_name + strlen(result->d_name) - 4, ".log") == 0) {
                    _hardfault_file_present = true;
                }
            }

            closedir(dp);
        }
    }

    if (_hardfault_file_present && _param_com_arm_hardfault_check.get()) {
        /* EVENT
         * @description
         * The SD card contains crash dump files, service the vehicle before continuing to fly.
         *
         * <profile name="dev">
         * Check how to debug hardfaults on <a>https://docs.px4.io/main/en/debug/gdb_debugging.html#hard-fault-debugging</a>.
         * When completed, remove the files 'fault_*.log' on the SD card.
         *
         * This check can be configured via <param>COM_ARM_HFLT_CHK</param> parameter.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::system,
                               events::ID("check_hardfault_present"),
                               events::Log::Error, "Crash dumps present on SD card");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Crash dumps present on SD, vehicle needs service");
        }
    }

#   endif /* __PX4_NUTTX */
#endif    /* PX4_STORAGEDIR */
}
