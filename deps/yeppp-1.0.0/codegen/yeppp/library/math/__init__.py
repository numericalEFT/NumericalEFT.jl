#
#					  Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.
#

class Log:
	c2  = '-0x1.FFFFFFFFFFFF2p-2'
	c3  =  '0x1.5555555555103p-2'
	c4  = '-0x1.00000000013C7p-2'
	c5  =  '0x1.9999999A43E4Fp-3'
	c6  = '-0x1.55555554A6A2Bp-3'
	c7  =  '0x1.249248DAE4B2Ap-3'
	c8  = '-0x1.FFFFFFBD8606Dp-4'
	c9  =  '0x1.C71C90DB06248p-4'
	c10 = '-0x1.9999C5BE751E3p-4'
	c11 =  '0x1.745980F3FB889p-4'
	c12 = '-0x1.554D5ACD502ABp-4'
	c13 =  '0x1.3B4ED39194B87p-4'
	c14 = '-0x1.25480A82633AFp-4'
	c15 =  '0x1.0F23916A44515p-4'
	c16 = '-0x1.EED2E2BB64B2Ep-5'
	c17 =  '0x1.EA17E14773369p-5'
	c18 = '-0x1.1654764F478ECp-4'
	c19 =  '0x1.0266CD08DB2F2p-4'
	c20 = '-0x1.CC4EC078138E3p-6'
	
	sqrt2 = '0x1.6A09E667F3BCDp+0'
	one   = '0x1.0000000000000p+0'

	mantissa_mask = 0x000FFFFFFFFFFFFFL
	min_normal = '0x1.0000000000000p-1022'
	max_normal = '0x1.FFFFFFFFFFFFFp+1023'
	x_min = 0x0010000000000000L
	x_max = 0x7FEFFFFFFFFFFFFFL

	denormal_magic = '0x1.0000000000000p+52'
	denormal_exponent_shift = 1022L + 52L
	exponent_mask  = 0x7FF0000000000000L
	exponent_magic = 0x4338000000000000L - 1023L
	exponent_bias  = '0x1.8000000000000p+52'
	
	exponent_shift = 1023
	normalized_exponent_bias   = '0x1.800003FF00000p+32'
	denormalized_exponent_bias = '0x1.8000083100000p+32'
	scaled_exponent_magic	  = '0x1.8000000000000p+32'
	nan	   = "nan"
	plus_inf  = "+inf"
	minus_inf = "-inf"

	class preFMA:
		ln2_hi = '0x1.62E42FEFA3800p-1'
		ln2_lo = '0x1.EF35793C76730p-45'

	class FMA:
		ln2_hi = '0x1.62E42FEFA39EFp-1'
		ln2_lo = '0x1.ABC9E3B39803Fp-56'

class Exp:
	c2  = '0x1.0000000000005p-1'
	c3  = '0x1.5555555555540p-3'
	c4  = '0x1.5555555552115p-5'
	c5  = '0x1.11111111173CAp-7'
	c6  = '0x1.6C16C17F2BF99p-10'
	c7  = '0x1.A01A017EEB164p-13'
	c8  = '0x1.A019A6AC02A7Dp-16'
	c9  = '0x1.71DE71651CE7Ap-19'
	c10 = '0x1.28A284098D813p-22'
	c11 = '0x1.AE9043CA87A40p-26'

	magic_bias = '0x1.8000000000000p+52'
	log2e	  = '0x1.71547652B82FEp+0'

	zero_cutoff = '-0x1.74910D52D3051p+9'
	inf_cutoff  =  '0x1.62E42FEFA39EFp+9'
	
	min_exponent	 = -1022L << 52
	max_exponent	 = 1023L << 52
	default_exponent = 0x3FF0000000000000L

	nan	   = "nan"
	plus_inf  = "+inf"
	minus_inf = "-inf"

	class preFMA:
		minus_ln2_hi = '-0x1.62E42FEFA3800p-1'
		minus_ln2_lo = '-0x1.EF35793C76730p-45'
		# The largest x for which s does not overflow
		max_normal = '0x1.62B7D369A5AA7p+9'
		x_max = 0x40862B7D369A5AA7
		# The smallest x for which s is normal
		min_normal = '-0x1.625F1A5DA9C19p+9'
		x_min = 0xC08625F1A5DA9C19

	class FMA:
		minus_ln2_hi = '-0x1.62E42FEFA39EFp-1'
		minus_ln2_lo = '-0x1.ABC9E3B39803Fp-56'

		# The largest x for which s does not overflow
		max_normal = '0x1.62B7D369A5AA8p+9'
		x_max = 0x40862B7D369A5AA8
		# The smallest x for which s is normal
		min_normal = '-0x1.625F1A5DA9C19p+9'
		x_min = 0xC08625F1A5DA9C19

class TrigReduction:
	minus_pi_o2_hi = '-0x1.921FB54440000p+0'
	minus_pi_o2_me = '-0x1.68C234C4C8000p-39'
	minus_pi_o2_lo =  '0x1.9D747F23E32EDp-79'
	two_over_pi	=  '0x1.45F306DC9C883p-1'

class Sin:
	sign_mask  = '-0x0.0000000000000p+0'
	magic_bias =  '0x1.8000000000000p+52'

	c3  = '-0x1.5555555555546p-3'
	c5  =  '0x1.111111110F51Ep-7'
	c7  = '-0x1.A01A019BB92C0p-13'
	c9  =  '0x1.71DE3535C8A8Ap-19'
	c11 = '-0x1.AE5E38936D046p-26'
	c13 =  '0x1.5D8711D281543p-33'

class Cos:
	sign_mask  = '-0x0.0000000000000p+0'
	magic_bias =  '0x1.8000000000002p+52'

	c0  =  '0x1.0000000000000p+0'
	c2  = '-0x1.0000000000000p-1'
	c4  =  '0x1.555555555554Bp-5'
	c6  = '-0x1.6C16C16C15038p-10'
	c8  =  '0x1.A01A019C94874p-16'
	c10 = '-0x1.27E4F7F65104Fp-22'
	c12 =  '0x1.1EE9DF6693F7Ep-29'
	c14 = '-0x1.8FA87EF79AE3Fp-37'
	
	minus_c0  = '-0x1.0000000000000p+0'
	minus_c2  =  '0x1.0000000000000p-1'
	minus_c4  = '-0x1.555555555554Bp-5'
	minus_c6  =  '0x1.6C16C16C15038p-10'
	minus_c8  = '-0x1.A01A019C94874p-16'
	minus_c10 =  '0x1.27E4F7F65104Fp-22'
	minus_c12 = '-0x1.1EE9DF6693F7Ep-29'
	minus_c14 =  '0x1.8FA87EF79AE3Fp-37'

class Tan:
	magic_bias    = '0x1.8000000000000p+52'
	half          = '0x1.0000000000000p-1'
	one           = '0x1.0000000000000p+0'

	exponent_mask = 'inf'
