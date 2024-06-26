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
 * Test code for the Velocity Smoothing library
 * Run this test only using make tests TESTFILTER=VelocitySmoothing
 */

#include <gtest/gtest.h>
#include <matrix/matrix/math.hpp>

#include "VelocitySmoothing.hpp"

using namespace matrix;

class VelocitySmoothingTest : public ::testing::Test
{
public:
	void setConstraints(float j_max, float a_max, float v_max);
	void setInitialConditions(Vector3f acc, Vector3f vel, Vector3f pos);
	void updateTrajectories(float dt, Vector3f velocity_setpoints);

	VelocitySmoothing _trajectories[3];
};

void VelocitySmoothingTest::setConstraints(float j_max, float a_max, float v_max)
{
	for (int i = 0; i < 3; i++) {
		_trajectories[i].setMaxJerk(j_max);
		_trajectories[i].setMaxAccel(a_max);
		_trajectories[i].setMaxVel(v_max);
	}
}

void VelocitySmoothingTest::setInitialConditions(Vector3f a0, Vector3f v0, Vector3f x0)
{
	for (int i = 0; i < 3; i++) {
		_trajectories[i].setCurrentAcceleration(a0(i));
		_trajectories[i].setCurrentVelocity(v0(i));
		_trajectories[i].setCurrentPosition(x0(i));
	}
}

void VelocitySmoothingTest::updateTrajectories(float dt, Vector3f velocity_setpoints)
{
	for (int i = 0; i < 3; i++) {
		_trajectories[i].updateTraj(dt);
		EXPECT_LE(fabsf(_trajectories[i].getCurrentJerk()), _trajectories[i].getMaxJerk());
		EXPECT_LE(fabsf(_trajectories[i].getCurrentAcceleration()), _trajectories[i].getMaxAccel());
		EXPECT_LE(fabsf(_trajectories[i].getCurrentVelocity()), _trajectories[i].getMaxVel());
	}

	for (int i = 0; i < 3; i++) {
		_trajectories[i].updateDurations(velocity_setpoints(i));
	}

	VelocitySmoothing::timeSynchronization(_trajectories, 2);
}

TEST_F(VelocitySmoothingTest, testTimeSynchronization)
{
	// GIVEN: A set of constraints
	const float j_max = 55.2f;
	const float a_max = 6.f;
	const float v_max = 6.f;

	setConstraints(j_max, a_max, v_max);

	// AND: A set of initial conditions
	Vector3f a0(0.22f, 0.f, 0.22f);
	Vector3f v0(2.47f, -5.59e-6f, 2.47f);
	Vector3f x0(0.f, 0.f, 0.f);

	setInitialConditions(a0, v0, x0);

	// WHEN: We generate trajectories (time synchronized in XY) with constant setpoints and dt
	Vector3f velocity_setpoints(-3.f, 1.f, 0.f);
	updateTrajectories(0.f, velocity_setpoints);


	// THEN: The X and Y trajectories should have the same total time (= time sunchronized)
	EXPECT_LE(fabsf(_trajectories[0].getTotalTime() - _trajectories[1].getTotalTime()), 0.0001);
}

TEST_F(VelocitySmoothingTest, testTimeSynchronizationSameDelta)
{
	// GIVEN: a set of initial conditions
	Vector3f a0(0.f, 0.f, 0.f);
	Vector3f v0(0.5f, -0.2f, 0.f);
	Vector3f x0(0.f, 0.f, 0.f);

	setInitialConditions(a0, v0, x0);

	// WHEN: the same delta velocity is set to the XY-axes
	const float delta_v = 0.3f;
	Vector3f velocity_setpoints{v0(0) + delta_v, v0(1) + delta_v, 0.f};

	for (int i = 0; i < 3; i++) {
		_trajectories[i].updateDurations(velocity_setpoints(i));
	}

	VelocitySmoothing::timeSynchronization(_trajectories, 3);

	// THEN: they should have the same T1, T2 and T3 dirations
	EXPECT_FLOAT_EQ(_trajectories[0].getTotalTime(), _trajectories[1].getTotalTime());
	EXPECT_FLOAT_EQ(_trajectories[0].getT1(), _trajectories[1].getT1());
	EXPECT_FLOAT_EQ(_trajectories[0].getT2(), _trajectories[1].getT2());
	EXPECT_FLOAT_EQ(_trajectories[0].getT3(), _trajectories[1].getT3());

	// AND: the Z axis should have the same duration but spend all its time in T2 (constant phase)
	EXPECT_FLOAT_EQ(_trajectories[2].getTotalTime(), _trajectories[0].getTotalTime());
	EXPECT_FLOAT_EQ(_trajectories[2].getT1(), 0.f);
	EXPECT_FLOAT_EQ(_trajectories[2].getT2(), _trajectories[0].getTotalTime());
	EXPECT_FLOAT_EQ(_trajectories[2].getT3(), 0.f);
}

TEST_F(VelocitySmoothingTest, testConstantSetpoint)
{
	// GIVEN: A set of constraints
	const float j_max = 55.2f;
	const float a_max = 6.f;
	const float v_max = 6.f;

	setConstraints(j_max, a_max, v_max);

	// AND: A set of initial conditions
	Vector3f a0(0.f, 0.f, 0.f);
	Vector3f v0(0.f, 0.f, 0.f);
	Vector3f x0(0.f, 0.f, 0.f);

	setInitialConditions(a0, v0, x0);

	// WHEN: We generate trajectories with constant setpoints and dt
	Vector3f velocity_setpoints(-3.f, 0.f, -1.f);

	// Compute the number of steps required to reach desired value
	// The updateTrajectories is first called once to compute the total time
	const float dt = 0.01;
	updateTrajectories(0.f, velocity_setpoints);
	float t123 = _trajectories[0].getTotalTime();
	int nb_steps = ceilf(t123 / dt);

	for (int i = 0; i < nb_steps; i++) {
		updateTrajectories(dt, velocity_setpoints);
	}

	// THEN: All the trajectories should have reach their
	// final state: desired velocity target and zero acceleration
	for (int i = 0; i < 3; i++) {
		EXPECT_LE(fabsf(_trajectories[i].getCurrentVelocity() - velocity_setpoints(i)), 0.01f);
		EXPECT_LE(fabsf(_trajectories[i].getCurrentAcceleration()), 0.0001f);
	}
}

TEST_F(VelocitySmoothingTest, testZeroSetpoint)
{
	// GIVEN: A set of null initial conditions
	Vector3f a0(0.f, 0.f, 0.f);
	Vector3f v0(0.f, 0.f, 0.f);
	Vector3f x0(0.f, 0.f, 0.f);

	setInitialConditions(a0, v0, x0);

	// AND: Zero setpoints
	Vector3f velocity_setpoints(0.f, 0.f, 0.f);
	const float dt = 0.01f;

	// WHEN: We run a few times the algorithm
	for (int i = 0; i < 60; i++) {
		updateTrajectories(dt, velocity_setpoints);
	}

	// THEN: All the trajectories should still be zero
	for (int i = 0; i < 3; i++) {
		EXPECT_FLOAT_EQ(_trajectories[i].getCurrentJerk(), 0.f);
		EXPECT_FLOAT_EQ(_trajectories[i].getCurrentAcceleration(), 0.f);
		EXPECT_FLOAT_EQ(_trajectories[i].getCurrentVelocity(), 0.f);
		EXPECT_FLOAT_EQ(_trajectories[i].getCurrentPosition(), 0.f);
	}
}
