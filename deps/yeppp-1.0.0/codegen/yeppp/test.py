import peachpy.codegen
import yeppp.codegen

class ReferenceUnitTest:
	def __init__(self, max_ulp_error = None, **kwargs):
		self.arguments = kwargs
		self.max_ulp_error = max_ulp_error

class Uniform:
	def __init__(self, minimum = None, maximum = None):
		if minimum is None or isinstance(minimum, int) or isinstance(minimum, long) or isinstance(minimum, float):
			self.minimum = minimum
		else:
			raise TypeError('Minimum value must be of numeric type')
		if maximum is None or isinstance(maximum, int) or isinstance(maximum, long) or isinstance(maximum, float):
			self.maximum = maximum
		else:
			raise TypeError('Maximum value must be of numeric type')
		if self.minimum is not None and self.maximum is not None and self.maximum <= self.minimum:
			raise ValueError('Minimum value must be less than maximum value')

	def check(self, ctype):
		if isinstance(ctype, peachpy.c.Type):
			type = ctype.get_std_analog().primitive_type
			if type == 'uint8_t':
				minimum = None if self.minimum is None else int(self.minimum)
				maximum = None if self.maximum is None else int(self.maximum)
				if minimum is not None and (minimum > 255 or minimum < 0):
					raise ValueError('Minimum value {0} can not be represented as an 8-bit unsigned integer'.format(minimum))
				elif maximum is not None and (maximum > 255 or maximum < 0):
					raise ValueError('Maximum value {0} can not be represented as an 8-bit unsigned integer'.format(maximum))
			elif type == 'uint16_t':
				minimum = None if self.minimum is None else int(self.minimum)
				maximum = None if self.maximum is None else int(self.maximum)
				if minimum is not None and (minimum > 65535 or minimum < 0):
					raise ValueError('Minimum value {0} can not be represented as a 16-bit unsigned integer'.format(minimum))
				elif maximum is not None and (maximum > 65535 or maximum < 0):
					raise ValueError('Maximum value {0} can not be represented as a 16-bit unsigned integer'.format(maximum))
			elif type == 'uint32_t':
				minimum = None if self.minimum is None else int(self.minimum)
				maximum = None if self.maximum is None else int(self.maximum)
				if minimum is not None and (minimum > 4294967295 or minimum < 0):
					raise ValueError('Minimum value {0} can not be represented as a 32-bit unsigned integer'.format(minimum))
				elif maximum is not None and (maximum > 4294967295 or maximum < 0):
					raise ValueError('Maximum value {0} can not be represented as a 32-bit unsigned integer'.format(maximum))
			elif type == 'uint64_t':
				minimum = None if self.minimum is None else int(self.minimum)
				maximum = None if self.maximum is None else int(self.maximum)
				if minimum is not None and (minimum > 18446744073709551615 or minimum < 0):
					raise ValueError('Minimum value {0} can not be represented as a 64-bit unsigned integer'.format(minimum))
				elif maximum is not None and (maximum > 18446744073709551615 or maximum < 0):
					raise ValueError('Maximum value {0} can not be represented as a 64-bit unsigned integer'.format(maximum))
			elif type == 'int8_t':
				minimum = None if self.minimum is None else int(self.minimum)
				maximum = None if self.maximum is None else int(self.maximum)
				if minimum is not None and (minimum > 127 or minimum < -128):
					raise ValueError('Minimum value {0} can not be represented as an 8-bit signed integer'.format(minimum))
				elif maximum is not None and (maximum > 127 or maximum < -128):
					raise ValueError('Maximum value {0} can not be represented as an 8-bit signed integer'.format(maximum))
			elif type == 'int16_t':
				minimum = None if self.minimum is None else int(self.minimum)
				maximum = None if self.maximum is None else int(self.maximum)
				if minimum is not None and (minimum > 32767 or minimum < -32768):
					raise ValueError('Minimum value {0} can not be represented as a 16-bit signed integer'.format(minimum))
				elif maximum is not None and (maximum > 32767 or maximum < -32768):
					raise ValueError('Maximum value {0} can not be represented as a 16-bit signed integer'.format(maximum))
			elif type == 'int32_t':
				minimum = None if self.minimum is None else int(self.minimum)
				maximum = None if self.maximum is None else int(self.maximum)
				if minimum is not None and (minimum > 2147483647 or minimum < -2147483648):
					raise ValueError('Minimum value {0} can not be represented as a 32-bit signed integer'.format(minimum))
				elif maximum is not None and (maximum > 2147483647 or maximum < -2147483648):
					raise ValueError('Maximum value {0} can not be represented as a 32-bit signed integer'.format(maximum))
			elif type == 'int64_t':
				minimum = None if self.minimum is None else int(self.minimum)
				maximum = None if self.maximum is None else int(self.maximum)
				if minimum is not None and (minimum > 9223372036854775807 or minimum < -9223372036854775808):
					raise ValueError('Minimum value {0} can not be represented as a 64-bit signed integer'.format(minimum))
				elif maximum is not None and (maximum > 9223372036854775807 or maximum < -9223372036854775808):
					raise ValueError('Maximum value {0} can not be represented as a 64-bit signed integer'.format(maximum))
			elif type == 'float':
				minimum = None if self.minimum is None else float(self.minimum)
				maximum = None if self.maximum is None else float(self.maximum)
				if minimum is not None and (minimum > 3.4028234e+38 or minimum < -3.4028234e+38):
					raise ValueError('Minimum value {0} can not be represented in single precision floating-point format'.format(minimum))
				elif maximum is not None and (maximum > 3.4028234e+38 or maximum < -3.4028234e+38):
					raise ValueError('Maximum value {0} can not be represented in single precision floating-point format'.format(maximum))
			elif type == 'double':
				minimum = None if self.minimum is None else float(self.minimum)
				maximum = None if self.maximum is None else float(self.maximum)
				if minimum is not None and (minimum > 1.7976931348623157e+308 or minimum < -1.7976931348623157e+308):
					raise ValueError('Minimum value {0} can not be represented in double precision floating-point format'.format(minimum))
				elif maximum is not None and (maximum > 1.7976931348623157e+308 or maximum < -1.7976931348623157e+308):
					raise ValueError('Maximum value {0} can not be represented in double precision floating-point format'.format(maximum))
			else:
				raise TypeError('Unknown primitive type {0}'.format(type))
		else:
			raise TypeError('Invalid value type {0}'.format(ctype))

	def format(self, ctype):
		if isinstance(ctype, peachpy.c.Type):
			type = ctype.get_std_analog().primitive_type
			if type == 'uint8_t':
				return ["0u" if self.minimum is None else str(int(self.minimum)) + "u",
						"255u" if self.maximum is None else str(int(self.maximum)) + "u"]
			elif type == 'uint16_t':
				return ["0u" if self.minimum is None else str(int(self.minimum)) + "u",
						"65535u" if self.maximum is None else str(int(self.maximum)) + "u"]
			elif type == 'uint32_t':
				return ["0u" if self.minimum is None else str(int(self.minimum)) + "u",
						"4294967295u" if self.maximum is None else str(int(self.maximum)) + "u"]
			elif type == 'uint64_t':
				return ["0ull" if self.minimum is None else str(int(self.minimum)) + "ull",
						"18446744073709551615ull" if self.maximum is None else str(int(self.maximum)) + "ull"]
			elif type in 'int8_t':
				return ["-128" if self.minimum is None else str(int(self.minimum)),
						"127" if self.maximum is None else str(int(self.maximum))]
			elif type in 'int16_t':
				return ["-32768" if self.minimum is None else str(int(self.minimum)),
						"32767" if self.maximum is None else str(int(self.maximum))]
			elif type in 'int32_t':
				return ["-2147483648" if self.minimum is None else str(int(self.minimum)),
						"2147483647" if self.maximum is None else str(int(self.maximum))]
			elif type == 'int64_t':
				return ["-9223372036854775808ll" if self.minimum is None else str(int(self.minimum)) + "ll",
						"9223372036854775807ll" if self.maximum is None else str(int(self.maximum)) + "ll"] 
			elif type == 'float':
				return ["-1.0f" if self.minimum is None else str(float(self.minimum)) + "f",
						"1.0f" if self.maximum is None else str(float(self.maximum)) + "f"] 
			elif type == 'double':
				return ["-1.0" if self.minimum is None else str(float(self.minimum)),
						"1.0" if self.maximum is None else str(float(self.maximum))]
			else:
				raise TypeError('Unknown primitive type {0}'.format(type))
		else:
			raise TypeError('Invalid value type {0}'.format(ctype))
