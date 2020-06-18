#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

import yeppp.module

import yeppp.library.math.x86
import yeppp.library.math.x64
import yeppp.library.math.arm

def generate_log(module):
	with yeppp.module.Function(module, 'Log', 'Natural logarithm') as function:
		function.assembly_implementations.append(yeppp.library.math.x64.Log_V64f_V64f)

		function.c_documentation = """
@brief	Computes natural logarithm on an array of %(InputType0)s elements.
@param[in]	x	Pointer to the array of elements on which logarithm will be computed.
@param[out]	y	Pointer the array where the computed logarithms will be stored.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.java_documentation = """
@brief	Computes natural logarithm on %(InputType0)s elements.
@param[in]	xArray	Input array.
@param[in]   xOffset Offset of the first element in @a xArray.
@param[out]	yArray	Output array.
@param[in]   yOffset Offset of the first element in @a yArray.
@param[in]	length	The length of the subarrays to be used in computation.
"""
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(InputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = yepBuiltin_Log_%(InputType0)s_%(OutputType0)s(x);
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepMath_Log_V64f_V64f(x, y, YepSize length)")

def generate_exp(module):
	with yeppp.module.Function(module, 'Exp', 'Base-e exponent') as function:
		function.assembly_implementations.append(yeppp.library.math.x64.Exp_V64f_V64f)

		function.c_documentation = """
