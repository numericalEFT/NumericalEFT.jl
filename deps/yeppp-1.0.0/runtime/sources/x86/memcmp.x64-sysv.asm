;
;               Yeppp! library runtime infrastructure
;
; This file is part of Yeppp! library and licensed under MIT license.
; See runtime/LICENSE.txt for details.
;
;

%ifidn __OUTPUT_FORMAT__,elf64
section .text.memset align=32
global memcmp:function internal
memcmp:
%else
section .text align=32
global _memcmp
_memcmp:
%endif
.process_by_1_prologue:
	TEST rdi, 15
	JZ .process_by_16_prologue

	MOVZX eax, byte [rdi]
	MOVZX ecx, byte [rsi]
	CMP eax, ecx
	JNZ .return_sign

	ADD rdi, 1
	ADD rsi, 1
	SUB rdx, 1
	JNZ .process_by_1_prologue

.process_by_16_prologue:
	SUB rdx, 16
	JB .process_by_16_epilogue

	align 16
.process_by_16:
	MOVDQU xmm0, [rsi]
	PCMPEQB xmm0, [rdi]
	PMOVMSKB eax, xmm0
	XOR eax, 0xFFFF
	JNZ .find_mismatch

	ADD rdi, 16
	ADD rsi, 16
	SUB rdx, 16
	JAE .process_by_16

.process_by_16_epilogue:
	ADD rdx, 16
	JZ .return_zero

.process_by_1_epilogue:
	MOVZX eax, byte [rdi]
	MOVZX ecx, byte [rsi]
	CMP eax, ecx
	JNZ .return_sign

	ADD rdi, 1
	ADD rsi, 1
	SUB rdx, 1
	JNZ .process_by_1_epilogue

.return_zero:
	XOR eax, eax
	RET

.find_mismatch:
	BSF ecx, eax
	MOVZX eax, byte [rdi + rcx * 1]
	MOVZX ecx, byte [rsi + rcx * 1]
	CMP eax, ecx

.return_sign:
	SETA al
	SETB cl
	SUB al, cl
	MOVSX eax, al
	RET
