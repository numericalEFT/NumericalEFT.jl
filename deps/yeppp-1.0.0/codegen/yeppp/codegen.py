#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

import peachpy
import peachpy.codegen
import peachpy.c
import peachpy.java
import peachpy.fortran
import peachpy.csharp
import peachpy.doxygen
import string
import copy
import re
import yeppp.test

def format_or_list(elements, prefix = "", suffix = ""):
	elements = [prefix + str(element) + suffix for element in elements]
	if len(elements) <= 2:
		return " or ".join(elements)
	else:
		return ", ".join(elements[:-1]) + " or " + elements[-1]
	

class FunctionArgument(object):
	def __init__(self, name, type = None):
		# Argument name as specified in function signature
		self.declared_name = name
		# Argument type as string as specified in function signature (None if not specified)
		self.declared_type = type
		# The following member variables contain a list of tuples (name, type)
		# with the expansion of this Yeppp! function argument in different contexts and languages

		# Arguments used for public C headers 
		self.c_public_arguments = list()
		# Arguments used for internal C code (e.g. function implementation)
		self.c_private_arguments = list()
		# Arguments used for Java bindings (these map 1-to-1 to JNI types for the C part of bindings)
		self.java_arguments = list()
		# Arguments used for FORTRAN bindings (almost the same as public C arguments)
		self.fortran_arguments = list()
		# Arguments used for DllImport declaration in C#
		self.csharp_dllimport_arguments = list()
		# Arguments used for C# bindings with pointers
		self.csharp_unsafe_arguments = list()
		# Arguments used for C# bindings without pointers
		self.csharp_safe_arguments = list()
		# True if the argument is converted to function return value in Java/C# bindings
		self.is_return_argument = False
		# True if the argument specifies the length of an array argument
		self.is_length_argument = False

		# True if the argument is marked as != 0
		self.is_nonzero = False

	def is_automatic(self):
		# True is the argument type is deduced from function signature
		return self.declared_type is None

	def get_name(self):
		return self.declared_name

	def get_c_public_name(self, index = None):
		if index is None:
			if len(self.c_public_arguments) == 1:
				return self.c_public_arguments[0].name
			else:
				raise ValueError("The function argument %s expands to multiple public C arguments" % self.get_name())
		else:
			return self.c_public_arguments[index].name

	def get_c_public_type(self, index = None):
		if index is None:
			if len(self.c_public_arguments) == 1:
				return self.c_public_arguments[0].type
			else:
				raise ValueError("The function argument %s expands to multiple public C arguments" % self.get_name())
		else:
			return self.c_public_arguments[index].type

	def get_c_private_name(self, index = None):
		if index is None:
			if len(self.c_private_arguments) == 1:
				return self.c_private_arguments[0].name
			else:
				raise ValueError("The function argument %s expands to multiple private C arguments" % self.get_name())
		else:
			return self.c_private_arguments[index].name

	def get_c_private_type(self, index = None):
		if index is None:
			if len(self.c_private_arguments) == 1:
				return self.c_private_arguments[0].type
			else:
				raise ValueError("The function argument %s expands to multiple private C arguments" % self.get_name())
		else:
			return self.c_private_arguments[index].type

	def get_java_name(self, index = None):
		if index is None:
			if len(self.java_arguments) == 1:
				return self.java_arguments[0].name
			else:
				raise ValueError("The function argument %s expands to multiple Java arguments" % self.get_name())
		else:
			return self.java_arguments[index].name

	def get_java_type(self, index = None):
		if index is None:
			if len(self.java_arguments) == 1:
				return self.java_arguments[0].type
			else:
				raise ValueError("The function argument %s expands to multiple Java arguments" % self.get_name())
		else:
			return self.java_arguments[index].type

	def get_fortran_name(self, index = None):
		if index is None:
			if len(self.fortran_arguments) == 1:
				return self.fortran_arguments[0].name
			else:
				raise ValueError("The function argument %s expands to multiple FORTRAN arguments" % self.get_name())
		else:
			return self.fortran_arguments[index].name

	def get_fortran_type(self, index = None):
		if index is None:
			if len(self.fortran_arguments) == 1:
				return self.fortran_arguments[0].type
			else:
				raise ValueError("The function argument %s expands to multiple FORTRAN arguments" % self.get_name())
		else:
			return self.fortran_arguments[index].type

	def get_csharp_dllimport_name(self, index = None):
		if index is None:
			if len(self.csharp_dllimport_arguments) == 1:
				return self.csharp_dllimport_arguments[0].name
			else:
				raise ValueError("The function argument %s expands to multiple DllImport C# arguments" % self.get_name())
		else:
			return self.csharp_dllimport_arguments[index].name

	def get_csharp_dllimport_type(self, index = None):
		if index is None:
			if len(self.csharp_dllimport_arguments) == 1:
				return self.csharp_dllimport_arguments[0].type
			else:
				raise ValueError("The function argument %s expands to multiple DllImport C# arguments" % self.get_name())
		else:
			return self.csharp_dllimport_arguments[index].type

	def get_csharp_unsafe_name(self, index = None):
		if index is None:
			if len(self.csharp_unsafe_arguments) == 1:
				return self.csharp_unsafe_arguments[0].name
			else:
				raise ValueError("The function argument %s expands to multiple unsafe C# arguments" % self.get_name())
		else:
			return self.csharp_unsafe_arguments[index].name

	def get_csharp_unsafe_type(self, index = None):
		if index is None:
			if len(self.csharp_unsafe_arguments) == 1:
				return self.csharp_unsafe_arguments[0].type
			else:
				raise ValueError("The function argument %s expands to multiple unsafe C# arguments" % self.get_name())
		else:
			return self.csharp_unsafe_arguments[index].type

	def get_csharp_safe_name(self, index = None):
		if index is None:
			if len(self.csharp_safe_arguments) == 1:
				return self.csharp_safe_arguments[0].name
			else:
				raise ValueError("The function argument %s expands to multiple safe C# arguments" % self.get_name())
		else:
			return self.csharp_safe_arguments[index].name

	def get_csharp_safe_type(self, index = None):
		if index is None:
			if len(self.csharp_safe_arguments) == 1:
				return self.csharp_safe_arguments[0].type
			else:
				raise ValueError("The function argument %s expands to multiple safe C# arguments" % self.get_name())
		else:
			return self.csharp_safe_arguments[index].type


class ExplicitlyTypedFunctionArgument(FunctionArgument):
	def __init__(self, name, type):
		super(ExplicitlyTypedFunctionArgument, self).__init__(name, type)
		c_type = peachpy.c.Type(type)
		self.c_public_arguments = [peachpy.c.Parameter(name, c_type)]
		if c_type.is_pointer():
			raise ValueError("Invalid argument %s: only implicitly types arguments can be of pointer type" % name)
		else:
			self.c_private_arguments = [peachpy.c.Parameter(name, c_type)]
		java_type = c_type.get_java_analog()
		if java_type:
			self.java_arguments = [peachpy.java.Parameter(name, java_type)]
		else:
			self.java_arguments = None
		fortran_type = c_type.get_fortran_iso_c_analog()
		if fortran_type:
			self.fortran_arguments = [peachpy.fortran.Parameter(name, fortran_type, is_input = True, is_output = False)]
		else:
			self.fortran_arguments = None
		csharp_dllimport_type = c_type.get_csharp_analog(use_unsafe_types = True)
		if csharp_dllimport_type:
			self.csharp_dllimport_arguments = [peachpy.csharp.Parameter(name, csharp_dllimport_type, is_output = False)]
		else:
			self.csharp_dllimport_arguments = None
		csharp_unsafe_type = c_type.get_csharp_analog(size_t_analog = "int", ssize_t_analog = "int")
		if csharp_unsafe_type:
			self.csharp_unsafe_arguments = [peachpy.csharp.Parameter(name, csharp_unsafe_type, is_output = False)]
		else:
			self.csharp_unsafe_arguments = None
		csharp_safe_type = c_type.get_csharp_analog(size_t_analog = "int", ssize_t_analog = "int")
		if csharp_safe_type:
			self.csharp_safe_arguments = [peachpy.csharp.Parameter(name, csharp_safe_type, is_output = False)]
		else:
			self.csharp_safe_arguments = None

	def __str__(self):
		return "%s %s" % (self.declared_type, self.declared_name)

class ImplicitlyTypedFunctionArgument(FunctionArgument):
	def __init__(self, name, type_abbreviation, is_output, length_argument_name = None):
		super(ImplicitlyTypedFunctionArgument, self).__init__(name)
		self.type_abbreviation = type_abbreviation
		# In-place arguments are both input and output 
		self.is_inplace = type_abbreviation.startswith("I")
		self.is_output = is_output or self.is_inplace
		self.is_input = not is_output or self.is_inplace
		if self.is_inplace:
			type_abbreviation = type_abbreviation[1:]

		self.is_vector = type_abbreviation[0] == "V"
		self.is_scalar = type_abbreviation[0] == "S"
		if self.is_vector:
			self.length_argument_name = length_argument_name if length_argument_name else "length"
		type_abbreviation = type_abbreviation[1:]

		type = "Yep" + type_abbreviation
		if self.is_vector or self.is_output:
			type = type + "*"
			if not self.is_output:
				type = "const " + type
		c_type = peachpy.c.Type(type)

		self.c_public_arguments = [peachpy.c.Parameter(name, c_type)]
		if c_type.is_pointer():
			self.c_private_arguments = [peachpy.c.Parameter(name + "Pointer", c_type)]
		else:
			self.c_private_arguments = [peachpy.c.Parameter(name, c_type)]
		java_type = c_type.get_java_analog()
		if java_type:
			if java_type.is_array():
				self.java_arguments = [peachpy.java.Parameter(self.declared_name + "Array", java_type), peachpy.java.Parameter(self.declared_name + "Offset", peachpy.java.Type("int"))]
			else:
				self.java_arguments = [peachpy.java.Parameter(self.declared_name, java_type)]
		else:
			self.java_arguments = None
		if self.is_vector:
			fortran_type = c_type.get_fortran_iso_c_analog()
		else:
			fortran_type = c_type.get_primitive_type().get_fortran_iso_c_analog()
		if fortran_type:
			if self.is_vector:
				fortran_type.set_dimension(self.length_argument_name)
			self.fortran_arguments = [peachpy.fortran.Parameter(self.declared_name, fortran_type, is_input = self.is_input, is_output = self.is_output)]
		else:
			self.fortran_arguments = None

		if self.is_vector:
			csharp_dllimport_type = c_type.get_csharp_analog(use_unsafe_types = True)
		else:
			csharp_dllimport_type = c_type.get_primitive_type().get_csharp_analog()
		if csharp_dllimport_type:
			self.csharp_dllimport_arguments = [peachpy.csharp.Parameter(name, csharp_dllimport_type, is_output = self.is_scalar and self.is_output and not self.is_input)]
		else:
			self.csharp_dllimport_arguments = None
		if self.is_vector:
			csharp_dllimport_type = c_type.get_csharp_analog(use_unsafe_types = True)
		else:
			csharp_dllimport_type = c_type.get_primitive_type().get_csharp_analog()

		if self.is_vector:
			csharp_unsafe_type = c_type.get_csharp_analog(use_unsafe_types = True)
		else:
			csharp_unsafe_type = c_type.get_primitive_type().get_csharp_analog(use_unsafe_types = True)
		if csharp_unsafe_type:
			self.csharp_unsafe_arguments = [peachpy.csharp.Parameter(name, csharp_unsafe_type, is_output = self.is_scalar and self.is_output and not self.is_input)]
		else:
			self.csharp_unsafe_arguments = None

		if self.is_vector:
			csharp_safe_type = c_type.get_csharp_analog(use_unsafe_types = False)
		else:
			csharp_safe_type = c_type.get_primitive_type().get_csharp_analog(use_unsafe_types = False)
		if csharp_safe_type:
			if csharp_safe_type.is_array():
				self.csharp_safe_arguments = [peachpy.csharp.Parameter(name + "Array", csharp_safe_type, is_output = False),
											  peachpy.csharp.Parameter(name + "Offset", peachpy.csharp.Type("int"))]
			else:
				self.csharp_safe_arguments = [peachpy.csharp.Parameter(name, csharp_safe_type, is_output = self.is_output and not self.is_input)]
		else:
			self.csharp_safe_arguments = None

	def __str__(self):
		return "%s: %s" % (self.declared_name, self.type_abbreviation)

