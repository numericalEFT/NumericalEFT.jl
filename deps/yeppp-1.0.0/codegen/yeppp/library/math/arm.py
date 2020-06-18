#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

__author__ = 'Marat'

from peachpy.arm import *
from yeppp import Status
from yeppp.library.math import *
import math

def ilog2(n):
	return {1: 0, 2: 1, 4: 2, 8: 3, 16: 4, 32: 5, 64: 6, 128: 7, 256: 8}[n]

def Map_Vf_Vf(scalarFunction, batchFunction, xPointer, yPointer, length, batchBytes, elementSize):
	TST( xPointer, xPointer )
	BEQ( 'return_null_pointer' )
	
	TST( xPointer, elementSize - 1 )
	BNE( 'return_misaligned_pointer' )
	
	TST( yPointer, yPointer )
	BEQ( 'return_null_pointer' )
	
	TST( yPointer, elementSize - 1 )
	BNE( 'return_misaligned_pointer' )
	
	TST( length, length )
	BEQ( 'return_ok' )

	if batchFunction is not None:
		SUBS( length, batchBytes / elementSize )
		BLO( 'process_restore' )
	
		LABEL( 'process_batch' )
		batchFunction(xPointer, yPointer)
		SUBS( length, batchBytes / elementSize )
		BHS( 'process_batch' )
			
		LABEL( 'process_restore' )
		ADDS( length, batchBytes / elementSize )
		BEQ( 'return_ok' )

	LABEL( 'process_single')
	scalarFunction(xPointer, yPointer)
	SUBS( length, 1 )
	BNE( 'process_single' )

	LABEL( 'return_ok' )
	RETURN( Status.Ok )
	
	LABEL( 'return_null_pointer' )
	RETURN( Status.NullPointer )
	
	LABEL( 'return_misaligned_pointer' )
	RETURN( Status.MisalignedPointer )


def SCALAR_POLYNOMIAL_EVALUATION_VFP(cPointer, xPointer, yPointer, count, element_type):
	scalar_polevl_finish = Label("scalar_polevl_finish")
	scalar_polevl_next   = Label("scalar_polevl_next")

	MOV_SCALAR  = {4: VMOV.F32, 8: VMOV.F64 }[element_type.get_size()]
	MUL_SCALAR  = {4: VMUL.F32, 8: VMUL.F64 }[element_type.get_size()]
	ADD_SCALAR  = {4: VADD.F32, 8: VADD.F64 }[element_type.get_size()]
	FMA_SCALAR  = {4: VFMA.F32, 8: VFMA.F64 }[element_type.get_size()]
	
	x = SRegister() if element_type.get_size() == 4 else DRegister()
	LOAD.ELEMENT( x, [xPointer], element_type, increment_pointer = True )
	
	ccPointer = GeneralPurposeRegister()
	ADD( ccPointer, cPointer, count.LSL(ilog2(element_type.get_size())) )
	SUB( ccPointer, element_type.get_size() )
	
	y = SRegister() if element_type.get_size() == 4 else DRegister()
	LOAD.ELEMENT( y, [ccPointer], element_type )
	
	SUB( ccPointer, element_type.get_size() )
	CMP( ccPointer, cPointer )
	BLO( scalar_polevl_finish )
	
	LABEL( scalar_polevl_next )
	c = SRegister() if element_type.get_size() == 4 else DRegister()
	LOAD.ELEMENT( c, [ccPointer], element_type )
	if Target.has_vfp4():
		FMA_SCALAR( c, y, x )
		SWAP.REGISTERS( c, y )
	else:
		MUL_SCALAR( y, y, x )
		ADD_SCALAR( y, y, c )

	SUB( ccPointer, element_type.get_size() )
	CMP( ccPointer, cPointer )
	BHS( scalar_polevl_next )
	
	LABEL( scalar_polevl_finish )
	STORE.ELEMENT( [yPointer], y, element_type, increment_pointer = True )

