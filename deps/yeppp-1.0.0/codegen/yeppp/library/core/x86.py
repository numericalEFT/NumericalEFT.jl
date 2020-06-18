#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

__author__ = 'Marat'

from peachpy.x86 import *

def AddSub_VusVus_Vus_implementation(codegen, function_signature, module, function, inputs, outputs, arguments):
	if codegen.abi.name in ['x86']:
		if module == 'Core':
			if function in ['Add', 'Subtract', 'Multiply']:
				x_argument = arguments[0]
				y_argument = arguments[1]
				sum_argument = arguments[2]
				length_argument = arguments[3]

				x_size = x_argument.size
				y_size = y_argument.size
				sum_size = sum_argument.size

				if function == 'Multiply':
					if x_size == 8 or sum_size == 64:
						return

				parameters = [x_argument.name, y_argument.name, sum_argument.name, length_argument.name]
				codegen.begin_function(function_signature, parameters, 'Nehalem')
				xPointer = edx
				yPointer = esi
				sumPointer = edi
				length = ecx
				accumulator = eax
				accumulator_long = ebx
				temp = ebp

				LOAD.PARAMETER( xPointer, x_argument.name )
				LOAD.PARAMETER( yPointer, y_argument.name )
				LOAD.PARAMETER( sumPointer, sum_argument.name )
				LOAD.PARAMETER( length, length_argument.name )

				def process_scalar():
					if sum_size == 64:
						if x_size == 64:
							MOV( accumulator, [xPointer] )
							MOV( accumulator_long, [xPointer + 4] )
							if function == 'Add':
								ADD( accumulator, [yPointer] )
								ADC( accumulator_long, [yPointer + 4] )
							elif function == 'Subtract':
								SUB( accumulator, [yPointer] )
								SBB( accumulator_long, [yPointer + 4] )
						elif x_size == 32 and x_argument.is_unsigned_integer:
							if function == 'Add':
								MOV( accumulator, [xPointer] )
								ADD( accumulator, [yPointer] )
								SETC( accumulator_long.get_low_byte() )
								MOVZX( accumulator_long, accumulator_long.get_low_byte() )
							elif function == 'Subtract':
								MOV( accumulator, [xPointer] )
								XOR( accumulator_long, accumulator_long )
								SUB( accumulator, [yPointer] )
								SBB( accumulator_long, 0 )
						elif x_size == 32 and x_argument.is_signed_integer:
							MOV( accumulator, [xPointer] )
							MOV( accumulator_long, accumulator )
							SAR( accumulator_long, 31 )
							MOV( temp, [yPointer] )
							SAR( temp, 31 )
							if function == 'Add':
								ADD( accumulator, [yPointer] )
								ADC( accumulator_long, temp )
							elif function == 'Subtract':
								SUB( accumulator, [yPointer] )
								SBB( accumulator_long, temp )
						MOV( [sumPointer], accumulator )
						MOV( [sumPointer + 4], accumulator_long )
					else:
						size_map = { 8: byte, 16: word, 32: dword }
						if x_argument.is_signed_integer:
							LOAD_map = { 8: MOVSX, 16: MOVSX, 32: MOV }
						else:
							LOAD_map = { 8: MOVZX, 16: MOVZX, 32: MOV }
						LOAD = LOAD_map[x_size]
						size = size_map[x_size]

						LOAD( accumulator, size[xPointer] )
						LOAD( temp, size[yPointer] )
						if function == 'Add':
							ADD( accumulator, temp )
						elif function == 'Subtract':
							SUB( accumulator, temp )
						elif function == 'Multiply':
							IMUL( accumulator, temp )
						if sum_size == 8:
							MOV( [sumPointer], accumulator.get_low_byte() )
						elif sum_size == 16:
							MOV( [sumPointer], accumulator.get_word() )
						else:
							MOV( [sumPointer], accumulator )

				LABEL( "dstM16" )
				TEST(sumPointer, 15)
				JZ( "dstA16" )

				process_scalar()
				ADD( xPointer, x_size / 8 )
				ADD( yPointer, y_size / 8 )
				ADD( sumPointer, sum_size / 8 )
				SUB( length, 1 )
				JZ( "return_ok" )
				JMP( "dstM16" )

				LABEL( "dstA16" )
				SUB( length, 64 )
				JB( "dstA16_restore" )
				ALIGN( 16 )
				LABEL( "dstA16_loop" )

				if x_size == sum_size:
					SIMD_LOAD = MOVDQU
					load_increment = 16
				else:
					load_increment = 8
					PMOVSX_map = { 8: PMOVSXBW, 16: PMOVSXWD, 32: PMOVSXDQ }
					PMOVZX_map = { 8: PMOVZXBW, 16: PMOVZXWD, 32: PMOVZXDQ }
					if x_argument.is_signed_integer:
						SIMD_LOAD = PMOVSX_map[x_argument.size]
					else:
						SIMD_LOAD = PMOVZX_map[x_argument.size]

				if function == 'Add':
					PADD_map = { 8: PADDB, 16: PADDW, 32: PADDD, 64: PADDQ }
					SIMD_COMPUTE = PADD_map[sum_argument.size]
				elif function == 'Subtract':
					PSUB_map = { 8: PSUBB, 16: PSUBW, 32: PSUBD, 64: PSUBQ }
					SIMD_COMPUTE = PSUB_map[sum_argument.size]
				elif function == 'Multiply':
					PMULL_map = { 16: PMULLW, 32: PMULLD }
					SIMD_COMPUTE = PMULL_map[sum_argument.size]

				SIMD_STORE = MOVDQA

				SIMD_LOAD( xmm0, [xPointer] )
				SIMD_LOAD( xmm4, [yPointer] )
				SIMD_COMPUTE( xmm0, xmm4 )
				SIMD_STORE( [sumPointer], xmm0 )

				SIMD_LOAD( xmm1, [xPointer + load_increment] )
				SIMD_LOAD( xmm5, [yPointer + load_increment] )
				SIMD_COMPUTE( xmm1, xmm5 )
				SIMD_STORE( [sumPointer + 16], xmm1 )

				SIMD_LOAD( xmm2, [xPointer + load_increment * 2] )
				SIMD_LOAD( xmm6, [yPointer + load_increment * 2] )
				SIMD_COMPUTE( xmm2, xmm6 )
				SIMD_STORE( [sumPointer + 32], xmm2 )

				SIMD_LOAD( xmm3, [xPointer + load_increment * 3] )
				SIMD_LOAD( xmm7, [yPointer + load_increment * 3] )
				SIMD_COMPUTE( xmm3, xmm7 )
				SIMD_STORE( [sumPointer + 48], xmm3 )

				ADD( xPointer, load_increment * 4 )
				ADD( yPointer, load_increment * 4 )
				ADD( sumPointer, 64 )
				SUB( length, 512 / sum_size )
				JAE( "dstA16_loop" )

				LABEL( "dstA16_restore" )
				ADD( length, 64 )
				JZ( "return_ok" )
				LABEL( "finalize" )

				process_scalar()
				ADD( xPointer, x_size / 8 )
				ADD( yPointer, y_size / 8 )
				ADD( sumPointer, sum_size / 8 )
				SUB( length, 1 )
				JNZ( "finalize" )

				LABEL( "return_ok" )
				XOR(eax, eax)
				LABEL( "return" )
				RET()

				return codegen.end_function()

