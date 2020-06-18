;
;               Yeppp! library runtime infrastructure
;
; This file is part of Yeppp! library and licensed under MIT license.
; See runtime/LICENSE.txt for details.
;
;

%ifidn __OUTPUT_FORMAT__,elf32
section .text.__udivdi3 align=32
global __udivdi3:function internal
%else
section .text
global __udivdi3
%endif

; Version from AMD Optimization manual
__udivdi3:
	push ebx             ; Save EBX as per calling convention.
	mov  ecx, [esp+20]   ; divisor_hi
	mov  ebx, [esp+16]   ; divisor_lo
	mov  edx, [esp+12]   ; dividend_hi
	mov  eax, [esp+8]    ; dividend_lo
	test ecx, ecx        ; divisor > (2^32 – 1)?
	jnz  big_divisor     ; Yes, divisor > 2^32 – 1.
	cmp  edx, ebx        ; Only one division needed (ECX = 0)?
	jae  two_divs        ; Need two divisions.
	div  ebx             ; EAX = quotient_lo
	mov  edx, ecx        ; EDX = quotient_hi = 0 (quotient in EDX:EAX)
	pop  ebx             ; Restore EBX as per calling convention.
	ret  16              ; Done, return to caller.
two_divs:
	mov  ecx, eax   ; Save dividend_lo in ECX.
	mov  eax, edx   ; Get dividend_hi.
	xor  edx, edx   ; Zero-extend it into EDX:EAX.
	div  ebx        ; quotient_hi in EAX
	xchg eax, ecx   ; ECX = quotient_hi, EAX = dividend_lo
	div  ebx        ; EAX = quotient_lo
	mov  edx, ecx   ; EDX = quotient_hi (quotient in EDX:EAX)
	pop  ebx        ; Restore EBX as per calling convention.
	ret  16         ; Done, return to caller.
big_divisor:
	push edi                  ; Save EDI as per calling convention.
	mov  edi, ecx             ; Save divisor_hi.
	shr  edx, 1               ; Shift both divisor and dividend right
	rcr  eax, 1               ;  by 1 bit.
	ror  edi, 1
	rcr  ebx, 1
	bsr  ecx, ecx             ; ECX = number of remaining shifts
	shrd ebx, edi, cl         ; Scale down divisor and dividend
	shrd eax, edx, cl         ;  such that divisor is less than
	shr  edx, cl              ;  2^32 (that is, it fits in EBX).
	rol  edi, 1               ; Restore original divisor_hi.
	div  ebx                  ; Compute quotient.
	mov  ebx, [esp+12]        ; dividend_lo
	mov  ecx, eax             ; Save quotient.
	imul edi, eax             ; quotient * divisor high word (low only)
	mul  dword [esp+20]   ; quotient * divisor low word
	add  edx, edi             ; EDX:EAX = quotient * divisor
	sub  ebx, eax             ; dividend_lo – (quot.*divisor)_lo
	mov  eax, ecx             ; Get quotient.
	mov  ecx, [esp+16]        ; dividend_hi
	sbb  ecx, edx             ; Subtract (divisor * quot.) from dividend.
	sbb  eax, 0               ; Adjust quotient if remainder negative.
	xor  edx, edx             ; Clear high word of quot. (EAX<=FFFFFFFFh).
	pop  edi                  ; Restore EDI as per calling convention.
	pop  ebx                  ; Restore EBX as per calling convention.
	ret  16                   ; Done, return to caller.
