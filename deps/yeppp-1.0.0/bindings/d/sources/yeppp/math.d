/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under 2-clause BSD license.
 * See LICENSE.txt for details.
 *
 */

module yeppp.math;
import yeppp.types;

extern (C) {
	extern Status yepMath_Log_V64f_V64f(const double* x, double* y, size_t length);
}
