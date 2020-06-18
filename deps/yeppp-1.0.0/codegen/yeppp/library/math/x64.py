#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

__author__ = 'Marat'

from peachpy.x64 import *
from yeppp import Status
from yeppp.library.math import *
import math

def Map_Vf_Vf(scalarFunction, batchFunctionFull, batchFunctionFast, xPointer, yPointer, length, sourceAlignment, batchBytes, elementSize):
	TEST( xPointer, xPointer )
	JZ( 'return_null_pointer' )
	
	TEST( xPointer, elementSize - 1 )
	JNZ( 'return_misaligned_pointer' )
	
	TEST( yPointer, yPointer )
	JZ( 'return_null_pointer' )
	
	TEST( yPointer, elementSize - 1 )
	JNZ( 'return_misaligned_pointer' )
	
	TEST( length, length )
	JZ( 'return_ok' )

	if batchFunctionFull is not None:
		TEST( xPointer, sourceAlignment - 1 )
		JZ( 'source_%db_aligned' % sourceAlignment )
	
		# Alignment loop
		LABEL( 'source_%db_misaligned' % sourceAlignment )
		scalarFunction(xPointer, yPointer, is_prologue = True)
		ADD( xPointer, elementSize )
		ADD( yPointer, elementSize )
		SUB( length, 1 )
		JZ( 'return_ok' )
		TEST( xPointer, sourceAlignment - 1 )
		JNZ( 'source_%db_misaligned' % sourceAlignment )
	
		LABEL( 'source_%db_aligned' % sourceAlignment )
	
		SUB( length, batchBytes / elementSize )
		JB( 'process_restore' )
	
		ALIGN( 32 )
		if batchFunctionFast is not None:
			LABEL( 'process_batch_fast' )
			batchFunctionFast(xPointer, yPointer, 'process_batch_full')
			LABEL( 'process_batch_increment' )
		else:
			LABEL( 'process_batch_full' )
			batchFunctionFull(xPointer, yPointer)
		ADD( xPointer, batchBytes )
		ADD( yPointer, batchBytes )
		SUB( length, batchBytes / elementSize )
		if batchFunctionFast is not None:
			JAE( 'process_batch_fast' )
		else:
			JAE( 'process_batch_full' )
			
		LABEL( 'process_restore' )
		ADD( length, batchBytes / elementSize )
		JZ( 'return_ok' )

	LABEL( 'process_single')
	scalarFunction(xPointer, yPointer, is_prologue = False)
	ADD( xPointer, elementSize )
	ADD( yPointer, elementSize )
	SUB( length, 1 )
	JNZ( 'process_single' )

	LABEL( 'return_ok' )
	XOR( eax, eax )
	LABEL( 'return' )
	RET()
	
	LABEL( 'return_null_pointer' )
	MOV( eax, Status.NullPointer )
	JMP( 'return' )
	
	LABEL( 'return_misaligned_pointer' )
	MOV( eax, Status.MisalignedPointer )
	JMP( 'return' )

	if batchFunctionFull is not None and batchFunctionFast is not None:
		ALIGN( 16 )
		LABEL( 'process_batch_full' )
		batchFunctionFull(xPointer, yPointer)
		JMP( 'process_batch_increment' )

def SCALAR_LOG_SSE(xPointer, yPointer, is_prologue):
	if Target.get_int_eu_width() == 64:
		def SCALAR_COPY(destination, source):
			ASSUME.INITIALIZED( destination )
			MOVSD( destination, source )
	else:
		def SCALAR_COPY(destination, source):
			MOVAPS( destination, source )
	
	# x = *xPointer
	xmm_x = SSERegister()
	MOVSD( xmm_x, [xPointer] )

	# ne = as_ulong(x) >> 52
	xmm_ne = SSERegister()
	SCALAR_COPY( xmm_ne, xmm_x )
	PSRLQ( xmm_ne, 52 )

	# dx = as_double(as_ulong(x) | denormal_magic) - denormal_bias
	xmm_dx = SSERegister()
	SCALAR_COPY( xmm_dx, xmm_x )
	ORPS( xmm_dx, Constant.float64x2(Log.denormal_magic) )
	SUBSD( xmm_dx, Constant.float64(Log.denormal_magic) )

	# dmask = (ne == 0)
	if Target.has_sse4_1():
		xmm_dmask = xmm0
	else:
		xmm_dmask = SSERegister()
	PXOR( xmm_dmask, xmm_dmask )
	if Target.has_sse4_1():
		PCMPEQQ( xmm_dmask, xmm_ne )
	else:
		CMPEQSD( xmm_dmask, xmm_ne )

	# de = (as_ulong(dx) >> 52) - denormal_exponent_shift
	xmm_de = SSERegister()
	MOVDQA( xmm_de, xmm_dx )
	PSRLQ( xmm_de, 52 )
	PSUBQ( xmm_de, Constant.uint64x2(Log.denormal_exponent_shift) )

	# e = (de & dmask) | ne
	PAND( xmm_de, xmm_dmask )
	POR( xmm_de, xmm_ne )
	xmm_e = xmm_de
	
	# t = dmask ? dx : x
	if Target.has_sse4_1():
		PBLENDVB( xmm_x, xmm_dx, xmm_dmask )
		xmm_t = xmm_x
	else:
		PAND( xmm_dx, xmm_dmask )
		PANDN( xmm_dmask, xmm_x )
		POR( xmm_dmask, xmm_dx )
		xmm_t = xmm_dmask

	# t = (t & mantissa_mask) | default_exponent
	PAND( xmm_t, Constant.uint64x2(Log.mantissa_mask) )
	POR( xmm_t, Constant.float64x2(Log.one) )

	# amask = m >= sqrt2
	xmm_amask = SSERegister()
	LOAD.CONSTANT( xmm_amask, Constant.float64(Log.sqrt2) )
	CMPLTSD( xmm_amask, xmm_t )
	# If (amask) then e += 1
	PSUBQ( xmm_e, xmm_amask )
	# If (amask) then m *= 0.5
	PAND( xmm_amask, Constant.float64x2(Log.min_normal) )
	PSUBD( xmm_t, xmm_amask )

	# t = t - 1.0
	SUBSD( xmm_t, Constant.float64(Log.one) )

	# rf = c20
	xmm_rf = SSERegister()
	LOAD.CONSTANT( xmm_rf, Constant.float64(Log.c20) )

	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c19) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c18) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c17) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c16) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c15) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c14) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c13) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c12) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c11) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c10) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c9) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c8) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c7) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c6) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c5) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c4) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c3) )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, Constant.float64(Log.c2) )

	# rf = (rf * t) * t + t
	MULSD( xmm_rf, xmm_t )
	MULSD( xmm_rf, xmm_t )
	ADDSD( xmm_rf, xmm_t )

	# Convert e to double
	# e = as_double(as_ulong(e) + exponent_magic) - exponent_bias
	PADDQ( xmm_e, Constant.uint64x2(Log.exponent_magic) )
	SUBSD( xmm_e, Constant.float64x2(Log.exponent_bias) )

	# f = e * ln2_lo + rf
	xmm_f = SSERegister()
	SCALAR_COPY( xmm_f, xmm_e )
	MULSD( xmm_f, Constant.float64(Log.preFMA.ln2_lo) )
	ADDSD( xmm_f, xmm_rf )
	# f += e * ln2_hi
	MULSD( xmm_e, Constant.float64(Log.preFMA.ln2_hi) )
	ADDSD( xmm_f, xmm_e )

	if Target.has_sse4_1():
		xmm_x = xmm0
		# x = *xPointer
		MOVSD( xmm_x, [xPointer] )
		# if sign(x) == -1 then f = NaN
		BLENDVPD( xmm_f, Constant.float64x2(Log.nan), xmm_x )

		xmm_zero_mask = SSERegister()
		xmm_minus_inf = SSERegister()
		PXOR( xmm_zero_mask, xmm_zero_mask )
		LOAD.CONSTANT( xmm_minus_inf, Constant.float64(Log.minus_inf) )
		# zeroMask = (x == +0.0)
		PCMPEQQ( xmm_zero_mask, xmm_x )
		# f = (zeroMask) ? -inf : f
		PAND( xmm_minus_inf, xmm_zero_mask )
		PANDN( xmm_zero_mask, xmm_f )
		POR( xmm_zero_mask, xmm_minus_inf )
		xmm_f = xmm_zero_mask

		# if !(x < inf) then f = x
		xmm_x_copy = SSERegister()
		SCALAR_COPY( xmm_x_copy, xmm_x )
		CMPNLTSD( xmm_x, Constant.float64(Log.plus_inf) )
		BLENDVPD( xmm_f, xmm_x_copy, xmm_x )

		# *yPointer = f
		MOVSD( [yPointer], xmm_f )
	else:
		# x = *xPointer
		xmm_x = SSERegister()
		MOVSD( xmm_x, [xPointer] )

		xmm_zero_mask = SSERegister()
		xmm_minus_inf = SSERegister()
		PXOR( xmm_zero_mask, xmm_zero_mask )
		LOAD.CONSTANT( xmm_minus_inf, Constant.float64(Log.minus_inf) )
		# zeroMask = (x == 0.0)
		CMPEQSD( xmm_zero_mask, xmm_x )
		# f = (zeroMask) ? -inf : f
		PAND( xmm_minus_inf, xmm_zero_mask )
		PANDN( xmm_zero_mask, xmm_f )
		POR( xmm_zero_mask, xmm_minus_inf )
		xmm_f = xmm_zero_mask
		
		# if sign(x) == -1 then f = NaN
		xmm_negative_mask = SSERegister()
		if Target.get_int_eu_width() == 64:
			ASSUME.INITIALIZED( xmm_negative_mask )
			PSHUFLW( xmm_negative_mask, xmm_x, 0xEE )
		else:
			PSHUFD( xmm_negative_mask, xmm_x, 0xF5 )
		PSRAD( xmm_negative_mask, 31 )
		xmm_nan = SSERegister()
		MOVSD( xmm_nan, Constant.float64(Log.nan) )
		PAND( xmm_nan, xmm_negative_mask )
		PANDN( xmm_negative_mask, xmm_f )
		POR( xmm_negative_mask, xmm_nan )
		xmm_f = xmm_negative_mask

		# if !(x < inf) then f = x
		xmm_x_copy = SSERegister()
		SCALAR_COPY( xmm_x_copy, xmm_x )
		CMPNLTSD( xmm_x, Constant.float64(Log.plus_inf) )
		PAND( xmm_x_copy, xmm_x )
		PANDN( xmm_x, xmm_f )
		POR( xmm_x, xmm_x_copy )
		xmm_f = xmm_x

		# *yPointer = f
		MOVSD( [yPointer], xmm_f )

def SCALAR_LOG_AVX(xPointer, yPointer, is_prologue):
	# x = *xPointer
	xmm_x = SSERegister()
	VMOVSD( xmm_x, [xPointer] )
	# ne = as_ulong(x) >> 52
	xmm_ne = SSERegister()
	VPSRLQ( xmm_ne, xmm_x, 52 )
	# dx = as_double(as_ulong(x) | denormal_magic) - denormal_bias
	xmm_dx = SSERegister()
	VORPS( xmm_dx, xmm_x, Constant.float64x2(Log.denormal_magic) )
	VSUBSD( xmm_dx, xmm_dx, Constant.float64(Log.denormal_magic) )
	# dmask = (ne == 0)
	xmm_dmask = SSERegister()
	VPXOR( xmm_dmask, xmm_dmask, xmm_dmask )
	VPCMPEQQ( xmm_dmask, xmm_dmask, xmm_ne )
	# de = (as_ulong(dx) >> 52) - denormal_exponent_shift
	xmm_de = SSERegister()
	VPSRLQ( xmm_de, xmm_dx, 52 )
	VPSUBQ( xmm_de, xmm_de, Constant.uint64x2(Log.denormal_exponent_shift) )
	# x = dmask ? dx : x
	VPBLENDVB( xmm_x, xmm_x, xmm_dx, xmm_dmask )
	# x = (x & mantissa_mask) | default_exponent
	VPAND( xmm_x, xmm_x, Constant.uint64x2(Log.mantissa_mask) )
	VPOR( xmm_x, xmm_x, Constant.float64x2(Log.one) )
	# e = (de & dmask) | ne
	xmm_e = SSERegister()
	VPAND( xmm_e, xmm_de, xmm_dmask )
	VPOR( xmm_e, xmm_e, xmm_ne )

	# amask = x >= sqrt2
	xmm_amask = SSERegister()
	LOAD.CONSTANT( xmm_amask, Constant.float64(Log.sqrt2) )
	VCMPLTSD( xmm_amask, xmm_amask, xmm_x )
	# If (amask) then e += 1
	VPSUBQ( xmm_e, xmm_e, xmm_amask )
	# If (amask) then x *= 0.5
	VPAND( xmm_amask, xmm_amask, Constant.float64x2(Log.min_normal) )
	VPXOR( xmm_x, xmm_x, xmm_amask )

	# t = x - 1.0
	xmm_t = SSERegister()
	ASSUME.INITIALIZED( xmm_t )
	VSUBSD( xmm_t, xmm_x, Constant.float64(Log.one) )

	# rf = c20
	xmm_rf = SSERegister()
	LOAD.CONSTANT( xmm_rf, Constant.float64(Log.c20) )

	def HORNER_STEP( f, t, c ):
		if Target.has_fma4():
			VFMADDSD( f, f, t, c )
		elif Target.has_fma3():
			VFMADD213SD( f, f, t, c )
		else:
			VMULSD( f, f, t )
			VADDSD( f, f, c )

	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c19) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c18) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c17) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c16) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c15) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c14) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c13) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c12) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c11) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c10) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c9) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c8) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c7) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c6) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c5) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c4) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c3) )
	HORNER_STEP( xmm_rf, xmm_t, Constant.float64(Log.c2) )

	# rf = (pt * t) * t + t
	VMULSD( xmm_rf, xmm_t )
	if Target.has_fma4():
		VFMADDSD( xmm_rf, xmm_rf, xmm_t, xmm_t )
	elif Target.has_fma3():
		VFMADD213SD( xmm_rf, xmm_rf, xmm_t, xmm_t )
	else:
		VMULSD( xmm_rf, xmm_t )
		VADDSD( xmm_rf, xmm_t )

	# fe = as_double(as_ulong(e) + exponent_magic) - exponent_bias
	VPADDQ( xmm_e, Constant.uint64x2(Log.exponent_magic) )
	VSUBSD( xmm_e, Constant.float64(Log.exponent_bias) )

	# x = *xPointer
	xmm_x = SSERegister()
	VMOVSD( xmm_x, [xPointer] )

	# f = e * ln2_lo + rf
	xmm_f = SSERegister()
	ASSUME.INITIALIZED( xmm_f )
	if Target.has_fma4():
		VFMADDSD( xmm_f, xmm_e, Constant.float64(Log.FMA.ln2_lo), xmm_rf )
	elif Target.has_fma3():
		VMOVAPD( xmm_f, xmm_e )
		VFMADD132SD( xmm_f, xmm_f, Constant.float64(Log.FMA.ln2_lo), xmm_rf )
	else:
		VMULSD( xmm_f, xmm_e, Constant.float64(Log.preFMA.ln2_lo) )
		VADDSD( xmm_f, xmm_f, xmm_rf )
	# f = fe * ln2_hi + f
	if Target.has_fma4():
		VFMADDSD( xmm_f, xmm_e, Constant.float64(Log.FMA.ln2_hi), xmm_f )
	elif Target.has_fma3():
		VFMADD231SD( xmm_f, xmm_e, Constant.float64(Log.FMA.ln2_hi), xmm_f )
	else:
		VMULSD( xmm_e, xmm_e, Constant.float64(Log.preFMA.ln2_hi) )
		VADDSD( xmm_f, xmm_f, xmm_e )

	# if sign(x) == -1 then f = NaN
	VBLENDVPD( xmm_f, xmm_f, Constant.float64x2(Log.nan), xmm_x )

	xmm_zero_mask = SSERegister()
	VPXOR( xmm_zero_mask, xmm_zero_mask, xmm_zero_mask )
	VPCMPEQQ( xmm_zero_mask, xmm_zero_mask, xmm_x )
	VBLENDVPD( xmm_f, xmm_f, Constant.float64x2(Log.minus_inf), xmm_zero_mask )

	# if !(x < inf) then f = x
	xmm_inf_mask = SSERegister()
	ASSUME.INITIALIZED( xmm_inf_mask )
	VCMPNLTSD( xmm_inf_mask, xmm_x, Constant.float64x2(Log.plus_inf) )
	VBLENDVPD( xmm_f, xmm_f, xmm_x, xmm_inf_mask )

	# *yPointer = f
	VMOVSD( [yPointer], xmm_f )

def BATCH_LOG_FAST_Bobcat(xPointer, yPointer, vectorLogFullLabel):
	xmm_x = [SSERegister() for i in range(3)]
	xmm_e = [SSERegister() for i in range(3)]

	x_temp = GeneralPurposeRegister64()
	x_shift = GeneralPurposeRegister64()
	x_threshold = GeneralPurposeRegister64()

	MOV(x_shift, Log.x_min)
	MOV(x_threshold, Log.x_max - Log.x_min)

	check_instructions = InstructionStream()
	with check_instructions: 
		for i in range(3*2):
			MOV( x_temp, [xPointer + i * 8] )
			SUB( x_temp, x_shift )
			CMP( x_temp, x_threshold )
			JA( vectorLogFullLabel )

	for i in range(3):
		# x = *xPointer
		MOVAPS( xmm_x[i], [xPointer + i * 16] )
		# e = as_ulong(x) >> 52
		MOVAPS( xmm_e[i], xmm_x[i] )
		PSRLQ( xmm_e[i], 52 )
	
	xmm_mantissa_mask = SSERegister()
	xmm_one = SSERegister()
	MOVAPS( xmm_mantissa_mask, Constant.uint64x2(Log.mantissa_mask) )
	MOVAPS( xmm_one, Constant.float64x2(Log.one) )

	xmm_amask = [SSERegister() for i in range(3)]
	MOVAPS( xmm_amask[0], Constant.float64x2(Log.sqrt2) )
	for i in range(3):
		# m = (m & mantissa_mask) | default_exponent
		PAND( xmm_x[i], xmm_mantissa_mask )
		check_instructions.issue()
		POR( xmm_x[i], xmm_one )

	xmm_min_normal = SSERegister()
	MOVAPS( xmm_min_normal, Constant.float64x2(Log.min_normal) )
	MOVAPS( xmm_amask[1], xmm_amask[0] )
	MOVAPS( xmm_amask[2], xmm_amask[0] )
	for i in range(3):
		# amask = m >= sqrt2
		CMPLTPD( xmm_amask[i], xmm_x[i] )
		# If (amask) then e += 1
		PSUBQ( xmm_e[i], xmm_amask[i] )
		check_instructions.issue()
		# If (amask) then m *= 0.5
		PAND( xmm_amask[i], xmm_min_normal )
		PSUBD( xmm_x[i], xmm_amask[i] )
		check_instructions.issue()

	xmm_rf = [SSERegister() for i in range(3)]
	MOVAPS( xmm_rf[0], Constant.float64x2(Log.c20) )
	xmm_t = [SSERegister() for i in range(3)]
	for i in range(3):
		# t = m - 1.0
		SUBPD( xmm_x[i], xmm_one )
		check_instructions.issue()
		xmm_t[i] = xmm_x[i]

	MOVAPS( xmm_rf[1], xmm_rf[0] )
	MOVAPS( xmm_rf[2], xmm_rf[0] )
	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(3):
			MULPD( xmm_rf[i], xmm_t[i] )
			check_instructions.issue()
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Log.c19))
	HORNER_STEP(Constant.float64x2(Log.c18))
	HORNER_STEP(Constant.float64x2(Log.c17))
	HORNER_STEP(Constant.float64x2(Log.c16))
	HORNER_STEP(Constant.float64x2(Log.c15))
	HORNER_STEP(Constant.float64x2(Log.c14))
	HORNER_STEP(Constant.float64x2(Log.c13))
	HORNER_STEP(Constant.float64x2(Log.c12))
	HORNER_STEP(Constant.float64x2(Log.c11))
	HORNER_STEP(Constant.float64x2(Log.c10))
	HORNER_STEP(Constant.float64x2(Log.c9))
	HORNER_STEP(Constant.float64x2(Log.c8))
	HORNER_STEP(Constant.float64x2(Log.c7))
	HORNER_STEP(Constant.float64x2(Log.c6))
	HORNER_STEP(Constant.float64x2(Log.c5))
	HORNER_STEP(Constant.float64x2(Log.c4))
	HORNER_STEP(Constant.float64x2(Log.c3))
	HORNER_STEP(Constant.float64x2(Log.c2))

	assert len(check_instructions) == 0

	# rf = (pt * t) * t + t
	for i in range(3):
		MULPD( xmm_rf[i], xmm_t[i] )

	xmm_exponent_magic = SSERegister()
	xmm_exponent_bias = SSERegister()
	LOAD.CONSTANT( xmm_exponent_magic, Constant.uint64x2(Log.exponent_magic) )
	LOAD.CONSTANT( xmm_exponent_bias, Constant.float64x2(Log.exponent_bias) )
	for i in range(3):
		MULPD( xmm_rf[i], xmm_t[i] )
		ADDPD( xmm_rf[i], xmm_t[i] )

	xmm_f = [SSERegister() for i in range(3)]

	xmm_ln2_lo = SSERegister()
	xmm_ln2_hi = SSERegister()
	LOAD.CONSTANT( xmm_ln2_lo, Constant.float64x2(Log.preFMA.ln2_lo) )
	for i in range(3):
		# e = as_double(as_ulong(e) + exponent_magic) - exponent_bias
		PADDQ( xmm_e[i], xmm_exponent_magic )
		SUBPD( xmm_e[i], xmm_exponent_bias )

	LOAD.CONSTANT( xmm_ln2_hi, Constant.float64x2(Log.preFMA.ln2_hi) )
	for i in range(3):
		# f = e * ln2_lo + rf
		MOVAPS( xmm_f[i], xmm_e[i] )
		MULPD( xmm_f[i], xmm_ln2_lo )
		ADDPD( xmm_f[i], xmm_rf[i] )

	for i in range(3):
		# f += e * ln2_hi
		MULPD( xmm_e[i], xmm_ln2_hi )
		ADDPD( xmm_f[i], xmm_e[i] )
		# *yPointer = f
		MOVUPS( [yPointer + i * 16], xmm_f[i] )

def BATCH_LOG_FULL_Bobcat(xPointer, yPointer):
	xmm_x = [SSERegister() for i in range(3)]
	xmm_e = [SSERegister() for i in range(3)]

	xmm_dx = [SSERegister() for i in range(3)]
	xmm_ne = [SSERegister() for i in range(3)]
	xmm_de = [SSERegister() for i in range(3)]
	xmm_dmask = [SSERegister() for i in range(3)]
	for i in range(3):
		# x = *xPointer
		MOVAPS( xmm_x[i], [xPointer + i * 16] )
		# ne = as_ulong(x) >> 52
		MOVAPS( xmm_ne[i], xmm_x[i] )

	xmm_denormal_magic = SSERegister()
	MOVAPS( xmm_denormal_magic, Constant.float64x2(Log.denormal_magic) )
	for i in range(3):
		PSRLQ( xmm_ne[i], 52 )
		# dx = as_double(as_ulong(x) | denormal_magic) - denormal_bias
		MOVAPS( xmm_dx[i], xmm_x[i] )

	xmm_denormal_exponent_shift = SSERegister()
	MOVAPS( xmm_denormal_exponent_shift, Constant.uint64x2(Log.denormal_exponent_shift) ) 
	for i in range(3):
		ORPS( xmm_dx[i], xmm_denormal_magic )
		SUBPD( xmm_dx[i], xmm_denormal_magic )

	for i in range(3):
		# dmask = (ne == 0)
		PXOR( xmm_dmask[i], xmm_dmask[i] )
		CMPEQPD( xmm_dmask[i], xmm_ne[i] )

	for i in range(3):
		# de = (as_ulong(dx) >> 52) - denormal_exponent_shift
		MOVDQA( xmm_de[i], xmm_dx[i] )
		PSRLQ( xmm_de[i], 52 )
		PSUBQ( xmm_de[i], xmm_denormal_exponent_shift )

	xmm_mantissa_mask = SSERegister()
	MOVAPS( xmm_mantissa_mask, Constant.uint64x2(Log.mantissa_mask) )
	for i in range(3):
		# e = (de & dmask) | ne
		PAND( xmm_de[i], xmm_dmask[i] )
		POR( xmm_de[i], xmm_ne[i] )
		xmm_e[i] = xmm_de[i]

	xmm_one = SSERegister()
	MOVAPS( xmm_one, Constant.float64x2(Log.one) )
	for i in range(3):
		# m = dmask ? dx : x
		PAND( xmm_dx[i], xmm_dmask[i] )
		PANDN( xmm_dmask[i], xmm_x[i] )
		POR( xmm_dmask[i], xmm_dx[i] )
		xmm_x[i] = xmm_dmask[i]

	xmm_amask = [SSERegister() for i in range(3)]
	MOVAPS( xmm_amask[0], Constant.float64x2(Log.sqrt2) )
	for i in range(3):
		# m = (m & mantissa_mask) | default_exponent
		PAND( xmm_x[i], xmm_mantissa_mask )
		POR( xmm_x[i], xmm_one )

	xmm_min_normal = SSERegister()
	MOVAPS( xmm_min_normal, Constant.float64x2(Log.min_normal) )
	MOVAPS( xmm_amask[1], xmm_amask[0] )
	MOVAPS( xmm_amask[2], xmm_amask[0] )
	for i in range(3):
		# amask = m >= sqrt2
		CMPLTPD( xmm_amask[i], xmm_x[i] )
		# If (amask) then e += 1
		PSUBQ( xmm_e[i], xmm_amask[i] )
		# If (amask) then m *= 0.5
		PAND( xmm_amask[i], xmm_min_normal )
		PSUBD( xmm_x[i], xmm_amask[i] )

	xmm_rf = [SSERegister() for i in range(3)]
	MOVAPS( xmm_rf[0], Constant.float64x2(Log.c20) )
	xmm_t = [SSERegister() for i in range(3)]
	for i in range(3):
		# t = m - 1.0
		SUBPD( xmm_x[i], xmm_one )
		xmm_t[i] = xmm_x[i]

	MOVAPS( xmm_rf[1], xmm_rf[0] )
	MOVAPS( xmm_rf[2], xmm_rf[0] )
	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(3):
			MULPD( xmm_rf[i], xmm_t[i] )
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Log.c19))
	HORNER_STEP(Constant.float64x2(Log.c18))
	HORNER_STEP(Constant.float64x2(Log.c17))
	HORNER_STEP(Constant.float64x2(Log.c16))
	HORNER_STEP(Constant.float64x2(Log.c15))
	HORNER_STEP(Constant.float64x2(Log.c14))
	HORNER_STEP(Constant.float64x2(Log.c13))
	HORNER_STEP(Constant.float64x2(Log.c12))
	HORNER_STEP(Constant.float64x2(Log.c11))
	HORNER_STEP(Constant.float64x2(Log.c10))
	HORNER_STEP(Constant.float64x2(Log.c9))
	HORNER_STEP(Constant.float64x2(Log.c8))
	HORNER_STEP(Constant.float64x2(Log.c7))
	HORNER_STEP(Constant.float64x2(Log.c6))
	HORNER_STEP(Constant.float64x2(Log.c5))
	HORNER_STEP(Constant.float64x2(Log.c4))
	HORNER_STEP(Constant.float64x2(Log.c3))
	HORNER_STEP(Constant.float64x2(Log.c2))

	# rf = (pt * t) * t + t
	for i in range(3):
		MULPD( xmm_rf[i], xmm_t[i] )

	xmm_exponent_magic = SSERegister()
	xmm_exponent_bias = SSERegister()
	LOAD.CONSTANT( xmm_exponent_magic, Constant.uint64x2(Log.exponent_magic) )
	LOAD.CONSTANT( xmm_exponent_bias, Constant.float64x2(Log.exponent_bias) )
	for i in range(3):
		MULPD( xmm_rf[i], xmm_t[i] )
		ADDPD( xmm_rf[i], xmm_t[i] )

	xmm_f = [SSERegister() for i in range(3)]

	xmm_ln2_lo = SSERegister()
	LOAD.CONSTANT( xmm_ln2_lo, Constant.float64x2(Log.preFMA.ln2_lo) )
	for i in range(3):
		# e = as_double(as_ulong(e) + exponent_magic) - exponent_bias
		PADDQ( xmm_e[i], xmm_exponent_magic )
		SUBPD( xmm_e[i], xmm_exponent_bias )

	xmm_ln2_hi = SSERegister()
	LOAD.CONSTANT( xmm_ln2_hi, Constant.float64x2(Log.preFMA.ln2_hi) )
	for i in range(3):
		# f = e * ln2_lo + rf
		MOVAPS( xmm_f[i], xmm_e[i] )
		MULPD( xmm_f[i], xmm_ln2_lo )
		ADDPD( xmm_f[i], xmm_rf[i] )

	for i in range(3):
		# f += e * ln2_hi
		MULPD( xmm_e[i], xmm_ln2_hi )
		ADDPD( xmm_f[i], xmm_e[i] )

	xmm_x = [SSERegister() for i in range(3)]
	for i in range(3):
		# x = *xPointer
		MOVAPS( xmm_x[i], [xPointer + i * 16] )

		xmm_zero_mask = SSERegister()
		xmm_minus_inf = SSERegister()
		PXOR( xmm_zero_mask, xmm_zero_mask )
		MOVAPS( xmm_minus_inf, Constant.float64x2(Log.minus_inf) )
		# zeroMask = (x == 0.0)
		CMPEQPD( xmm_zero_mask, xmm_x[i] )
		# f = (zeroMask) ? -inf : f
		PAND( xmm_minus_inf, xmm_zero_mask )
		PANDN( xmm_zero_mask, xmm_f[i] )
		POR( xmm_zero_mask, xmm_minus_inf )
		xmm_f[i] = xmm_zero_mask.get_oword()

	xmm_plus_inf = SSERegister()
	MOVAPS( xmm_plus_inf, Constant.float64x2(Log.plus_inf) )
	for i in range(3):
		# if sign(x) == -1 then f = NaN
		xmm_negative_mask = SSERegister()
		PSHUFD( xmm_negative_mask, xmm_x[i], 0xF5 )
		PSRAD( xmm_negative_mask, 31 )
		xmm_nan = SSERegister()
		MOVAPS( xmm_nan, Constant.float64x2(Log.nan) )
		PAND( xmm_nan, xmm_negative_mask )
		PANDN( xmm_negative_mask, xmm_f[i] )
		POR( xmm_negative_mask, xmm_nan )
		xmm_f[i] = xmm_negative_mask

	for i in range(3):
		# if !(x < inf) then f = x
		xmm_x_copy = SSERegister()
		MOVAPS( xmm_x_copy, xmm_x[i] )
		CMPNLTPD( xmm_x[i], xmm_plus_inf )
		PAND( xmm_x_copy, xmm_x[i] )
		PANDN( xmm_x[i], xmm_f[i] )
		POR( xmm_x[i], xmm_x_copy )
		xmm_f[i] = xmm_x[i]

		# *yPointer = f
		MOVUPS( [yPointer + i * 16], xmm_f[i] )

def BATCH_LOG_FAST_K10(xPointer, yPointer, vectorLogFullLabel):
	xmm_x = [SSERegister() for i in range(8)]
	e = [LocalVariable(SSERegister) for i in range(8)]
	xmm_e = [SSERegister() for i in range(8)]

	x_temp = GeneralPurposeRegister64()
	x_shift = GeneralPurposeRegister64()
	x_threshold = GeneralPurposeRegister64()

	MOV(x_shift, Log.x_min)
	MOV(x_threshold, Log.x_max - Log.x_min)

	check_instructions = InstructionStream()
	with check_instructions:
		for i in range(3*2):
			MOV( x_temp, [xPointer + i * 8] )
			SUB( x_temp, x_shift )
			CMP( x_temp, x_threshold )
			JA( vectorLogFullLabel )

	xmm_one = xmm_mantissa_mask = None
	for iBlock in range(0, 8, 4):
		for i in range(iBlock, iBlock + 4):
			# x = *xPointer
			MOVAPS( xmm_x[i], [xPointer + i * 16] )
			# ne = as_ulong(x) >> 52
			MOVAPS( xmm_e[i], xmm_x[i] )
			PSRLQ( xmm_e[i], 52 )

		for i in range(iBlock, iBlock + 4):
			# m = (m & mantissa_mask) | default_exponent
			xmm_mantissa_mask = INIT.ONCE( SSERegister, Constant.uint64x2(Log.mantissa_mask), xmm_mantissa_mask )
			PAND( xmm_x[i], xmm_mantissa_mask )
			xmm_one = INIT.ONCE( SSERegister, Constant.float64x2(Log.one), xmm_one )
			POR( xmm_x[i], xmm_one )

		for i in range(iBlock, iBlock + 4):
			# amask = m >= sqrt2
			xmm_amask = SSERegister()
			MOVAPS( xmm_amask, Constant.float64x2(Log.sqrt2) )
			CMPLTPD( xmm_amask, xmm_x[i] )
			check_instructions.issue()
			# If (amask) then e += 1
			PSUBQ( xmm_e[i], xmm_amask )
			# If (amask) then m *= 0.5
			PAND( xmm_amask, Constant.float64x2(Log.min_normal) )
			PSUBD( xmm_x[i], xmm_amask )
			MOVAPS( e[i], xmm_e[i] )

	xmm_rf = [SSERegister() for i in range(8)]
	t = [SSERegister() for i in range(6)] + [LocalVariable(SSERegister) for i in range(2)]
	for i in range(8):
		# t = m - 1.0
		SUBPD( xmm_x[i], xmm_one )
		check_instructions.issue()
		if isinstance(t[i], SSERegister):
			t[i] = xmm_x[i]
		else:
			MOVAPS( t[i], xmm_x[i] )
		if i % 4 == 0:
			MOVAPS( xmm_rf[i], Constant.float64x2(Log.c20) )
		else:
			MOVAPS( xmm_rf[i], xmm_rf[(i/4)*4] )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		MOVAPS( xmm_c, coef )
		for i in range(8):
			MULPD( xmm_rf[i], t[i] )
			ADDPD( xmm_rf[i], xmm_c )
			check_instructions.issue()

	HORNER_STEP(Constant.float64x2(Log.c19))
	HORNER_STEP(Constant.float64x2(Log.c18))
	HORNER_STEP(Constant.float64x2(Log.c17))
	HORNER_STEP(Constant.float64x2(Log.c16))
	HORNER_STEP(Constant.float64x2(Log.c15))
	HORNER_STEP(Constant.float64x2(Log.c14))
	HORNER_STEP(Constant.float64x2(Log.c13))
	HORNER_STEP(Constant.float64x2(Log.c12))
	HORNER_STEP(Constant.float64x2(Log.c11))
	HORNER_STEP(Constant.float64x2(Log.c10))
	HORNER_STEP(Constant.float64x2(Log.c9))
	HORNER_STEP(Constant.float64x2(Log.c8))
	HORNER_STEP(Constant.float64x2(Log.c7))
	HORNER_STEP(Constant.float64x2(Log.c6))
	HORNER_STEP(Constant.float64x2(Log.c5))
	HORNER_STEP(Constant.float64x2(Log.c4))
	HORNER_STEP(Constant.float64x2(Log.c3))
	HORNER_STEP(Constant.float64x2(Log.c2))

	assert len(check_instructions) == 0

	# rf = (pt * t) * t + t
	for i in range(8):
		if isinstance(t[i], SSERegister):
			xmm_t = t[i]
		else:
			xmm_t = SSERegister()
			MOVAPS( xmm_t, t[i] )

		MULPD( xmm_rf[i], xmm_t )
		MULPD( xmm_rf[i], xmm_t )
		ADDPD( xmm_rf[i], xmm_t )

	xmm_f = [SSERegister() for i in range(8)]
	xmm_e = [SSERegister() for i in range(8)]

	xmm_exponent_magic = SSERegister()
	xmm_exponent_bias = SSERegister()
	LOAD.CONSTANT( xmm_exponent_magic, Constant.uint64x2(Log.exponent_magic) )
	LOAD.CONSTANT( xmm_exponent_bias, Constant.float64x2(Log.exponent_bias) )
	xmm_ln2_lo = None
	xmm_ln2_hi = None
	for iBlock in range(0, 8, 4):
		for i in range(iBlock, iBlock + 4):
			MOVAPS( xmm_e[i], e[i] )
			# e = as_double(as_ulong(e) + exponent_magic) - exponent_bias
			PADDQ( xmm_e[i], xmm_exponent_magic )
			SUBPD( xmm_e[i], xmm_exponent_bias )

		xmm_ln2_lo = INIT.ONCE( SSERegister, Constant.float64x2(Log.preFMA.ln2_lo), xmm_ln2_lo )
		for i in range(iBlock, iBlock + 4):
			# f = e * ln2_lo + rf
			MOVAPS( xmm_f[i], xmm_e[i] )
			MULPD( xmm_f[i], xmm_ln2_lo )
			ADDPD( xmm_f[i], xmm_rf[i] )

		xmm_ln2_hi = INIT.ONCE( SSERegister, Constant.float64x2(Log.preFMA.ln2_hi), xmm_ln2_hi )		
		for i in range(iBlock, iBlock + 4):
			# f += e * ln2_hi
			MULPD( xmm_e[i], xmm_ln2_hi )
			ADDPD( xmm_f[i], xmm_e[i] )
			# *yPointer = f
			MOVUPS( [yPointer + i * 16], xmm_f[i] )

