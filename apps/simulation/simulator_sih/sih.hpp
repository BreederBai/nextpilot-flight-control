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
 * @file sih.hpp
 * Simulator in Hardware
 *
 * @author Romain Chiappinelli      <romain.chiap@gmail.com>
 *
 * Coriolis g Corporation - January 2019
 */

// The sensor signals reconstruction and noise levels are from [1]
// [1] Bulka E, and Nahon M, "Autonomous fixed-wing aerobatics: from theory to flight."
//     In 2018 IEEE International Conference on Robotics and Automation (ICRA), pp. 6573-6580. IEEE, 2018.
// The aerodynamic model is from [2]
// [2] Khan W, supervised by Nahon M, "Dynamics modeling of agile fixed-wing unmanned aerial vehicles."
//     McGill University (Canada), PhD thesis, 2016.
// The quaternion integration are from [3]
// [3] Sveier A, Sjøberg AM, Egeland O. "Applied Runge–Kutta–Munthe-Kaas Integration for the Quaternion Kinematics."
//     Journal of Guidance, Control, and Dynamics. 2019 Dec;42(12):2747-54.
// The tailsitter model is from [4]
// [4] Chiappinelli R, supervised by Nahon M, "Modeling and control of a flying wing tailsitter unmanned aerial vehicle."
//     McGill University (Canada), Masters Thesis, 2018.

#pragma once

#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <module/module_thread.hpp>
#include <matrix/math.hpp>       // matrix, vectors, dcm, quaterions
#include <conversion/rotation.h> // math::radians,
#include <geo/geo.h>             // to get the physical constants
#include <hrtimer.h>             // to get the real time
#include <accelerometer/PX4Accelerometer.hpp>
#include <gyroscope/PX4Gyroscope.hpp>
#include <perf/perf_counter.h>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/airspeed.h>
#include <uORB/topics/actuator_outputs.h>
#include <uORB/topics/distance_sensor.h>
#include <uORB/topics/vehicle_angular_velocity.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_local_position.h>

#if defined(ENABLE_LOCKSTEP_SCHEDULER)
#   include <sys/time.h>
#endif

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::param;

class Sih : public ModuleCommand<Sih>, public ModuleParams, public ModuleThread {
public:
    Sih();
    virtual ~Sih();

    /** @see ModuleCommand */
    static Sih *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    int init() override {
        return ModuleThread::init();
    }

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::run() */
    void Run() override;

    static float generate_wgn(); // generate white Gaussian noise sample

    // generate white Gaussian noise sample as a 3D vector with specified std
    static matrix::Vector3f noiseGauss3f(float stdx, float stdy, float stdz);

private:
    void parameters_updated();

    // simulated sensors
    PX4Accelerometer                     _px4_accel{1310988}; // 1310988: DRV_IMU_DEVTYPE_SIM, BUS: 1, ADDR: 1, TYPE: SIMULATION
    PX4Gyroscope                         _px4_gyro{1310988};  // 1310988: DRV_IMU_DEVTYPE_SIM, BUS: 1, ADDR: 1, TYPE: SIMULATION
    sensor_accel_s                       _sensor_accel{};
    sensor_gyro_s                        _sensor_gyro{};
    uORB::Publication<sensor_accel_s>    _sensor_accel_pub{ORB_ID(sensor_accel)};
    uORB::Publication<sensor_gyro_s>     _sensor_gyro_pub{ORB_ID(sensor_gyro)};
    uORB::Publication<distance_sensor_s> _distance_snsr_pub{ORB_ID(distance_sensor)};
    uORB::Publication<airspeed_s>        _airspeed_pub{ORB_ID(airspeed)};

    // groundtruth
    uORB::Publication<vehicle_angular_velocity_s> _angular_velocity_ground_truth_pub{ORB_ID(vehicle_angular_velocity_groundtruth)};
    uORB::Publication<vehicle_attitude_s>         _attitude_ground_truth_pub{ORB_ID(vehicle_attitude_groundtruth)};
    uORB::Publication<vehicle_local_position_s>   _local_position_ground_truth_pub{ORB_ID(vehicle_local_position_groundtruth)};
    uORB::Publication<vehicle_global_position_s>  _global_position_ground_truth_pub{ORB_ID(vehicle_global_position_groundtruth)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};
    uORB::Subscription         _actuator_out_sub{ORB_ID(actuator_outputs)};

