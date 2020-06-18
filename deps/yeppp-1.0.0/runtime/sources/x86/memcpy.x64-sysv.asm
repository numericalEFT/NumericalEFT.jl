;
;               Yeppp! library runtime infrastructure
;
; This file is part of Yeppp! library and licensed under MIT license.
; See runtime/LICENSE.txt for details.
;
;

%ifidn __OUTPUT_FORMAT__,elf64
section .text.memcpy align=32
global memcpy:function internal
memcpy:
%else
section .text
global _memcpy
_memcpy:
%endif
	MOV rax, rdi

.process_by_1_prologue:
	TEST rdi, 15
	JZ .process_by_32_prologue

	MOVZX ecx, byte [rsi]
	MOV [rdi], cl
	ADD rsi, 1
	ADD rdi, 1
	SUB rdx, 1
	JNZ .process_by_1_prologue

.process_by_32_prologue:
	SUB rdx, 32
	JB .process_by_32_epilogue
	
	align 32
.process_by_32:
	MOVUPS xmm0, [byte rsi]
	MOVUPS xmm1, [byte rsi + 16]
	MOVAPS [byte rdi + 0], xmm0
	MOVAPS [byte rdi + 16], xmm1
	ADD rsi, 32
	ADD rdi, 32
	SUB rdx, 32
	JAE .process_by_32

.process_by_32_epilogue:
	ADD rdx, 32
	JZ .return

.process_by_1_epilogue:
	MOVZX ecx, byte [rsi]
	MOV [rdi], cl
	ADD rsi, 1
	ADD rdi, 1
	SUB rdx, 1
	JNZ .process_by_1_epilogue

.return:
	RET
