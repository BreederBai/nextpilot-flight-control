/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file spektrum_rssi.h
 *
 * RSSI dBm to percentage conversion for Spektrum telemetry receivers
 *
 * @author Kurt Kiefer <kekiefer@gmail.com>
 */

#pragma once

#include <climits>

/*
min_rssi_dbm = -92.0
max_rssi_dbm = -42.0

def dbm_to_percent(abs_dbm):
	if -abs_dbm < min_rssi_dbm:
		return 0.0
	elif -abs_dbm > max_rssi_dbm:
		return 100.0
	else:
		return 100.0 * math.log10(1.0 + (float)(-abs_dbm - min_rssi_dbm) * (9.0 / (max_rssi_dbm - min_rssi_dbm)))
*/
constexpr int8_t lu_dbm_percent[] = {
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99,  98,  98,  97,  96,
	95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  79,
	78,  77,  75,  74,  73,  71,  70,  68,  66,  65,  63,  61,  59,  57,  55,  52,
	50,  47,  45,  42,  39,  35,  32,  28,  24,  19,  13,  7,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,
};

#define MASK(n) (n >> (sizeof(n) * CHAR_BIT - 1))

/* constexpr-compatible version of abs() */
constexpr unsigned c_abs(int n)
{
	return (n + MASK(n)) ^ MASK(n);
}

/* convert signed spektrum dbm (-92 to -42) to percentage */
constexpr int8_t spek_dbm_to_percent(int8_t dbm)
{
	return lu_dbm_percent[c_abs(dbm)];
}

static_assert(spek_dbm_to_percent(0) == 100, "0 dbm should be 100%");
static_assert(spek_dbm_to_percent(-42) == 100, "-42 dbm should be 100%");
static_assert(spek_dbm_to_percent(-80) == 50, "-80 dbm should be 50%");
static_assert(spek_dbm_to_percent(-92) == 0, "-92 dbm should be 0%");
static_assert(spek_dbm_to_percent(-128) == 0, "-128 dbm should be 0%");
