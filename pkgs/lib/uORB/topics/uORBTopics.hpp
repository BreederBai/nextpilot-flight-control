
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

#include <stddef.h>
#include <uORB.h>
#include <topics/action_request.h>
#include <topics/actuator_armed.h>
#include <topics/actuator_controls_status.h>
#include <topics/actuator_motors.h>
#include <topics/actuator_outputs.h>
#include <topics/actuator_servos.h>
#include <topics/actuator_servos_trim.h>
#include <topics/actuator_test.h>
#include <topics/adc_report.h>
#include <topics/airspeed.h>
#include <topics/airspeed_validated.h>
#include <topics/airspeed_wind.h>
#include <topics/autotune_attitude_control_status.h>
#include <topics/battery_status.h>
#include <topics/button_event.h>
#include <topics/camera_capture.h>
#include <topics/camera_status.h>
#include <topics/camera_trigger.h>
#include <topics/cellular_status.h>
#include <topics/collision_constraints.h>
#include <topics/collision_report.h>
#include <topics/control_allocator_status.h>
#include <topics/cpuload.h>
#include <topics/debug_array.h>
#include <topics/debug_key_value.h>
#include <topics/debug_value.h>
#include <topics/debug_vect.h>
#include <topics/differential_pressure.h>
#include <topics/distance_sensor.h>
#include <topics/ekf2_timestamps.h>
#include <topics/esc_report.h>
#include <topics/esc_status.h>
#include <topics/estimator_aid_source1d.h>
#include <topics/estimator_aid_source2d.h>
#include <topics/estimator_aid_source3d.h>
#include <topics/estimator_bias.h>
#include <topics/estimator_bias3d.h>
#include <topics/estimator_event_flags.h>
#include <topics/estimator_gps_status.h>
#include <topics/estimator_innovations.h>
#include <topics/estimator_selector_status.h>
#include <topics/estimator_sensor_bias.h>
#include <topics/estimator_states.h>
#include <topics/estimator_status.h>
#include <topics/estimator_status_flags.h>
#include <topics/event.h>
#include <topics/failsafe_flags.h>
#include <topics/failure_detector_status.h>
#include <topics/follow_target.h>
#include <topics/follow_target_estimator.h>
#include <topics/follow_target_status.h>
#include <topics/generator_status.h>
#include <topics/geofence_result.h>
#include <topics/gimbal_controls.h>
#include <topics/gimbal_device_attitude_status.h>
#include <topics/gimbal_device_information.h>
#include <topics/gimbal_device_set_attitude.h>
#include <topics/gimbal_manager_information.h>
#include <topics/gimbal_manager_set_attitude.h>
#include <topics/gimbal_manager_set_manual_control.h>
#include <topics/gimbal_manager_status.h>
#include <topics/gps_dump.h>
#include <topics/gps_inject_data.h>
#include <topics/gripper.h>
#include <topics/health_report.h>
#include <topics/heater_status.h>
#include <topics/home_position.h>
#include <topics/hover_thrust_estimate.h>
#include <topics/input_rc.h>
#include <topics/internal_combustion_engine_status.h>
#include <topics/iridiumsbd_status.h>
#include <topics/irlock_report.h>
#include <topics/landing_gear.h>
#include <topics/landing_gear_wheel.h>
#include <topics/landing_target_innovations.h>
#include <topics/landing_target_pose.h>
#include <topics/launch_detection_status.h>
#include <topics/led_control.h>
#include <topics/log_message.h>
#include <topics/logger_status.h>
#include <topics/mag_worker_data.h>
#include <topics/magnetometer_bias_estimate.h>
#include <topics/manual_control_setpoint.h>
#include <topics/manual_control_switches.h>
#include <topics/mavlink_log.h>
#include <topics/mavlink_tunnel.h>
#include <topics/mission.h>
#include <topics/mission_result.h>
#include <topics/mode_completed.h>
#include <topics/mount_orientation.h>
#include <topics/navigator_mission_item.h>
#include <topics/normalized_unsigned_setpoint.h>
#include <topics/npfg_status.h>
#include <topics/obstacle_distance.h>
#include <topics/offboard_control_mode.h>
#include <topics/onboard_computer_status.h>
#include <topics/orb_test.h>
#include <topics/orb_test_large.h>
#include <topics/orb_test_medium.h>
#include <topics/orbit_status.h>
#include <topics/parameter_update.h>
#include <topics/ping.h>
#include <topics/position_controller_landing_status.h>
#include <topics/position_controller_status.h>
#include <topics/position_setpoint.h>
#include <topics/position_setpoint_triplet.h>
#include <topics/power_button_state.h>
#include <topics/power_monitor.h>
#include <topics/pps_capture.h>
#include <topics/pwm_input.h>
#include <topics/px4io_status.h>
#include <topics/qshell_req.h>
#include <topics/qshell_retval.h>
#include <topics/radio_status.h>
#include <topics/rate_ctrl_status.h>
#include <topics/rc_channels.h>
#include <topics/rc_parameter_map.h>
#include <topics/rpm.h>
#include <topics/rtl_time_estimate.h>
#include <topics/satellite_info.h>
#include <topics/sensor_accel.h>
#include <topics/sensor_accel_fifo.h>
#include <topics/sensor_baro.h>
#include <topics/sensor_combined.h>
#include <topics/sensor_correction.h>
#include <topics/sensor_gnss_relative.h>
#include <topics/sensor_gps.h>
#include <topics/sensor_gyro.h>
#include <topics/sensor_gyro_fft.h>
#include <topics/sensor_gyro_fifo.h>
#include <topics/sensor_hygrometer.h>
#include <topics/sensor_mag.h>
#include <topics/sensor_optical_flow.h>
#include <topics/sensor_preflight_mag.h>
#include <topics/sensor_selection.h>
#include <topics/sensor_uwb.h>
#include <topics/sensors_status.h>
#include <topics/sensors_status_imu.h>
#include <topics/system_power.h>
#include <topics/takeoff_status.h>
#include <topics/task_stack_info.h>
#include <topics/tecs_status.h>
#include <topics/telemetry_status.h>
#include <topics/tiltrotor_extra_controls.h>
#include <topics/timesync_status.h>
#include <topics/trajectory_bezier.h>
#include <topics/trajectory_setpoint.h>
#include <topics/trajectory_waypoint.h>
#include <topics/transponder_report.h>
#include <topics/tune_control.h>
#include <topics/uavcan_parameter_request.h>
#include <topics/uavcan_parameter_value.h>
#include <topics/ulog_stream.h>
#include <topics/ulog_stream_ack.h>
#include <topics/vehicle_acceleration.h>
#include <topics/vehicle_air_data.h>
#include <topics/vehicle_angular_acceleration_setpoint.h>
#include <topics/vehicle_angular_velocity.h>
#include <topics/vehicle_attitude.h>
#include <topics/vehicle_attitude_setpoint.h>
#include <topics/vehicle_command.h>
#include <topics/vehicle_command_ack.h>
#include <topics/vehicle_constraints.h>
#include <topics/vehicle_control_mode.h>
#include <topics/vehicle_global_position.h>
#include <topics/vehicle_imu.h>
#include <topics/vehicle_imu_status.h>
#include <topics/vehicle_land_detected.h>
#include <topics/vehicle_local_position.h>
#include <topics/vehicle_local_position_setpoint.h>
#include <topics/vehicle_magnetometer.h>
#include <topics/vehicle_odometry.h>
#include <topics/vehicle_optical_flow.h>
#include <topics/vehicle_optical_flow_vel.h>
#include <topics/vehicle_rates_setpoint.h>
#include <topics/vehicle_roi.h>
#include <topics/vehicle_status.h>
#include <topics/vehicle_thrust_setpoint.h>
#include <topics/vehicle_torque_setpoint.h>
#include <topics/vehicle_trajectory_bezier.h>
#include <topics/vehicle_trajectory_waypoint.h>
#include <topics/vtol_vehicle_status.h>
#include <topics/wind.h>
#include <topics/yaw_estimator_status.h>


