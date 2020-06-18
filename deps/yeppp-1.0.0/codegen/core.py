#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

import yeppp.module

import yeppp.library.core.x86
import yeppp.library.core.x64
import yeppp.library.core.arm
from yeppp.test import *

def generate_add(module):
	with yeppp.module.Function(module, 'Add', 'Addition') as function:
		function.java_documentation = """
@brief	Adds corresponding elements in two %(InputType0)s arrays. Produces an array of %(OutputType0)s elements.
"""
		function.c_documentation = """
@brief	Adds corresponding elements in two %(InputType0)s arrays. Produces an array of %(OutputType0)s elements.
@param[in]	x	Pointer to the first addend array of %(InputType0)s elements.
@param[in]	y	Pointer to the second addend array of %(InputType1)s elements.
@param[out]	sum	Pointer to the summand array of %(OutputType0)s elements.
@param[in]	length	Length of the arrays specified by @a x, @a y, and @a sum.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.AddSub_VXusVXus_VYus_SSE) 
		function.assembly_implementations.append(yeppp.library.core.x64.AddSub_VXusVXus_VYus_AVX)
		function.assembly_implementations.append(yeppp.library.core.x64.AddSubMulMinMax_VfVf_Vf)
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VXus_VFPv3)
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VXus_NEON)
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VYus_NEON)
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s sum = x + y;
	*sumPointer++ = sum;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Add_V8sV8s_V8s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V8sV8s_V16s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V8uV8u_V16u(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V16sV16s_V16s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V16sV16s_V32s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V16uV16u_V32u(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V32sV32s_V32s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V32sV32s_V64s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V32uV32u_V64u(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V64sV64s_V64s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V32fV32f_V32f(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V64fV64f_V64f(x, y, sum, YepSize length)")

		function.java_documentation = """
@brief	Adds a constant to %(InputType0)s array elements. Produces an array of %(OutputType0)s elements.
"""
		function.c_documentation = """
