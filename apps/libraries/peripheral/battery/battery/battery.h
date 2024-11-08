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
 * @file battery.h
 *
 * Library calls for battery functionality.
 *
 * @author Julian Oes <julian@oes.ch>
 * @author Timothy Scott <timothy@auterion.com>
 */

#pragma once

#include <math.h>
#include <float.h>

// #include <board_config.h>
#include <module/module_params.hpp>
#include <matrix/math.hpp>

#include <hrtimer.h>
#include <param/param.h>
#include <mathlib/math/filter/AlphaFilter.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/vehicle_thrust_setpoint.h>

using namespace nextpilot;

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : public ModuleParams
{
public:
	Battery(int index, ModuleParams *parent, const int sample_interval_us, const uint8_t source);
	~Battery() = default;

	/**
	 * Get the battery cell count
	 */
	int cell_count() { return _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	float empty_cell_voltage() { return _params.v_empty; }

	/**
	 * Get the full voltage per cell
	 */
	float full_cell_voltage() { return _params.v_charged; }

	void setPriority(const uint8_t priority) { _priority = priority; }
	void setConnected(const bool connected) { _connected = connected; }
	void setStateOfCharge(const float soc) { _state_of_charge = soc; _external_state_of_charge = true; }
	void updateVoltage(const float voltage_v);
	void updateCurrent(const float current_a);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	void updateBatteryStatus(const hrt_abstime &timestamp);

	battery_status_s getBatteryStatus();
	void publishBatteryStatus(const battery_status_s &battery_status);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	void updateAndPublishBatteryStatus(const hrt_abstime &timestamp);

protected:
	static constexpr float LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	struct {
		param_t v_empty;
		param_t v_charged;
		param_t n_cells;
		param_t capacity;
		param_t v_load_drop;
		param_t r_internal;
		param_t low_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t source;
		param_t bat_avrg_current;
	} _param_handles{};

	struct {
		float v_empty;
		float v_charged;
		int32_t  n_cells;
		float capacity;
		float v_load_drop;
		float r_internal;
		float low_thr;
		float crit_thr;
		float emergen_thr;
		int32_t source;
		float bat_avrg_current;
	} _params{};

	const int _index;

	bool _first_parameter_update{true};
	void updateParams() override;

private:
	void sumDischarged(const hrt_abstime &timestamp, float current_a);
	float calculateStateOfChargeVoltageBased(const float voltage_v, const float current_a);
	void estimateStateOfCharge();
	uint8_t determineWarning(float state_of_charge);
	void computeScale();
	float computeRemainingTime(float current_a);

	uORB::Subscription _vehicle_thrust_setpoint_0_sub{ORB_ID(vehicle_thrust_setpoint)};
	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
	uORB::PublicationMulti<battery_status_s> _battery_status_pub{ORB_ID(battery_status)};

	bool _external_state_of_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	const uint8_t _source;
	uint8_t _priority{0};
	bool _battery_initialized{false};
	float _voltage_v{0.f};
	AlphaFilter<float> _voltage_filter_v;
	float _current_a{-1};
	AlphaFilter<float> _current_filter_a;
	AlphaFilter<float> _current_average_filter_a;
	AlphaFilter<float> _throttle_filter;
	float _discharged_mah{0.f};
	float _discharged_mah_loop{0.f};
	float _state_of_charge_volt_based{-1.f}; // [0,1]
	float _state_of_charge{-1.f}; // [0,1]
	float _scale{1.f};
	uint8_t _warning{battery_status_s::BATTERY_WARNING_NONE};
	hrt_abstime _last_timestamp{0};
	bool _armed{false};
	hrt_abstime _last_unconnected_timestamp{0};
};