def BATCH_LOG_FULL_K10(xPointer, yPointer):
	xmm_x = [SSERegister() for i in range(8)]
	e = [LocalVariable(SSERegister) for i in range(8)]
	xmm_e = [SSERegister() for i in range(8)]

	xmm_denormal_magic = SSERegister()
	MOVAPS( xmm_denormal_magic, Constant.float64x2(Log.denormal_magic) )
	for iBlock in range(0, 8, 4):
		for i in range(iBlock, iBlock + 4):
			# x = *xPointer
			MOVAPS( xmm_x[i], [xPointer + i * 16] )
			# ne = as_ulong(x) >> 52
			MOVAPS( xmm_e[i], xmm_x[i] )
			PSRLQ( xmm_e[i], 52 )

		for i in range(iBlock, iBlock + 4):
			# dx = as_double(as_ulong(x) | denormal_magic) - denormal_bias
			xmm_dx = SSERegister()
			MOVAPS( xmm_dx, xmm_x[i] )
			ORPS( xmm_dx, xmm_denormal_magic )
			SUBPD( xmm_dx, xmm_denormal_magic )

			xmm_dmask = SSERegister()
			# dmask = (ne == 0)
			PXOR( xmm_dmask, xmm_dmask )
			CMPEQPD( xmm_dmask, xmm_e[i] )
			# de = (as_ulong(dx) >> 52) - denormal_exponent_shift
			xmm_de = SSERegister()
			MOVAPS( xmm_de, xmm_dx )
			PSRLQ( xmm_de, 52 )
			PSUBQ( xmm_de, Constant.uint64x2(Log.denormal_exponent_shift) )
			# e = (de & dmask) | ne
			PAND( xmm_de, xmm_dmask )
			POR( xmm_e[i], xmm_de )
			# m = dmask ? dx : x
			ANDPS( xmm_dx, xmm_dmask )
			ANDNPS( xmm_dmask, xmm_x[i] )
			ORPS( xmm_dmask, xmm_dx )
			xmm_x[i] = xmm_dmask
		
		xmm_one = SSERegister()
		MOVAPS( xmm_one, Constant.float64x2(Log.one) )
		for i in range(iBlock, iBlock + 4):
			# m = (m & mantissa_mask) | default_exponent
			PAND( xmm_x[i], Constant.uint64x2(Log.mantissa_mask) )
			POR( xmm_x[i], xmm_one )

		for i in range(iBlock, iBlock + 4):
			# amask = m >= sqrt2
			xmm_amask = SSERegister()
			MOVAPS( xmm_amask, Constant.float64x2(Log.sqrt2) )
			CMPLTPD( xmm_amask, xmm_x[i] )
			# If (amask) then e += 1
			PSUBQ( xmm_e[i], xmm_amask )
			# If (amask) then m *= 0.5
			PAND( xmm_amask, Constant.float64x2(Log.min_normal) )
			PSUBD( xmm_x[i], xmm_amask )
			MOVAPS( e[i], xmm_e[i] )

	xmm_rf = [SSERegister() for i in range(8)]
	t = [SSERegister() for i in range(6)] + [LocalVariable(SSERegister) for i in range(2)]
	for i in range(8):
		# t = m - 1.0
		SUBPD( xmm_x[i], xmm_one )
		if isinstance(t[i], SSERegister):
			t[i] = xmm_x[i]
		else:
			MOVAPS( t[i], xmm_x[i] )
		if i % 4 == 0:
			MOVAPS( xmm_rf[i], Constant.float64x2(Log.c20) )
		else:
			MOVAPS( xmm_rf[i], xmm_rf[(i/4)*4] )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		MOVAPS( xmm_c, coef )
		for i in range(8):
			MULPD( xmm_rf[i], t[i] )
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Log.c19))
	HORNER_STEP(Constant.float64x2(Log.c18))
	HORNER_STEP(Constant.float64x2(Log.c17))
	HORNER_STEP(Constant.float64x2(Log.c16))
	HORNER_STEP(Constant.float64x2(Log.c15))
	HORNER_STEP(Constant.float64x2(Log.c14))
	HORNER_STEP(Constant.float64x2(Log.c13))
	HORNER_STEP(Constant.float64x2(Log.c12))
	HORNER_STEP(Constant.float64x2(Log.c11))
	HORNER_STEP(Constant.float64x2(Log.c10))
	HORNER_STEP(Constant.float64x2(Log.c9))
	HORNER_STEP(Constant.float64x2(Log.c8))
	HORNER_STEP(Constant.float64x2(Log.c7))
	HORNER_STEP(Constant.float64x2(Log.c6))
	HORNER_STEP(Constant.float64x2(Log.c5))
	HORNER_STEP(Constant.float64x2(Log.c4))
	HORNER_STEP(Constant.float64x2(Log.c3))
	HORNER_STEP(Constant.float64x2(Log.c2))

	# rf = (pt * t) * t + t
	for i in range(8):
		if isinstance(t[i], SSERegister):
			xmm_t = t[i]
		else:
			xmm_t = SSERegister()
			MOVAPS( xmm_t, t[i] )

		MULPD( xmm_rf[i], xmm_t )
		MULPD( xmm_rf[i], xmm_t )
		ADDPD( xmm_rf[i], xmm_t )

	xmm_f = [SSERegister() for i in range(8)]
	xmm_e = [SSERegister() for i in range(8)]

	xmm_exponent_magic = SSERegister()
	xmm_exponent_bias = SSERegister()
	LOAD.CONSTANT( xmm_exponent_magic, Constant.uint64x2(Log.exponent_magic) )
	LOAD.CONSTANT( xmm_exponent_bias, Constant.float64x2(Log.exponent_bias) )
	for iBlock in range(0, 8, 4):
		for i in range(iBlock, iBlock + 4):
			MOVAPS( xmm_e[i], e[i] )
			# e = as_double(as_ulong(e) + exponent_magic) - exponent_bias
			PADDQ( xmm_e[i], xmm_exponent_magic )
			SUBPD( xmm_e[i], xmm_exponent_bias )

		xmm_ln2_lo = SSERegister()
		LOAD.CONSTANT( xmm_ln2_lo, Constant.float64x2(Log.preFMA.ln2_lo) )
		for i in range(iBlock, iBlock + 4):
			# f = e * ln2_lo + rf
			MOVAPS( xmm_f[i], xmm_e[i] )
			MULPD( xmm_f[i], xmm_ln2_lo )
			ADDPD( xmm_f[i], xmm_rf[i] )

		xmm_ln2_hi = SSERegister()
		LOAD.CONSTANT( xmm_ln2_hi, Constant.float64x2(Log.preFMA.ln2_hi) )
		for i in range(iBlock, iBlock + 4):
			# f += e * ln2_hi
			MULPD( xmm_e[i], xmm_ln2_hi )
			ADDPD( xmm_f[i], xmm_e[i] )

	xmm_x = [SSERegister() for i in range(8)]
	xmm_plus_inf = SSERegister()
	MOVAPS( xmm_plus_inf, Constant.float64x2(Log.plus_inf) )
	for iBlock in range(0, 8, 4):
		for i in range(iBlock, iBlock + 4):
			# x = *xPointer
			MOVAPS( xmm_x[i], [xPointer + i * 16] )

			xmm_zero_mask = SSERegister()
			xmm_minus_inf = SSERegister()
			PXOR( xmm_zero_mask, xmm_zero_mask )
			MOVAPS( xmm_minus_inf, Constant.float64x2(Log.minus_inf) )
			# zeroMask = (x == 0.0)
			CMPEQPD( xmm_zero_mask, xmm_x[i] )
			# f = (zeroMask) ? -inf : f
			ANDPS( xmm_minus_inf, xmm_zero_mask )
			ANDNPS( xmm_zero_mask, xmm_f[i] )
			ORPS( xmm_zero_mask, xmm_minus_inf )
			xmm_f[i] = xmm_zero_mask

		for i in range(iBlock, iBlock + 4):
			# if sign(x) == -1 then f = NaN
			xmm_negative_mask = SSERegister()
			PSHUFD( xmm_negative_mask, xmm_x[i], 0xF5 )
			PSRAD( xmm_negative_mask, 31 )
			xmm_nan = SSERegister()
			MOVAPS( xmm_nan, Constant.float64x2(Log.nan) )
			ANDPS( xmm_nan, xmm_negative_mask )
			ANDNPS( xmm_negative_mask, xmm_f[i] )
			ORPS( xmm_negative_mask, xmm_nan )
			xmm_f[i] = xmm_negative_mask

		for i in range(iBlock, iBlock + 4):
			# if !(x < inf) then f = x
			xmm_x_copy = SSERegister()
			MOVAPS( xmm_x_copy, xmm_x[i] )
			CMPNLTPD( xmm_x[i], xmm_plus_inf )
			ANDPS( xmm_x_copy, xmm_x[i] )
			ANDNPS( xmm_x[i], xmm_f[i] )
			ORPS( xmm_x[i], xmm_x_copy )
			xmm_f[i] = xmm_x[i]

			# *yPointer = f
			MOVUPS( [yPointer + i * 16], xmm_f[i] )

def BATCH_LOG_FAST_Nehalem(xPointer, yPointer, vectorLogFullLabel):
	xmm_x = [SSERegister() for i in range(8)]
	e = [LocalVariable(SSERegister) for i in range(8)]
	xmm_e = [SSERegister() for i in range(8)]

	x_temp = GeneralPurposeRegister64()

	xmm_min_normal = None

	xmm_one = SSERegister()
	LOAD.CONSTANT( xmm_one, Constant.float64x2(Log.one) )

	x_shift = GeneralPurposeRegister64()
	MOV(x_shift, Log.x_min)
	xmm_mantissa_mask = SSERegister()
	LOAD.CONSTANT( xmm_mantissa_mask, Constant.uint64x2(Log.mantissa_mask) )

	x_threshold = GeneralPurposeRegister64()
	MOV(x_threshold, Log.x_max - Log.x_min)
	xmm_exponent_shift = SSERegister()
	LOAD.CONSTANT( xmm_exponent_shift, Constant.uint32x4(Log.exponent_shift) )

	for iBlock in range(0, 8, 4):
		for i in range(iBlock, iBlock + 4):
			# x = *xPointer
			MOVAPS( xmm_x[i], [xPointer + i * 16] )
			# e = as_ulong(x) >> 52
			MOVAPS( xmm_e[i], xmm_x[i] )
			MOV( x_temp, [xPointer + i * 16] )
			PSRLD( xmm_e[i], 20 )
			# m = (m & mantissa_mask) | default_exponent
			SUB( x_temp, x_shift )
			PAND( xmm_x[i], xmm_mantissa_mask )
			CMP( x_temp, x_threshold )
			JA( vectorLogFullLabel )
			PSUBD( xmm_e[i], xmm_exponent_shift )
			POR( xmm_x[i], xmm_one )

		if xmm_min_normal is None:
			xmm_min_normal = SSERegister()
			LOAD.CONSTANT( xmm_min_normal, Constant.float64x2(Log.min_normal) )

		for i in range(iBlock, iBlock + 4):
			# amask = m >= sqrt2
			xmm_amask = SSERegister()
			LOAD.CONSTANT( xmm_amask, Constant.float64x2(Log.sqrt2) )
			CMPLTPD( xmm_amask, xmm_x[i] )
			MOV( x_temp, [xPointer + i * 16 + 8] )
			# If (amask) then e += 1
			PSUBD( xmm_e[i], xmm_amask )
			# If (amask) then m *= 0.5
			PAND( xmm_amask, xmm_min_normal )
			SUB( x_temp, x_shift )
			PSUBD( xmm_x[i], xmm_amask )
			PSHUFD( xmm_e[i], xmm_e[i], 0xDD)
			CMP( x_temp, x_threshold )
			JA( vectorLogFullLabel )
			MOVAPD( e[i], xmm_e[i] )

	xmm_rf = [SSERegister() for i in range(8)]
	t = [SSERegister() for i in range(6)] + [LocalVariable(SSERegister) for i in range(2)]
	for i in range(8):
		# t = m - 1.0
		SUBPD( xmm_x[i], xmm_one )
		if isinstance(t[i], SSERegister):
			t[i] = xmm_x[i]
		else:
			MOVAPD( t[i], xmm_x[i] )
		if i % 4 == 0:
			LOAD.CONSTANT( xmm_rf[i], Constant.float64x2(Log.c20) )
		else:
			MOVAPD( xmm_rf[i], xmm_rf[(i/4)*4] )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(8):
			MULPD( xmm_rf[i], t[i] )
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Log.c19))
	HORNER_STEP(Constant.float64x2(Log.c18))
	HORNER_STEP(Constant.float64x2(Log.c17))
	HORNER_STEP(Constant.float64x2(Log.c16))
	HORNER_STEP(Constant.float64x2(Log.c15))
	HORNER_STEP(Constant.float64x2(Log.c14))
	HORNER_STEP(Constant.float64x2(Log.c13))
	HORNER_STEP(Constant.float64x2(Log.c12))
	HORNER_STEP(Constant.float64x2(Log.c11))
	HORNER_STEP(Constant.float64x2(Log.c10))
	HORNER_STEP(Constant.float64x2(Log.c9))
	HORNER_STEP(Constant.float64x2(Log.c8))
	HORNER_STEP(Constant.float64x2(Log.c7))
	HORNER_STEP(Constant.float64x2(Log.c6))
	HORNER_STEP(Constant.float64x2(Log.c5))
	HORNER_STEP(Constant.float64x2(Log.c4))
	HORNER_STEP(Constant.float64x2(Log.c3))
	HORNER_STEP(Constant.float64x2(Log.c2))

	# rf = (pt * t) * t + t
	xmm_t = [SSERegister() for i in range(8)]
	for i in range(8):
		if i % 2 != 0:
			# e = double(e)
			xmm_e = SSERegister()
			CVTDQ2PD( xmm_e, e[i] )
			MOVAPS( e[i], xmm_e )

		if isinstance(t[i], SSERegister):
			xmm_t[i] = t[i]
		else:
			MOVAPD( xmm_t[i], t[i] )

		MULPD( xmm_rf[i], xmm_t[i] )

	xmm_f = [SSERegister() for i in range(8)]
	xmm_e = [SSERegister() for i in range(8)]
	for i in range(8):
		MULPD( xmm_rf[i], xmm_t[i] )
		ADDPD( xmm_rf[i], xmm_t[i] )

		if i % 2 == 0:
			# f = double(e)
			CVTDQ2PD( xmm_f[i], e[i] )
			MOVAPS( e[i], xmm_f[i] )

	xmm_ln2_lo = SSERegister()
	LOAD.CONSTANT( xmm_ln2_lo, Constant.float64x2(Log.preFMA.ln2_lo) )
	for i in range(8):
		# f = e * ln2_lo + rf
		if i % 2 != 0:
			MOVAPS( xmm_f[i], e[i] )
		else:
			MOVAPS( xmm_e[i], xmm_f[i] )
		MULPD( xmm_f[i], xmm_ln2_lo )
		ADDPD( xmm_f[i], xmm_rf[i] )

	xmm_ln2_hi = SSERegister()
	LOAD.CONSTANT( xmm_ln2_hi, Constant.float64x2(Log.preFMA.ln2_hi) )
	for i in range(8):
		# f += e * ln2_hi
		if i % 2 != 0:
			MOVAPS( xmm_e[i], e[i] )
		MULPD( xmm_e[i], xmm_ln2_hi )
		ADDPD( xmm_f[i], xmm_e[i] )
		# *yPointer = f
		MOVUPD( [yPointer + i * 16], xmm_f[i] )

def BATCH_LOG_FULL_Nehalem(xPointer, yPointer):
	xmm_x = [SSERegister() for i in range(8)]
	e = [LocalVariable(SSERegister) for i in range(8)]
	xmm_e = [SSERegister() for i in range(8)]

	xmm_denormal_magic = SSERegister()
	MOVAPS( xmm_denormal_magic, Constant.float64x2(Log.denormal_magic) )
	for iBlock in range(0, 8, 4):
		for i in range(iBlock, iBlock + 4):
			xmm_dx = SSERegister()
			xmm_ne = SSERegister()
			xmm_de = SSERegister()
			xmm_dmask = xmm0

			# x = *xPointer
			MOVAPS( xmm_x[i], [xPointer + i * 16] )
			# ne = as_ulong(x) >> 52
			MOVAPS( xmm_ne, xmm_x[i] )
			PSRLQ( xmm_ne, 52 )
			# dx = as_double(as_ulong(x) | denormal_magic) - denormal_bias
			MOVAPS( xmm_dx, xmm_x[i] )
			ORPS( xmm_dx, xmm_denormal_magic )
			SUBPD( xmm_dx, Constant.float64x2(Log.denormal_magic) )
			# dmask = (ne == 0)
			PXOR( xmm_dmask, xmm_dmask )
			PCMPEQQ( xmm_dmask, xmm_ne )
			# de = (as_ulong(dx) >> 52) - denormal_exponent_shift
			MOVDQA( xmm_de, xmm_dx )
			PSRLQ( xmm_de, 52 )
			PSUBQ( xmm_de, Constant.uint64x2(Log.denormal_exponent_shift) )
			# m = dmask ? dx : x
			PBLENDVB( xmm_x[i], xmm_dx, xmm_dmask )
			# m = (m & mantissa_mask) | default_exponent
			PAND( xmm_x[i], Constant.uint64x2(Log.mantissa_mask) )
			POR( xmm_x[i], Constant.float64x2(Log.one) )
			# e = (de & dmask) | ne
			PAND( xmm_de, xmm_dmask )
			POR( xmm_de, xmm_ne )
			xmm_e[i] = xmm_de

		for i in range(iBlock, iBlock + 4):
			# amask = m >= sqrt2
			xmm_amask = SSERegister()
			LOAD.CONSTANT( xmm_amask, Constant.float64x2(Log.sqrt2) )
			CMPLTPD( xmm_amask, xmm_x[i] )
			# If (amask) then e += 1
			PSUBQ( xmm_e[i], xmm_amask )
			# If (amask) then m *= 0.5
			PAND( xmm_amask, Constant.float64x2(Log.min_normal) )
			PSUBD( xmm_x[i], xmm_amask )
			MOVAPD( e[i], xmm_e[i] )

	xmm_one = SSERegister()
	LOAD.CONSTANT( xmm_one, Constant.float64x2(Log.one) )
	xmm_rf = [SSERegister() for i in range(8)]
	t = [SSERegister() for i in range(6)] + [LocalVariable(SSERegister) for i in range(2)]
	for i in range(8):
		# t = m - 1.0
		SUBPD( xmm_x[i], xmm_one )
		if isinstance(t[i], SSERegister):
			t[i] = xmm_x[i]
		else:
			MOVAPD( t[i], xmm_x[i] )
		if i % 4 == 0:
			LOAD.CONSTANT( xmm_rf[i], Constant.float64x2(Log.c20) )
		else:
			MOVAPD( xmm_rf[i], xmm_rf[(i/4)*4] )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(8):
			MULPD( xmm_rf[i], t[i] )
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Log.c19))
	HORNER_STEP(Constant.float64x2(Log.c18))
	HORNER_STEP(Constant.float64x2(Log.c17))
	HORNER_STEP(Constant.float64x2(Log.c16))
	HORNER_STEP(Constant.float64x2(Log.c15))
	HORNER_STEP(Constant.float64x2(Log.c14))
	HORNER_STEP(Constant.float64x2(Log.c13))
	HORNER_STEP(Constant.float64x2(Log.c12))
	HORNER_STEP(Constant.float64x2(Log.c11))
	HORNER_STEP(Constant.float64x2(Log.c10))
	HORNER_STEP(Constant.float64x2(Log.c9))
	HORNER_STEP(Constant.float64x2(Log.c8))
	HORNER_STEP(Constant.float64x2(Log.c7))
	HORNER_STEP(Constant.float64x2(Log.c6))
	HORNER_STEP(Constant.float64x2(Log.c5))
	HORNER_STEP(Constant.float64x2(Log.c4))
	HORNER_STEP(Constant.float64x2(Log.c3))
	HORNER_STEP(Constant.float64x2(Log.c2))

	# rf = (pt * t) * t + t
	for i in range(8):
		if isinstance(t[i], SSERegister):
			xmm_t = t[i]
		else:
			xmm_t = SSERegister()
			MOVAPD( xmm_t, t[i] )

		MULPD( xmm_rf[i], xmm_t )
		MULPD( xmm_rf[i], xmm_t )
		ADDPD( xmm_rf[i], xmm_t )

	xmm_f = [SSERegister() for i in range(8)]

	xmm_exponent_magic = SSERegister()
	LOAD.CONSTANT( xmm_exponent_magic, Constant.uint64x2(Log.exponent_magic) )
	xmm_exponent_bias = SSERegister()
	LOAD.CONSTANT( xmm_exponent_bias, Constant.float64x2(Log.exponent_bias) )
	xmm_ln2_lo = SSERegister()
	LOAD.CONSTANT( xmm_ln2_lo, Constant.float64x2(Log.preFMA.ln2_lo) )
	xmm_ln2_hi = SSERegister()
	LOAD.CONSTANT( xmm_ln2_hi, Constant.float64x2(Log.preFMA.ln2_hi) )
	for i in range(8):
		xmm_e = SSERegister()

		MOVAPD( xmm_e, e[i] )
		# e = as_double(as_ulong(e) + exponent_magic) - exponent_bias
		PADDQ( xmm_e, xmm_exponent_magic )
		SUBPD( xmm_e, xmm_exponent_bias )

		# f = e * ln2_lo + rf
		MOVAPS( xmm_f[i], xmm_e )
		MULPD( xmm_f[i], xmm_ln2_lo )
		ADDPD( xmm_f[i], xmm_rf[i] )
		# f += e * ln2_hi
		MULPD( xmm_e, xmm_ln2_hi )
		ADDPD( xmm_f[i], xmm_e )

	xmm_nan = SSERegister()
	LOAD.CONSTANT( xmm_nan, Constant.float64x2(Log.nan) )
	xmm_plus_inf = SSERegister()
	LOAD.CONSTANT( xmm_plus_inf, Constant.float64x2(Log.plus_inf) )
	for i in range(8):
		xmm_x = xmm0

		# x = *xPointer
		MOVAPD( xmm_x, [xPointer + i * 16] )
		# if sign(x) == -1 then f = NaN
		BLENDVPD( xmm_f[i], xmm_nan, xmm_x )

		# if !(x < inf) then f = x
		xmm_x_copy = SSERegister()
		MOVAPS( xmm_x_copy, xmm_x )
		CMPNLTPD( xmm_x, xmm_plus_inf )
		BLENDVPD( xmm_f[i], xmm_x_copy, xmm_x )

		# x = *xPointer
		PXOR( xmm_x, xmm_x )
		PCMPEQQ( xmm_x, xmm_x_copy )
		# if (x == +0.0) f = -inf
		BLENDVPD( xmm_f[i], Constant.float64x2(Log.minus_inf), xmm_x )

		# *yPointer = f
		MOVUPD( [yPointer + i * 16], xmm_f[i] )

def BATCH_LOG_FAST_SandyBridge(xPointer, yPointer, vectorLogFullLabel):
	t = [AVXRegister() for i in range(6)] + [LocalVariable(AVXRegister) for i in range(2)]
	e = [LocalVariable(AVXRegister) for i in range(8)]

	ymm_x = [AVXRegister() for i in range(8)]
	ymm_e = [AVXRegister() for i in range(8)]
	ymm_m = [AVXRegister() for i in range(8)]

	ymm_min_normal = AVXRegister()
	VMOVAPD( ymm_min_normal, Constant.float64x4(Log.min_normal) )
	for iBlock in range(0, 8, 4):
		ymm_exponent_mask = AVXRegister()
		ymm_max_normal = AVXRegister()
		VMOVAPD( ymm_exponent_mask, Constant.uint64x4(Log.exponent_mask) )
		VMOVAPD( ymm_max_normal, Constant.float64x4(Log.max_normal) )
		for i in range(iBlock, iBlock + 4):
			# x = *xPointer
			VMOVAPD( ymm_x[i], [xPointer + i * 32] )

			ymm_below_min_mask = AVXRegister()
			ymm_above_max_mask = AVXRegister()
			ymm_beyond_normal_mask = AVXRegister()
			VCMPLTPD( ymm_below_min_mask, ymm_x[i], ymm_min_normal )
			# e = x & exponent_mask
			VANDPD( ymm_e[i], ymm_x[i], ymm_exponent_mask )
			VMOVSHDUP( ymm_e[i], ymm_e[i] )
			VCMPGTPD( ymm_above_max_mask, ymm_x[i], ymm_max_normal )
			VORPD( ymm_beyond_normal_mask, ymm_below_min_mask, ymm_above_max_mask )
			VTESTPD( ymm_beyond_normal_mask, ymm_beyond_normal_mask )
			JNZ( vectorLogFullLabel )

		ymm_scaled_exponent_magic = AVXRegister()
		VMOVAPD( ymm_scaled_exponent_magic, Constant.float64x4(Log.scaled_exponent_magic) )
		ymm_one = AVXRegister()
		VMOVAPD( ymm_one, Constant.float64x4(Log.one) )
		for i in range(iBlock, iBlock + 4):
			# m = (x & mantissa_mask) | one
			VANDPD( ymm_m[i], ymm_x[i], Constant.uint64x4(Log.mantissa_mask) )
			VORPD( ymm_m[i], ymm_m[i], ymm_one )
			# convert e to double
			VBLENDPS( ymm_e[i], ymm_e[i], ymm_scaled_exponent_magic, 0xAA )
			VSUBPD( ymm_e[i], Constant.float64x4(Log.normalized_exponent_bias) )

		for i in range(iBlock, iBlock + 4):
			ymm_amask = AVXRegister()
			if isinstance(t[i], AVXRegister):
				ymm_t = t[i]
			else:
				ymm_t = AVXRegister()

			# amask = m > sqrt(2)
			VCMPGTPD( ymm_amask, ymm_m[i], Constant.float64x4(Log.sqrt2) )
			ymm_temp = AVXRegister()
			VANDPD( ymm_temp, ymm_amask, ymm_one )
			VADDPD( ymm_e[i], ymm_e[i], ymm_temp )
			VMOVAPD( e[i], ymm_e[i] )
			# if m > sqrt(2) then m *= 0.5
			ymm_exponent_bitflip = AVXRegister()
			VANDPD( ymm_exponent_bitflip, ymm_amask, ymm_min_normal )
			VXORPD( ymm_m[i], ymm_m[i], ymm_exponent_bitflip )
			VSUBPD( ymm_t, ymm_m[i], ymm_one )
			if isinstance(t[i], LocalVariable):
				VMOVAPD( t[i], ymm_t )

	ymm_f = [AVXRegister() for i in range(8)]
	ymm_c20 = AVXRegister()
	ymm_c19 = AVXRegister()
	VMOVAPD( ymm_c20, Constant.float64x4(Log.c20) )
	VMOVAPD( ymm_c19, Constant.float64x4(Log.c19) )
	for i in range(8):
		VMULPD( ymm_f[i], ymm_c20, t[i] )
		VADDPD( ymm_f[i], ymm_f[i], ymm_c19 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		VMOVAPD( ymm_c, coef )
		for i in range(8):
			VMULPD( ymm_f[i], ymm_f[i], t[i] )
			VADDPD( ymm_f[i], ymm_f[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Log.c18))
	HORNER_STEP(Constant.float64x4(Log.c17))
	HORNER_STEP(Constant.float64x4(Log.c16))
	HORNER_STEP(Constant.float64x4(Log.c15))
	HORNER_STEP(Constant.float64x4(Log.c14))
	HORNER_STEP(Constant.float64x4(Log.c13))
	HORNER_STEP(Constant.float64x4(Log.c12))
	HORNER_STEP(Constant.float64x4(Log.c11))
	HORNER_STEP(Constant.float64x4(Log.c10))
	HORNER_STEP(Constant.float64x4(Log.c9))
	HORNER_STEP(Constant.float64x4(Log.c8))
	HORNER_STEP(Constant.float64x4(Log.c7))
	HORNER_STEP(Constant.float64x4(Log.c6))
	HORNER_STEP(Constant.float64x4(Log.c5))
	HORNER_STEP(Constant.float64x4(Log.c4))
	HORNER_STEP(Constant.float64x4(Log.c3))
	HORNER_STEP(Constant.float64x4(Log.c2))

	ymm_t = [AVXRegister() for i in range(8)]
	for i in range(8):
		if isinstance(t[i], AVXRegister):
			ymm_t[i] = t[i]
		else:
			VMOVAPD( ymm_t[i], t[i] )

		VMULPD( ymm_f[i], ymm_f[i], ymm_t[i] )
	for i in range(8):
		VMULPD( ymm_f[i], ymm_f[i], ymm_t[i] )
		VADDPD( ymm_f[i], ymm_f[i], ymm_t[i] )

	ymm_e = [AVXRegister() for i in range(8)]

	ymm_ln2_lo = AVXRegister()
	ymm_ln2_hi = AVXRegister()
	LOAD.CONSTANT( ymm_ln2_lo, Constant.float64x4(Log.preFMA.ln2_lo) )
	LOAD.CONSTANT( ymm_ln2_hi, Constant.float64x4(Log.preFMA.ln2_hi) )
	for iBlock in range(0, 8, 4):
		for i in range(iBlock, iBlock + 4):
			VMOVAPD( ymm_e[i], e[i] )

			ymm_temp = AVXRegister()
			VMULPD( ymm_temp, ymm_e[i], ymm_ln2_lo )
			VADDPD( ymm_f[i], ymm_f[i], ymm_temp )

		for i in range(iBlock, iBlock + 4):	
			ymm_temp = AVXRegister()
			VMULPD( ymm_temp, ymm_e[i], ymm_ln2_hi )
			VADDPD( ymm_f[i], ymm_f[i], ymm_temp )

			VMOVUPD( [yPointer + i * 32], ymm_f[i].get_oword() )
			VEXTRACTF128( [yPointer + i * 32 + 16], ymm_f[i], 1 )

def BATCH_LOG_FULL_SandyBridge(xPointer, yPointer):
	t = [AVXRegister() for i in range(6)] + [LocalVariable(AVXRegister) for i in range(2)]
	e = [LocalVariable(AVXRegister) for i in range(8)]

	ymm_normalized_exponent_bias = AVXRegister()
	LOAD.CONSTANT( ymm_normalized_exponent_bias, Constant.float64x4(Log.normalized_exponent_bias) )
	ymm_denormal_magic = AVXRegister()
	LOAD.CONSTANT( ymm_denormal_magic, Constant.float64x4(Log.denormal_magic) )
	ymm_one = AVXRegister()
	LOAD.CONSTANT( ymm_one, Constant.float64x4(Log.one) )
	ymm_exponent_mask = AVXRegister()
	LOAD.CONSTANT( ymm_exponent_mask, Constant.uint64x4(Log.exponent_mask) )
	for i in range(8):
		if isinstance(t[i], AVXRegister):
			ymm_t = t[i]
		else:
			ymm_t = AVXRegister()

		# x = *xPointer
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, [xPointer + i * 32] )
		# dx = as_double(as_ulong(x) | denormal_magic) - denormal_shift
		ymm_dx = AVXRegister()
		VORPD( ymm_dx, ymm_x, ymm_denormal_magic )
		VSUBPD( ymm_dx, ymm_dx, ymm_denormal_magic )
		# dmask = x < min_normal
		ymm_dmask = AVXRegister()
		VCMPLTPD( ymm_dmask, ymm_x, Constant.float64x4(Log.min_normal) )
		# x = (dmask ? dx : x )
		VBLENDVPD( ymm_x, ymm_x, ymm_dx, ymm_dmask )
		# m = (tx & mantissa_mask) | one
		ymm_m = AVXRegister()
		VANDPD( ymm_m, ymm_x, Constant.uint64x4(Log.mantissa_mask) )
		VORPD( ymm_m, ymm_m, ymm_one )
		# e = tx & exponent_mask
		ymm_e = AVXRegister()
		VANDPD( ymm_e, ymm_x, ymm_exponent_mask )
		# exponent_bias = (dmask ? denormalized_exponent_bias : normalized_exponent_bias)
		ymm_exponent_bias = AVXRegister()
		VBLENDVPD( ymm_exponent_bias, ymm_normalized_exponent_bias, Constant.float64x4(Log.denormalized_exponent_bias), ymm_dmask )
		# convert e to double
		VMOVSHDUP( ymm_e, ymm_e )
		VBLENDPS( ymm_e, ymm_e, Constant.float64x4(Log.scaled_exponent_magic), 0xAA )
		VSUBPD( ymm_e, ymm_exponent_bias )

		# amask = m > sqrt(2)
		ymm_amask = AVXRegister()
		VCMPGTPD( ymm_amask, ymm_m, Constant.float64x4(Log.sqrt2) )
		ymm_temp = AVXRegister()
		VANDPD( ymm_temp, ymm_amask, ymm_one )
		VADDPD( ymm_e, ymm_e, ymm_temp )
		VMOVAPD( e[i], ymm_e )
		# if m > sqrt(2) then m *= 0.5
		VANDPD( ymm_amask, ymm_amask, Constant.float64x4(Log.min_normal) )
		VXORPD( ymm_m, ymm_m, ymm_amask )
		VSUBPD( ymm_t, ymm_m, ymm_one )
		if isinstance(t[i], LocalVariable):
			VMOVAPD( t[i], ymm_t )

	ymm_f = [AVXRegister() for i in range(8)]
	ymm_c20 = AVXRegister()
	ymm_c19 = AVXRegister()
	LOAD.CONSTANT( ymm_c20, Constant.float64x4(Log.c20) )
	LOAD.CONSTANT( ymm_c19, Constant.float64x4(Log.c19) )
	for i in range(8):
		VMULPD( ymm_f[i], ymm_c20, t[i] )
		VADDPD( ymm_f[i], ymm_f[i], ymm_c19 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(8):
			VMULPD( ymm_f[i], ymm_f[i], t[i] )
			VADDPD( ymm_f[i], ymm_f[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Log.c18))
	HORNER_STEP(Constant.float64x4(Log.c17))
	HORNER_STEP(Constant.float64x4(Log.c16))
	HORNER_STEP(Constant.float64x4(Log.c15))
	HORNER_STEP(Constant.float64x4(Log.c14))
	HORNER_STEP(Constant.float64x4(Log.c13))
	HORNER_STEP(Constant.float64x4(Log.c12))
	HORNER_STEP(Constant.float64x4(Log.c11))
	HORNER_STEP(Constant.float64x4(Log.c10))
	HORNER_STEP(Constant.float64x4(Log.c9))
	HORNER_STEP(Constant.float64x4(Log.c8))
	HORNER_STEP(Constant.float64x4(Log.c7))
	HORNER_STEP(Constant.float64x4(Log.c6))
	HORNER_STEP(Constant.float64x4(Log.c5))
	HORNER_STEP(Constant.float64x4(Log.c4))
	HORNER_STEP(Constant.float64x4(Log.c3))
	HORNER_STEP(Constant.float64x4(Log.c2))

	for i in range(8):
		if isinstance(t[i], AVXRegister):
			ymm_t = t[i]
		else:
			ymm_t = AVXRegister()
			VMOVAPD( ymm_t, t[i] )

		VMULPD( ymm_f[i], ymm_f[i], ymm_t )
		VMULPD( ymm_f[i], ymm_f[i], ymm_t )
		VADDPD( ymm_f[i], ymm_f[i], ymm_t )

	ymm_ln2_lo = AVXRegister()
	ymm_ln2_hi = AVXRegister()
	LOAD.CONSTANT( ymm_ln2_lo, Constant.float64x4(Log.preFMA.ln2_lo) )
	LOAD.CONSTANT( ymm_ln2_hi, Constant.float64x4(Log.preFMA.ln2_hi) )
	for i in range(8):
		ymm_e = AVXRegister()
		VMOVAPD( ymm_e, e[i] )

		ymm_temp = AVXRegister()
		VMULPD( ymm_temp, ymm_e, ymm_ln2_lo )
		VADDPD( ymm_f[i], ymm_f[i], ymm_temp )

		ymm_temp = AVXRegister()
		VMULPD( ymm_temp, ymm_e, ymm_ln2_hi )
		VADDPD( ymm_f[i], ymm_f[i], ymm_temp )

	ymm_zero = AVXRegister()
	ymm_minus_inf = AVXRegister()
	ymm_plus_inf = AVXRegister()
	ymm_nan = AVXRegister()
	VXORPD( ymm_zero, ymm_zero, ymm_zero )
	VMOVAPD( ymm_plus_inf, Constant.float64x4(Log.plus_inf) )
	VSUBPD( ymm_minus_inf, ymm_zero, ymm_plus_inf )
	VMOVAPD( ymm_nan, Constant.float64x4(Log.nan) )
	for i in range(8):
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, [xPointer + i * 32] )

		ymm_zero_mask = AVXRegister()
		VCMPEQPD( ymm_zero_mask, ymm_x, ymm_zero )
		VBLENDVPD( ymm_f[i], ymm_f[i], ymm_minus_inf, ymm_zero_mask )
		VBLENDVPD( ymm_f[i], ymm_f[i], ymm_nan, ymm_x )
		ymm_inf_nan_mask = AVXRegister()
		VCMPNLTPD( ymm_inf_nan_mask, ymm_x, ymm_plus_inf )
		VBLENDVPD( ymm_f[i], ymm_f[i], ymm_x, ymm_inf_nan_mask )

		VMOVUPD( [yPointer + i * 32], ymm_f[i].get_oword() )
		VEXTRACTF128( [yPointer + i * 32 + 16], ymm_f[i], 1 )