    // hard constants
    static constexpr uint16_t NB_MOTORS     = 6;
    static constexpr float    T1_C          = 15.0f;                                  // ground temperature in Celsius
    static constexpr float    T1_K          = T1_C - CONSTANTS_ABSOLUTE_NULL_CELSIUS; // ground temperature in Kelvin
    static constexpr float    TEMP_GRADIENT = -6.5f / 1000.0f;                        // temperature gradient in degrees per metre
    // Aerodynamic coefficients
    static constexpr float RHO      = 1.225f;         // air density at sea level [kg/m^3]
    static constexpr float SPAN     = 0.86f;          // wing span [m]
    static constexpr float MAC      = 0.21f;          // wing mean aerodynamic chord [m]
    static constexpr float RP       = 0.1f;           // radius of the propeller [m]
    static constexpr float FLAP_MAX = M_PI_F / 12.0f; // 15 deg, maximum control surface deflection

    void init_variables();

    // read the motor signals outputted from the mixer
    void read_motors(const float dt);

    // generate the motors thrust and torque in the body frame
    void generate_force_and_torques();

    // apply the equations of motion of a rigid body and integrate one step
    void equations_of_motion(const float dt);

    // reconstruct the noisy sensor signals
    void reconstruct_sensors_signals(const hrt_abstime &time_now_us);
    void send_airspeed(const hrt_abstime &time_now_us);
    void send_dist_snsr(const hrt_abstime &time_now_us);
    void publish_ground_truth(const hrt_abstime &time_now_us);
    void generate_fw_aerodynamics();
    void generate_ts_aerodynamics();
    void sensor_step();

#if defined(ENABLE_LOCKSTEP_SCHEDULER)
    void     lockstep_loop();
    uint64_t _current_simulation_time_us{0};
    float    _achieved_speedup{0.f};
#endif

    void realtime_loop();

    struct rt_semaphore _data_semaphore;
    hrt_call            _timer_call{};