static constexpr size_t ORB_TOPICS_COUNT{243};
static constexpr size_t orb_topics_count() { return ORB_TOPICS_COUNT; }

/*
 * Returns array of topics metadata
 */
extern const struct orb_metadata *const *orb_get_topics() __EXPORT;

enum class ORB_ID : uint8_t {
	action_request = 0,
	actuator_armed = 1,
	actuator_controls_status_0 = 2,
	actuator_controls_status_1 = 3,
	actuator_motors = 4,
	actuator_outputs = 5,
	actuator_outputs_debug = 6,
	actuator_outputs_sim = 7,
	actuator_servos = 8,
	actuator_servos_trim = 9,
	actuator_test = 10,
	adc_report = 11,
	airspeed = 12,
	airspeed_validated = 13,
	airspeed_wind = 14,
	autotune_attitude_control_status = 15,
	battery_status = 16,
	button_event = 17,
	camera_capture = 18,
	camera_status = 19,
	camera_trigger = 20,
	cellular_status = 21,
	collision_constraints = 22,
	collision_report = 23,
	control_allocator_status = 24,
	cpuload = 25,
	debug_array = 26,
	debug_key_value = 27,
	debug_value = 28,
	debug_vect = 29,
	differential_pressure = 30,
	distance_sensor = 31,
	ekf2_timestamps = 32,
	esc_report = 33,
	esc_status = 34,
	estimator_aid_src_airspeed = 35,
	estimator_aid_src_aux_vel = 36,
	estimator_aid_src_baro_hgt = 37,
	estimator_aid_src_ev_hgt = 38,
	estimator_aid_src_ev_pos = 39,
	estimator_aid_src_ev_vel = 40,
	estimator_aid_src_ev_yaw = 41,
	estimator_aid_src_fake_hgt = 42,
	estimator_aid_src_fake_pos = 43,
	estimator_aid_src_gnss_hgt = 44,
	estimator_aid_src_gnss_pos = 45,
	estimator_aid_src_gnss_vel = 46,
	estimator_aid_src_gnss_yaw = 47,
	estimator_aid_src_gravity = 48,
	estimator_aid_src_mag = 49,
	estimator_aid_src_mag_heading = 50,
	estimator_aid_src_optical_flow = 51,
	estimator_aid_src_rng_hgt = 52,
	estimator_aid_src_sideslip = 53,
	estimator_aid_src_terrain_optical_flow = 54,
	estimator_attitude = 55,
	estimator_baro_bias = 56,
	estimator_bias3d = 57,
	estimator_ev_pos_bias = 58,
	estimator_event_flags = 59,
	estimator_global_position = 60,
	estimator_gnss_hgt_bias = 61,
	estimator_gps_status = 62,
	estimator_innovation_test_ratios = 63,
	estimator_innovation_variances = 64,
	estimator_innovations = 65,
	estimator_local_position = 66,
	estimator_odometry = 67,
	estimator_optical_flow_vel = 68,
	estimator_rng_hgt_bias = 69,
	estimator_selector_status = 70,
	estimator_sensor_bias = 71,
	estimator_states = 72,
	estimator_status = 73,
	estimator_status_flags = 74,
	estimator_wind = 75,
	event = 76,
	external_ins_attitude = 77,
	external_ins_global_position = 78,
	external_ins_local_position = 79,
	failsafe_flags = 80,
	failure_detector_status = 81,
	flaps_setpoint = 82,
	follow_target = 83,
	follow_target_estimator = 84,
	follow_target_status = 85,
	fw_virtual_attitude_setpoint = 86,
	generator_status = 87,
	geofence_result = 88,
	gimbal_controls = 89,
	gimbal_device_attitude_status = 90,
	gimbal_device_information = 91,
	gimbal_device_set_attitude = 92,
	gimbal_manager_information = 93,
	gimbal_manager_set_attitude = 94,
	gimbal_manager_set_manual_control = 95,
	gimbal_manager_status = 96,
	gimbal_v1_command = 97,
	gps_dump = 98,
	gps_inject_data = 99,
	gripper = 100,
	health_report = 101,
	heater_status = 102,
	home_position = 103,
	hover_thrust_estimate = 104,
	input_rc = 105,
	internal_combustion_engine_status = 106,
	iridiumsbd_status = 107,
	irlock_report = 108,
	landing_gear = 109,
	landing_gear_wheel = 110,
	landing_target_innovations = 111,
	landing_target_pose = 112,
	launch_detection_status = 113,
	led_control = 114,
	log_message = 115,
	logger_status = 116,
	mag_worker_data = 117,
	magnetometer_bias_estimate = 118,
	manual_control_input = 119,
	manual_control_setpoint = 120,
	manual_control_switches = 121,
	mavlink_log = 122,
	mavlink_tunnel = 123,
	mc_virtual_attitude_setpoint = 124,
	mission = 125,
	mission_result = 126,
	mode_completed = 127,
	mount_orientation = 128,
	navigator_mission_item = 129,
	npfg_status = 130,
	obstacle_distance = 131,
	obstacle_distance_fused = 132,
	offboard_control_mode = 133,
	onboard_computer_status = 134,
	orb_multitest = 135,
	orb_test = 136,
	orb_test_large = 137,
	orb_test_medium = 138,
	orb_test_medium_multi = 139,
	orb_test_medium_queue = 140,
	orb_test_medium_queue_poll = 141,
	orb_test_medium_wrap_around = 142,
	orbit_status = 143,
	parameter_update = 144,
	ping = 145,
	position_controller_landing_status = 146,
	position_controller_status = 147,
	position_setpoint = 148,
	position_setpoint_triplet = 149,
	power_button_state = 150,
	power_monitor = 151,
	pps_capture = 152,
	pwm_input = 153,
	px4io_status = 154,
	qshell_req = 155,
	qshell_retval = 156,
	radio_status = 157,
	rate_ctrl_status = 158,
	rc_channels = 159,
	rc_parameter_map = 160,
	rpm = 161,
	rtl_time_estimate = 162,
	safety_button = 163,
	satellite_info = 164,
	sensor_accel = 165,
	sensor_accel_fifo = 166,
	sensor_baro = 167,
	sensor_combined = 168,
	sensor_correction = 169,
	sensor_gnss_relative = 170,
	sensor_gps = 171,
	sensor_gyro = 172,
	sensor_gyro_fft = 173,
	sensor_gyro_fifo = 174,
	sensor_hygrometer = 175,
	sensor_mag = 176,
	sensor_optical_flow = 177,
	sensor_preflight_mag = 178,
	sensor_selection = 179,
	sensor_uwb = 180,
	sensors_status_baro = 181,
	sensors_status_imu = 182,
	sensors_status_mag = 183,
	spoilers_setpoint = 184,
	system_power = 185,
	takeoff_status = 186,
	task_stack_info = 187,
	tecs_status = 188,
	telemetry_status = 189,
	tiltrotor_extra_controls = 190,
	timesync_status = 191,
	trajectory_bezier = 192,
	trajectory_setpoint = 193,
	trajectory_waypoint = 194,
	transponder_report = 195,
	tune_control = 196,
	uavcan_parameter_request = 197,
	uavcan_parameter_value = 198,
	ulog_stream = 199,
	ulog_stream_ack = 200,
	vehicle_acceleration = 201,
	vehicle_air_data = 202,
	vehicle_angular_acceleration_setpoint = 203,
	vehicle_angular_velocity = 204,
	vehicle_angular_velocity_groundtruth = 205,
	vehicle_attitude = 206,
	vehicle_attitude_groundtruth = 207,
	vehicle_attitude_setpoint = 208,
	vehicle_command = 209,
	vehicle_command_ack = 210,
	vehicle_constraints = 211,
	vehicle_control_mode = 212,
	vehicle_global_position = 213,
	vehicle_global_position_groundtruth = 214,
	vehicle_gps_position = 215,
	vehicle_imu = 216,
	vehicle_imu_status = 217,
	vehicle_land_detected = 218,
	vehicle_local_position = 219,
	vehicle_local_position_groundtruth = 220,
	vehicle_local_position_setpoint = 221,
	vehicle_magnetometer = 222,
	vehicle_mocap_odometry = 223,
	vehicle_odometry = 224,
	vehicle_optical_flow = 225,
	vehicle_optical_flow_vel = 226,
	vehicle_rates_setpoint = 227,
	vehicle_roi = 228,
	vehicle_status = 229,
	vehicle_thrust_setpoint = 230,
	vehicle_thrust_setpoint_virtual_fw = 231,
	vehicle_thrust_setpoint_virtual_mc = 232,
	vehicle_torque_setpoint = 233,
	vehicle_torque_setpoint_virtual_fw = 234,
	vehicle_torque_setpoint_virtual_mc = 235,
	vehicle_trajectory_bezier = 236,
	vehicle_trajectory_waypoint = 237,
	vehicle_trajectory_waypoint_desired = 238,
	vehicle_visual_odometry = 239,
	vtol_vehicle_status = 240,
	wind = 241,
	yaw_estimator_status = 242,

	INVALID
};

const struct orb_metadata *get_orb_meta(ORB_ID id);
