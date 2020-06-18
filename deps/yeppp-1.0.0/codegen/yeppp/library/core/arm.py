#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

__author__ = 'Marat'

from peachpy.arm import *

class SCALAR:
	@staticmethod
	def AddSubtractMultiply_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, input_type, output_type, operation):
		if output_type.is_integer():
			if output_type.get_size() != 8:
				acc = GeneralPurposeRegister()
				LOAD.ELEMENT( acc, [xPointer], input_type, increment_pointer = True )
				temp = GeneralPurposeRegister()
				LOAD.ELEMENT( temp, [yPointer], input_type, increment_pointer = True )
		
				COMPUTE = { 'Add': ADD, 'Subtract': SUB }[operation]
				COMPUTE( acc, temp )
			
				STORE.ELEMENT( [zPointer], acc, output_type, increment_pointer = True )
			else:
				assert input_type.get_size() == 8
				acc_lo = GeneralPurposeRegister()
				LDR( acc_lo, [xPointer], 4 )
				acc_hi = GeneralPurposeRegister()
				LDR( acc_hi, [xPointer], 4 )
				
				temp_lo = GeneralPurposeRegister()
				LDR( temp_lo, [yPointer], 4 )
				temp_hi = GeneralPurposeRegister()
				LDR( temp_hi, [yPointer], 4 )
				
				if operation == "Add":
					ADDS( acc_lo, temp_lo )
					ADC( acc_hi, temp_hi )
				elif operation == "Subtract":
					SUBS( acc_lo, temp_lo )
					SBC( acc_hi, temp_hi )
				
				STR( acc_lo, [zPointer], 4 )
				STR( acc_hi, [zPointer], 4 )
		elif output_type.is_floating_point():
			acc = { 4: SRegister(), 8: DRegister() }[output_type.get_size()]
			LOAD.ELEMENT( acc, [xPointer], input_type, increment_pointer = True )
			temp = { 4: SRegister(), 8: DRegister() }[output_type.get_size()]
			LOAD.ELEMENT( temp, [yPointer], input_type, increment_pointer = True )

			COMPUTE = { ('Add', 4): VADD.F32, ('Subtract', 4): VSUB.F32, ('Multiply', 4): VMUL.F32,
			            ('Add', 8): VADD.F64, ('Subtract', 8): VSUB.F64, ('Multiply', 8): VMUL.F64}[operation, output_type.get_size()]
			COMPUTE( acc, temp )

			STORE.ELEMENT( [zPointer], acc, output_type, increment_pointer = True )

	@staticmethod
	def MinMax_VXusVXus_VYus(xPointer, yPointer, zPointer, ctype, operation):
		acc = GeneralPurposeRegister()
		LOAD.ELEMENT( acc, [xPointer], ctype, increment_pointer = True )
		temp = GeneralPurposeRegister()
		LOAD.ELEMENT( temp, [yPointer], ctype, increment_pointer = True )

		CMP( acc, temp )
		if operation == "Min":
			if ctype.is_unsigned_integer():
				MOVHI( acc, temp )
			else:
				MOVGT( acc, temp )
		elif operation == "Max":
			if ctype.is_unsigned_integer():
				MOVLO( acc, temp )
			else:
				MOVLT( acc, temp )
	
		STORE.ELEMENT( [zPointer], acc, ctype, increment_pointer = True )

	@staticmethod
	def AddSubtractMultiply_VXfVXf_VYf(xPointer, yPointer, zPointer, ctype, operation):
		acc = DRegister() if ctype.get_size() == 8 else SRegister() 
		LOAD.ELEMENT( acc, [xPointer], ctype, increment_pointer = True )
		temp = DRegister() if ctype.get_size() == 8 else SRegister()
		LOAD.ELEMENT( temp, [yPointer], ctype, increment_pointer = True )

		if ctype.get_size() == 8:
			COMPUTE = { 'Add': VADD.F64, 'Subtract': VSUB.F64 }[operation]
		else:
			COMPUTE = { 'Add': VADD.F32, 'Subtract': VSUB.F32 }[operation]
		COMPUTE( acc, temp )
	
		STORE.ELEMENT( [zPointer], acc, ctype, increment_pointer = True )

def PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, batch_elements, input_type, output_type, scalar_function, instruction_columns, instruction_offsets):
	# Check that we have an offset for each instruction column
	assert len(instruction_columns) == len(instruction_offsets)

	max_instructions  = max(map(len, instruction_columns))
	
	return_ok                 = Label("return_ok")
	return_null_pointer       = Label("return_null_pointer")
	return_misaligned_pointer = Label("return_misaligned_pointer")
	return_any                = Label("return")
	batch_process_finish      = Label("batch_process_finish")
	process_single            = Label("process_single")
	process_batch             = Label("process_batch")
	process_batch_prologue    = Label("process_batch_prologue") 
	process_batch_epilogue    = Label("process_batch_epilogue") 

	# Check parameters
	TST( xPointer, xPointer )
	BEQ( return_null_pointer )

	if input_type.get_size() != 1:
		TST( xPointer, input_type.get_size() - 1 )
		BNE( return_misaligned_pointer )
	
	TST( yPointer, yPointer )
	BEQ( return_null_pointer )

	if input_type.get_size() != 1:	
		TST( yPointer, input_type.get_size() - 1 )
		BNE( return_misaligned_pointer )

	TST( zPointer, zPointer )
	BEQ( return_null_pointer )

	if output_type.get_size() != 1:	
		TST( zPointer, output_type.get_size() - 1 )
		BNE( return_misaligned_pointer )

	SUBS( length, batch_elements )
	BLO( batch_process_finish )

	LABEL( process_batch_prologue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i >= instruction_offset:
				Function.get_current().add_instruction(instruction_column[i - instruction_offset])

	SUBS( length, batch_elements )
	BLO( process_batch_epilogue )

	LABEL( process_batch )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	SUBS( length, batch_elements )
	BHS( process_batch )

	LABEL( process_batch_epilogue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i < instruction_offset:
				Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	LABEL( batch_process_finish )
	ADDS( length, batch_elements )
	BEQ( return_ok )

	LABEL( process_single )
	scalar_function(xPointer, yPointer, zPointer)
	SUBS( length, 1 )
	BNE( process_single )

	LABEL( return_ok )
	MOV( r0, 0 )
	
	LABEL( return_any )
	RETURN()

	LABEL( return_null_pointer )
	RETURN( 1 )

	if input_type.get_size() != 1 or output_type.get_size() != 1:	
		LABEL( return_misaligned_pointer )
		RETURN( 2 )

def PipelineMap_VXusfVSusf_VYusf(xPointer, y, zPointer, length, batch_elements, input_type, output_type, scalar_function, instruction_columns, instruction_offsets):
	# Check that we have an offset for each instruction column
	assert len(instruction_columns) == len(instruction_offsets)

	max_instructions  = max(map(len, instruction_columns))
	
	return_ok                 = Label("return_ok")
	return_null_pointer       = Label("return_null_pointer")
	return_misaligned_pointer = Label("return_misaligned_pointer")
	return_any                = Label("return")
	batch_process_finish      = Label("batch_process_finish")
	process_single            = Label("process_single")
	process_batch             = Label("process_batch")
	process_batch_prologue    = Label("process_batch_prologue") 
	process_batch_epilogue    = Label("process_batch_epilogue") 

	# Check parameters
	TST( xPointer, xPointer )
	BEQ( return_null_pointer )

	if input_type.get_size() != 1:
		TST( xPointer, input_type.get_size() - 1 )
		BNE( return_misaligned_pointer )
	
	TST( zPointer, zPointer )
	BEQ( return_null_pointer )

	if output_type.get_size() != 1:	
		TST( zPointer, output_type.get_size() - 1 )
		BNE( return_misaligned_pointer )

	SUBS( length, batch_elements )
	BLO( batch_process_finish )

	LABEL( process_batch_prologue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i >= instruction_offset:
				Function.get_current().add_instruction(instruction_column[i - instruction_offset])

	SUBS( length, batch_elements )
	BLO( process_batch_epilogue )

	LABEL( process_batch )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	SUBS( length, batch_elements )
	BHS( process_batch )

	LABEL( process_batch_epilogue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i < instruction_offset:
				Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	LABEL( batch_process_finish )
	ADDS( length, batch_elements )
	BEQ( return_ok )

	LABEL( process_single )
	scalar_function(xPointer, y, zPointer)
	SUBS( length, 1 )
	BNE( process_single )

	LABEL( return_ok )
	MOV( r0, 0 )
	
	LABEL( return_any )
	RETURN()

	LABEL( return_null_pointer )
	RETURN( 1 )

	if input_type.get_size() != 1 or output_type.get_size() != 1:	
		LABEL( return_misaligned_pointer )
		RETURN( 2 )

def AddSubMul_VXusVXus_VXus_NEON(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['arm-softeabi', 'arm-hardeabi']:
		if module == 'Core':
			if function in ['Add', 'Subtract']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				if function_signature in ['V8sV8s_V8s',  'V16sV16s_V16s', 'V32sV32s_V32s', 'V64sV64s_V64s', 'V32fV32f_V32f']:
					if function != "Multiply" or function_signature != 'V64sV64s_V64s':
						ctype = x_argument.get_type().get_primitive_type()
				else:
					return

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, ctype, ctype, function)

				VLOAD  = { 1: VLD1.I8, 2: VLD1.I16, 4: VLD1.I32, 8: VLD1.I64 }[ctype.get_size()]
				VSTORE = { 1: VST1.I8, 2: VST1.I16, 4: VST1.I32, 8: VST1.I64 }[ctype.get_size()]
				if ctype.is_integer():
					if function == 'Add':
						VCOMPUTE = { 1: VADD.I8, 2: VADD.I16, 4: VADD.I32, 8: VADD.I64 }[ctype.get_size()]
					elif function == 'Subtract':
						VCOMPUTE = { 1: VSUB.I8, 2: VSUB.I16, 4: VSUB.I32, 8: VSUB.I64 }[ctype.get_size()]
					elif function == 'Multiply':
						VCOMPUTE = { 1: VMUL.I8, 2: VMUL.I16, 4: VMUL.I32 }[ctype.get_size()]
				elif ctype.is_floating_point():
					VCOMPUTE = { 'Add': VADD.F32, 'Subtract': VSUB.F32, 'Multiply': VMUL.F32 }[function]

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'CortexA9', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 6
					register_size    = 16
					batch_elements   = unroll_registers * register_size / ctype.get_size()

					Qx = [QRegister() for _ in range(unroll_registers)]
					Qy = [QRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 1, 1, 2)
					instruction_columns = [InstructionStream() for _ in range(5)] 
					for i in range(0, unroll_registers, 2):
						with instruction_columns[0]:
							VLOAD( (Qx[i].get_low_part(), Qx[i].get_high_part(), Qx[i+1].get_low_part(), Qx[i+1].get_high_part()), [xPointer.wb()] )
						with instruction_columns[1]:
							VLOAD( (Qy[i].get_low_part(), Qy[i].get_high_part(), Qy[i+1].get_low_part(), Qy[i+1].get_high_part()), [yPointer.wb()] )
						with instruction_columns[2]:
							VCOMPUTE( Qx[i], Qy[i] )
						with instruction_columns[3]:
							VCOMPUTE( Qx[i+1], Qy[i+1] )
						with instruction_columns[4]:
							VSTORE( (Qx[i].get_low_part(), Qx[i].get_high_part(), Qx[i+1].get_low_part(), Qx[i+1].get_high_part()), [zPointer.wb()] )
				
					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, batch_elements, ctype, ctype, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def AddSubMul_VXusVXus_VYus_NEON(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['arm-softeabi', 'arm-hardeabi']:
		if module == 'Core':
			if function in ['Add', 'Subtract']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				if function_signature in ['V8uV8u_V16u',  'V16uV16u_V32u',
				                          'V8sV8s_V16s',  'V16sV16s_V32s']:
					input_type = x_argument.get_type().get_primitive_type()
					output_type = z_argument.get_type().get_primitive_type()
				else:
					return

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, input_type, output_type, function)

				VLOAD  = { 1: VLD1.I8, 2: VLD1.I16, 4: VLD1.I32, 8: VLD1.I64 }[input_type.get_size()]
				VSTORE = { 1: VST1.I8, 2: VST1.I16, 4: VST1.I32, 8: VST1.I64 }[input_type.get_size()]
				if function == 'Add':
					if input_type.is_signed_integer():
						VCOMPUTE = { 1: VADDL.S8, 2: VADDL.S16, 4: VADDL.S32 }[input_type.get_size()]
					else:
						VCOMPUTE = { 1: VADDL.U8, 2: VADDL.U16, 4: VADDL.U32 }[input_type.get_size()]
				elif function == 'Subtract':
					if input_type.is_signed_integer():
						VCOMPUTE = { 1: VSUBL.S8, 2: VSUBL.S16, 4: VSUBL.S32 }[input_type.get_size()]
					else:
						VCOMPUTE = { 1: VSUBL.U8, 2: VSUBL.U16, 4: VSUBL.U32 }[input_type.get_size()]
				elif function == 'Multiply':
					if input_type.is_signed_integer():
						VCOMPUTE = { 1: VMULL.S8, 2: VMULL.S16, 4: VMULL.S32 }[input_type.get_size()]
					else:
						VCOMPUTE = { 1: VMULL.U8, 2: VMULL.U16, 4: VMULL.U32 }[input_type.get_size()]

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'CortexA9', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 3
					register_size    = 16
					batch_elements   = unroll_registers * register_size / input_type.get_size()

					Qx = [QRegister() for _ in range(unroll_registers)]
					Qy = [QRegister() for _ in range(unroll_registers)]
					Qz = [QRegister() for _ in range(unroll_registers * 2)]

					instruction_offsets = (0, 0, 1, 1, 2)
					instruction_columns = [InstructionStream() for _ in range(5)] 
					for i in range(0, unroll_registers):
						with instruction_columns[0]:
							VLOAD( (Qx[i].get_low_part(), Qx[i].get_high_part()), [xPointer.wb()] )
						with instruction_columns[1]:
							VLOAD( (Qy[i].get_low_part(), Qy[i].get_high_part()), [yPointer.wb()] )
						with instruction_columns[2]:
							VCOMPUTE( Qz[2*i], Qx[i].get_low_part(), Qy[i].get_low_part() )
						with instruction_columns[3]:
							VCOMPUTE( Qz[2*i+1], Qx[i].get_high_part(), Qy[i].get_high_part() )
						with instruction_columns[4]:
							VSTORE( (Qz[2*i].get_low_part(), Qz[2*i].get_high_part(), Qz[2*i+1].get_low_part(), Qz[2*i+1].get_high_part()), [zPointer.wb()] )
				
					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def AddSubMul_VXusVXus_VXus_VFPv3(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['arm-softeabi', 'arm-hardeabi']:
		if module == 'Core':
			if function in ['Add', 'Subtract', 'Multiply']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				if function_signature in ['V64fV64f_V64f']:
					ctype = x_argument.get_type().get_primitive_type()
				else:
					return

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, ctype, ctype, function)

				VCOMPUTE = { ('Add', 4): VADD.F32, ('Subtract', 4): VSUB.F32, ('Multiply', 4): VMUL.F32,
				             ('Add', 8): VADD.F64, ('Subtract', 8): VSUB.F64, ('Multiply', 8): VMUL.F64 }[function, ctype.get_size()]

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'CortexA9', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = { 4: 12, 8: 8 }[ctype.get_size()]
					SDx = [{ 4: SRegister(), 8: DRegister() }[ctype.get_size()] for _ in range(unroll_registers)]
					SDy = [{ 4: SRegister(), 8: DRegister() }[ctype.get_size()] for _ in range(unroll_registers)]

					instruction_offsets = { 4: (0, 1, 3, 4, 5), 8: (0, 0, 1, 2, 3) }[ctype.get_size()]
					instruction_columns = [InstructionStream() for _ in range(5)] 
					for i in range(0, unroll_registers, 2):
						with instruction_columns[0]:
							VLDM( xPointer.wb(), tuple(SDx[i:i+2]) )
						with instruction_columns[1]:
							VLDM( yPointer.wb(), tuple(SDy[i:i+2]) )
						with instruction_columns[2]:
							VCOMPUTE( SDx[i], SDy[i] )
						with instruction_columns[3]:
							VCOMPUTE( SDx[i+1], SDy[i+1] )
						with instruction_columns[4]:
							VSTM( zPointer.wb(), tuple(SDx[i:i+2]) )
				
					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, unroll_registers, ctype, ctype, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def MinMax_VXusVXus_VXus_NEON(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['arm-softeabi', 'arm-hardeabi']:
		if module == 'Core':
			if function in ['Min', 'Max']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				if function_signature in ['V8uV8u_V8u', 'V16uV16u_V16u', 'V32uV32u_V32u', 'V8sV8s_V8s', 'V16sV16s_V16s', 'V32sV32s_V32s']:
					ctype = x_argument.get_type().get_primitive_type()
				else:
					return

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.MinMax_VXusVXus_VYus(xPointer, yPointer, zPointer, ctype, function)

				VLOAD  = { 1: VLD1.I8, 2: VLD1.I16, 4: VLD1.I32, 8: VLD1.I64 }[ctype.get_size()]
				VSTORE = { 1: VST1.I8, 2: VST1.I16, 4: VST1.I32, 8: VST1.I64 }[ctype.get_size()]
				if function == 'Min':
					if ctype.is_unsigned_integer():
						VCOMPUTE = { 1: VMIN.U8, 2: VMIN.U16, 4: VMIN.U32 }[ctype.get_size()]
					else:
						VCOMPUTE = { 1: VMIN.S8, 2: VMIN.S16, 4: VMIN.S32 }[ctype.get_size()]
				elif function == 'Max':
					if ctype.is_unsigned_integer():
						VCOMPUTE = { 1: VMAX.U8, 2: VMAX.U16, 4: VMAX.U32 }[ctype.get_size()]
					else:
						VCOMPUTE = { 1: VMAX.S8, 2: VMAX.S16, 4: VMAX.S32 }[ctype.get_size()]

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'CortexA9', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 6
					register_size    = 16
					batch_elements   = unroll_registers * register_size / ctype.get_size()

					Qx = [QRegister() for _ in range(unroll_registers)]
					Qy = [QRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 1, 1, 2)
					instruction_columns = [InstructionStream() for _ in range(5)] 
					for i in range(0, unroll_registers, 2):
						with instruction_columns[0]:
							VLOAD( (Qx[i].get_low_part(), Qx[i].get_high_part(), Qx[i+1].get_low_part(), Qx[i+1].get_high_part()), [xPointer.wb()] )
						with instruction_columns[1]:
							VLOAD( (Qy[i].get_low_part(), Qy[i].get_high_part(), Qy[i+1].get_low_part(), Qy[i+1].get_high_part()), [yPointer.wb()] )
						with instruction_columns[2]:
							VCOMPUTE( Qx[i], Qy[i] )
						with instruction_columns[3]:
							VCOMPUTE( Qx[i+1], Qy[i+1] )
						with instruction_columns[4]:
							VSTORE( (Qx[i].get_low_part(), Qx[i].get_high_part(), Qx[i+1].get_low_part(), Qx[i+1].get_high_part()), [zPointer.wb()] )
				
					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, batch_elements, ctype, ctype, PROCESS_SCALAR, instruction_columns, instruction_offsets)

