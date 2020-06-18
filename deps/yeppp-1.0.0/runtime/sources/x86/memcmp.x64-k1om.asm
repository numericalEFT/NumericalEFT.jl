#
#               Yeppp! library runtime infrastructure
#
# This file is part of Yeppp! library and licensed under MIT license.
# See runtime/LICENSE.txt for details.
#
#

.include "common.inc"

BEGIN_X86_FUNCTION memcmp
BEGIN_X86_SUBFUNCTION _intel_fast_memcmp
	CLD
	MOV rdx, rcx
	REPE CMPSB
	JNE .return_sign
	XOR eax, eax
	RET

.return_sign:
	MOVZX eax, byte ptr [rsi - 1]
	MOVZX ecx, byte ptr [rdi - 1]
	CMP eax, ecx
	SETA al
	SETB cl
	SUB al, cl
	MOVSX eax, al
	RET
END_X86_SUBFUNCTION _intel_fast_memcmp
END_X86_FUNCTION memcmp