class FunctionSpecialization:
	def __init__(self, declaration):
		function_name_matcher = re.match("yep([A-Za-z]+)_([A-Za-z]+)_", declaration)
		if function_name_matcher is None:
			raise ValueError("Function declaration {0} does not follow Yeppp! naming convention".format(declaration))
		else:
			self.module_name = function_name_matcher.group(1)
			self.function_name = function_name_matcher.group(2)
			arguments_declaration = declaration[function_name_matcher.end():]
			self.specialization_signature = declaration[function_name_matcher.end():declaration.index('(')]
			type_abbreviation_regex = "[I]?[VS](?:8[us]|16[usf]|32[usf]|64[usf]|128[us]|16fc|32fc|64fc|32df|64df)"
			inputs_abbreviations = list()
			abbreviation_matcher = re.match(type_abbreviation_regex, arguments_declaration)
			while abbreviation_matcher:
				arguments_declaration = arguments_declaration[abbreviation_matcher.end():]
				inputs_abbreviations.append(abbreviation_matcher.group())
				abbreviation_matcher = re.match(type_abbreviation_regex, arguments_declaration)
			self.inputs_abbreviations = [re.match("[I]?[VS](.+)", input_abbreviation).group(1) for input_abbreviation in inputs_abbreviations]
			if not arguments_declaration.startswith("_"):
				raise ValueError("Function declaration {0} does not contain separator between inputs and outputs".format(declaration))
			else:
				arguments_declaration = arguments_declaration[1:]
				outputs_abbreviations = list()
				abbreviation_matcher = re.match(type_abbreviation_regex, arguments_declaration)
				while abbreviation_matcher:
					arguments_declaration = arguments_declaration[abbreviation_matcher.end():]
					outputs_abbreviations.append(abbreviation_matcher.group())
					abbreviation_matcher = re.match(type_abbreviation_regex, arguments_declaration)
				self.outputs_abbreviations = [re.match("[I]?[VS](.+)", output_abbreviation).group(1) for output_abbreviation in outputs_abbreviations]
				# Check that the numbers of in-place arguments in the input and output sections is the same
				if sum(input.startswith("I") for input in inputs_abbreviations) != sum(output.startswith("I") for output in outputs_abbreviations):
					raise ValueError("Function declaration {0} contains different number of intput and output in-place arguments".format(declaration))
				else:
					if not arguments_declaration.startswith("(") or not arguments_declaration.endswith(")"):
						raise ValueError("Function declaration {0} misses argument list".format(declaration))
					else:
						self.c_function_signature = declaration[:declaration.index("(")]
						self.short_function_signature = self.c_function_signature[len("yep" + self.module_name + "_"):]
						arguments_declaration = map(string.strip, arguments_declaration[1:-1].split(","))
						self.arguments = list()
						while arguments_declaration:
							argument_string = arguments_declaration.pop(0)
							if argument_string.find(":") != -1:
								restriction_string = argument_string[argument_string.index(":") + 1:].strip()
								argument_string = argument_string[:argument_string.index(":")]
							else:
								restriction_string = None
							name_matcher = re.search("([A-Za-z_][A-Za-z0-9_]*)(\[[A-Za-z_][A-Za-z0-9_]+\])?$", argument_string)
							if not name_matcher:
								raise ValueError("Invalid name for argument {0}".format(argument_string))
							else:
								name = name_matcher.group(1)
								type_string = argument_string[:name_matcher.start()].strip()
								if name_matcher.group(2):
									length_argument_name = name_matcher.group(2)[1:-1]
								else:
									length_argument_name = None
								if type_string:
									if length_argument_name:
										raise ValueError("Length specification is allowed only for implicitly types arguments")
									else:
										argument = ExplicitlyTypedFunctionArgument(name, type_string)
										if restriction_string:
											if restriction_string.startswith(name):
												new_restriction_string = restriction_string[len(name):].lstrip()
												if new_restriction_string.startswith("!="):
													new_restriction_string = new_restriction_string[2:].lstrip()
													if new_restriction_string == "0":
														argument.is_nonzero = True
													else:
														raise ValueError("Invalid restriction {0} for argument {1}".format(restriction_string, name))
												else:
													raise ValueError("Invalid restriction {0} for argument {1}".format(restriction_string, name))
											else:
												raise ValueError("Invalid restriction {0} for argument {1}".format(restriction_string, name))
										self.arguments.append(argument)
								else:
									# Consider input arguments if there are any, then output arguments
									is_output_parameter = not inputs_abbreviations
									if is_output_parameter:
										type_abbreviation = outputs_abbreviations.pop(0)
									else:
										type_abbreviation = inputs_abbreviations.pop(0)
										# If this is an in-place argument, remove the matching abbreviation from output list
										if type_abbreviation.startswith("I"):
											outputs_abbreviations.remove(type_abbreviation)
									self.arguments.append(ImplicitlyTypedFunctionArgument(name, type_abbreviation, is_output_parameter, length_argument_name))

						# Detect if any argument can be converted to return value
						scalar_output_arguments = filter(lambda argument: argument.is_automatic() and argument.is_scalar and argument.is_output, self.arguments)
						if len(scalar_output_arguments) == 1:
							self.return_argument = scalar_output_arguments[0]
							self.return_argument.is_return_argument = True
							self.return_argument.java_arguments = [	peachpy.java.Parameter(self.return_argument.get_name(), 
																	self.return_argument.java_arguments[0].get_type().get_primitive_type()) ]
							self.return_argument.csharp_safe_arguments = [	peachpy.csharp.Parameter(self.return_argument.get_name(),
																			self.return_argument.csharp_safe_arguments[0].get_type().get_primitive_type()) ] 
						else:
							self.return_argument = None

						# Detect which arguments are used to specify length of other arguments
						length_arguments = set([argument.length_argument_name for argument in self.arguments if argument.is_automatic() and argument.is_vector and isinstance(argument.length_argument_name, str)])
						for argument in self.arguments:
							if argument.get_name() in length_arguments:
								argument.is_length_argument = True

						# Define variables to be used by default implementation
						self.implementation_macros = dict(
							[("InputType" + str(i), input_abbreviation) for (i, input_abbreviation) in enumerate(self.inputs_abbreviations)] +
							[("OutputType" + str(i), output_abbreviation) for (i, output_abbreviation) in enumerate(self.outputs_abbreviations)])

						# Define variables to be used by documentation
						description_map = {	'8u': 'unsigned 8-bit integer',   '8s': 'signed 8-bit integer',
											'16u': 'unsigned 16-bit integer', '16s': 'signed 16-bit integer',
											'32u': 'unsigned 32-bit integer', '32s': 'signed 32-bit integer',
											'64u': 'unsigned 64-bit integer', '64s': 'signed 64-bit integer',
											'32f': 'single precision (32-bit) floating-point',
											'64f': 'double precision (64-bit) floating-point'}
						self.documentation_macros = dict(
							[("InputType" + str(i), description_map[input_abbreviation]) for (i, input_abbreviation) in enumerate(self.inputs_abbreviations)] +
							[("OutputType" + str(i), description_map[output_abbreviation]) for (i, output_abbreviation) in enumerate(self.outputs_abbreviations)])

						self.assembly_functions = dict()

						self.c_public_arguments = [c_public_argument for argument in self.arguments for c_public_argument in argument.c_public_arguments]
						self.c_private_arguments = [c_private_argument for argument in self.arguments for c_private_argument in argument.c_private_arguments]
						self.java_arguments = [java_argument for argument in self.arguments for java_argument in argument.java_arguments if not argument.is_return_argument]
						self.fortran_arguments = [fortran_argument for argument in self.arguments for fortran_argument in argument.fortran_arguments]
						self.csharp_dllimport_arguments = [csharp_dllimport_argument for argument in self.arguments for csharp_dllimport_argument in argument.csharp_dllimport_arguments]
						self.csharp_unsafe_arguments = [csharp_unsafe_argument for argument in self.arguments for csharp_unsafe_argument in argument.csharp_unsafe_arguments if not argument.is_return_argument]
						self.csharp_safe_arguments = [csharp_safe_argument for argument in self.arguments for csharp_safe_argument in argument.csharp_safe_arguments if not argument.is_return_argument]

	def generate_assembly_implementation(self, assembler, assembly_implementation, assembly_cache):
		try:
			assembly_implementation(assembler, self.specialization_signature, self.module_name, self.function_name, self.c_private_arguments, assembly_cache = assembly_cache, error_diagnostics_mode = False)
		except peachpy.RegisterAllocationError:
			assembly_implementation(assembler, self.specialization_signature, self.module_name, self.function_name, self.c_private_arguments, error_diagnostics_mode = True)
		self.assembly_functions[assembler.abi.name] = assembler.find_functions(self.c_function_signature)

	def generate_public_header(self, public_header_generator, default_documentation):
		named_arguments_list = [argument.get_type().format(compact_pointers = False, restrict_qualifier = "YEP_RESTRICT") + " " + argument.get_name()
			for argument in self.c_public_arguments] 

		if default_documentation:
			documentation = peachpy.doxygen.Documentation(default_documentation % self.documentation_macros)
			documentation.ingroup = "yep" + self.module_name + "_" + self.function_name
			documentation.retval["#YepStatusOk"] = "The computation finished successfully."
			pointer_names = [argument.get_name() for argument in self.c_public_arguments if argument.get_type().is_pointer()]
			if pointer_names:
				documentation.retval["#YepStatusNullPointer"] = "%s argument is null." % format_or_list(pointer_names, prefix = "@a ")
				documentation.retval["#YepStatusMisalignedPointer"] = "%s argument is not naturally aligned." % format_or_list(pointer_names, prefix = "@a ")
			nonzero_names = [argument.get_c_public_name() for argument in self.arguments if argument.is_nonzero]
			if nonzero_names:
				documentation.retval["#YepStatusInvalidArgument"] = "%s argument is zero." % format_or_list(nonzero_names, prefix = "@a ")

			optimized_implementations = self.get_optimized_implementations()
			if optimized_implementations:
				documentation.par["Optimized implementations"] = optimized_implementations
			else:
				documentation.add_warning("This version of @Yeppp does not include optimized implementations for this function") 
			public_header_generator.add_c_comment(str(documentation), doxygen = True)
		else:
			print "Warning: no documentation for function %s" % self.c_function_signature
		public_header_generator.add_line("YEP_PUBLIC_SYMBOL enum YepStatus YEPABI {0}({1});".format(self.c_function_signature, ", ".join(named_arguments_list)))

	def generate_dispatch_table_header(self, dispatch_table_header_generator): 
		unnamed_arguments_list = [argument.get_type().format(compact_pointers = False, restrict_qualifier = "YEP_RESTRICT") for argument in self.c_public_arguments] 
		dispatch_table_header_generator.add_line("extern \"C\" YEP_PRIVATE_SYMBOL const FunctionDescriptor<YepStatus (YEPABI*)({0})> _dispatchTable_{1}[];".format(", ".join(unnamed_arguments_list), self.c_function_signature))

	def generate_dispatch_pointer_header(self, dispatch_pointer_header_generator):
		named_arguments_list = [argument.format(compact_pointers = False, restrict_qualifier = "YEP_RESTRICT") for argument in self.c_private_arguments] 
		dispatch_pointer_header_generator.add_line("extern \"C\" YEP_PRIVATE_SYMBOL YepStatus (YEPABI* _{0})({1});".format(self.c_function_signature, ", ".join(named_arguments_list)))

	def generate_dispatch_table(self, dispatch_table_generator):
		unnamed_arguments_list = [argument.get_type().format(compact_pointers = False, restrict_qualifier = "YEP_RESTRICT") for argument in self.c_public_arguments] 
		named_arguments_list = [argument.format(compact_pointers = False, restrict_qualifier = "YEP_RESTRICT") for argument in self.c_public_arguments] 
		yeppp_abi_list = [('x86',          'YEP_X86_ABI'),
						  ('x64-ms',       'YEP_MICROSOFT_X64_ABI'),
						  ('x64-sysv',     'YEP_SYSTEMV_X64_ABI'),
						  ('x64-k1om',     'YEP_K1OM_X64_ABI'),
						  ('arm-softeabi', 'YEP_SOFTEABI_ARM_ABI'),
						  ('arm-hardeabi', 'YEP_HARDEABI_ARM_ABI')]

		dispatch_table_generator.add_line("extern \"C\" YEP_LOCAL_SYMBOL YepStatus YEPABI _{0}_Default({1});".format(self.c_function_signature, ", ".join(named_arguments_list)))
		for (abi_name, abi_test_macro) in yeppp_abi_list:
			if abi_name in self.assembly_functions and self.assembly_functions[abi_name]:
				dispatch_table_generator.add_line("#if defined(%s)" % abi_test_macro).indent()
				for assembly_function in self.assembly_functions[abi_name]:
					dispatch_table_generator.add_line("extern \"C\" YEP_LOCAL_SYMBOL YepStatus YEPABI {0}({1});".format(assembly_function.symbol_name, ", ".join(named_arguments_list)))
				dispatch_table_generator.dedent().add_line("#endif // %s" % abi_test_macro)