def BATCH_LOG_FAST_Bulldozer(xPointer, yPointer, vectorLogFullLabel):
	x_shift = GeneralPurposeRegister64()
	MOV(x_shift, Log.x_min)

	x_threshold = GeneralPurposeRegister64()
	MOV(x_threshold, Log.x_max - Log.x_min)

	x_temp = GeneralPurposeRegister64()
	check_instructions = InstructionStream()
	with check_instructions:
		for i in range(3*2):
			MOV( x_temp, [xPointer + i * 8] )
			SUB( x_temp, x_shift )
			CMP( x_temp, x_threshold )
			JA( vectorLogFullLabel )

	ymm_t = [AVXRegister() for i in range(6)]
	ymm_m = [AVXRegister() for i in range(6)]
	e = [LocalVariable(AVXRegister) for i in range(6)]

	ymm_one = xmm_exponent_bias = xmm_exponent_magic = None
	for i in range(6):
		# x = *xPointer
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, [xPointer + i * 32] )

		# e = tx & exponent_mask
		ymm_one = INIT.ONCE( AVXRegister, Constant.float64x4(Log.one), ymm_one )
		xmm_e_low = SSERegister()
		VPSRLQ( xmm_e_low, ymm_x.get_oword(), 52 )
		xmm_e_high = SSERegister()
		VEXTRACTF128( xmm_e_high, ymm_x, 1 )

		# m = (x & mantissa_mask) | one
		VPCMOV( ymm_m[i], ymm_x, ymm_one, Constant.uint64x4(Log.mantissa_mask) )
		xmm_exponent_magic = INIT.ONCE( SSERegister, Constant.uint64x2(Log.exponent_magic), xmm_exponent_magic)
		VPSRLQ( xmm_e_high, xmm_e_high, 52 )

		VPADDQ( xmm_e_low, xmm_e_low, xmm_exponent_magic )
		VPADDQ( xmm_e_high, xmm_e_high, xmm_exponent_magic )

		xmm_exponent_bias = INIT.ONCE( SSERegister, Constant.float64x2(Log.exponent_bias), xmm_exponent_bias )

		# amask = m > sqrt(2)
		ymm_amask = AVXRegister()
		VCMPGTPD( ymm_amask, ymm_m[i], Constant.float64x4(Log.sqrt2) )
		xmm_amask_high = SSERegister()
		VEXTRACTF128( xmm_amask_high, ymm_amask, 1 )
		VPSUBQ( xmm_e_low, ymm_amask.get_oword() )
		VPSUBQ( xmm_e_high, xmm_amask_high )
		# if m > sqrt(2) then m *= 0.5
		VANDPD( ymm_amask, ymm_amask, Constant.float64x4(Log.min_normal) )
		VSUBPD( xmm_e_low, xmm_e_low, xmm_exponent_bias )
		VSUBPD( xmm_e_high, xmm_e_high, xmm_exponent_bias )
		VXORPD( ymm_m[i], ymm_m[i], ymm_amask )
		VMOVAPD( e[i].get_low(), xmm_e_low )
		VSUBPD( ymm_t[i], ymm_m[i], ymm_one )
		VMOVAPD( e[i].get_high(), xmm_e_high )

	ymm_c20 = AVXRegister()
	LOAD.CONSTANT( ymm_c20, Constant.float64x4(Log.c20) )
	ymm_c19 = AVXRegister()
	LOAD.CONSTANT( ymm_c19, Constant.float64x4(Log.c19) )

	ymm_f = [AVXRegister() for i in range(6)]
	for i in range(6):
		VFMADDPD( ymm_f[i], ymm_c20, ymm_t[i], ymm_c19 )
		check_instructions.issue(2)

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(6):
			VFMADDPD( ymm_f[i], ymm_f[i], ymm_t[i], ymm_c )
			check_instructions.issue(2)

	HORNER_STEP(Constant.float64x4(Log.c18))
	HORNER_STEP(Constant.float64x4(Log.c17))
	HORNER_STEP(Constant.float64x4(Log.c16))
	HORNER_STEP(Constant.float64x4(Log.c15))
	HORNER_STEP(Constant.float64x4(Log.c14))
	HORNER_STEP(Constant.float64x4(Log.c13))
	HORNER_STEP(Constant.float64x4(Log.c12))
	HORNER_STEP(Constant.float64x4(Log.c11))
	HORNER_STEP(Constant.float64x4(Log.c10))
	HORNER_STEP(Constant.float64x4(Log.c9))
	HORNER_STEP(Constant.float64x4(Log.c8))
	HORNER_STEP(Constant.float64x4(Log.c7))
	HORNER_STEP(Constant.float64x4(Log.c6))
	HORNER_STEP(Constant.float64x4(Log.c5))
	HORNER_STEP(Constant.float64x4(Log.c4))
	HORNER_STEP(Constant.float64x4(Log.c3))
	HORNER_STEP(Constant.float64x4(Log.c2))

	assert len(check_instructions) == 0

	for i in range(6):
		VMULPD( ymm_f[i], ymm_f[i], ymm_t[i] )

	ymm_ln2_lo = AVXRegister()
	LOAD.CONSTANT( ymm_ln2_lo, Constant.float64x4(Log.FMA.ln2_lo) )
	for i in range(6):
		VFMADDPD( ymm_f[i], ymm_f[i], ymm_t[i], ymm_t[i] )

	for i in range(6):
		VFMADDPD( ymm_f[i], ymm_ln2_lo, e[i], ymm_f[i] )

	xmm_ln2_hi = SSERegister()
	LOAD.CONSTANT( xmm_ln2_hi, Constant.float64x2(Log.FMA.ln2_hi) )
	for i in range(6):
		xmm_f_high = SSERegister()
		VEXTRACTF128( xmm_f_high, ymm_f[i], 1 )
		xmm_f_low = ymm_f[i].get_oword()

		VFMADDPD( xmm_f_low, xmm_ln2_hi, e[i].get_low(), xmm_f_low )
		VMOVUPD( [yPointer + i * 32], xmm_f_low )
		VFMADDPD( xmm_f_high, xmm_ln2_hi, e[i].get_high(), xmm_f_high )
		VMOVUPD( [yPointer + i * 32 + 16], xmm_f_high )

def BATCH_LOG_FULL_Bulldozer(xPointer, yPointer):
	ymm_t = [AVXRegister() for i in range(6)]
	ymm_m = [AVXRegister() for i in range(6)]
	ymm_e = [AVXRegister() for i in range(6)]
	e = [LocalVariable(AVXRegister) for i in range(6)]

	ymm_normalized_exponent_bias = AVXRegister()
	LOAD.CONSTANT( ymm_normalized_exponent_bias, Constant.float64x4(Log.normalized_exponent_bias) )
	ymm_denormal_magic = AVXRegister()
	LOAD.CONSTANT( ymm_denormal_magic, Constant.float64x4(Log.denormal_magic) )
	ymm_one = AVXRegister()
	LOAD.CONSTANT( ymm_one, Constant.float64x4(Log.one) )
	for i in range(6):
		ymm_x = AVXRegister()
		ymm_dx = AVXRegister()
		ymm_dmask = AVXRegister()
		ymm_tx = AVXRegister()
		ymm_exponent_bias = AVXRegister()
		ymm_amask = AVXRegister()

		# x = *xPointer
		VMOVAPD( ymm_x, [xPointer + i * 32] )
		# dx = as_double(as_ulong(x) | denormal_magic) - denormal_shift
		VORPD( ymm_dx, ymm_x, ymm_denormal_magic )
		VSUBPD( ymm_dx, ymm_dx, ymm_denormal_magic )
		# dmask = x < min_normal
		VCMPLTPD( ymm_dmask, ymm_x, Constant.float64x4(Log.min_normal) )
		# tx = (dmask ? dx : x )
		VBLENDVPD( ymm_tx, ymm_x, ymm_dx, ymm_dmask )
		# m = (tx & mantissa_mask) | one
		VANDPD( ymm_m[i], ymm_tx, Constant.uint64x4(Log.mantissa_mask) )
		VORPD( ymm_m[i], ymm_m[i], ymm_one )
		# e = tx & exponent_mask
		VANDPD( ymm_e[i], ymm_tx, Constant.uint64x4(Log.exponent_mask) )
		# exponent_bias = (dmask ? denormalized_exponent_bias : normalized_exponent_bias)
		VBLENDVPD( ymm_exponent_bias, ymm_normalized_exponent_bias, Constant.float64x4(Log.denormalized_exponent_bias), ymm_dmask )
		# convert e to double
		VMOVSHDUP( ymm_e[i], ymm_e[i] )
		VBLENDPS( ymm_e[i], ymm_e[i], Constant.float64x4(Log.scaled_exponent_magic), 0xAA )
		VSUBPD( ymm_e[i], ymm_exponent_bias )

	for i in range(6):
		# amask = m > sqrt(2)
		VCMPGTPD( ymm_amask, ymm_m[i], Constant.float64x4(Log.sqrt2) )
		ymm_temp = AVXRegister()
		VANDPD( ymm_temp, ymm_amask, ymm_one )
		VADDPD( ymm_e[i], ymm_e[i], ymm_temp )
		VMOVAPD( e[i].get_low(), ymm_e[i].get_oword() )
		# if m > sqrt(2) then m *= 0.5
		VANDPD( ymm_amask, ymm_amask, Constant.float64x4(Log.min_normal) )
		VEXTRACTF128( e[i].get_high(), ymm_e[i], 1 )
		VXORPD( ymm_m[i], ymm_m[i], ymm_amask )
		VSUBPD( ymm_t[i], ymm_m[i], ymm_one )

	ymm_f = [AVXRegister() for i in range(6)]
	ymm_c20 = AVXRegister()
	ymm_c19 = AVXRegister()
	LOAD.CONSTANT( ymm_c20, Constant.float64x4(Log.c20) )
	LOAD.CONSTANT( ymm_c19, Constant.float64x4(Log.c19) )
	for i in range(6):
		VFMADDPD( ymm_f[i], ymm_c20, ymm_t[i], ymm_c19 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(6):
			VFMADDPD( ymm_f[i], ymm_f[i], ymm_t[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Log.c18))
	HORNER_STEP(Constant.float64x4(Log.c17))
	HORNER_STEP(Constant.float64x4(Log.c16))
	HORNER_STEP(Constant.float64x4(Log.c15))
	HORNER_STEP(Constant.float64x4(Log.c14))
	HORNER_STEP(Constant.float64x4(Log.c13))
	HORNER_STEP(Constant.float64x4(Log.c12))
	HORNER_STEP(Constant.float64x4(Log.c11))
	HORNER_STEP(Constant.float64x4(Log.c10))
	HORNER_STEP(Constant.float64x4(Log.c9))
	HORNER_STEP(Constant.float64x4(Log.c8))
	HORNER_STEP(Constant.float64x4(Log.c7))
	HORNER_STEP(Constant.float64x4(Log.c6))
	HORNER_STEP(Constant.float64x4(Log.c5))
	HORNER_STEP(Constant.float64x4(Log.c4))
	HORNER_STEP(Constant.float64x4(Log.c3))
	HORNER_STEP(Constant.float64x4(Log.c2))

	for i in range(6):
		VMULPD( ymm_f[i], ymm_f[i], ymm_t[i] )
		VFMADDPD( ymm_f[i], ymm_f[i], ymm_t[i], ymm_t[i] )

	ymm_ln2_lo = AVXRegister()
	ymm_ln2_hi = AVXRegister()
	LOAD.CONSTANT( ymm_ln2_lo, Constant.float64x4(Log.FMA.ln2_lo) )
	LOAD.CONSTANT( ymm_ln2_hi, Constant.float64x4(Log.FMA.ln2_hi) )
	for i in range(6):
		ymm_e = AVXRegister()
		VMOVAPD( ymm_e, e[i] )

		VFMADDPD( ymm_f[i], ymm_e, ymm_ln2_lo, ymm_f[i] )
		VFMADDPD( ymm_f[i], ymm_e, ymm_ln2_hi, ymm_f[i] )

	ymm_zero = AVXRegister()
	ymm_minus_inf = AVXRegister()
	ymm_plus_inf = AVXRegister()
	ymm_nan = AVXRegister()
	VXORPD( ymm_zero, ymm_zero, ymm_zero )
	VMOVAPD( ymm_plus_inf, Constant.float64x4(Log.plus_inf) )
	VSUBPD( ymm_minus_inf, ymm_zero, ymm_plus_inf )
	VMOVAPD( ymm_nan, Constant.float64x4(Log.nan) )
	for i in range(6):
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, [xPointer + i * 32] )

		ymm_zero_mask = AVXRegister()
		VCMPEQPD( ymm_zero_mask, ymm_x, ymm_zero )
		VBLENDVPD( ymm_f[i], ymm_f[i], ymm_minus_inf, ymm_zero_mask )
		VBLENDVPD( ymm_f[i], ymm_f[i], ymm_nan, ymm_x )
		ymm_inf_nan_mask = AVXRegister()
		VCMPNLTPD( ymm_inf_nan_mask, ymm_x, ymm_plus_inf )
		VBLENDVPD( ymm_f[i], ymm_f[i], ymm_x, ymm_inf_nan_mask )

		VMOVUPD( [yPointer + i * 32], ymm_f[i].get_oword() )
		VEXTRACTF128( [yPointer + i * 32 + 16], ymm_f[i], 1 )

def BATCH_LOG_FAST_Haswell(xPointer, yPointer, vectorLogFullLabel):
	t = [AVXRegister() if i % 2 == 0 else LocalVariable(AVXRegister) for i in range(10)]
	e = [LocalVariable(AVXRegister) for i in range(10)]
	ymm_x = [AVXRegister() for _ in range(10)]

	ymm_one = ymm_exponent_bias = ymm_exponent_magic = ymm_min_normal = ymm_max_normal = None
	for i in range(10):
		# x = *xPointer
		VMOVAPD( ymm_x[i], [xPointer + i * 32] )

		ymm_min_normal = INIT.ONCE( AVXRegister, Constant.float64x4(Log.min_normal), ymm_min_normal )
		ymm_below_min_mask = AVXRegister()
		VCMPLTPD( ymm_below_min_mask, ymm_x[i], ymm_min_normal )

		# e = tx & exponent_mask
		ymm_one = INIT.ONCE( AVXRegister, Constant.float64x4(Log.one), ymm_one )
		ymm_e = AVXRegister()
		VPSRLQ( ymm_e, ymm_x[i], 52 )

		ymm_above_max_mask = AVXRegister()
		VCMPGTPD( ymm_above_max_mask, ymm_x[i], Constant.float64x4(Log.max_normal) )

		# m = (x & mantissa_mask) | one
		VPAND( ymm_x[i], Constant.uint64x4(Log.mantissa_mask) )
		VPOR( ymm_x[i], ymm_one )


		ymm_beyond_normal_mask = AVXRegister()
		VORPD( ymm_beyond_normal_mask, ymm_below_min_mask, ymm_above_max_mask )

		ymm_exponent_magic = INIT.ONCE( AVXRegister, Constant.uint64x4(Log.exponent_magic), ymm_exponent_magic)
		VPADDQ( ymm_e, ymm_e, ymm_exponent_magic )


		# amask = m > sqrt(2)
		ymm_amask = AVXRegister()
		VCMPGTPD( ymm_amask, ymm_x[i], Constant.float64x4(Log.sqrt2) )
		VPSUBQ( ymm_e, ymm_amask )

		VTESTPD( ymm_beyond_normal_mask, ymm_beyond_normal_mask )
		JNZ( vectorLogFullLabel )

		# if m > sqrt(2) then m *= 0.5
		VANDPD( ymm_amask, Constant.float64x4(Log.min_normal) )
		ymm_exponent_bias = INIT.ONCE( AVXRegister, Constant.float64x4(Log.exponent_bias), ymm_exponent_bias )
		VSUBPD( ymm_e, ymm_exponent_bias )
		VXORPD( ymm_x[i], ymm_amask )
		VMOVAPD( e[i], ymm_e )
		VFMSUB213PD( ymm_x[i], ymm_x[i], ymm_one, ymm_one )
		if isinstance(t[i], AVXRegister):
			t[i] = ymm_x[i]
		else:
			VMOVAPD( t[i], ymm_x[i] )

	ymm_f = [AVXRegister() for i in range(10)]
	LOAD.CONSTANT( ymm_f[9], Constant.float64x4(Log.c20) )

	ymm_c19 = AVXRegister()
	LOAD.CONSTANT( ymm_c19, Constant.float64x4(Log.c19) )
	for i in range(10):
		VMOVAPD( ymm_f[i], ymm_f[9] )
		if i != 9:
			VFMADD132PD( ymm_f[i], ymm_f[i], ymm_x[i], ymm_c19 )
		else:
			VFMADD132PD( ymm_f[i], ymm_f[i], t[i], ymm_c19 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(10):
			VFMADD132PD( ymm_f[i], ymm_f[i], t[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Log.c18))
	HORNER_STEP(Constant.float64x4(Log.c17))
	HORNER_STEP(Constant.float64x4(Log.c16))
	HORNER_STEP(Constant.float64x4(Log.c15))
	HORNER_STEP(Constant.float64x4(Log.c14))
	HORNER_STEP(Constant.float64x4(Log.c13))
	HORNER_STEP(Constant.float64x4(Log.c12))
	HORNER_STEP(Constant.float64x4(Log.c11))
	HORNER_STEP(Constant.float64x4(Log.c10))
	HORNER_STEP(Constant.float64x4(Log.c9))
	HORNER_STEP(Constant.float64x4(Log.c8))
	HORNER_STEP(Constant.float64x4(Log.c7))
	HORNER_STEP(Constant.float64x4(Log.c6))
	HORNER_STEP(Constant.float64x4(Log.c5))
	HORNER_STEP(Constant.float64x4(Log.c4))
	HORNER_STEP(Constant.float64x4(Log.c3))
	HORNER_STEP(Constant.float64x4(Log.c2))

	for i in range(10):
		if isinstance(t[i], AVXRegister):
			ymm_t = t[i]
		else:
			ymm_t = AVXRegister()
			VMOVAPD( ymm_t, t[i] )
		VMULPD( ymm_f[i], ymm_f[i], ymm_t )
		VFMADD132PD( ymm_f[i], ymm_f[i], ymm_t, ymm_t )

	ymm_ln2_lo = ymm_ln2_hi = None 
	for i in range(10):
		ymm_e = AVXRegister()
		VMOVAPD( ymm_e, e[i] )
		ymm_ln2_lo = INIT.ONCE( AVXRegister, Constant.float64x4(Log.FMA.ln2_lo), ymm_ln2_lo )
		VFMADD231PD( ymm_f[i], ymm_ln2_lo, ymm_e, ymm_f[i] )
		ymm_ln2_hi = INIT.ONCE( AVXRegister, Constant.float64x4(Log.FMA.ln2_hi), ymm_ln2_hi )
		VFMADD231PD( ymm_f[i], ymm_ln2_hi, ymm_e, ymm_f[i] )
		VMOVUPD( [yPointer + i * 32], ymm_f[i] )

def BATCH_LOG_FULL_Haswell(xPointer, yPointer):
	ymm_x = [AVXRegister() for i in range(10)]
	e = [LocalVariable(AVXRegister) for i in range(10)]

	ymm_one = ymm_denormal_magic = ymm_sqrt2 = None
	for i in range(10):
		# x = *xPointer
		VMOVAPS( ymm_x[i], [xPointer + i * 32] )
		# ne = as_ulong(x) >> 52
		ymm_ne = AVXRegister()
		VPSRLQ( ymm_ne, ymm_x[i], 52 )
		# dx = as_double(as_ulong(x) | denormal_magic) - denormal_bias
		ymm_dx = AVXRegister()
		ymm_denormal_magic = INIT.ONCE(AVXRegister, Constant.float64x4(Log.denormal_magic), ymm_denormal_magic )
		VORPS( ymm_dx, ymm_x[i], ymm_denormal_magic )
		VSUBPD( ymm_dx, ymm_denormal_magic )
		# dmask = (ne == 0)
		ymm_dmask = AVXRegister()
		LOAD.ZERO( ymm_dmask, peachpy.c.Type('uint64_t') )
		VPCMPEQQ( ymm_dmask, ymm_ne )
		# e = (as_ulong(dx) >> 52) - denormal_exponent_shift
		ymm_e = AVXRegister()
		VPSRLQ( ymm_e, ymm_dx, 52 )
		VPSUBQ( ymm_e, Constant.uint64x4(Log.denormal_exponent_shift) )
		# m = dmask ? dx : x
		VPBLENDVB( ymm_x[i], ymm_x[i], ymm_dx, ymm_dmask )
		# m = (m & mantissa_mask) | default_exponent
		VPAND( ymm_x[i], Constant.uint64x4(Log.mantissa_mask) )
		ymm_one = INIT.ONCE(AVXRegister, Constant.float64x4(Log.one), ymm_one )
		VPOR( ymm_x[i], ymm_one )
		# e = (e & dmask) | ne
		VPAND( ymm_e, ymm_dmask )
		VPOR( ymm_e, ymm_ne )

		# amask = m >= sqrt2
		ymm_sqrt2 = INIT.ONCE(AVXRegister, Constant.float64x4(Log.sqrt2), ymm_sqrt2 )
		ymm_amask = AVXRegister()
		VCMPLTPD( ymm_amask, ymm_sqrt2, ymm_x[i] )
		# If (amask) then e += 1
		VPSUBQ( ymm_e, ymm_amask )
		# If (amask) then m *= 0.5
		VPAND( ymm_amask, Constant.float64x4(Log.min_normal) )
		VPSUBD( ymm_x[i], ymm_amask )
		VMOVAPD( e[i], ymm_e )

	ymm_rf = [AVXRegister() for i in range(10)]
	LOAD.CONSTANT( ymm_rf[9], Constant.float64x4(Log.c20) )

	t = [AVXRegister() if i % 2 == 0 else LocalVariable(AVXRegister) for i in range(10)]
	for i in range(10):
		# t = m - 1.0
		if i % 2 == 0:
			VSUBPD( ymm_x[i], ymm_one )
		else:
			VFMSUB132PD( ymm_x[i], ymm_x[i], ymm_one, ymm_one )
		if isinstance(t[i], AVXRegister):
			t[i] = ymm_x[i]
		else:
			VMOVAPD( t[i], ymm_x[i] )

	ymm_c19 = AVXRegister()
	LOAD.CONSTANT( ymm_c19, Constant.float64x4(Log.c19) )
	for i in range(10):
		VMOVAPD( ymm_rf[i], ymm_rf[9] )
		if i != 9:
			VFMADD132PD( ymm_rf[i], ymm_rf[i], ymm_x[i], ymm_c19 )
		else:
			VFMADD132PD( ymm_rf[i], ymm_rf[i], t[i], ymm_c19 )


	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(10):
			VFMADD132PD( ymm_rf[i], ymm_rf[i], t[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Log.c18))
	HORNER_STEP(Constant.float64x4(Log.c17))
	HORNER_STEP(Constant.float64x4(Log.c16))
	HORNER_STEP(Constant.float64x4(Log.c15))
	HORNER_STEP(Constant.float64x4(Log.c14))
	HORNER_STEP(Constant.float64x4(Log.c13))
	HORNER_STEP(Constant.float64x4(Log.c12))
	HORNER_STEP(Constant.float64x4(Log.c11))
	HORNER_STEP(Constant.float64x4(Log.c10))
	HORNER_STEP(Constant.float64x4(Log.c9))
	HORNER_STEP(Constant.float64x4(Log.c8))
	HORNER_STEP(Constant.float64x4(Log.c7))
	HORNER_STEP(Constant.float64x4(Log.c6))
	HORNER_STEP(Constant.float64x4(Log.c5))
	HORNER_STEP(Constant.float64x4(Log.c4))
	HORNER_STEP(Constant.float64x4(Log.c3))
	HORNER_STEP(Constant.float64x4(Log.c2))

	# rf = (pt * t) * t + t
	for i in range(10):
		if isinstance(t[i], AVXRegister):
			ymm_t = t[i]
		else:
			ymm_t = AVXRegister()
			VMOVAPD( ymm_t, t[i] )

		VMULPD( ymm_rf[i], ymm_t )
		VFMADD213PD( ymm_rf[i], ymm_rf[i], ymm_t, ymm_t )

	ymm_f = [AVXRegister() for i in range(10)]

	ymm_exponent_magic = ymm_exponent_bias = ymm_ln2_lo = ymm_ln2_hi = None
	for i in range(10):
		ymm_e = AVXRegister()

		VMOVAPD( ymm_e, e[i] )
		# e = as_double(as_ulong(e) + exponent_magic) - exponent_bias
		ymm_exponent_magic = INIT.ONCE(AVXRegister, Constant.uint64x4(Log.exponent_magic), ymm_exponent_magic )
		VPADDQ( ymm_e, ymm_exponent_magic )
		ymm_exponent_bias = INIT.ONCE(AVXRegister, Constant.float64x4(Log.exponent_bias), ymm_exponent_bias )
		VSUBPD( ymm_e, ymm_exponent_bias )

		# f = e * ln2_lo + rf
		VMOVAPS( ymm_f[i], ymm_e )
		ymm_ln2_lo = INIT.ONCE(AVXRegister, Constant.float64x4(Log.FMA.ln2_lo), ymm_ln2_lo )
		VFMADD213PD( ymm_f[i], ymm_f[i], ymm_ln2_lo, ymm_rf[i] )
		# f += e * ln2_hi
		ymm_ln2_hi = INIT.ONCE(AVXRegister, Constant.float64x4(Log.FMA.ln2_hi), ymm_ln2_hi )
		VFMADD231PD( ymm_f[i], ymm_e, ymm_ln2_hi, ymm_f[i] )

	ymm_nan = ymm_plus_inf = None
	for i in range(10):
		ymm_x = AVXRegister()

		# x = *xPointer
		VMOVAPD( ymm_x, [xPointer + i * 32] )
		# if sign(x) == -1 then f = NaN
		ymm_nan = INIT.ONCE(AVXRegister, Constant.float64x4(Log.nan), ymm_nan)
		VBLENDVPD( ymm_f[i], ymm_f[i], ymm_nan, ymm_x )

		# if !(x < inf) then f = x
		ymm_plus_inf = INIT.ONCE(AVXRegister, Constant.float64x4(Log.plus_inf), ymm_plus_inf)
		ymm_inf_mask = AVXRegister()
		VCMPNLTPD( ymm_inf_mask, ymm_x, ymm_plus_inf )
		VBLENDVPD( ymm_f[i], ymm_f[i], ymm_x, ymm_inf_mask )

		ymm_zero_mask = AVXRegister()
		LOAD.ZERO( ymm_zero_mask, peachpy.c.Type('uint64_t') )
		VPCMPEQQ( ymm_zero_mask, ymm_x, ymm_zero_mask )
		# if (x == +0.0) f = -inf
		VBLENDVPD( ymm_f[i], ymm_f[i], Constant.float64x4(Log.minus_inf), ymm_zero_mask )

		# *yPointer = f
		VMOVUPD( [yPointer + i * 32], ymm_f[i] )

def Log_V64f_V64f(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-sysv', 'x64-ms']:
		if module == 'Math':
			if function == 'Log':
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not x_type.is_floating_point() or not y_type.is_floating_point():
					return

				if x_type.get_size(codegen.abi) != 8 or y_type.get_size(codegen.abi) != 8:
					return

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bobcat', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_LOG_SSE, BATCH_LOG_FULL_Bobcat, BATCH_LOG_FAST_Bobcat, xPointer, yPointer, length, 16, 16 * 3, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'K10', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_LOG_SSE, BATCH_LOG_FULL_K10, BATCH_LOG_FAST_K10, xPointer, yPointer, length, 16, 16 * 8, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_LOG_SSE, BATCH_LOG_FULL_Nehalem, BATCH_LOG_FAST_Nehalem, xPointer, yPointer, length, 16, 16 * 8, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_LOG_AVX, BATCH_LOG_FULL_SandyBridge, BATCH_LOG_FAST_SandyBridge, xPointer, yPointer, length, 32, 32 * 8, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )

					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )

					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )

					Map_Vf_Vf(SCALAR_LOG_AVX, BATCH_LOG_FULL_Bulldozer, BATCH_LOG_FAST_Bulldozer, xPointer, yPointer, length, 32, 32 * 6, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )

					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )

					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )

					Map_Vf_Vf(SCALAR_LOG_AVX, BATCH_LOG_FULL_Haswell, BATCH_LOG_FAST_Haswell, xPointer, yPointer, length, 32, 32 * 10, 8)

