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
 * @file airspeed.c
 * Airspeed estimation
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 *
 */

#include "airspeed.h"

#include <defines.h>
#include <geo/geo.h>

float calc_IAS_corrected(enum AIRSPEED_COMPENSATION_MODEL pmodel, enum AIRSPEED_SENSOR_MODEL smodel,
			 float tube_len, float tube_dia_mm, float differential_pressure, float pressure_ambient, float temperature_celsius)
{
	if (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	// air density in kg/m3
	const float rho_air = get_air_density(pressure_ambient, temperature_celsius);

	const float dp = fabsf(differential_pressure);
	float dp_tot = dp;

	float dv = 0.0f;

	switch (smodel) {

	case AIRSPEED_SENSOR_MODEL_MEMBRANE: {
			// do nothing
		}
		break;

	case AIRSPEED_SENSOR_MODEL_SDP3X: {
			// assumes a metal pitot tube with round tip as here: https://drotek.com/shop/2986-large_default/sdp3x-airspeed-sensor-kit-sdp31.jpg
			// and tubing as provided by px4/drotek (1.5 mm diameter)
			// The tube_len represents the length of the tubes connecting the pitot to the sensor.
			switch (pmodel) {
			case AIRSPEED_COMPENSATION_MODEL_PITOT:
			case AIRSPEED_COMPENSATION_MODEL_NO_PITOT: {
					const float dp_corr = dp * 96600.0f / pressure_ambient;
					// flow through sensor
					float flow_SDP33 = (300.805f - 300.878f / (0.00344205f * powf(dp_corr, 0.68698f) + 1.0f)) * 1.29f / rho_air;

					// for too small readings the compensation might result in a negative flow which causes numerical issues
					if (flow_SDP33 < 0.0f) {
						flow_SDP33 = 0.0f;
					}

					float dp_pitot = 0.0f;

					switch (pmodel) {
					case AIRSPEED_COMPENSATION_MODEL_PITOT:
						dp_pitot = (0.0032f * flow_SDP33 * flow_SDP33 + 0.0123f * flow_SDP33 + 1.0f) * 1.29f / rho_air;
						break;

					default:
						// do nothing
						break;
					}

					// pressure drop through tube
					const float dp_tube = (flow_SDP33 * 0.674f) / 450.0f * tube_len * rho_air / 1.29f;

					// speed at pitot-tube tip due to flow through sensor
					dv = 0.125f * flow_SDP33;

					// sum of all pressure drops
					dp_tot = dp_corr + dp_tube + dp_pitot;
				}
				break;

			case AIRSPEED_COMPENSATION_TUBE_PRESSURE_LOSS: {
					// Pressure loss compensation as defined in https://goo.gl/UHV1Vv.
					// tube_dia_mm: Diameter in mm of the pitot and tubes, must have the same diameter.
					// tube_len: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.

					// check if the tube diameter and dp is nonzero to avoid division by 0
					if ((tube_dia_mm > 0.0f) && (dp > 0.0f)) {
						const float d_tubePow4 = powf(tube_dia_mm * 1e-3f, 4);
						const float denominator = M_PI_F * d_tubePow4 * rho_air * dp;

						// avoid division by 0
						float eps = 0.0f;

						if (fabsf(denominator) > 1e-32f) {
							const float viscosity = (18.205f + 0.0484f * (temperature_celsius - 20.0f)) * 1e-6f;

							// 4.79 * 1e-7 -> mass flow through sensor
							// 59.5 -> dp sensor constant where linear and quadratic contribution to dp vs flow is equal
							eps = -64.0f * tube_len * viscosity * 4.79f * 1e-7f * (sqrtf(1.0f + 8.0f * dp / 59.3319f) - 1.0f) / denominator;
						}

						// range check on eps
						if (fabsf(eps) >= 1.0f) {
							eps = 0.0f;
						}

						// pressure correction
						dp_tot = dp / (1.0f + eps);
					}
				}
				break;

			default: {
					// do nothing
				}
				break;
			}

		}
		break;

	default: {
			// do nothing
		}
		break;
	}

	// if (!PX4_ISFINITE(dp_tube)) {
	// 	dp_tube = 0.0f;
	// }

	// if (!PX4_ISFINITE(dp_pitot)) {
	// 	dp_pitot = 0.0f;
	// }

	// if (!PX4_ISFINITE(dv)) {
	// 	dv = 0.0f;
	// }

	// computed airspeed without correction for inflow-speed at tip of pitot-tube
	const float airspeed_uncorrected = sqrtf(2.0f * dp_tot / CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);

	// corrected airspeed
	const float airspeed_corrected = airspeed_uncorrected + dv;

	// return result with correct sign
	return (differential_pressure > 0.0f) ? airspeed_corrected : -airspeed_corrected;
}

float calc_IAS(float differential_pressure)
{
	if (differential_pressure > 0.0f) {
		return sqrtf((2.0f * differential_pressure) / CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);

	} else {
		return -sqrtf((2.0f * fabsf(differential_pressure)) / CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);
	}

}

float calc_TAS_from_CAS(float speed_calibrated, float pressure_ambient, float temperature_celsius)
{
	if (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	return speed_calibrated * sqrtf(CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C / get_air_density(pressure_ambient,
					temperature_celsius));
}

float calc_CAS_from_IAS(float speed_indicated, float scale)
{
	return speed_indicated * scale;
}

float calc_TAS(float total_pressure, float static_pressure, float temperature_celsius)
{
	float density = get_air_density(static_pressure, temperature_celsius);

	if (density < 0.0001f || !PX4_ISFINITE(density)) {
		density = CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C;
	}

	float pressure_difference = total_pressure - static_pressure;

	if (pressure_difference > 0) {
		return sqrtf((2.0f * (pressure_difference)) / density);

	} else {
		return -sqrtf((2.0f * fabsf(pressure_difference)) / density);
	}
}

float get_air_density(float static_pressure, float temperature_celsius)
{
	if (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	return static_pressure / (CONSTANTS_AIR_GAS_CONST * (temperature_celsius - CONSTANTS_ABSOLUTE_NULL_CELSIUS));
}

float calc_calibrated_from_true_airspeed(float speed_true, float air_density)
{
	return speed_true * sqrtf(air_density / CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);
}