@brief	Adds a constant to %(InputType0)s array elements. Produces an array of %(OutputType0)s elements.
@param[in]	x	Pointer to the addend array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant to be added.
@param[out]	sum	Pointer to the summand array of %(OutputType0)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a sum.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.AddSub_VXusSXus_VYus_SSE) 
		function.assembly_implementations.append(yeppp.library.core.x64.AddSub_VXusSXus_VYus_AVX)
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s sum = x + y;
	*sumPointer++ = sum;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Add_V8sS8s_V8s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V8sS8s_V16s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V8uS8u_V16u(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V16sS16s_V16s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V16sS16s_V32s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V16uS16u_V32u(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V32sS32s_V32s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V32uS32u_V64u(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V32sS32s_V64s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V64sS64s_V64s(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V32fS32f_V32f(x, y, sum, YepSize length)")
		function.generate("yepCore_Add_V64fS64f_V64f(x, y, sum, YepSize length)")

		function.java_documentation = """
@brief	Adds corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
"""
		function.c_documentation = """
@brief	Adds corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
@param[in,out]	x	Pointer to the first addend array of %(InputType0)s elements.
@param[in]	y	Pointer to the second addend array of %(InputType1)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	const Yep%(OutputType0)s y = *yPointer++;
	x += y;
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.generate("yepCore_Add_IV8sV8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Add_IV16sV16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Add_IV32sV32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Add_IV64sV64s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Add_IV32fV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Add_IV64fV64f_IV64f(x, y, YepSize length)")

		function.java_documentation = """
@brief	Adds a constant to %(InputType0)s array elements and writes the result to the same array.
"""
		function.c_documentation = """
@brief	Adds a constant to %(InputType0)s array elements and writes the result to the same array.
@param[in,out]	x	Pointer to the addend array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant to be added.
@param[in]	length	Length of the array specified by @a x.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	x += y;
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.generate("yepCore_Add_IV8sS8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Add_IV16sS16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Add_IV32sS32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Add_IV64sS64s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Add_IV32fS32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Add_IV64fS64f_IV64f(x, y, YepSize length)")

def generate_subtract(module):
	with yeppp.module.Function(module, 'Subtract', 'Subtraction') as function:
		function.java_documentation = """
@brief	Subtracts corresponding elements in two %(InputType0)s arrays. Produces an array of %(OutputType0)s elements.
"""
		function.c_documentation = """
@brief	Subtracts corresponding elements in two %(InputType0)s arrays. Produces an array of %(OutputType0)s elements.
@param[in]	x	Pointer to the minuend array of %(InputType0)s elements.
@param[in]	y	Pointer to the subtrahend array of %(InputType1)s elements.
@param[out]	diff	Pointer to the difference array of %(OutputType0)s elements.
@param[in]	length	Length of the arrays specified by @a x, @a y, and @a diff.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.AddSub_VXusVXus_VYus_SSE) 
		function.assembly_implementations.append(yeppp.library.core.x64.AddSub_VXusVXus_VYus_AVX)
		function.assembly_implementations.append(yeppp.library.core.x64.AddSubMulMinMax_VfVf_Vf) 
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VXus_VFPv3)
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VXus_NEON)
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VYus_NEON)
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s diff = x - y;
	*diffPointer++ = diff;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Subtract_V8sV8s_V8s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V8sV8s_V16s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V8uV8u_V16u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V16sV16s_V16s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V16sV16s_V32s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V16uV16u_V32u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V32sV32s_V32s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V32sV32s_V64s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V32uV32u_V64u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V64sV64s_V64s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V32fV32f_V32f(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V64fV64f_V64f(x, y, diff, YepSize length)")
	
		function.java_documentation = """
@brief	Subtracts corresponding elements in two %(InputType0)s arrays. Produces an array of %(OutputType0)s elements.
"""
		function.c_documentation = """
@brief	Subtracts a constant from %(InputType0)s array elements. Produces an array of %(OutputType0)s elements.
@param[in]	x	Pointer to the minuend array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant to be subtracted.
@param[out]	diff	Pointer to the difference array of %(OutputType0)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a diff.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s diff = x - y;
	*diffPointer++ = diff;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Subtract_V8sS8s_V8s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V8sS8s_V16s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V8uS8u_V16u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V16sS16s_V16s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V16sS16s_V32s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V16uS16u_V32u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V32sS32s_V32s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V32sS32s_V64s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V32uS32u_V64u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V64sS64s_V64s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V32fS32f_V32f(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_V64fS64f_V64f(x, y, diff, YepSize length)")

		function.java_documentation = """
@brief	Subtracts %(InputType1)s array elements from a constant. Produces an array of %(OutputType0)s elements.
"""
		function.c_documentation = """
@brief	Subtracts %(InputType1)s array elements from a constant. Produces an array of %(OutputType0)s elements.
@param[in]	x	The %(InputType0)s constant to be subtracted from.
@param[in]	y	Pointer to the subtrahend array of %(InputType1)s elements.
@param[out]	diff	Pointer to the difference array of %(OutputType0)s elements.
@param[in]	length	Length of the arrays specified by @a y and @a diff.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s diff = x - y;
	*diffPointer++ = diff;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Subtract_S8sV8s_V8s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S8sV8s_V16s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S8uV8u_V16u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S16sV16s_V16s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S16sV16s_V32s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S16uV16u_V32u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S32sV32s_V32s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S32sV32s_V64s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S32uV32u_V64u(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S64sV64s_V64s(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S32fV32f_V32f(x, y, diff, YepSize length)")
		function.generate("yepCore_Subtract_S64fV64f_V64f(x, y, diff, YepSize length)")

		function.java_documentation = """
@brief	Subtracts corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
"""
		function.c_documentation = """
@brief	Subtracts corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
@param[in,out]	x	Pointer to the minuend array of %(InputType0)s elements.
@param[in]	y	Pointer to the subtrahend array of %(InputType1)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	const Yep%(OutputType0)s y = *yPointer++;
	x -= y;
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.generate("yepCore_Subtract_IV8sV8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV16sV16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV32sV32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV64sV64s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV32fV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV64fV64f_IV64f(x, y, YepSize length)")

		function.java_documentation = """
@brief	Subtracts corresponding elements in two %(InputType0)s arrays and writes the result to the second array.
"""
		function.c_documentation = """
@brief	Subtracts corresponding elements in two %(InputType0)s arrays and writes the result to the second array.
@param[in]	x	Pointer to the minuend array of %(InputType0)s elements.
@param[in,out]	y	Pointer to the subtrahend array of %(InputType1)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	Yep%(OutputType0)s y = *yPointer;
	y = x - y;
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepCore_Subtract_V8sIV8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_V16sIV16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_V32sIV32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_V64sIV64s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_V32fIV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Subtract_V64fIV64f_IV64f(x, y, YepSize length)")

		function.java_documentation = """
@brief	Subtracts a constant from %(InputType0)s array elements and writes the result to the same array.
"""
		function.c_documentation = """
@brief	Subtracts a constant from %(InputType0)s array elements and writes the result to the same array.
@param[in,out]	x	Pointer to the minuend array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant to be subtracted.
@param[in]	length	Length of the array specified by @a x.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	x -= y;
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.generate("yepCore_Subtract_IV8sS8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV16sS16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV32sS32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV64sS64s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV32fS32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Subtract_IV64fS64f_IV64f(x, y, YepSize length)")

		function.java_documentation = """
@brief	Subtracts %(InputType1)s array elements from a constant and writes the result to the same array.
"""
		function.c_documentation = """
@brief	Subtracts %(InputType1)s array elements from a constant and writes the result to the same array.
@param[in]	x	The %(InputType0)s constant to be subtracted from.
@param[in,out]	y	Pointer to the subtrahend array of %(InputType1)s elements.
@param[in]	length	Length of the array specified by @a y.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s y = *yPointer;
	y = x - y;
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepCore_Subtract_S8sIV8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_S16sIV16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_S32sIV32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_S64sIV64s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Subtract_S32fIV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Subtract_S64fIV64f_IV64f(x, y, YepSize length)")

def generate_negate(module):
	with yeppp.module.Function(module, 'Negate', 'Negation') as function:
		function.java_documentation = """
@brief	Negates elements in %(InputType0)s array.
"""
		function.c_documentation = """
@brief	Negates elements in %(InputType0)s array.
@param[in]	x	Pointer to the array of %(InputType0)s elements to be negated.
@param[out]	y	Pointer to the %(OutputType0)s array to store negated elements.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = []
#		function.assembly_implementations = [yeppp.library.core.x86.Negate_Vf_Vf_implementation]
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = -x;
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepCore_Negate_V8s_V8s(x, y, YepSize length)")
		function.generate("yepCore_Negate_V16s_V16s(x, y, YepSize length)")
		function.generate("yepCore_Negate_V32s_V32s(x, y, YepSize length)")
		function.generate("yepCore_Negate_V64s_V64s(x, y, YepSize length)")
		function.generate("yepCore_Negate_V32f_V32f(x, y, YepSize length)")
		function.generate("yepCore_Negate_V64f_V64f(x, y, YepSize length)")
	
		function.java_documentation = """
@brief	Negates elements in %(InputType0)s array and writes the results to the same array.
"""
		function.c_documentation = """
@brief	Negates elements in %(InputType0)s array and writes the results to the same array.
@param[in,out]	v	Pointer to the array of %(InputType0)s elements to be negated.
@param[in]	length	Length of the array specified by @a v.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s v = *vPointer;
	const Yep%(OutputType0)s minusV = -v;
	*vPointer++ = minusV;
}
return YepStatusOk;
"""
		function.generate("yepCore_Negate_IV8s_IV8s(v, YepSize length)")
		function.generate("yepCore_Negate_IV16s_IV16s(v, YepSize length)")
		function.generate("yepCore_Negate_IV32s_IV32s(v, YepSize length)")
		function.generate("yepCore_Negate_IV64s_IV64s(v, YepSize length)")
		function.generate("yepCore_Negate_IV32f_IV32f(v, YepSize length)")
		function.generate("yepCore_Negate_IV64f_IV64f(v, YepSize length)")
	
def generate_multiply(module):
	with yeppp.module.Function(module, 'Multiply', 'Multiplication') as function:
		function.java_documentation = """
@brief	Multiples corresponding elements in two %(InputType0)s arrays, producing an array of %(OutputType0)s elements.
"""
		function.c_documentation = """
@brief	Multiples corresponding elements in two %(InputType0)s arrays, producing an array of %(OutputType0)s elements.
@param[in]	x	Pointer to the first factor array of %(InputType0)s elements.
@param[in]	y	Pointer to the second factor array of %(InputType1)s elements.
@param[out]	product	Pointer to the product array of %(OutputType0)s elements.
@param[in]	length	Length of the arrays specified by @a x, @a y, and @a product.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.Multiply_VXuVXu_VXu)
		function.assembly_implementations.append(yeppp.library.core.x64.Multiply_V16usV16us_V32us)
		function.assembly_implementations.append(yeppp.library.core.x64.Multiply_V32usV32us_V64us)
		function.assembly_implementations.append(yeppp.library.core.x64.AddSubMulMinMax_VfVf_Vf)
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VXus_VFPv3)
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VXus_NEON)
		function.assembly_implementations.append(yeppp.library.core.arm.AddSubMul_VXusVXus_VYus_NEON)
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s product = x * y;
	*productPointer++ = product;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Multiply_V8sV8s_V8s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V8sV8s_V16s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V8uV8u_V16u(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V16sV16s_V16s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V16sV16s_V32s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V16uV16u_V32u(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V32sV32s_V32s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V32sV32s_V64s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V32uV32u_V64u(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V64sV64s_V64s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V32fV32f_V32f(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V64fV64f_V64f(x, y, product, YepSize length)")
	
		function.java_documentation = """
@brief	Multiplies %(InputType0)s array elements by a constant. Produces an array of %(OutputType0)s elements.
"""
		function.c_documentation = """
@brief	Multiplies %(InputType0)s array elements by a constant. Produces an array of %(OutputType0)s elements.
@param[in]	x	Pointer to the factor array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant to be multiplied by.
@param[out]	product	Pointer to the product array of %(OutputType0)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a product.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s product = x * y;
	*productPointer++ = product;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Multiply_V8sS8s_V8s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V8sS8s_V16s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V8uS8u_V16u(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V16sS16s_V16s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V16sS16s_V32s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V16uS16u_V32u(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V32sS32s_V32s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V32sS32s_V64s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V32uS32u_V64u(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V64sS64s_V64s(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V32fS32f_V32f(x, y, product, YepSize length)")
		function.generate("yepCore_Multiply_V64fS64f_V64f(x, y, product, YepSize length)")

		function.java_documentation = """
@brief	Multiplies corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
"""
		function.c_documentation = """
@brief	Multiplies corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
@param[in,out]	x	Pointer to the first factor array of %(InputType0)s elements.
@param[in]	y	Pointer to the second factor array of %(InputType1)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	const Yep%(OutputType0)s y = *yPointer++;
	x *= y;
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Multiply_IV8sV8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV16sV16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV32sV32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV64sV64s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV32fV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV64fV64f_IV64f(x, y, YepSize length)")

		function.java_documentation = """
@brief	Multiplies %(InputType0)s array elements by a constant and writes the result to the same array.
"""
		function.c_documentation = """
@brief	Multiplies %(InputType0)s array elements by a constant and writes the result to the same array.
@param[in,out]	x	Pointer to the factor array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant factor.
@param[in]	length	Length of the array specified by @a x.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	x *= y;
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Multiply_IV8sS8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV16sS16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV32sS32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV64sS64s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV32fS32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Multiply_IV64fS64f_IV64f(x, y, YepSize length)")

def generate_multiply_add(module):
	with yeppp.module.Function(module, 'MultiplyAdd', 'Multiplication and addition') as function:

		function.c_documentation = """
@brief	Multiples corresponding elements in two %(InputType0)s arrays and adds corresponding elements of the third array, producing an array of %(OutputType0)s elements.
@param[in]	x	Pointer to the first factor array of %(InputType0)s elements.
@param[in]	y	Pointer to the second factor array of %(InputType1)s elements.
@param[in]	z	Pointer to the addend array of %(InputType2)s elements.
@param[out]	mac	Pointer to the resulting array of %(OutputType0)s elements.
@param[in]	length	Length of the arrays specified by @a x, @a y, @a z, and @a mac.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s z = *zPointer++;
	const Yep%(OutputType0)s mac = x * y + z;
	*macPointer++ = mac;
}
return YepStatusOk;
"""
		function.generate("yepCore_MultiplyAdd_V32fV32fV32f_V32f(x, y, z, mac, YepSize length)")
		function.generate("yepCore_MultiplyAdd_V64fV64fV64f_V64f(x, y, z, mac, YepSize length)")

		function.c_documentation = """
@brief	Computes pairwise products of %(InputType0)s elements in two arrays and then adds the third %(InputType2)s array to the result, overwriting the third array.
@param[in]	x	Pointer the first input array of %(InputType0)s elements to be multiplied.
@param[in]	y	Pointer the second input array of %(InputType1)s elements to be multiplied.
@param[in,out]	z	Pointer the input/output array of %(InputType2)s elements to be added to the intermediate multiplication result.
@param[in]	length	Length of the arrays pointed by @a xPointer, @a yPointer, and @a zPointer.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	Yep%(OutputType0)s z = *zPointer;
	z = x * y + z;
	*zPointer++ = z;
}
return YepStatusOk;
"""
		function.generate("yepCore_MultiplyAdd_V32fS32fIV32f_IV32f(x, y, z, YepSize length)")
		function.generate("yepCore_MultiplyAdd_V64fS64fIV64f_IV64f(x, y, z, YepSize length)")

def generate_divide(module):
	with yeppp.module.Function(module, 'Divide', 'Division') as function:
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s fraction = x / y;
	*fractionPointer++ = fraction;
}
return YepStatusOk;
"""
		function.generate("yepCore_Divide_V32fV32f_V32f(x, y, fraction, YepSize length)")
		function.generate("yepCore_Divide_V64fV64f_V64f(x, y, fraction, YepSize length)")
	
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s fraction = x / y;
	*fractionPointer++ = fraction;
}
return YepStatusOk;
"""
		function.generate("yepCore_Divide_V32fS32f_V32f(x, y, fraction, YepSize length)")
		function.generate("yepCore_Divide_V64fS64f_V64f(x, y, fraction, YepSize length)")
	
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s fraction = x / y;
	*fractionPointer++ = fraction;
}
return YepStatusOk;
"""
		function.generate("yepCore_Divide_S32fV32f_V32f(x, y, fraction, YepSize length)")
		function.generate("yepCore_Divide_S64fV64f_V64f(x, y, fraction, YepSize length)")
	
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	const Yep%(OutputType0)s y = *yPointer++;
	x /= y;
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.generate("yepCore_Divide_IV32fV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Divide_IV64fV64f_IV64f(x, y, YepSize length)")
	
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	Yep%(OutputType0)s y = *yPointer;
	y = x / y;
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepCore_Divide_V32fIV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Divide_V64fIV64f_IV64f(x, y, YepSize length)")
	
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	x /= y;
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.generate("yepCore_Divide_IV32fS32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Divide_IV64fS64f_IV64f(x, y, YepSize length)")
	
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s y = *yPointer;
	y = x / y;
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepCore_Divide_S32fIV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Divide_S64fIV64f_IV64f(x, y, YepSize length)")
	
def generate_reciprocal(module):
	with yeppp.module.Function(module, 'Reciprocal', 'Reciprocal') as function:
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = Yep%(OutputType0)s(1) / x;
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepCore_Reciprocal_V32f_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V64f_V64f(x, y, YepSize length)")
	
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(InputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = Yep%(OutputType0)s(1) / yepBuiltin_Convert_%(InputType0)s_%(OutputType0)s(x);
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepCore_Reciprocal_V8u_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V8s_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V16u_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V16s_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V32u_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V32s_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V64u_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V64s_V32f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V8u_V64f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V8s_V64f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V16u_V64f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V16s_V64f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V32u_V64f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V32s_V64f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V64u_V64f(x, y, YepSize length)")
		function.generate("yepCore_Reciprocal_V64s_V64f(x, y, YepSize length)")
	
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s v = *vPointer;
	const Yep%(OutputType0)s rcpV = Yep%(OutputType0)s(1) / v;
	*vPointer++ = rcpV;
}
return YepStatusOk;
"""
		function.generate("yepCore_Reciprocal_IV32f_IV32f(v, YepSize length)")
		function.generate("yepCore_Reciprocal_IV64f_IV64f(v, YepSize length)")

def generate_convert(module):
	with yeppp.module.Function(module, 'Convert', 'Type conversion') as function:
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = yepBuiltin_Convert_%(InputType0)s_%(OutputType0)s(x);
	*yPointer++ = y;
}
return YepStatusOk;
"""
		function.generate("yepCore_Convert_V8s_V32f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V8u_V32f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V16s_V32f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V16u_V32f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V32s_V32f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V32u_V32f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V64s_V32f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V64u_V32f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V8s_V64f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V8u_V64f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V16s_V64f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V16u_V64f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V32s_V64f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V32u_V64f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V64s_V64f(x, y, YepSize length)")
		function.generate("yepCore_Convert_V64u_V64f(x, y, YepSize length)")

def generate_min(module):
	with yeppp.module.Function(module, 'Min', 'Minimum') as function:
		function.java_documentation = """
@brief	Computes the minimum of %(InputType0)s array elements.
"""
		function.c_documentation = """
@brief	Computes the minimum of %(InputType0)s array elements.
@param[in]	v	Pointer to the array of elements whose minimum will be computed.
@param[out]	minimum	Pointer to the variable where the minimum will be stored.
@param[in]	length	Length of the array specified by @a v. Must be non-zero.
"""
		function.assembly_implementations = list()
		function.c_implementation = """
Yep%(InputType0)s minimum = *vPointer++;
while (--length != 0) {
	const Yep%(InputType0)s v = *vPointer++;
	minimum = yepBuiltin_Min_%(InputType0)s%(InputType0)s_%(InputType0)s(v, minimum);
}
*minimumPointer++ = minimum;
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(v = Uniform())
		function.generate("yepCore_Min_V8s_S8s(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V8u_S8u(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V16s_S16s(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V16u_S16u(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V32s_S32s(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V32u_S32u(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V64s_S64s(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V64u_S64u(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V32f_S32f(v, minimum, YepSize length: length != 0)")
		function.generate("yepCore_Min_V64f_S64f(v, minimum, YepSize length: length != 0)")
	
		function.java_documentation = """
@brief	Computes pairwise minima of corresponding elements in two %(InputType0)s arrays.
"""
		function.c_documentation = """
@brief	Computes pairwise minima of corresponding elements in two %(InputType0)s arrays.
@param[in]	x	Pointer to the first array of %(InputType0)s elements.
@param[in]	y	Pointer to the second array of %(InputType1)s elements.
@param[out]	minimum	Pointer to the array of pairwise minimum elements.
@param[in]	length	Length of the arrays specified by @a x, @a y, and @a minimum.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.AddSubMulMinMax_VfVf_Vf)
		function.assembly_implementations.append(yeppp.library.core.arm.MinMax_VXusVXus_VXus_NEON)
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s minimum = yepBuiltin_Min_%(OutputType0)s%(OutputType0)s_%(OutputType0)s(x, y);
	*minimumPointer++ = minimum;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Min_V8sV8s_V8s(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V8uV8u_V8u(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V16sV16s_V16s(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V16uV16u_V16u(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V32sV32s_V32s(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V32uV32u_V32u(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V64sV32s_V64s(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V64uV32u_V64u(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V32fV32f_V32f(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V64fV64f_V64f(x, y, minimum, YepSize length)")
	
		function.java_documentation = """
@brief	Computes pairwise minima of %(InputType0)s array elements and a constant.
"""
		function.c_documentation = """
@brief	Computes pairwise minima of %(InputType0)s array elements and a constant.
@param[in]	x	Pointer to the first array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant.
@param[out]	minimum	Pointer to the array of pairwise minimum elements.
@param[in]	length	Length of the arrays specified by @a x, @a y, and @a minimum.
"""
		function.assembly_implementations = list()
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s minimum = yepBuiltin_Min_%(OutputType0)s%(OutputType0)s_%(OutputType0)s(x, y);
	*minimumPointer++ = minimum;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Min_V8sS8s_V8s(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V8uS8u_V8u(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V16sS16s_V16s(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V16uS16u_V16u(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V32sS32s_V32s(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V32uS32u_V32u(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V64sS32s_V64s(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V64uS32u_V64u(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V32fS32f_V32f(x, y, minimum, YepSize length)")
		function.generate("yepCore_Min_V64fS64f_V64f(x, y, minimum, YepSize length)")
	
		function.java_documentation = """
@brief	Computes pairwise minima of corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
"""
		function.c_documentation = """
@brief	Computes pairwise minima of corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
@param[in,out]	x	Pointer to the first array of %(InputType0)s elements.
@param[in]	y	Pointer to the second array of %(InputType1)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	const Yep%(OutputType0)s y = *yPointer++;
	x = yepBuiltin_Min_%(OutputType0)s%(OutputType0)s_%(OutputType0)s(x, y);
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Min_IV8sV8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Min_IV8uV8u_IV8u(x, y, YepSize length)")
		function.generate("yepCore_Min_IV16sV16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Min_IV16uV16u_IV16u(x, y, YepSize length)")
		function.generate("yepCore_Min_IV32sV32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Min_IV32uV32u_IV32u(x, y, YepSize length)")
		function.generate("yepCore_Min_IV64sV32s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Min_IV64uV32u_IV64u(x, y, YepSize length)")
		function.generate("yepCore_Min_IV32fV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Min_IV64fV64f_IV64f(x, y, YepSize length)")
	
		function.java_documentation = """
@brief	Computes pairwise minima of %(InputType0)s array elements and a constant and writes the result to the same array.
"""
		function.c_documentation = """
@brief	Computes pairwise minima of %(InputType0)s array elements and a constant and writes the result to the same array.
@param[in,out]	x	Pointer to the array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = list()
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	x = yepBuiltin_Min_%(OutputType0)s%(OutputType0)s_%(OutputType0)s(x, y);
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Min_IV8sS8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Min_IV8uS8u_IV8u(x, y, YepSize length)")
		function.generate("yepCore_Min_IV16sS16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Min_IV16uS16u_IV16u(x, y, YepSize length)")
		function.generate("yepCore_Min_IV32sS32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Min_IV32uS32u_IV32u(x, y, YepSize length)")
		function.generate("yepCore_Min_IV64sS32s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Min_IV64uS32u_IV64u(x, y, YepSize length)")
		function.generate("yepCore_Min_IV32fS32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Min_IV64fS64f_IV64f(x, y, YepSize length)")

def generate_max(module):
	with yeppp.module.Function(module, 'Max', 'Maximum') as function:
		function.java_documentation = """
@brief	Computes the maximum of %(InputType0)s array elements.
"""
		function.c_documentation = """
@brief	Computes the maximum of %(InputType0)s array elements.
@param[in]	v	Pointer to the array of elements whose maximum will be computed.
@param[out]	maximum	Pointer to the variable where the maximum will be stored.
@param[in]	length	Length of the array specified by @a v. Must be non-zero.
"""
		function.assembly_implementations = list()
		function.c_implementation = """
Yep%(InputType0)s maximum = *vPointer++;
while (--length != 0) {
	const Yep%(InputType0)s v = *vPointer++;
	maximum = yepBuiltin_Max_%(InputType0)s%(InputType0)s_%(InputType0)s(v, maximum);
}
*maximumPointer = maximum;
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(v = Uniform())
		function.generate("yepCore_Max_V8s_S8s(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V8u_S8u(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V16s_S16s(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V16u_S16u(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V32s_S32s(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V32u_S32u(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V64s_S64s(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V64u_S64u(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V32f_S32f(v, maximum, YepSize length: length != 0)")
		function.generate("yepCore_Max_V64f_S64f(v, maximum, YepSize length: length != 0)")

		function.java_documentation = """
@brief	Computes pairwise maxima of corresponding elements in two %(InputType0)s arrays.
"""
		function.c_documentation = """
@brief	Computes pairwise maxima of corresponding elements in two %(InputType0)s arrays.
@param[in]	x	Pointer to the first array of %(InputType0)s elements.
@param[in]	y	Pointer to the second array of %(InputType1)s elements.
@param[out]	maximum	Pointer to the array of pairwise maximum elements.
@param[in]	length	Length of the arrays specified by @a x, @a y, and @a maximum.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.AddSubMulMinMax_VfVf_Vf)
		function.assembly_implementations.append(yeppp.library.core.arm.MinMax_VXusVXus_VXus_NEON)
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s y = *yPointer++;
	const Yep%(OutputType0)s maximum = yepBuiltin_Max_%(OutputType0)s%(OutputType0)s_%(OutputType0)s(x, y);
	*maximumPointer++ = maximum;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Max_V8sV8s_V8s(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V8uV8u_V8u(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V16sV16s_V16s(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V16uV16u_V16u(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V32sV32s_V32s(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V32uV32u_V32u(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V64sV32s_V64s(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V64uV32u_V64u(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V32fV32f_V32f(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V64fV64f_V64f(x, y, maximum, YepSize length)")
	
		function.java_documentation = """
@brief	Computes pairwise maxima of %(InputType0)s array elements and a constant.
"""
		function.c_documentation = """
@brief	Computes pairwise maxima of %(InputType0)s array elements and a constant.
@param[in]	x	Pointer to the first array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant.
@param[out]	maximum	Pointer to the array of pairwise maximum elements.
@param[in]	length	Length of the arrays specified by @a x, @a y, and @a maximum.
"""
		function.assembly_implementations = list()
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(OutputType0)s x = *xPointer++;
	const Yep%(OutputType0)s maximum = yepBuiltin_Max_%(OutputType0)s%(OutputType0)s_%(OutputType0)s(x, y);
	*maximumPointer++ = maximum;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Max_V8sS8s_V8s(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V8uS8u_V8u(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V16sS16s_V16s(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V16uS16u_V16u(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V32sS32s_V32s(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V32uS32u_V32u(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V64sS32s_V64s(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V64uS32u_V64u(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V32fS32f_V32f(x, y, maximum, YepSize length)")
		function.generate("yepCore_Max_V64fS64f_V64f(x, y, maximum, YepSize length)")
	
		function.java_documentation = """
@brief	Computes pairwise maxima of corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
"""
		function.c_documentation = """
@brief	Computes pairwise maxima of corresponding elements in two %(InputType0)s arrays and writes the result to the first array.
@param[in,out]	x	Pointer to the first array of %(InputType0)s elements.
@param[in]	y	Pointer to the second array of %(InputType1)s elements.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	const Yep%(OutputType0)s y = *yPointer++;
	x = yepBuiltin_Max_%(OutputType0)s%(OutputType0)s_%(OutputType0)s(x, y);
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Max_IV8sV8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Max_IV8uV8u_IV8u(x, y, YepSize length)")
		function.generate("yepCore_Max_IV16sV16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Max_IV16uV16u_IV16u(x, y, YepSize length)")
		function.generate("yepCore_Max_IV32sV32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Max_IV32uV32u_IV32u(x, y, YepSize length)")
		function.generate("yepCore_Max_IV64sV32s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Max_IV64uV32u_IV64u(x, y, YepSize length)")
		function.generate("yepCore_Max_IV32fV32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Max_IV64fV64f_IV64f(x, y, YepSize length)")
	
		function.java_documentation = """
@brief	Computes pairwise maxima of %(InputType0)s array elements and a constant and writes the result to the same array.
"""
		function.c_documentation = """
@brief	Computes pairwise maxima of %(InputType0)s array elements and a constant and writes the result to the same array.
@param[in,out]	x	Pointer to the array of %(InputType0)s elements.
@param[in]	y	The %(InputType1)s constant.
@param[in]	length	Length of the arrays specified by @a x and @a y.
"""
		function.assembly_implementations = list()
		function.c_implementation = """
while (length-- != 0) {
	Yep%(OutputType0)s x = *xPointer;
	x = yepBuiltin_Max_%(OutputType0)s%(OutputType0)s_%(OutputType0)s(x, y);
	*xPointer++ = x;
}
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(), y = Uniform())
		function.generate("yepCore_Max_IV8sS8s_IV8s(x, y, YepSize length)")
		function.generate("yepCore_Max_IV8uS8u_IV8u(x, y, YepSize length)")
		function.generate("yepCore_Max_IV16sS16s_IV16s(x, y, YepSize length)")
		function.generate("yepCore_Max_IV16uS16u_IV16u(x, y, YepSize length)")
		function.generate("yepCore_Max_IV32sS32s_IV32s(x, y, YepSize length)")
		function.generate("yepCore_Max_IV32uS32u_IV32u(x, y, YepSize length)")
		function.generate("yepCore_Max_IV64sS32s_IV64s(x, y, YepSize length)")
		function.generate("yepCore_Max_IV64uS32u_IV64u(x, y, YepSize length)")
		function.generate("yepCore_Max_IV32fS32f_IV32f(x, y, YepSize length)")
		function.generate("yepCore_Max_IV64fS64f_IV64f(x, y, YepSize length)")

def generate_min_max(module):
	with yeppp.module.Function(module, 'MinMax', 'Minimum and maximum') as function:
		function.c_documentation = None
		function.assembly_implementations = list()
		function.c_implementation = """
Yep%(InputType0)s minimum = *vPointer++;
Yep%(InputType0)s maximum = minimum;
while (--length != 0) {
	const Yep%(InputType0)s v = *vPointer++;
	maximum = yepBuiltin_Max_%(InputType0)s%(InputType0)s_%(InputType0)s(v, maximum);
	minimum = yepBuiltin_Min_%(InputType0)s%(InputType0)s_%(InputType0)s(v, minimum);
}
*minimumPointer = minimum;
*maximumPointer = maximum;
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(v = Uniform())
		function.generate("yepCore_MinMax_V8s_S8sS8s(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V8u_S8uS8u(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V16s_S16sS16s(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V16u_S16uS16u(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V32s_S32sS32s(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V32u_S32uS32u(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V64s_S64sS64s(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V64u_S64uS64u(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V32f_S32fS32f(v, minimum, maximum, YepSize length)")
		function.generate("yepCore_MinMax_V64f_S64fS64f(v, minimum, maximum, YepSize length)")
	
def generate_sum(module):
	with yeppp.module.Function(module, 'Sum', 'Sum') as function:
		function.java_documentation = """
@brief	Computes the sum of %(InputType0)s array elements.
"""
		function.c_documentation = """
@brief	Computes the sum of %(InputType0)s array elements.
@param[in]	v	Pointer to the array of elements which will be summed up.
@param[out]	sum	Pointer to the variable where the sum will be stored.
@param[in]	length	Length of the array specified by @a v. If @a length is zero, the computed sum will be 0.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.Sum_VXf_SXf_SSE)
		function.assembly_implementations.append(yeppp.library.core.x64.Sum_VXf_SXf_AVX)
		function.c_implementation = """
Yep%(InputType0)s sum = Yep%(InputType0)s(0);
while (length-- != 0) {
	const Yep%(InputType0)s v = *vPointer++;
	sum += v;
}
*sumPointer = sum;
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(v = Uniform(0.0, 1000.0))
		function.generate("yepCore_Sum_V32f_S32f(v, sum, YepSize length)")
		function.generate("yepCore_Sum_V64f_S64f(v, sum, YepSize length)")

def generate_sum_abs(module):
	with yeppp.module.Function(module, 'SumAbs', 'Sum of absolute values') as function:
		function.java_documentation = """
@brief	Computes the sum of absolute values of %(InputType0)s array elements.
"""
		function.c_documentation = """
@brief	Computes the sum of absolute values of %(InputType0)s array elements.
@param[in]	v	Pointer to the array of elements whose absolute values will be summed up.
@param[out]	sumAbs	Pointer to the variable where the sum of absolute values will be stored.
@param[in]	length	Length of the array specified by @a v. If @a length is zero, the computed sum will be 0.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.SumAbs_VXf_SXf_SSE)
		function.assembly_implementations.append(yeppp.library.core.x64.SumAbs_VXf_SXf_AVX)
		function.c_implementation = """
Yep%(InputType0)s sumAbs = Yep%(InputType0)s(0);
while (length-- != 0) {
	const Yep%(InputType0)s v = *vPointer++;
	sumAbs += yepBuiltin_Abs_%(InputType0)s_%(OutputType0)s(v);
}
*sumAbsPointer = sumAbs;
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(v = Uniform(-100.0, 100.0))
		function.generate("yepCore_SumAbs_V32f_S32f(v, sumAbs, YepSize length)")
		function.generate("yepCore_SumAbs_V64f_S64f(v, sumAbs, YepSize length)")

def generate_sum_squares(module):
	with yeppp.module.Function(module, 'SumSquares', 'Sum of squares (squared L2 norm)') as function:
		function.java_documentation = """
@brief	Computes the sum of squares of %(InputType0)s array elements.
"""
		function.c_documentation = """
@brief	Computes the sum of squares of %(InputType0)s array elements.
@param[in]	v	Pointer to the array of elements which will be squared (without write-back) and summed up.
@param[out]	sumSquares	Pointer to the variable where the sum of squares will be stored.
@param[in]	length	Length of the array specified by @a v. If @a length is zero, the computed sum of squares will be 0.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.SumSquares_VXf_SXf_SSE)
		function.assembly_implementations.append(yeppp.library.core.x64.SumSquares_VXf_SXf_AVX)
		function.c_implementation = """
Yep%(InputType0)s sumSquares = Yep%(InputType0)s(0);
while (length-- != 0) {
	const Yep%(InputType0)s v = *vPointer++;
	sumSquares += v * v;
}
*sumSquaresPointer = sumSquares;
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(v = Uniform())
		function.generate("yepCore_SumSquares_V32f_S32f(v, sumSquares, YepSize length)")
		function.generate("yepCore_SumSquares_V64f_S64f(v, sumSquares, YepSize length)")

def generate_dot_product(module):
	with yeppp.module.Function(module, 'DotProduct', 'Dot product') as function:
		function.java_documentation = """
@brief	Computes the dot product of two vectors of %(InputType0)s elements.
"""
		function.c_documentation = """
@brief	Computes the dot product of two vectors of %(InputType0)s elements.
@param[in]	x	Pointer to the first vector of elements.
@param[in]	y	Pointer to the second vector of elements.
@param[out]	dotProduct	Pointer to the variable where the dot product value will be stored.
@param[in]	length	Length of the vectors specified by @a x and @a y.
"""
		function.assembly_implementations = list()
		function.assembly_implementations.append(yeppp.library.core.x64.DotProduct_VXfVXf_SXf_SSE)
		function.assembly_implementations.append(yeppp.library.core.x64.DotProduct_VXfVXf_SXf_AVX)
		function.c_implementation = """
Yep%(InputType0)s dotProduct = Yep%(InputType0)s(0);
while (length-- != 0) {
	const Yep%(InputType0)s x = *xPointer++;
	const Yep%(InputType0)s y = *yPointer++;
	dotProduct += x * y;
}
*dotProductPointer = dotProduct;
return YepStatusOk;
"""
		function.unit_test = ReferenceUnitTest(x = Uniform(0.0, 100.0), y = Uniform(-200.0, -20.0))
		function.generate("yepCore_DotProduct_V32fV32f_S32f(x, y, dotProduct, YepSize length)")
		function.generate("yepCore_DotProduct_V64fV64f_S64f(x, y, dotProduct, YepSize length)")

def generate_gather(module):
	with yeppp.module.Function(module, 'Gather', 'Gather') as function:
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const YepSize index = YepSize(*indexPointer++);
	const Yep%(InputType0)s element = sourcePointer[index];
	*destinationPointer++ = element;
}
return YepStatusOk;
"""
		function.generate("yepCore_Gather_V8uV8u_V8u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V8uV16u_V8u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V8uV32u_V8u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V8uV64u_V8u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V16uV8u_V16u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V16uV16u_V16u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V16uV32u_V16u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V16uV64u_V16u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V32uV8u_V32u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V32uV16u_V32u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V32uV32u_V32u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V32uV64u_V32u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V64uV8u_V64u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V64uV16u_V64u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V64uV32u_V64u(source, index, destination, YepSize length)")
		function.generate("yepCore_Gather_V64uV64u_V64u(source, index, destination, YepSize length)")

def generate_scatter_increment(module):
	with yeppp.module.Function(module, 'ScatterIncrement', 'Scatter-increment') as function:
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const YepSize index = YepSize(*indexPointer++);
	basePointer[index] += 1;
}
return YepStatusOk;
"""
		function.generate("yepCore_ScatterIncrement_IV8uV8u_IV8u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV16uV8u_IV16u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV32uV8u_IV32u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV64uV8u_IV64u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV8uV16u_IV8u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV16uV16u_IV16u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV32uV16u_IV32u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV64uV16u_IV64u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV8uV32u_IV8u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV16uV32u_IV16u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV32uV32u_IV32u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV64uV32u_IV64u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV8uV64u_IV8u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV16uV64u_IV16u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV32uV64u_IV32u(base, index, YepSize length)")
		function.generate("yepCore_ScatterIncrement_IV64uV64u_IV64u(base, index, YepSize length)")

def generate_scatter_add(module):
	with yeppp.module.Function(module, 'ScatterAdd', 'Scatter-add') as function:
		function.c_documentation = None
		function.assembly_implementations = []
		function.c_implementation = """
while (length-- != 0) {
	const Yep%(InputType0)s weight = *weightPointer++;
	const YepSize index = YepSize(*indexPointer++);
	basePointer[index] += weight;
}
return YepStatusOk;
"""
		function.generate("yepCore_ScatterAdd_IV8uV8uV8u_IV8u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV16uV8uV8u_IV16u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV16uV8uV16u_IV16u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV8uV8u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV8uV16u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV8uV32u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV8uV8u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV8uV16u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV8uV32u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV8uV64u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV8uV16uV8u_IV8u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV16uV16uV8u_IV16u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV16uV16uV16u_IV16u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV16uV8u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV16uV16u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV16uV32u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV16uV8u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV16uV16u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV16uV32u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV16uV64u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV8uV32uV8u_IV8u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV16uV32uV8u_IV16u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV16uV32uV16u_IV16u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV32uV8u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV32uV16u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV32uV32u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV32uV8u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV32uV16u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV32uV32u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV32uV64u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV8uV64uV8u_IV8u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV16uV64uV8u_IV16u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV16uV64uV16u_IV16u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV64uV8u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV64uV16u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV32uV64uV32u_IV32u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV64uV8u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV64uV16u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV64uV32u_IV64u(base, index, weight, YepSize length)")
		function.generate("yepCore_ScatterAdd_IV64uV64uV64u_IV64u(base, index, weight, YepSize length)")

if __name__ == '__main__':
	with yeppp.module.Module('Core', 'Basic arithmetic operations') as module:
		generate_add(module)
		generate_subtract(module)
		generate_negate(module)
		generate_multiply(module)
# 		generate_multiply_add(module)
# 		generate_divide(module)
# 		generate_reciprocal(module)
# 		generate_convert(module)
		generate_min(module)
		generate_max(module)
# 		generate_min_max(module)
		generate_sum(module)
		generate_sum_abs(module)
		generate_sum_squares(module)
		generate_dot_product(module)
# 		generate_gather(module)
# 		generate_scatter_increment(module)
# 		generate_scatter_add(module)
