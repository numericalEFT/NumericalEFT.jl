/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under 2-clause BSD license.
 * See LICENSE.txt for details.
 *
 */

#include <R.h>
#include <yepLibrary.h>
#include <yepCore.h>
#include <yepMath.h>
#include <Rdefines.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

void R_init_libyerrr(DllInfo *info) {
	yepLibrary_Init();
}

void R_unload_libyerrr(DllInfo *info) {
	yepLibrary_Release();
}

SEXP yerCore_SumSquares_V64f_S64f(SEXP RvecX) {
	SEXP Rresult;
	double *vecX = NULL, result = 0.0;
	int n = 0;
	vecX = REAL(RvecX);
	n = length(RvecX);
	yepCore_SumSquares_V64f_S64f(vecX, &result, n);
	PROTECT(Rresult = NEW_NUMERIC(1));
	REAL(Rresult)[0] = result;
	UNPROTECT(1);
	return Rresult;
}

SEXP yerCore_DotProduct_V64fV64f_S64f(SEXP RvecX, SEXP RvecY) {
	SEXP Rresult;
	double *vecX = NULL, *vecY = NULL, result = 0.0;
	int n = 0;
	vecX = REAL(RvecX);
	vecY = REAL(RvecY);
	n = length(RvecX);
	yepCore_DotProduct_V64fV64f_S64f(vecX, vecY, &result, n);
	PROTECT(Rresult = NEW_NUMERIC(1));
	REAL(Rresult)[0] = result;
	UNPROTECT(1);
	return Rresult;
}

SEXP yerCore_Add_V64fV64f_V64f(SEXP RvecX, SEXP RvecY) {
	SEXP RvecZ;
	int n = 0;
	double *vecX = NULL, *vecY = NULL, *vecZ = NULL;
	vecX = REAL(RvecX);
	vecY = REAL(RvecY);
	n = length(RvecX);
	PROTECT(RvecZ = NEW_NUMERIC(n));
	vecZ = REAL(RvecZ);
	yepCore_Add_V64fV64f_V64f(vecX, vecY, vecZ, n);
	UNPROTECT(1);
	return RvecZ;
}

SEXP yerCore_Subtract_V64fV64f_V64f(SEXP RvecX, SEXP RvecY) {
	SEXP RvecZ;
	int n = 0;
	double *vecX = NULL, *vecY = NULL, *vecZ = NULL;
	vecX = REAL(RvecX);
	vecY = REAL(RvecY);
	n = length(RvecX);
	PROTECT(RvecZ = NEW_NUMERIC(n));
	vecZ = REAL(RvecZ);
	yepCore_Subtract_V64fV64f_V64f(vecX, vecY, vecZ, n);
	UNPROTECT(1);
	return RvecZ;
}

SEXP yerCore_Multiply_V64fV64f_V64f(SEXP RvecX, SEXP RvecY) {
	SEXP RvecZ;
	int n = 0;
	double *vecX = NULL, *vecY = NULL, *vecZ = NULL;
	vecX = REAL(RvecX);
	vecY = REAL(RvecY);
	n = length(RvecX);
	PROTECT(RvecZ = NEW_NUMERIC(n));
	vecZ = REAL(RvecZ);
	yepCore_Multiply_V64fV64f_V64f(vecX, vecY, vecZ, n);
	UNPROTECT(1);
	return RvecZ;
}

SEXP yerMath_Log_V64f_V64f(SEXP RvecX) {
	SEXP RvecY;
	int n = 0;
	double *vecX = NULL, *vecY = NULL;
	vecX = REAL(RvecX);
	n = length(RvecX);
	PROTECT(RvecY = NEW_NUMERIC(n));
	vecY = REAL(RvecY);
	yepMath_Log_V64f_V64f(vecX, vecY, n);
	UNPROTECT(1);
	return RvecY;
}