    perf_counter_t _loop_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};
    perf_counter_t _loop_interval_perf{perf_alloc(PC_INTERVAL, MODULE_NAME ": cycle interval")};

    hrt_abstime _last_run{0};
    hrt_abstime _last_actuator_output_time{0};
    hrt_abstime _airspeed_time{0};
    hrt_abstime _dist_snsr_time{0};

    bool _grounded{true};             // whether the vehicle is on the ground

    matrix::Vector3f _T_B{};          // thrust force in body frame [N]
    matrix::Vector3f _Fa_I{};         // aerodynamic force in inertial frame [N]
    matrix::Vector3f _Mt_B{};         // thruster moments in the body frame [Nm]
    matrix::Vector3f _Ma_B{};         // aerodynamic moments in the body frame [Nm]
    matrix::Vector3f _p_I{};          // inertial position [m]
    matrix::Vector3f _v_I{};          // inertial velocity [m/s]
    matrix::Vector3f _v_B{};          // body frame velocity [m/s]
    matrix::Vector3f _p_I_dot{};      // inertial position differential
    matrix::Vector3f _v_I_dot{};      // inertial velocity differential
    matrix::Quatf    _q{};            // quaternion attitude
    matrix::Dcmf     _C_IB{};         // body to inertial transformation
    matrix::Vector3f _w_B{};          // body rates in body frame [rad/s]
    matrix::Quatf    _dq{};           // quaternion differential
    matrix::Vector3f _w_B_dot{};      // body rates differential
    float            _u[NB_MOTORS]{}; // thruster signals

    enum class VehicleType {
        MC,
        FW,
        TS,
        VTOL
    };
    VehicleType _vehicle = VehicleType::MC;

    // aerodynamic segments for the fixedwing
    AeroSeg _wing_l    = AeroSeg(SPAN / 2.0f, MAC, -4.0f, matrix::Vector3f(0.0f, -SPAN / 4.0f, 0.0f), 3.0f,
                                 SPAN / MAC, MAC / 3.0f);
    AeroSeg _wing_r    = AeroSeg(SPAN / 2.0f, MAC, -4.0f, matrix::Vector3f(0.0f, SPAN / 4.0f, 0.0f), -3.0f,
                                 SPAN / MAC, MAC / 3.0f);
    AeroSeg _tailplane = AeroSeg(0.3f, 0.1f, 0.0f, matrix::Vector3f(-0.4f, 0.0f, 0.0f), 0.0f, -1.0f, 0.05f, RP);
    AeroSeg _fin       = AeroSeg(0.25, 0.18, 0.0f, matrix::Vector3f(-0.45f, 0.0f, -0.1f), -90.0f, -1.0f, 0.12f, RP);
    AeroSeg _fuselage  = AeroSeg(0.2, 0.8, 0.0f, matrix::Vector3f(0.0f, 0.0f, 0.0f), -90.0f);

    // aerodynamic segments for the tailsitter
    static constexpr const int   NB_TS_SEG      = 11;
    static constexpr const float TS_AR          = 3.13f;
    static constexpr const float TS_CM          = 0.115f;                                                         // longitudinal position of the CM from trailing edge
    static constexpr const float TS_RP          = 0.0625f;                                                        // propeller radius [m]
    static constexpr const float TS_DEF_MAX     = math::radians(39.0f);                                           // max deflection
    matrix::Dcmf                 _C_BS          = matrix::Dcmf(matrix::Eulerf(0.0f, math::radians(90.0f), 0.0f)); // segment to body 90 deg pitch
    AeroSeg                      _ts[NB_TS_SEG] = {
        AeroSeg(0.0225f, 0.110f, 0.0f, matrix::Vector3f(0.083f - TS_CM, -0.239f, 0.0f), 0.0f, TS_AR),
        AeroSeg(0.0383f, 0.125f, 0.0f, matrix::Vector3f(0.094f - TS_CM, -0.208f, 0.0f), 0.0f, TS_AR, 0.063f),
        // AeroSeg(0.0884f, 0.148f, 0.0f, matrix::Vector3f(0.111f-TS_CM, -0.143f, 0.0f), 0.0f, TS_AR, 0.063f, TS_RP),
        AeroSeg(0.0884f, 0.085f, 0.0f, matrix::Vector3f(0.158f - TS_CM, -0.143f, 0.0f), 0.0f, TS_AR),
        AeroSeg(0.0884f, 0.063f, 0.0f, matrix::Vector3f(0.047f - TS_CM, -0.143f, 0.0f), 0.0f, TS_AR, 0.063f, TS_RP),
        AeroSeg(0.0633f, 0.176f, 0.0f, matrix::Vector3f(0.132f - TS_CM, -0.068f, 0.0f), 0.0f, TS_AR, 0.063f),
        AeroSeg(0.0750f, 0.231f, 0.0f, matrix::Vector3f(0.173f - TS_CM, 0.000f, 0.0f), 0.0f, TS_AR),
        AeroSeg(0.0633f, 0.176f, 0.0f, matrix::Vector3f(0.132f - TS_CM, 0.068f, 0.0f), 0.0f, TS_AR, 0.063f),
        // AeroSeg(0.0884f, 0.148f, 0.0f, matrix::Vector3f(0.111f-TS_CM,  0.143f, 0.0f), 0.0f, TS_AR, 0.063f, TS_RP),
        AeroSeg(0.0884f, 0.085f, 0.0f, matrix::Vector3f(0.158f - TS_CM, 0.143f, 0.0f), 0.0f, TS_AR),
        AeroSeg(0.0884f, 0.063f, 0.0f, matrix::Vector3f(0.047f - TS_CM, 0.143f, 0.0f), 0.0f, TS_AR, 0.063f, TS_RP),
        AeroSeg(0.0383f, 0.125f, 0.0f, matrix::Vector3f(0.094f - TS_CM, 0.208f, 0.0f), 0.0f, TS_AR, 0.063f),
        AeroSeg(0.0225f, 0.110f, 0.0f, matrix::Vector3f(0.083f - TS_CM, 0.239f, 0.0f), 0.0f, TS_AR)};

    // AeroSeg _ts[NB_TS_SEG] = {
    // 	AeroSeg(0.0225f, 0.110f, -90.0f, matrix::Vector3f(0.0f, -0.239f, TS_CM-0.083f), 0.0f, TS_AR),
    // 	AeroSeg(0.0383f, 0.125f, -90.0f, matrix::Vector3f(0.0f, -0.208f, TS_CM-0.094f), 0.0f, TS_AR, 0.063f),
    // 	AeroSeg(0.0884f, 0.148f, -90.0f, matrix::Vector3f(0.0f, -0.143f, TS_CM-0.111f), 0.0f, TS_AR, 0.063f, TS_RP),
    // 	AeroSeg(0.0633f, 0.176f, -90.0f, matrix::Vector3f(0.0f, -0.068f, TS_CM-0.132f), 0.0f, TS_AR, 0.063f),
    // 	AeroSeg(0.0750f, 0.231f, -90.0f, matrix::Vector3f(0.0f,  0.000f, TS_CM-0.173f), 0.0f, TS_AR),
    // 	AeroSeg(0.0633f, 0.176f, -90.0f, matrix::Vector3f(0.0f,  0.068f, TS_CM-0.132f), 0.0f, TS_AR, 0.063f),
    // 	AeroSeg(0.0884f, 0.148f, -90.0f, matrix::Vector3f(0.0f,  0.143f, TS_CM-0.111f), 0.0f, TS_AR, 0.063f, TS_RP),
    // 	AeroSeg(0.0383f, 0.125f, -90.0f, matrix::Vector3f(0.0f,  0.208f, TS_CM-0.094f), 0.0f, TS_AR, 0.063f),
    // 	AeroSeg(0.0225f, 0.110f, -90.0f, matrix::Vector3f(0.0f,  0.239f, TS_CM-0.083f), 0.0f, TS_AR)
    // 	};

    // parameters
    float            _MASS, _T_MAX, _Q_MAX, _L_ROLL, _L_PITCH, _KDV, _KDW, _H0, _T_TAU;
    double           _LAT0, _LON0, _COS_LAT0;
    matrix::Vector3f _W_I; // weight of the vehicle in inertial frame [N]
    matrix::Matrix3f _I;   // vehicle inertia matrix
    matrix::Matrix3f _Im1; // inverse of the inertia matrix

    float _distance_snsr_min, _distance_snsr_max, _distance_snsr_override;

    // parameters defined in sih_params.c
    DEFINE_PARAMETERS(
        (ParamInt<params_id::IMU_GYRO_RATEMAX>)_imu_gyro_ratemax,
        (ParamInt<params_id::IMU_INTEG_RATE>)_imu_integration_rate,
        (ParamFloat<params_id::SIH_MASS>)_sih_mass,
        (ParamFloat<params_id::SIH_IXX>)_sih_ixx,
        (ParamFloat<params_id::SIH_IYY>)_sih_iyy,
        (ParamFloat<params_id::SIH_IZZ>)_sih_izz,
        (ParamFloat<params_id::SIH_IXY>)_sih_ixy,
        (ParamFloat<params_id::SIH_IXZ>)_sih_ixz,
        (ParamFloat<params_id::SIH_IYZ>)_sih_iyz,
        (ParamFloat<params_id::SIH_T_MAX>)_sih_t_max,
        (ParamFloat<params_id::SIH_Q_MAX>)_sih_q_max,
        (ParamFloat<params_id::SIH_L_ROLL>)_sih_l_roll,
        (ParamFloat<params_id::SIH_L_PITCH>)_sih_l_pitch,
        (ParamFloat<params_id::SIH_KDV>)_sih_kdv,
        (ParamFloat<params_id::SIH_KDW>)_sih_kdw,
        (ParamInt<params_id::SIH_LOC_LAT0>)_sih_lat0,
        (ParamInt<params_id::SIH_LOC_LON0>)_sih_lon0,
        (ParamFloat<params_id::SIH_LOC_H0>)_sih_h0,
        (ParamFloat<params_id::SIH_DISTSNSR_MIN>)_sih_distance_snsr_min,
        (ParamFloat<params_id::SIH_DISTSNSR_MAX>)_sih_distance_snsr_max,
        (ParamFloat<params_id::SIH_DISTSNSR_OVR>)_sih_distance_snsr_override,
        (ParamFloat<params_id::SIH_T_TAU>)_sih_thrust_tau,
        (ParamInt<params_id::SIH_VEHICLE_TYPE>)_sih_vtype)
};
