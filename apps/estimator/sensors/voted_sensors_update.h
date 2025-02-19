/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

/**
 * @file voted_sensors_update.h
 *
 * @author Beat Kueng <beat-kueng@gmx.net>
 */

#include "data_validator/DataValidator.hpp"
#include "data_validator/DataValidatorGroup.hpp"

#include <events/events.h>
#include <module/module_params.hpp>
#include <hrtimer.h>
#include <mathlib/mathlib.h>
#include <matrix/math.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/sensor_accel.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/sensors_status_imu.h>
#include <uORB/topics/sensor_selection.h>
#include <uORB/topics/vehicle_imu.h>
#include <uORB/topics/vehicle_imu_status.h>

using namespace nextpilot;
using namespace nextpilot::param;

namespace sensors {

static constexpr uint8_t MAX_SENSOR_COUNT = 4;

/**
 ** class VotedSensorsUpdate
 *
 * Handling of sensor updates with voting
 */
class VotedSensorsUpdate : public ModuleParams {
public:
    /**
     * @param parameters parameter values. These do not have to be initialized when constructing this object.
     * Only when calling init(), they have to be initialized.
     */
    VotedSensorsUpdate(bool hil_enabled, uORB::SubscriptionCallbackWorkItem (&vehicle_imu_sub)[MAX_SENSOR_COUNT]);

    /**
     * This tries to find new sensor instances. This is called from init(), then it can be called periodically.
     */
    void initializeSensors();

    void printStatus();

    /**
     * call this whenever parameters got updated. Make sure to have initializeSensors() called at least
     * once before calling this.
     */
    void parametersUpdate();

    /**
     * read new sensor data
     */
    void sensorsPoll(sensor_combined_s &raw);

    /**
     * set the relative timestamps of each sensor timestamp, based on the last sensorsPoll,
     * so that the data can be published.
     */
    void setRelativeTimestamps(sensor_combined_s &raw);

private:
    static constexpr uint8_t DEFAULT_PRIORITY = 50;

    struct SensorData {
        SensorData() = delete;

        explicit SensorData(ORB_ID meta) :
            subscription{{meta, 0}, {meta, 1}, {meta, 2}, {meta, 3}} {
        }

        uORB::Subscription subscription[MAX_SENSOR_COUNT]; /**< raw sensor data subscription */
        DataValidatorGroup voter{1};
        unsigned int       last_failover_count{0};
        int32_t            priority[MAX_SENSOR_COUNT]{};
        int32_t            priority_configured[MAX_SENSOR_COUNT]{};
        uint8_t            last_best_vote{0}; /**< index of the latest best vote */
        uint8_t            subscription_count{0};
        bool               advertised[MAX_SENSOR_COUNT]{false, false, false};
    };

    void initSensorClass(SensorData &sensor_data, uint8_t sensor_count_max);

    /**
     * Poll IMU for updated data.
     *
     * @param raw	Combined sensor data structure into which
     *		data should be returned.
     */
    void imuPoll(sensor_combined_s &raw);

    /**
     * Check & handle failover of a sensor
     * @return true if a switch occured (could be for a non-critical reason)
     */
    bool checkFailover(SensorData &sensor, const char *sensor_name, events::px4::enums::sensor_type_t sensor_type);

    /**
     * Calculates the magnitude in m/s/s of the largest difference between each accelerometer vector and the mean of all vectors
     */
    void calcAccelInconsistency();

    /**
     * Calculates the magnitude in rad/s of the largest difference between each gyro vector and the mean of all vectors
     */
    void calcGyroInconsistency();

    SensorData _accel{ORB_ID::sensor_accel};
    SensorData _gyro{ORB_ID::sensor_gyro};

    hrt_abstime  _last_error_message{0};
    orb_advert_t _mavlink_log_pub{nullptr};

    uORB::Publication<sensor_selection_s>   _sensor_selection_pub{ORB_ID(sensor_selection)}; /**< handle to the sensor selection uORB topic */
    uORB::Publication<sensors_status_imu_s> _sensors_status_imu_pub{ORB_ID(sensors_status_imu)};

    uORB::SubscriptionCallbackWorkItem (&_vehicle_imu_sub)[MAX_SENSOR_COUNT];
    uORB::SubscriptionMultiArray<vehicle_imu_status_s, MAX_SENSOR_COUNT> _vehicle_imu_status_subs{ORB_ID::vehicle_imu_status};

    uORB::Subscription _sensor_selection_sub{ORB_ID(sensor_selection)};

    sensor_combined_s _last_sensor_data[MAX_SENSOR_COUNT]{}; /**< latest sensor data from all sensors instances */

    const bool _hil_enabled{false};                          /**< is hardware-in-the-loop mode enabled? */

    bool _selection_changed{true};                           /**< true when a sensor selection has changed and not been published */

    matrix::Vector3f _accel_diff[MAX_SENSOR_COUNT]{};        /**< filtered accel differences between IMU units (m/s/s) */
    matrix::Vector3f _gyro_diff[MAX_SENSOR_COUNT]{};         /**< filtered gyro differences between IMU uinits (rad/s) */

    uint32_t _accel_device_id[MAX_SENSOR_COUNT]{};           /**< accel driver device id for each uorb instance */
    uint32_t _gyro_device_id[MAX_SENSOR_COUNT]{};            /**< gyro driver device id for each uorb instance */

    uint64_t _last_accel_timestamp[MAX_SENSOR_COUNT]{};      /**< latest full timestamp */

    sensor_selection_s _selection{};                         /**< struct containing the sensor selection to be published to the uORB */

    bool _parameter_update{false};

    DEFINE_PARAMETERS(
        (ParamBool<params_id::SENS_IMU_MODE>)_param_sens_imu_mode)
};

} /* namespace sensors */
