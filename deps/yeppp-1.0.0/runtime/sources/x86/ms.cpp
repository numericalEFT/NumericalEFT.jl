/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.hpp>
#include <yepIntrinsics.h>

#if defined(YEP_CUSTOM_RUNTIME)

	#if defined(YEP_MICROSOFT_COMPILER)

		/** The Microsoft compiler generates a reference to this symbol for any C++ source which uses floating-point types. The sole purpose of this symbol is to guarantee that floating-point support library from Microsoft CRT is linked. */
		extern "C" YEP_PRIVATE_SYMBOL int _fltused = 1;

		#pragma function(memcpy)
		extern "C" void * __cdecl memcpy(void *destination, const void *source, size_t num) {
			__movsb(static_cast<unsigned char*>(destination), static_cast<unsigned const char*>(source), num);
			return destination;
		}

		#pragma function(memset)
		extern "C" void * __cdecl memset(void *ptr, int value, size_t num) {
			__stosb(static_cast<unsigned char*>(ptr), static_cast<unsigned char>(value), num);
			return ptr;
		}

		#pragma function(memcmp)
		extern "C" int __cdecl memcmp(const void *ptrX, const void *ptrY, size_t num) {
			const Yep8u *curX = static_cast<const Yep8u*>(ptrX);
			const Yep8u *curY = static_cast<const Yep8u*>(ptrY);
			while (num--) {
				const Yep32u x = Yep32u(Yep8u(*curX++));
				const Yep32u y = Yep32u(Yep8u(*curY++));
				if (x != y) {
					return x - y;
				}
			}
			return 0;
		}

		#pragma function(sqrt)
		#if defined(YEP_X64_CPU)
			extern "C" YEP_PRIVATE_SYMBOL double sqrt(double x) {
				const __m128d xmm = _mm_set_sd(x);
				return _mm_cvtsd_f64(_mm_sqrt_sd(xmm, xmm));
			}
			
		#elif defined(YEP_X86_CPU)
			extern "C" YEP_PRIVATE_SYMBOL __declspec(naked) double __cdecl sqrt(double x) {
				__asm {
					fld QWORD PTR [esp + 4]
					fsqrt
					ret
				}
			}
			
			extern "C" YEP_PRIVATE_SYMBOL __declspec(naked) Yep64u __cdecl _allshl(Yep64u x, Yep32u n) {
				__asm {
					test cl, 32
					jnz large_shift
					shld edx, eax, cl
					shl eax, cl
					ret
				large_shift:
					shl eax, cl
					mov edx, eax
					xor eax, eax
					ret
				}
			}

			extern "C" YEP_PRIVATE_SYMBOL Yep64u __cdecl _allmul(Yep64u a, Yep64u b) {
				const Yep32u aLow = yepBuiltin_GetLowPart_64u_32u(a);
				const Yep32u aHigh = yepBuiltin_GetHighPart_64u_32u(a);
				const Yep32u bLow = yepBuiltin_GetLowPart_64u_32u(b);
				const Yep32u bHigh = yepBuiltin_GetHighPart_64u_32u(b);

				const Yep64u x = yepBuiltin_Multiply_32u32u_64u(aLow, bLow);
				const Yep32u xLow = yepBuiltin_GetLowPart_64u_32u(x);
				const Yep32u xHigh = yepBuiltin_GetHighPart_64u_32u(x);

				const Yep32u c = aHigh * bLow;
				const Yep32u d = aLow * bHigh;
				return yepBuiltin_CombineParts_32u32u_64u(xHigh + c + d, xLow);
			}
			
			// Version from AMD Optimization manual
			extern "C" YEP_PRIVATE_SYMBOL __declspec(naked) Yep64u __cdecl _aulldiv(Yep64u a, Yep64u b) {
				__asm {
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
					mul  dword ptr [esp+20]   ; quotient * divisor low word
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
				}
			}
			
			// Version from AMD Optimization manual
			extern "C" YEP_PRIVATE_SYMBOL __declspec(naked) Yep64u __cdecl _alldiv(Yep64u a, Yep64u b) {
				__asm {
					push ebx    ; Save EBX as per calling convention.
					push esi    ; Save ESI as per calling convention.
					push edi    ; Save EDI as per calling convention.
					mov  ecx, [esp+28]   ; divisor_hi
					mov  ebx, [esp+24]   ; divisor_lo
					mov  edx, [esp+20]   ; dividend_hi
					mov  eax, [esp+16]   ; dividend_lo
					mov  esi, ecx        ; divisor_hi
					xor  esi, edx        ; divisor_hi ^ dividend_hi 
					sar  esi, 31         ; (quotient < 0) ? -1 : 0
					mov  edi, edx        ; dividend_hi
					sar  edi, 31         ; (dividend < 0) ? -1 : 0
					xor  eax, edi        ; If (dividend < 0),
					xor  edx, edi        ;  compute 1s complement of dividend.
					sub  eax, edi        ; If (dividend < 0),
					sbb  edx, edi        ;  compute 2s complement of dividend.
					mov  edi, ecx        ; divisor_hi
					sar  edi, 31         ; (divisor < 0) ? -1 : 0
					xor  ebx, edi        ; If (divisor < 0),
					xor  ecx, edi        ;  compute 1s complement of divisor.
					sub  ebx, edi        ; If (divisor < 0),
					sbb  ecx, edi        ;  compute 2s complement of divisor.
					jnz  big_divisor     ; divisor > 2^32 - 1
					cmp  edx, ebx        ; Only one division needed (ECX = 0)?
					jae  two_divs        ; Need two divisions.
					div  ebx             ; EAX = quotient_lo
					mov  edx, ecx        ; EDX = quotient_hi = 0 (quotient in EDX:EAX)
					xor  eax, esi        ; If (quotient < 0),
					xor  edx, esi        ;  compute 1s complement of result.
					sub  eax, esi        ; If (quotient < 0),
					sbb  edx, esi        ;  compute 2s complement of result.
					pop  edi             ; Restore EDI as per calling convention.
					pop  esi             ; Restore ESI as per calling convention.
					pop  ebx             ; Restore EBX as per calling convention.
					ret  16              ; Done, return to caller.
				two_divs:
					mov  ecx, eax     ; Save dividend_lo in ECX.
					mov  eax, edx     ; Get dividend_hi.
					xor  edx, edx     ; Zero-extend it into EDX:EAX.
					div  ebx          ; quotient_hi in EAX
					xchg eax, ecx     ; ECX = quotient_hi, EAX = dividend_lo
					div  ebx          ; EAX = quotient_lo
					mov  edx, ecx     ; EDX = quotient_hi (quotient in EDX:EAX)
					jmp  make_sign   ; Make quotient signed.
				big_divisor:
					sub  esp, 12             ; Create three local variables.
					mov  [esp], eax          ; dividend_lo
					mov  [esp+4], ebx        ; divisor_lo
					mov  [esp+8], edx        ; dividend_hi
					mov  edi, ecx            ; Save divisor_hi.
					shr  edx, 1              ; Shift both
					rcr  eax, 1              ;  divisor and
					ror  edi, 1              ;  and dividend
					rcr  ebx, 1              ;  right by 1 bit.
					bsr  ecx, ecx            ; ECX = number of remaining shifts
					shrd ebx, edi, cl        ; Scale down divisor and
					shrd eax, edx, cl        ;  dividend such that divisor is
					shr  edx, cl             ;  less than 2^32 (that is, fits in EBX).
					rol  edi, 1              ; Restore original divisor_hi.
					div  ebx                 ; Compute quotient.
					mov  ebx, [esp]          ; dividend_lo
					mov  ecx, eax            ; Save quotient.
					imul edi, eax            ; quotient * divisor high word (low only)
					mul  DWORD PTR [esp+4]   ; quotient * divisor low word
					add  edx, edi            ; EDX:EAX = quotient * divisor
					sub  ebx, eax            ; dividend_lo - (quot.*divisor)_lo
					mov  eax, ecx            ; Get quotient.
					mov  ecx, [esp+8]        ; dividend_hi
					sbb  ecx, edx            ; Subtract (divisor * quot.) from dividend
					sbb  eax, 0              ; Adjust quotient if remainder is negative.
					xor  edx, edx            ; Clear high word of quotient.
					add  esp, 12             ; Remove local variables.
				make_sign:
					xor eax, esi   ; If (quotient < 0),
					xor edx, esi   ;  compute 1s complement of result.
					sub eax, esi   ; If (quotient < 0),
					sbb edx, esi   ;  compute 2s complement of result.
					pop edi        ; Restore EDI as per calling convention.
					pop esi        ; Restore ESI as per calling convention.
					pop ebx        ; Restore EBX as per calling convention.
					ret 16         ; Done, return to caller.
				}
			}
			
			extern "C" YEP_PRIVATE_SYMBOL Yep64u _isaFeatures;
			
			// ControlWord = (PrecisionControl << 8) + (RoundingControl << 10)
			// PrecisionControl:
			// * 0b00 - single precision (24 bits)
			// * 0b01 - reserved
			// * 0b10 - double precision (53 bits)
			// * 0b11 - double extended precision (64 bits)
			// RoundingControl:
			// * 0b00 - round to nearest (even)
			// * 0b01 - round down (toward -inf)
			// * 0b10 - round up (toward +inf)
			// * 0b11 - round toward zero (truncate)
			extern "C" YEP_PRIVATE_SYMBOL __declspec(naked) long __cdecl _ftol2(double x) {
				__asm {
					// Control word: double precision + round toward zero
					fld QWORD PTR [esp + 4]
					push DWORD PTR 111000000000b
					fstcw WORD PTR [esp + 2]
					fldcw WORD PTR [esp]
					sub esp, 4
					fistp DWORD PTR [esp]
					fldcw WORD PTR [esp + 6]
					pop eax
					ret 4
				}
			}
			
			extern "C" YEP_PRIVATE_SYMBOL long __cdecl _ftol2_sse(double x) {
				__asm {
					// Control word: double precision + round toward zero
					fld QWORD PTR [esp + 4]
					push DWORD PTR 111000000000b
					fstcw WORD PTR [esp + 2]
					fldcw WORD PTR [esp]
					sub esp, 4
					fistp DWORD PTR [esp]
					fldcw WORD PTR [esp + 6]
					pop eax
					ret 4
				}
			}

			extern "C" YEP_PRIVATE_SYMBOL __declspec(naked) double __cdecl _CIsqrt(double x) {
				__asm {
					// Control word: double precision + round to even
					push DWORD PTR 001000000000b
					fstcw WORD PTR [esp + 2]
					fldcw WORD PTR [esp]
					fsqrt
					fldcw WORD PTR [esp + 2]
					ret 4
				}
			}
		#endif

	#endif

#endif