def SCALAR_EXP_SSE(xPointer, yPointer, is_prologue):
	if Target.get_int_eu_width() == 64:
		def SCALAR_COPY(destination, source):
			ASSUME.INITIALIZED( destination )
			MOVSD( destination, source )
	else:
		def SCALAR_COPY(destination, source):
			MOVAPS( destination, source )

	# x = *xPointer
	xmm_x = SSERegister()
	MOVSD( xmm_x, [xPointer] )

	xmm_magic_bias = SSERegister()
	LOAD.CONSTANT( xmm_magic_bias, Constant.float64(Exp.magic_bias) )

	# t = x * log2e + magic_bias
	xmm_t = SSERegister()
	SCALAR_COPY( xmm_t, xmm_x )
	MULSD( xmm_t, Constant.float64(Exp.log2e) )
	ADDSD( xmm_t, xmm_magic_bias )

	# e2 = as_uint(t) << 52
	xmm_e2 = SSERegister()
	MOVDQA( xmm_e2, xmm_t )
	PSLLQ( xmm_e2, 52 )
	# t = t - magic_bias
	SUBSD( xmm_t, xmm_magic_bias )
	# rx = t * (-ln2.high) + x
	xmm_rx = SSERegister()
	SCALAR_COPY( xmm_rx, xmm_t )
	MULSD( xmm_rx, Constant.float64(Exp.preFMA.minus_ln2_hi) )
	ADDSD( xmm_rx, xmm_x )
	# rx = t * (-ln2.low) + rx
	MULSD( xmm_t, Constant.float64(Exp.preFMA.minus_ln2_lo) )
	ADDSD( xmm_rx, xmm_t )

	# e1 = min(e2, max_exponent)
	xmm_e1 = SSERegister()
	MOVDQA( xmm_e1, xmm_e2 )
	PMINSW( xmm_e1, Constant.uint64x2(Exp.max_exponent) )
	# e1 = max(e1, min_exponent)
	PMAXSW( xmm_e1, Constant.uint64x2(Exp.min_exponent) )
	# e2 -= e1
	PSUBD( xmm_e2, xmm_e1 )

	xmm_default_exponent = SSERegister()
	LOAD.CONSTANT( xmm_default_exponent, Constant.uint64x2(Exp.default_exponent) )
	# s1 = as_double(e1 + default_exponent)
	PADDD( xmm_e1, xmm_default_exponent)
	# s2 = as_double(e2 + default_exponent)
	PADDD( xmm_e2, xmm_default_exponent)

	xmm_rf = SSERegister()
	LOAD.CONSTANT( xmm_rf, Constant.float64(Exp.c11) )

	def HORNER_STEP(coef):
		MULSD( xmm_rf, xmm_rx )
		ADDSD( xmm_rf, coef )

	HORNER_STEP(Constant.float64(Exp.c10))
	HORNER_STEP(Constant.float64(Exp.c9))
	HORNER_STEP(Constant.float64(Exp.c8))
	HORNER_STEP(Constant.float64(Exp.c7))
	HORNER_STEP(Constant.float64(Exp.c6))
	HORNER_STEP(Constant.float64(Exp.c5))
	HORNER_STEP(Constant.float64(Exp.c4))
	HORNER_STEP(Constant.float64(Exp.c3))
	HORNER_STEP(Constant.float64(Exp.c2))

	# rf = rf * rx
	MULSD( xmm_rf, xmm_rx )
	# rf = rf * rx + rx
	MULSD( xmm_rf, xmm_rx )
	ADDSD( xmm_rf, xmm_rx )
	# rf = rf * s1 + s1
	MULSD( xmm_rf, xmm_e1 )
	ADDSD( xmm_rf, xmm_e1 )
	# rf = rf * s2
	MULSD( xmm_rf, xmm_e2 )

	if Target.has_sse4_1():
		xmm_inf_mask = xmm0
	else:
		xmm_inf_mask = SSERegister()
	LOAD.CONSTANT( xmm_inf_mask, Constant.float64(Exp.inf_cutoff) )
	# Fixup overflow:
	# - If x > inf_cutoff then inf_mask is true
	# - If x is NaN then inf_mask is false
	CMPLESD( xmm_inf_mask, xmm_x )
	# If (inf_mask) rf = inf
	if Target.has_sse4_1():
		BLENDVPD( xmm_rf, Constant.float64x2(Exp.plus_inf), xmm_inf_mask )
	else:
		xmm_plus_inf = SSERegister()
		LOAD.CONSTANT( xmm_plus_inf, Constant.float64(Exp.plus_inf) )
		ANDPS( xmm_plus_inf, xmm_inf_mask )
		ANDNPS( xmm_inf_mask, xmm_rf )
		ORPS( xmm_inf_mask, xmm_plus_inf )
		xmm_rf = xmm_inf_mask
		
	# Fixup underflow to zero:
	# - If x < zero_cutoff then zero_mask is true
	# - If x is NaN then zero_mask is false
	CMPLTSD( xmm_x, Constant.float64(Exp.zero_cutoff) )
	xmm_zero_mask = xmm_x

	# - If (zero_mask) rf = 0.0
	ANDNPD( xmm_zero_mask, xmm_rf )
	xmm_rf = xmm_zero_mask

	MOVSD( [yPointer], xmm_rf )

def SCALAR_EXP_AVX(xPointer, yPointer, is_prologue):
	# x = *xPointer
	xmm_x = SSERegister()
	VMOVSD( xmm_x, [xPointer] )

	xmm_magic_bias = SSERegister()
	LOAD.CONSTANT( xmm_magic_bias, Constant.float64(Exp.magic_bias) )
	# t = x * log2e + magic_bias
	xmm_t = SSERegister()
	ASSUME.INITIALIZED( xmm_t )
	if Target.has_fma4():
		VFMADDSD( xmm_t, xmm_x, Constant.float64(Exp.log2e), xmm_magic_bias )
	elif Target.has_fma3():
		LOAD.CONSTANT( xmm_t, Constant.float64(Exp.log2e) )
		VFMADD132SD( xmm_t, xmm_t, xmm_x, xmm_magic_bias )
	else:
		VMULSD( xmm_t, xmm_x, Constant.float64(Exp.log2e) )
		VADDSD( xmm_t, xmm_t, xmm_magic_bias )
	# e2 = as_uint(t) << 52
	xmm_e2 = SSERegister()
	VPSLLQ( xmm_e2, xmm_t, 52 )
	# t = t - magic_bias
	VSUBSD( xmm_t, xmm_t, xmm_magic_bias )
	# rx = t * (-ln2.high) + x
	xmm_rx = SSERegister()
	ASSUME.INITIALIZED( xmm_rx )
	if Target.has_fma4():
		VFMADDSD( xmm_rx, xmm_t, Constant.float64(Exp.FMA.minus_ln2_hi), xmm_x )
	elif Target.has_fma3():
		LOAD.CONSTANT( xmm_rx, Constant.float64(Exp.FMA.minus_ln2_hi) )
		VFMADD132SD( xmm_rx, xmm_rx, xmm_t, xmm_x )
	else:
		VMULSD( xmm_rx, xmm_t, Constant.float64(Exp.preFMA.minus_ln2_hi) )
		VADDSD( xmm_rx, xmm_rx, xmm_x )
	# rx = t * (-ln2.low) + rx
	if Target.has_fma4():
		VFMADDSD( xmm_rx, xmm_t, Constant.float64(Exp.FMA.minus_ln2_lo), xmm_rx )
	elif Target.has_fma3():
		VFMADD231SD( xmm_rx, xmm_t, Constant.float64(Exp.FMA.minus_ln2_lo), xmm_rx )
	else:
		xmm_temp = SSERegister()
		ASSUME.INITIALIZED( xmm_temp )
		VMULSD( xmm_temp, xmm_t, Constant.float64(Exp.preFMA.minus_ln2_lo) )
		VADDSD( xmm_rx, xmm_rx, xmm_temp )

	# e1 = min(e2, max_exponent)
	xmm_e1 = SSERegister()
	VPMINSD( xmm_e1, xmm_e2, Constant.uint64x2(Exp.max_exponent) )
	# e1 = max(e1, min_exponent)
	VPMAXSD( xmm_e1, xmm_e1, Constant.uint64x2(Exp.min_exponent) )
	# e2 -= e1
	VPSUBD( xmm_e2, xmm_e2, xmm_e1 )

	xmm_default_exponent = SSERegister()
	LOAD.CONSTANT( xmm_default_exponent, Constant.uint64x2(Exp.default_exponent) )
	# s1 = as_double(e1 + default_exponent)
	xmm_s1 = SSERegister()
	VPADDD( xmm_s1, xmm_e1, xmm_default_exponent )
	# s2 = as_double(e2 + default_exponent)
	xmm_s2 = SSERegister()
	VPADDD( xmm_s2, xmm_e2, xmm_default_exponent )

	xmm_rf = SSERegister()
	LOAD.CONSTANT( xmm_rf, Constant.float64(Exp.c11) )

	def HORNER_STEP(coef):
		if Target.has_fma4():
			VFMADDSD( xmm_rf, xmm_rf, xmm_rx, coef )
		elif Target.has_fma3():
			VFMADD213SD( xmm_rf, xmm_rf, xmm_rx, coef )
		else:
			VMULSD( xmm_rf, xmm_rf, xmm_rx )
			VADDSD( xmm_rf, xmm_rf, coef )

	HORNER_STEP(Constant.float64(Exp.c10))
	HORNER_STEP(Constant.float64(Exp.c9))
	HORNER_STEP(Constant.float64(Exp.c8))
	HORNER_STEP(Constant.float64(Exp.c7))
	HORNER_STEP(Constant.float64(Exp.c6))
	HORNER_STEP(Constant.float64(Exp.c5))
	HORNER_STEP(Constant.float64(Exp.c4))
	HORNER_STEP(Constant.float64(Exp.c3))
	HORNER_STEP(Constant.float64(Exp.c2))

	VMULSD( xmm_rf, xmm_rf, xmm_rx )
	if Target.has_fma4():
		VFMADDSD( xmm_rf, xmm_rf, xmm_rx, xmm_rx )
	elif Target.has_fma3():
		VFMADD132SD( xmm_rf, xmm_rf, xmm_rx, xmm_rx )
	else:
		VMULSD( xmm_rf, xmm_rf, xmm_rx )
		VADDSD( xmm_rf, xmm_rf, xmm_rx )
	if Target.has_fma4():
		VFMADDSD( xmm_rf, xmm_rf, xmm_s1, xmm_s1 )
	elif Target.has_fma3():
		VFMADD132SD( xmm_rf, xmm_rf, xmm_s1, xmm_s1 )
	else:
		VMULSD( xmm_rf, xmm_rf, xmm_s1 )
		VADDSD( xmm_rf, xmm_rf, xmm_s1 )
	VMULSD( xmm_rf, xmm_rf, xmm_s2 )

	# Fixup underflow to zero:
	# - If x < zero_cutoff then zeroMask is true
	# - If x is NaN then zeroMask is false
	xmm_zero_mask = SSERegister()
	ASSUME.INITIALIZED( xmm_zero_mask )
	VCMPLTSD( xmm_zero_mask, xmm_x, Constant.float64(Exp.zero_cutoff) )
	# - If (zeroMask) rf = 0.0
	VANDNPD( xmm_rf, xmm_zero_mask, xmm_rf )

	# Fixup overflow:
	# - If x > inf_cutoff then infMask is true
	# - If x is NaN then infMask is false
	xmm_inf_mask = SSERegister()
	ASSUME.INITIALIZED( xmm_inf_mask )
	VCMPGTSD( xmm_inf_mask, xmm_x, Constant.float64(Exp.inf_cutoff) )
	# If (infMask) rf = inf
	VBLENDVPD( xmm_rf, xmm_rf, Constant.float64x2(Exp.plus_inf), xmm_inf_mask )
	VMOVSD( [yPointer], xmm_rf )

def BATCH_EXP_FAST_Bobcat(xPointer, yPointer, vectorExpFullLabel):
	x_temp = GeneralPurposeRegister64()
	x_min_normal = GeneralPurposeRegister64()
	x_max_normal = GeneralPurposeRegister64()
	check_instructions = InstructionStream()
	with check_instructions:
		MOV( x_min_normal, Exp.preFMA.x_min )
		MOV( x_max_normal, Exp.preFMA.x_max - 1 )
		for i in range(4*2):
			MOV( x_temp, [xPointer + i * 8] )
			CMP( x_temp, x_min_normal )
			CMOVA( x_temp, x_max_normal )
			CMP( x_temp, x_max_normal )
			JGE( vectorExpFullLabel )

	xmm_log2e = xmm_magic_bias = xmm_default_exponent = xmm_minus_ln2_hi = xmm_minus_ln2_lo = None
	xmm_t = [SSERegister() for i in range(4)]
	for i in range(4):
		# x = *xPointer
		xmm_x = SSERegister()
		MOVAPD( xmm_x, [xPointer + i * 16] )
		# t = x * log2e + magic_bias
		MOVAPD( xmm_t[i], xmm_x )
		xmm_log2e = INIT.ONCE( SSERegister, Constant.float64x2(Exp.log2e), xmm_log2e )
		MULPD( xmm_t[i], xmm_log2e )
		xmm_magic_bias = INIT.ONCE( SSERegister, Constant.float64x2(Exp.magic_bias), xmm_magic_bias )
		ADDPD( xmm_t[i], xmm_magic_bias )
		check_instructions.issue()

	xmm_e = [SSERegister() for i in range(4)]
	for i in range(4):
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		# e = as_uint(t) << 52
		MOVDQA( xmm_e[i], xmm_t[i] )
		PSLLQ( xmm_e[i], 52 )
		# s = as_double(e + default_exponent)
		PADDD( xmm_e[i], xmm_default_exponent )
		# t = t - magic_bias
		SUBPD( xmm_t[i], xmm_magic_bias )
		check_instructions.issue()

	xmm_minus_ln2_hi = INIT.ONCE( SSERegister, Constant.float64x2(Exp.preFMA.minus_ln2_hi), xmm_minus_ln2_hi )
	xmm_rx = [SSERegister() for i in range(4)]
	for i in range(4):
		# rx = t * (-ln2.high) + x
		MOVAPS( xmm_rx[i], xmm_t[i] )
		MULPD( xmm_rx[i], xmm_minus_ln2_hi )
		ADDPD( xmm_rx[i], [xPointer + i * 16] )
	for i in range(4):
		# rx = t * (-ln2.low) + rx
		xmm_minus_ln2_lo = INIT.ONCE( SSERegister, Constant.float64x2(Exp.preFMA.minus_ln2_lo), xmm_minus_ln2_lo )
		MULPD( xmm_t[i], xmm_minus_ln2_lo )
		ADDPD( xmm_rx[i], xmm_t[i] )
		check_instructions.issue()

	xmm_rf = [SSERegister() for i in range(4)]
	MOVAPS( xmm_rf[3], Constant.float64x2(Exp.c11) )
	xmm_c10 = None
	for i in range(4):
		if i != 3:
			MOVAPS( xmm_rf[i], xmm_rf[3] )
		else:
			check_instructions.issue()
		MULPD( xmm_rf[i], xmm_rx[i] )
		xmm_c10 = INIT.ONCE( SSERegister, Constant.float64x2(Exp.c10), xmm_c10 )
		ADDPD( xmm_rf[i], xmm_c10 )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(4):
			MULPD( xmm_rf[i], xmm_rx[i] )
			ADDPD( xmm_rf[i], xmm_c )
			check_instructions.issue()

	HORNER_STEP(Constant.float64x2(Exp.c9))
	HORNER_STEP(Constant.float64x2(Exp.c8))
	HORNER_STEP(Constant.float64x2(Exp.c7))
	HORNER_STEP(Constant.float64x2(Exp.c6))
	HORNER_STEP(Constant.float64x2(Exp.c5))
	HORNER_STEP(Constant.float64x2(Exp.c4))
	HORNER_STEP(Constant.float64x2(Exp.c3))
	HORNER_STEP(Constant.float64x2(Exp.c2))
	
	for i in range(4):
		# rf = rf * rx
		MULPD( xmm_rf[i], xmm_rx[i] )
		check_instructions.issue()

	for i in range(4):
		# rf = rf * rx + rx
		MULPD( xmm_rf[i], xmm_rx[i] )
		ADDPD( xmm_rf[i], xmm_rx[i] )

	assert len(check_instructions) == 0

	for i in range(4):
		# rf = rf * s + s
		MULPD( xmm_rf[i], xmm_e[i] )
		ADDPD( xmm_rf[i], xmm_e[i] )
		MOVUPS( [yPointer + i * 16], xmm_rf[i] )
	

def BATCH_EXP_FULL_Bobcat(xPointer, yPointer):
	xmm_x = [SSERegister() for i in range(4)]
	xmm_t = [SSERegister() for i in range(4)]
	xmm_rx = [SSERegister() for i in range(4)]

	xmm_log2e = xmm_magic_bias = xmm_default_exponent = xmm_max_exponent = xmm_min_exponent = None
	e1 = [LocalVariable(SSERegister) for i in range(4)]
	for i in range(4):
		# x = *xPointer
		MOVAPD( xmm_x[i], [xPointer + i * 16] )
		# t = x * log2e + magic_bias
		MOVAPD( xmm_t[i], xmm_x[i] )
		xmm_log2e = INIT.ONCE( SSERegister, Constant.float64x2(Exp.log2e), xmm_log2e )
		MULPD( xmm_t[i], xmm_log2e )
		xmm_magic_bias = INIT.ONCE( SSERegister, Constant.float64x2(Exp.magic_bias), xmm_magic_bias )
		ADDPD( xmm_t[i], xmm_magic_bias )

	xmm_e2 = [SSERegister() for i in range(4)]
	for i in range(4):
		# e2 = as_uint(t) << 52
		MOVDQA( xmm_e2[i], xmm_t[i] )
		PSLLQ( xmm_e2[i], 52 )
		# t = t - magic_bias
		SUBPD( xmm_t[i], xmm_magic_bias )

	xmm_minus_ln2_hi = xmm_minus_ln2_lo = None
	for i in range(4):
		# rx = t * (-ln2.high) + x
		MOVAPS( xmm_rx[i], xmm_t[i] )
		xmm_minus_ln2_hi = INIT.ONCE( SSERegister, Constant.float64x2(Exp.preFMA.minus_ln2_hi), xmm_minus_ln2_hi )
		MULPD( xmm_rx[i], xmm_minus_ln2_hi )
		ADDPD( xmm_rx[i], xmm_x[i] )
		# rx = t * (-ln2.low) + rx
		xmm_minus_ln2_lo = INIT.ONCE( SSERegister, Constant.float64x2(Exp.preFMA.minus_ln2_lo), xmm_minus_ln2_lo )
		MULPD( xmm_t[i], xmm_minus_ln2_lo )
		ADDPD( xmm_rx[i], xmm_t[i] )

	for i in range(4):
		xmm_e1 = SSERegister()
		xmm_max_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.max_exponent), xmm_max_exponent )
		# e1 = min(e2, max_exponent)
		MOVAPD( xmm_e1, xmm_e2[i] )
		PMINSW( xmm_e1, xmm_max_exponent )
		xmm_min_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.min_exponent), xmm_min_exponent )
		# e1 = max(e1, min_exponent)
		PMAXSW( xmm_e1, xmm_min_exponent )
		# e2 -= e1
		PSUBD( xmm_e2[i], xmm_e1 )
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		# s1 = as_double(e1 + default_exponent)
		PADDD( xmm_e1, xmm_default_exponent)
		MOVDQA( e1[i], xmm_e1 )
		# s2 = as_double(e2 + default_exponent)
		PADDD( xmm_e2[i], xmm_default_exponent)

	xmm_rf = [SSERegister() for i in range(4)]

	MOVAPS( xmm_rf[3], Constant.float64x2(Exp.c11) )

	xmm_c10 = SSERegister()
	LOAD.CONSTANT( xmm_c10, Constant.float64x2(Exp.c10) )
	for i in range(4):
		if i != 3:
			MOVAPS( xmm_rf[i], xmm_rf[3] )
		MULPD( xmm_rf[i], xmm_rx[i] )
		ADDPD( xmm_rf[i], xmm_c10 )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(4):
			MULPD( xmm_rf[i], xmm_rx[i] )
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Exp.c9))
	HORNER_STEP(Constant.float64x2(Exp.c8))
	HORNER_STEP(Constant.float64x2(Exp.c7))
	HORNER_STEP(Constant.float64x2(Exp.c6))
	HORNER_STEP(Constant.float64x2(Exp.c5))
	HORNER_STEP(Constant.float64x2(Exp.c4))
	HORNER_STEP(Constant.float64x2(Exp.c3))
	HORNER_STEP(Constant.float64x2(Exp.c2))

	for i in range(4):
		# rf = rf * rx
		MULPD( xmm_rf[i], xmm_rx[i] )
		# rf = rf * rx + rx
		MULPD( xmm_rf[i], xmm_rx[i] )
		ADDPD( xmm_rf[i], xmm_rx[i] )

	for i in range(4):
		xmm_e1 = SSERegister()
		MOVAPS( xmm_e1, e1[i] )
		# rf = rf * s1 + s1
		MULPD( xmm_rf[i], xmm_e1 )
		ADDPD( xmm_rf[i], xmm_e1 )

	for i in range(4):
		# rf = rf * s2
		MULPD( xmm_rf[i], xmm_e2[i] )

	xmm_inf_cutoff = xmm_zero_cutoff = None
	for i in range(4):
		xmm_x = SSERegister()
		MOVAPS( xmm_x, [xPointer + i * 16] )

		xmm_inf_cutoff = INIT.ONCE( SSERegister, Constant.float64x2(Exp.inf_cutoff), xmm_inf_cutoff )
		xmm_inf_mask = SSERegister()
		MOVAPS( xmm_inf_mask, xmm_inf_cutoff )
		# Fixup overflow:
		# - If x > inf_cutoff then inf_mask is true
		# - If x is NaN then inf_mask is false
		CMPLEPD( xmm_inf_mask, xmm_x )
		# If (inf_mask) rf = inf
		xmm_inf = SSERegister()
		MOVAPS( xmm_inf, Constant.float64x2(Exp.plus_inf) )
		ANDPS( xmm_inf, xmm_inf_mask )
		ANDNPS( xmm_inf_mask, xmm_rf[i] )
		ORPS( xmm_inf_mask, xmm_inf )
		xmm_rf[i] = xmm_inf_mask

		# Fixup underflow to zero:
		# - If x < zero_cutoff then zero_mask is true
		# - If x is NaN then zero_mask is false
		xmm_zero_cutoff = INIT.ONCE( SSERegister, Constant.float64x2(Exp.zero_cutoff), xmm_zero_cutoff )
		CMPLTPD( xmm_x, xmm_zero_cutoff )
		xmm_zero_mask = xmm_x
		# xmm_x = None
		# - If (zero_mask) rf = 0.0
		ANDNPD( xmm_zero_mask, xmm_rf[i] )
		xmm_rf[i] = xmm_zero_mask

		MOVUPS( [yPointer + i * 16], xmm_rf[i] )

def BATCH_EXP_FAST_K10(xPointer, yPointer, vectorExpFullLabel):
	rx = [SSERegister() for i in range(6)] + [LocalVariable(SSERegister) for i in range(2)]

	check_instructions = InstructionStream()
	with check_instructions:
		x_min_normal = GeneralPurposeRegister64()
		MOV( x_min_normal, Exp.preFMA.x_min )
		x_max_normal = GeneralPurposeRegister64()
		MOV( x_max_normal, Exp.preFMA.x_max - 1 )
		for i in range(8*2):
			x_temp = GeneralPurposeRegister64()
			MOV( x_temp, [xPointer + i * 8] )
			CMP( x_temp, x_min_normal )
			CMOVA( x_temp, x_max_normal )
			CMP( x_temp, x_max_normal )
			JGE( vectorExpFullLabel )

	xmm_log2e = xmm_magic_bias = xmm_default_exponent = xmm_minus_ln2_hi = xmm_minus_ln2_lo = None
	e = [LocalVariable(SSERegister) for i in range(8)]
	xmm_t = [SSERegister() for i in range(8)]
	for i in range(8):
		# x = *xPointer
		xmm_x = SSERegister()
		MOVAPD( xmm_x, [xPointer + i * 16] )
		# t = x * log2e + magic_bias
		MOVAPD( xmm_t[i], xmm_x )
		xmm_log2e = INIT.ONCE( SSERegister, Constant.float64x2(Exp.log2e), xmm_log2e )
		MULPD( xmm_t[i], xmm_log2e )
		xmm_magic_bias = INIT.ONCE( SSERegister, Constant.float64x2(Exp.magic_bias), xmm_magic_bias )
		ADDPD( xmm_t[i], xmm_magic_bias )
		check_instructions.issue(2)

	for i in range(8):
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		# e = as_uint(t) << 52
		xmm_e = SSERegister()
		MOVDQA( xmm_e, xmm_t[i] )
		PSLLQ( xmm_e, 52 )
		# s = as_double(e + default_exponent)
		PADDD( xmm_e, xmm_default_exponent )
		MOVDQA( e[i], xmm_e )
		# t = t - magic_bias
		SUBPD( xmm_t[i], xmm_magic_bias )
		check_instructions.issue()

	xmm_minus_ln2_hi = INIT.ONCE( SSERegister, Constant.float64x2(Exp.preFMA.minus_ln2_hi), xmm_minus_ln2_hi )
	xmm_rx = [SSERegister() for i in range(8)]
	for i in range(8):
		# rx = t * (-ln2.high) + x
		MOVAPS( xmm_rx[i], xmm_t[i] )
		MULPD( xmm_rx[i], xmm_minus_ln2_hi )
		ADDPD( xmm_rx[i], [xPointer + i * 16] )
		if i == 6:
			MOVAPS( rx[i], xmm_rx[i] )

	xmm_rf = [SSERegister() for i in range(8)]
	xmm_c10 = SSERegister()
	for i in range(8):
		if i == 4:
			xmm_rx[6] = SSERegister()
			MOVAPS( xmm_rx[6], rx[6] )
		# rx = t * (-ln2.low) + rx
		xmm_minus_ln2_lo = INIT.ONCE( SSERegister, Constant.float64x2(Exp.preFMA.minus_ln2_lo), xmm_minus_ln2_lo )
		MULPD( xmm_t[i], xmm_minus_ln2_lo )
		ADDPD( xmm_rx[i], xmm_t[i] )
		if isinstance( rx[i], LocalVariable ):
			MOVAPS( rx[i], xmm_rx[i] )
		else:
			rx[i] = xmm_rx[i]
			check_instructions.issue()

	MOVAPS( xmm_rf[7], Constant.float64x2(Exp.c11) )
	MOVAPS( xmm_c10, Constant.float64x2(Exp.c10) )
	check_instructions.issue()
	for i in range(8):
		if i != 7:
			MOVAPS( xmm_rf[i], xmm_rf[7] )
		else:
			check_instructions.issue()
		MULPD( xmm_rf[i], rx[i] )
		ADDPD( xmm_rf[i], xmm_c10 )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(8):
			if isinstance(rx[i], SSERegister):
				xmm_rx = rx[i]
				check_instructions.issue()
			else:
				xmm_rx = SSERegister()
				MOVAPS( xmm_rx, rx[i] )
			MULPD( xmm_rf[i], xmm_rx )
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Exp.c9))
	HORNER_STEP(Constant.float64x2(Exp.c8))
	HORNER_STEP(Constant.float64x2(Exp.c7))
	HORNER_STEP(Constant.float64x2(Exp.c6))
	HORNER_STEP(Constant.float64x2(Exp.c5))
	HORNER_STEP(Constant.float64x2(Exp.c4))
	HORNER_STEP(Constant.float64x2(Exp.c3))
	HORNER_STEP(Constant.float64x2(Exp.c2))
	
	xmm_rx = [SSERegister() for i in range(8)]
	for i in range(8):
		if isinstance(rx[i], SSERegister):
			xmm_rx[i] = rx[i]
		else:
			check_instructions.issue()
			MOVAPS( xmm_rx[i], rx[i] )
		# rf = rf * rx
		MULPD( xmm_rf[i], xmm_rx[i] )
		check_instructions.issue()

	for i in range(8):
		# rf = rf * rx + rx
		MULPD( xmm_rf[i], xmm_rx[i] )
		ADDPD( xmm_rf[i], xmm_rx[i] )
		check_instructions.issue()

	assert len(check_instructions) == 0

	for i in range(8):
		xmm_e = SSERegister()
		MOVAPS( xmm_e, e[i] )
		# rf = rf * s + s
		MULPD( xmm_rf[i], xmm_e )
		ADDPD( xmm_rf[i], xmm_e )
		MOVUPS( [yPointer + i * 16], xmm_rf[i] )

def BATCH_EXP_FULL_K10(xPointer, yPointer):
	rx = [SSERegister() for i in range(6)] + [LocalVariable(SSERegister) for i in range(2)]

	xmm_log2e = xmm_magic_bias = xmm_default_exponent = xmm_max_exponent = xmm_min_exponent = None
	e1 = [LocalVariable(SSERegister) for i in range(8)]
	e2 = [LocalVariable(SSERegister) for i in range(8)]
	for i in range(8):
		xmm_x = SSERegister()
		xmm_t = SSERegister()
		if isinstance(rx[i], SSERegister):
			xmm_rx = rx[i]
		else:
			xmm_rx = SSERegister()
		xmm_e1 = SSERegister()
		xmm_e2 = SSERegister()

		# x = *xPointer
		MOVAPD( xmm_x, [xPointer + i * 16] )
		# t = x * log2e + magic_bias
		MOVAPD( xmm_t, xmm_x )
		xmm_log2e = INIT.ONCE( SSERegister, Constant.float64x2(Exp.log2e), xmm_log2e )
		MULPD( xmm_t, xmm_log2e )
		xmm_magic_bias = INIT.ONCE( SSERegister, Constant.float64x2(Exp.magic_bias), xmm_magic_bias )
		ADDPD( xmm_t, xmm_magic_bias )
		# e2 = as_uint(t) << 52
		MOVDQA( xmm_e2, xmm_t )
		PSLLQ( xmm_e2, 52 )
		# t = t - magic_bias
		SUBPD( xmm_t, xmm_magic_bias )
		# rx = t * (-ln2.high) + x
		MOVAPS( xmm_rx, xmm_t )
		MULPD( xmm_rx, Constant.float64x2(Exp.preFMA.minus_ln2_hi) )
		ADDPD( xmm_rx, xmm_x )
		# rx = t * (-ln2.low) + rx
		MULPD( xmm_t, Constant.float64x2(Exp.preFMA.minus_ln2_lo) )
		ADDPD( xmm_rx, xmm_t )
		if isinstance( rx[i], LocalVariable ):
			MOVAPS( rx[i], xmm_rx )

		xmm_max_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.max_exponent), xmm_max_exponent )
		# e1 = min(e2, max_exponent)
		MOVAPD( xmm_e1, xmm_e2 )
		PMINSW( xmm_e1, xmm_max_exponent )
		xmm_min_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.min_exponent), xmm_min_exponent )
		# e1 = max(e1, min_exponent)
		PMAXSW( xmm_e1, xmm_min_exponent )
		# e2 -= e1
		PSUBD( xmm_e2, xmm_e1 )
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		# s1 = as_double(e1 + default_exponent)
		PADDD( xmm_e1, xmm_default_exponent)
		MOVDQA( e1[i], xmm_e1 )
		# s2 = as_double(e2 + default_exponent)
		PADDD( xmm_e2, xmm_default_exponent)
		MOVDQA( e2[i], xmm_e2 )

	xmm_rf = [SSERegister() for i in range(8)]

	for i in range(8):
		if i % 4 == 0:
			MOVAPS( xmm_rf[i], Constant.float64x2(Exp.c11) )
		else:
			MOVAPS( xmm_rf[i], xmm_rf[(i/4)*4] )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(8):
			MULPD( xmm_rf[i], rx[i] )
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Exp.c10))
	HORNER_STEP(Constant.float64x2(Exp.c9))
	HORNER_STEP(Constant.float64x2(Exp.c8))
	HORNER_STEP(Constant.float64x2(Exp.c7))
	HORNER_STEP(Constant.float64x2(Exp.c6))
	HORNER_STEP(Constant.float64x2(Exp.c5))
	HORNER_STEP(Constant.float64x2(Exp.c4))
	HORNER_STEP(Constant.float64x2(Exp.c3))
	HORNER_STEP(Constant.float64x2(Exp.c2))

	for i in range(8):
		if isinstance(rx[i], SSERegister):
			xmm_rx = rx[i]
		else:
			xmm_rx = SSERegister()
			MOVAPS( xmm_rx, rx[i] )
		# rf = rf * rx
		MULPD( xmm_rf[i], xmm_rx )
		# rf = rf * rx + rx
		MULPD( xmm_rf[i], xmm_rx )
		ADDPD( xmm_rf[i], xmm_rx )

	for i in range(8):
		xmm_e1 = SSERegister()
		MOVAPS( xmm_e1, e1[i] )
		# rf = rf * s1 + s1
		MULPD( xmm_rf[i], xmm_e1 )
		ADDPD( xmm_rf[i], xmm_e1 )

	for i in range(8):
		# rf = rf * s2
		MULPD( xmm_rf[i], e2[i] )

	xmm_inf_cutoff = xmm_zero_cutoff = None
	for i in range(8):
		xmm_x = SSERegister()
		MOVAPS( xmm_x, [xPointer + i * 16] )

		xmm_inf_cutoff = INIT.ONCE( SSERegister, Constant.float64x2(Exp.inf_cutoff), xmm_inf_cutoff )
		xmm_inf_mask = SSERegister()
		MOVAPS( xmm_inf_mask, xmm_inf_cutoff )
		# Fixup overflow:
		# - If x > inf_cutoff then inf_mask is true
		# - If x is NaN then inf_mask is false
		CMPLEPD( xmm_inf_mask, xmm_x )
		# If (inf_mask) rf = inf
		xmm_inf = SSERegister()
		MOVAPS( xmm_inf, Constant.float64x2(Exp.plus_inf) )
		ANDPS( xmm_inf, xmm_inf_mask )
		ANDNPS( xmm_inf_mask, xmm_rf[i] )
		ORPS( xmm_inf_mask, xmm_inf )
		xmm_rf[i] = xmm_inf_mask

		# Fixup underflow to zero:
		# - If x < zero_cutoff then zero_mask is true
		# - If x is NaN then zero_mask is false
		xmm_zero_cutoff = INIT.ONCE( SSERegister, Constant.float64x2(Exp.zero_cutoff), xmm_zero_cutoff )
		CMPLTPD( xmm_x, xmm_zero_cutoff )
		xmm_zero_mask = xmm_x
		# xmm_x = None
		# - If (zero_mask) rf = 0.0
		ANDNPD( xmm_zero_mask, xmm_rf[i] )
		xmm_rf[i] = xmm_zero_mask

		MOVUPS( [yPointer + i * 16], xmm_rf[i] )

