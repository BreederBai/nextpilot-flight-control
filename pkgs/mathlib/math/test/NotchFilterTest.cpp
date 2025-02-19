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
 * Test code for the Notch filter
 * Run this test only using make tests TESTFILTER=NotchFilter
 */

#include <gtest/gtest.h>
#include <matrix/matrix/math.hpp>
#include <mathlib/math/filter/NotchFilter.hpp>

using namespace math;
using matrix::Vector3f;

class NotchFilterTest : public ::testing::Test
{
public:
	NotchFilter<float> _notch_float;
	NotchFilter<Vector3f> _notch_vector3f;
	const float _sample_freq = 1000.f;
	const float _notch_freq = 50.f;
	const float _bandwidth = 15.f;

	const float _epsilon_near = 1e-6f;
};

TEST_F(NotchFilterTest, simple)
{
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	EXPECT_EQ(_notch_float.getBandwidth(), _bandwidth);
	EXPECT_EQ(_notch_float.getNotchFreq(), _notch_freq);

	float a[3];
	float b[3];

	// a and b coefficients computed from an external python script
	const float b_expected[3] = {0.95496499f, -1.81645136f, 0.95496499f};
	const float a_expected[3] = {1.f, -1.81645136f, 0.90992999f};

	_notch_float.getCoefficients(a, b);

	for (int i = 0; i < 3; i++) {
		EXPECT_NEAR(a[i], a_expected[i], _epsilon_near);
		EXPECT_NEAR(b[i], b_expected[i], _epsilon_near);
	}
}

TEST_F(NotchFilterTest, filteringLowSide)
{
	// Send a 25Hz sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	const float signal_freq = 25.f;
	const float omega = 2.f * M_PI_F * signal_freq;
	float phase_delay = 11.4f * M_PI_F / 180.f; // Given by simulation
	float t = 0.f;
	float dt = 1.f / _sample_freq;
	float out = 0.f;

	for (int i = 0; i < 1000; i++) {
		float input = sinf(omega * t);
		float output_expected = sinf(omega * t - phase_delay);
		out = _notch_float.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (i > 30) {
			EXPECT_NEAR(out, output_expected, 0.05f);
		}
	}
}

TEST_F(NotchFilterTest, filteringLowSideDF1)
{
	// Send a 25Hz sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	const float signal_freq = 25.f;
	const float omega = 2.f * M_PI_F * signal_freq;
	float phase_delay = 11.4f * M_PI_F / 180.f; // Given by simulation
	float t = 0.f;
	float dt = 1.f / _sample_freq;
	float out = 0.f;

	for (int i = 0; i < 1000; i++) {
		float input = sinf(omega * t);
		float output_expected = sinf(omega * t - phase_delay);
		out = _notch_float.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (i > 30) {
			EXPECT_NEAR(out, output_expected, 0.05f);
		}
	}
}

TEST_F(NotchFilterTest, filteringHighSide)
{
	// Send a 98 sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	const float signal_freq = 98.4f;
	const float omega = 2.f * M_PI_F * signal_freq;
	float phase_delay = 11.4f * M_PI_F / 180.f; // Given by simulation
	float t = 0.f;
	float dt = 1.f / _sample_freq;
	float out = 0.f;

	for (int i = 0; i < 1000; i++) {
		float input = sinf(omega * t);
		float output_expected = sinf(omega * t + phase_delay);
		out = _notch_float.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (i > 30) {
			EXPECT_NEAR(out, output_expected, 0.05f);
		}
	}
}

TEST_F(NotchFilterTest, filteringHighSideDF1)
{
	// Send a 98 sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	const float signal_freq = 98.4f;
	const float omega = 2.f * M_PI_F * signal_freq;
	float phase_delay = 11.4f * M_PI_F / 180.f; // Given by simulation
	float t = 0.f;
	float dt = 1.f / _sample_freq;
	float out = 0.f;

	for (int i = 0; i < 1000; i++) {
		float input = sinf(omega * t);
		float output_expected = sinf(omega * t + phase_delay);
		out = _notch_float.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (i > 30) {
			EXPECT_NEAR(out, output_expected, 0.05f);
		}
	}
}

TEST_F(NotchFilterTest, filterOnNotch)
{
	// Send a 50 sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	const float signal_freq = 50.0f;
	const float omega = 2.f * M_PI_F * signal_freq;
	float t = 0.f;
	float dt = 1.f / _sample_freq;
	float out = 0.f;

	for (int i = 0; i < 1000; i++) {
		float input = sinf(omega * t);
		out = _notch_float.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (i > 50) {
			EXPECT_NEAR(out, 0.f, 0.1f);
		}
	}
}