# 
		dispatch_table_generator.add_line("YEP_USE_DISPATCH_TABLE_SECTION const FunctionDescriptor<YepStatus (YEPABI*)({0})> _dispatchTable_{1}[] = ".format(", ".join(unnamed_arguments_list), self.c_function_signature));
		dispatch_table_generator.add_line("{")
		dispatch_table_generator.indent()

		# Descriptors for function implementations
		for (abi_name, abi_test_macro) in yeppp_abi_list:
			if abi_name in self.assembly_functions and self.assembly_functions[abi_name]:
				dispatch_table_generator.add_line("#if defined(%s)" % abi_test_macro).indent()
				for assembly_function in self.assembly_functions[abi_name]:
					isa_extensions = assembly_function.get_isa_extensions()
					(isa_features, simd_features, system_features) = assembly_function.get_yeppp_isa_extensions()
					isa_features = " | ".join(isa_features)
					simd_features = " | ".join(simd_features)
					system_features = " | ".join(system_features)
					is_amd_specific = any(amd_specific_extension in isa_extensions for amd_specific_extension in ['3dnow!', '3dnow!+', 'SSE4A', 'FMA4', 'XOP', 'TBM'])
					if is_amd_specific:
						dispatch_table_generator.add_line("#ifndef YEP_MACOSX_OS").indent()
					dispatch_table_generator.add_line("YEP_DESCRIBE_FUNCTION_IMPLEMENTATION({0}, {1}, {2}, {3}, YepCpuMicroarchitecture{4}, \"asm\", YEP_NULL_POINTER, YEP_NULL_POINTER),".
						format(assembly_function.symbol_name, isa_features, simd_features, system_features, assembly_function.target.microarchitecture.get_name()))
					if is_amd_specific:
						dispatch_table_generator.dedent().add_line("#endif // YEP_MACOSX_OS")
				dispatch_table_generator.dedent().add_line("#endif // %s" % abi_test_macro)
		dispatch_table_generator.add_line("YEP_DESCRIBE_FUNCTION_IMPLEMENTATION(_{0}_Default, YepIsaFeaturesDefault, YepSimdFeaturesDefault, YepSystemFeaturesDefault, YepCpuMicroarchitectureUnknown, \"c++\", \"Naive\", \"None\")".format(self.c_function_signature))

		dispatch_table_generator.dedent()
		dispatch_table_generator.add_line("};")
		dispatch_table_generator.add_line()

	def generate_initialization_function(self, initialization_function_generator):
		initialization_function_generator.add_line("*reinterpret_cast<FunctionPointer*>(&_{0}) = _yepLibrary_InitFunction((const FunctionDescriptor<YepStatus (*)()>*)_dispatchTable_{0});".format(self.c_function_signature))

	def generate_dispatch_pointer(self, dispatch_pointer_generator):
		unnamed_arguments_list = [argument.get_type().format(restrict_qualifier = "YEP_RESTRICT", compact_pointers = False) for argument in self.c_public_arguments] 
		dispatch_pointer_generator.add_line("YEP_USE_DISPATCH_POINTER_SECTION YepStatus (YEPABI*_{0})({1}) = YEP_NULL_POINTER;".format(self.c_function_signature, ", ".join(unnamed_arguments_list)))

	def generate_dispatch_function(self, dispatch_function_generator):
		named_arguments_list = [argument.format(compact_pointers = False, restrict_qualifier = "YEP_RESTRICT")	for argument in self.c_private_arguments] 
		argument_names = [argument.get_name() for argument in self.c_private_arguments]

		dispatch_function_generator.add_line("YEP_USE_DISPATCH_FUNCTION_SECTION YepStatus YEPABI {0}({1}) {{".format(self.c_function_signature, ", ".join(named_arguments_list)))
		dispatch_function_generator.indent().add_line("return _{0}({1});".format(self.c_function_signature, ", ".join(argument_names))).dedent()
		dispatch_function_generator.add_line("}")
		dispatch_function_generator.add_line()

	def generate_default_cpp_implementation(self, default_cpp_implementation_generator, default_cpp_implementation):
		named_arguments_list = [argument.format(compact_pointers = False, restrict_qualifier = "YEP_RESTRICT") for argument in self.c_private_arguments] 

		default_cpp_implementation_generator.add_line("extern \"C\" YEP_LOCAL_SYMBOL YepStatus _{0}_Default({1}) {{".format(self.c_function_signature, ", ".join(named_arguments_list)))
		default_cpp_implementation_generator.indent()

		# Generate parameter checks
		for argument in self.arguments:
			if argument.get_c_private_type().is_pointer():
				default_cpp_implementation_generator.add_line("if YEP_UNLIKELY({0} == YEP_NULL_POINTER) {{".format(argument.get_c_private_name()))
				default_cpp_implementation_generator.indent()
				default_cpp_implementation_generator.add_line("return YepStatusNullPointer;")
				default_cpp_implementation_generator.dedent()
				default_cpp_implementation_generator.add_line("}")
				if argument.get_c_private_type().get_primitive_type().get_size() != 1:
					default_cpp_implementation_generator.add_line("if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment({0}, sizeof({1})) != 0) {{".format(argument.get_c_private_name(), argument.get_c_private_type().get_primitive_type()))
					default_cpp_implementation_generator.indent()
					default_cpp_implementation_generator.add_line("return YepStatusMisalignedPointer;")
					default_cpp_implementation_generator.dedent()
					default_cpp_implementation_generator.add_line("}")
			elif argument.is_nonzero:
				default_cpp_implementation_generator.add_line("if YEP_UNLIKELY({0} == 0) {{".format(argument.get_c_private_name()))
				default_cpp_implementation_generator.indent()
				default_cpp_implementation_generator.add_line("return YepStatusInvalidArgument;")
				default_cpp_implementation_generator.dedent()
				default_cpp_implementation_generator.add_line("}")

		default_cpp_implementation_generator.add_lines(filter(bool, (default_cpp_implementation % self.implementation_macros).split("\n")))

		default_cpp_implementation_generator.dedent()
		default_cpp_implementation_generator.add_line("}")
		default_cpp_implementation_generator.add_line()

	def generate_jni_function(self, jni_implementation_generator):
		jni_function_signature = "Java_info_yeppp_" + str(self.module_name) + "_" + self.short_function_signature.replace("_", "_1")
		named_arguments_list = [str(argument.get_jni_analog()) for argument in self.java_arguments]

		return_type = self.return_argument.get_java_type().get_jni_analog() if self.return_argument else "void" 
		jni_implementation_generator.add_line("JNIEXPORT {0} JNICALL {1}(JNIEnv *env, jclass class, {2}) {{".format(return_type, jni_function_signature, ", ".join(named_arguments_list)))
		jni_implementation_generator.indent()

		# Start of the function. This code might be compiled in C89 mode, so all variables should be defined in the function prologue.		
		jni_implementation_generator.add_line("enum YepStatus status;")
		for argument in self.arguments:
			if argument.is_automatic() and argument.is_vector:
				# Define variables for array pointers: for each array passed to function define a corresponding pointer
				element_type = argument.get_c_private_type().get_primitive_type()
				pointer_name = argument.get_c_private_name()
				jni_implementation_generator.add_line("{0}* {1} = NULL;".format(element_type, pointer_name))
			elif argument.is_automatic() and argument.is_scalar and argument.is_output:
				# Define variables for scalar output arguments
				variable_type = argument.get_c_public_type().get_primitive_type()
				variable_name = argument.get_c_public_name()
				jni_implementation_generator.add_line("{0} {1};".format(variable_type, variable_name))
		jni_implementation_generator.add_line()

		# Check parameters:
		for argument in self.arguments:
			if argument.is_automatic() and (argument.is_vector or argument.is_scalar and argument.is_output and not argument.is_return_argument):
				array_name = argument.get_java_name(0)
				offset_name = argument.get_java_name(1)

				jni_implementation_generator.add_line("if YEP_UNLIKELY({0} == NULL) {{".format(array_name))
				jni_implementation_generator.indent()
				jni_implementation_generator.add_line("(*env)->ThrowNew(env, NullPointerException, \"Argument {0} is null\");".format(array_name))
				if self.return_argument:
					jni_implementation_generator.add_line("return ({0})0;".format(self.return_argument.get_java_type().get_jni_analog()))
				else:
					jni_implementation_generator.add_line("return;")
				jni_implementation_generator.dedent()
				jni_implementation_generator.add_line("}")

				jni_implementation_generator.add_line("if YEP_UNLIKELY({0} < 0) {{".format(offset_name))
				jni_implementation_generator.indent()
				jni_implementation_generator.add_line("(*env)->ThrowNew(env, IllegalArgumentException, \"Argument {0} is negative\");".format(offset_name))
				if self.return_argument:
					jni_implementation_generator.add_line("return ({0})0;".format(self.return_argument.get_java_type().get_jni_analog()))
				else:
					jni_implementation_generator.add_line("return;")
				jni_implementation_generator.dedent()
				jni_implementation_generator.add_line("}")

				if argument.is_vector:				
					array_length = argument.length_argument_name
					jni_implementation_generator.add_line("if YEP_UNLIKELY(((YepSize){0}) + ((YepSize){1}) > (YepSize)((*env)->GetArrayLength(env, {2}))) {{".format(offset_name, array_length, array_name))
					jni_implementation_generator.indent()
					jni_implementation_generator.add_line("(*env)->ThrowNew(env, IndexOutOfBoundsException, \"{0} + {1} exceed the length of {2}\");".format(offset_name, array_length, array_name))
					if self.return_argument:
						jni_implementation_generator.add_line("return ({0})0;".format(self.return_argument.get_java_type().get_jni_analog()))
					else:
						jni_implementation_generator.add_line("return;")
					jni_implementation_generator.dedent()
					jni_implementation_generator.add_line("}")
			elif argument.is_length_argument:
				jni_implementation_generator.add_line("if YEP_UNLIKELY({0} < 0) {{".format(argument.get_name()))
				jni_implementation_generator.indent()
				jni_implementation_generator.add_line("(*env)->ThrowNew(env, NegativeArraySizeException, \"Argument {0} is negative\");".format(argument.get_name()))
				if self.return_argument:
					jni_implementation_generator.add_line("return ({0})0;".format(self.return_argument.get_java_type().get_jni_analog()))
				else:
					jni_implementation_generator.add_line("return;")
				jni_implementation_generator.dedent()
				jni_implementation_generator.add_line("}")
				
		jni_implementation_generator.add_line()

		# Initialize pointer for arrays passed to the function
		for argument in self.arguments:
			if argument.is_automatic() and argument.is_vector:
				pointer_name = argument.get_c_private_name()
				array_name = argument.get_java_name(0)
				jni_implementation_generator.add_line("{1} = (*env)->GetPrimitiveArrayCritical(env, {0}, NULL);".format(array_name, pointer_name))
		jni_implementation_generator.add_line()

		# Emit the function call
		call_arguments = list()
		for argument in self.arguments:
			if argument.is_automatic() and argument.is_scalar and argument.is_output:
				call_arguments += ["&" + str(argument.get_c_public_name())]
			elif argument.is_automatic() and argument.is_vector:
				call_arguments += ["&{0}[{1}]".format(argument.get_c_private_name(), argument.get_java_name(1))]
			else:
				call_arguments += [c_private_argument.get_name() for c_private_argument in argument.c_private_arguments]
		jni_implementation_generator.add_line("status = {0}({1});".format(self.c_function_signature, ", ".join(call_arguments)))
		jni_implementation_generator.add_line()

		# Release arrays passed to the function
		for argument in reversed(self.arguments):
			if argument.is_automatic() and argument.is_vector:
				pointer_name = argument.get_c_private_name()
				array_name = argument.get_java_name(0)
				mode = "0" if argument.is_output else "JNI_ABORT"
				jni_implementation_generator.add_line("(*env)->ReleasePrimitiveArrayCritical(env, {0}, {1}, {2});".format(array_name, pointer_name, mode))

		# If function has scalar output other than return argument, they must be written to their place
		if any([argument.is_automatic() and argument.is_scalar and argument.is_output and not argument.is_return_argument for argument in self.arguments]):
			jni_implementation_generator.add_line()
			for argument in self.arguments:
				if argument.is_automatic() and argument.is_scalar and argument.is_output and not argument.is_return_argument:
					method_name = "Set" + str(argument.java_arguments[0].get_type().get_primitive_type()).title() + "ArrayRegion"
					jni_implementation_generator.add_line("(*env)->{0}(env, {1}, {2}, 1, &{3});".format(
						method_name, argument.get_java_name(0), argument.get_java_name(1), argument.get_name()))
					jni_implementation_generator.add_line("if YEP_UNLIKELY((*env)->ExceptionCheck(env) == JNI_TRUE) {")
					jni_implementation_generator.indent()
					if self.return_argument:
						jni_implementation_generator.add_line("return ({0})0;".format(self.return_argument.get_java_type().get_jni_analog()))
					else:
						jni_implementation_generator.add_line("return;")
					jni_implementation_generator.dedent()
					jni_implementation_generator.add_line("}")

		# Check return value and throw exception as necessary
		jni_implementation_generator.add_line("if YEP_UNLIKELY(status != YepStatusOk) {")
		jni_implementation_generator.indent()
		jni_implementation_generator.add_line("yepJNI_ThrowSuitableException(env, status);")
		jni_implementation_generator.dedent()
		jni_implementation_generator.add_line("}")
		jni_implementation_generator.add_line()


		# If function has a non-void return value, emit a return statement
		if self.return_argument:
			jni_implementation_generator.add_line()
			jni_implementation_generator.add_line("return " + str(self.return_argument.get_name()) + ";")
		jni_implementation_generator.dedent()
		jni_implementation_generator.add_line("}")
		jni_implementation_generator.add_line()

	def generate_java_method(self, java_class_generator, java_documentation):
		named_arguments_list = map(str, self.java_arguments)
		return_type = self.return_argument.get_java_type() if self.return_argument else "void" 

		if java_documentation:
			documentation = peachpy.doxygen.Documentation(java_documentation % self.documentation_macros)
			array_names = [argument.get_name() for argument in self.java_arguments if argument.get_type().is_array()]
			if array_names:
				documentation.throws["NullPointerException"] = "If %s is null." % format_or_list(array_names, prefix = "@a ")
				documentation.throws["MisalignedPointerError"] = "If %s is not naturally aligned." % format_or_list(array_names, prefix = "@a ")
			offset_names = [argument.get_java_name(1) for argument in self.arguments if not argument.is_return_argument and argument.is_automatic() and (argument.is_vector or argument.is_scalar and argument.is_output)]
			nonzero_names = [argument.get_java_name() for argument in self.arguments if argument.is_nonzero]
			if offset_names and nonzero_names:
				documentation.throws["InvalidArgumentException"] = "If %s is negative or %s is zero." % (format_or_list(offset_names, prefix = "@a "), format_or_list(nonzero_names, prefix = "@a "))
			elif offset_names:
				documentation.throws["InvalidArgumentException"] = "If %s is negative." % format_or_list(offset_names, prefix = "@a ")
			elif nonzero_names:
				documentation.throws["InvalidArgumentException"] = "If %s is zero." % format_or_list(nonzero_names, prefix = "@a ")
			length_names = [argument.get_java_name() for argument in self.arguments if argument.is_length_argument]
			if length_names:
				documentation.throws["NegativeArraySizeException"] = "If %s is negative." % format_or_list(length_names, prefix = "@a ")
			array_arguments = [argument for argument in self.arguments if not argument.is_return_argument and argument.is_automatic() and (argument.is_vector or argument.is_scalar and argument.is_output)]
			array_bound_exceptions = list()
			for array_argument in array_arguments:
				if array_argument.is_vector:
					array_bound_exceptions.append("@a {0} + @a {1} exceeds the length of @a {2}".format(array_argument.get_java_name(1), array_argument.length_argument_name, array_argument.get_java_name(0)))
				else:
					array_bound_exceptions.append("@a {0} is greater or equal to the length of @a {1}".format(array_argument.get_java_name(1), array_argument.get_java_name(0)))
			if array_bound_exceptions:
				documentation.throws["IndexOutOfBoundsException"] = "If %s." % format_or_list(array_bound_exceptions)

			optimized_implementations = self.get_optimized_implementations()
			if optimized_implementations:
				documentation.par["Optimized implementations"] = optimized_implementations 
			else:
				documentation.add_warning("This version of @Yeppp does not include optimized implementations for this function") 
			java_class_generator.add_c_comment(str(documentation), doxygen = True)
		java_class_generator.add_line("public static native {0} {1}({2});".format(return_type, self.short_function_signature, ", ".join(named_arguments_list)))

	def generate_fortran_interface(self, fortran_module_generator, c_documentation):
		argument_names = [fortran_argument.get_name() for fortran_argument in self.fortran_arguments]

		if c_documentation:
			documentation = peachpy.doxygen.Documentation(c_documentation % self.documentation_macros)
			documentation.ingroup = "yep" + self.module_name + "_" + self.function_name
			documentation.retval["0"] = "The computation finished successfully."
			pointer_names = [argument.get_name() for argument in self.c_public_arguments if argument.get_type().is_pointer()]
			if pointer_names:
				documentation.retval["2"] = "%s argument is not naturally aligned." % format_or_list(pointer_names, prefix = "@a ")
			for (name, (direction, description)) in documentation.parameters.items():
				if description.startswith("Pointer to the"):
					description = "The " + description[len("Pointer to the"):].lstrip()
					documentation.parameters[name] = (direction, description)
			nonzero_names = [argument.get_name() for argument in self.arguments if argument.is_nonzero]
			if nonzero_names:
				documentation.retval["3"] = "%s argument is zero." % format_or_list(nonzero_names, prefix = "@a ")

			optimized_implementations = self.get_optimized_implementations()
			if optimized_implementations:
				documentation.par["Optimized implementations"] = optimized_implementations 
			else:
				documentation.add_warning("This version of @Yeppp does not include optimized implementations for this function") 
			fortran_module_generator.add_fortran90_comment(str(documentation), doxygen = True)

		fortran_module_generator.add_line("INTEGER(C_INT) FUNCTION {0} & ".format(self.c_function_signature))
		fortran_module_generator.indent()
		fortran_module_generator.add_line("({0}) &".format(", ".join(argument_names)))
		fortran_module_generator.add_line("BIND(C, NAME='{0}')".format(self.c_function_signature))
		fortran_module_generator.add_line()

		argument_symbols = set([fortran_argument.get_type().get_symbol() for fortran_argument in self.fortran_arguments])
		argument_symbols = sorted(argument_symbols, key = lambda symbol: peachpy.fortran.Type.iso_c_symbols.index(symbol))
		fortran_module_generator.add_line("USE ISO_C_BINDING, ONLY: C_INT, {0}".format(", ".join(argument_symbols)))
		fortran_module_generator.add_line("IMPLICIT NONE")

		# Find the maximum type width to align the definitions
		type_width_max = max(len(argument.format(format_type = True, format_name = False)) for argument in self.fortran_arguments)

		# Find which arguments are used as array size: they must be defined first, or an error will be generated during compilation
		length_arguments = set([argument.get_type().dimension for argument in self.fortran_arguments if isinstance(argument.get_type().dimension, str)])
		for fortran_argument in self.fortran_arguments:
			if fortran_argument.get_name() in length_arguments:
				fortran_module_generator.add_line(fortran_argument.format(type_alignment = type_width_max)) 
		for fortran_argument in self.fortran_arguments:
			if fortran_argument.get_name() not in length_arguments:
				fortran_module_generator.add_line(fortran_argument.format(type_alignment = type_width_max)) 

		fortran_module_generator.dedent()
		fortran_module_generator.add_line("END FUNCTION {0}".format(self.c_function_signature))

	def generate_csharp_dllimport_method(self, csharp_dllimport_method_generator):
		named_arguments_list = map(str, self.csharp_dllimport_arguments)
		csharp_dllimport_method_generator.add_line("[DllImport(\"yeppp\", ExactSpelling=true, CallingConvention=CallingConvention.Cdecl, EntryPoint=\"{0}\")]".format(self.c_function_signature))
		csharp_dllimport_method_generator.add_line("private static unsafe extern Status {0}({1});".format(self.c_function_signature, ", ".join(named_arguments_list)))
		csharp_dllimport_method_generator.add_line()

	def generate_csharp_unsafe_method(self, csharp_unsafe_method_generator, c_documentation, java_documentation):
		named_arguments_list = map(str, self.csharp_unsafe_arguments)
		return_type = self.return_argument.get_csharp_unsafe_type() if self.return_argument else "void"

		if c_documentation:
			documentation = peachpy.doxygen.Documentation(c_documentation.replace("@a ", "") % self.documentation_macros)
			if self.return_argument:
				for (name, (direction, description)) in documentation.parameters.items():
					if name == self.return_argument.get_csharp_unsafe_name():
						del documentation.parameters[name]
						break
			if java_documentation:
				javadoc = peachpy.doxygen.Documentation(java_documentation.replace("@a ", "") % self.documentation_macros)
				if javadoc.returns:
					documentation.returns = javadoc.returns

			pointer_names = [argument.get_name() for argument in self.csharp_unsafe_arguments if argument.get_type().is_pointer()]
			if pointer_names:
				documentation.throws["System.NullReferenceException"] = "If %s is null." % format_or_list(pointer_names)
			pointer_names = [argument.get_name() for argument in self.csharp_unsafe_arguments if argument.get_type().is_pointer() and argument.get_type().get_primitive_type().get_size() != 1]
			if pointer_names:
				documentation.throws["System.DataMisalignedException"] = "If %s is not naturally aligned." % format_or_list(pointer_names)
			length_names = [argument.get_csharp_unsafe_name() for argument in self.arguments if argument.is_length_argument]
			nonzero_names = [argument.get_csharp_unsafe_name() for argument in self.arguments if argument.is_nonzero]
			if length_names and nonzero_names:
				documentation.throws["System.ArgumentException"] = "If %s is negative or %s is zero." % (format_or_list(length_names), format_or_list(nonzero_names))
			elif length_names:
				documentation.throws["System.ArgumentException"] = "If %s is negative." % format_or_list(length_names)
			elif nonzero_names:
				documentation.throws["System.ArgumentException"] = "If %s is zero." % format_or_list(nonzero_names)

			csharp_unsafe_method_generator.add_csharp_comment(documentation.xml_comment(), doxygen = True)

		csharp_unsafe_method_generator.add_line("public static unsafe {0} {1}({2})".format(return_type, self.short_function_signature, ", ".join(named_arguments_list)))
		csharp_unsafe_method_generator.add_line("{").indent()

		# Generate parameter checks
		for argument in self.arguments:
			if argument.is_length_argument:
				csharp_unsafe_method_generator.add_line("if (%s < 0)" % argument.get_csharp_unsafe_name())
				csharp_unsafe_method_generator.indent().add_line("throw new System.ArgumentException();").dedent().add_line()

		# If needed, define a variable for return value
		if self.return_argument is not None:
			csharp_unsafe_method_generator.add_line("{0} {1};".format(self.return_argument.get_csharp_unsafe_type(), self.return_argument.get_name()))

		# Emit function call
		call_arguments = list()
		for argument in self.arguments:
			for csharp_unsafe_argument, csharp_dllimport_argument in zip(argument.csharp_unsafe_arguments, argument.csharp_dllimport_arguments):
				if csharp_unsafe_argument.get_type() != csharp_dllimport_argument.get_type():
					if csharp_dllimport_argument.get_type().is_unsigned_integer():
						call_arguments.append("new {0}(unchecked((uint) {1}))".format(csharp_dllimport_argument.get_type(), csharp_unsafe_argument.get_name()))
					else:
						call_arguments.append("new {0}(unchecked({1}))".format(csharp_dllimport_argument.get_type(), csharp_unsafe_argument.get_name()))
				else:
					if argument.is_automatic() and argument.is_scalar and argument.is_output:
						call_arguments.append("out " + csharp_unsafe_argument.get_name())
					else:
						call_arguments.append(csharp_unsafe_argument.get_name())
		csharp_unsafe_method_generator.add_line("Status status = {0}({1});".format(self.c_function_signature, ", ".join(call_arguments)))
		csharp_unsafe_method_generator.add_line("if (status != Status.Ok)")
		csharp_unsafe_method_generator.indent().add_line("throw Library.GetException(status);").dedent()

		# If function has non-void return value, emit return statement
		if self.return_argument is not None:
			csharp_unsafe_method_generator.add_line("return {0};".format(self.return_argument.get_name()))

		csharp_unsafe_method_generator.dedent().add_line("}")
		csharp_unsafe_method_generator.add_empty_lines(2)

	def generate_csharp_safe_method(self, csharp_safe_method_generator, java_documentation):
		named_arguments_list = map(str, self.csharp_safe_arguments)
		return_type = self.return_argument.get_csharp_safe_type() if self.return_argument else "void"


		if java_documentation:
			documentation = peachpy.doxygen.Documentation(java_documentation.replace("@a ", "") % self.documentation_macros)
			array_names = [argument.get_name() for argument in self.csharp_safe_arguments if argument.get_type().is_array()]
			if array_names:
				documentation.throws["System.NullReferenceException"] = "If %s is null." % format_or_list(array_names)
			array_names = [argument.get_name() for argument in self.csharp_safe_arguments if argument.get_type().is_array() and argument.get_type().get_primitive_type().get_size() != 1]
			if array_names:
				documentation.throws["System.DataMisalignedException"] = "If %s is not naturally aligned." % format_or_list(array_names)
			length_names = [argument.get_csharp_safe_name() for argument in self.arguments if argument.is_length_argument]
			nonzero_names = [argument.get_csharp_safe_name() for argument in self.arguments if argument.is_nonzero]
			if length_names and nonzero_names:
				documentation.throws["System.ArgumentException"] = "If %s is negative or %s is zero." % (format_or_list(length_names), format_or_list(nonzero_names))
			elif length_names:
				documentation.throws["System.ArgumentException"] = "If %s is negative." % format_or_list(length_names)
			elif nonzero_names:
				documentation.throws["System.ArgumentException"] = "If %s is zero." % format_or_list(nonzero_names)
			array_bound_exceptions = list()
			for argument in self.arguments:
				if argument.is_length_argument:
					array_bound_exceptions.append("{0} is negative".format(argument.get_csharp_safe_name()))
				elif argument.is_vector:
					array_bound_exceptions.append("{0} is negative".format(argument.get_csharp_safe_name(1)))
					array_bound_exceptions.append("{0} + {1} exceeds the length of {2}".format(argument.get_csharp_safe_name(1), argument.length_argument_name, argument.get_csharp_safe_name(0)))
				elif argument.is_scalar and argument.is_output and not argument.is_return_argument:
					array_bound_exceptions.append("{0} is negative".format(argument.get_csharp_safe_name(1)))
					array_bound_exceptions.append("{0} is greater or equal to the length of {1}".format(argument.get_csharp_safe_name(1), argument.get_csharp_safe_name(0)))
			if array_bound_exceptions:
				if len(array_bound_exceptions) <= 2:
					documentation.throws["System.IndexOutOfRangeException"] = "If " + " or ".join(array_bound_exceptions) + "."
				else:
					documentation.throws["System.IndexOutOfRangeException"] = "If " + ", ".join(array_bound_exceptions[:-1]) + ", or " + array_bound_exceptions[-1] + "."
			csharp_safe_method_generator.add_csharp_comment(documentation.xml_comment(), doxygen = True)

		csharp_safe_method_generator.add_line("public static unsafe {0} {1}({2})".format(return_type, self.short_function_signature, ", ".join(named_arguments_list)))
		csharp_safe_method_generator.add_line("{").indent()

		# Generate parameter checks
		for argument in self.arguments:
			if argument.is_length_argument:
				csharp_safe_method_generator.add_line("if (%s < 0)" % argument.get_csharp_safe_name())
				csharp_safe_method_generator.indent().add_line("throw new System.ArgumentException();").dedent().add_line()
			elif argument.is_vector:
				csharp_safe_method_generator.add_line("if ({0} < 0)".format(argument.get_csharp_safe_name(1)))
				csharp_safe_method_generator.indent().add_line("throw new System.IndexOutOfRangeException();").dedent().add_line()

				csharp_safe_method_generator.add_line("if ({0} + {1} > {2}.Length)".format(argument.get_csharp_safe_name(1), argument.length_argument_name, argument.get_csharp_safe_name(0)))
				csharp_safe_method_generator.indent().add_line("throw new System.IndexOutOfRangeException();").dedent().add_line()
			elif argument.is_scalar and argument.is_output and not argument.is_return_argument:
				csharp_safe_method_generator.add_line("if ({0} < 0)".format(argument.get_csharp_safe_name(1)))
				csharp_safe_method_generator.indent().add_line("throw new System.IndexOutOfRangeException();").dedent().add_line()

				csharp_safe_method_generator.add_line("if ({0} >= {1}.Length)".format(argument.get_csharp_safe_name(1), argument.get_csharp_safe_name(0)))
				csharp_safe_method_generator.indent().add_line("throw new System.IndexOutOfRangeException();").dedent().add_line()

		# Emit pinning of arrays passed to function
		for argument in self.arguments:
			if argument.is_automatic() and argument.is_vector:
				csharp_safe_method_generator.add_line("fixed ({0} {1} = &{2}[{3}])".format(	argument.get_csharp_unsafe_type(),
																							argument.get_csharp_unsafe_name(),
																							argument.get_csharp_safe_name(0),
																							argument.get_csharp_safe_name(1)) )
				csharp_safe_method_generator.add_line("{").indent()

		# Emit call to unsafe method
		call_arguments = [argument.format(include_type = False) for argument in self.csharp_unsafe_arguments]
		if self.return_argument:
			csharp_safe_method_generator.add_line("return {0}({1});".format(self.short_function_signature, ", ".join(call_arguments)))
		else:
			csharp_safe_method_generator.add_line("{0}({1});".format(self.short_function_signature, ", ".join(call_arguments)))

		# Emit end of pinning regions
		for argument in self.arguments:
			if argument.is_automatic() and argument.is_vector:
				csharp_safe_method_generator.dedent().add_line("}")

		csharp_safe_method_generator.dedent().add_line("}")
		csharp_safe_method_generator.add_empty_lines(2)

	def generate_cpp_unit_test(self, cpp_unit_test_generator, unit_test, cpp_units_tests):
		if isinstance(unit_test, yeppp.test.ReferenceUnitTest):
			# Create a copy of unit test arguments which will be updated specifically for this specialization 
			test_arguments = copy.deepcopy(unit_test.arguments)
			# Check that all arguments specified for the test are also among the function arguments
			argument_names = set([argument.get_name() for argument in self.arguments])
			for argument_name in test_arguments.iterkeys():
				if argument_name not in argument_names:
					raise ValueError('Unit test argument {0} in not a function argument'.format(argument_name))
			for argument in self.arguments:
				if argument.get_name() in test_arguments.iterkeys():
					if argument.is_automatic():
						test_arguments[argument.get_name()].check(argument.get_c_public_type())
				else:
					if argument.is_automatic() and argument.is_input:
						raise KeyError("The input argument {0} is not specified in the unit test".format(argument.get_name()))
					elif argument.is_automatic() and argument.is_output:
						test_arguments[argument.get_name()] = yeppp.test.Uniform()
					elif not argument.is_automatic() and argument.is_length_argument:
						if argument.get_name() == "length":
							test_arguments["length"] = [slice(0, 64), slice(1024, 1024 + 64)]
						else:
							raise KeyError("The length argument {0} is not specified in the unit test".format(argument.get_name()))

			cpp_units_tests.append(self.short_function_signature)
			cpp_unit_test_generator.add_line("static Yep32s Test_{0}(Yep64u supportedIsaFeatures, Yep64u supportedSimdFeatures, Yep64u supportedSystemFeatures) {{".format(self.short_function_signature)).indent()
			cpp_unit_test_generator.add_line("YepRandom_WELL1024a rng;")
			cpp_unit_test_generator.add_line("YepStatus status = yepRandom_WELL1024a_Init(&rng);")
			cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
			cpp_unit_test_generator.add_line()

			unnamed_arguments_list = [argument.get_type().format(compact_pointers = False, restrict_qualifier = "YEP_RESTRICT") for argument in self.c_public_arguments] 
			cpp_unit_test_generator.add_line("typedef YepStatus (YEPABI* FunctionPointer)({0});".format(", ".join(unnamed_arguments_list)))
			cpp_unit_test_generator.add_line("typedef const FunctionDescriptor<FunctionPointer>* DescriptorPointer;")
			cpp_unit_test_generator.add_line("const DescriptorPointer defaultDescriptor = findDefaultDescriptor(_dispatchTable_{0});".format(self.c_function_signature))
			cpp_unit_test_generator.add_line("const FunctionPointer defaultImplementation = defaultDescriptor->function;".format(", ".join(unnamed_arguments_list)))
			cpp_unit_test_generator.add_line("Yep32s failedTests = 0;")
			cpp_unit_test_generator.add_line()

			def get_length_bound(argument):
				assert argument.is_automatic() and argument.is_vector
				ranges = test_arguments[argument.length_argument_name]
				length_bound = None
				for range in ranges:
					if isinstance(range, int):
						length_bound = range if length_bound is None else max(length_bound, range)
					elif isinstance(range, slice): 
						length_bound = range.stop if length_bound is None else max(length_bound, range.stop)
					else:
						raise TypeError('Unsupported type for range %s' % range)
				if length_bound is None:
					raise KeyError('Unspecified length bound for argument %s' % argument.get_name())
				else:
					return length_bound
				

			for argument in self.arguments:
				if argument.is_automatic():
					if argument.is_vector:
						cpp_unit_test_generator.add_line("YEP_ALIGN(64) {0} {1}Array[{2} + (64 / sizeof({0}))];".format(argument.get_c_public_type().get_primitive_type(), argument.get_name(), get_length_bound(argument)))
						if argument.is_output:
							cpp_unit_test_generator.add_line("YEP_ALIGN(64) {0} {1}InitArray[{2} + (64 / sizeof({0}))];".format(argument.get_c_public_type().get_primitive_type(), argument.get_name(), get_length_bound(argument)))
							cpp_unit_test_generator.add_line("YEP_ALIGN(64) {0} {1}RefArray[{2} + (64 / sizeof({0}))];".format(argument.get_c_public_type().get_primitive_type(), argument.get_name(), get_length_bound(argument)))
					elif argument.is_scalar:
						cpp_unit_test_generator.add_line("{0} {1};".format(argument.get_c_public_type().get_primitive_type(), argument.get_name()))
						if argument.is_output:
							cpp_unit_test_generator.add_line("{0} {1}Init;".format(argument.get_c_public_type().get_primitive_type(), argument.get_name()))
							cpp_unit_test_generator.add_line("{0} {1}Ref;".format(argument.get_c_public_type().get_primitive_type(), argument.get_name()))
			cpp_unit_test_generator.add_line()

			random_generator_function_map = {'Yep8u' : 'yepRandom_WELL1024a_GenerateDiscreteUniform_S8uS8u_V8u',
			                                 'Yep16u': 'yepRandom_WELL1024a_GenerateDiscreteUniform_S16uS16u_V16u',
			                                 'Yep32u': 'yepRandom_WELL1024a_GenerateDiscreteUniform_S32uS32u_V32u',
			                                 'Yep64u': 'yepRandom_WELL1024a_GenerateDiscreteUniform_S64uS64u_V64u',
			                                 'Yep8s' : 'yepRandom_WELL1024a_GenerateDiscreteUniform_S8sS8s_V8s',
			                                 'Yep16s': 'yepRandom_WELL1024a_GenerateDiscreteUniform_S16sS16s_V16s',
			                                 'Yep32s': 'yepRandom_WELL1024a_GenerateDiscreteUniform_S32sS32s_V32s',
			                                 'Yep64s': 'yepRandom_WELL1024a_GenerateDiscreteUniform_S64sS64s_V64s',
			                                 'Yep32f': 'yepRandom_WELL1024a_GenerateUniform_S32fS32f_V32f_Acc32',
			                                 'Yep64f': 'yepRandom_WELL1024a_GenerateUniform_S64fS64f_V64f_Acc64'}
			for argument in self.arguments:
				if argument.is_automatic():
					if argument.is_vector:
						argument_array_name = argument.get_name() + ("InitArray" if argument.is_output else "Array")
						cpp_unit_test_generator.add_line("status = {0}(&rng, {1}, {2}, YEP_COUNT_OF({2}));".format(
							random_generator_function_map[str(argument.c_public_arguments[0].get_type().get_primitive_type())],
							", ".join(test_arguments[argument.get_name()].format(argument.get_c_public_type())),
							argument_array_name))
						cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
					elif argument.is_scalar:
						argument_address_name = "&" + argument.get_name() + ("Init" if argument.is_output else "")
						cpp_unit_test_generator.add_line("status = {0}(&rng, {1}, {2}, 1);".format(
							random_generator_function_map[str(argument.c_public_arguments[0].get_type().get_primitive_type())],
							", ".join(test_arguments[argument.get_name()].format(argument.get_c_public_type())),
							argument_address_name))
						cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
			cpp_unit_test_generator.add_line()

			cpp_unit_test_generator.add_line("for (DescriptorPointer descriptor = &_dispatchTable_{0}[0]; descriptor != defaultDescriptor; descriptor++) {{".format(self.c_function_signature)).indent()
			cpp_unit_test_generator.add_line("const Yep64u unsupportedRequiredFeatures = (descriptor->isaFeatures & ~supportedIsaFeatures) |")
			cpp_unit_test_generator.add_line("\t(descriptor->simdFeatures & ~supportedSimdFeatures) |")
			cpp_unit_test_generator.add_line("\t(descriptor->systemFeatures & ~supportedSystemFeatures);")
			cpp_unit_test_generator.add_line("if (unsupportedRequiredFeatures == 0) {").indent()
			def generate_loops(arguments):
				if arguments:
					argument = arguments[0]
					arguments = arguments[1:]
					if argument.is_automatic() and argument.is_vector:
						cpp_unit_test_generator.add_line("for (YepSize {0}Offset = 0; {0}Offset < 64 / sizeof({1}); {0}Offset++) {{".format(argument.get_name(), argument.get_c_public_type().get_primitive_type()))
						cpp_unit_test_generator.indent()
	
						generate_loops(arguments)
	
						cpp_unit_test_generator.dedent()
						cpp_unit_test_generator.add_line("}")
					elif not argument.is_automatic() and argument.is_length_argument:
						for range in test_arguments[argument.get_name()]:
							if isinstance(range, int) or isinstance(range, long) or isinstance(range, float):
								cpp_unit_test_generator.add_line("const {0} {1} = {2}".format(argument.get_c_public_type(), argument.get_name(), range))
								generate_loops(arguments)
							elif isinstance(range, slice):
								start = 0 if range.start is None else int(range.start)
								stop = int(range.stop)
								step = -cmp(range.start, range.stop) if range.step is None else range.step
								cpp_unit_test_generator.add_line("for ({1} {0} = {2}; {0} < {3}; {0} += {4}) {{".format(argument.get_name(), argument.get_c_public_type().get_primitive_type(), start, stop, step))
								cpp_unit_test_generator.indent()
	
								generate_loops(arguments)
	
								cpp_unit_test_generator.dedent()
								cpp_unit_test_generator.add_line("}")
					else:
						generate_loops(arguments)
				else:
					call_arguments = list()
					reference_call_arguments = list()
					for argument in self.arguments:
						if argument.is_automatic() and argument.is_vector:
							call_arguments.append("&{0}Array[{0}Offset]".format(argument.get_name()))
							if argument.is_output:
								reference_call_arguments.append("&{0}RefArray[{0}Offset]".format(argument.get_name()))
							else:
								reference_call_arguments.append("&{0}Array[{0}Offset]".format(argument.get_name()))
						elif argument.is_automatic() and argument.is_scalar and argument.is_output:
							call_arguments.append("&" + argument.get_name())
							reference_call_arguments.append("&" + argument.get_name() + "Ref")
						else:
							call_arguments.append(argument.get_name())
							reference_call_arguments.append(argument.get_name())

					# Initialize the reference outputs with default values
					for argument in self.arguments:
						if argument.is_automatic() and argument.is_vector and argument.is_output:
							cpp_unit_test_generator.add_line("memcpy({0}RefArray, {0}InitArray, sizeof({0}RefArray));".format(argument.get_name()))
						elif argument.is_automatic() and argument.is_scalar and argument.is_output:
							cpp_unit_test_generator.add_line("{0}Ref = {0}Init;".format(argument.get_name()))

					# Emit the reference function call
					cpp_unit_test_generator.add_line("status = defaultImplementation(%s);" % ", ".join(reference_call_arguments))
					cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
					cpp_unit_test_generator.add_line()

					# Initialize the outputs with default values
					for argument in self.arguments:
						if argument.is_automatic() and argument.is_vector and argument.is_output:
							cpp_unit_test_generator.add_line("memcpy({0}Array, {0}InitArray, sizeof({0}Array));".format(argument.get_name()))
						elif argument.is_automatic() and argument.is_scalar and argument.is_output:
							cpp_unit_test_generator.add_line("{0} = {0}Init;".format(argument.get_name()))

					# Emit the optimized function call
					cpp_unit_test_generator.add_line("status = descriptor->function(%s);" % ", ".join(call_arguments))
					cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
					cpp_unit_test_generator.add_line()

					# Check the results
					for argument in self.arguments:
						if argument.is_automatic() and argument.is_output:
							if argument.is_vector:
								if argument.get_c_public_type().get_primitive_type().is_integer():
									cpp_unit_test_generator.add_line("if (memcmp({0}Array, {0}RefArray, sizeof({0}Array)) != 0) {{".format(argument.get_name())).indent()
									cpp_unit_test_generator.add_line("failedTests += 1;")
									cpp_unit_test_generator.add_line("reportFailedTest(\"{0}\", descriptor->microarchitecture);".format(self.c_function_signature))
									cpp_unit_test_generator.add_line("goto next_descriptor;")
									cpp_unit_test_generator.dedent().add_line("}")
								elif argument.get_c_public_type().get_primitive_type().is_floating_point():
									max_ulp_error = unit_test.max_ulp_error
									if max_ulp_error is None:
										max_ulp_error = 5.0

									cpp_unit_test_generator.add_line("for (YepSize i = 0; i < YEP_COUNT_OF({0}Array); i++) {{".format(argument.get_name())).indent()
									cpp_unit_test_generator.add_line("const {1} ulpError = yepBuiltin_Abs_{2}f_{2}f({0}RefArray[i] - {0}Array[i]) / yepBuiltin_Ulp_{2}f_{2}f({0}RefArray[i]);".format(
										argument.get_name(), argument.get_c_public_type().get_primitive_type(), argument.get_c_public_type().get_primitive_type().get_size() * 8))
									cpp_unit_test_generator.add_line("if (ulpError > {0}) {{".format(max_ulp_error)).indent()
									cpp_unit_test_generator.add_line("failedTests += 1;")
									cpp_unit_test_generator.add_line("reportFailedTest(\"{0}\", descriptor->microarchitecture, float(ulpError));".format(self.c_function_signature))
									cpp_unit_test_generator.add_line("goto next_descriptor;")
									cpp_unit_test_generator.dedent().add_line("}")
									cpp_unit_test_generator.dedent().add_line("}")
							else:
								if argument.get_c_public_type().get_primitive_type().is_integer():
									cpp_unit_test_generator.add_line("if ({0} != {0}Ref) {{".format(argument.get_name())).indent()
									cpp_unit_test_generator.add_line("failedTests += 1;")
									cpp_unit_test_generator.add_line("reportFailedTest(\"{0}\", descriptor->microarchitecture);".format(self.c_function_signature))
									cpp_unit_test_generator.add_line("goto next_descriptor;")
									cpp_unit_test_generator.dedent().add_line("}")
								elif argument.get_c_public_type().get_primitive_type().is_floating_point():
									max_ulp_error = unit_test.max_ulp_error
									if max_ulp_error is None:
										max_ulp_error = 1000.0

									cpp_unit_test_generator.add_line("const {1} ulpError = yepBuiltin_Abs_{2}f_{2}f({0}Ref - {0}) / yepBuiltin_Ulp_{2}f_{2}f({0}Ref);".format(
										argument.get_name(), argument.get_c_public_type().get_primitive_type(), argument.get_c_public_type().get_primitive_type().get_size() * 8))
									cpp_unit_test_generator.add_line("if (ulpError > {0}f) {{".format(max_ulp_error)).indent()
									cpp_unit_test_generator.add_line("failedTests += 1;")
									cpp_unit_test_generator.add_line("reportFailedTest(\"{0}\", descriptor->microarchitecture, float(ulpError));".format(self.c_function_signature))
									cpp_unit_test_generator.add_line("goto next_descriptor;")
									cpp_unit_test_generator.dedent().add_line("}")

			generate_loops(self.arguments)
			cpp_unit_test_generator.add_line("reportPassedTest(\"{0}\", descriptor->microarchitecture);".format(self.c_function_signature))
			cpp_unit_test_generator.dedent().add_line("} else {").indent()
			cpp_unit_test_generator.add_line("reportSkippedTest(\"{0}\", descriptor->microarchitecture);".format(self.c_function_signature))
			cpp_unit_test_generator.dedent().add_line("}")
			cpp_unit_test_generator.add_line("next_descriptor:", indent = 0)
			cpp_unit_test_generator.add_line("continue;")
			cpp_unit_test_generator.dedent().add_line("}")
			cpp_unit_test_generator.add_line("return -failedTests;")
			cpp_unit_test_generator.dedent().add_line("}")
			cpp_unit_test_generator.add_line()
		else:
			raise TypeError('Unsupported unit test type')
		
	def get_optimized_implementations(self):
		implementations = list()
		if any(map(bool, self.assembly_functions.itervalues())):
			implementations.append("\t<table>")
			implementations.append("\t\t<tr><th>Architecture</th><th>Target microarchitecture</th><th>Required instruction extensions</th></tr>")
