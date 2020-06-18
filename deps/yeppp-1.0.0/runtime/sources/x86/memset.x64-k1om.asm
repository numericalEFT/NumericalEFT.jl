#
#               Yeppp! library runtime infrastructure
#
# This file is part of Yeppp! library and licensed under MIT license.
# See runtime/LICENSE.txt for details.
#
#

.include "common.inc"

BEGIN_X86_FUNCTION memset
BEGIN_X86_SUBFUNCTION _intel_fast_memset
	CLD
	MOV eax, esi
	MOV rcx, rdx
	MOV rdx, rdi
	REP STOSB
	MOV rax, rdi
	RET
END_X86_SUBFUNCTION _intel_fast_memset
END_X86_FUNCTION memset