def BATCH_EXP_FAST_Nehalem(xPointer, yPointer, vectorExpFullLabel):
	rx = [SSERegister() for i in range(6)] + [LocalVariable(SSERegister) for i in range(2)]

	check_instructions = InstructionStream()
	with check_instructions:
		x_min_normal = GeneralPurposeRegister64()
		MOV( x_min_normal, Exp.preFMA.x_min )
		x_max_normal = GeneralPurposeRegister64()
		MOV( x_max_normal, Exp.preFMA.x_max )
		for i in range(8*2):
			x_temp = GeneralPurposeRegister64()
			MOV( x_temp, [xPointer + i * 8] )
			CMP.JA( x_temp, x_min_normal, vectorExpFullLabel )
			CMP.JG( x_temp, x_max_normal, vectorExpFullLabel )

	xmm_log2e = xmm_magic_bias = xmm_default_exponent = xmm_minus_ln2_hi = xmm_minus_ln2_lo = None
	e = [LocalVariable(SSERegister) for i in range(8)]
	xmm_t = [SSERegister() for i in range(8)]
	for i in range(8):
		# x = *xPointer
		xmm_x = SSERegister()
		MOVAPD( xmm_x, [xPointer + i * 16] )
		# t = x * log2e + magic_bias
		MOVAPD( xmm_t[i], xmm_x )
		xmm_log2e = INIT.ONCE( SSERegister, Constant.float64x2(Exp.log2e), xmm_log2e )
		MULPD( xmm_t[i], xmm_log2e )
		xmm_magic_bias = INIT.ONCE( SSERegister, Constant.float64x2(Exp.magic_bias), xmm_magic_bias )
		ADDPD( xmm_t[i], xmm_magic_bias )

	for i in range(8):
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		# e = as_uint(t) << 52
		xmm_e = SSERegister()
		MOVDQA( xmm_e, xmm_t[i] )
		PSLLQ( xmm_e, 52 )
		# s = as_double(e + default_exponent)
		PADDD( xmm_e, xmm_default_exponent )
		MOVDQA( e[i], xmm_e )
		# t = t - magic_bias
		SUBPD( xmm_t[i], xmm_magic_bias )
		check_instructions.issue()

	xmm_minus_ln2_hi = INIT.ONCE( SSERegister, Constant.float64x2(Exp.preFMA.minus_ln2_hi), xmm_minus_ln2_hi )
	xmm_rx = [SSERegister() for i in range(8)]
	for i in range(8):
		# rx = t * (-ln2.high) + x
		MOVAPS( xmm_rx[i], xmm_t[i] )
		MULPD( xmm_rx[i], xmm_minus_ln2_hi )
		ADDPD( xmm_rx[i], [xPointer + i * 16] )
		if i == 6:
			MOVAPS( rx[i], xmm_rx[i] )

	xmm_rf = [SSERegister() for i in range(8)]
	xmm_c10 = SSERegister()
	for i in range(8):
		if i == 4:
			xmm_rx[6] = SSERegister()
			MOVAPS( xmm_rx[6], rx[6] )
		# rx = t * (-ln2.low) + rx
		xmm_minus_ln2_lo = INIT.ONCE( SSERegister, Constant.float64x2(Exp.preFMA.minus_ln2_lo), xmm_minus_ln2_lo )
		MULPD( xmm_t[i], xmm_minus_ln2_lo )
		ADDPD( xmm_rx[i], xmm_t[i] )
		if isinstance( rx[i], LocalVariable ):
			MOVAPS( rx[i], xmm_rx[i] )
		else:
			rx[i] = xmm_rx[i]
			check_instructions.issue()

	MOVAPS( xmm_rf[7], Constant.float64x2(Exp.c11) )
	MOVAPS( xmm_c10, Constant.float64x2(Exp.c10) )
	for i in range(8):
		if i != 7:
			MOVAPS( xmm_rf[i], xmm_rf[7] )
		else:
			check_instructions.issue()
		MULPD( xmm_rf[i], rx[i] )
		ADDPD( xmm_rf[i], xmm_c10 )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(8):
			MULPD( xmm_rf[i], rx[i] )
			ADDPD( xmm_rf[i], xmm_c )
			check_instructions.issue()

	HORNER_STEP(Constant.float64x2(Exp.c9))
	HORNER_STEP(Constant.float64x2(Exp.c8))
	HORNER_STEP(Constant.float64x2(Exp.c7))
	HORNER_STEP(Constant.float64x2(Exp.c6))
	HORNER_STEP(Constant.float64x2(Exp.c5))
	HORNER_STEP(Constant.float64x2(Exp.c4))
	HORNER_STEP(Constant.float64x2(Exp.c3))
	HORNER_STEP(Constant.float64x2(Exp.c2))
	
	assert len(check_instructions) == 0

	for i in range(8):
		if isinstance(rx[i], SSERegister):
			xmm_rx = rx[i]
		else:
			xmm_rx = SSERegister()
			MOVAPS( xmm_rx, rx[i] )
		# rf = rf * rx
		MULPD( xmm_rf[i], xmm_rx )
		# rf = rf * rx + rx
		MULPD( xmm_rf[i], xmm_rx )
		ADDPD( xmm_rf[i], xmm_rx )

	for i in range(8):
		xmm_e = SSERegister()
		MOVAPS( xmm_e, e[i] )
		# rf = rf * s + s
		MULPD( xmm_rf[i], xmm_e )
		ADDPD( xmm_rf[i], xmm_e )
		MOVUPS( [yPointer + i * 16], xmm_rf[i] )

def BATCH_EXP_FULL_Nehalem(xPointer, yPointer):
	rx = [SSERegister() for i in range(6)] + [LocalVariable(SSERegister) for i in range(2)]

	xmm_log2e = xmm_magic_bias = xmm_default_exponent = xmm_max_exponent = xmm_min_exponent = None
	e1 = [LocalVariable(SSERegister) for i in range(8)]
	e2 = [LocalVariable(SSERegister) for i in range(8)]
	for i in range(8):
		xmm_x = SSERegister()
		xmm_t = SSERegister()
		if isinstance(rx[i], SSERegister):
			xmm_rx = rx[i]
		else:
			xmm_rx = SSERegister()
		xmm_e1 = SSERegister()
		xmm_e2 = SSERegister()

		# x = *xPointer
		MOVAPD( xmm_x, [xPointer + i * 16] )
		# t = x * log2e + magic_bias
		MOVAPD( xmm_t, xmm_x )
		xmm_log2e = INIT.ONCE( SSERegister, Constant.float64x2(Exp.log2e), xmm_log2e )
		MULPD( xmm_t, xmm_log2e )
		xmm_magic_bias = INIT.ONCE( SSERegister, Constant.float64x2(Exp.magic_bias), xmm_magic_bias )
		ADDPD( xmm_t, xmm_magic_bias )
		# e2 = as_uint(t) << 52
		MOVDQA( xmm_e2, xmm_t )
		PSLLQ( xmm_e2, 52 )
		# t = t - magic_bias
		SUBPD( xmm_t, xmm_magic_bias )
		# rx = t * (-ln2.high) + x
		MOVAPS( xmm_rx, xmm_t )
		MULPD( xmm_rx, Constant.float64x2(Exp.preFMA.minus_ln2_hi) )
		ADDPD( xmm_rx, xmm_x )
		# rx = t * (-ln2.low) + rx
		MULPD( xmm_t, Constant.float64x2(Exp.preFMA.minus_ln2_lo) )
		ADDPD( xmm_rx, xmm_t )
		if isinstance( rx[i], LocalVariable ):
			MOVAPS( rx[i], xmm_rx )

		xmm_max_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.max_exponent), xmm_max_exponent )
		# e1 = min(e2, max_exponent)
		MOVAPD( xmm_e1, xmm_e2 )
		PMINSW( xmm_e1, xmm_max_exponent )
		xmm_min_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.min_exponent), xmm_min_exponent )
		# e1 = max(e1, min_exponent)
		PMAXSW( xmm_e1, xmm_min_exponent )
		# e2 -= e1
		PSUBD( xmm_e2, xmm_e1 )
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		# s1 = as_double(e1 + default_exponent)
		PADDD( xmm_e1, xmm_default_exponent)
		MOVDQA( e1[i], xmm_e1 )
		# s2 = as_double(e2 + default_exponent)
		PADDD( xmm_e2, xmm_default_exponent)
		MOVDQA( e2[i], xmm_e2 )

	xmm_rf = [SSERegister() for i in range(8)]

	for i in range(8):
		if i % 4 == 0:
			MOVAPS( xmm_rf[i], Constant.float64x2(Exp.c11) )
		else:
			MOVAPS( xmm_rf[i], xmm_rf[(i/4)*4] )

	def HORNER_STEP(coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(8):
			MULPD( xmm_rf[i], rx[i] )
			ADDPD( xmm_rf[i], xmm_c )

	HORNER_STEP(Constant.float64x2(Exp.c10))
	HORNER_STEP(Constant.float64x2(Exp.c9))
	HORNER_STEP(Constant.float64x2(Exp.c8))
	HORNER_STEP(Constant.float64x2(Exp.c7))
	HORNER_STEP(Constant.float64x2(Exp.c6))
	HORNER_STEP(Constant.float64x2(Exp.c5))
	HORNER_STEP(Constant.float64x2(Exp.c4))
	HORNER_STEP(Constant.float64x2(Exp.c3))
	HORNER_STEP(Constant.float64x2(Exp.c2))

	for i in range(8):
		if isinstance(rx[i], SSERegister):
			xmm_rx = rx[i]
		else:
			xmm_rx = SSERegister()
			MOVAPS( xmm_rx, rx[i] )
		# rf = rf * rx
		MULPD( xmm_rf[i], xmm_rx )
		# rf = rf * rx + rx
		MULPD( xmm_rf[i], xmm_rx )
		ADDPD( xmm_rf[i], xmm_rx )

	for i in range(8):
		xmm_e1 = SSERegister()
		MOVAPS( xmm_e1, e1[i] )
		# rf = rf * s1 + s1
		MULPD( xmm_rf[i], xmm_e1 )
		ADDPD( xmm_rf[i], xmm_e1 )

	for i in range(8):
		# rf = rf * s2
		MULPD( xmm_rf[i], e2[i] )

	xmm_inf = xmm_inf_cutoff = xmm_zero_cutoff = None
	for i in range(8):
		xmm_x = SSERegister()
		MOVAPS( xmm_x, [xPointer + i * 16] )

		xmm_inf_mask = xmm0
		xmm_inf_cutoff = INIT.ONCE( SSERegister, Constant.float64x2(Exp.inf_cutoff), xmm_inf_cutoff )
		MOVAPS( xmm_inf_mask, xmm_inf_cutoff )
		# Fixup overflow:
		# - If x > inf_cutoff then inf_mask is true
		# - If x is NaN then inf_mask is false
		CMPLEPD( xmm_inf_mask, xmm_x )
		# If (inf_mask) rf = inf
		xmm_inf = INIT.ONCE( SSERegister, Constant.float64x2(Exp.plus_inf), xmm_inf )
		BLENDVPD( xmm_rf[i], xmm_inf, xmm_inf_mask )

		# Fixup underflow to zero:
		# - If x < zero_cutoff then zero_mask is true
		# - If x is NaN then zero_mask is false
		xmm_zero_cutoff = INIT.ONCE( SSERegister, Constant.float64x2(Exp.zero_cutoff), xmm_zero_cutoff )
		CMPLTPD( xmm_x, xmm_zero_cutoff )
		xmm_zero_mask = xmm_x
		# - If (zero_mask) rf = 0.0
		ANDNPD( xmm_zero_mask, xmm_rf[i] )
		xmm_rf[i] = xmm_zero_mask

		MOVUPS( [yPointer + i * 16], xmm_rf[i] )

def BATCH_EXP_FAST_Bulldozer(xPointer, yPointer, vectorExpFullLabel):
	check_instructions = InstructionStream()
	with check_instructions:
		x_min_normal = GeneralPurposeRegister64()
		MOV( x_min_normal, Exp.FMA.x_min )
		x_max_normal = GeneralPurposeRegister64()
		MOV( x_max_normal, Exp.FMA.x_max )
		for i in range(5*4):
			x_temp = GeneralPurposeRegister64()
			MOV( x_temp, [xPointer + i * 8] )
			CMP.JA( x_temp, x_min_normal, vectorExpFullLabel )
			CMP.JG( x_temp, x_max_normal, vectorExpFullLabel )

	ymm_magic_bias = AVXRegister()
	LOAD.CONSTANT( ymm_magic_bias, Constant.float64x4(Exp.magic_bias) )
	ymm_t = [AVXRegister() for i in range(5)]
	ymm_log2e = None
	for i in range(5):
		# x = *xPointer
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, [xPointer + i * 32] )
		check_instructions.issue()
		# t = x * log2e + magic_bias
		ymm_log2e = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.log2e), ymm_log2e )
		VFMADDPD( ymm_t[i], ymm_x, ymm_log2e, ymm_magic_bias )
		check_instructions.issue()

	xmm_default_exponent = ymm_minus_ln2_hi = ymm_minus_ln2_lo = None
	ymm_rx = [AVXRegister() for i in range(5)]

	e = [LocalVariable(AVXRegister) for i in range(5)]
	xmm_e_lo = [SSERegister() for i in range(5)]
	xmm_e_hi = [SSERegister() for i in range(5)]
	for i in range(5):
		# e = as_uint(t) << 52
		VEXTRACTF128( xmm_e_hi[i], ymm_t[i], 1 )
		VPSLLQ( xmm_e_lo[i], ymm_t[i].get_oword(), 52 )
		check_instructions.issue()
		VPSLLQ( xmm_e_hi[i], xmm_e_hi[i], 52 )
		# t = t - magic_bias
		VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )
		check_instructions.issue()
	for i in range(5):
		# s = as_double(e + defaultExponent)
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		VPADDD( xmm_e_lo[i], xmm_e_lo[i], xmm_default_exponent)
		VPADDD( xmm_e_hi[i], xmm_e_hi[i], xmm_default_exponent )
		VMOVDQA( e[i].get_low(), xmm_e_lo[i] )
		VMOVDQA( e[i].get_high(), xmm_e_hi[i] )
		# rx = t * (-ln2.high) + x
		ymm_minus_ln2_hi = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.minus_ln2_hi), ymm_minus_ln2_hi )
		VFMADDPD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_hi, [xPointer + i * 32] )

	for i in range(5):
		# rx = t * (-ln2.low) + rx
		ymm_minus_ln2_lo = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.minus_ln2_lo), ymm_minus_ln2_lo )
		VFMADDPD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_lo, ymm_rx[i] )
		check_instructions.issue()

	ymm_rf = [AVXRegister() for i in range(5)]

	ymm_c11 = AVXRegister()
	ymm_c10 = AVXRegister()
	LOAD.CONSTANT( ymm_c11, Constant.float64x4(Exp.c11) )
	LOAD.CONSTANT( ymm_c10, Constant.float64x4(Exp.c10) )
	for i in range(5):
		VFMADDPD( ymm_rf[i], ymm_c11, ymm_rx[i], ymm_c10 )
		check_instructions.issue()

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(5):
			VFMADDPD( ymm_rf[i], ymm_rf[i], ymm_rx[i], ymm_c )
			check_instructions.issue()

	HORNER_STEP(Constant.float64x4(Exp.c9))
	HORNER_STEP(Constant.float64x4(Exp.c8))
	HORNER_STEP(Constant.float64x4(Exp.c7))
	HORNER_STEP(Constant.float64x4(Exp.c6))
	HORNER_STEP(Constant.float64x4(Exp.c5))
	HORNER_STEP(Constant.float64x4(Exp.c4))
	HORNER_STEP(Constant.float64x4(Exp.c3))
	HORNER_STEP(Constant.float64x4(Exp.c2))

	assert len(check_instructions) == 0

	for i in range(5):
		# rf = rf * rx
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_rx[i] )

	xmm_e_lo = [SSERegister() for i in range(5)]
	xmm_e_hi = [SSERegister() for i in range(5)]
	for i in range(5):
		# rf = rf * rx + rx
		VFMADDPD( ymm_rf[i], ymm_rf[i], ymm_rx[i], ymm_rx[i] )
		VMOVAPD( xmm_e_lo[i], e[i].get_low() )
		VMOVAPD( xmm_e_hi[i], e[i].get_high() )

	for i in range(5):
		xmm_rf_hi = SSERegister()
		VEXTRACTF128( xmm_rf_hi, ymm_rf[i], 1 )
		xmm_rf_lo = ymm_rf[i].get_oword()
		# rf = rf * s + s
		# *yPointer = rf
		VFMADDPD( xmm_rf_lo, xmm_rf_lo, xmm_e_lo[i], xmm_e_lo[i] )
		VMOVUPD( [yPointer + i * 32], xmm_rf_lo )

		VFMADDPD( xmm_rf_hi, xmm_rf_hi, xmm_e_hi[i], xmm_e_hi[i] )
		VMOVUPD( [yPointer + i * 32 + 16], xmm_rf_hi )

def BATCH_EXP_FULL_Bulldozer(xPointer, yPointer):
	ymm_magic_bias = AVXRegister()
	LOAD.CONSTANT( ymm_magic_bias, Constant.float64x4(Exp.magic_bias) )
	ymm_t = [AVXRegister() for i in range(5)]
	ymm_log2e = None
	for i in range(5):
		# x = *xPointer
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, [xPointer + i * 32] )
		# t = x * log2e + magic_bias
		ymm_log2e = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.log2e), ymm_log2e )
		VFMADDPD( ymm_t[i], ymm_x, ymm_log2e, ymm_magic_bias )

	xmm_max_exponent = xmm_min_exponent = xmm_default_exponent = None
	ymm_minus_ln2_hi = ymm_minus_ln2_lo = None
	ymm_rx = [AVXRegister() for i in range(5)]

	e1 = [LocalVariable(AVXRegister) for i in range(5)]
	e2 = [LocalVariable(AVXRegister) for i in range(5)]
	for i in range(5):
		# e2 = as_uint(t) << 52
		xmm_e2_hi = SSERegister()
		VEXTRACTF128( xmm_e2_hi, ymm_t[i], 1 )
		xmm_e2_lo = SSERegister()
		VPSLLQ( xmm_e2_lo, ymm_t[i].get_oword(), 52 )
		VPSLLQ( xmm_e2_hi, xmm_e2_hi, 52 )
		# t = t - magic_bias
		VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )
		# e1 = min(e2, max_exponent)
		xmm_max_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.max_exponent), xmm_max_exponent )
		xmm_e1_lo = SSERegister()
		VPMINSD( xmm_e1_lo, xmm_e2_lo, xmm_max_exponent )
		xmm_e1_hi = SSERegister()
		VPMINSD( xmm_e1_hi, xmm_e2_hi, xmm_max_exponent )
		xmm_min_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.min_exponent), xmm_min_exponent )
		# e1 = max(e1, min_exponent)
		VPMAXSD( xmm_e1_lo, xmm_e1_lo, xmm_min_exponent )
		VPMAXSD( xmm_e1_hi, xmm_e1_hi, xmm_min_exponent )
		# rx = t * (-ln2.high) + x
		ymm_minus_ln2_hi = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.minus_ln2_hi), ymm_minus_ln2_hi )
		VFMADDPD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_hi, [xPointer + i * 32] )
		# e2 -= e1
		VPSUBD( xmm_e2_lo, xmm_e2_lo, xmm_e1_lo )
		VPSUBD( xmm_e2_hi, xmm_e2_hi, xmm_e1_hi )
		# s1 = as_double(e1 + defaultExponent)
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		VPADDD( xmm_e1_lo, xmm_e1_lo, xmm_default_exponent)
		VPADDD( xmm_e1_hi, xmm_e1_hi, xmm_default_exponent )
		VMOVDQA( e1[i].get_low(), xmm_e1_lo )
		VMOVDQA( e1[i].get_high(), xmm_e1_hi )
		# rx = t * (-ln2.low) + rx
		ymm_minus_ln2_lo = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.minus_ln2_lo), ymm_minus_ln2_lo )
		VFMADDPD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_lo, ymm_rx[i] )
		# s2 = as_double(e2 + defaultExponent)
		VPADDD( xmm_e2_lo, xmm_e2_lo, xmm_default_exponent)
		VPADDD( xmm_e2_hi, xmm_e2_hi, xmm_default_exponent )
		VMOVDQA( e2[i].get_low(), xmm_e2_lo )
		VMOVDQA( e2[i].get_high(), xmm_e2_hi )

	ymm_rf = [AVXRegister() for i in range(5)]

	ymm_c11 = AVXRegister()
	ymm_c10 = AVXRegister()
	LOAD.CONSTANT( ymm_c11, Constant.float64x4(Exp.c11) )
	LOAD.CONSTANT( ymm_c10, Constant.float64x4(Exp.c10) )
	for i in range(5):
		VFMADDPD( ymm_rf[i], ymm_c11, ymm_rx[i], ymm_c10 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(5):
			VFMADDPD( ymm_rf[i], ymm_rf[i], ymm_rx[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Exp.c9))
	HORNER_STEP(Constant.float64x4(Exp.c8))
	HORNER_STEP(Constant.float64x4(Exp.c7))
	HORNER_STEP(Constant.float64x4(Exp.c6))
	HORNER_STEP(Constant.float64x4(Exp.c5))
	HORNER_STEP(Constant.float64x4(Exp.c4))
	HORNER_STEP(Constant.float64x4(Exp.c3))
	HORNER_STEP(Constant.float64x4(Exp.c2))

	for i in range(5):
		# rf = rf * rx
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_rx[i] )
	for i in range(5):
		# rf = rf * rx + rx
		VFMADDPD( ymm_rf[i], ymm_rf[i], ymm_rx[i], ymm_rx[i] )

	for i in range(5):
		ymm_e1 = AVXRegister()
		VMOVAPD( ymm_e1, e1[i] )
		# rf = rf * s1 + s1
		VFMADDPD( ymm_rf[i], ymm_rf[i], ymm_e1, ymm_e1 )

	for i in range(5):
		# rf = rf * s2
		VMULPD( ymm_rf[i], ymm_rf[i], e2[i] )

	xmm_inf = xmm_inf_cutoff = xmm_zero_cutoff = None
	for i in range(5):
		xmm_rf_hi = SSERegister()
		VEXTRACTF128( xmm_rf_hi, ymm_rf[i], 1 )
		xmm_rf_lo = ymm_rf[i].get_oword()

		xmm_x_lo = SSERegister()
		xmm_x_hi = SSERegister()
		VMOVAPD( xmm_x_lo, [xPointer + i * 32] )
		VMOVAPD( xmm_x_hi, [xPointer + i * 32 + 16] )
		# Fixup underflow to zero:
		# - If x < zero_cutoff then zero_mask is true
		# - If x is NaN then zero_mask is false
		xmm_zero_mask_lo = SSERegister()
		xmm_zero_mask_hi = SSERegister()
		xmm_zero_cutoff = INIT.ONCE( SSERegister, Constant.float64x2(Exp.zero_cutoff), xmm_zero_cutoff )
		VCMPLTPD( xmm_zero_mask_lo, xmm_x_lo, xmm_zero_cutoff )
		VCMPLTPD( xmm_zero_mask_hi, xmm_x_hi, xmm_zero_cutoff )
		# - If (zero_mask) rf = 0.0
		xmm_inf_cutoff = INIT.ONCE( SSERegister, Constant.float64x2(Exp.inf_cutoff), xmm_inf_cutoff )
		VANDNPD( xmm_rf_lo, xmm_zero_mask_lo, xmm_rf_lo )
		VANDNPD( xmm_rf_hi, xmm_zero_mask_hi, xmm_rf_hi )
		# Fixup overflow:
		# - If x > inf_cutoff then inf_mask is true
		# - If x is NaN then inf_mask is false
		xmm_inf = INIT.ONCE( SSERegister, Constant.float64x2(Exp.plus_inf), xmm_inf )
		xmm_inf_mask_lo = SSERegister()
		VCMPGTPD( xmm_inf_mask_lo, xmm_x_lo, xmm_inf_cutoff )
		xmm_inf_mask_hi = SSERegister()
		VCMPGTPD( xmm_inf_mask_hi, xmm_x_hi, xmm_inf_cutoff )
		# If (inf_mask) rf = inf
		VBLENDVPD( xmm_rf_lo, xmm_rf_lo, xmm_inf, xmm_inf_mask_lo )
		VBLENDVPD( xmm_rf_hi, xmm_rf_hi, xmm_inf, xmm_inf_mask_hi )
		# *yPointer = rf
		VMOVUPD( [yPointer + i * 32], xmm_rf_lo )
		VMOVUPD( [yPointer + i * 32 + 16], xmm_rf_hi )

def BATCH_EXP_FAST_SandyBridge(xPointer, yPointer, vectorExpFullLabel):
	ymm_log2e = ymm_magic_bias = ymm_min_normal = ymm_max_normal = None 
	ymm_t = [AVXRegister() for i in range(8)]
	ymm_x = [AVXRegister() for i in range(8)]
	for i in range(8):
		# x = *xPointer
		VMOVAPD( ymm_x[i], [xPointer + i * 32] )
		ymm_min_normal = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.preFMA.min_normal), ymm_min_normal )
		ymm_below_normal_mask = AVXRegister()
		VCMPLTPD( ymm_below_normal_mask, ymm_x[i], ymm_min_normal )
		ymm_max_normal = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.preFMA.max_normal), ymm_max_normal )
		ymm_above_normal_mask = AVXRegister()
		VCMPGTPD( ymm_above_normal_mask, ymm_x[i], ymm_max_normal )
		ymm_special_mask = AVXRegister()
		VORPD( ymm_special_mask, ymm_below_normal_mask, ymm_above_normal_mask )
		VTESTPD( ymm_special_mask, ymm_special_mask )
		JNZ( vectorExpFullLabel )
		# t = x * log2e + magic_bias
		ymm_log2e = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.log2e), ymm_log2e )
		VMULPD( ymm_t[i], ymm_x[i], ymm_log2e )
		ymm_magic_bias = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.magic_bias), ymm_magic_bias )
		VADDPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

	e = [LocalVariable(AVXRegister) for i in range(8)]
	xmm_default_exponent = None 
	for i in range(8):
		# e = as_uint(t) << 52
		xmm_e_hi = SSERegister()
		VEXTRACTF128( xmm_e_hi, ymm_t[i], 1 )
		xmm_e_lo = SSERegister()
		VPSLLQ( xmm_e_lo, ymm_t[i].get_oword(), 52 )
		VPSLLQ( xmm_e_hi, xmm_e_hi, 52 )

		# t = t - magic_bias
		VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

		# s = as_double(e + defaultExponent)
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		VPADDD( xmm_e_lo, xmm_e_lo, xmm_default_exponent)
		VPADDD( xmm_e_hi, xmm_e_hi, xmm_default_exponent )
		VMOVDQA( e[i].get_low(), xmm_e_lo )
		VMOVDQA( e[i].get_high(), xmm_e_hi )

	rx = ([AVXRegister() for i in range(6)] + [LocalVariable(AVXRegister) for i in range(2)])
	rx = [rx[i] for i in [3, 1, 5, 4, 7, 0, 6, 2]]

	ymm_minus_ln2_hi = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.preFMA.minus_ln2_hi) )
	ymm_rx = [AVXRegister() for i in range(8)]
	for i in range(8):
		# rx = t * (-ln2.high) + x
		VMULPD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_hi )
		VADDPD( ymm_rx[i], ymm_rx[i], [xPointer + i * 32] )

	minus_ln2_lo = Constant.float64x4(Exp.preFMA.minus_ln2_lo)
	for i in range(8):
		# rx = t * (-ln2.low) + rx
		ymm_temp = AVXRegister()
		if i != 0:
			minus_ln2_lo = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.preFMA.minus_ln2_lo), minus_ln2_lo )
		VMULPD( ymm_temp, ymm_t[i], minus_ln2_lo )
		VADDPD( ymm_rx[i], ymm_rx[i], ymm_temp )
		if isinstance(rx[i], LocalVariable):
			VMOVAPD( rx[i], ymm_rx[i] )
		else:
			rx[i] = ymm_rx[i]

	ymm_rf = [AVXRegister() for i in range(8)]

	ymm_c11 = AVXRegister()
	ymm_c10 = AVXRegister()
	LOAD.CONSTANT( ymm_c11, Constant.float64x4(Exp.c11) )
	LOAD.CONSTANT( ymm_c10, Constant.float64x4(Exp.c10) )
	for i in range(8):
		VMULPD( ymm_rf[i], ymm_c11, rx[i] )
		VADDPD( ymm_rf[i], ymm_rf[i], ymm_c10 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(8):
			VMULPD( ymm_rf[i], ymm_rf[i], rx[i] )
			VADDPD( ymm_rf[i], ymm_rf[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Exp.c9))
	HORNER_STEP(Constant.float64x4(Exp.c8))
	HORNER_STEP(Constant.float64x4(Exp.c7))
	HORNER_STEP(Constant.float64x4(Exp.c6))
	HORNER_STEP(Constant.float64x4(Exp.c5))
	HORNER_STEP(Constant.float64x4(Exp.c4))
	HORNER_STEP(Constant.float64x4(Exp.c3))
	HORNER_STEP(Constant.float64x4(Exp.c2))

	ymm_rx[i] = [None] * 8
	for i in range(8):
		if isinstance(rx[i], AVXRegister):
			ymm_rx[i] = rx[i]
		else:
			ymm_rx[i] = AVXRegister()
			VMOVAPD( ymm_rx[i], rx[i] )
		# rf = rf * rx
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_rx[i] )

	ymm_e = [AVXRegister() for i in range(8)]
	for i in range(8):
		# rf = rf * rx + rx
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_rx[i] )
		VADDPD( ymm_rf[i], ymm_rf[i], ymm_rx[i] )
		VMOVAPD( ymm_e[i], e[i] )

	for i in range(8):
		# rf = rf * s1 + s1
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_e[i] )
		VADDPD( ymm_rf[i], ymm_rf[i], ymm_e[i] )

		# *yPointer = rf
		VMOVUPD( [yPointer + i * 32], ymm_rf[i].get_oword() )
		VEXTRACTF128( [yPointer + i * 32 + 16], ymm_rf[i], 1 )

def BATCH_EXP_FULL_SandyBridge(xPointer, yPointer):
	ymm_log2e = ymm_magic_bias = None 
	ymm_t = [AVXRegister() for i in range(8)]
	for i in range(8):
		ymm_x = AVXRegister()
		# x = *xPointer
		VMOVAPD( ymm_x, [xPointer + i * 32] )
		# t = x * log2e + magic_bias
		ymm_log2e = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.log2e), ymm_log2e )
		VMULPD( ymm_t[i], ymm_x, ymm_log2e )
		ymm_magic_bias = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.magic_bias), ymm_magic_bias )
		VADDPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

	e1 = [LocalVariable(AVXRegister) for i in range(8)]
	e2 = [LocalVariable(AVXRegister) for i in range(8)]
	xmm_max_exponent = xmm_min_exponent = xmm_default_exponent = None 
	for i in range(8):
		xmm_e1_lo = SSERegister()
		xmm_e1_hi = SSERegister()
		xmm_e2_lo = SSERegister()
		xmm_e2_hi = SSERegister()

		# e2 = as_uint(t) << 52
		VEXTRACTF128( xmm_e2_hi, ymm_t[i], 1 )
		VPSLLQ( xmm_e2_lo, ymm_t[i].get_oword(), 52 )
		VPSLLQ( xmm_e2_hi, xmm_e2_hi, 52 )

		# t = t - magic_bias
		VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

		# e1 = min(e2, max_exponent)
		xmm_max_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.max_exponent), xmm_max_exponent )
		VPMINSW( xmm_e1_lo, xmm_e2_lo, xmm_max_exponent )
		VPMINSW( xmm_e1_hi, xmm_e2_hi, xmm_max_exponent )
		# e1 = max(e1, min_exponent)
		xmm_min_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.min_exponent), xmm_min_exponent )
		VPMAXSW( xmm_e1_lo, xmm_e1_lo, xmm_min_exponent )
		VPMAXSW( xmm_e1_hi, xmm_e1_hi, xmm_min_exponent )
		# e2 -= e1
		VPSUBD( xmm_e2_lo, xmm_e2_lo, xmm_e1_lo )
		VPSUBD( xmm_e2_hi, xmm_e2_hi, xmm_e1_hi )
		# s1 = as_double(e1 + defaultExponent)
		xmm_default_exponent = INIT.ONCE( SSERegister, Constant.uint64x2(Exp.default_exponent), xmm_default_exponent )
		VPADDD( xmm_e1_lo, xmm_e1_lo, xmm_default_exponent)
		VPADDD( xmm_e1_hi, xmm_e1_hi, xmm_default_exponent )
		VMOVDQA( e1[i].get_low(), xmm_e1_lo )
		VMOVDQA( e1[i].get_high(), xmm_e1_hi )
		# s2 = as_double(e2 + defaultExponent)
		VPADDD( xmm_e2_lo, xmm_e2_lo, xmm_default_exponent)
		VPADDD( xmm_e2_hi, xmm_e2_hi, xmm_default_exponent )
		VMOVDQA( e2[i].get_low(), xmm_e2_lo )
		VMOVDQA( e2[i].get_high(), xmm_e2_hi )

	rx = ([AVXRegister() for i in range(6)] + [LocalVariable(AVXRegister) for i in range(2)])
	rx = [rx[i] for i in [3, 1, 5, 4, 7, 0, 6, 2]]

	ymm_minus_ln2_hi = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.preFMA.minus_ln2_hi) )
	ymm_rx = [AVXRegister() for i in range(8)]
	for i in range(8):
		# rx = t * (-ln2.high) + x
		VMULPD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_hi )
		VADDPD( ymm_rx[i], ymm_rx[i], [xPointer + i * 32] )

	minus_ln2_lo = Constant.float64x4(Exp.preFMA.minus_ln2_lo)
	for i in range(8):
		# rx = t * (-ln2.low) + rx
		ymm_temp = AVXRegister()
		if i != 0:
			minus_ln2_lo = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.preFMA.minus_ln2_lo), minus_ln2_lo )
		VMULPD( ymm_temp, ymm_t[i], minus_ln2_lo )
		VADDPD( ymm_rx[i], ymm_rx[i], ymm_temp )
		if isinstance(rx[i], LocalVariable):
			VMOVAPD( rx[i], ymm_rx[i] )
		else:
			rx[i] = ymm_rx[i]

	ymm_rf = [AVXRegister() for i in range(8)]

	ymm_c11 = AVXRegister()
	ymm_c10 = AVXRegister()
	LOAD.CONSTANT( ymm_c11, Constant.float64x4(Exp.c11) )
	LOAD.CONSTANT( ymm_c10, Constant.float64x4(Exp.c10) )
	for i in range(8):
		VMULPD( ymm_rf[i], ymm_c11, rx[i] )
		VADDPD( ymm_rf[i], ymm_rf[i], ymm_c10 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(8):
			VMULPD( ymm_rf[i], ymm_rf[i], rx[i] )
			VADDPD( ymm_rf[i], ymm_rf[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Exp.c9))
	HORNER_STEP(Constant.float64x4(Exp.c8))
	HORNER_STEP(Constant.float64x4(Exp.c7))
	HORNER_STEP(Constant.float64x4(Exp.c6))
	HORNER_STEP(Constant.float64x4(Exp.c5))
	HORNER_STEP(Constant.float64x4(Exp.c4))
	HORNER_STEP(Constant.float64x4(Exp.c3))
	HORNER_STEP(Constant.float64x4(Exp.c2))

	ymm_rx[i] = [None] * 8
	for i in range(8):
		if isinstance(rx[i], AVXRegister):
			ymm_rx[i] = rx[i]
		else:
			ymm_rx[i] = AVXRegister()
			VMOVAPD( ymm_rx[i], rx[i] )
		# rf = rf * rx
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_rx[i] )

	ymm_e1 = [AVXRegister() for i in range(8)]
	for i in range(8):
		# rf = rf * rx + rx
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_rx[i] )
		VADDPD( ymm_rf[i], ymm_rf[i], ymm_rx[i] )
		VMOVAPD( ymm_e1[i], e1[i] )

	ymm_e2 = [AVXRegister() for i in range(8)]
	for i in range(8):
		# rf = rf * s1 + s1
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_e1[i] )
		VADDPD( ymm_rf[i], ymm_rf[i], ymm_e1[i] )
		VMOVAPD( ymm_e2[i], e2[i] )

	ymm_x = [AVXRegister() for i in range(8)]
	ymm_zero_cutoff = None
	for i in range(8):
		# rf = rf * s2
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_e2[i] )

		VMOVAPD( ymm_x[i], [xPointer + i * 32] )
		# Fixup underflow to zero:
		# - If x < zero_cutoff then zero_mask is true
		# - If x is NaN then zero_mask is false
		ymm_zero_mask = AVXRegister()
		if i != 0:
			ymm_zero_cutoff = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.zero_cutoff), ymm_zero_cutoff )
			zero_cutoff = ymm_zero_cutoff
		else:
			zero_cutoff = Constant.float64x4(Exp.zero_cutoff)
		VCMPLTPD( ymm_zero_mask, ymm_x[i], zero_cutoff )
		# - If (zero_mask) rf = 0.0
		VANDNPD( ymm_rf[i], ymm_zero_mask, ymm_rf[i] )

	ymm_inf = ymm_inf_cutoff = ymm_zero_cutoff = None
	for i in range(8):
		if i < 3:
			ymm_x[i] = AVXRegister()
			VMOVAPD( ymm_x[i], [xPointer + i * 32] )

		# Fixup overflow:
		# - If x > inf_cutoff then inf_mask is true
		# - If x is NaN then inf_mask is false
		ymm_inf_mask = AVXRegister()
		ymm_inf_cutoff = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.inf_cutoff), ymm_inf_cutoff )
		VCMPGTPD( ymm_inf_mask, ymm_x[i], ymm_inf_cutoff )
		# If (inf_mask) rf = inf
		ymm_inf = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.plus_inf), ymm_inf )
		VBLENDVPD( ymm_rf[i], ymm_rf[i], ymm_inf, ymm_inf_mask )
		# *yPointer = rf
		VMOVUPD( [yPointer + i * 32], ymm_rf[i].get_oword() )
		VEXTRACTF128( [yPointer + i * 32 + 16], ymm_rf[i], 1 )

