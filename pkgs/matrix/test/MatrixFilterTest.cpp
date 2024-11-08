/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <gtest/gtest.h>
#include <matrix/filter.hpp>

using namespace matrix;

TEST(MatrixFilterTest, Filter)
{
	const size_t n_x = 6;
	const size_t n_y = 5;
	SquareMatrix<float, n_x> P = eye<float, n_x>();
	SquareMatrix<float, n_y> R = eye<float, n_y>();
	Matrix<float, n_y, n_x> C;
	C.setIdentity();
	float data[] = {1, 2, 3, 4, 5};
	Vector<float, n_y> r(data);

	Vector<float, n_x> dx;
	SquareMatrix<float, n_x> dP;
	float beta = 0;
	kalman_correct<float, 6, 5>(P, C, R, r, dx, dP, beta);

	float data_check[] = {0.5, 1, 1.5, 2, 2.5, 0};
	Vector<float, n_x> dx_check(data_check);
	EXPECT_EQ(dx, dx_check);
}
