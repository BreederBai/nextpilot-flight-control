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

#include <Integrator.hpp>

#include <mathlib/math/Limits.hpp>
#include <mathlib/math/WelfordMean.hpp>
#include <mathlib/math/WelfordMeanVector.hpp>
#include <matrix/matrix/math.hpp>
#include <perf/perf_counter.h>
#include <sensor_calibration/Accelerometer.hpp>
#include <sensor_calibration/Gyroscope.hpp>
#include <ulog/log.h>
#include <module/module_params.hpp>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/estimator_sensor_bias.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_accel.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_imu.h>
#include <uORB/topics/vehicle_imu_status.h>

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::workq;
using namespace nextpilot::param;

namespace sensors {

class VehicleIMU : public ModuleParams, public WorkItemScheduled {
public:
    VehicleIMU() = delete;
    VehicleIMU(int instance, uint8_t accel_index, uint8_t gyro_index, const wq_config_t &config);

    ~VehicleIMU() override;

    bool Start();
    void Stop();

    void PrintStatus();

private:
    bool ParametersUpdate(bool force = false);
    bool Publish();
    void Run() override;

    bool UpdateAccel();
    bool UpdateGyro();

    void UpdateIntegratorConfiguration();

    inline void UpdateAccelVibrationMetrics(const matrix::Vector3f &acceleration);
    inline void UpdateGyroVibrationMetrics(const matrix::Vector3f &angular_velocity);

    void SensorCalibrationUpdate();
    void SensorCalibrationSaveAccel();
    void SensorCalibrationSaveGyro();

    // return the square of two floating point numbers
    static constexpr float sq(float var) {
        return var * var;
    }

    uORB::PublicationMulti<vehicle_imu_s>        _vehicle_imu_pub{ORB_ID(vehicle_imu)};
    uORB::PublicationMulti<vehicle_imu_status_s> _vehicle_imu_status_pub{ORB_ID(vehicle_imu_status)};

    static constexpr hrt_abstime kIMUStatusPublishingInterval{100_ms};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    // Used to check, save and use learned magnetometer biases
    uORB::SubscriptionMultiArray<estimator_sensor_bias_s> _estimator_sensor_bias_subs{ORB_ID::estimator_sensor_bias};

    uORB::Subscription                 _sensor_accel_sub;
    uORB::SubscriptionCallbackWorkItem _sensor_gyro_sub;

    uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};

    calibration::Accelerometer _accel_calibration{};
    calibration::Gyroscope     _gyro_calibration{};

    sensors::Integrator       _accel_integrator{};
    sensors::IntegratorConing _gyro_integrator{};

    uint32_t _imu_integration_interval_us{5000};

    hrt_abstime _accel_timestamp_sample_last{0};
    hrt_abstime _gyro_timestamp_sample_last{0};
    hrt_abstime _gyro_timestamp_last{0};

    math::WelfordMeanVector<float, 3> _raw_accel_mean{};
    math::WelfordMeanVector<float, 3> _raw_gyro_mean{};

    math::WelfordMean<float> _accel_mean_interval_us{};
    math::WelfordMean<float> _accel_fifo_mean_interval_us{};

    math::WelfordMean<float> _gyro_mean_interval_us{};
    math::WelfordMean<float> _gyro_fifo_mean_interval_us{};

    math::WelfordMean<float> _gyro_update_latency_mean_us{};
    math::WelfordMean<float> _gyro_publish_latency_mean_us{};

    float _accel_interval_us{NAN};
    float _gyro_interval_us{NAN};

    unsigned _accel_last_generation{0};
    unsigned _gyro_last_generation{0};

    float _accel_temperature_sum{NAN};
    float _gyro_temperature_sum{NAN};

    int _accel_temperature_sum_count{0};
    int _gyro_temperature_sum_count{0};

    matrix::Vector3f _acceleration_prev{};     // acceleration from the previous IMU measurement for vibration metrics
    matrix::Vector3f _angular_velocity_prev{}; // angular velocity from the previous IMU measurement for vibration metrics

    vehicle_imu_status_s _status{};

    float _coning_norm_accum{0};
    float _coning_norm_accum_total_time_s{0};

    uint8_t _delta_angle_clipping{0};
    uint8_t _delta_velocity_clipping{0};

    hrt_abstime _last_accel_clipping_notify_time{0};
    hrt_abstime _last_gyro_clipping_notify_time{0};

    uint64_t _last_accel_clipping_notify_total_count{0};
    uint64_t _last_gyro_clipping_notify_total_count{0};

    orb_advert_t _mavlink_log_pub{nullptr};

    uint32_t _backup_schedule_timeout_us{20000};

    bool _data_gap{false};
    bool _update_integrator_config{true};
    bool _intervals_configured{false};
    bool _publish_status{true};

    const uint8_t _instance;

    bool _armed{false};

    bool _accel_cal_available{false};
    bool _gyro_cal_available{false};

    struct InFlightCalibration {
        matrix::Vector3f offset{};
        matrix::Vector3f bias_variance{};
        bool             valid{false};
    };

    InFlightCalibration _accel_learned_calibration[ORB_MULTI_MAX_INSTANCES]{};
    InFlightCalibration _gyro_learned_calibration[ORB_MULTI_MAX_INSTANCES]{};

    static constexpr hrt_abstime INFLIGHT_CALIBRATION_QUIET_PERIOD_US{30_s};

    hrt_abstime _in_flight_calibration_check_timestamp_last{0};

    perf_counter_t _accel_generation_gap_perf{perf_alloc(PC_COUNT, MODULE_NAME ": accel data gap")};
    perf_counter_t _gyro_generation_gap_perf{perf_alloc(PC_COUNT, MODULE_NAME ": gyro data gap")};

    DEFINE_PARAMETERS(
        (ParamInt<params_id::IMU_INTEG_RATE>)_param_imu_integ_rate,
        (ParamBool<params_id::SENS_IMU_AUTOCAL>)_param_sens_imu_autocal)
};

} // namespace sensors