def BATCH_EXP_FAST_Haswell(xPointer, yPointer, vectorExpFullLabel):
	ymm_log2e = ymm_magic_bias = ymm_min_normal = ymm_max_normal = None 
	ymm_t = [AVXRegister() for _ in range(10)]
	ymm_x = [AVXRegister() for _ in range(10)]
	for i in range(10):
		# x = *xPointer
		VMOVAPD( ymm_x[i], [xPointer + i * 32] )
		
		ymm_min_normal = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.min_normal), ymm_min_normal )
		ymm_below_normal_mask = AVXRegister()
		VCMPLTPD( ymm_below_normal_mask, ymm_x[i], ymm_min_normal )
		
		ymm_max_normal = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.max_normal), ymm_max_normal )
		ymm_above_normal_mask = AVXRegister()
		VCMPGTPD( ymm_above_normal_mask, ymm_x[i], ymm_max_normal )
		
		ymm_special_mask = AVXRegister()
		VORPD( ymm_special_mask, ymm_below_normal_mask, ymm_above_normal_mask )

		VTESTPD( ymm_special_mask, ymm_special_mask )
		JNZ( vectorExpFullLabel )
		# t = x * log2e + magic_bias
		ymm_log2e = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.log2e), ymm_log2e )
		ymm_magic_bias = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.magic_bias), ymm_magic_bias )
		SWAP.REGISTERS( ymm_t[i], ymm_x[i] )
		VFMADD132PD( ymm_t[i], ymm_t[i], ymm_log2e, ymm_magic_bias)

	e = [LocalVariable(AVXRegister) for i in range(10)]
	ymm_default_exponent = None 
	for i in range(10):
		# e = as_uint(t) << 52
		ymm_e = AVXRegister()
		VPSLLQ( ymm_e, ymm_t[i], 52 )

		# t = t - magic_bias
		VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

		# s = as_double(e + defaultExponent)
		ymm_default_exponent = INIT.ONCE( AVXRegister, Constant.uint64x4(Exp.default_exponent), ymm_default_exponent )
		VPADDD( ymm_e, ymm_e, ymm_default_exponent)
		VMOVDQA( e[i], ymm_e )

	rx = [AVXRegister() if i % 2 == 0 else LocalVariable(AVXRegister) for i in range(10)]

	ymm_rx = [AVXRegister() for i in range(10)]
	ymm_minus_ln2_lo = ymm_minus_ln2_hi = None
	for i in range(10):
		# rx = t * (-ln2.high) + x
		VMOVAPD( ymm_rx[i], [xPointer + i * 32] )
		ymm_minus_ln2_hi = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.minus_ln2_hi), ymm_minus_ln2_hi )
		VFMADD231PD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_hi, ymm_rx[i] )

		ymm_minus_ln2_lo = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.minus_ln2_lo), ymm_minus_ln2_lo )
		VFMADD231PD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_lo, ymm_rx[i] )
		if isinstance(rx[i], LocalVariable):
			VMOVAPD( rx[i], ymm_rx[i] )
		else:
			rx[i] = ymm_rx[i]

	ymm_rf = [AVXRegister() for i in range(10)]

	ymm_c10 = AVXRegister()
	LOAD.CONSTANT( ymm_rf[9], Constant.float64x4(Exp.c11) )
	LOAD.CONSTANT( ymm_c10, Constant.float64x4(Exp.c10) )
	for i in range(10):
		VMOVAPD( ymm_rf[i], ymm_rf[9] )
		if i != 9:
			VFMADD132PD( ymm_rf[i], ymm_rf[i], ymm_rx[i], ymm_c10 )
		else:
			VFMADD132PD( ymm_rf[i], ymm_rf[i], rx[i], ymm_c10 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(10):
			VFMADD132PD( ymm_rf[i], ymm_rf[i], rx[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Exp.c9))
	HORNER_STEP(Constant.float64x4(Exp.c8))
	HORNER_STEP(Constant.float64x4(Exp.c7))
	HORNER_STEP(Constant.float64x4(Exp.c6))
	HORNER_STEP(Constant.float64x4(Exp.c5))
	HORNER_STEP(Constant.float64x4(Exp.c4))
	HORNER_STEP(Constant.float64x4(Exp.c3))
	HORNER_STEP(Constant.float64x4(Exp.c2))

	for i in range(10):
		if isinstance(rx[i], AVXRegister):
			ymm_rx = rx[i]
		else:
			ymm_rx = AVXRegister()
			VMOVAPD( ymm_rx, rx[i] )
		# rf = rf * rx
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_rx )
		# rf = rf * rx + rx
		VFMADD132PD( ymm_rf[i], ymm_rf[i], ymm_rx, ymm_rx )


	for i in range(10):
		# rf = rf * s1 + s1
		ymm_e = AVXRegister()
		VMOVAPD( ymm_e, e[i] )
		VFMADD132PD( ymm_rf[i], ymm_rf[i], ymm_e, ymm_e )

		# *yPointer = rf
		VMOVUPD( [yPointer + i * 32], ymm_rf[i] )

def BATCH_EXP_FULL_Haswell(xPointer, yPointer):
	ymm_log2e = ymm_magic_bias = None 
	ymm_t = [AVXRegister() for i in range(10)]
	for i in range(10):
		# x = *xPointer
		VMOVAPD( ymm_t[i], [xPointer + i * 32] )
		# t = x * log2e + magic_bias
		ymm_log2e = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.log2e), ymm_log2e )
		ymm_magic_bias = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.magic_bias), ymm_magic_bias )
		VFMADD213PD( ymm_t[i], ymm_t[i], ymm_log2e, ymm_magic_bias )

	e1 = [LocalVariable(AVXRegister) for i in range(10)]
	e2 = [LocalVariable(AVXRegister) for i in range(10)]
	ymm_default_exponent = ymm_min_exponent = ymm_max_exponent = None 
	for i in range(10):
		# e2 = as_uint(t) << 52
		ymm_e2 = AVXRegister()
		VPSLLQ( ymm_e2, ymm_t[i], 52 )

		# t = t - magic_bias
		VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

		ymm_e1 = AVXRegister()
		# e1 = min(e2, max_exponent)
		ymm_max_exponent = INIT.ONCE( AVXRegister, Constant.uint64x4(Exp.max_exponent), ymm_max_exponent )
		VPMINSW( ymm_e1, ymm_e2, ymm_max_exponent )
		# e1 = max(e1, min_exponent)
		ymm_min_exponent = INIT.ONCE( AVXRegister, Constant.uint64x4(Exp.min_exponent), ymm_min_exponent )
		VPMAXSW( ymm_e1, ymm_e1, ymm_min_exponent )
		# e2 -= e1
		VPSUBD( ymm_e2, ymm_e2, ymm_e1 )
		# s1 = as_double(e1 + defaultExponent)
		ymm_default_exponent = INIT.ONCE( AVXRegister, Constant.uint64x4(Exp.default_exponent), ymm_default_exponent )
		VPADDD( ymm_e1, ymm_e1, ymm_default_exponent)
		VMOVDQA( e1[i], ymm_e1 )
		# s2 = as_double(e2 + defaultExponent)
		VPADDD( ymm_e2, ymm_e2, ymm_default_exponent)
		VMOVDQA( e2[i], ymm_e2 )

	rx = ([AVXRegister() if i % 2 == 0 else LocalVariable(AVXRegister) for i in range(10)])

	ymm_rx = [AVXRegister() for i in range(10)]
	ymm_minus_ln2_hi = ymm_minus_ln2_lo = None
	for i in range(10):
		# rx = t * (-ln2.high) + x
		VMOVAPD( ymm_rx[i], [xPointer + i * 32] )
		ymm_minus_ln2_hi = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.minus_ln2_hi), ymm_minus_ln2_hi )
		VFMADD231PD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_hi, ymm_rx[i] )
		# rx = t * (-ln2.low) + rx
		ymm_minus_ln2_lo = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.FMA.minus_ln2_lo), ymm_minus_ln2_lo )
		VFMADD231PD( ymm_rx[i], ymm_t[i], ymm_minus_ln2_lo, ymm_rx[i] )
		if isinstance(rx[i], LocalVariable):
			VMOVAPD( rx[i], ymm_rx[i] )
		else:
			rx[i] = ymm_rx[i]

	ymm_rf = [AVXRegister() for i in range(10)]
	LOAD.CONSTANT( ymm_rf[9], Constant.float64x4(Exp.c11) )

	ymm_c10 = AVXRegister()
	LOAD.CONSTANT( ymm_c10, Constant.float64x4(Exp.c10) )
	for i in range(10):
		VMOVAPD( ymm_rf[i], ymm_rf[9] )
		VFMADD132PD( ymm_rf[i], ymm_rf[i], rx[i], ymm_c10 )

	def HORNER_STEP(coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(10):
			VFMADD132PD( ymm_rf[i], ymm_rf[i], rx[i], ymm_c )

	HORNER_STEP(Constant.float64x4(Exp.c9))
	HORNER_STEP(Constant.float64x4(Exp.c8))
	HORNER_STEP(Constant.float64x4(Exp.c7))
	HORNER_STEP(Constant.float64x4(Exp.c6))
	HORNER_STEP(Constant.float64x4(Exp.c5))
	HORNER_STEP(Constant.float64x4(Exp.c4))
	HORNER_STEP(Constant.float64x4(Exp.c3))
	HORNER_STEP(Constant.float64x4(Exp.c2))

	for i in range(10):
		if isinstance(rx[i], AVXRegister):
			ymm_rx = rx[i]
		else:
			ymm_rx = AVXRegister()
			VMOVAPD( ymm_rx, rx[i] )
		# rf = rf * rx
		VMULPD( ymm_rf[i], ymm_rf[i], ymm_rx )
		# rf = rf * rx + rx
		VFMADD132PD( ymm_rf[i], ymm_rf[i], ymm_rx, ymm_rx )

	for i in range(10):
		# rf = rf * s1 + s1
		ymm_e1 = AVXRegister()
		VMOVAPD( ymm_e1, e1[i] )
		VFMADD132PD( ymm_rf[i], ymm_rf[i], ymm_e1, ymm_e1 )

	for i in range(10):
		# rf = rf * s2
		VMULPD( ymm_rf[i], ymm_rf[i], e2[i] )

	for i in range(10):
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, [xPointer + i * 32] )
		# Fixup underflow to zero:
		# - If x < zero_cutoff then zero_mask is true
		# - If x is NaN then zero_mask is false
		ymm_zero_mask = AVXRegister()
		VCMPLTPD( ymm_zero_mask, ymm_x, Constant.float64x4(Exp.zero_cutoff) )
		# - If (zero_mask) rf = 0.0
		VANDNPD( ymm_rf[i], ymm_zero_mask, ymm_rf[i] )

		# Fixup overflow:
		# - If x > inf_cutoff then inf_mask is true
		# - If x is NaN then inf_mask is false
		ymm_inf_mask = AVXRegister()
		ymm_inf_cutoff = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.inf_cutoff) )
		VCMPGTPD( ymm_inf_mask, ymm_x, ymm_inf_cutoff )
		# If (inf_mask) rf = inf
		ymm_inf = INIT.ONCE( AVXRegister, Constant.float64x4(Exp.plus_inf) )
		VBLENDVPD( ymm_rf[i], ymm_rf[i], ymm_inf, ymm_inf_mask )
		# *yPointer = rf
		VMOVUPD( [yPointer + i * 32], ymm_rf[i] )

def Exp_V64f_V64f(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-sysv', 'x64-ms']:
		if module == 'Math':
			if function == 'Exp':
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not x_type.is_floating_point() or not y_type.is_floating_point():
					return

				if x_type.get_size(codegen.abi) != 8 or y_type.get_size(codegen.abi) != 8:
					return

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bobcat', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_EXP_SSE, BATCH_EXP_FULL_Bobcat, BATCH_EXP_FAST_Bobcat, xPointer, yPointer, length, 16, 16 * 4, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'K10', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_EXP_SSE, BATCH_EXP_FULL_K10, BATCH_EXP_FAST_K10, xPointer, yPointer, length, 16, 16 * 8, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_EXP_SSE, BATCH_EXP_FULL_Nehalem, BATCH_EXP_FAST_Nehalem, xPointer, yPointer, length, 16, 16 * 8, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_EXP_AVX, BATCH_EXP_FULL_SandyBridge, BATCH_EXP_FAST_SandyBridge, xPointer, yPointer, length, 32, 32 * 8, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_EXP_AVX, BATCH_EXP_FULL_Bulldozer, BATCH_EXP_FAST_Bulldozer, xPointer, yPointer, length, 32, 32 * 5, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_EXP_AVX, BATCH_EXP_FULL_Haswell, BATCH_EXP_FAST_Haswell, xPointer, yPointer, length, 32, 32 * 10, 8)

def SCALAR_SIN_SSE(xPointer, yPointer, is_prologue):
	if Target.get_int_eu_width() == 64:
		def SCALAR_COPY(destination, source):
			ASSUME.INITIALIZED( destination )
			MOVSD( destination, source )
	else:
		def SCALAR_COPY(destination, source):
			MOVAPS( destination, source )

	# x = *xPointer
	xmm_x = SSERegister()
	MOVSD( xmm_x, [xPointer] )
	# t = (x * (2/Pi)) + magic_bias
	xmm_t = SSERegister()
	SCALAR_COPY( xmm_t, xmm_x )
	MULSD( xmm_t, Constant.float64(TrigReduction.two_over_pi) )
	ADDSD( xmm_t, Constant.float64(Sin.magic_bias) )
	# n = (int(t) % 4) << 62
	xmm_n = xmm0
	SCALAR_COPY( xmm_n, xmm_t )
	PSLLQ( xmm_n, 62 )
	# t -= magic_bias
	SUBSD( xmm_t, Constant.float64(Sin.magic_bias) )
	# x += t * (-pi/2)_hi
	xmm_temp = SSERegister()
	LOAD.CONSTANT( xmm_temp, Constant.float64(TrigReduction.minus_pi_o2_hi) )
	MULSD( xmm_temp, xmm_t )
	ADDSD( xmm_x, xmm_temp )
	# x += t * (-pi/2)_me
	xmm_a = SSERegister()
	SCALAR_COPY( xmm_a, xmm_x )
	xmm_temp = SSERegister()
	LOAD.CONSTANT( xmm_temp, Constant.float64(TrigReduction.minus_pi_o2_me) )
	MULSD( xmm_temp, xmm_t )
	ADDSD( xmm_x, xmm_temp )
	xmm_z = SSERegister()
	SCALAR_COPY( xmm_z, xmm_x )
	SUBSD( xmm_z, xmm_a )
	SUBSD( xmm_temp, xmm_z )
	# x += t * (-pi/2)_lo
	MULSD( xmm_t, Constant.float64(TrigReduction.minus_pi_o2_lo) )
	ADDSD( xmm_t, xmm_temp )
	ADDSD( xmm_x, xmm_t )

	# sqrx = x * x
	xmm_sqrx = SSERegister()
	SCALAR_COPY( xmm_sqrx, xmm_x )
	MULSD( xmm_sqrx, xmm_sqrx )

	xmm_sin = SSERegister()
	xmm_cos = SSERegister()
	LOAD.CONSTANT( xmm_cos, Constant.float64(Cos.c14) )
	LOAD.CONSTANT( xmm_sin, Constant.float64(Sin.c13) )

	def HORNER_STEP(acc, coef):
		MULSD( acc, xmm_sqrx )
		ADDSD( acc, coef )

	HORNER_STEP(xmm_cos, Constant.float64(Cos.c12) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c11) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c10) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c9) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c8) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c7) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c6) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c5) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c4) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c3) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c2) )
	MULSD( xmm_sin, xmm_sqrx )
	MULSD( xmm_cos, xmm_sqrx )
	MULSD( xmm_sin, xmm_x )
	ADDSD( xmm_sin, xmm_x )
	ADDSD( xmm_cos, Constant.float64(Cos.c0) )

	xmm_sign = SSERegister()
	LOAD.CONSTANT( xmm_sign, Constant.float64(Sin.sign_mask) )
	PAND( xmm_sign, xmm_n )
	PADDQ( xmm_n, xmm_n )
	BLENDVPD( xmm_sin, xmm_cos, xmm_n )
	XORPD( xmm_sin, xmm_sign )
	MOVSD( [yPointer], xmm_sin )

def SCALAR_SIN_AVX(xPointer, yPointer, is_prologue):
	# x = *xPointer
	xmm_x = SSERegister()
	VMOVSD( xmm_x, [xPointer] )
	# t = (x * (2/Pi)) + magic_bias
	xmm_t = SSERegister()
	ASSUME.INITIALIZED( xmm_t )
	VMULSD( xmm_t, xmm_x, Constant.float64(TrigReduction.two_over_pi) )
	VADDSD( xmm_t, xmm_t, Constant.float64(Sin.magic_bias) )
	# n = (int(t) % 4) << 62
	xmm_n = SSERegister()
	VPSLLQ( xmm_n, xmm_t, 62 )
	# t -= magic_bias
	VSUBSD( xmm_t, xmm_t, Constant.float64(Sin.magic_bias) )
	# x += t * (-pi/2)_hi
	xmm_temp = SSERegister()
	ASSUME.INITIALIZED( xmm_temp )
	VMULSD( xmm_temp, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_hi) )
	VADDSD( xmm_x, xmm_x, xmm_temp )
	# x += t * (-pi/2)_me
	xmm_a = SSERegister()
	VMOVAPS( xmm_a, xmm_x )
	xmm_temp = SSERegister()
	ASSUME.INITIALIZED( xmm_temp )
	VMULSD( xmm_temp, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_me) )
	VADDSD( xmm_x, xmm_temp )
	xmm_z = SSERegister()
	ASSUME.INITIALIZED( xmm_z )
	VSUBSD( xmm_z, xmm_x, xmm_a )
	VSUBSD( xmm_temp, xmm_temp, xmm_z )
	# x += t * (-pi/2)_lo
	VMULSD( xmm_t, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_lo) )
	VADDSD( xmm_t, xmm_t, xmm_temp )
	VADDSD( xmm_x, xmm_x, xmm_t )

	# sqrx = x * x
	xmm_sqrx = SSERegister()
	ASSUME.INITIALIZED( xmm_sqrx )
	VMULSD( xmm_sqrx, xmm_x, xmm_x )

	xmm_sin = SSERegister()
	xmm_cos = SSERegister()
	LOAD.CONSTANT( xmm_cos, Constant.float64(Cos.c14) )
	LOAD.CONSTANT( xmm_sin, Constant.float64(Sin.c13) )

	def HORNER_STEP(acc, coef):
		if Target.has_fma4():
			VFMADDSD( acc, acc, xmm_sqrx, coef )
		elif Target.has_fma3():
			VFMADD213SD( acc, acc, xmm_sqrx, coef )
		else:
			VMULSD( acc, acc, xmm_sqrx )
			VADDSD( acc, acc, coef )

	HORNER_STEP(xmm_cos, Constant.float64(Cos.c12) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c11) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c10) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c9) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c8) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c7) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c6) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c5) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c4) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c3) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c2) )
	VMULSD( xmm_sin, xmm_sin, xmm_sqrx )
	if Target.has_fma4():
		VFMADDSD( xmm_cos, xmm_cos, xmm_sqrx, Constant.float64(Cos.c0) )
		VFMADDSD( xmm_sin, xmm_sin, xmm_x, xmm_x )
	elif Target.has_fma3():
		VFMADD213SD( xmm_cos, xmm_cos, xmm_sqrx, Constant.float64(Cos.c0) )
		VFMADD213SD( xmm_sin, xmm_sin, xmm_x, xmm_x )
	else:
		VMULSD( xmm_cos, xmm_cos, xmm_sqrx )
		VMULSD( xmm_sin, xmm_sin, xmm_x )
		VADDSD( xmm_cos, xmm_cos, Constant.float64(Cos.c0) )
		VADDSD( xmm_sin, xmm_sin, xmm_x )

	xmm_sign = SSERegister()
	LOAD.CONSTANT( xmm_sign, Constant.float64(Sin.sign_mask) )
	VPAND( xmm_sign, xmm_n )
	VPADDQ( xmm_n, xmm_n )
	VBLENDVPD( xmm_sin, xmm_sin, xmm_cos, xmm_n )
	VXORPD( xmm_sin, xmm_sign )
	VMOVSD( [yPointer], xmm_sin )

def BATCH_SIN_SSE(xPointer, yPointer):
	xmm_x = [SSERegister() for i in range(5)]
	xmm_t = [SSERegister() for i in range(5)]
	xmm_r = [SSERegister() for i in range(5)]
	n = [LocalVariable(SSERegister) for i in range(5)]
	x = [LocalVariable(SSERegister) for i in range(5)]
	for i in range(5):
		xmm_n = SSERegister()

		# x = *xPointer
		MOVAPD( xmm_x[i], [xPointer + i * 16] )
		# t = (x * (2/Pi)) + magicBias
		MOVAPS( xmm_t[i], xmm_x[i] )
		MULPD( xmm_t[i], Constant.float64x2(TrigReduction.two_over_pi) )
		ADDPD( xmm_t[i], Constant.float64x2(Sin.magic_bias) )

	for i in range(5):
		# n = (int(t) % 4) << 62
		MOVAPS( xmm_n, xmm_t[i] )
		PSLLQ( xmm_n, 62 )
		MOVDQA( n[i], xmm_n )
		# t -= magicBias
		SUBPD( xmm_t[i], Constant.float64x2(Sin.magic_bias) )

	for i in range(5):
		# x += t * minusPio2_hi
		xmm_temp = SSERegister()
		MOVAPS( xmm_temp, Constant.float64x2(TrigReduction.minus_pi_o2_hi) )
		MULPD( xmm_temp, xmm_t[i] )
		ADDPD( xmm_x[i], xmm_temp )

	for i in range(5):
		# x += t * minusPio2_me
		xmm_a = SSERegister()
		MOVAPS( xmm_a, xmm_x[i] )
		MOVAPS( xmm_r[i], Constant.float64x2(TrigReduction.minus_pi_o2_me) )
		MULPD( xmm_r[i], xmm_t[i] )
		ADDPD( xmm_x[i], xmm_r[i] )
		xmm_z = SSERegister()
		MOVAPS( xmm_z, xmm_x[i] )
		SUBPD( xmm_z, xmm_a )
		SUBPD( xmm_r[i], xmm_z )
		# x += t * minusPio2_lo
		MULPD( xmm_t[i], Constant.float64x2(TrigReduction.minus_pi_o2_lo) )
		ADDPD( xmm_t[i], xmm_r[i] )
		ADDPD( xmm_x[i], xmm_t[i] )


	xmm_sqrx = [SSERegister() for i in range(5)]
	for i in range(5):
		# sqrx = x * x
		MOVAPD( x[i], xmm_x[i] )
		MULPD( xmm_x[i], xmm_x[i] )
		xmm_sqrx[i] = xmm_x[i]

	xmm_sin = [SSERegister() for i in range(5)]
	xmm_cos = [SSERegister() for i in range(5)]
	for i in range(5):
		LOAD.CONSTANT( xmm_cos[i], Constant.float64x2(Cos.c14) )
		LOAD.CONSTANT( xmm_sin[i], Constant.float64x2(Sin.c13) )

	def HORNER_STEP(acc, coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(5):
			MULPD( acc[i], xmm_sqrx[i] )
			ADDPD( acc[i], xmm_c )

	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c12) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c11) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c10) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c9) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c8) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c7) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c6) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c5) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c4) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c3) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c2) )

	for i in range(5):
		MULPD( xmm_sin[i], xmm_sqrx[i] )
		MULPD( xmm_cos[i], xmm_sqrx[i] )
		xmm_x = SSERegister()
		MOVAPD( xmm_x, x[i] )
		MULPD( xmm_sin[i], xmm_x )
		ADDPD( xmm_sin[i], xmm_x )
		ADDPD( xmm_cos[i], Constant.float64x2(Cos.c0) )

	for i in range(5):
		xmm_n = xmm0
		MOVAPD( xmm_n, n[i] )
		xmm_sign = SSERegister()
		LOAD.CONSTANT( xmm_sign, Constant.float64x2(Sin.sign_mask) )
		PAND( xmm_sign, xmm_n )
		PADDQ( xmm_n, xmm_n )
		BLENDVPD( xmm_sin[i], xmm_cos[i], xmm_n )
		XORPD( xmm_sin[i], xmm_sign )
		MOVUPD( [yPointer + 16 * i], xmm_sin[i] )

def BATCH_SIN_Bulldozer(xPointer, yPointer):
	xmm_x = [SSERegister() for i in range(5)]
	xmm_t = [SSERegister() for i in range(5)]
	n = [LocalVariable(SSERegister) for i in range(5)]
	x = [LocalVariable(SSERegister) for i in range(5)]

	xmm_two_over_pi = SSERegister()
	xmm_magic_bias = SSERegister()
	LOAD.CONSTANT( xmm_two_over_pi, Constant.float64x2(TrigReduction.two_over_pi) )
	LOAD.CONSTANT( xmm_magic_bias, Constant.float64x2(Sin.magic_bias) )
	for i in range(5):
		# x = *xPointer
		VMOVAPD( xmm_x[i], [xPointer + i * 16] )
		# t = (x * (2/Pi)) + magicBias
		VMULPD( xmm_t[i], xmm_x[i], xmm_two_over_pi )
		VADDPD( xmm_t[i], xmm_t[i], xmm_magic_bias )

	for i in range(5):
		xmm_n = SSERegister()
		# n = (int(t) % 4) << 62
		VPSLLQ( xmm_n, xmm_t[i], 62 )
		VMOVDQA( n[i], xmm_n )
		# t -= magicBias
		VSUBPD( xmm_t[i], xmm_t[i], xmm_magic_bias )

	for i in range(5):
		# x += t * minusPio2_hi
		xmm_temp = SSERegister()
		VMULPD( xmm_temp, xmm_t[i], Constant.float64x2(TrigReduction.minus_pi_o2_hi) )
		VADDPD( xmm_x[i], xmm_x[i], xmm_temp )

	for i in range(5):
		xmm_temp = SSERegister()
		# x += t * minusPio2_me
		xmm_a = xmm_x[i]
		xmm_x[i] = SSERegister()
		VMULPD( xmm_temp, xmm_t[i], Constant.float64x2(TrigReduction.minus_pi_o2_me) )
		VADDPD( xmm_x[i], xmm_a, xmm_temp )
		xmm_z = SSERegister()
		VSUBPD( xmm_z, xmm_x[i], xmm_a )
		VSUBPD( xmm_temp, xmm_temp, xmm_z )
		# x += t * minusPio2_lo
		VMULPD( xmm_t[i], xmm_t[i], Constant.float64x2(TrigReduction.minus_pi_o2_lo) )
		VADDPD( xmm_t[i], xmm_t[i], xmm_temp )
		VADDPD( xmm_x[i], xmm_x[i], xmm_t[i] )

	xmm_sqrx = [SSERegister() for i in range(5)]
	for i in range(5):
		# sqrx = x * x
		VMOVAPD( x[i], xmm_x[i] )
		VMULPD( xmm_sqrx[i], xmm_x[i], xmm_x[i] )

	xmm_sin = [SSERegister() for i in range(5)]
	xmm_cos = [SSERegister() for i in range(5)]

	xmm_c14 = SSERegister()
	xmm_c12 = SSERegister()
	LOAD.CONSTANT( xmm_c14, Constant.float64x2(Cos.c14) )
	LOAD.CONSTANT( xmm_c12, Constant.float64x2(Cos.c12) )
	for i in range(5):
		VFMADDPD( xmm_cos[i], xmm_c14, xmm_sqrx[i], xmm_c12 )

	xmm_c13 = SSERegister()
	xmm_c11 = SSERegister()
	LOAD.CONSTANT( xmm_c13, Constant.float64x2(Sin.c13) )
	LOAD.CONSTANT( xmm_c11, Constant.float64x2(Sin.c11) )
	for i in range(5):
		VFMADDPD( xmm_sin[i], xmm_c13, xmm_sqrx[i], xmm_c11 )

	def HORNER_STEP(acc, coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(5):
			VFMADDPD( acc[i], acc[i], xmm_sqrx[i], xmm_c )

	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c10) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c9) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c8) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c7) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c6) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c5) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c4) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c3) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.c2) )

	for i in range(5):
		VMULPD( xmm_sin[i], xmm_sqrx[i] )

	xmm_c0 = SSERegister()
	LOAD.CONSTANT( xmm_c0, Constant.float64x2(Cos.c0) )
	for i in range(5):
		VFMADDPD( xmm_cos[i], xmm_cos[i], xmm_sqrx[i], xmm_c0 )

	for i in range(5):
		xmm_x = SSERegister()
		VMOVAPD( xmm_x, x[i] )
		VFMADDPD( xmm_sin[i], xmm_sin[i], xmm_x, xmm_x )

	xmm_sign_mask = SSERegister()
	LOAD.CONSTANT( xmm_sign_mask, Constant.float64x2(Sin.sign_mask) )
	for i in range(5):
		xmm_n = SSERegister()
		VMOVAPD( xmm_n, n[i] )
		xmm_sign = SSERegister()
		VANDPD( xmm_sign, xmm_n, xmm_sign_mask )
		VPADDQ( xmm_n, xmm_n, xmm_n )
		VBLENDVPD( xmm_sin[i], xmm_sin[i], xmm_cos[i], xmm_n )
		VXORPD( xmm_sin[i], xmm_sign )
		VMOVUPD( [yPointer + 16 * i], xmm_sin[i] )