# 				for assembly_function in self.assembly_functions['x86']:
# 					isa_extensions = [isa_extension for isa_extension in assembly_function.get_isa_extensions() if isa_extension]
# 					documentation_lines.append(" * \t\t\t<tr><td>x86</td><td>{0}</td><td>{1}</td></tr>".format(assembly_function.microarchitecture, ", ".join(isa_extensions)))
			for assembly_function in sorted(self.assembly_functions['x64-sysv'], key = lambda function: function.target.microarchitecture.get_number()):
				isa_extensions = [isa_extension for isa_extension in assembly_function.get_isa_extensions() if isa_extension]
				isa_extensions = sorted(isa_extensions, key = lambda isa_extension: peachpy.x64.supported_isa_extensions.index(isa_extension))
				implementations.append("\t\t<tr><td>x86-64</td><td>{0}</td><td>{1}</td></tr>".format(assembly_function.target.microarchitecture, ", ".join(isa_extensions)))
			for assembly_function in sorted(self.assembly_functions['arm-softeabi'], key = lambda function: function.target.microarchitecture.get_number()):
				isa_extensions = [isa_extension for isa_extension in assembly_function.get_isa_extensions() if isa_extension]
				isa_extensions = sorted(isa_extensions, key = lambda isa_extension: peachpy.arm.supported_isa_extensions.index(isa_extension))
				implementations.append("\t\t<tr><td>ARM</td><td>{0}</td><td>{1}</td></tr>".format(assembly_function.target.microarchitecture, ", ".join(isa_extensions)))
			implementations.append("\t</table>")
		return implementations



