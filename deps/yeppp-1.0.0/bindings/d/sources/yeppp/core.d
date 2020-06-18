/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under 2-clause BSD license.
 * See LICENSE.txt for details.
 *
 */

module yeppp.core;
import yeppp.types;

extern (C) {
	extern Status yepCore_DotProduct_V64fV64f_S64f(const double* x, const double* y, out double dotProduct, size_t length);
}