TEST_F(NotchFilterTest, filterOnNotchDF1)
{
	// Send a 50 sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	const float signal_freq = 50.0f;
	const float omega = 2.f * M_PI_F * signal_freq;
	float t = 0.f;
	float dt = 1.f / _sample_freq;
	float out = 0.f;

	for (int i = 0; i < 1000; i++) {
		float input = sinf(omega * t);
		out = _notch_float.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (i > 50) {
			EXPECT_NEAR(out, 0.f, 0.1f);
		}
	}
}

TEST_F(NotchFilterTest, updateFilter)
{
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	float new_notch_freq = 100.f;
	float new_bandwidth = 10.f;
	_notch_float.setParameters(_sample_freq, new_notch_freq, new_bandwidth);

	EXPECT_EQ(new_notch_freq, _notch_float.getNotchFreq());
	EXPECT_EQ(new_bandwidth, _notch_float.getBandwidth());

}

TEST_F(NotchFilterTest, filterVector3f)
{
	// Send three sinusoidal signals (25, 50 and 98.5Hz) into a 50Hz triple notch filter
	_notch_vector3f.setParameters(_sample_freq, _notch_freq, _bandwidth);

	const Vector3f signal_freq(25.f, 50.f, 98.4f);
	const Vector3f omega = 2.f * M_PI_F * signal_freq;
	const Vector3f phase_delay = Vector3f(-11.4f, 0.f, 11.4f) * M_PI_F / 180.f;

	float t = 0.f;
	float dt = 1.f / _sample_freq;
	Vector3f out{};

	for (int i = 0; i < 1000; i++) {
		const Vector3f input(sinf(omega(0) * t), sinf(omega(1) * t), sinf(omega(2) * t));
		const Vector3f arg = omega * t + phase_delay;
		const Vector3f output_expected(sinf(arg(0)), 0.f, sinf(arg(2)));
		out = _notch_vector3f.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (i > 50) {
			EXPECT_NEAR(out(0), output_expected(0), 0.1f);
			EXPECT_NEAR(out(1), output_expected(1), 0.1f);
			EXPECT_NEAR(out(2), output_expected(2), 0.1f);
		}
	}
}

TEST_F(NotchFilterTest, filterVector3fDF1)
{
	// Send three sinusoidal signals (25, 50 and 98.5Hz) into a 50Hz triple notch filter
	_notch_vector3f.setParameters(_sample_freq, _notch_freq, _bandwidth);

	const Vector3f signal_freq(25.f, 50.f, 98.4f);
	const Vector3f omega = 2.f * M_PI_F * signal_freq;
	const Vector3f phase_delay = Vector3f(-11.4f, 0.f, 11.4f) * M_PI_F / 180.f;

	float t = 0.f;
	float dt = 1.f / _sample_freq;
	Vector3f out{};

	for (int i = 0; i < 1000; i++) {
		const Vector3f input(sinf(omega(0) * t), sinf(omega(1) * t), sinf(omega(2) * t));
		const Vector3f arg = omega * t + phase_delay;
		const Vector3f output_expected(sinf(arg(0)), 0.f, sinf(arg(2)));
		out = _notch_vector3f.apply(input);
		t = i * dt;

		// Let some time for the filter to settle
		if (i > 50) {
			EXPECT_NEAR(out(0), output_expected(0), 0.1f);
			EXPECT_NEAR(out(1), output_expected(1), 0.1f);
			EXPECT_NEAR(out(2), output_expected(2), 0.1f);
		}
	}
}

TEST_F(NotchFilterTest, disabled)
{
	const float zero_notch_freq = 0.f;
	_notch_float.setParameters(_sample_freq, zero_notch_freq, _bandwidth);

	float a[3];
	float b[3];

	// The filter should just be a static gain of 1
	const float b_expected[3] = {1.f, 0.f, 0.f};
	const float a_expected[3] = {1.f, 0.f, 0.f};

	_notch_float.getCoefficients(a, b);

	for (int i = 0; i < 3; i++) {
		EXPECT_EQ(a[i], a_expected[i]);
		EXPECT_EQ(b[i], b_expected[i]);
	}

	// Run the filter with a 1Hz sinusoid
	const float signal_freq = 1.0f;
	const float omega = 2.f * M_PI_F * signal_freq;
	float t = 0.f;
	float dt = 1.f / _sample_freq;
	float out = 0.f;

	for (int i = 0; i < 10; i++) {
		float input = sinf(omega * t);
		out = _notch_float.apply(input);
		t = i * dt;

		// The filter should not do anything to the input
		EXPECT_EQ(out, input);
	}
}

TEST_F(NotchFilterTest, setCoefficients)
{

	float a_new[2] = {1.f, 2.f};
	float b_new[3] = {1.f, 2.f, 3.f};
	float a[3];
	float b[3];

	_notch_float.setCoefficients(a_new, b_new);
	_notch_float.getCoefficients(a, b);

	for (int i = 0; i < 3; i++) {
		if (i >= 1) {EXPECT_EQ(a[i], a_new[i - 1]);} //a0 is not part of set function

		EXPECT_EQ(b[i], b_new[i]);
	}
}
