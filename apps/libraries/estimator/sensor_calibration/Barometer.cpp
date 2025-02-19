/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "Barometer.hpp"

#include "Utilities.hpp"

#include <param/param.h>

using namespace matrix;
using namespace time_literals;

namespace calibration
{

Barometer::Barometer()
{
	Reset();
}

Barometer::Barometer(uint32_t device_id)
{
	set_device_id(device_id);
}

void Barometer::set_device_id(uint32_t device_id)
{
	bool external = DeviceExternal(device_id);

	if (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(true);
	}
}

void Barometer::SensorCorrectionsUpdate(bool force)
{
	// check if the selected sensor has updated
	if (_sensor_correction_sub.updated() || force) {

		// valid device id required
		if (_device_id == 0) {
			return;
		}

		sensor_correction_s corrections;

		if (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			for (int i = 0; i < MAX_SENSOR_COUNT; i++) {
				if (corrections.baro_device_ids[i] == _device_id) {
					switch (i) {
					case 0:
						_thermal_offset = corrections.baro_offset_0;
						return;

					case 1:
						_thermal_offset = corrections.baro_offset_1;
						return;

					case 2:
						_thermal_offset = corrections.baro_offset_2;
						return;

					case 3:
						_thermal_offset = corrections.baro_offset_3;
						return;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset = 0;
	}
}

bool Barometer::set_offset(const float &offset)
{
	if (fabsf(_offset - offset) > 0.01f) {
		if (PX4_ISFINITE(offset)) {
			_offset = offset;
			_calibration_count++;
			return true;
		}
	}

	return false;
}

bool Barometer::set_calibration_index(int calibration_index)
{
	if ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_COUNT)) {
		_calibration_index = calibration_index;
		return true;
	}

	return false;
}

void Barometer::ParametersUpdate()
{
	if (_device_id == 0) {
		return;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	if (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} else {
		ParametersLoad();
	}
}

bool Barometer::ParametersLoad()
{
	if (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// CAL_BAROx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		if ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			static constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			if (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_BAROx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		return true;
	}

	return false;
}

void Barometer::Reset()
{
	_offset = 0;

	_thermal_offset = 0;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	_calibration_count = 0;
}

bool Barometer::ParametersSave(int desired_calibration_index, bool force)
{
	if (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_COUNT) {
		_calibration_index = desired_calibration_index;

	} else if (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		if (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	if (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// save calibration
		bool success = true;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		return success;
	}

	return false;
}

void Barometer::PrintStatus()
{
	if (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: %05.3f, Ext\n",
			     SensorString(), device_id(), enabled(),
			     (double)_offset);

	} else {
		PX4_INFO_RAW("%s %" PRIu32 " EN: %d, offset: %05.3f, Internal\n",
			     SensorString(), device_id(), enabled(),
			     (double)_offset);
	}

	if (fabsf(_thermal_offset) > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: %.4f\n", SensorString(), _device_id, (double)_thermal_offset);
	}
}

} // namespace calibration
