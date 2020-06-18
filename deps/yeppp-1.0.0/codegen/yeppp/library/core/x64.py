#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

__author__ = 'Marat'

from peachpy.x64 import *

class SCALAR:
	@staticmethod
	def AddSubtractMultiply_VXusVXus_VYus(xPointer, yPointer, zPointer, input_type, output_type, operation):
		acc = GeneralPurposeRegister64() if output_type.get_size() == 8 else GeneralPurposeRegister32()
		LOAD.ELEMENT( acc, [xPointer], input_type, increment_pointer = True )
		temp = GeneralPurposeRegister64() if output_type.get_size() == 8 else GeneralPurposeRegister32()
		LOAD.ELEMENT( temp, [yPointer], input_type, increment_pointer = True )
	
		COMPUTE = { 'Add': ADD, 'Subtract': SUB, 'Multiply': IMUL }[operation]
		COMPUTE( acc, temp )
	
		STORE.ELEMENT( [zPointer], acc, output_type, increment_pointer = True )
	
	@staticmethod
	def AddSubtractMultiply_VXusSXus_VYus(xPointer, y, zPointer, input_type, output_type, operation):
		acc = GeneralPurposeRegister64() if output_type.get_size() == 8 else GeneralPurposeRegister32()
		LOAD.ELEMENT( acc, [xPointer], input_type, increment_pointer = True )
	
		COMPUTE = { 'Add': ADD, 'Subtract': SUB, 'Multiply': IMUL }[operation]
		if isinstance(y, GeneralPurposeRegister64) and output_type.get_size() != 8:
			COMPUTE( acc, y.get_dword() )
		else:
			COMPUTE( acc, y )
	
		STORE.ELEMENT( [zPointer], acc, output_type, increment_pointer = True )
	
	@staticmethod
	def AddSubtractMultiplyMinimumMaximum_VXfVXf_VXf(xPointer, yPointer, zPointer, ctype, operation):
		x = SSERegister()
		LOAD.ELEMENT( x, [xPointer], ctype, increment_pointer = True )
		y = SSERegister()
		LOAD.ELEMENT( y, [yPointer], ctype, increment_pointer = True )
	
		if Target.has_avx():
			if ctype.get_size() == 4:
				COMPUTE = { "Add": VADDSS, "Subtract": VSUBSS, "Multiply": VMULSS, 'Min': VMINSS, 'Max': VMAXSS }[operation]
			else:
				COMPUTE = { "Add": VADDSD, "Subtract": VSUBSD, "Multiply": VMULSD, 'Min': VMINSD, 'Max': VMAXSD }[operation]
		else:
			if ctype.get_size() == 4:
				COMPUTE = { "Add": ADDSS, "Subtract": SUBSS, "Multiply": MULSS, 'Min': MINSS, 'Max': MAXSS }[operation]
			else:
				COMPUTE = { "Add": ADDSD, "Subtract": SUBSD, "Multiply": MULSD, 'Min': MINSD, 'Max': MAXSD }[operation]
	
		COMPUTE( x, y )
	
		STORE.ELEMENT( [zPointer], x, ctype, increment_pointer = True )

def PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, max_register_size, batch_elements, input_type, output_type, scalar_function, instruction_columns, instruction_offsets, use_simd = True):
	# Check that we have an offset for each instruction column
	assert len(instruction_columns) == len(instruction_offsets)

	max_instructions  = max(map(len, instruction_columns))
	
	source_z_aligned          = Label("source_z_%sb_aligned" % max_register_size)
	source_z_misaligned       = Label("source_z_%sb_misaligned" % max_register_size)
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
	TEST( xPointer, xPointer )
	JZ( return_null_pointer )
	
	if input_type.get_size() != 1:
		TEST( xPointer, input_type.get_size() - 1 )
		JNZ( return_misaligned_pointer )
	
	TEST( yPointer, yPointer )
	JZ( return_null_pointer )

	if input_type.get_size() != 1:	
		TEST( yPointer, input_type.get_size() - 1 )
		JNZ( return_misaligned_pointer )

	TEST( zPointer, zPointer )
	JZ( return_null_pointer )

	if output_type.get_size() != 1:	
		TEST( zPointer, output_type.get_size() - 1 )
		JNZ( return_misaligned_pointer )

	# If length is zero, return immediately
	TEST( length, length )
	JZ( return_ok )

	if use_simd:
		# If the y pointer is not aligned by register size, process by one element until it becomes aligned
		TEST( zPointer, max_register_size - 1 )
		JZ( source_z_aligned )
	
		LABEL( source_z_misaligned )
		scalar_function(xPointer, yPointer, zPointer)
		SUB( length, 1 )
		JZ( return_ok )
	
		TEST( zPointer, max_register_size - 1 )
		JNZ( source_z_misaligned )
	
		LABEL( source_z_aligned )

	SUB( length, batch_elements )
	JB( batch_process_finish )

	LABEL( process_batch_prologue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i >= instruction_offset:
				Function.get_current().add_instruction(instruction_column[i - instruction_offset])

	SUB( length, batch_elements )
	JB( process_batch_epilogue )

	ALIGN( 16 )
	LABEL( process_batch )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	SUB( length, batch_elements )
	JAE( process_batch )

	LABEL( process_batch_epilogue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i < instruction_offset:
				Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	LABEL( batch_process_finish )
	ADD( length, batch_elements )
	JZ( return_ok )

	LABEL( process_single )
	scalar_function(xPointer, yPointer, zPointer)
	SUB( length, 1 )
	JNZ( process_single )

	LABEL( return_ok )
	XOR( eax, eax )
	
	LABEL( return_any )
	RETURN()

	LABEL( return_null_pointer )
	MOV( eax, 1 )
	JMP( return_any )

	if input_type.get_size() != 1 or output_type.get_size() != 1:	
		LABEL( return_misaligned_pointer )
		MOV( eax, 2 )
		JMP( return_any )

def PipelineMap_VXusfSXusf_VYusf(xPointer, y, zPointer, length, max_register_size, batch_elements, input_type, output_type, scalar_function, instruction_columns, instruction_offsets, use_simd = True):
	# Check that we have an offset for each instruction column
	assert len(instruction_columns) == len(instruction_offsets)

	max_instructions  = max(map(len, instruction_columns))
	
	source_z_aligned          = Label("source_z_%sb_aligned" % max_register_size)
	source_z_misaligned       = Label("source_z_%sb_misaligned" % max_register_size)
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
	TEST( xPointer, xPointer )
	JZ( return_null_pointer )

	if input_type.get_size() != 1:	
		TEST( xPointer, input_type.get_size() - 1 )
		JNZ( return_misaligned_pointer )
	
	TEST( zPointer, zPointer )
	JZ( return_null_pointer )

	if output_type.get_size() != 1:	
		TEST( zPointer, output_type.get_size() - 1 )
		JNZ( return_misaligned_pointer )

	# If length is zero, return immediately
	TEST( length, length )
	JZ( return_ok )

	if use_simd:
		# If the y pointer is not aligned by register size, process by one element until it becomes aligned
		TEST( zPointer, max_register_size - 1 )
		JZ( source_z_aligned )
	
		LABEL( source_z_misaligned )
		scalar_function(xPointer, y, zPointer)
		SUB( length, 1 )
		JZ( return_ok )
	
		TEST( zPointer, max_register_size - 1 )
		JNZ( source_z_misaligned )
	
		LABEL( source_z_aligned )

	SUB( length, batch_elements )
	JB( batch_process_finish )

	LABEL( process_batch_prologue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i >= instruction_offset:
				Function.get_current().add_instruction(instruction_column[i - instruction_offset])

	SUB( length, batch_elements )
	JB( process_batch_epilogue )

	ALIGN( 16 )
	LABEL( process_batch )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	SUB( length, batch_elements )
	JAE( process_batch )

	LABEL( process_batch_epilogue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i < instruction_offset:
				Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	LABEL( batch_process_finish )
	ADD( length, batch_elements )
	JZ( return_ok )

	LABEL( process_single )
	scalar_function(xPointer, y, zPointer)
	SUB( length, 1 )
	JNZ( process_single )

	LABEL( return_ok )
	XOR( eax, eax )
	
	LABEL( return_any )
	RETURN()

	LABEL( return_null_pointer )
	MOV( eax, 1 )
	JMP( return_any )

	if input_type.get_size() != 1 or output_type.get_size() != 1:	
		LABEL( return_misaligned_pointer )
		MOV( eax, 2 )
		JMP( return_any )

def AddSub_VXusVXus_VYus_SSE(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['Add', 'Subtract']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				if function_signature in ['V8sV8s_V8s',  'V16sV16s_V16s', 'V32sV32s_V32s', 'V64sV64s_V64s', 
										  'V8uV8u_V16u', 'V16uV16u_V32u', 'V32uV32u_V64u',   
										  'V8sV8s_V16s', 'V16sV16s_V32s', 'V32sV32s_V64s']:
					input_type = x_argument.get_type().get_primitive_type()
					output_type = z_argument.get_type().get_primitive_type()
				else:
					return

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusVXus_VYus(xPointer, yPointer, zPointer, input_type, output_type, function)

				if input_type.get_size() == output_type.get_size():
					SIMD_LOAD = MOVDQU
					load_increment = 16
				else:
					load_increment = 8
					SIMD_UNPACK_LOW = { 1: PUNPCKLBW, 2: PUNPCKLWD, 4: PUNPCKLDQ }[input_type.get_size()]
					SIMD_COMPARE    = { 1: PCMPGTB, 2: PCMPGTW, 4: PCMPGTD }[input_type.get_size()]
					if input_type.is_signed_integer():
						SIMD_LOAD = { 1: PMOVSXBW, 2: PMOVSXWD, 4: PMOVSXDQ }[input_type.get_size()]
					else:
						SIMD_LOAD = { 1: PMOVZXBW, 2: PMOVZXWD, 4: PMOVZXDQ }[input_type.get_size()]

				if function == 'Add':
					SIMD_COMPUTE = { 1: PADDB, 2: PADDW, 4: PADDD, 8: PADDQ }[output_type.get_size()]
				elif function == 'Subtract':
					SIMD_COMPUTE = { 1: PSUBB, 2: PSUBW, 4: PSUBD, 8: PSUBQ }[output_type.get_size()]
				SIMD_STORE = MOVDQA

				if input_type.get_size() * 2 == output_type.get_size(): 
					with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'K10', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
						xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
						
						if input_type.is_unsigned_integer():
							xmm_zero = SSERegister()
							LOAD.ZERO( xmm_zero, input_type )
							
							load_increment   = 8
							unroll_registers = 7
							register_size    = 16
							batch_elements   = unroll_registers * register_size / output_type.get_size()
		
							x = [SSERegister() for _ in range(unroll_registers)]
							y = [SSERegister() for _ in range(unroll_registers)]
		
							instruction_offsets = (0, 0, 3, 4, 5, 6)
							instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
							for i in range(unroll_registers):
								with instruction_columns[0]:
									MOVQ( x[i], [xPointer + i * load_increment] )
								with instruction_columns[1]:
									MOVQ( y[i], [yPointer + i * load_increment] )
								with instruction_columns[2]:
									SIMD_UNPACK_LOW( x[i], xmm_zero )
								with instruction_columns[3]:
									SIMD_UNPACK_LOW( y[i], xmm_zero )
								with instruction_columns[4]:
									SIMD_COMPUTE( x[i], y[i] )
								with instruction_columns[5]:
									SIMD_STORE( [zPointer + i * register_size], x[i] )
							with instruction_columns[0]:
								ADD( xPointer, load_increment * unroll_registers )
							with instruction_columns[1]:
								ADD( yPointer, load_increment * unroll_registers )
							with instruction_columns[5]:
								ADD( zPointer, register_size * unroll_registers )
						else:
							unroll_registers = 4
							register_size    = 16
							batch_elements   = unroll_registers * register_size / output_type.get_size()
		
							x = [SSERegister() for _ in range(unroll_registers)]
							x_hi = [SSERegister() for _ in range(unroll_registers)]
							y = [SSERegister() for _ in range(unroll_registers)]
							y_hi = [SSERegister() for _ in range(unroll_registers)]
		
							instruction_offsets = (0, 0, 0, 0, 1, 1, 2, 2, 2, 3)
							instruction_columns = [InstructionStream() for _ in range(10)] 
							for i in range(unroll_registers):
								with instruction_columns[0]:
									MOVQ( x[i], [xPointer + i * load_increment] )
								with instruction_columns[1]:
									MOVQ( y[i], [yPointer + i * load_increment] )
								with instruction_columns[2]:
									LOAD.ZERO( x_hi[i], input_type )
								with instruction_columns[3]:
									LOAD.ZERO( y_hi[i], input_type )
								with instruction_columns[4]:
									SIMD_COMPARE( x_hi[i], x[i] )
								with instruction_columns[5]:
									SIMD_COMPARE( y_hi[i], y[i] )
								with instruction_columns[6]:
									SIMD_UNPACK_LOW( x[i], x_hi[i] )
								with instruction_columns[7]:
									SIMD_UNPACK_LOW( y[i], y_hi[i] )
								with instruction_columns[8]:
									SIMD_COMPUTE( x[i], y[i] )
								with instruction_columns[9]:
									SIMD_STORE( [zPointer + i * register_size], x[i] )
							with instruction_columns[0]:
								ADD( xPointer, load_increment * unroll_registers )
							with instruction_columns[1]:
								ADD( yPointer, load_increment * unroll_registers )
							with instruction_columns[9]:
								ADD( zPointer, register_size * unroll_registers )
	
						PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 8
					register_size    = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 1, 6, 7)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * load_increment] )
						with instruction_columns[2]:
							SIMD_COMPUTE( x[i], y[i] )
						with instruction_columns[3]:
							SIMD_STORE( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, load_increment * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def AddSub_VXusVXus_VYus_AVX(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['Add', 'Subtract']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				if function_signature in ['V8sV8s_V8s',  'V16sV16s_V16s', 'V32sV32s_V32s', 'V64sV64s_V64s', 
										  'V8uV8u_V16u', 'V16uV16u_V32u', 'V32uV32u_V64u',   
										  'V8sV8s_V16s', 'V16sV16s_V32s', 'V32sV32s_V64s']:
					input_type = x_argument.get_type().get_primitive_type()
					output_type = z_argument.get_type().get_primitive_type()
				else:
					return

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusVXus_VYus(xPointer, yPointer, zPointer, input_type, output_type, function)

				if input_type.get_size() == output_type.get_size():
					SIMD_LOAD = VMOVDQU
				else:
					if input_type.is_signed_integer():
						SIMD_LOAD = { 1: VPMOVSXBW, 2: VPMOVSXWD, 4: VPMOVSXDQ }[input_type.get_size()]
					else:
						SIMD_LOAD = { 1: VPMOVZXBW, 2: VPMOVZXWD, 4: VPMOVZXDQ }[input_type.get_size()]

				if function == 'Add':
					SIMD_COMPUTE = { 1: VPADDB, 2: VPADDW, 4: VPADDD, 8: VPADDQ }[output_type.get_size()]
				elif function == 'Subtract':
					SIMD_COMPUTE = { 1: VPSUBB, 2: VPSUBW, 4: VPSUBD, 8: VPSUBQ }[output_type.get_size()]
				SIMD_STORE = VMOVDQA

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					load_increment   = 16 if input_type.get_size() == output_type.get_size() else 8
					unroll_registers = 8
					register_size    = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 1, 6, 7)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * load_increment] )
						with instruction_columns[2]:
							SIMD_COMPUTE( x[i], y[i] )
						with instruction_columns[3]:
							SIMD_STORE( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, load_increment * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					load_increment   = 32 if input_type.get_size() == output_type.get_size() else 16
					unroll_registers = 8
					register_size    = 32
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [AVXRegister() for _ in range(unroll_registers)]
					y = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 1, 6, 7)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * load_increment] )
						with instruction_columns[2]:
							SIMD_COMPUTE( x[i], y[i] )
						with instruction_columns[3]:
							SIMD_STORE( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, load_increment * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def AddSub_VXusSXus_VYus_SSE(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['Add', 'Subtract']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				if function_signature in ['V8uS8u_V8u',  'V16uS16u_V16u', 'V32uS32u_V32u', 'V64uS64u_V64u', 
										  'V8uS8u_V16u', 'V16uS16u_V32u', 'V32uS32u_V64u',   
										  'V8sS8s_V16s', 'V16sS16s_V32s', 'V32sS32s_V64s']:
					input_type = x_argument.get_type().get_primitive_type()
					output_type = z_argument.get_type().get_primitive_type()
				else:
					return

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusSXus_VYus(xPointer, y, zPointer, input_type, output_type, function)

				if input_type.get_size() == output_type.get_size():
					SIMD_LOAD = MOVDQU
					load_increment = 16
				else:
					load_increment = 8
					SIMD_UNPACK_LOW = { 1: PUNPCKLBW, 2: PUNPCKLWD, 4: PUNPCKLDQ }[input_type.get_size()]
					SIMD_COMPARE    = { 1: PCMPGTB, 2: PCMPGTW, 4: PCMPGTD }[input_type.get_size()]
					if input_type.is_signed_integer():
						SIMD_LOAD = { 1: PMOVSXBW, 2: PMOVSXWD, 4: PMOVSXDQ }[input_type.get_size()]
					else:
						SIMD_LOAD = { 1: PMOVZXBW, 2: PMOVZXWD, 4: PMOVZXDQ }[input_type.get_size()]

				if function == 'Add':
					SIMD_COMPUTE = { 1: PADDB, 2: PADDW, 4: PADDD, 8: PADDQ }[output_type.get_size()]
				elif function == 'Subtract':
					SIMD_COMPUTE = { 1: PSUBB, 2: PSUBW, 4: PSUBD, 8: PSUBQ }[output_type.get_size()]
				SIMD_STORE = MOVDQA

				if input_type.get_size() * 2 == output_type.get_size(): 
					with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'K10', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
						xPointer, y, zPointer, length = LOAD.PARAMETERS()
						
						if input_type.is_unsigned_integer():
							xmm_zero = SSERegister()
							LOAD.ZERO( xmm_zero, input_type )
							
							load_increment   = 8
							unroll_registers = 7
							register_size    = 16
							batch_elements   = unroll_registers * register_size / output_type.get_size()
		
							xmm_x = [SSERegister() for _ in range(unroll_registers)]
							
							xmm_y = SSERegister()
							BROADCAST.ELEMENT( xmm_y, y, input_type, output_type )
		
							instruction_offsets = (0, 4, 5, 6)
							instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
							for i in range(unroll_registers):
								with instruction_columns[0]:
									MOVQ( xmm_x[i], [xPointer + i * load_increment] )
								with instruction_columns[1]:
									SIMD_UNPACK_LOW( xmm_x[i], xmm_zero )
								with instruction_columns[2]:
									SIMD_COMPUTE( xmm_x[i], xmm_y )
								with instruction_columns[3]:
									SIMD_STORE( [zPointer + i * register_size], xmm_x[i] )
							with instruction_columns[0]:
								ADD( xPointer, load_increment * unroll_registers )
							with instruction_columns[3]:
								ADD( zPointer, register_size * unroll_registers )
						else:
							unroll_registers = 6
							register_size    = 16
							batch_elements   = unroll_registers * register_size / output_type.get_size()
		
							xmm_x = [SSERegister() for _ in range(unroll_registers)]
							xmm_x_hi = [SSERegister() for _ in range(unroll_registers)]

							xmm_y = SSERegister()
							BROADCAST.ELEMENT( xmm_y, y, input_type, output_type )
		
							instruction_offsets = (0, 0, 2, 3, 4, 5)
							instruction_columns = [InstructionStream() for _ in range(6)] 
							for i in range(unroll_registers):
								with instruction_columns[0]:
									MOVQ( xmm_x[i], [xPointer + i * load_increment] )
								with instruction_columns[1]:
									LOAD.ZERO( xmm_x_hi[i], input_type )
								with instruction_columns[2]:
									SIMD_COMPARE( xmm_x_hi[i], xmm_x[i] )
								with instruction_columns[3]:
									SIMD_UNPACK_LOW( xmm_x[i], xmm_x_hi[i] )
								with instruction_columns[4]:
									SIMD_COMPUTE( xmm_x[i], xmm_y )
								with instruction_columns[5]:
									SIMD_STORE( [zPointer + i * register_size], xmm_x[i] )
							with instruction_columns[0]:
								ADD( xPointer, load_increment * unroll_registers )
							with instruction_columns[5]:
								ADD( zPointer, register_size * unroll_registers )

						PipelineMap_VXusfSXusf_VYusf(xPointer, y, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, y, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 8
					register_size    = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					xmm_x = [SSERegister() for _ in range(unroll_registers)]

					xmm_y = SSERegister()
					BROADCAST.ELEMENT( xmm_y, y, input_type, output_type )
					
					instruction_offsets = (0, 4, 5)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( xmm_x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							SIMD_COMPUTE( xmm_x[i], xmm_y )
						with instruction_columns[2]:
							SIMD_STORE( [zPointer + i * register_size], xmm_x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[2]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfSXusf_VYusf(xPointer, y, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def AddSub_VXusSXus_VYus_AVX(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['Add', 'Subtract']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				if function_signature in ['V8uS8u_V8u',  'V16uS16u_V16u', 'V32uS32u_V32u', 'V64uS64u_V64u', 
										  'V8uS8u_V16u', 'V16uS16u_V32u', 'V32uS32u_V64u',   
										  'V8sS8s_V16s', 'V16sS16s_V32s', 'V32sS32s_V64s']:
					input_type = x_argument.get_type().get_primitive_type()
					output_type = z_argument.get_type().get_primitive_type()
				else:
					return

				def PROCESS_SCALAR(xPointer, y, zPointer):
					SCALAR.AddSubtractMultiply_VXusSXus_VYus(xPointer, y, zPointer, input_type, output_type, function)

				if input_type.get_size() == output_type.get_size():
					SIMD_LOAD = VMOVDQU
				else:
					if input_type.is_signed_integer():
						SIMD_LOAD = { 1: VPMOVSXBW, 2: VPMOVSXWD, 4: VPMOVSXDQ }[input_type.get_size()]
					else:
						SIMD_LOAD = { 1: VPMOVZXBW, 2: VPMOVZXWD, 4: VPMOVZXDQ }[input_type.get_size()]

				if function == 'Add':
					SIMD_COMPUTE = { 1: VPADDB, 2: VPADDW, 4: VPADDD, 8: VPADDQ }[output_type.get_size()]
				elif function == 'Subtract':
					SIMD_COMPUTE = { 1: VPSUBB, 2: VPSUBW, 4: VPSUBD, 8: VPSUBQ }[output_type.get_size()]
				SIMD_STORE = VMOVDQA

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, y, zPointer, length = LOAD.PARAMETERS()
					
					load_increment   = 16 if input_type.get_size() == output_type.get_size() else 8
					unroll_registers = 8
					register_size    = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					xmm_x = [SSERegister() for _ in range(unroll_registers)]
					xmm_y = SSERegister()
					BROADCAST.ELEMENT( xmm_y, y, input_type, output_type )

					instruction_offsets = (0, 4, 5)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( xmm_x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							SIMD_COMPUTE( xmm_x[i], xmm_y )
						with instruction_columns[2]:
							SIMD_STORE( [zPointer + i * register_size], xmm_x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[2]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfSXusf_VYusf(xPointer, y, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, y, zPointer, length = LOAD.PARAMETERS()
					
					load_increment   = 32 if input_type.get_size() == output_type.get_size() else 16
					unroll_registers = 8
					register_size    = 32
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					ymm_x = [AVXRegister() for _ in range(unroll_registers)]
					ymm_y = AVXRegister()

					instruction_offsets = (0, 4, 5)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( ymm_x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							SIMD_COMPUTE( ymm_x[i], ymm_y )
						with instruction_columns[2]:
							SIMD_STORE( [zPointer + i * register_size], ymm_x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[2]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfSXusf_VYusf(xPointer, y, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def Multiply_VXuVXu_VXu(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function == 'Multiply':
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()
				z_type = z_argument.get_type().get_primitive_type()

				if any(type.is_floating_point() for type in (x_type, y_type, z_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				elif x_type.get_size() != z_type.get_size():
					return
				elif x_type.get_size() not in [2, 4]:
					return 
				else:
					input_type = x_type
					output_type = z_type

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusVXus_VYus(xPointer, yPointer, zPointer, input_type, output_type, function)

				SIMD_LOAD = MOVDQU
				SIMD_COMPUTE = { 2: PMULLW, 4: PMULLD }[output_type.get_size()]
				SIMD_STORE = MOVDQA

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 8
					register_size    = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 1, 5, 7)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							SIMD_COMPUTE( x[i], y[i] )
						with instruction_columns[3]:
							SIMD_STORE( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				SIMD_LOAD = VMOVDQU
				SIMD_COMPUTE = { 2: VPMULLW, 4: VPMULLD }[output_type.get_size()]
				SIMD_STORE = VMOVDQA

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 8
					register_size    = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 1, 5, 7)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							SIMD_COMPUTE( x[i], y[i] )
						with instruction_columns[3]:
							SIMD_STORE( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 8
					register_size    = 32
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [AVXRegister() for _ in range(unroll_registers)]
					y = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 1, 5, 7)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							SIMD_COMPUTE( x[i], y[i] )
						with instruction_columns[3]:
							SIMD_STORE( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def Multiply_V16usV16us_V32us(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function == 'Multiply':
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()
				z_type = z_argument.get_type().get_primitive_type()

				if any(type.is_floating_point() for type in (x_type, y_type, z_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				elif x_type.get_size() != 2 or z_type.get_size() != 4:
					return
				else:
					input_type = x_type
					output_type = z_type

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusVXus_VYus(xPointer, yPointer, zPointer, input_type, output_type, function)

				SIMD_MUL_LO = PMULLW
				SIMD_MUL_HI = PMULHW if z_type.is_signed_integer() else PMULHUW

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()

					unroll_registers = 4
					register_size    = 16
					batch_elements   = unroll_registers * register_size * 2 / output_type.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					h = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]
					t = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 1, 1, 1, 2, 2, 2, 3, 3)
					instruction_columns = [InstructionStream() for _ in range(10)] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							MOVDQU( x[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							MOVDQU( y[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							MOVDQA( h[i], x[i] )
						with instruction_columns[3]:
							SIMD_MUL_LO( x[i], y[i] )
						with instruction_columns[4]:
							SIMD_MUL_HI( h[i], y[i] )
						with instruction_columns[5]:
							MOVDQA( t[i], x[i] )
						with instruction_columns[6]:
							PUNPCKLWD( x[i], h[i] )
						with instruction_columns[7]:
							PUNPCKHWD( t[i], h[i] )
						with instruction_columns[8]:
							MOVDQA( [zPointer + i * register_size * 2], x[i] )
						with instruction_columns[9]:
							MOVDQA( [zPointer + i * register_size * 2 + register_size], t[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )
					with instruction_columns[9]:
						ADD( zPointer, register_size * unroll_registers * 2 )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				SIMD_MUL_LO = VPMULLW
				SIMD_MUL_HI = VPMULHW if z_type.is_signed_integer() else VPMULHUW

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 3
					register_size    = 16
					batch_elements   = unroll_registers * register_size / input_type.get_size()

					x       = [SSERegister() for _ in range(unroll_registers)]
					y       = [SSERegister() for _ in range(unroll_registers)]
					prod_lo = [SSERegister() for _ in range(unroll_registers)]
					prod_hi = [SSERegister() for _ in range(unroll_registers)]
					z_lo    = [SSERegister() for _ in range(unroll_registers)]
					z_hi    = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 1, 1, 2, 2, 2, 2)
					instruction_columns = [InstructionStream() for _ in range(8)] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							VMOVDQU( x[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							VMOVDQU( y[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							SIMD_MUL_LO( prod_lo[i], x[i], y[i] )
						with instruction_columns[3]:
							SIMD_MUL_HI( prod_hi[i], x[i], y[i] )
						with instruction_columns[4]:
							VPUNPCKLWD( z_lo[i], prod_lo[i], prod_hi[i] )
						with instruction_columns[5]:
							VPUNPCKHWD( z_hi[i], prod_lo[i], prod_hi[i] )
						with instruction_columns[6]:
							VMOVDQA( [zPointer + i * register_size * 2], z_lo[i] )
						with instruction_columns[7]:
							VMOVDQA( [zPointer + i * register_size * 2 + register_size], z_hi[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )
					with instruction_columns[7]:
						ADD( zPointer, register_size * unroll_registers * 2 )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				SIMD_MUL  = VPMULLD
				SIMD_LOAD = VPMOVSXWD if z_type.is_signed_integer() else VPMOVZXWD

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 5
					register_size    = 16
					load_increment   = 8
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]
					z = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 2, 4)
					instruction_columns = [InstructionStream() for _ in range(4)] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * load_increment] )
						with instruction_columns[2]:
							SIMD_MUL( z[i], x[i], y[i] )
						with instruction_columns[3]:
							VMOVDQA( [zPointer + i * register_size], z[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, load_increment * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 5
					register_size    = 32
					load_increment   = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [AVXRegister() for _ in range(unroll_registers)]
					y = [AVXRegister() for _ in range(unroll_registers)]
					z = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 2, 4)
					instruction_columns = [InstructionStream() for _ in range(4)] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * load_increment] )
						with instruction_columns[2]:
							SIMD_MUL( z[i], x[i], y[i] )
						with instruction_columns[3]:
							VMOVDQA( [zPointer + i * register_size], z[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, load_increment * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def Multiply_V32usV32us_V64us(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function == 'Multiply':
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()
				z_type = z_argument.get_type().get_primitive_type()

				if any(type.is_floating_point() for type in (x_type, y_type, z_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				elif x_type.get_size() != 4 or z_type.get_size() != 8:
					return
				else:
					input_type = x_type
					output_type = z_type

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiply_VXusVXus_VYus(xPointer, yPointer, zPointer, input_type, output_type, function)

				SIMD_MUL = PMULDQ if z_type.is_signed_integer() else PMULUDQ

				if output_type.is_unsigned_integer():
					with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'K10', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
						xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
	
						load_increment   = 8
						unroll_registers = 8
						register_size    = 16
						batch_elements   = unroll_registers * register_size / output_type.get_size()
	
						x = [SSERegister() for _ in range(unroll_registers)]
						y = [SSERegister() for _ in range(unroll_registers)]
	
						instruction_offsets = (0, 0, 2, 2, 4, 7)
						instruction_columns = [InstructionStream() for _ in range(6)] 
						for i in range(unroll_registers):
							with instruction_columns[0]:
								MOVQ( x[i], [xPointer + i * load_increment] )
							with instruction_columns[1]:
								MOVQ( y[i], [yPointer + i * load_increment] )
							with instruction_columns[2]:
								PUNPCKLDQ( x[i], x[i] )
							with instruction_columns[3]:
								PUNPCKLDQ( y[i], y[i] )
							with instruction_columns[4]:
								SIMD_MUL( x[i], y[i] )
							with instruction_columns[5]:
								MOVDQA( [zPointer + i * register_size], x[i] )
						with instruction_columns[0]:
							ADD( xPointer, load_increment * unroll_registers )
						with instruction_columns[1]:
							ADD( yPointer, load_increment * unroll_registers )
						with instruction_columns[5]:
							ADD( zPointer, register_size * unroll_registers )

						PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()

					load_increment   = 8
					unroll_registers = 8
					register_size    = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 5, 7)
					instruction_columns = [InstructionStream() for _ in range(4)] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							PMOVZXDQ( x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							PMOVZXDQ( y[i], [yPointer + i * load_increment] )
						with instruction_columns[2]:
							SIMD_MUL( x[i], y[i] )
						with instruction_columns[3]:
							MOVDQA( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, load_increment * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				SIMD_MUL = VPMULDQ if z_type.is_signed_integer() else VPMULUDQ

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()

					load_increment   = 8
					unroll_registers = 8
					register_size    = 16
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 4, 7)
					instruction_columns = [InstructionStream() for _ in range(4)] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							VPMOVZXDQ( x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							VPMOVZXDQ( y[i], [yPointer + i * load_increment] )
						with instruction_columns[2]:
							SIMD_MUL( x[i], y[i] )
						with instruction_columns[3]:
							VMOVDQA( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, load_increment * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()

					load_increment   = 16
					unroll_registers = 8
					register_size    = 32
					batch_elements   = unroll_registers * register_size / output_type.get_size()

					x = [AVXRegister() for _ in range(unroll_registers)]
					y = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 0, 4, 7)
					instruction_columns = [InstructionStream() for _ in range(4)] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							VPMOVZXDQ( x[i], [xPointer + i * load_increment] )
						with instruction_columns[1]:
							VPMOVZXDQ( y[i], [yPointer + i * load_increment] )
						with instruction_columns[2]:
							SIMD_MUL( x[i], y[i] )
						with instruction_columns[3]:
							VMOVDQA( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, load_increment * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, load_increment * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, input_type, output_type, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def AddSubMulMinMax_VfVf_Vf(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['Add', 'Subtract', 'Multiply', 'Min', 'Max']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()
				z_type = z_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type, z_type)):
					return

				if len(set([x_type, y_type, z_type])) != 1:
					return
				else:
					ctype = x_type

				if ctype.get_size() == 4:
					SIMD_LOAD = MOVUPS
					SIMD_COMPUTE = { "Add": ADDPS, "Subtract": SUBPS, "Multiply": MULPS, 'Min': MINPS, 'Max': MAXPS }[function]
					SIMD_STORE = MOVAPS
				else:
					SIMD_LOAD = MOVUPD
					SIMD_COMPUTE = { "Add": ADDPD, "Subtract": SUBPD, "Multiply": MULPD, 'Min': MINPD, 'Max': MAXPD }[function]
					SIMD_STORE = MOVAPD

				def PROCESS_SCALAR(xPointer, yPointer, zPointer):
					SCALAR.AddSubtractMultiplyMinimumMaximum_VXfVXf_VXf(xPointer, yPointer, zPointer, ctype, function)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 7
					register_size    = 16
					batch_elements   = unroll_registers * register_size / ctype.get_size()

					x = [SSERegister() for _ in range(unroll_registers)]
					y = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 1, 5, 6)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							SIMD_COMPUTE( x[i], y[i] )
						with instruction_columns[3]:
							SIMD_STORE( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, ctype, ctype, PROCESS_SCALAR, instruction_columns, instruction_offsets)

				if ctype.get_size() == 4:
					SIMD_LOAD = VMOVUPS
					SIMD_COMPUTE = { "Add": VADDPS, "Subtract": VSUBPS, "Multiply": VMULPS, 'Min': VMINPS, 'Max': VMAXPS }[function]
					SIMD_STORE = VMOVAPS
				else:
					SIMD_LOAD = VMOVUPD
					SIMD_COMPUTE = { "Add": VADDPD, "Subtract": VSUBPD, "Multiply": VMULPD, 'Min': VMINPD, 'Max': VMAXPD }[function]
					SIMD_STORE = VMOVAPD

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
					
					unroll_registers = 7
					register_size    = 32
					batch_elements   = unroll_registers * register_size / ctype.get_size()

					x = [AVXRegister() for _ in range(unroll_registers)]
					y = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 1, 5, 6)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( x[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_LOAD( y[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							SIMD_COMPUTE( x[i], y[i] )
						with instruction_columns[3]:
							SIMD_STORE( [zPointer + i * register_size], x[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )
					with instruction_columns[3]:
						ADD( zPointer, register_size * unroll_registers )

					PipelineMap_VXusfVXusf_VYusf(xPointer, yPointer, zPointer, length, register_size, batch_elements, ctype, ctype, PROCESS_SCALAR, instruction_columns, instruction_offsets)

def PipelineReduce_VXf_SXf(xPointer, yPointer, length, accumulators, ctype, scalar_function, reduction_function, instruction_columns, instruction_offsets, use_simd = True):
	# Check that we have an offset for each instruction column
	assert len(instruction_columns) == len(instruction_offsets)

	max_instructions  = max(map(len, instruction_columns))
	max_register_size = max(register.get_size() for register in accumulators)
	if use_simd:
		batch_bytes       = sum(register.get_size() for register in accumulators)
		batch_elements    = batch_bytes / ctype.get_size()
	else:
		batch_bytes       = len(accumulators) * ctype.get_size()
		batch_elements    = len(accumulators)
	
	source_aligned          = Label("source_%sb_aligned" % max_register_size)
	source_misaligned       = Label("source_%sb_misaligned" % max_register_size)
	return_ok                 = Label("return_ok")
	return_null_pointer       = Label("return_null_pointer")
	return_misaligned_pointer = Label("return_misaligned_pointer")
	return_any                = Label("return")
	reduce_batch              = Label("reduce_batch")
	batch_process_finish      = Label("batch_process_finish")
	process_single            = Label("process_single")
	process_batch             = Label("process_batch")
	process_batch_prologue    = Label("process_batch_prologue") 
	process_batch_epilogue    = Label("process_batch_epilogue") 

	# Check parameters
	TEST( xPointer, xPointer )
	JZ( return_null_pointer )
	
	TEST( xPointer, ctype.get_size() - 1 )
	JNZ( return_misaligned_pointer )
	
	TEST( yPointer, yPointer )
	JZ( return_null_pointer )
	
	TEST( yPointer, ctype.get_size() - 1 )
	JNZ( return_misaligned_pointer )

	LOAD.ZERO( accumulators[0], ctype )

	# If length is zero, return immediately
	TEST( length, length )
	JZ( return_ok )

	# Initialize accumulators to zero
	for accumulator in accumulators[1:]:
		LOAD.ZERO( accumulator, ctype )

	if use_simd:
		# If the y pointer is not aligned by register size, process by one element until it becomes aligned
		TEST( xPointer, max_register_size - 1 )
		JZ( source_aligned )

		LABEL( source_misaligned )
		scalar_function(accumulators[0], xPointer)
		ADD( xPointer, ctype.get_size() )
		SUB( length, 1 )
		JZ( reduce_batch )

		TEST( xPointer, max_register_size - 1 )
		JNZ( source_misaligned )

		LABEL( source_aligned )

	SUB( length, batch_elements )
	JB( batch_process_finish )

	LABEL( process_batch_prologue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i >= instruction_offset:
				Function.get_current().add_instruction(instruction_column[i - instruction_offset])

	SUB( length, batch_elements )
	JB( process_batch_epilogue )

	ALIGN( 16 )
	LABEL( process_batch )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	SUB( length, batch_elements )
	JAE( process_batch )

	LABEL( process_batch_epilogue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i < instruction_offset:
				Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	LABEL( batch_process_finish )
	ADD( length, batch_elements )
	JZ( reduce_batch )

	LABEL( process_single )
	scalar_function(accumulators[0], xPointer)
	ADD( xPointer, ctype.get_size() )
	SUB( length, 1 )
	JNZ( process_single )

	LABEL( reduce_batch )
	reduction_function(accumulators, ctype, ctype)

	LABEL( return_ok )

	STORE.ELEMENT( [yPointer], accumulators[0], ctype )
	XOR( eax, eax )
	
	LABEL( return_any )
	RETURN()

	LABEL( return_null_pointer )
	MOV( eax, 1 )
	JMP( return_any )
	
	LABEL( return_misaligned_pointer )
	MOV( eax, 2 )
	JMP( return_any )

def SumAbs_VXf_SXf_SSE(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['SumAbs']:
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				else:
					ctype = x_type

				SIMD_LOAD  = {4: MOVUPS, 8: MOVUPD}[x_type.get_size()]
				SIMD_ADD   = {4: ADDPS, 8: ADDPD}[x_type.get_size()]
				SIMD_AND   = {4: ANDPS, 8: ANDPD}[x_type.get_size()]

				def PROCESS_SCALAR(acc, xPointer, xmm_abs_mask):
					# Load x
					x = SSERegister()
					LOAD.ELEMENT( x, [xPointer], x_type )
					# Take absolute value
					SIMD_AND( x, xmm_abs_mask )
					# Accumulate
					SIMD_ADD( acc, x )

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					xmm_abs_mask = SSERegister()
					if ctype.get_size() == 4:
						LOAD.CONSTANT( xmm_abs_mask, Constant.uint32x4(0x7FFFFFFF))
					else:
						LOAD.CONSTANT( xmm_abs_mask, Constant.uint64x2(0x7FFFFFFFFFFFFFFFL))

					unroll_registers  = 7
					register_size     = 16
					acc  = [SSERegister() for _ in range(unroll_registers)]
					temp = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 3, 4)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_AND( temp[i], xmm_abs_mask )
						with instruction_columns[2]:
							SIMD_ADD( acc[i], temp[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )

					scalar_function = lambda accumulator, x_pointer: PROCESS_SCALAR(accumulator, x_pointer, xmm_abs_mask)
					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, scalar_function, REDUCE.SUM, instruction_columns, instruction_offsets)

def SumAbs_VXf_SXf_AVX(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['SumAbs']:
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				else:
					ctype = x_type

				SIMD_LOAD  = {4: VMOVUPS, 8: VMOVUPD}[x_type.get_size()]
				SIMD_ADD   = {4: VADDPS, 8: VADDPD}[x_type.get_size()]
				SIMD_AND   = {4: VANDPS, 8: VANDPD}[x_type.get_size()]

				def PROCESS_SCALAR(acc, xPointer, ymm_abs_mask):
					# Load x
					x = SSERegister()
					LOAD.ELEMENT( x, [xPointer], x_type )
					# Take absolute value
					SIMD_AND( x, ymm_abs_mask.get_oword() )
					# Accumulate
					if isinstance(acc, AVXRegister):
						SIMD_ADD( acc, x.get_hword() )
					else:
						SIMD_ADD( acc, x )

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					ymm_abs_mask = AVXRegister()
					if ctype.get_size() == 4:
						LOAD.CONSTANT( ymm_abs_mask, Constant.uint32x8(0x7FFFFFFF))
					else:
						LOAD.CONSTANT( ymm_abs_mask, Constant.uint64x4(0x7FFFFFFFFFFFFFFFL))

					unroll_registers  = 7
					register_size     = 32
					acc  = [AVXRegister() for _ in range(unroll_registers)]
					temp = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 4, 5)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_AND( temp[i], ymm_abs_mask )
						with instruction_columns[2]:
							SIMD_ADD( acc[i], temp[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )

					scalar_function = lambda accumulator, x_pointer: PROCESS_SCALAR(accumulator, x_pointer, ymm_abs_mask)
					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, scalar_function, REDUCE.SUM, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					ymm_abs_mask = AVXRegister()
					if ctype.get_size() == 4:
						LOAD.CONSTANT( ymm_abs_mask, Constant.uint32x8(0x7FFFFFFF))
					else:
						LOAD.CONSTANT( ymm_abs_mask, Constant.uint64x4(0x7FFFFFFFFFFFFFFFL))

					unroll_registers  = 6
					acc  = [AVXRegister() if i % 3 == 2 else SSERegister() for i in range(unroll_registers)]
					temp = [AVXRegister() if i % 3 == 2 else SSERegister() for i in range(unroll_registers)]

					instruction_offsets = (0, 3, 4)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					offset = 0 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + offset] )
						with instruction_columns[1]:
							if isinstance(temp[i], AVXRegister):
								SIMD_AND( temp[i], ymm_abs_mask )
							else:
								SIMD_AND( temp[i], ymm_abs_mask.get_oword() )
						with instruction_columns[2]:
							SIMD_ADD( acc[i], temp[i] )
						offset += acc[i].get_size()
					with instruction_columns[0]:
						ADD( xPointer, sum(register.get_size() for register in acc) )

					scalar_function = lambda accumulator, x_pointer: PROCESS_SCALAR(accumulator, x_pointer, ymm_abs_mask)
					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, scalar_function, REDUCE.SUM, instruction_columns, instruction_offsets)

def Sum_VXf_SXf_SSE(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['Sum']:
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				else:
					ctype = x_type

				SIMD_LOAD  = {4: MOVUPS, 8: MOVUPD}[x_type.get_size()]
				SIMD_ADD   = {4: ADDPS, 8: ADDPD}[x_type.get_size()]

				def PROCESS_SCALAR(acc, xPointer):
					# Load x
					x = SSERegister()
					LOAD.ELEMENT( x, [xPointer], x_type )
					# Accumulate
					SIMD_ADD( acc, x )

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 8
					register_size     = 16
					acc  = [SSERegister() for _ in range(unroll_registers)]
					temp = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 5)
					instruction_columns = [InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_ADD( acc[i], temp[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )

					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

def Sum_VXf_SXf_AVX(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['Sum']:
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				else:
					ctype = x_type

				SIMD_LOAD  = {4: VMOVUPS, 8: VMOVUPD}[x_type.get_size()]
				SIMD_ADD   = {4: VADDPS, 8: VADDPD}[x_type.get_size()]

				def PROCESS_SCALAR(acc, xPointer):
					# Load x
					x = SSERegister()
					LOAD.ELEMENT( x, [xPointer], x_type )
					# Accumulate
					if isinstance(acc, AVXRegister):
						SIMD_ADD( acc, x.get_hword() )
					else:
						SIMD_ADD( acc, x )

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 8
					register_size     = 32
					acc  = [AVXRegister() for _ in range(unroll_registers)]
					temp = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 4)
					instruction_columns = [InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_ADD( acc[i], temp[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )

					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 6
					acc  = [AVXRegister() if i % 3 == 2 else SSERegister() for i in range(unroll_registers)]
					temp = [AVXRegister() if i % 3 == 2 else SSERegister() for i in range(unroll_registers)]

					instruction_offsets = (0, 3)
					instruction_columns = [InstructionStream(), InstructionStream()] 
					offset = 0 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + offset])
						with instruction_columns[1]:
							SIMD_ADD( acc[i], temp[i] )
						offset += acc[i].get_size()
					with instruction_columns[0]:
						ADD( xPointer, sum(register.get_size() for register in acc) )

					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

def SumSquares_VXf_SXf_SSE(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['SumSquares']:
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				else:
					ctype = x_type

				SCALAR_MUL = {4: MULSS, 8: MULSD}[x_type.get_size()]
				SIMD_LOAD  = {4: MOVUPS, 8: MOVUPD}[x_type.get_size()]
				SIMD_ADD   = {4: ADDPS, 8: ADDPD}[x_type.get_size()]
				SIMD_MUL   = {4: MULPS, 8: MULPD}[x_type.get_size()]

				def PROCESS_SCALAR(acc, xPointer):
					# Load x
					temp = SSERegister()
					LOAD.ELEMENT( temp, [xPointer], x_type )
					# Square x
					SCALAR_MUL( temp, temp )
					# Accumulate
					SIMD_ADD( acc, temp )

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 8
					register_size     = 16
					acc  = [SSERegister() for _ in range(unroll_registers)]
					temp = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 2, 5)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_MUL( temp[i], temp[i] )
						with instruction_columns[2]:
							SIMD_ADD( acc[i], temp[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )

					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

def SumSquares_VXf_SXf_AVX(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['SumSquares']:
				x_argument, y_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type)):
					return

				if len(set([x_type, y_type])) != 1:
					return
				else:
					ctype = x_type

				SCALAR_MUL = {4: VMULSS, 8: VMULSD}[x_type.get_size()]
				SIMD_LOAD  = {4: VMOVUPS, 8: VMOVUPD}[x_type.get_size()]
				SIMD_ADD   = {4: VADDPS, 8: VADDPD}[x_type.get_size()]
				SIMD_MUL   = {4: VMULPS, 8: VMULPD}[x_type.get_size()]
				SIMD_FMA4  = {4: VFMADDPS, 8: VFMADDPD}[x_type.get_size()]
				SIMD_FMA3  = {4: VFMADD231PS, 8: VFMADD231PD}[x_type.get_size()]

				def PROCESS_SCALAR(acc, xPointer):
					if Target.has_fma():
						# Load x
						x = SSERegister()
						LOAD.ELEMENT( x, [xPointer], x_type )
						# Square x and accumulate
						if Target.has_fma4():
							if isinstance(acc, AVXRegister):
								SIMD_FMA4( acc, x.get_hword(), x.get_hword(), acc )
							else:
								SIMD_FMA4( acc, x, x, acc )
						else:
							SIMD_FMA3( acc, x.get_hword(), x.get_hword(), acc )
					else:
						# Load x
						temp = SSERegister()
						LOAD.ELEMENT( temp, [xPointer], x_type )
						# Square x
						SCALAR_MUL( temp, temp )
						# Accumulate
						SIMD_ADD( acc, temp.get_hword() )

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 8
					register_size     = 32
					acc  = [AVXRegister() for _ in range(unroll_registers)]
					temp = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 2, 5)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_MUL( temp[i], temp[i] )
						with instruction_columns[2]:
							SIMD_ADD( acc[i], temp[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )

					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 9
					acc  = [SSERegister() if i % 3 != 2 else AVXRegister() for i in range(unroll_registers)]
					temp = [SSERegister() if i % 3 != 2 else AVXRegister() for i in range(unroll_registers)]

					instruction_offsets = (0, 3)
					instruction_columns = [InstructionStream(), InstructionStream()]
					offset = 0 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + offset])
						with instruction_columns[1]:
							SIMD_FMA4( acc[i], temp[i], temp[i], acc[i] )
						offset += acc[i].get_size()
					with instruction_columns[0]:
						ADD( xPointer, sum(register.get_size() for register in acc) )

					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 8
					register_size     = 32
					acc  = [AVXRegister() for _ in range(unroll_registers)]
					temp = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 3)
					instruction_columns = [InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_FMA3( acc[i], temp[i], temp[i], acc[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )

					PipelineReduce_VXf_SXf(xPointer, yPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)
				
def PipelineReduce_VXfVXf_SXf(xPointer, yPointer, zPointer, length, accumulators, ctype, scalar_function, reduction_function, instruction_columns, instruction_offsets, use_simd = True):
	# Check that we have an offset for each instruction column
	assert len(instruction_columns) == len(instruction_offsets)

	max_instructions  = max(map(len, instruction_columns))
	max_register_size = max(register.get_size() for register in accumulators)
	if use_simd:
		batch_bytes       = sum(register.get_size() for register in accumulators)
		batch_elements    = batch_bytes / ctype.get_size()
	else:
		batch_bytes       = len(accumulators) * ctype.get_size()
		batch_elements    = len(accumulators)
	
	source_y_aligned          = Label("source_y_%sb_aligned" % max_register_size)
	source_y_misaligned       = Label("source_y_%sb_misaligned" % max_register_size)
	return_ok                 = Label("return_ok")
	return_null_pointer       = Label("return_null_pointer")
	return_misaligned_pointer = Label("return_misaligned_pointer")
	return_any                = Label("return")
	reduce_batch              = Label("reduce_batch")
	batch_process_finish      = Label("batch_process_finish")
	process_single            = Label("process_single")
	process_batch             = Label("process_batch")
	process_batch_prologue    = Label("process_batch_prologue") 
	process_batch_epilogue    = Label("process_batch_epilogue") 

	# Check parameters
	TEST( xPointer, xPointer )
	JZ( return_null_pointer )
	
	TEST( xPointer, ctype.get_size() - 1 )
	JNZ( return_misaligned_pointer )
	
	TEST( yPointer, yPointer )
	JZ( return_null_pointer )
	
	TEST( yPointer, ctype.get_size() - 1 )
	JNZ( return_misaligned_pointer )

	TEST( zPointer, zPointer )
	JZ( return_null_pointer )
	
	TEST( zPointer, ctype.get_size() - 1 )
	JNZ( return_misaligned_pointer )

	LOAD.ZERO( accumulators[0], ctype )

	# If length is zero, return immediately
	TEST( length, length )
	JZ( return_ok )

	# Initialize accumulators to zero
	for accumulator in accumulators[1:]:
		LOAD.ZERO( accumulator, ctype )

	if use_simd:
		# If the y pointer is not aligned by register size, process by one element until it becomes aligned
		TEST( yPointer, max_register_size - 1 )
		JZ( source_y_aligned )
	
		LABEL( source_y_misaligned )
		scalar_function(accumulators[0], xPointer, yPointer)
		ADD( xPointer, ctype.get_size() )
		ADD( yPointer, ctype.get_size() )
		SUB( length, 1 )
		JZ( reduce_batch )
	
		TEST( yPointer, max_register_size - 1 )
		JNZ( source_y_misaligned )
	
		LABEL( source_y_aligned )

	SUB( length, batch_elements )
	JB( batch_process_finish )

	LABEL( process_batch_prologue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i >= instruction_offset:
				Function.get_current().add_instruction(instruction_column[i - instruction_offset])

	SUB( length, batch_elements )
	JB( process_batch_epilogue )

	ALIGN( 16 )
	LABEL( process_batch )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	SUB( length, batch_elements )
	JAE( process_batch )

	LABEL( process_batch_epilogue )
	for i in range(max_instructions):
		for instruction_column, instruction_offset in zip(instruction_columns, instruction_offsets):
			if i < instruction_offset:
				Function.get_current().add_instruction(instruction_column[(i - instruction_offset) % max_instructions])

	LABEL( batch_process_finish )
	ADD( length, batch_elements )
	JZ( reduce_batch )

	LABEL( process_single )
	scalar_function(accumulators[0], xPointer, yPointer)
	ADD( xPointer, ctype.get_size() )
	ADD( yPointer, ctype.get_size() )
	SUB( length, 1 )
	JNZ( process_single )

	LABEL( reduce_batch )
	reduction_function(accumulators, ctype, ctype)
	
	LABEL( return_ok )

	STORE.ELEMENT( [zPointer], accumulators[0], ctype )
	XOR( eax, eax )
	
	LABEL( return_any )
	RETURN()

	LABEL( return_null_pointer )
	MOV( eax, 1 )
	JMP( return_any )
	
	LABEL( return_misaligned_pointer )
	MOV( eax, 2 )
	JMP( return_any )

def DotProduct_VXfVXf_SXf_SSE(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['DotProduct']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()
				z_type = z_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type, z_type)):
					return

				if len(set([x_type, y_type, z_type])) != 1:
					return
				else:
					ctype = x_type

				SCALAR_MUL  = {4: MULSS, 8: MULSD}[x_type.get_size()]
				SIMD_LOAD   = {4: MOVUPS, 8: MOVUPD}[x_type.get_size()]
				SIMD_ADD    = {4: ADDPS, 8: ADDPD}[x_type.get_size()]
				SIMD_MUL    = {4: MULPS, 8: MULPD}[x_type.get_size()]

				def PROCESS_SCALAR(acc, xPointer, yPointer):
					# Load x
					temp = SSERegister()
					LOAD.ELEMENT( temp, [xPointer], x_type )
					# Load y and multiply
					SCALAR_MUL( temp, [yPointer] )
					# Accumulate
					SIMD_ADD( acc, temp )

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Nehalem', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 8
					register_size     = 16
					acc  = [SSERegister() for _ in range(unroll_registers)]
					temp = [SSERegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 2, 5)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_MUL( temp[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							SIMD_ADD( acc[i], temp[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )

					PipelineReduce_VXfVXf_SXf(xPointer, yPointer, zPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)
					
				if ctype.get_size() == 8:
					with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bonnell', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
						xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()
	
						unroll_registers  = 8
						acc  = [SSERegister() for _ in range(unroll_registers)]
						temp = [SSERegister() for _ in range(unroll_registers)]
	
						instruction_offsets = (0, 1, 5)
						instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
						for i in range(unroll_registers):
							with instruction_columns[0]:
								MOVSD( temp[i], [xPointer + i * ctype.get_size()] )
							with instruction_columns[1]:
								MULSD( temp[i], [yPointer + i * ctype.get_size()] )
							with instruction_columns[2]:
								ADDSD( acc[i], temp[i] )
						with instruction_columns[0]:
							ADD( xPointer, ctype.get_size() * unroll_registers )
						with instruction_columns[1]:
							ADD( yPointer, ctype.get_size() * unroll_registers )
	
						PipelineReduce_VXfVXf_SXf(xPointer, yPointer, zPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets, use_simd = False)

def DotProduct_VXfVXf_SXf_AVX(codegen, function_signature, module, function, arguments, assembly_cache = dict(), error_diagnostics_mode = False):
	if codegen.abi.name in ['x64-ms', 'x64-sysv']:
		if module == 'Core':
			if function in ['DotProduct']:
				x_argument, y_argument, z_argument, length_argument = tuple(arguments)

				x_type = x_argument.get_type().get_primitive_type()
				y_type = y_argument.get_type().get_primitive_type()
				z_type = z_argument.get_type().get_primitive_type()

				if not all(type.is_floating_point() for type in (x_type, y_type, z_type)):
					return

				if len(set([x_type, y_type, z_type])) != 1:
					return
				else:
					ctype = x_type

				SCALAR_MUL = {4: VMULSS, 8: VMULSD}[x_type.get_size()]
				SIMD_LOAD  = {4: VMOVUPS, 8: VMOVUPD}[x_type.get_size()]
				SIMD_ADD   = {4: VADDPS, 8: VADDPD}[x_type.get_size()]
				SIMD_MUL   = {4: VMULPS, 8: VMULPD}[x_type.get_size()]
				SIMD_FMA4  = {4: VFMADDPS, 8: VFMADDPD}[x_type.get_size()]
				SIMD_FMA3  = {4: VFMADD231PS, 8: VFMADD231PD}[x_type.get_size()]

				def PROCESS_SCALAR(acc, xPointer, yPointer):
					if Target.has_fma():
						# Load x
						x = SSERegister()
						LOAD.ELEMENT( x, [xPointer], x_type )
						# Load y
						y = SSERegister()
						LOAD.ELEMENT( y, [yPointer], y_type )
						# Multiply-accumulate
						if Target.has_fma4():
							if isinstance(acc, AVXRegister):
								SIMD_FMA4( acc, x.get_hword(), y.get_hword(), acc )
							else:
								SIMD_FMA4( acc, x, y, acc )
						else:
							SIMD_FMA3( acc, x.get_hword(), y.get_hword(), acc )
					else:
						# Load x
						temp = SSERegister()
						LOAD.ELEMENT( temp, [xPointer], x_type )
						# Load y and multiply
						SCALAR_MUL( temp, [yPointer] )
						# Accumulate
						SIMD_ADD( acc, temp.get_hword() )

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'SandyBridge', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 8
					register_size     = 32
					acc  = [AVXRegister() for _ in range(unroll_registers)]
					temp = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 2, 5)
					instruction_columns = [InstructionStream(), InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_MUL( temp[i], [yPointer + i * register_size] )
						with instruction_columns[2]:
							SIMD_ADD( acc[i], temp[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )

					PipelineReduce_VXfVXf_SXf(xPointer, yPointer, zPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Bulldozer', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 6
					acc  = [SSERegister() if i % 3 != 2 else AVXRegister() for i in range(unroll_registers)]
					temp = [SSERegister() if i % 3 != 2 else AVXRegister() for i in range(unroll_registers)]

					instruction_offsets = (0, 3)
					instruction_columns = [InstructionStream(), InstructionStream()]
					offset = 0 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + offset])
						with instruction_columns[1]:
							SIMD_FMA4( acc[i], temp[i], [yPointer + offset], acc[i] )
						offset += acc[i].get_size()
					with instruction_columns[0]:
						ADD( xPointer, sum(register.get_size() for register in acc) )
					with instruction_columns[1]:
						ADD( yPointer, sum(register.get_size() for register in acc) )

					PipelineReduce_VXfVXf_SXf(xPointer, yPointer, zPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

				with Function(codegen, "yep" + module + "_" + function + "_" + function_signature, arguments, 'Haswell', assembly_cache = assembly_cache, collect_origin = bool(error_diagnostics_mode), check_only = bool(error_diagnostics_mode)):
					xPointer, yPointer, zPointer, length = LOAD.PARAMETERS()

					unroll_registers  = 8
					register_size     = 32
					acc  = [AVXRegister() for _ in range(unroll_registers)]
					temp = [AVXRegister() for _ in range(unroll_registers)]

					instruction_offsets = (0, 3)
					instruction_columns = [InstructionStream(), InstructionStream()] 
					for i in range(unroll_registers):
						with instruction_columns[0]:
							SIMD_LOAD( temp[i], [xPointer + i * register_size] )
						with instruction_columns[1]:
							SIMD_FMA3( acc[i], temp[i], [yPointer + i * register_size], acc[i] )
					with instruction_columns[0]:
						ADD( xPointer, register_size * unroll_registers )
					with instruction_columns[1]:
						ADD( yPointer, register_size * unroll_registers )

					PipelineReduce_VXfVXf_SXf(xPointer, yPointer, zPointer, length, acc, ctype, PROCESS_SCALAR, REDUCE.SUM, instruction_columns, instruction_offsets)

