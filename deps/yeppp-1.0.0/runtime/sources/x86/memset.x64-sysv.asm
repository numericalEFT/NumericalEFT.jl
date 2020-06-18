;
;               Yeppp! library runtime infrastructure
;
; This file is part of Yeppp! library and licensed under MIT license.
; See runtime/LICENSE.txt for details.
;
;

%ifidn __OUTPUT_FORMAT__,elf64
section .text.memset align=32
global memset:function internal
memset:
%else
section .text
global _memset
_memset:
%endif
	MOV rax, rdi
	MOV ecx, esi

.process_by_1_prologue:
	TEST rdi, 15
	JZ .process_by_32_prologue

	MOV [rdi], cl
	ADD rdi, 1
	SUB rdx, 1
	JNZ .process_by_1_prologue

.process_by_32_prologue:
	SUB rdx, 32
	JB .process_by_32_epilogue

	MOVZX ecx, cl
	IMUL ecx, ecx, 0x01010101
	MOVD xmm0, ecx
	SHUFPS xmm0, xmm0, 0

	align 16
.process_by_32:
	MOVAPS [byte rdi + 0], xmm0
	MOVAPS [byte rdi + 16], xmm0
	ADD rdi, 32
	SUB rdx, 32
	JAE .process_by_32

.process_by_32_epilogue:
	ADD rdx, 32
	JZ .return

.process_by_1_epilogue:
	MOV [rdi], cl
	ADD rdi, 1
	SUB rdx, 1
	JNZ .process_by_1_epilogue

.return:
	RET
