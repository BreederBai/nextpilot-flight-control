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
 * @file pid_design.hpp
 * @brief Helper functions to design PID controllers using identified models
 *
 * @author Mathieu Bresciani <mathieu@auterion.com>
 */

#pragma once

#include <matrix/matrix/math.hpp>
#include <mathlib/mathlib.h>

namespace pid_design
{

/*
* Compute a set of PID gains using General Minimum Variance Control law design
*
* @param num: the coefficients of the numerator of the discrete-time plant transfer function
* @param den: the coefficients of the denominator of the discrete-time plant transfer function
* @param dt: the sampling time in seconds
* @param sigma: the desired closed-loop rise time in seconds
* @param delta: the damping index (between 0 and 2). 0 is critical damping, 1 is Butterworth
* @param lbda: the "detuning" coefficients. This affects the gain of the controller kc only (increase to detune the controller)
*
* @return The PID gains in standard form: u = kc*[1 + ki*dt + kd/dt]*e
*         kc: the controller gain
*         ki: the integral gain (= 1/Ti)
*         kd: the derivative gain (= Td)
*
* Reference:
* T.Yamatoto, K.Fujii and M.Kaneda, Design and implementation of a self-tuning pid controller, 1998
*/

inline matrix::Vector3f computePidGmvc(const matrix::Vector3f &num, const matrix::Vector3f &den, float dt,
				       float sigma = 0.1f, float delta = 1.f, float lbda = 0.5f)
{
	sigma = math::constrain(sigma, 0.01f, 1.f);
	delta = math::constrain(delta, 0.f, 1.f);
	lbda = math::constrain(lbda, 0.f, 10.f);

	const float a1 = den(1);
	const float a2 = den(2);
	const float b0 = num(0);
	const float b1 = num(1);
	const float b2 = num(2);

	// Solve GMVC law (see derivation in pid_synthesis_symbolic.py)
	const float rho = dt / sigma;
	const float mu = 0.25f * (1.f - delta) + 0.51f * delta; // mu is in the interval [0.25 0.51]
	const float p1 = -2.f * expf(-rho / (2.f * mu)) * cosf(sqrtf(4.f * mu - 1.f) * rho / (2.f * mu));
	const float p2 = expf(-rho / mu);
	const float e1 = -a1 + p1 + 1.f;
	const float f0 = -a1 * e1 + a1 - a2 + e1 + p2;
	const float f1 = a1 * e1 - a2 * e1 + a2;
	const float f2 = a2 * e1;

	// Translate to PID gains
	const float nu = lbda + (e1 + 1.f) * (b0 + b1 + b2);

	if (fabsf(nu) < FLT_EPSILON) {
		return matrix::Vector3f();
	}

	const float kc = -(f1 + 2.f * f2) / nu;
	float ki = -(f0 + f1 + f2) / (dt * (f1 + 2.f * f2));
	ki /= 5.f; // This is not part of the original implementation but is required to produce reasonable gains
	const float kd = -dt * f2 / (f1 + 2.f * f2);

	return matrix::Vector3f(kc, ki, kd);
}
} // namespace pid_design
