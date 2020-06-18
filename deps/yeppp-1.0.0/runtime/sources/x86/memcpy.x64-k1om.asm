#
#               Yeppp! library runtime infrastructure
#
# This file is part of Yeppp! library and licensed under MIT license.
# See runtime/LICENSE.txt for details.
#
#

.include "common.inc"

BEGIN_X86_FUNCTION memcpy
BEGIN_X86_SUBFUNCTION _intel_fast_memcpy
	CLD
	MOV rcx, rdx
	MOV rax, rdi
	REP MOVSB
	RET
END_X86_SUBFUNCTION _intel_fast_memcpy
END_X86_FUNCTION memcpy
