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
 * @file sih_params.c
 * Parameters for quadcopter X simulator in hardware.
 *
 * @author Romain Chiappinelli <romain.chiap@gmail.com>
 * February 2019
 */

/**
 * Vehicle mass
 *
 * This value can be measured by weighting the quad on a scale.
 *
 * @unit kg
 * @min 0.0
 * @decimal 2
 * @increment 0.1
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_MASS, 1.0f);

/**
 * Vehicle inertia about X axis
 *
 * The inertia is a 3 by 3 symmetric matrix.
 * It represents the difficulty of the vehicle to modify its angular rate.
 *
 * @unit kg m^2
 * @min 0.0
 * @decimal 3
 * @increment 0.005
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_IXX, 0.025f);

/**
 * Vehicle inertia about Y axis
 *
 * The inertia is a 3 by 3 symmetric matrix.
 * It represents the difficulty of the vehicle to modify its angular rate.
 *
 * @unit kg m^2
 * @min 0.0
 * @decimal 3
 * @increment 0.005
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_IYY, 0.025f);

/**
 * Vehicle inertia about Z axis
 *
 * The inertia is a 3 by 3 symmetric matrix.
 * It represents the difficulty of the vehicle to modify its angular rate.
 *
 * @unit kg m^2
 * @min 0.0
 * @decimal 3
 * @increment 0.005
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_IZZ, 0.030f);

/**
 * Vehicle cross term inertia xy
 *
 * The inertia is a 3 by 3 symmetric matrix.
 * This value can be set to 0 for a quad symmetric about its center of mass.
 *
 * @unit kg m^2
 * @decimal 3
 * @increment 0.005
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_IXY, 0.0f);

/**
 * Vehicle cross term inertia xz
 *
 * The inertia is a 3 by 3 symmetric matrix.
 * This value can be set to 0 for a quad symmetric about its center of mass.
 *
 * @unit kg m^2
 * @decimal 3
 * @increment 0.005
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_IXZ, 0.0f);

/**
 * Vehicle cross term inertia yz
 *
 * The inertia is a 3 by 3 symmetric matrix.
 * This value can be set to 0 for a quad symmetric about its center of mass.
 *
 * @unit kg m^2
 * @decimal 3
 * @increment 0.005
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_IYZ, 0.0f);

/**
 * Max propeller thrust force
 *
 * This is the maximum force delivered by one propeller
 * when the motor is running at full speed.
 *
 * This value is usually about 5 times the mass of the quadrotor.
 *
 * @unit N
 * @min 0.0
 * @decimal 2
 * @increment 0.5
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_T_MAX, 5.0f);

/**
 * Max propeller torque
 *
 * This is the maximum torque delivered by one propeller
 * when the motor is running at full speed.
 *
 * This value is usually about few percent of the maximum thrust force.
 *
 * @unit Nm
 * @min 0.0
 * @decimal 3
 * @increment 0.05
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_Q_MAX, 0.1f);

/**
 * Roll arm length
 *
 * This is the arm length generating the rolling moment
 *
 * This value can be measured with a ruler.
 * This corresponds to half the distance between the left and right motors.
 *
 * @unit m
 * @min 0.0
 * @decimal 2
 * @increment 0.05
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_L_ROLL, 0.2f);

/**
 * Pitch arm length
 *
 * This is the arm length generating the pitching moment
 *
 * This value can be measured with a ruler.
 * This corresponds to half the distance between the front and rear motors.
 *
 * @unit m
 * @min 0.0
 * @decimal 2
 * @increment 0.05
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_L_PITCH, 0.2f);

/**
 * First order drag coefficient
 *
 * Physical coefficient representing the friction with air particules.
 * The greater this value, the slower the quad will move.
 *
 * Drag force function of velocity: D=-KDV*V.
 * The maximum freefall velocity can be computed as V=10*MASS/KDV [m/s]
 *
 * @unit N/(m/s)
 * @min 0.0
 * @decimal 2
 * @increment 0.05
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_KDV, 1.0f);

/**
 * First order angular damper coefficient
 *
 * Physical coefficient representing the friction with air particules during rotations.
 * The greater this value, the slower the quad will rotate.
 *
 * Aerodynamic moment function of body rate: Ma=-KDW*W_B.
 * This value can be set to 0 if unknown.
 *
 * @unit Nm/(rad/s)
 * @min 0.0
 * @decimal 3
 * @increment 0.005
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_KDW, 0.025f);

/**
 * Initial geodetic latitude
 *
 * This value represents the North-South location on Earth where the simulation begins.
 * A value of 45 deg should be written 450000000.
 *
 * LAT0, LON0, H0, MU_X, MU_Y, and MU_Z should ideally be consistent among each others
 * to represent a physical ground location on Earth.
 *
 * @unit deg*1e7
 * @min -850000000
 * @max  850000000
 * @group Simulation In Hardware
 */
PARAM_DEFINE_INT32(SIH_LOC_LAT0, 454671160);

/**
 * Initial geodetic longitude
 *
 * This value represents the East-West location on Earth where the simulation begins.
 * A value of 45 deg should be written 450000000.
 *
 * LAT0, LON0, H0, MU_X, MU_Y, and MU_Z should ideally be consistent among each others
 * to represent a physical ground location on Earth.
 *
 * @unit deg*1e7
 * @min -1800000000
 * @max  1800000000
 * @group Simulation In Hardware
 */
PARAM_DEFINE_INT32(SIH_LOC_LON0, -737578370);

/**
 * Initial AMSL ground altitude
 *
 * This value represents the Above Mean Sea Level (AMSL) altitude where the simulation begins.
 *
 * If using FlightGear as a visual animation,
 * this value can be tweaked such that the vehicle lies on the ground at takeoff.
 *
 * LAT0, LON0, H0, MU_X, MU_Y, and MU_Z should ideally be consistent among each others
 * to represent a physical ground location on Earth.
 *
 *
 * @unit m
 * @min -420.0
 * @max 8848.0
 * @decimal 2
 * @increment 0.01
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_LOC_H0, 32.34f);

/**
 * distance sensor minimum range
 *
 * @unit m
 * @min 0.0
 * @max 10.0
 * @decimal 4
 * @increment 0.01
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_DISTSNSR_MIN, 0.0f);

/**
 * distance sensor maximum range
 *
 * @unit m
 * @min 0.0
 * @max 1000.0
 * @decimal 4
 * @increment 0.01
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_DISTSNSR_MAX, 100.0f);

/**
 * if >= 0 the distance sensor measures will be overridden by this value
 *
 * Absolute value superior to 10000 will disable distance sensor
 *
 * @unit m
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_DISTSNSR_OVR, -1.0f);

/**
 * thruster time constant tau
 *
 * the time taken for the thruster to step from 0 to 100% should be about 4 times tau
 *
 * @unit s
 * @group Simulation In Hardware
 */
PARAM_DEFINE_FLOAT(SIH_T_TAU, 0.05f);

/**
 * Vehicle type
 *
 * @value 0 Multicopter
 * @value 1 Fixed-Wing
 * @value 2 Tailsitter
 * @value 3 Standard VTOL
 * @reboot_required true
 * @group Simulation In Hardware
 */
PARAM_DEFINE_INT32(SIH_VEHICLE_TYPE, 0);