def AddSubMulMinMax_VfVf_Vf_implementation(codegen, abi, function_signature, module, function, inputs, outputs, arguments):
	if abi in ['x86']:
		if module == 'Core':
			if function in ['Add', 'Subtract', 'Multiply', 'Min', 'Max']:
				x_argument = arguments[0]
				y_argument = arguments[1]
				sum_argument = arguments[2]
				length_argument = arguments[3]

				x_size = x_argument.size
				y_size = y_argument.size
				sum_size = sum_argument.size

				parameters = [x_argument.name, y_argument.name, sum_argument.name, length_argument.name]
				codegen.begin_function(function_signature, parameters, 'Nehalem')
				xPointer = edx
				yPointer = esi
				sumPointer = edi
				length = ecx

				LOAD.PARAMETER( xPointer, x_argument.name )
				LOAD.PARAMETER( yPointer, y_argument.name )
				LOAD.PARAMETER( sumPointer, sum_argument.name )
				LOAD.PARAMETER( length, length_argument.name )

				if x_size == 32:
					SCALAR_LOAD = MOVSS
					SCALAR_COMPUTE_map = { "Add": ADDSS, "Subtract": SUBSS, "Multiply": MULSS, 'Min': MINSS, 'Max': MAXSS }
					SCALAR_STORE = MOVSS
					SIMD_LOAD = MOVUPS
					SIMD_COMPUTE_map = { "Add": ADDPS, "Subtract": SUBPS, "Multiply": MULPS, 'Min': MINPS, 'Max': MAXPS }
					SIMD_STORE = MOVUPS
				else:
					SCALAR_LOAD = MOVSD
					SCALAR_COMPUTE_map = { "Add": ADDSD, "Subtract": SUBSD, "Multiply": MULSD, 'Min': MINSD, 'Max': MAXSD }
					SCALAR_STORE = MOVSD
					SIMD_LOAD = MOVUPD
					SIMD_COMPUTE_map = { "Add": ADDPD, "Subtract": SUBPD, "Multiply": MULPD, 'Min': MINPD, 'Max': MAXPD }
					SIMD_STORE = MOVUPD
				SCALAR_COMPUTE = SCALAR_COMPUTE_map[function]
				SIMD_COMPUTE = SIMD_COMPUTE_map[function]

				def process_scalar():
					SCALAR_LOAD( xmm0, [xPointer] )
					SCALAR_LOAD( xmm1, [yPointer] )
					SCALAR_COMPUTE( xmm0, xmm1 )
					SCALAR_STORE( [sumPointer], xmm0 )

				LABEL( "dstM16" )
				TEST( yPointer, 15)
				JZ( "dstA16" )

				process_scalar()
				ADD( xPointer, x_size / 8 )
				ADD( yPointer, y_size / 8 )
				ADD( sumPointer, sum_size / 8 )
				SUB( length, 1 )
				JZ( "return_ok" )
				JMP( "dstM16" )

				LABEL( "dstA16" )
				SUB( length, 1024 / sum_size )
				JB( "dstA16_restore" )

				SIMD_LOAD( xmm0, [xPointer] )
				SIMD_LOAD( xmm1, [xPointer + 16 * 1] )

				SIMD_LOAD( xmm2, [xPointer + 16 * 2] )
				SIMD_COMPUTE( xmm0, [yPointer] )

				SIMD_LOAD( xmm3, [xPointer + 16 * 3] )
				SIMD_COMPUTE( xmm1, [yPointer + 16 * 1] )

				SIMD_LOAD( xmm4, [xPointer + 16 * 4] )
				SIMD_COMPUTE( xmm2, [yPointer + 16 * 2] )

				SIMD_LOAD( xmm5, [xPointer + 16 * 5] )
				SIMD_COMPUTE( xmm3, [yPointer + 16 * 3] )

				SIMD_LOAD( xmm6, [xPointer + 16 * 6] )
				SIMD_COMPUTE( xmm4, [yPointer + 16 * 4] )

				SIMD_LOAD( xmm7, [xPointer + 16 * 7] )
				SIMD_COMPUTE( xmm5, [yPointer + 16 * 5] )

				ADD( xPointer, 128 )
				SUB( length, 1024 / sum_size )
				JB( "skip_SWP" )

				ALIGN( 16 )
				LABEL( "dstA16_loop" )

				SIMD_STORE( [sumPointer], xmm0 )
				SIMD_LOAD( xmm0, [xPointer] )
				SIMD_COMPUTE( xmm6, [yPointer + 16 * 6] )

				SIMD_STORE( [sumPointer + 16 * 1], xmm1 )
				SIMD_LOAD( xmm1, [xPointer + 16 * 1] )
				SIMD_COMPUTE( xmm7, [yPointer + 16 * 7] )

				ADD( yPointer, 128 )

				SIMD_STORE( [sumPointer + 16 * 2], xmm2 )
				SIMD_LOAD( xmm2, [xPointer + 16 * 2] )
				SIMD_COMPUTE( xmm0, [yPointer] )

				SIMD_STORE( [sumPointer + 16 * 3], xmm3 )
				SIMD_LOAD( xmm3, [xPointer + 16 * 3] )
				SIMD_COMPUTE( xmm1, [yPointer + 16 * 1] )

				SIMD_STORE( [sumPointer + 16 * 4], xmm4 )
				SIMD_LOAD( xmm4, [xPointer + 16 * 4] )
				SIMD_COMPUTE( xmm2, [yPointer + 16 * 2] )

				SIMD_STORE( [sumPointer + 16 * 5], xmm5 )
				SIMD_LOAD( xmm5, [xPointer + 16 * 5] )
				SIMD_COMPUTE( xmm3, [yPointer + 16 * 3] )

				SIMD_STORE( [sumPointer + 16 * 6], xmm6 )
				SIMD_LOAD( xmm6, [xPointer + 16 * 6] )
				SIMD_COMPUTE( xmm4, [yPointer + 16 * 4] )

				SIMD_STORE( [sumPointer + 16 * 7], xmm7 )
				SIMD_LOAD( xmm7, [xPointer + 16 * 7] )
				SIMD_COMPUTE( xmm5, [yPointer + 16 * 5] )

				ADD( xPointer, 128 )
				ADD( sumPointer, 128 )
				SUB( length, 1024 / sum_size )
				JAE( "dstA16_loop" )

				LABEL( "skip_SWP" )

				SIMD_STORE( [sumPointer], xmm0 )
				SIMD_COMPUTE( xmm6, [yPointer + 16 * 6] )

				SIMD_STORE( [sumPointer + 16 * 1], xmm1 )
				SIMD_COMPUTE( xmm7, [yPointer + 16 * 7] )

				SIMD_STORE( [sumPointer + 16 * 2], xmm2 )
				SIMD_STORE( [sumPointer + 16 * 3], xmm3 )
				SIMD_STORE( [sumPointer + 16 * 4], xmm4 )
				SIMD_STORE( [sumPointer + 16 * 5], xmm5 )
				SIMD_STORE( [sumPointer + 16 * 6], xmm6 )
				SIMD_STORE( [sumPointer + 16 * 7], xmm7 )

				ADD( yPointer, 128 )
				ADD( sumPointer, 128 )

				LABEL( "dstA16_restore" )
				ADD( length, 1024 / sum_size )
				JZ( "return_ok" )
				LABEL( "finalize" )

				process_scalar()
				ADD( xPointer, x_size / 8 )
				ADD( yPointer, y_size / 8 )
				ADD( sumPointer, sum_size / 8 )
				SUB( length, 1 )
				JNZ( "finalize" )

				LABEL( "return_ok" )
				XOR(eax, eax)
				LABEL( "return" )
				RET()

				return codegen.end_function()