def EvaluatePolynomial_V32fV32f_V32f(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ('arm-softeabi', 'arm-hardeabi'):
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
				else:
					element_type = x_type
					element_size = x_type.get_size()

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'CortexA9', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					(cPointer, xPointer, yPointer, count, length) = LOAD.PARAMETERS()
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						SCALAR_POLYNOMIAL_EVALUATION_VFP(cPointer, xPointer, yPointer, count, element_type)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						ccPointer = GeneralPurposeRegister()
						ADD( ccPointer, cPointer, count.LSL(2) )
						SUB( ccPointer, 4 )
						
						increment = GeneralPurposeRegister()
						MOV( increment, -4 )
	
						y = [QRegister() for _ in range(6)]
						VLD1.F32( (y[5].get_low_part()[:], y[5].get_high_part()[:]), [ccPointer], increment )
						for i in range(6):
							VMOV( y[i], y[5] )

						CMP( ccPointer, cPointer )
						BLO( 'batch_polevl_finish' )
	
						c = QRegister()
						VLD1.F32( (c.get_low_part()[:], c.get_high_part()[:]), [ccPointer], increment )
						x = [QRegister() for _ in range(6)]
						for i in range(0, 6, 2):
							VLD1.F32( (x[i].get_low_part(), x[i].get_high_part(), x[i+1].get_low_part(), x[i+1].get_high_part()), [xPointer.wb()] )
							VMUL.F32( y[i], x[i] )
							VMUL.F32( y[i + 1], x[i + 1] )
							VADD.F32( y[i], c )
							VADD.F32( y[i + 1], c )
	
						CMP( ccPointer, cPointer )
						BLO( 'batch_polevl_finish' )
	
						LABEL( 'batch_polevl_next' )
	
						c = QRegister()
						VLD1.F32( (c.get_low_part()[:], c.get_high_part()[:]), [ccPointer], increment )
						for i in range(0, 6, 2):
							VMUL.F32( y[i], x[i] )
							VMUL.F32( y[i + 1], x[i + 1] )
							VADD.F32( y[i], c )
							VADD.F32( y[i + 1], c )
	
						CMP( ccPointer, cPointer )
						BHS( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						for i in range(0, 6, 2):
							VST1.F32( (y[i].get_low_part(), y[i].get_high_part(), y[i+1].get_low_part(), y[i+1].get_high_part()), [yPointer.wb()] )

					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, xPointer, yPointer, length, 16 * 6, element_size)

def EvaluatePolynomial_V64fV64f_V64f(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ('arm-softeabi', 'arm-hardeabi'):
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
				else:
					element_type = x_type
					element_size = x_type.get_size()

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'CortexA9', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					(cPointer, xPointer, yPointer, count, length) = LOAD.PARAMETERS()
	
					def SCALAR_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						SCALAR_POLYNOMIAL_EVALUATION_VFP(cPointer, xPointer, yPointer, count, element_type)
	
					def BATCH_POLYNOMIAL_EVALUATION(xPointer, yPointer):
						ccPointer = GeneralPurposeRegister()
						ADD( ccPointer, cPointer, count.LSL(3) )
						
						increment = GeneralPurposeRegister()
	
						y = [DRegister() for _ in range(15)]
						VLDR( y[14], [ccPointer, -8] )
						SUB( ccPointer, 16 )
						x = [DRegister() for _ in range(15)]
						VLDM( xPointer.wb(), tuple(x) )
						for i in range(13):
							VMOV.F64( y[i], y[14] )

						CMP( ccPointer, cPointer )
						VMOV.F64( y[13], y[14] )
						BLO( 'batch_polevl_finish' )
	
						LABEL( 'batch_polevl_next' )
	
						c = DRegister()
						VLDR( c, [ccPointer] )
						VMUL.F64( y[0], x[0] )
						SUB( ccPointer, 8 )
						VMUL.F64( y[1], x[1] )
						for i in range(2, 15):
							VMUL.F64( y[i], x[i] )
							VADD.F64( y[i - 2], c )
						VADD.F64( y[13], c )
	
						CMP( ccPointer, cPointer )
						VADD.F64( y[14], c )
						BHS( 'batch_polevl_next' )
	
						LABEL( 'batch_polevl_finish' )
						VSTM( yPointer.wb(), tuple(y) )

					Map_Vf_Vf(SCALAR_POLYNOMIAL_EVALUATION, BATCH_POLYNOMIAL_EVALUATION, xPointer, yPointer, length, 8 * 15, element_size)
