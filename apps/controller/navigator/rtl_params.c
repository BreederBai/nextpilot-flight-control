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
 * @file rtl_params.c
 *
 * Parameters for return mode
 *
 * @author Julian Oes <julian@oes.ch>
 */

/*
 * Return mode parameters, accessible via MAVLink
 */

/**
 * Return mode return altitude
 *
 * Default minimum altitude above destination (e.g. home, safe point, landing pattern) for return flight.
 * The vehicle will climb to this altitude when Return mode is enganged, unless it currently is flying higher already.
 * This is affected by RTL_MIN_DIST and RTL_CONE_ANG.
 *
 * @unit m
 * @min 0
 * @decimal 1
 * @increment 0.5
 * @group Return Mode
 */
PARAM_DEFINE_FLOAT(RTL_RETURN_ALT, 60.f);

/**
 * Return mode loiter altitude
 *
 * Descend to this altitude (above destination position) after return, and wait for time defined in RTL_LAND_DELAY.
 * Land (i.e. slowly descend) from this altitude if autolanding allowed.
 * VTOLs do transition to hover in this altitdue above the landing point.
 *
 * @unit m
 * @min 0
 * @decimal 1
 * @increment 0.5
 * @group Return Mode
 */
PARAM_DEFINE_FLOAT(RTL_DESCEND_ALT, 30.f);

/**
 * Return mode delay
 *
 * Delay before landing (after initial descent) in Return mode.
 * If set to -1 the system will not land but loiter at RTL_DESCEND_ALT.
 *
 * @unit s
 * @min -1
 * @decimal 1
 * @increment 0.5
 * @group Return Mode
 */
PARAM_DEFINE_FLOAT(RTL_LAND_DELAY, 0.0f);

/**
 * Horizontal radius from return point within which special rules for return mode apply.
 *
 * The return altitude will be calculated based on RTL_CONE_ANG parameter.
 * The yaw setpoint will switch to the one defined by corresponding waypoint.
 *
 *
 * @unit m
 * @min 0.5
 * @decimal 1
 * @increment 0.5
 * @group Return Mode
 */
PARAM_DEFINE_FLOAT(RTL_MIN_DIST, 10.0f);

/**
 * Return type
 *
 * Return mode destination and flight path (home location, rally point, mission landing pattern, reverse mission)
 *
 * @value 0 Return to closest safe point (home or rally point) via direct path.
 * @value 1 Return to closest safe point other than home (mission landing pattern or rally point), via direct path. If no mission landing or rally points are defined return home via direct path. Always chose closest safe landing point if vehicle is a VTOL in hover mode.
 * @value 2 Return to a planned mission landing, if available, using the mission path, else return to home via the reverse mission path. Do not consider rally points.
 * @value 3 Return via direct path to closest destination: home, start of mission landing pattern or safe point. If the destination is a mission landing pattern, follow the pattern to land.
 * @group Return Mode
 */
PARAM_DEFINE_INT32(RTL_TYPE, 0);

/**
 * Half-angle of the return mode altitude cone
 *
 * Defines the half-angle of a cone centered around the destination position that
 * affects the altitude at which the vehicle returns.
 *
 * @unit deg
 * @min 0
 * @max 90
 * @value 0 No cone, always climb to RTL_RETURN_ALT above destination.
 * @value 25 25 degrees half cone angle.
 * @value 45 45 degrees half cone angle.
 * @value 65 65 degrees half cone angle.
 * @value 80 80 degrees half cone angle.
 * @value 90 Only climb to at least RTL_DESCEND_ALT above destination.
 * @group Return Mode
 */
PARAM_DEFINE_INT32(RTL_CONE_ANG, 45);

/**
 * RTL precision land mode
 *
 * Use precision landing when doing an RTL landing phase.
 *
 * @value 0 No precision landing
 * @value 1 Opportunistic precision landing
 * @value 2 Required precision landing
 * @group Return Mode
 */
PARAM_DEFINE_INT32(RTL_PLD_MD, 0);

/**
 * Loiter radius for rtl descend
 *
 * Set the radius for loitering to a safe altitude for VTOL transition.
 *
 * @unit m
 * @min 25
 * @decimal 1
 * @increment 0.5
 * @group Return Mode
 */
PARAM_DEFINE_FLOAT(RTL_LOITER_RAD, 80.0f);

/**
 * RTL heading mode
 *
 * Defines the heading behavior during RTL
 *
 * @value 0 Towards next waypoint.
 * @value 1 Heading matches destination.
 * @value 2 Use current heading.
 * @group Return Mode
 */
PARAM_DEFINE_INT32(RTL_HDG_MD, 0);

/**
 * RTL time estimate safety margin factor
 *
 * Safety factor that is used to scale the actual RTL time estimate.
 * Time with margin = RTL_TIME_FACTOR * time + RTL_TIME_MARGIN
 *
 * @min 1.0
 * @max 2.0
 * @decimal 1
 * @increment 0.1
 * @group Return To Land
 */
PARAM_DEFINE_FLOAT(RTL_TIME_FACTOR, 1.1f);

/**
 * RTL time estimate safety margin offset
 *
 * Margin that is added to the time estimate, after it has already been scaled
 * Time with margin = RTL_TIME_FACTOR * time + RTL_TIME_MARGIN
 *
 * @unit s
 * @min 0
 * @max 3600
 * @decimal 1
 * @increment 1
 * @group Return To Land
 */
PARAM_DEFINE_INT32(RTL_TIME_MARGIN, 100);