def BATCH_SIN_AVX(xPointer, yPointer):
	ymm_x = [AVXRegister() for i in range(5)]
	ymm_t = [AVXRegister() for i in range(5)]
	n = [LocalVariable(AVXRegister) for i in range(5)]
	x = [LocalVariable(AVXRegister) for i in range(5)]

	ymm_two_over_pi = AVXRegister()
	ymm_magic_bias = AVXRegister()
	LOAD.CONSTANT( ymm_two_over_pi, Constant.float64x4(TrigReduction.two_over_pi) )
	LOAD.CONSTANT( ymm_magic_bias, Constant.float64x4(Sin.magic_bias) )
	for i in range(5):
		# x = *xPointer
		VMOVAPD( ymm_x[i], [xPointer + i * 32] )
		# t = (x * (2/Pi)) + magicBias
		VMULPD( ymm_t[i], ymm_x[i], ymm_two_over_pi )
		VADDPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

	for i in range(5):
		xmm_n_lo = SSERegister()
		xmm_n_hi = SSERegister()
		# n = (int(t) % 4) << 62
		VEXTRACTF128( xmm_n_hi, ymm_t[i], 1 )
		VPSLLQ( xmm_n_lo, ymm_t[i].get_oword(), 62 )
		VMOVDQA( n[i].get_low(), xmm_n_lo )
		VPSLLQ( xmm_n_hi, xmm_n_hi, 62 )
		VMOVDQA( n[i].get_high(), xmm_n_hi )
		# t -= magicBias
		VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

	for i in range(5):
		# x += t * minusPio2_hi
		ymm_temp = AVXRegister()
		VMULPD( ymm_temp, ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_hi) )
		VADDPD( ymm_x[i], ymm_x[i], ymm_temp )

	for i in range(5):
		ymm_temp = AVXRegister()
		# x += t * minusPio2_me
		ymm_a = ymm_x[i]
		ymm_x[i] = AVXRegister()
		VMULPD( ymm_temp, ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_me) )
		VADDPD( ymm_x[i], ymm_a, ymm_temp )
		ymm_z = AVXRegister()
		VSUBPD( ymm_z, ymm_x[i], ymm_a )
		VSUBPD( ymm_temp, ymm_temp, ymm_z )
		# x += t * minusPio2_lo
		VMULPD( ymm_t[i], ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_lo) )
		VADDPD( ymm_t[i], ymm_t[i], ymm_temp )
		VADDPD( ymm_x[i], ymm_x[i], ymm_t[i] )

	ymm_sqrx = [AVXRegister() for i in range(5)]
	for i in range(5):
		# sqrx = x * x
		VMOVAPD( x[i], ymm_x[i] )
		VMULPD( ymm_sqrx[i], ymm_x[i], ymm_x[i] )

	ymm_sin = [AVXRegister() for i in range(5)]
	ymm_cos = [AVXRegister() for i in range(5)]

	ymm_c14 = AVXRegister()
	ymm_c12 = AVXRegister()
	LOAD.CONSTANT( ymm_c14, Constant.float64x4(Cos.c14) )
	LOAD.CONSTANT( ymm_c12, Constant.float64x4(Cos.c12) )
	for i in range(5):
		if Target.has_fma3():
			VMOVAPD( ymm_cos[i], ymm_c14 )
			VFMADD213PD( ymm_cos[i], ymm_cos[i], ymm_sqrx[i], ymm_c12 )
		else:
			VMULPD( ymm_cos[i], ymm_c14, ymm_sqrx[i] )
			VADDPD( ymm_cos[i], ymm_cos[i], ymm_c12 )

	ymm_c13 = AVXRegister()
	ymm_c11 = AVXRegister()
	LOAD.CONSTANT( ymm_c13, Constant.float64x4(Sin.c13) )
	LOAD.CONSTANT( ymm_c11, Constant.float64x4(Sin.c11) )
	for i in range(5):
		if Target.has_fma3():
			VMOVAPD( ymm_sin[i], ymm_c13 )
			VFMADD213PD( ymm_sin[i], ymm_sin[i], ymm_sqrx[i], ymm_c11 )
		else:
			VMULPD( ymm_sin[i], ymm_c13, ymm_sqrx[i] )
			VADDPD( ymm_sin[i], ymm_sin[i], ymm_c11 )

	def HORNER_STEP(acc, coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(5):
			if Target.has_fma3():
				VFMADD213PD( acc[i], acc[i], ymm_sqrx[i], ymm_c )
			else:
				VMULPD( acc[i], acc[i], ymm_sqrx[i] )
				VADDPD( acc[i], acc[i], ymm_c )

	HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c10) )
	HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c9) )
	HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c8) )
	HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c7) )
	HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c6) )
	HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c5) )
	HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c4) )
	HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c3) )
	HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c2) )

	for i in range(5):
		VMULPD( ymm_sin[i], ymm_sqrx[i] )

	ymm_c0 = AVXRegister()
	LOAD.CONSTANT( ymm_c0, Constant.float64x4(Cos.c0) )
	for i in range(5):
		if Target.has_fma3():
			VFMADD213PD( ymm_cos[i], ymm_cos[i], ymm_sqrx[i], ymm_c0 )
		else:
			VMULPD( ymm_cos[i], ymm_sqrx[i] )
			VADDPD( ymm_cos[i], ymm_c0 )

	for i in range(5):
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, x[i] )
		if Target.has_fma3():
			VFMADD213PD( ymm_sin[i], ymm_sin[i], ymm_x, ymm_x )
		else:
			VMULPD( ymm_sin[i], ymm_x )
			VADDPD( ymm_sin[i], ymm_x )

	ymm_sign_mask = AVXRegister()
	LOAD.CONSTANT( ymm_sign_mask, Constant.float64x4(Sin.sign_mask) )
	for i in range(5):
		ymm_n = AVXRegister()
		VMOVAPD( ymm_n, n[i] )
		ymm_sign = AVXRegister()
		VANDPD( ymm_sign, ymm_n, ymm_sign_mask )
		if Target.has_avx2():
			VPADDQ( ymm_n, ymm_n, ymm_n )
			VBLENDVPD( ymm_sin[i], ymm_sin[i], ymm_cos[i], ymm_n )
		else:
			VANDNPD( ymm_n, ymm_sign_mask, ymm_n )
			# n = !(n & 1)
			VCMPEQPD( ymm_n, ymm_n, ymm_sign )
			VBLENDVPD( ymm_sin[i], ymm_cos[i], ymm_sin[i], ymm_n )
		VXORPD( ymm_sin[i], ymm_sign )

		if Target.get_st_eu_width() == 256:
			VMOVUPD( [yPointer + 32 * i], ymm_sin[i] )
		else:
			VMOVUPD( [yPointer + 32 * i], ymm_sin[i].get_oword() )
			VEXTRACTF128( [yPointer + 32 * i + 16], ymm_sin[i], 1 )

def Sin_V64f_V64f(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-sysv', 'x64-ms']:
		if module == 'Math':
			if function == 'Sin':
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not x_type.is_floating_point() or not y_type.is_floating_point():
					return

				x_size = x_type.get_size(codegen.abi)
				y_size = y_type.get_size(codegen.abi)

				if x_size != 8 or y_size != 8:
					return

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )

					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )

					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_SIN_SSE, BATCH_SIN_SSE, None, xPointer, yPointer, length, 32, 16 * 5, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_SIN_AVX, BATCH_SIN_AVX, None, xPointer, yPointer, length, 32, 32 * 5, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )

					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )

					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_SIN_AVX, BATCH_SIN_Bulldozer, None, xPointer, yPointer, length, 16, 16 * 5, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_SIN_AVX, BATCH_SIN_AVX, None, xPointer, yPointer, length, 32, 32 * 5, 8)

def SCALAR_COS_SSE(xPointer, yPointer, is_prologue):
	if Target.get_int_eu_width() == 64:
		def SCALAR_COPY(destination, source):
			ASSUME.INITIALIZED( destination )
			MOVSD( destination, source )
	else:
		def SCALAR_COPY(destination, source):
			MOVAPS( destination, source )

	# x = *xPointer
	xmm_x = SSERegister()
	MOVSD( xmm_x, [xPointer] )
	# t = (x * (2/Pi)) + magic_bias
	xmm_t = SSERegister()
	SCALAR_COPY( xmm_t, xmm_x )
	MULSD( xmm_t, Constant.float64(TrigReduction.two_over_pi) )
	ADDSD( xmm_t, Constant.float64(Cos.magic_bias) )
	# n = (int(t) % 4) << 62
	xmm_n = xmm0
	SCALAR_COPY( xmm_n, xmm_t )
	PSLLQ( xmm_n, 62 )
	# t -= magic_bias
	SUBSD( xmm_t, Constant.float64(Cos.magic_bias) )
	# x += t * (-pi/2)_hi
	xmm_temp = SSERegister()
	LOAD.CONSTANT( xmm_temp, Constant.float64(TrigReduction.minus_pi_o2_hi) )
	MULSD( xmm_temp, xmm_t )
	ADDSD( xmm_x, xmm_temp )
	# x += t * (-pi/2)_me
	xmm_a = SSERegister()
	SCALAR_COPY( xmm_a, xmm_x )
	xmm_temp = SSERegister()
	LOAD.CONSTANT( xmm_temp, Constant.float64(TrigReduction.minus_pi_o2_me) )
	MULSD( xmm_temp, xmm_t )
	ADDSD( xmm_x, xmm_temp )
	xmm_z = SSERegister()
	SCALAR_COPY( xmm_z, xmm_x )
	SUBSD( xmm_z, xmm_a )
	SUBSD( xmm_temp, xmm_z )
	# x += t * (-pi/2)_lo
	MULSD( xmm_t, Constant.float64(TrigReduction.minus_pi_o2_lo) )
	ADDSD( xmm_t, xmm_temp )
	ADDSD( xmm_x, xmm_t )

	# sqrx = x * x
	xmm_sqrx = SSERegister()
	SCALAR_COPY( xmm_sqrx, xmm_x )
	MULSD( xmm_sqrx, xmm_sqrx )

	xmm_sin = SSERegister()
	xmm_cos = SSERegister()
	LOAD.CONSTANT( xmm_cos, Constant.float64(Cos.minus_c14) )
	LOAD.CONSTANT( xmm_sin, Constant.float64(Sin.c13) )

	def HORNER_STEP(acc, coef):
		MULSD( acc, xmm_sqrx )
		ADDSD( acc, coef )

	HORNER_STEP(xmm_cos, Constant.float64(Cos.minus_c12) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c11) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.minus_c10) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c9) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.minus_c8) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c7) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.minus_c6) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c5) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.minus_c4) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c3) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.minus_c2) )
	MULSD( xmm_sin, xmm_sqrx )
	MULSD( xmm_cos, xmm_sqrx )
	MULSD( xmm_sin, xmm_x )
	ADDSD( xmm_sin, xmm_x )
	ADDSD( xmm_cos, Constant.float64(Cos.minus_c0) )

	xmm_sign = SSERegister()
	LOAD.CONSTANT( xmm_sign, Constant.float64(Sin.sign_mask) )
	PAND( xmm_sign, xmm_n )
	PADDQ( xmm_n, xmm_n )
	BLENDVPD( xmm_cos, xmm_sin, xmm_n )
	XORPD( xmm_cos, xmm_sign )
	MOVSD( [yPointer], xmm_cos )

def SCALAR_COS_AVX(xPointer, yPointer, is_prologue):
	# x = *xPointer
	xmm_x = SSERegister()
	VMOVSD( xmm_x, [xPointer] )
	# t = (x * (2/Pi)) + magic_bias
	xmm_t = SSERegister()
	ASSUME.INITIALIZED( xmm_t )
	VMULSD( xmm_t, xmm_x, Constant.float64(TrigReduction.two_over_pi) )
	VADDSD( xmm_t, xmm_t, Constant.float64(Cos.magic_bias) )
	# n = (int(t) % 4) << 62
	xmm_n = SSERegister()
	VPSLLQ( xmm_n, xmm_t, 62 )
	# t -= magic_bias
	VSUBSD( xmm_t, xmm_t, Constant.float64(Cos.magic_bias) )
	# x += t * (-pi/2)_hi
	xmm_temp = SSERegister()
	ASSUME.INITIALIZED( xmm_temp )
	VMULSD( xmm_temp, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_hi) )
	VADDSD( xmm_x, xmm_x, xmm_temp )
	# x += t * (-pi/2)_me
	xmm_a = SSERegister()
	VMOVAPS( xmm_a, xmm_x )
	xmm_temp = SSERegister()
	ASSUME.INITIALIZED( xmm_temp )
	VMULSD( xmm_temp, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_me) )
	VADDSD( xmm_x, xmm_temp )
	xmm_z = SSERegister()
	ASSUME.INITIALIZED( xmm_z )
	VSUBSD( xmm_z, xmm_x, xmm_a )
	VSUBSD( xmm_temp, xmm_temp, xmm_z )
	# x += t * (-pi/2)_lo
	VMULSD( xmm_t, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_lo) )
	VADDSD( xmm_t, xmm_t, xmm_temp )
	VADDSD( xmm_x, xmm_x, xmm_t )

	# sqrx = x * x
	xmm_sqrx = SSERegister()
	ASSUME.INITIALIZED( xmm_sqrx )
	VMULSD( xmm_sqrx, xmm_x, xmm_x )

	xmm_sin = SSERegister()
	xmm_minus_cos = SSERegister()
	LOAD.CONSTANT( xmm_minus_cos, Constant.float64(Cos.minus_c14) )
	LOAD.CONSTANT( xmm_sin, Constant.float64(Sin.c13) )

	def HORNER_STEP(acc, coef):
		if Target.has_fma4():
			VFMADDSD( acc, acc, xmm_sqrx, coef )
		elif Target.has_fma3():
			VFMADD213SD( acc, acc, xmm_sqrx, coef )
		else:
			VMULSD( acc, acc, xmm_sqrx )
			VADDSD( acc, acc, coef )

	HORNER_STEP(xmm_minus_cos, Constant.float64(Cos.minus_c12) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c11) )
	HORNER_STEP(xmm_minus_cos, Constant.float64(Cos.minus_c10) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c9) )
	HORNER_STEP(xmm_minus_cos, Constant.float64(Cos.minus_c8) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c7) )
	HORNER_STEP(xmm_minus_cos, Constant.float64(Cos.minus_c6) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c5) )
	HORNER_STEP(xmm_minus_cos, Constant.float64(Cos.minus_c4) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c3) )
	HORNER_STEP(xmm_minus_cos, Constant.float64(Cos.minus_c2) )
	VMULSD( xmm_sin, xmm_sin, xmm_sqrx )
	if Target.has_fma4():
		VFMADDSD( xmm_minus_cos, xmm_minus_cos, xmm_sqrx, Constant.float64(Cos.minus_c0) )
		VFMADDSD( xmm_sin, xmm_sin, xmm_x, xmm_x )
	elif Target.has_fma3():
		VFMADD213SD( xmm_minus_cos, xmm_minus_cos, xmm_sqrx, Constant.float64(Cos.minus_c0) )
		VFMADD213SD( xmm_sin, xmm_sin, xmm_x, xmm_x )
	else:
		VMULSD( xmm_minus_cos, xmm_minus_cos, xmm_sqrx )
		VMULSD( xmm_sin, xmm_sin, xmm_x )
		VADDSD( xmm_minus_cos, xmm_minus_cos, Constant.float64(Cos.minus_c0) )
		VADDSD( xmm_sin, xmm_sin, xmm_x )

	xmm_sign = SSERegister()
	LOAD.CONSTANT( xmm_sign, Constant.float64(Sin.sign_mask) )
	VPAND( xmm_sign, xmm_n )
	VPADDQ( xmm_n, xmm_n )
	
	xmm_cos = SSERegister()
	VBLENDVPD( xmm_cos, xmm_minus_cos, xmm_sin, xmm_n )
	VXORPD( xmm_cos, xmm_sign )
	VMOVSD( [yPointer], xmm_cos )

def BATCH_COS_SSE(xPointer, yPointer):
	xmm_x = [SSERegister() for i in range(5)]
	xmm_t = [SSERegister() for i in range(5)]
	xmm_r = [SSERegister() for i in range(5)]
	n = [LocalVariable(SSERegister) for i in range(5)]
	x = [LocalVariable(SSERegister) for i in range(5)]
	for i in range(5):
		xmm_n = SSERegister()

		# x = *xPointer
		MOVAPD( xmm_x[i], [xPointer + i * 16] )
		# t = (x * (2/Pi)) + magicBias
		MOVAPS( xmm_t[i], xmm_x[i] )
		MULPD( xmm_t[i], Constant.float64x2(TrigReduction.two_over_pi) )
		ADDPD( xmm_t[i], Constant.float64x2(Cos.magic_bias) )

	for i in range(5):
		# n = (int(t) % 4) << 62
		MOVAPS( xmm_n, xmm_t[i] )
		PSLLQ( xmm_n, 62 )
		MOVDQA( n[i], xmm_n )
		# t -= magicBias
		SUBPD( xmm_t[i], Constant.float64x2(Cos.magic_bias) )

	for i in range(5):
		# x += t * minusPio2_hi
		xmm_temp = SSERegister()
		MOVAPS( xmm_temp, Constant.float64x2(TrigReduction.minus_pi_o2_hi) )
		MULPD( xmm_temp, xmm_t[i] )
		ADDPD( xmm_x[i], xmm_temp )

	for i in range(5):
		# x += t * minusPio2_me
		xmm_a = SSERegister()
		MOVAPS( xmm_a, xmm_x[i] )
		MOVAPS( xmm_r[i], Constant.float64x2(TrigReduction.minus_pi_o2_me) )
		MULPD( xmm_r[i], xmm_t[i] )
		ADDPD( xmm_x[i], xmm_r[i] )
		xmm_z = SSERegister()
		MOVAPS( xmm_z, xmm_x[i] )
		SUBPD( xmm_z, xmm_a )
		SUBPD( xmm_r[i], xmm_z )
		# x += t * minusPio2_lo
		MULPD( xmm_t[i], Constant.float64x2(TrigReduction.minus_pi_o2_lo) )
		ADDPD( xmm_t[i], xmm_r[i] )
		ADDPD( xmm_x[i], xmm_t[i] )


	xmm_sqrx = [SSERegister() for i in range(5)]
	for i in range(5):
		# sqrx = x * x
		MOVAPD( x[i], xmm_x[i] )
		MULPD( xmm_x[i], xmm_x[i] )
		xmm_sqrx[i] = xmm_x[i]

	xmm_sin = [SSERegister() for i in range(5)]
	xmm_cos = [SSERegister() for i in range(5)]
	for i in range(5):
		LOAD.CONSTANT( xmm_cos[i], Constant.float64x2(Cos.minus_c14) )
		LOAD.CONSTANT( xmm_sin[i], Constant.float64x2(Sin.c13) )

	def HORNER_STEP(acc, coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(5):
			MULPD( acc[i], xmm_sqrx[i] )
			ADDPD( acc[i], xmm_c )

	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.minus_c12) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c11) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.minus_c10) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c9) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.minus_c8) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c7) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.minus_c6) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c5) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.minus_c4) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c3) )
	HORNER_STEP(xmm_cos, Constant.float64x2(Cos.minus_c2) )

	for i in range(5):
		MULPD( xmm_sin[i], xmm_sqrx[i] )
		MULPD( xmm_cos[i], xmm_sqrx[i] )
		xmm_x = SSERegister()
		MOVAPD( xmm_x, x[i] )
		MULPD( xmm_sin[i], xmm_x )
		ADDPD( xmm_sin[i], xmm_x )
		ADDPD( xmm_cos[i], Constant.float64x2(Cos.minus_c0) )

	for i in range(5):
		xmm_n = xmm0
		MOVAPD( xmm_n, n[i] )
		xmm_sign = SSERegister()
		LOAD.CONSTANT( xmm_sign, Constant.float64x2(Sin.sign_mask) )
		PAND( xmm_sign, xmm_n )
		PADDQ( xmm_n, xmm_n )
		BLENDVPD( xmm_cos[i], xmm_sin[i], xmm_n )
		XORPD( xmm_cos[i], xmm_sign )
		MOVUPD( [yPointer + 16 * i], xmm_cos[i] )

def BATCH_COS_Bulldozer(xPointer, yPointer):
	xmm_x = [SSERegister() for i in range(5)]
	xmm_t = [SSERegister() for i in range(5)]
	n = [LocalVariable(SSERegister) for i in range(5)]
	x = [LocalVariable(SSERegister) for i in range(5)]

	xmm_two_over_pi = SSERegister()
	xmm_magic_bias = SSERegister()
	LOAD.CONSTANT( xmm_two_over_pi, Constant.float64x2(TrigReduction.two_over_pi) )
	LOAD.CONSTANT( xmm_magic_bias, Constant.float64x2(Cos.magic_bias) )
	for i in range(5):
		# x = *xPointer
		VMOVAPD( xmm_x[i], [xPointer + i * 16] )
		# t = (x * (2/Pi)) + magicBias
		VMULPD( xmm_t[i], xmm_x[i], xmm_two_over_pi )
		VADDPD( xmm_t[i], xmm_t[i], xmm_magic_bias )

	for i in range(5):
		xmm_n = SSERegister()
		# n = (int(t) % 4) << 62
		VPSLLQ( xmm_n, xmm_t[i], 62 )
		VMOVDQA( n[i], xmm_n )
		# t -= magicBias
		VSUBPD( xmm_t[i], xmm_t[i], xmm_magic_bias )

	for i in range(5):
		# x += t * minusPio2_hi
		xmm_temp = SSERegister()
		VMULPD( xmm_temp, xmm_t[i], Constant.float64x2(TrigReduction.minus_pi_o2_hi) )
		VADDPD( xmm_x[i], xmm_x[i], xmm_temp )

	for i in range(5):
		xmm_temp = SSERegister()
		# x += t * minusPio2_me
		xmm_a = xmm_x[i]
		xmm_x[i] = SSERegister()
		VMULPD( xmm_temp, xmm_t[i], Constant.float64x2(TrigReduction.minus_pi_o2_me) )
		VADDPD( xmm_x[i], xmm_a, xmm_temp )
		xmm_z = SSERegister()
		VSUBPD( xmm_z, xmm_x[i], xmm_a )
		VSUBPD( xmm_temp, xmm_temp, xmm_z )
		# x += t * minusPio2_lo
		VMULPD( xmm_t[i], xmm_t[i], Constant.float64x2(TrigReduction.minus_pi_o2_lo) )
		VADDPD( xmm_t[i], xmm_t[i], xmm_temp )
		VADDPD( xmm_x[i], xmm_x[i], xmm_t[i] )

	xmm_sqrx = [SSERegister() for i in range(5)]
	for i in range(5):
		# sqrx = x * x
		VMOVAPD( x[i], xmm_x[i] )
		VMULPD( xmm_sqrx[i], xmm_x[i], xmm_x[i] )

	xmm_c14 = SSERegister()
	xmm_c12 = SSERegister()
	LOAD.CONSTANT( xmm_c14, Constant.float64x2(Cos.minus_c14) )
	LOAD.CONSTANT( xmm_c12, Constant.float64x2(Cos.minus_c12) )
	xmm_minus_cos = [SSERegister() for i in range(5)]
	for i in range(5):
		VFMADDPD( xmm_minus_cos[i], xmm_c14, xmm_sqrx[i], xmm_c12 )

	xmm_c13 = SSERegister()
	xmm_c11 = SSERegister()
	LOAD.CONSTANT( xmm_c13, Constant.float64x2(Sin.c13) )
	LOAD.CONSTANT( xmm_c11, Constant.float64x2(Sin.c11) )
	xmm_sin = [SSERegister() for i in range(5)]
	for i in range(5):
		VFMADDPD( xmm_sin[i], xmm_c13, xmm_sqrx[i], xmm_c11 )

	def HORNER_STEP(acc, coef):
		xmm_c = SSERegister()
		LOAD.CONSTANT( xmm_c, coef )
		for i in range(5):
			VFMADDPD( acc[i], acc[i], xmm_sqrx[i], xmm_c )

	HORNER_STEP(xmm_minus_cos, Constant.float64x2(Cos.minus_c10) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c9) )
	HORNER_STEP(xmm_minus_cos, Constant.float64x2(Cos.minus_c8) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c7) )
	HORNER_STEP(xmm_minus_cos, Constant.float64x2(Cos.minus_c6) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c5) )
	HORNER_STEP(xmm_minus_cos, Constant.float64x2(Cos.minus_c4) )
	HORNER_STEP(xmm_sin, Constant.float64x2(Sin.c3) )
	HORNER_STEP(xmm_minus_cos, Constant.float64x2(Cos.minus_c2) )

	for i in range(5):
		VMULPD( xmm_sin[i], xmm_sqrx[i] )
	
	xmm_c0 = SSERegister()
	LOAD.CONSTANT( xmm_c0, Constant.float64x2(Cos.minus_c0) )
	for i in range(5):
		VFMADDPD( xmm_minus_cos[i], xmm_minus_cos[i], xmm_sqrx[i], xmm_c0 )

	for i in range(5):
		xmm_x = SSERegister()
		VMOVAPD( xmm_x, x[i] )
		VFMADDPD( xmm_sin[i], xmm_sin[i], xmm_x, xmm_x )

	xmm_sign_mask = SSERegister()
	LOAD.CONSTANT( xmm_sign_mask, Constant.float64x2(Sin.sign_mask) )
	for i in range(5):
		xmm_n = SSERegister()
		VMOVAPD( xmm_n, n[i] )
		xmm_sign = SSERegister()
		VANDPD( xmm_sign, xmm_n, xmm_sign_mask )
		VPADDQ( xmm_n, xmm_n, xmm_n )
		xmm_cos = SSERegister()
		VBLENDVPD( xmm_cos, xmm_minus_cos[i], xmm_sin[i], xmm_n )
		VXORPD( xmm_cos, xmm_sign )
		VMOVUPD( [yPointer + 16 * i], xmm_cos )

def BATCH_COS_AVX(xPointer, yPointer):
	ymm_x = [AVXRegister() for i in range(5)]
	ymm_t = [AVXRegister() for i in range(5)]
	n = [LocalVariable(AVXRegister) for i in range(5)]
	x = [LocalVariable(AVXRegister) for i in range(5)]

	ymm_two_over_pi = AVXRegister()
	ymm_magic_bias = AVXRegister()
	LOAD.CONSTANT( ymm_two_over_pi, Constant.float64x4(TrigReduction.two_over_pi) )
	LOAD.CONSTANT( ymm_magic_bias, Constant.float64x4(Cos.magic_bias) )
	for i in range(5):
		# x = *xPointer
		VMOVAPD( ymm_x[i], [xPointer + i * 32] )
		# t = (x * (2/Pi)) + magicBias
		VMULPD( ymm_t[i], ymm_x[i], ymm_two_over_pi )
		VADDPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

	for i in range(5):
		xmm_n_lo = SSERegister()
		xmm_n_hi = SSERegister()
		# n = (int(t) % 4) << 62
		VEXTRACTF128( xmm_n_hi, ymm_t[i], 1 )
		VPSLLQ( xmm_n_lo, ymm_t[i].get_oword(), 62 )
		VMOVDQA( n[i].get_low(), xmm_n_lo )
		VPSLLQ( xmm_n_hi, xmm_n_hi, 62 )
		VMOVDQA( n[i].get_high(), xmm_n_hi )
		# t -= magicBias
		VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )

	for i in range(5):
		# x += t * minusPio2_hi
		ymm_temp = AVXRegister()
		VMULPD( ymm_temp, ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_hi) )
		VADDPD( ymm_x[i], ymm_x[i], ymm_temp )

	for i in range(5):
		ymm_temp = AVXRegister()
		# x += t * minusPio2_me
		ymm_a = ymm_x[i]
		ymm_x[i] = AVXRegister()
		VMULPD( ymm_temp, ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_me) )
		VADDPD( ymm_x[i], ymm_a, ymm_temp )
		ymm_z = AVXRegister()
		VSUBPD( ymm_z, ymm_x[i], ymm_a )
		VSUBPD( ymm_temp, ymm_temp, ymm_z )
		# x += t * minusPio2_lo
		VMULPD( ymm_t[i], ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_lo) )
		VADDPD( ymm_t[i], ymm_t[i], ymm_temp )
		VADDPD( ymm_x[i], ymm_x[i], ymm_t[i] )

	ymm_sqrx = [AVXRegister() for i in range(5)]
	for i in range(5):
		# sqrx = x * x
		VMOVAPD( x[i], ymm_x[i] )
		VMULPD( ymm_sqrx[i], ymm_x[i], ymm_x[i] )

	ymm_c14 = AVXRegister()
	ymm_c12 = AVXRegister()
	LOAD.CONSTANT( ymm_c14, Constant.float64x4(Cos.minus_c14) )
	LOAD.CONSTANT( ymm_c12, Constant.float64x4(Cos.minus_c12) )
	ymm_minus_cos = [AVXRegister() for i in range(5)]
	for i in range(5):
		if Target.has_fma3():
			VMOVAPD( ymm_minus_cos[i], ymm_c14 )
			VFMADD132PD( ymm_minus_cos[i], ymm_minus_cos[i], ymm_sqrx[i], ymm_c12 )
		else:
			VMULPD( ymm_minus_cos[i], ymm_c14, ymm_sqrx[i] )
			VADDPD( ymm_minus_cos[i], ymm_minus_cos[i], ymm_c12 )

	ymm_c13 = AVXRegister()
	ymm_c11 = AVXRegister()
	LOAD.CONSTANT( ymm_c13, Constant.float64x4(Sin.c13) )
	LOAD.CONSTANT( ymm_c11, Constant.float64x4(Sin.c11) )
	ymm_sin = [AVXRegister() for i in range(5)]
	for i in range(5):
		if Target.has_fma3():
			VMOVAPD( ymm_sin[i], ymm_c13 )
			VFMADD132PD( ymm_sin[i], ymm_sin[i], ymm_sqrx[i], ymm_c11 )
		else:
			VMULPD( ymm_sin[i], ymm_c13, ymm_sqrx[i] )
			VADDPD( ymm_sin[i], ymm_sin[i], ymm_c11 )

	def HORNER_STEP(acc, coef):
		ymm_c = AVXRegister()
		LOAD.CONSTANT( ymm_c, coef )
		for i in range(5):
			if Target.has_fma3():
				VFMADD132PD( acc[i], acc[i], ymm_sqrx[i], ymm_c )
			else:
				VMULPD( acc[i], acc[i], ymm_sqrx[i] )
				VADDPD( acc[i], acc[i], ymm_c )

	HORNER_STEP(ymm_minus_cos, Constant.float64x4(Cos.minus_c10) )
	HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c9) )
	HORNER_STEP(ymm_minus_cos, Constant.float64x4(Cos.minus_c8) )
	HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c7) )
	HORNER_STEP(ymm_minus_cos, Constant.float64x4(Cos.minus_c6) )
	HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c5) )
	HORNER_STEP(ymm_minus_cos, Constant.float64x4(Cos.minus_c4) )
	HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c3) )
	HORNER_STEP(ymm_minus_cos, Constant.float64x4(Cos.minus_c2) )

	for i in range(5):
		VMULPD( ymm_sin[i], ymm_sqrx[i] )
	
	ymm_c0 = AVXRegister()
	LOAD.CONSTANT( ymm_c0, Constant.float64x4(Cos.minus_c0) )
	for i in range(5):
		if Target.has_fma3():
			VFMADD132PD( ymm_minus_cos[i], ymm_minus_cos[i], ymm_sqrx[i], ymm_c0 )
		else:
			VMULPD( ymm_minus_cos[i], ymm_sqrx[i] )
			VADDPD( ymm_minus_cos[i], ymm_c0 )

	for i in range(5):
		ymm_x = AVXRegister()
		VMOVAPD( ymm_x, x[i] )
		if Target.has_fma3():
			VFMADD132PD( ymm_sin[i], ymm_sin[i], ymm_x, ymm_x )
		else:
			VMULPD( ymm_sin[i], ymm_x )
			VADDPD( ymm_sin[i], ymm_x )

	ymm_sign_mask = AVXRegister()
	LOAD.CONSTANT( ymm_sign_mask, Constant.float64x4(Sin.sign_mask) )
	for i in range(5):
		ymm_n = AVXRegister()
		VMOVAPD( ymm_n, n[i] )
		ymm_sign = AVXRegister()
		VANDPD( ymm_sign, ymm_n, ymm_sign_mask )
		if Target.has_avx2():
			VPADDQ( ymm_n, ymm_n, ymm_n )
			ymm_cos = AVXRegister()
			VBLENDVPD( ymm_cos, ymm_minus_cos[i], ymm_sin[i], ymm_n )
		else:
			VANDNPD( ymm_n, ymm_sign_mask, ymm_n )
			# n = !(n & 1)
			VCMPEQPD( ymm_n, ymm_n, ymm_sign )
			ymm_cos = AVXRegister()
			VBLENDVPD( ymm_cos, ymm_sin[i], ymm_minus_cos[i], ymm_n )
		VXORPD( ymm_cos, ymm_sign )
		
		if Target.get_st_eu_width() == 256:
			VMOVUPD( [yPointer + 32 * i], ymm_cos )
		else:
			VMOVUPD( [yPointer + 32 * i], ymm_cos.get_oword() )
			VEXTRACTF128( [yPointer + 32 * i + 16], ymm_cos, 1 )

def Cos_V64f_V64f(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-sysv', 'x64-ms']:
		if module == 'Math':
			if function == 'Cos':
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not x_type.is_floating_point() or not y_type.is_floating_point():
					return

				x_size = x_type.get_size(codegen.abi)
				y_size = y_type.get_size(codegen.abi)

				if x_size != 8 or y_size != 8:
					return

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )

					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_COS_SSE, BATCH_COS_SSE, None, xPointer, yPointer, length, 32, 16 * 5, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )

					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )

					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_COS_AVX, BATCH_COS_AVX, None, xPointer, yPointer, length, 32, 32 * 5, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )

					Map_Vf_Vf(SCALAR_COS_AVX, BATCH_COS_Bulldozer, None, xPointer, yPointer, length, 16, 16 * 5, 8)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )

					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )

					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					Map_Vf_Vf(SCALAR_COS_AVX, BATCH_COS_AVX, None, xPointer, yPointer, length, 32, 32 * 5, 8)

