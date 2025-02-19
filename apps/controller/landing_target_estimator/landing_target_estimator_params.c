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
 * @file landing_target_estimator_params.c
 * Landing target estimator algorithm parameters.
 *
 * @author Nicolas de Palezieux (Sunflower Labs) <ndepal@gmail.com>
 * @author Mohammed Kabir <kabir@uasys.io>
 *
 */

/**
 * Landing target mode
 *
 * Configure the mode of the landing target. Depending on the mode, the landing target observations are used differently to aid position estimation.
 *
 * Mode Moving:     The landing target may be moving around while in the field of view of the vehicle. Landing target measurements are not used to aid positioning.
 * Mode Stationary: The landing target is stationary. Measured velocity w.r.t. the landing target is used to aid velocity estimation.
 *
 * @min 0
 * @max 1
 * @group Landing target Estimator
 * @value 0 Moving
 * @value 1 Stationary
 */
PARAM_DEFINE_INT32(LTEST_MODE, 0);

/**
 * Acceleration uncertainty
 *
 * Variance of acceleration measurement used for landing target position prediction.
 * Higher values results in tighter following of the measurements and more lenient outlier rejection
 *
 * @unit (m/s^2)^2
 * @min 0.01
 * @decimal 2
 *
 * @group Landing target Estimator
 */
PARAM_DEFINE_FLOAT(LTEST_ACC_UNC, 10.0f);

/**
 * Landing target measurement uncertainty
 *
 * Variance of the landing target measurement from the driver.
 * Higher values result in less aggressive following of the measurement and a smoother output as well as fewer rejected measurements.
 *
 * @unit tan(rad)^2
 * @decimal 4
 *
 * @group Landing target Estimator
 */
PARAM_DEFINE_FLOAT(LTEST_MEAS_UNC, 0.005f);

/**
 * Initial landing target position uncertainty
 *
 * Initial variance of the relative landing target position in x and y direction
 *
 * @unit m^2
 * @min 0.001
 * @decimal 3
 *
 * @group Landing target Estimator
 */
PARAM_DEFINE_FLOAT(LTEST_POS_UNC_IN, 0.1f);

/**
 * Initial landing target velocity uncertainty
 *
 * Initial variance of the relative landing target velocity in x and y directions
 *
 * @unit (m/s)^2
 * @min 0.001
 * @decimal 3
 *
 * @group Landing target Estimator
 */
PARAM_DEFINE_FLOAT(LTEST_VEL_UNC_IN, 0.1f);

/**
 * Scale factor for sensor measurements in sensor x axis
 *
 * Landing target x measurements are scaled by this factor before being used
 *
 * @min 0.01
 * @decimal 3
 *
 * @group Landing target Estimator
 */
PARAM_DEFINE_FLOAT(LTEST_SCALE_X, 1.0f);

/**
 * Scale factor for sensor measurements in sensor y axis
 *
 * Landing target y measurements are scaled by this factor before being used
 *
 * @min 0.01
 * @decimal 3
 *
 * @group Landing target Estimator
 */
PARAM_DEFINE_FLOAT(LTEST_SCALE_Y, 1.0f);

/**
 * Rotation of IRLOCK sensor relative to airframe
 *
 * Default orientation of Yaw 90°
 *
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 *
 * @min -1
 * @max 40
 * @reboot_required true
 * @group Landing Target Estimator
 */
PARAM_DEFINE_INT32(LTEST_SENS_ROT, 2);

/**
 * X Position of IRLOCK in body frame (forward)
 *
 * @reboot_required true
 * @unit m
 * @decimal 3
 * @group Landing Target Estimator
 *
 */
PARAM_DEFINE_FLOAT(LTEST_SENS_POS_X, 0.0f);

/**
 * Y Position of IRLOCK in body frame (right)
 *
 * @reboot_required true
 * @unit m
 * @decimal 3
 * @group Landing Target Estimator
 *
 */
PARAM_DEFINE_FLOAT(LTEST_SENS_POS_Y, 0.0f);

/**
 * Z Position of IRLOCK in body frame (downward)
 *
 * @reboot_required true
 * @unit m
 * @decimal 3
 * @group Landing Target Estimator
 *
 */
PARAM_DEFINE_FLOAT(LTEST_SENS_POS_Z, 0.0f);