def Negate_Vf_Vf_implementation(codegen, abi, function_signature, module, function, inputs, outputs, arguments):
	if abi in ['x86']:
		if module == 'Core':
			if function in ['Negate']:
				x_argument = arguments[0]
				y_argument = arguments[1]
				length_argument = arguments[2]

				x_size = x_argument.size
				y_size = y_argument.size
				assert x_size == y_size

				parameters = [x_argument.name, y_argument.name, length_argument.name]
				codegen.begin_function(function_signature, parameters, 'Nehalem')
				xPointer = edx
				yPointer = esi
				length = ecx

				LOAD.PARAMETER( xPointer, x_argument.name )
				LOAD.PARAMETER( yPointer, y_argument.name )
				LOAD.PARAMETER( length, length_argument.name )

				sign_mask = xmm7
				LOAD.CONSTANT( sign_mask, Constant.float64x2(-0.0) )

				if x_size == 32:
					SCALAR_LOAD = MOVSS
					SCALAR_COMPUTE = XORPS
					SCALAR_STORE = MOVSS
					SIMD_LOAD = MOVUPS
					SIMD_COMPUTE = XORPS
					SIMD_STORE = MOVAPS
				else:
					SCALAR_LOAD = MOVSD
					SCALAR_COMPUTE = XORPD
					SCALAR_STORE = MOVSD
					SIMD_LOAD = MOVUPD
					SIMD_COMPUTE = XORPD
					SIMD_STORE = MOVAPD

				def process_scalar():
					SCALAR_LOAD( xmm0, [xPointer] )
					SCALAR_COMPUTE( xmm0, sign_mask )
					SCALAR_STORE( [yPointer], xmm0 )

				LABEL( "dstM16" )
				TEST( yPointer, 15)
				JZ( "dstA16" )

				process_scalar()
				ADD( xPointer, x_size / 8 )
				ADD( yPointer, y_size / 8 )
				SUB( length, 1 )
				JZ( "return_ok" )
				JMP( "dstM16" )

				LABEL( "dstA16" )
				SUB( length, 512 / y_size )
				JB( "dstA16_restore" )

				ALIGN( 16 )
				LABEL( "dstA16_loop" )

				SIMD_LOAD( xmm0, [xPointer] )
				SIMD_LOAD( xmm1, [xPointer + 16] )
				SIMD_LOAD( xmm2, [xPointer + 32] )
				SIMD_LOAD( xmm3, [xPointer + 48] )

				ADD( xPointer, 64 )
				ADD( yPointer, 64 )
				SUB( length, 512 / x_size )
				JAE( "dstA16_loop" )

				LABEL( "dstA16_restore" )
				ADD( length, 512 / x_size )
				JZ( "return_ok" )
				LABEL( "finalize" )

				process_scalar()
				ADD( xPointer, x_size / 8 )
				ADD( yPointer, y_size / 8 )
				SUB( length, 1 )
				JNZ( "finalize" )

				LABEL( "return_ok" )
				XOR(eax, eax)
				LABEL( "return" )
				RET()

				return codegen.end_function()