def SCALAR_TAN_AVX(xPointer, yPointer, is_prologue):
	# x = *xPointer
	xmm_x = SSERegister()
	VMOVSD( xmm_x, [xPointer] )
	# t = (x * (2/Pi)) + magic_bias
	xmm_t = SSERegister()
	ASSUME.INITIALIZED( xmm_t )
	VMULSD( xmm_t, xmm_x, Constant.float64(TrigReduction.two_over_pi) )
	VADDSD( xmm_t, xmm_t, Constant.float64(Tan.magic_bias) )
	# n = (int(t) % 4) << 63
	xmm_n = SSERegister()
	VPSLLQ( xmm_n, xmm_t, 63 )
	# t -= magic_bias
	VSUBSD( xmm_t, xmm_t, Constant.float64(Tan.magic_bias) )
	# x += t * (-pi/2)_hi
	xmm_temp = SSERegister()
	ASSUME.INITIALIZED( xmm_temp )
	VMULSD( xmm_temp, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_hi) )
	VADDSD( xmm_x, xmm_x, xmm_temp )
	# x += t * (-pi/2)_me
	xmm_a = SSERegister()
	VMOVAPS( xmm_a, xmm_x )
	xmm_temp = SSERegister()
	ASSUME.INITIALIZED( xmm_temp )
	VMULSD( xmm_temp, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_me) )
	VADDSD( xmm_x, xmm_temp )
	xmm_z = SSERegister()
	ASSUME.INITIALIZED( xmm_z )
	VSUBSD( xmm_z, xmm_x, xmm_a )
	VSUBSD( xmm_temp, xmm_temp, xmm_z )
	# x += t * (-pi/2)_lo
	VMULSD( xmm_t, xmm_t, Constant.float64(TrigReduction.minus_pi_o2_lo) )
	VADDSD( xmm_t, xmm_t, xmm_temp )
	VADDSD( xmm_x, xmm_x, xmm_t )

	# sqrx = x * x
	xmm_sqrx = SSERegister()
	ASSUME.INITIALIZED( xmm_sqrx )
	VMULSD( xmm_sqrx, xmm_x, xmm_x )

	xmm_sin = SSERegister()
	xmm_cos = SSERegister()
	LOAD.CONSTANT( xmm_cos, Constant.float64(Cos.c14) )
	LOAD.CONSTANT( xmm_sin, Constant.float64(Sin.c13) )

	def HORNER_STEP(acc, coef):
		VFMADDSD( acc, acc, xmm_sqrx, coef )

	HORNER_STEP(xmm_cos, Constant.float64(Cos.c12) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c11) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c10) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c9) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c8) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c7) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c6) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c5) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c4) )
	HORNER_STEP(xmm_sin, Constant.float64(Sin.c3) )
	HORNER_STEP(xmm_cos, Constant.float64(Cos.c2) )
	VMULSD( xmm_sin, xmm_sin, xmm_sqrx )
	VFMADDSD( xmm_cos, xmm_cos, xmm_sqrx, Constant.float64(Cos.c0) )
	VFMADDSD( xmm_sin, xmm_sin, xmm_x, xmm_x )

	xmm_p = SSERegister()
	xmm_q = SSERegister()
	VXORPD( xmm_sin, xmm_sin, xmm_n )
	VBLENDVPD( xmm_p, xmm_sin, xmm_cos, xmm_n )
	VBLENDVPD( xmm_q, xmm_cos, xmm_sin, xmm_n )
	xmm_tan = SSERegister()
	# VDIVPD( xmm_tan, xmm_p, xmm_q )

	xmm_exponent_mask = SSERegister()
	LOAD.CONSTANT(xmm_exponent_mask, Constant.float64(Tan.exponent_mask) )

	xmm_q_exponent = SSERegister()
	VANDPD( xmm_q_exponent, xmm_q, xmm_exponent_mask )
	VXORPD( xmm_q_exponent, xmm_q_exponent, xmm_exponent_mask )
	VMULSD( xmm_q_exponent, Constant.float64(Tan.half) )
	xmm_q_mantissa = SSERegister()
	VANDNPD( xmm_q_mantissa, xmm_exponent_mask, xmm_q )
	VORPS( xmm_q_mantissa, xmm_q_mantissa, Constant.float64x2(Tan.one) )

	xmm_q_mantissa_32f = SSERegister()
	VCVTPD2PS( xmm_q_mantissa_32f, xmm_q_mantissa )
	VRCPPS( xmm_q_mantissa_32f, xmm_q_mantissa_32f )
	xmm_q_mantissa_reciprocal = SSERegister()
	VCVTPS2PD( xmm_q_mantissa_reciprocal, xmm_q_mantissa_32f )

	xmm_epsilon = SSERegister()
	ASSUME.INITIALIZED( xmm_epsilon )
	VFNMADDSD( xmm_epsilon, xmm_q_mantissa_reciprocal, xmm_q_mantissa, Constant.float64(Tan.one) )
	VFMADDSD( xmm_q_mantissa_reciprocal, xmm_q_mantissa_reciprocal, xmm_epsilon, xmm_q_mantissa_reciprocal )
	VFNMADDSD( xmm_epsilon, xmm_q_mantissa_reciprocal, xmm_q_mantissa, Constant.float64(Tan.one) )
	VFMADDSD( xmm_q_mantissa_reciprocal, xmm_q_mantissa_reciprocal, xmm_epsilon, xmm_q_mantissa_reciprocal )
	VFNMADDSD( xmm_epsilon, xmm_q_mantissa_reciprocal, xmm_q_mantissa, Constant.float64(Tan.one) )
	VFMADDSD( xmm_q_mantissa_reciprocal, xmm_q_mantissa_reciprocal, xmm_epsilon, xmm_q_mantissa_reciprocal )

	xmm_q_reciprocal = SSERegister()
	ASSUME.INITIALIZED( xmm_q_reciprocal )
	VMULSD( xmm_q_reciprocal, xmm_q_mantissa_reciprocal, xmm_q_exponent )

	ASSUME.INITIALIZED( xmm_tan )
	VMULSD( xmm_tan, xmm_p, xmm_q_reciprocal )
	VFNMADDSD( xmm_epsilon, xmm_q, xmm_tan, xmm_p )
	VFMADDSD( xmm_tan, xmm_epsilon, xmm_q_reciprocal, xmm_tan )

	VMOVSD( [yPointer], xmm_tan )

def Tan_V64f_V64f_Bulldozer(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-sysv', 'x64-ms']:
		if module == 'Math':
			if function == 'Tan':
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not x_type.is_floating_point() or not y_type.is_floating_point():
					return

				x_size = x_type.get_size(codegen.abi)
				y_size = y_type.get_size(codegen.abi)

				if x_size != 8 or y_size != 8:
					return

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache):
					xPointer = GeneralPurposeRegister64()
					yPointer = GeneralPurposeRegister64()
					length = GeneralPurposeRegister64()
	
					LOAD.PARAMETER( xPointer, x_argument )
					LOAD.PARAMETER( yPointer, y_argument )
					LOAD.PARAMETER( length, length_argument )
	
					def BATCH_TAN_FULL(xPointer, yPointer):
						ymm_x = [AVXRegister() for i in range(5)]
						ymm_t = [AVXRegister() for i in range(5)]
						n = [LocalVariable(AVXRegister) for i in range(5)]
						x = [LocalVariable(AVXRegister) for i in range(5)]
	
						ymm_two_over_pi = AVXRegister()
						ymm_magic_bias = AVXRegister()
						LOAD.CONSTANT( ymm_two_over_pi, Constant.float64x4(TrigReduction.two_over_pi) )
						LOAD.CONSTANT( ymm_magic_bias, Constant.float64x4(Tan.magic_bias) )
						for i in range(5):
							# x = *xPointer
							VMOVAPD( ymm_x[i], [xPointer + i * 32] )
							# t = (x * (2/Pi)) + magicBias
							VMULPD( ymm_t[i], ymm_x[i], ymm_two_over_pi )
							VADDPD( ymm_t[i], ymm_t[i], ymm_magic_bias )
	
						for i in range(5):
							xmm_n_lo = SSERegister()
							xmm_n_hi = SSERegister()
							# n = (int(t) % 4) << 62
							VEXTRACTF128( xmm_n_hi, ymm_t[i], 1 )
							VPSLLQ( xmm_n_lo, ymm_t[i].get_oword(), 63 )
							VMOVDQA( n[i].get_low(), xmm_n_lo )
							VPSLLQ( xmm_n_hi, xmm_n_hi, 63 )
							VMOVDQA( n[i].get_high(), xmm_n_hi )
							# t -= magicBias
							VSUBPD( ymm_t[i], ymm_t[i], ymm_magic_bias )
	
						for i in range(5):
							# x += t * (-pi/2)_hi
							ymm_temp = AVXRegister()
							VMULPD( ymm_temp, ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_hi) )
							VADDPD( ymm_x[i], ymm_x[i], ymm_temp )
	
						for i in range(5):
							ymm_temp = AVXRegister()
							# x += t * (-pi/2)_me
							ymm_a = ymm_x[i]
							ymm_x[i] = AVXRegister()
							VMULPD( ymm_temp, ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_me) )
							VADDPD( ymm_x[i], ymm_a, ymm_temp )
							ymm_z = AVXRegister()
							VSUBPD( ymm_z, ymm_x[i], ymm_a )
							VSUBPD( ymm_temp, ymm_temp, ymm_z )
							# x += t * (-pi/2)_lo
							VMULPD( ymm_t[i], ymm_t[i], Constant.float64x4(TrigReduction.minus_pi_o2_lo) )
							VADDPD( ymm_t[i], ymm_t[i], ymm_temp )
							VADDPD( ymm_x[i], ymm_x[i], ymm_t[i] )
	
						ymm_sqrx = [AVXRegister() for i in range(5)]
						for i in range(5):
							# sqrx = x * x
							VMOVAPD( x[i], ymm_x[i] )
							VMULPD( ymm_sqrx[i], ymm_x[i], ymm_x[i] )
	
	
						ymm_sin = [AVXRegister() for i in range(5)]
						ymm_cos = [AVXRegister() for i in range(5)]
	
						ymm_c14 = AVXRegister()
						ymm_c12 = AVXRegister()
						LOAD.CONSTANT( ymm_c14, Constant.float64x4(Cos.c14) )
						LOAD.CONSTANT( ymm_c12, Constant.float64x4(Cos.c12) )
						for i in range(5):
							VFMADDPD( ymm_cos[i], ymm_c14, ymm_sqrx[i], ymm_c12 )
	
						ymm_c13 = AVXRegister()
						ymm_c11 = AVXRegister()
						LOAD.CONSTANT( ymm_c13, Constant.float64x4(Sin.c13) )
						LOAD.CONSTANT( ymm_c11, Constant.float64x4(Sin.c11) )
						for i in range(5):
							VFMADDPD( ymm_sin[i], ymm_c13, ymm_sqrx[i], ymm_c11 )
	
						def HORNER_STEP(acc, coef):
							ymm_c = AVXRegister()
							LOAD.CONSTANT( ymm_c, coef )
							for i in range(5):
								VFMADDPD( acc[i], acc[i], ymm_sqrx[i], ymm_c )
	
						HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c10) )
						HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c9) )
						HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c8) )
						HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c7) )
						HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c6) )
						HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c5) )
						HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c4) )
						HORNER_STEP(ymm_sin, Constant.float64x4(Sin.c3) )
						HORNER_STEP(ymm_cos, Constant.float64x4(Cos.c2) )
	
						ymm_c0 = AVXRegister()
						LOAD.CONSTANT( ymm_c0, Constant.float64x4(Cos.c0) )
						for i in range(5):
							VMULPD( ymm_sin[i], ymm_sqrx[i] )
							VFMADDPD( ymm_cos[i], ymm_cos[i], ymm_sqrx[i], ymm_c0 )
							ymm_x = AVXRegister()
							VMOVAPD( ymm_x, x[i] )
							VFMADDPD( ymm_sin[i], ymm_sin[i], ymm_x, ymm_x )
	
						ymm_p = [AVXRegister() for i in range(5)]
						ymm_q = [AVXRegister() for i in range(5)]
						for i in range(5):
							ymm_n = AVXRegister()
							VMOVAPD( ymm_n, n[i] )
	
							VXORPD( ymm_sin[i], ymm_sin[i], ymm_n )
							VBLENDVPD( ymm_p[i], ymm_sin[i], ymm_cos[i], ymm_n )
							VBLENDVPD( ymm_q[i], ymm_cos[i], ymm_sin[i], ymm_n )
	
						ymm_exponent_mask = AVXRegister()
						ymm_one = AVXRegister()
						VMOVAPD(ymm_exponent_mask, Constant.float64x4(Tan.exponent_mask) )
						VMOVAPD(ymm_one, Constant.float64x4(Tan.one) )
						for i in range(5):
							ymm_tan = AVXRegister()
							# VDIVPD( ymm_tan, ymm_p[i], ymm_q[i] )
	
							ymm_q_exponent = AVXRegister()
							VANDPD( ymm_q_exponent, ymm_q[i], ymm_exponent_mask )
							VXORPD( ymm_q_exponent, ymm_q_exponent, ymm_exponent_mask )
							VMULPD( ymm_q_exponent, Constant.float64x4(Tan.half) )
							ymm_q_mantissa = AVXRegister()
							VANDNPD( ymm_q_mantissa, ymm_exponent_mask, ymm_q[i] )
							VORPS( ymm_q_mantissa, ymm_q_mantissa, ymm_one )
	
							xmm_q_mantissa_32f = SSERegister()
							VCVTPD2PS( xmm_q_mantissa_32f, ymm_q_mantissa )
							VRCPPS( xmm_q_mantissa_32f, xmm_q_mantissa_32f )
							ymm_q_mantissa_reciprocal = AVXRegister()
							VCVTPS2PD( ymm_q_mantissa_reciprocal, xmm_q_mantissa_32f )
	
							ymm_epsilon = AVXRegister()
							VFNMADDPD( ymm_epsilon, ymm_q_mantissa_reciprocal, ymm_q_mantissa, ymm_one )
							VFMADDPD( ymm_q_mantissa_reciprocal, ymm_q_mantissa_reciprocal, ymm_epsilon, ymm_q_mantissa_reciprocal )
							VFNMADDPD( ymm_epsilon, ymm_q_mantissa_reciprocal, ymm_q_mantissa, ymm_one )
							VFMADDPD( ymm_q_mantissa_reciprocal, ymm_q_mantissa_reciprocal, ymm_epsilon, ymm_q_mantissa_reciprocal )
							VFNMADDPD( ymm_epsilon, ymm_q_mantissa_reciprocal, ymm_q_mantissa, ymm_one )
							VFMADDPD( ymm_q_mantissa_reciprocal, ymm_q_mantissa_reciprocal, ymm_epsilon, ymm_q_mantissa_reciprocal )
	
							ymm_q_reciprocal = AVXRegister()
							VMULPD( ymm_q_reciprocal, ymm_q_mantissa_reciprocal, ymm_q_exponent )
	
							VMULPD( ymm_tan, ymm_p[i], ymm_q_reciprocal )
							VFNMADDPD( ymm_epsilon, ymm_q[i], ymm_tan, ymm_p[i] )
							VFMADDPD( ymm_tan, ymm_epsilon, ymm_q_reciprocal, ymm_tan )
	
							VMOVUPD( [yPointer + 32 * i], ymm_tan.get_oword() )
							VEXTRACTF128( [yPointer + 32 * i + 16], ymm_tan, 1 )
	
					Map_Vf_Vf(SCALAR_TAN_AVX, BATCH_TAN_FULL, None, xPointer, yPointer, length, 32, 32 * 5, 8)

def SCALAR_POLYNOMIAL_EVALUATION_SSE(cPointer, xPointer, yPointer, count, is_prologue, element_size):
	scalar_polevl_finish = Label("prologue_scalar_polevl_finish" if is_prologue else "epilogue_scalar_polevl_finish")
	scalar_polevl_next   = Label("prologue_scalar_polevl_next" if is_prologue else "epilogue_scalar_polevl_next")
	MOV_SCALAR = {4: MOVSS, 8: MOVSD }[element_size]
	MUL_SCALAR = {4: MULSS, 8: MULSD }[element_size]
	ADD_SCALAR = {4: ADDSS, 8: ADDSD }[element_size]
	
	xmm_x = SSERegister()
	MOV_SCALAR( xmm_x, [xPointer] )
	
	ccPointer = GeneralPurposeRegister64()
	LEA( ccPointer, [cPointer + count * element_size - element_size] )
	
	xmm_y = SSERegister()
	MOV_SCALAR( xmm_y, [ccPointer] )
	
	SUB( ccPointer, element_size )
	CMP( ccPointer, cPointer )
	JB( scalar_polevl_finish )
	
	LABEL( scalar_polevl_next )
	MUL_SCALAR( xmm_y, xmm_x )
	ADD_SCALAR( xmm_y, [ccPointer] )
	SUB( ccPointer, element_size )
	CMP( ccPointer, cPointer )
	JAE( scalar_polevl_next )
	
	LABEL( scalar_polevl_finish )
	MOV_SCALAR( [yPointer], xmm_y )

def SCALAR_POLYNOMIAL_EVALUATION_AVX(cPointer, xPointer, yPointer, count, is_prologue, element_size):
	scalar_polevl_finish = Label("prologue_scalar_polevl_finish" if is_prologue else "epilogue_scalar_polevl_finish")
	scalar_polevl_next   = Label("prologue_scalar_polevl_next" if is_prologue else "epilogue_scalar_polevl_next")
	
	MOV_SCALAR = {4: VMOVSS, 8: VMOVSD }[element_size]
	MUL_SCALAR = {4: VMULSS, 8: VMULSD }[element_size]
	ADD_SCALAR = {4: VADDSS, 8: VADDSD }[element_size]
	if Target.has_fma4():
		FMA_SCALAR = {4: VFMADDSS, 8: VFMADDSD }[element_size]
	elif Target.has_fma3():
		FMA_SCALAR = {4: VFMADD213SS, 8: VFMADD213SD }[element_size]
	
	xmm_x = SSERegister()
	MOV_SCALAR( xmm_x, [xPointer] )
	
	ccPointer = GeneralPurposeRegister64()
	LEA( ccPointer, [cPointer + count * element_size - element_size] )
	
	xmm_y = SSERegister()
	MOV_SCALAR( xmm_y, [ccPointer] )
	
	SUB( ccPointer, element_size )
	CMP( ccPointer, cPointer )
	JB( scalar_polevl_finish )
	
	LABEL( scalar_polevl_next )
	if Target.has_fma():
		FMA_SCALAR( xmm_y, xmm_y, xmm_x, [ccPointer] )
	else:
		MUL_SCALAR( xmm_y, xmm_y, xmm_x )
		ADD_SCALAR( xmm_y, xmm_y, [ccPointer] )

	SUB( ccPointer, element_size )
	CMP( ccPointer, cPointer )
	JAE( scalar_polevl_next )
	
	LABEL( scalar_polevl_finish )
	MOV_SCALAR( [yPointer], xmm_y )

def EvaluatePolynomial_VfVf_Vf_SSE(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ('x64-sysv', 'x64-ms'):
		if module == 'Math':
			if function == 'EvaluatePolynomial':
				c_argument, x_argument, y_argument, count_argument, length_argument = tuple(arguments)

				c_type = c_argument.get_type().get_primitive_type()
				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (c_type, x_type, y_type)):
					return

				if len(set([type.get_size() for type in (c_type, x_type, y_type)])) != 1:
					return
				else:
					element_size = x_type.get_size()

				if element_size == 4:
					def SIMD_DUPLICATE( xmm_dest, mem_src ):
						MOVSS( xmm_dest, mem_src )
						SHUFPS( xmm_dest, xmm_dest, 0 )
				else:
					def SIMD_DUPLICATE( xmm_dest, mem_src ):
						if Target.has_sse3():
							MOVDDUP( xmm_dest, mem_src )
						else:
							MOVSD( xmm_dest, mem_src )
							UNPCKLPD( xmm_dest, xmm_dest )
				SIMD_LOAD      = { 4: MOVAPS, 8: MOVAPD }[element_size]
				SIMD_MOV       = { 4: MOVAPS, 8: MOVAPD }[element_size]
				SIMD_STORE     = { 4: MOVUPS, 8: MOVUPD }[element_size]
				SIMD_MUL       = { 4: MULPS, 8: MULPD }[element_size] 
				SIMD_ADD       = { 4: ADDPS, 8: ADDPD }[element_size] 

				with Function(codegen, function_signature, arguments, 'Unknown', assembly_cache = assembly_cache):
					cPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( cPointer, c_argument )
	
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
					
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					count = GeneralPurposeRegister64()
					LOAD.PARAMETER( count, count_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer, is_prologue):
						SCALAR_POLYNOMIAL_EVALUATION_SSE(cPointer, xPointer, yPointer, count, is_prologue, element_size)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						ccPointer = GeneralPurposeRegister64()
						LEA( ccPointer, [cPointer + count * element_size - element_size] )
	
						xmm_y = [SSERegister() for i in range(6)]
						SIMD_DUPLICATE( xmm_y[0], [ccPointer] )
						for i in range(1, 6):
							SIMD_MOV( xmm_y[i], xmm_y[0] )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						xmm_c = SSERegister()
						SIMD_DUPLICATE( xmm_c, [ccPointer] )
						xmm_x = [SSERegister() for i in range(6)]
						for i in range(6):
							SIMD_LOAD( xmm_x[i], [xPointer + i * 16] )
							SIMD_MUL( xmm_y[i], xmm_x[i] )
							SIMD_ADD( xmm_y[i], xmm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						LABEL( 'batch_polevl_next' )
	
						xmm_c = SSERegister()
						SIMD_DUPLICATE( xmm_c, [ccPointer] )
						for i in range(6):
							SIMD_MUL( xmm_y[i], xmm_x[i] )
							SIMD_ADD( xmm_y[i], xmm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JAE( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						for i in range(6):
							SIMD_STORE( [yPointer + i * 16], xmm_y[i] )
	
					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, None, xPointer, yPointer, length, 16, 16 * 6, element_size)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache):
					cPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( cPointer, c_argument )
	
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
					
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					count = GeneralPurposeRegister64()
					LOAD.PARAMETER( count, count_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer, is_prologue):
						SCALAR_POLYNOMIAL_EVALUATION_SSE(cPointer, xPointer, yPointer, count, is_prologue, element_size)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						ccPointer = GeneralPurposeRegister64()
						LEA( ccPointer, [cPointer + count * element_size - element_size] )
	
						xmm_y = [SSERegister() for i in range(10)]
						SIMD_DUPLICATE( xmm_y[0], [ccPointer] )
						for i in range(1, 10):
							SIMD_MOV( xmm_y[i], xmm_y[0] )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						xmm_c = SSERegister()
						SIMD_DUPLICATE( xmm_c, [ccPointer] )
						x = [SSERegister(), [xPointer + 1 * 16], SSERegister(), [xPointer + 3 * 16], SSERegister(),
							[xPointer + 5 * 16], SSERegister(), [xPointer + 7 * 16], SSERegister(), [xPointer + 9 * 16]]
						for i in range(10):
							if isinstance(x[i], SSERegister):
								SIMD_LOAD( x[i], [xPointer + i * 16] )
							SIMD_MUL( xmm_y[i], x[i] )
							SIMD_ADD( xmm_y[i], xmm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						LABEL( 'batch_polevl_next' )
	
						xmm_c = SSERegister()
						SIMD_DUPLICATE( xmm_c, [ccPointer] )
						for i in range(10):
							SIMD_MUL( xmm_y[i], x[i] )
							SIMD_ADD( xmm_y[i], xmm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JAE( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						for i in range(10):
							SIMD_STORE( [yPointer + i * 16], xmm_y[i] )
	
					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, None, xPointer, yPointer, length, 16, 16 * 10, element_size)


def EvaluatePolynomial_V64fV64f_V64f_Bonnell(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ('x64-sysv', 'x64-ms'):
		if module == 'Math':
			if function == 'EvaluatePolynomial':
				c_argument, x_argument, y_argument, count_argument, length_argument = tuple(arguments)

				c_type = c_argument.get_type().get_primitive_type()
				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (c_type, x_type, y_type)):
					return

				if any([type.get_size() != 8 for type in (c_type, x_type, y_type)]):
					return

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bonnell', assembly_cache = assembly_cache):
					cPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( cPointer, c_argument )
	
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
					
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					count = GeneralPurposeRegister64()
					LOAD.PARAMETER( count, count_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer, is_prologue):
						SCALAR_POLYNOMIAL_EVALUATION_SSE(cPointer, xPointer, yPointer, count, is_prologue, 8)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						x = [SSERegister()] + [[xPointer + i * 8] for i in range(1, 14)]
	
						ccPointer = GeneralPurposeRegister64()
						LEA( ccPointer, [cPointer + count * 8 - 8] )
	
						xmm_y = [SSERegister() for i in range(14)]
						MOVSD( xmm_y[0], [ccPointer] )
						for i in range(14):
							if i != 0:
								MOVAPS( xmm_y[i], xmm_y[0] )
							if isinstance(x[i], SSERegister):
								MOVSD( x[i], [xPointer + i * 8] )
	
						SUB( ccPointer, 8 )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						LABEL( 'batch_polevl_next' )
	
						xmm_c = SSERegister()
						MOVSD( xmm_c, [ccPointer] )
						MULSD( xmm_y[0], x[0] )
						MULSD( xmm_y[1], x[1] )
						for i in range(2, 14):
							MULSD( xmm_y[i], x[i] )
							ADDSD( xmm_y[i - 2], xmm_c )
	
						SUB( ccPointer, 8 )
						ADDSD( xmm_y[12], xmm_c )
						CMP( ccPointer, cPointer )
						ADDSD( xmm_y[13], xmm_c )
						JAE( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						for i in range(14):
							MOVSD( [yPointer + i * 8], xmm_y[i] )
	
					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, None, xPointer, yPointer, length, 16, 8 * 14, 8)

def EvaluatePolynomial_V32fV32f_V32f_Bonnell(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ('x64-sysv', 'x64-ms'):
		if module == 'Math':
			if function == 'EvaluatePolynomial':
				c_argument, x_argument, y_argument, count_argument, length_argument = tuple(arguments)

				c_type = c_argument.get_type().get_primitive_type()
				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (c_type, x_type, y_type)):
					return

				if any([type.get_size() != 4 for type in (c_type, x_type, y_type)]):
					return

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bonnell', assembly_cache = assembly_cache):
					cPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( cPointer, c_argument )
	
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
					
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					count = GeneralPurposeRegister64()
					LOAD.PARAMETER( count, count_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer, is_prologue):
						SCALAR_POLYNOMIAL_EVALUATION_SSE(cPointer, xPointer, yPointer, count, is_prologue, 4)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						x = [SSERegister()] + [[xPointer + i * 16] for i in range(1, 14)]
	
						ccPointer = GeneralPurposeRegister64()
						LEA( ccPointer, [cPointer + count * 4 - 4] )
	
						xmm_y = [SSERegister() for i in range(14)]
						MOVSS( xmm_y[0], [ccPointer] )
						SHUFPS( xmm_y[0], xmm_y[0], 0x00 )
						for i in range(14):
							if isinstance(x[i], SSERegister):
								MOVAPS( x[i], [xPointer + i * 16] )
							if i != 0:
								MOVAPS( xmm_y[i], xmm_y[0] )
	
						SUB( ccPointer, 4 )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						LABEL( 'batch_polevl_next' )
	
						xmm_c = SSERegister()
						MOVSS( xmm_c, [ccPointer] )
						MULPS( xmm_y[0], x[0] )
						SHUFPS( xmm_c, xmm_c, 0x00 )
						MULPS( xmm_y[1], x[1] )
						for i in range(2, 14):
							MULPS( xmm_y[i], x[i] )
							ADDPS( xmm_y[i - 2], xmm_c )
	
						SUB( ccPointer, 4 )
						ADDPS( xmm_y[12], xmm_c )
						CMP( ccPointer, cPointer )
						ADDPS( xmm_y[13], xmm_c )
						JAE( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						for i in range(14):
							MOVUPS( [yPointer + i * 16], xmm_y[i] )
	
					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, None, xPointer, yPointer, length, 16, 16 * 14, 4)

def EvaluatePolynomial_VfVf_Vf_AVX(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ('x64-sysv', 'x64-ms'):
		if module == 'Math':
			if function == 'EvaluatePolynomial':
				c_argument, x_argument, y_argument, count_argument, length_argument = tuple(arguments)

				c_type = c_argument.get_type().get_primitive_type()
				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (c_type, x_type, y_type)):
					return

				if len(set([type.get_size() for type in (c_type, x_type, y_type)])) != 1:
					return
				else:
					element_size = x_type.get_size()

				SIMD_BROADCAST = { 4: VBROADCASTSS, 8: VBROADCASTSD }[element_size]
				SIMD_LOAD      = { 4: VMOVAPS, 8: VMOVAPD }[element_size]
				SIMD_MOV       = { 4: VMOVAPS, 8: VMOVAPD }[element_size]
				SIMD_STORE     = { 4: VMOVUPS, 8: VMOVUPD }[element_size] 
				SIMD_MUL       = { 4: VMULPS, 8: VMULPD }[element_size] 
				SIMD_ADD       = { 4: VADDPS, 8: VADDPD }[element_size] 
				SIMD_FMA4      = { 4: VFMADDPS, 8: VFMADDPD }[element_size] 
				SIMD_FMA3      = { 4: VFMADD132PS, 8: VFMADD132PD }[element_size] 

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache):
					cPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( cPointer, c_argument )
	
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
					
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					count = GeneralPurposeRegister64()
					LOAD.PARAMETER( count, count_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer, is_prologue):
						SCALAR_POLYNOMIAL_EVALUATION_AVX(cPointer, xPointer, yPointer, count, is_prologue, element_size)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						ccPointer = GeneralPurposeRegister64()
						LEA( ccPointer, [cPointer + count * element_size - element_size] )
	
						ymm_y = [SSERegister(), SSERegister(), AVXRegister(), AVXRegister(), AVXRegister(), AVXRegister()]
						SIMD_BROADCAST( ymm_y[5], [ccPointer] )
						for i in range(5):
							if isinstance(ymm_y[i], SSERegister):
								SIMD_MOV( ymm_y[i], ymm_y[5].get_oword() )
							else:
								SIMD_MOV( ymm_y[i], ymm_y[5] )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						ymm_c = AVXRegister()
						SIMD_BROADCAST( ymm_c, [ccPointer] )
						ymm_x = [SSERegister(), SSERegister(), AVXRegister(), AVXRegister(), AVXRegister(), AVXRegister()]
						xOffset = 0
						for i in range(6):
							SIMD_LOAD( ymm_x[i], [xPointer + xOffset] )
							xOffset += ymm_x[i].size
							if isinstance(ymm_y[i], SSERegister):
								SIMD_FMA4( ymm_y[i], ymm_y[i], ymm_x[i], ymm_c.get_oword() )
							else:
								SIMD_FMA4( ymm_y[i], ymm_y[i], ymm_x[i], ymm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						ALIGN( 16 )
						LABEL( 'batch_polevl_next' )
	
						ymm_c = AVXRegister()
						SIMD_BROADCAST( ymm_c, [ccPointer] )
						for i in range(6):
							if isinstance(ymm_y[i], SSERegister):
								SIMD_FMA4( ymm_y[i], ymm_y[i], ymm_x[i], ymm_c.get_oword() )
							else:
								SIMD_FMA4( ymm_y[i], ymm_y[i], ymm_x[i], ymm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JAE( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						yOffset = 0
						for i in range(6):
							if isinstance( ymm_y[i], SSERegister):
								SIMD_STORE( [yPointer + yOffset], ymm_y[i] )
							else:
								SIMD_STORE( [yPointer + yOffset], ymm_y[i].get_oword() )
								VEXTRACTF128( [yPointer + yOffset + 16], ymm_y[i], 1 )
							yOffset += ymm_y[i].size
	
					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, None, xPointer, yPointer, length, 32, 160, element_size)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache):
					cPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( cPointer, c_argument )
	
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					count = GeneralPurposeRegister64()
					LOAD.PARAMETER( count, count_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer, is_prologue):
						SCALAR_POLYNOMIAL_EVALUATION_AVX(cPointer, xPointer, yPointer, count, is_prologue, element_size)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						ccPointer = GeneralPurposeRegister64()
						LEA( ccPointer, [cPointer + count * element_size - element_size] )
	
						ymm_y = [AVXRegister() for i in range(8)]
						SIMD_BROADCAST( ymm_y[0], [ccPointer] )
						for i in range(1, 8):
							SIMD_MOV( ymm_y[i], ymm_y[0] )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						ymm_c = AVXRegister()
						SIMD_BROADCAST( ymm_c, [ccPointer] )
						x = [AVXRegister() for i in range(3)] + [[xPointer + 3 * 32]] + [AVXRegister() for i in range(4)]
						for i in range(8):
							if isinstance(x[i], AVXRegister):
								SIMD_LOAD( x[i], [xPointer + i * 32] )
							SIMD_MUL( ymm_y[i], ymm_y[i], x[i] )
							SIMD_ADD( ymm_y[i], ymm_y[i], ymm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						LABEL( 'batch_polevl_next' )
	
						ymm_c = AVXRegister()
						SIMD_BROADCAST( ymm_c, [ccPointer] )
						for i in range(8):
							SIMD_MUL( ymm_y[i], ymm_y[i], x[i] )
							SIMD_ADD( ymm_y[i], ymm_y[i], ymm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JAE( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						for i in range(8):
							SIMD_STORE( [yPointer + i * 32], ymm_y[i] )
	
					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, None, xPointer, yPointer, length, 32, 32 * 8, element_size)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache):
					cPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( cPointer, c_argument )
	
					xPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( xPointer, x_argument )
	
					yPointer = GeneralPurposeRegister64()
					LOAD.PARAMETER( yPointer, y_argument )
	
					count = GeneralPurposeRegister64()
					LOAD.PARAMETER( count, count_argument )
	
					length = GeneralPurposeRegister64()
					LOAD.PARAMETER( length, length_argument )
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer, is_prologue):
						SCALAR_POLYNOMIAL_EVALUATION_AVX(cPointer, xPointer, yPointer, count, is_prologue, element_size)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						ccPointer = GeneralPurposeRegister64()
						LEA( ccPointer, [cPointer + count * element_size - element_size] )
	
						ymm_y = [AVXRegister() for i in range(10)]
						SIMD_BROADCAST( ymm_y[0], [ccPointer] )
						for i in range(10):
							SIMD_MOV( ymm_y[i], ymm_y[0] )
						PREFETCHNTA( [xPointer + 768] )
						PREFETCHNTA( [xPointer + 768 + 32] )
						PREFETCHNTA( [xPointer + 768 + 64] )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						ymm_c = AVXRegister()
						SIMD_BROADCAST( ymm_c, [ccPointer] )
						x = [AVXRegister(), [xPointer + 1 * 32], AVXRegister(), [xPointer + 3 * 32], AVXRegister(),
							[xPointer + 5 * 32], AVXRegister(), [xPointer + 7 * 32], AVXRegister(), [xPointer + 9 * 32]]
						for i in range(10):
							if isinstance(x[i], AVXRegister):
								SIMD_LOAD( x[i], [xPointer + i * 32] )
							SIMD_FMA3( ymm_y[i], ymm_y[i], x[i], ymm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JB( 'batch_polevl_finish' )
	
						ALIGN( 16 )
						LABEL( 'batch_polevl_next' )
	
						ymm_c = AVXRegister()
						SIMD_BROADCAST( ymm_c, [ccPointer] )
						for i in range(10):
							SIMD_FMA3( ymm_y[i], ymm_y[i], x[i], ymm_c )
	
						SUB( ccPointer, element_size )
						CMP( ccPointer, cPointer )
						JAE( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						for i in range(10):
							SIMD_STORE( [yPointer + i * 32], ymm_y[i] )
	
					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, None, xPointer, yPointer, length, 32, 32 * 10, element_size)