class FunctionGenerator:
	def __init__(self):
		self.code = []
		self.public_header_generator = None
		self.module_header_generator = None
		self.module_initialization_generator = None
		self.initialization_function_generator = None
		self.default_cpp_implementation_generator = None
		self.dispatch_table_header_generator = None
		self.dispatch_table_generator = None
		self.dispatch_pointer_generator = None
		self.java_class_generator = None
		self.jni_implementation_generator = None
		self.fortran_module_generator = None
		self.csharp_safe_method_generator = None
		self.csharp_unsafe_method_generator = None
		self.csharp_extern_method_generator = None
		self.cpp_unit_test_generator = None
		self.assembly_implementation_generators = dict()
		self.assembly_cache = dict()

		self.default_cpp_implementation = None
		self.assembly_implementations = list()
		self.c_documentation = None
		self.java_documentation = None

		self.unit_test = None
		self.cpp_unit_tests = list()

	def generate_group_prolog(self, module_name, module_comment, group_name, group_comment, header_license, source_license):
		from peachpy import x86
		from peachpy import x64
		from peachpy import arm

		self.dispatch_table_header_generator = peachpy.codegen.CodeGenerator()
		self.dispatch_table_header_generator.add_c_comment(source_license)
		self.dispatch_table_header_generator.add_line()
		self.dispatch_table_header_generator.add_line("#pragma once")
		self.dispatch_table_header_generator.add_line()
		self.dispatch_table_header_generator.add_line("#include <yepPredefines.h>")
		self.dispatch_table_header_generator.add_line("#include <yepTypes.h>")
		self.dispatch_table_header_generator.add_line("#include <yepPrivate.h>")
		self.dispatch_table_header_generator.add_line("#include <yep{0}.h>".format(module_name))
		self.dispatch_table_header_generator.add_line("#include <library/functions.h>".format(module_name))
		self.dispatch_table_header_generator.add_line()

		self.dispatch_pointer_header_generator = peachpy.codegen.CodeGenerator()
		self.dispatch_pointer_header_generator.add_line()
		self.dispatch_pointer_header_generator.add_line()

		self.initialization_function_generator = peachpy.codegen.CodeGenerator()
		self.initialization_function_generator.add_line()
		self.initialization_function_generator.add_line()
		self.initialization_function_generator.add_line("inline static YepStatus _yep{0}_{1}_Init() {{".format(module_name, group_name))
		self.initialization_function_generator.indent()

		self.dispatch_table_generator = peachpy.codegen.CodeGenerator()
		self.dispatch_table_generator.add_c_comment(source_license)
		self.dispatch_table_generator.add_line()
		self.dispatch_table_generator.add_line("#include <yepPredefines.h>")
		self.dispatch_table_generator.add_line("#include <yepTypes.h>")
		self.dispatch_table_generator.add_line("#include <yepPrivate.h>")
		self.dispatch_table_generator.add_line("#include <{0}/{1}.disp.h>".format(module_name.lower(), group_name))
		self.dispatch_table_generator.add_line()
		self.dispatch_table_generator.add_line("#if defined(YEP_MSVC_COMPATIBLE_COMPILER)")
		self.dispatch_table_generator.indent()
		self.dispatch_table_generator.add_line("#pragma section(\".rdata$DispatchTable\", read)")
		self.dispatch_table_generator.add_line("#pragma section(\".data$DispatchPointer\", read, write)")
		self.dispatch_table_generator.dedent()
		self.dispatch_table_generator.add_line("#endif")
		self.dispatch_table_generator.add_line()

		self.module_header_generator.add_line("#include <{0}/{1}.disp.h>".format(module_name.lower(), group_name))

		self.module_initialization_generator.add_line("status = _yep{0}_{1}_Init();".format(module_name, group_name))
		self.module_initialization_generator.add_line("if YEP_UNLIKELY(status != YepStatusOk) {")
		self.module_initialization_generator.indent().add_line("return status;").dedent()
		self.module_initialization_generator.add_line("}")

		self.dispatch_pointer_generator = peachpy.codegen.CodeGenerator()
		self.dispatch_pointer_generator.add_line()
		self.dispatch_pointer_generator.add_line()

		self.dispatch_function_generator = peachpy.codegen.CodeGenerator()
		self.dispatch_function_generator.add_line()
		self.dispatch_function_generator.add_line()
		self.dispatch_function_generator.add_line("#if defined(YEP_MSVC_COMPATIBLE_COMPILER)")
		self.dispatch_function_generator.indent()
		self.dispatch_function_generator.add_line("#pragma code_seg( push, \".text$DispatchFunction\" )")
		self.dispatch_function_generator.dedent()
		self.dispatch_function_generator.add_line("#endif")
		self.dispatch_function_generator.add_line()

		self.default_cpp_implementation_generator = peachpy.codegen.CodeGenerator()
		self.default_cpp_implementation_generator.add_c_comment(source_license)
		self.default_cpp_implementation_generator.add_line()
		self.default_cpp_implementation_generator.add_line("#include <yepBuiltin.h>")
		self.default_cpp_implementation_generator.add_line("#include <yep{0}.h>".format(module_name))
		self.default_cpp_implementation_generator.add_line()
		self.default_cpp_implementation_generator.add_line()

		self.assembly_implementation_generators = [
# 			x86.Assembler(peachpy.c.ABI('x86')),
			x64.Assembler(peachpy.c.ABI('x64-ms')),
			x64.Assembler(peachpy.c.ABI('x64-sysv')),
			arm.Assembler(peachpy.c.ABI('arm-softeabi')),
			arm.Assembler(peachpy.c.ABI('arm-hardeabi'))
		]
		for assembly_implementation_generator in self.assembly_implementation_generators:
			assembly_implementation_generator.add_assembly_comment(source_license)
			assembly_implementation_generator.add_line()
			if assembly_implementation_generator.abi.get_name() in ['arm-hardeabi', 'arm-softeabi']:
				assembly_implementation_generator.add_line(".macro BEGIN_ARM_FUNCTION name")
				assembly_implementation_generator.indent()
				assembly_implementation_generator.add_line(".arm")
				assembly_implementation_generator.add_line(".globl \\name")
				assembly_implementation_generator.add_line(".align 2")
				assembly_implementation_generator.add_line(".func \\name")
				assembly_implementation_generator.add_line(".internal \\name")
				assembly_implementation_generator.add_line("\\name:")
				assembly_implementation_generator.dedent()
				assembly_implementation_generator.add_line(".endm")
				
				assembly_implementation_generator.add_line()
				
				assembly_implementation_generator.add_line(".macro END_ARM_FUNCTION name")
				assembly_implementation_generator.indent()
				assembly_implementation_generator.add_line(".endfunc")
				assembly_implementation_generator.add_line(".type \\name, %function")
				assembly_implementation_generator.add_line(".size \\name, .-\\name")
				assembly_implementation_generator.dedent()
				assembly_implementation_generator.add_line(".endm")
				
				assembly_implementation_generator.add_line()

		self.jni_implementation_generator = peachpy.codegen.CodeGenerator()
		self.jni_implementation_generator.add_c_comment(source_license)
		self.jni_implementation_generator.add_line()
		self.jni_implementation_generator.add_line("#include <jni.h>")
		self.jni_implementation_generator.add_line("#include <yepPrivate.h>")
		self.jni_implementation_generator.add_line("#include <yep{0}.h>".format(module_name))
		self.jni_implementation_generator.add_line("#include <yepJavaPrivate.h>")
		self.jni_implementation_generator.add_empty_lines(2)

		self.java_class_generator.add_line()
		self.java_class_generator.add_line("/** @name	{0} */".format(group_comment))
		self.java_class_generator.add_line("/**@{*/")

		self.public_header_generator.add_line("/**")
		self.public_header_generator.add_line(" * @ingroup yep{0}".format(module_name))
		self.public_header_generator.add_line(" * @defgroup yep{0}_{1}	{2}".format(module_name, group_name, group_comment))
		self.public_header_generator.add_line(" */")
		self.public_header_generator.add_line()

		self.csharp_safe_method_generator = peachpy.codegen.CodeGenerator()
		self.csharp_safe_method_generator.add_c_comment(source_license)
		self.csharp_safe_method_generator.add_line()
		self.csharp_safe_method_generator.add_line("using System.Runtime.InteropServices;")
		self.csharp_safe_method_generator.add_line()
		self.csharp_safe_method_generator.add_line("namespace Yeppp")
		self.csharp_safe_method_generator.add_line("{").indent().add_line()
		self.csharp_safe_method_generator.add_csharp_comment("<summary>%s</summary>" % module_comment, doxygen = True)
		self.csharp_safe_method_generator.add_line("public partial class %s" % module_name)
		self.csharp_safe_method_generator.add_line("{").indent().add_line()
		self.csharp_safe_method_generator.add_line()

		self.csharp_unsafe_method_generator = peachpy.codegen.CodeGenerator()
		self.csharp_unsafe_method_generator.add_line().indent().indent()

		self.csharp_dllimport_method_generator = peachpy.codegen.CodeGenerator()
		self.csharp_dllimport_method_generator.add_line().indent().indent()

		self.fortran_module_generator.add_fortran90_comment(["@ingroup yep{0}".format(module_name),
															 "@defgroup yep{0}_{1}	{2}".format(module_name, group_name, group_comment)], doxygen = True)
		self.fortran_module_generator.add_line()

		self.cpp_unit_test_generator = peachpy.codegen.CodeGenerator()
		self.cpp_unit_test_generator.add_c_comment(source_license)
		self.cpp_unit_test_generator.add_line()
		self.cpp_unit_test_generator.add_line("#include <yepPredefines.h>")
		self.cpp_unit_test_generator.add_line("#include <yepPrivate.h>")
		self.cpp_unit_test_generator.add_line("#include <yepLibrary.h>")
		self.cpp_unit_test_generator.add_line("#include <library/functions.h>")
		self.cpp_unit_test_generator.add_line("#include <yepRandom.h>")
		self.cpp_unit_test_generator.add_line("#include <{0}/functions.h>".format(module_name.lower()))
		self.cpp_unit_test_generator.add_line("#include <yepBuiltin.h>")
		self.cpp_unit_test_generator.add_line("#include <string.h>")
		self.cpp_unit_test_generator.add_line("#include <stdio.h>")
		self.cpp_unit_test_generator.add_line("#include <assert.h>")
		self.cpp_unit_test_generator.add_line()
		self.cpp_unit_test_generator.add_line("#ifdef YEP_WINDOWS_OS").indent()
		self.cpp_unit_test_generator.add_line("#include <windows.h>")
		self.cpp_unit_test_generator.add_line("#define YEP_ESCAPE_NORMAL_COLOR \"\"")
		self.cpp_unit_test_generator.add_line("#define YEP_ESCAPE_RED_COLOR \"\"")
		self.cpp_unit_test_generator.add_line("#define YEP_ESCAPE_GREEN_COLOR \"\"")
		self.cpp_unit_test_generator.add_line("#define YEP_ESCAPE_YELLOW_COLOR \"\"")
		self.cpp_unit_test_generator.dedent().add_line("#else").indent()
		self.cpp_unit_test_generator.add_line("#define YEP_ESCAPE_NORMAL_COLOR \"\\x1B[0m\"")
		self.cpp_unit_test_generator.add_line("#define YEP_ESCAPE_RED_COLOR \"\\x1B[31m\"")
		self.cpp_unit_test_generator.add_line("#define YEP_ESCAPE_GREEN_COLOR \"\\x1B[32m\"")
		self.cpp_unit_test_generator.add_line("#define YEP_ESCAPE_YELLOW_COLOR \"\\x1B[33m\"")
		self.cpp_unit_test_generator.dedent().add_line("#endif")
		self.cpp_unit_test_generator.add_line()
		self.cpp_unit_test_generator.add_line("static const char* getMicroarchitectureName(YepCpuMicroarchitecture microarchitecture) {").indent()
		self.cpp_unit_test_generator.add_line("const YepSize bufferSize = 1024;")
		self.cpp_unit_test_generator.add_line("static char buffer[bufferSize];")
		self.cpp_unit_test_generator.add_line("YepSize bufferLength = bufferSize - 1;")
		self.cpp_unit_test_generator.add_line("YepStatus status = yepLibrary_GetString(YepEnumerationCpuMicroarchitecture, microarchitecture, YepStringTypeDescription, buffer, &bufferLength);")
		self.cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
		self.cpp_unit_test_generator.add_line("buffer[bufferLength] = '\\0';")
		self.cpp_unit_test_generator.add_line("return buffer;")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.add_line()
		self.cpp_unit_test_generator.add_line("static void reportFailedTest(const char* functionName, YepCpuMicroarchitecture microarchitecture) {").indent()
		self.cpp_unit_test_generator.add_line("#ifdef YEP_WINDOWS_OS").indent()
		self.cpp_unit_test_generator.add_line("CONSOLE_SCREEN_BUFFER_INFO bufferInfo;")
		self.cpp_unit_test_generator.add_line("::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);")
		self.cpp_unit_test_generator.add_line("printf(\"%s (%s): \", functionName, getMicroarchitectureName(microarchitecture));")
		self.cpp_unit_test_generator.add_line("fflush(stdout);")
		self.cpp_unit_test_generator.add_line("::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);")
		self.cpp_unit_test_generator.add_line("printf(\"FAILED\\n\");")
		self.cpp_unit_test_generator.add_line("fflush(stdout);")
		self.cpp_unit_test_generator.add_line("::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), bufferInfo.wAttributes);")
		self.cpp_unit_test_generator.dedent().add_line("#else").indent()
		self.cpp_unit_test_generator.add_line("printf(\"%s (%s): %sFAILED%s\\n\", functionName, getMicroarchitectureName(microarchitecture), YEP_ESCAPE_RED_COLOR, YEP_ESCAPE_NORMAL_COLOR);")
		self.cpp_unit_test_generator.dedent().add_line("#endif")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.add_line()
		self.cpp_unit_test_generator.add_line("static void reportFailedTest(const char* functionName, YepCpuMicroarchitecture microarchitecture, float ulpError) {").indent()
		self.cpp_unit_test_generator.add_line("#ifdef YEP_WINDOWS_OS").indent()
		self.cpp_unit_test_generator.add_line("CONSOLE_SCREEN_BUFFER_INFO bufferInfo;")
		self.cpp_unit_test_generator.add_line("::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);")
		self.cpp_unit_test_generator.add_line("printf(\"%s (%s): \", functionName, getMicroarchitectureName(microarchitecture));")
		self.cpp_unit_test_generator.add_line("fflush(stdout);")
		self.cpp_unit_test_generator.add_line("::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);")
		self.cpp_unit_test_generator.add_line("printf(\"FAILED\");")
		self.cpp_unit_test_generator.add_line("fflush(stdout);")
		self.cpp_unit_test_generator.add_line("::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), bufferInfo.wAttributes);")
		self.cpp_unit_test_generator.add_line("printf(\" (%f ULP)\\n\", ulpError);")
		self.cpp_unit_test_generator.dedent().add_line("#else").indent()
		self.cpp_unit_test_generator.add_line("printf(\"%s (%s): %sFAILED%s (%f ULP)\\n\", functionName, getMicroarchitectureName(microarchitecture), YEP_ESCAPE_RED_COLOR, YEP_ESCAPE_NORMAL_COLOR, ulpError);")
		self.cpp_unit_test_generator.dedent().add_line("#endif")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.add_line()
		self.cpp_unit_test_generator.add_line("static void reportPassedTest(const char* functionName, YepCpuMicroarchitecture microarchitecture) {").indent()
		self.cpp_unit_test_generator.add_line("#ifdef YEP_WINDOWS_OS").indent()
		self.cpp_unit_test_generator.add_line("CONSOLE_SCREEN_BUFFER_INFO bufferInfo;")
		self.cpp_unit_test_generator.add_line("::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);")
		self.cpp_unit_test_generator.add_line("printf(\"%s (%s): \", functionName, getMicroarchitectureName(microarchitecture));")
		self.cpp_unit_test_generator.add_line("fflush(stdout);")
		self.cpp_unit_test_generator.add_line("::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);")
		self.cpp_unit_test_generator.add_line("printf(\"PASSED\\n\");")
		self.cpp_unit_test_generator.add_line("fflush(stdout);")
		self.cpp_unit_test_generator.add_line("::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), bufferInfo.wAttributes);")
		self.cpp_unit_test_generator.dedent().add_line("#else").indent()
		self.cpp_unit_test_generator.add_line("printf(\"%s (%s): %sPASSED%s\\n\", functionName, getMicroarchitectureName(microarchitecture), YEP_ESCAPE_GREEN_COLOR, YEP_ESCAPE_NORMAL_COLOR);")
		self.cpp_unit_test_generator.dedent().add_line("#endif")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.add_line()
		self.cpp_unit_test_generator.add_line("static void reportSkippedTest(const char* functionName, YepCpuMicroarchitecture microarchitecture) {").indent()
		self.cpp_unit_test_generator.add_line("#ifdef YEP_WINDOWS_OS").indent()
		self.cpp_unit_test_generator.add_line("CONSOLE_SCREEN_BUFFER_INFO bufferInfo;")
		self.cpp_unit_test_generator.add_line("::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);")
		self.cpp_unit_test_generator.add_line("printf(\"%s (%s): \", functionName, getMicroarchitectureName(microarchitecture));")
		self.cpp_unit_test_generator.add_line("fflush(stdout);")
		self.cpp_unit_test_generator.add_line("::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);")
		self.cpp_unit_test_generator.add_line("printf(\"SKIPPED\\n\");")
		self.cpp_unit_test_generator.add_line("fflush(stdout);")
		self.cpp_unit_test_generator.add_line("::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), bufferInfo.wAttributes);")
		self.cpp_unit_test_generator.dedent().add_line("#else").indent()
		self.cpp_unit_test_generator.add_line("printf(\"%s (%s): %sSKIPPED%s\\n\", functionName, getMicroarchitectureName(microarchitecture), YEP_ESCAPE_YELLOW_COLOR, YEP_ESCAPE_NORMAL_COLOR);")
		self.cpp_unit_test_generator.dedent().add_line("#endif")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.add_line()

	def generate_group_epilog(self, module_name, group_name):
		self.initialization_function_generator.add_line("return YepStatusOk;")
		self.initialization_function_generator.dedent()
		self.initialization_function_generator.add_line("}")

		self.java_class_generator.add_line("/**@}*/")
		self.java_class_generator.add_line()

		self.dispatch_function_generator.add_line("#if defined(YEP_MSVC_COMPATIBLE_COMPILER)")
		self.dispatch_function_generator.indent().add_line("#pragma code_seg( pop )").dedent()
		self.dispatch_function_generator.add_line("#endif")

		self.csharp_dllimport_method_generator.dedent().add_line("}")
		self.csharp_dllimport_method_generator.add_line().dedent().add_line("}")
		self.csharp_dllimport_method_generator.add_line()

		self.cpp_unit_test_generator.add_line("int main(int argc, char** argv) {").indent()
		for cpp_unit_test in self.cpp_unit_tests:
			self.cpp_unit_test_generator.add_line("YepBoolean test%s = YepBooleanFalse;" % cpp_unit_test)
		self.cpp_unit_test_generator.add_line("if (argc == 1) {").indent()
		self.cpp_unit_test_generator.add_c_comment("No tests specified: run all tests")
		for cpp_unit_test in self.cpp_unit_tests:
			self.cpp_unit_test_generator.add_line("test%s = YepBooleanTrue;" % cpp_unit_test)
		self.cpp_unit_test_generator.dedent().add_line("} else {").indent()
		self.cpp_unit_test_generator.add_c_comment("Some tests specified: run only specified tests")
		self.cpp_unit_test_generator.add_line("for (int i = 1; i < argc; i++) {").indent()
		for i, cpp_unit_test in enumerate(self.cpp_unit_tests):
			if i == 0:
				self.cpp_unit_test_generator.add_line("if (strcmp(argv[i], \"%s\") == 0) {" % cpp_unit_test[len(group_name) + 1:]).indent()
				self.cpp_unit_test_generator.add_line("test%s = YepBooleanTrue;" % cpp_unit_test)
			else:
				self.cpp_unit_test_generator.dedent().add_line("} else if (strcmp(argv[i], \"%s\") == 0) {" % cpp_unit_test[len(group_name) + 1:]).indent()
				self.cpp_unit_test_generator.add_line("test%s = YepBooleanTrue;" % cpp_unit_test)
		self.cpp_unit_test_generator.dedent().add_line("} else {").indent()
		self.cpp_unit_test_generator.add_line("fprintf(stderr, \"Unknown function name \\\"%s\\\"\", argv[i]);")
		self.cpp_unit_test_generator.add_line("return 1;")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.add_line("YepStatus status = _yepLibrary_InitCpuInfo();")
		self.cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
		self.cpp_unit_test_generator.add_line()
		
		self.cpp_unit_test_generator.add_line("Yep64u supportedIsaFeatures, supportedSimdFeatures, supportedSystemFeatures;")
		self.cpp_unit_test_generator.add_line("status = yepLibrary_GetCpuIsaFeatures(&supportedIsaFeatures);")
		self.cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
		self.cpp_unit_test_generator.add_line("status = yepLibrary_GetCpuSimdFeatures(&supportedSimdFeatures);")
		self.cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
		self.cpp_unit_test_generator.add_line("status = yepLibrary_GetCpuSystemFeatures(&supportedSystemFeatures);")
		self.cpp_unit_test_generator.add_line("assert(status == YepStatusOk);")
		self.cpp_unit_test_generator.add_line()

		self.cpp_unit_test_generator.add_line("Yep32s failedTests = 0;")		
		for cpp_unit_test in self.cpp_unit_tests:
			self.cpp_unit_test_generator.add_line("if YEP_LIKELY(test%s)" % cpp_unit_test).indent()
			self.cpp_unit_test_generator.add_line("failedTests += Test_%s(supportedIsaFeatures, supportedSimdFeatures, supportedSystemFeatures);" % cpp_unit_test)
			self.cpp_unit_test_generator.dedent()
		self.cpp_unit_test_generator.add_line("return failedTests;")
		self.cpp_unit_test_generator.dedent().add_line("}")
		self.cpp_unit_test_generator.add_line()

		with open("library/sources/{0}/{1}.disp.h".format(module_name.lower(), group_name), "w+") as dispatch_header_file:
			dispatch_header_file.write(self.dispatch_table_header_generator.get_code())
			dispatch_header_file.write(self.dispatch_pointer_header_generator.get_code())
			dispatch_header_file.write(self.initialization_function_generator.get_code())
			dispatch_header_file.write("\n")

		with open("library/sources/{0}/{1}.disp.cpp".format(module_name.lower(), group_name), "w+") as dispatch_table_file:
			dispatch_table_file.write(self.dispatch_table_generator.get_code())
			dispatch_table_file.write(self.dispatch_pointer_generator.get_code())
			dispatch_table_file.write(self.dispatch_function_generator.get_code())
			dispatch_table_file.write("\n")

		with open("library/sources/{0}/{1}.impl.cpp".format(module_name.lower(), group_name), "w+") as default_cpp_implementation_file:
			default_cpp_implementation_file.write(self.default_cpp_implementation_generator.get_code())

		with open("bindings/java/sources-jni/{0}/{1}.c".format(module_name.lower(), group_name), "w+") as jni_implementation_file:
			jni_implementation_file.write(self.jni_implementation_generator.get_code())

		with open("bindings/clr/sources-csharp/{0}/{1}.cs".format(module_name.lower(), group_name), "w+") as csharp_implementation_file:
			csharp_implementation_file.write(self.csharp_safe_method_generator.get_code())
			csharp_implementation_file.write(self.csharp_unsafe_method_generator.get_code())
			csharp_implementation_file.write(self.csharp_dllimport_method_generator.get_code())

		for assembly_implementation_generator in self.assembly_implementation_generators:
			with open('library/sources/{0}/{1}.{2}.asm'.format(module_name.lower(), group_name, assembly_implementation_generator.abi.get_name()), "w+") as assembly_implementation_file:
				assembly_implementation_file.write(str(assembly_implementation_generator))

		if self.unit_test:
			with open("unit-tests/sources/{0}/{1}.cpp".format(module_name.lower(), group_name), "w+") as cpp_unit_test_file:
				cpp_unit_test_file.write(self.cpp_unit_test_generator.get_code())

	def generate(self, declaration):
		specialization = FunctionSpecialization(declaration)
		for assembly_implementation in self.assembly_implementations:
			for assembly_implementation_generator in self.assembly_implementation_generators:
				specialization.generate_assembly_implementation(assembly_implementation_generator, assembly_implementation, self.assembly_cache[assembly_implementation_generator.abi])
		specialization.generate_public_header(self.public_header_generator, self.c_documentation)
		specialization.generate_dispatch_table_header(self.dispatch_table_header_generator)
		specialization.generate_dispatch_pointer_header(self.dispatch_pointer_header_generator)
		specialization.generate_dispatch_table(self.dispatch_table_generator)
		specialization.generate_initialization_function(self.initialization_function_generator)
		specialization.generate_dispatch_pointer(self.dispatch_pointer_generator)
		specialization.generate_dispatch_function(self.dispatch_function_generator)
		specialization.generate_default_cpp_implementation(self.default_cpp_implementation_generator, self.default_cpp_implementation)
		specialization.generate_jni_function(self.jni_implementation_generator)
		specialization.generate_java_method(self.java_class_generator, self.java_documentation)
		specialization.generate_fortran_interface(self.fortran_module_generator, self.c_documentation)
		specialization.generate_csharp_dllimport_method(self.csharp_dllimport_method_generator)
		specialization.generate_csharp_unsafe_method(self.csharp_unsafe_method_generator, self.c_documentation, self.java_documentation)
		specialization.generate_csharp_safe_method(self.csharp_safe_method_generator, self.java_documentation)

		if self.unit_test:
			specialization.generate_cpp_unit_test(self.cpp_unit_test_generator, self.unit_test, self.cpp_unit_tests)