@brief	Computes base-e exponent on an array of %(InputType0)s elements.
@param[in]	x	Pointer to the array of elements on which exponent will be computed.
@param[out]	y	Pointer the array where the computed exponents will be stored.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.java_documentation = """
@brief	Computes exponent on %(InputType0)s elements.
@param[in]	xArray	Input array.
@param[in]   xOffset Offset of the first element in @a xArray.
@param[out]	yArray	Output array.
@param[in]   yOffset Offset of the first element in @a yArray.
@param[in]	length	Length of the subarrays to be used in computation.
"""
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(InputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = yepBuiltin_Exp_%(InputType0)s_%(OutputType0)s(x);
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepMath_Exp_V64f_V64f(x, y, YepSize length)")

def generate_sin(module):
	with yeppp.module.Function(module, 'Sin', 'Sine') as function:
		function.assembly_implementations.append(yeppp.library.math.x64.Sin_V64f_V64f)

		function.c_documentation = """
@brief	Computes sine on an array of %(InputType0)s elements.
@param[in]	x	Pointer to the array of elements on which sine will be computed.
@param[out]	y	Pointer the array where the computed sines will be stored.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.java_documentation = """
@brief	Computes sine on %(InputType0)s elements.
@param[in]	xArray	Input array.
@param[in]   xOffset Offset of the first element in @a xArray.
@param[out]	yArray	Output array.
@param[in]   yOffset Offset of the first element in @a yArray.
@param[in]	length	The length of the subarrays to be used in computation.
"""
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(InputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = yepBuiltin_Sin_%(InputType0)s_%(OutputType0)s(x);
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepMath_Sin_V64f_V64f(x, y, YepSize length)")

def generate_cos(module):
	with yeppp.module.Function(module, 'Cos', 'Cosine') as function:
		function.assembly_implementations.append(yeppp.library.math.x64.Cos_V64f_V64f)

		function.c_documentation = """
@brief	Computes cosine on an array of %(InputType0)s elements.
@param[in]	x	Pointer to the array of elements on which cosine will be computed.
@param[out]	y	Pointer the array where the computed cosines will be stored.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.java_documentation = """
@brief	Computes cosine on %(InputType0)s elements.
@param[in]	xArray	Input array.
@param[in]   xOffset Offset of the first element in @a xArray.
@param[out]	yArray	Output array.
@param[in]   yOffset Offset of the first element in @a yArray.
@param[in]	length	The length of the subarrays to be used in computation.
"""
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(InputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = yepBuiltin_Cos_%(InputType0)s_%(OutputType0)s(x);
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepMath_Cos_V64f_V64f(x, y, YepSize length)")

def generate_tan(module):
	with yeppp.module.Function(module, 'Tan', 'Tangent') as function:
		function.assembly_implementations.append(yeppp.library.math.x64.Tan_V64f_V64f_Bulldozer)
	
		function.c_documentation = """
@brief	Computes tangent on an array of %(InputType0)s elements.
@param[in]	x	Pointer to the array of elements on which tangent will be computed.
@param[out]	y	Pointer the array where the computed tangents will be stored.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.java_documentation = """
@brief	Computes tangent on %(InputType0)s elements.
@param[in]	xArray	Input array.
@param[in]   xOffset Offset of the first element in @a xArray.
@param[out]	yArray	Output array.
@param[in]   yOffset Offset of the first element in @a yArray.
@param[in]	length	The length of the slices of @a xArray and @a yArray to use in computation.
"""
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(InputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = yepBuiltin_Tan_%(InputType0)s_%(OutputType0)s(x);
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepMath_Tan_V64f_V64f(x, y, YepSize length)")

def generate_evaluate_polynomial(module):
	with yeppp.module.Function(module, 'EvaluatePolynomial', 'Polynomial evaluation') as function:
		function.assembly_implementations = [yeppp.library.math.x64.EvaluatePolynomial_VfVf_Vf_SSE,
											 yeppp.library.math.x64.EvaluatePolynomial_V64fV64f_V64f_Bonnell,
											 yeppp.library.math.x64.EvaluatePolynomial_V32fV32f_V32f_Bonnell,
											 yeppp.library.math.x64.EvaluatePolynomial_VfVf_Vf_AVX,
											 yeppp.library.math.arm.EvaluatePolynomial_V32fV32f_V32f,
											 yeppp.library.math.arm.EvaluatePolynomial_V64fV64f_V64f]

		function.c_documentation = """
@brief	Evaluates polynomial with %(InputType0)s coefficients on an array of %(InputType0)s elements.
@param[in]	x	Pointer to the array of elements on which the polynomial will be evaluated.
@param[in]	coef	Pointer to the array of polynomial coefficients.
@param[out]	y	Pointer the array where the result of polynomial evaluation will be stored.
@param[in]	coefCount	Number of polynomial coefficients. Should equal the polynomial degree plus one.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.java_documentation = """
@brief	Evaluates polynomial with %(InputType0)s coefficients on an array of %(InputType0)s elements.
@param[in]	xArray	Array of elements on which the polynomial will be evaluated.
@param[in]   xOffset Offset of the first element in @a xArray.
@param[in]	coefArray	Array of polynomial coefficients.
@param[in]	coefOffset	Offset of the first element in @a yArray.
@param[out]	yArray	Array where the result of polynomial evaluation will be stored.
@param[in]   yOffset Offset of the first element in @a yArray.
@param[in]	coefCount	The length of the slice of @a coef to be used in computation.
@param[in]	length	The length of the slice of @a xArray and @a yArray to use in computation.
"""
		function.c_implementation = """
if YEP_UNLIKELY(coefCount == 0) {
	return YepStatusInvalidArgument;
}
while (length-- != 0) {
	const Yep%(InputType0)s x = *xPointer++;
	Yep%(OutputType0)s y = coefPointer[coefCount - 1];
	for (YepSize coefIndex = coefCount - 1; coefIndex != 0; coefIndex--) {
		const Yep%(InputType0)s coef = coefPointer[coefIndex - 1];
		y = yepBuiltin_MultiplyAdd_%(OutputType0)s%(InputType1)s%(InputType0)s_%(OutputType0)s(y, x, coef);
	}
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepMath_EvaluatePolynomial_V32fV32f_V32f(coef[coefCount], x, y, YepSize coefCount: coefCount != 0, YepSize length)")
		function.generate("yepMath_EvaluatePolynomial_V64fV64f_V64f(coef[coefCount], x, y, YepSize coefCount: coefCount != 0, YepSize length)")

if __name__ == '__main__':
	with yeppp.module.Module('Math', 'Vector mathematical functions') as module:
		generate_log(module)
		generate_exp(module)
		generate_sin(module)
		generate_cos(module)
		generate_tan(module)
		generate_evaluate_polynomial(module)
# 		generate_sqrt(module)
