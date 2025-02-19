#!/usr/bin/env python3
# /*****************************************************************
#  *     _   __             __   ____   _  __        __
#  *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
#  *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
#  *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
#  * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
#  *
#  * Copyright All Reserved © 2015-2024 NextPilot Development Team
#  ******************************************************************/

import json
import sys
import urllib.request

SAMPLING_RES = 5
SAMPLING_MIN_LAT = -50
SAMPLING_MAX_LAT = 60
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

header = """/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <gtest/gtest.h>
#include <math.h>
#include <mathlib/mathlib.h>

#include "geo_mag_declination.h"
"""

print(header)

print("")

# Declination
params = urllib.parse.urlencode(
    {
        "lat1": 0,
        "lat2": 0,
        "lon1": 0,
        "lon2": 0,
        "latStepSize": 1,
        "lonStepSize": 1,
        "magneticComponent": "d",
        "resultFormat": "json",
    }
)
key = sys.argv[1]  # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?key=%s&%s" % (key, params))
data = json.loads(f.read())


print("TEST(GeoLookupTest, declination)\n{")
for latitude in range(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT + 1, SAMPLING_RES):
    params = urllib.parse.urlencode(
        {
            "lat1": latitude,
            "lat2": latitude,
            "lon1": SAMPLING_MIN_LON,
            "lon2": SAMPLING_MAX_LON,
            "latStepSize": 1,
            "lonStepSize": SAMPLING_RES,
            "magneticComponent": "d",
            "resultFormat": "json",
        }
    )
    f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?key=%s&%s" % (key, params))
    data = json.loads(f.read())

    for p in data["result"]:
        error = 1

        # thing start getting worse here
        if latitude <= -44:
            error = 2

        print(
            "\tEXPECT_NEAR(get_mag_declination_degrees({}, {}), {}, {} + {});".format(
                p["latitude"], p["longitude"], p["declination"], p["declination_uncertainty"], error
            )
        )
print("}")

print("")

print("TEST(GeoLookupTest, inclination)\n{")
for latitude in range(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT + 1, SAMPLING_RES):
    params = urllib.parse.urlencode(
        {
            "lat1": latitude,
            "lat2": latitude,
            "lon1": SAMPLING_MIN_LON,
            "lon2": SAMPLING_MAX_LON,
            "latStepSize": 1,
            "lonStepSize": SAMPLING_RES,
            "magneticComponent": "i",
            "resultFormat": "json",
        }
    )
    f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?key=%s&%s" % (key, params))
    data = json.loads(f.read())

    for p in data["result"]:
        error = 1.2

        # thing start getting worse here
        if latitude <= -44:
            error = 2

        print(
            "\tEXPECT_NEAR(get_mag_inclination_degrees({}, {}), {}, {} + {});".format(
                p["latitude"], p["longitude"], p["inclination"], p["inclination_uncertainty"], error
            )
        )
print("}")

print("")

print("TEST(GeoLookupTest, strength)\n{")
for latitude in range(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT + 1, SAMPLING_RES):
    params = urllib.parse.urlencode(
        {
            "lat1": latitude,
            "lat2": latitude,
            "lon1": SAMPLING_MIN_LON,
            "lon2": SAMPLING_MAX_LON,
            "latStepSize": 1,
            "lonStepSize": SAMPLING_RES,
            "magneticComponent": "f",
            "resultFormat": "json",
        }
    )
    f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?key=%s&%s" % (key, params))
    data = json.loads(f.read())

    for p in data["result"]:
        error = 500

        print(
            "\tEXPECT_NEAR(get_mag_strength_tesla({}, {}) * 1e9, {}, {} + {});".format(
                p["latitude"], p["longitude"], p["totalintensity"], p["totalintensity_uncertainty"], error
            )
        )
print("}")
