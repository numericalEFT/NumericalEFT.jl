/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */
 
.macro BEGIN_ARM_FUNCTION name
	.arm
	.globl \name
	.align 2
	.func \name
	\name:
.endm

.macro END_ARM_FUNCTION name
	.endfunc
	.type \name, %function
	.size \name, .-\name
.endm

.macro BEGIN_THUMB_FUNCTION name
	.thumb
	.globl \name
	.align 2
	.func \name
	.thumb_func
	\name:
.endm

.macro END_THUMB_FUNCTION name
	.endfunc
	.type \name, %function
	.size \name, .-\name
.endm

.syntax unified

BEGIN_ARM_FUNCTION _yepLibrary_ProbeV6K
	.internal _yepLibrary_ProbeV6K
	.arch armv7-a
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	CLREX
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeV6K

BEGIN_ARM_FUNCTION _yepLibrary_ProbeV7MP
	.internal _yepLibrary_ProbeV6K
	.arch armv7-a
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	@ .cpu cortex-a9
	@ PLDW [sp]
	.long 0xF59DF000
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeV7MP

BEGIN_ARM_FUNCTION _yepLibrary_ProbeDiv
	.internal _yepLibrary_ProbeDiv
	.arch armv7-a
	MOV r0, #0
	MOV r1, #1
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	@ .cpu cortex-a15
	@ UDIV r1, r1, r1
	.long 0xE731F111
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeDiv

BEGIN_ARM_FUNCTION _yepLibrary_ProbeXScale
	.internal _yepLibrary_ProbeXScale
	.arch xscale
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	MIATT acc0, r0, r0
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeXScale

BEGIN_ARM_FUNCTION _yepLibrary_ProbeCnt32
	.internal _yepLibrary_ProbeCnt32
	.arch armv5t
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	@ Marvell-specific assembly instruction (not supported by GNU toolchain):
	@ 	CNT32 r0, r0
	MRC p6, 2, r0, cr0, cr0, 0
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeCnt32

BEGIN_ARM_FUNCTION _yepLibrary_ProbeVFP3
	.internal _yepLibrary_ProbeVFP3
	.arch armv7-a
	.fpu vfpv3-d16
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	VCVT.F32.S16 s0, s0, 1
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeVFP3

BEGIN_ARM_FUNCTION _yepLibrary_ProbeVFPd32
	.internal _yepLibrary_ProbeVFPd32
	.arch armv7-a
	.fpu vfpv3
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	VMOV.F64 d16, d16
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeVFPd32

BEGIN_ARM_FUNCTION _yepLibrary_ProbeVFP3HP
	.internal _yepLibrary_ProbeVFP3HP
	.arch armv7-a
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	@ .fpu vfpv3-fp16
	@ VCVTB.F16.F32 s0, s0
	CDP P10, 0xB, c0, c3, c0, 2
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeVFP3HP

BEGIN_ARM_FUNCTION _yepLibrary_ProbeVFP4
	.internal _yepLibrary_ProbeVFP4
	.arch armv7-a
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	@ .fpu vfpv4
	@ VFMA.F32 s0, s0, s0
	CDP P10, 0xA, c0, c0, c0, 0
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeVFP4

BEGIN_ARM_FUNCTION _yepLibrary_ProbeNeonHp
	.internal _yepLibrary_ProbeNeonHp
	.arch armv7-a
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	@ .fpu neon-fp16
	@ VCVT.F32.F16 q0, d0
	.long 0xF3B60700
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeNeonHp

BEGIN_ARM_FUNCTION _yepLibrary_ProbeNeon2
	.internal _yepLibrary_ProbeNeon2
	.arch armv7-a
	MOV r0, #0
	@ If the next line raises SIGILL,  the signal handler will change r0 to 1 and skip the instruction (4 bytes)
	@ .fpu neon-vfpv4
	@ VFMA.F32 d0, d0, d0
	.long 0xF2000C10
	BX lr
END_ARM_FUNCTION _yepLibrary_ProbeNeon2

BEGIN_ARM_FUNCTION _yepLibrary_ReadFPSID
	.internal _yepLibrary_ReadFPSID
	.arch armv5t
	.fpu vfp
	MOV r0, #1
	@ If the next line raises SIGILL,  the signal handle will set r0 to 0 and skip the instruction (4 bytes)
	MRC p10, 0x7, r1, cr0, cr0, 0
	BX lr
END_ARM_FUNCTION _yepLibrary_ReadFPSID

BEGIN_ARM_FUNCTION _yepLibrary_ReadMVFR0
	.internal _yepLibrary_ReadMVFR0
	.arch armv5t
	.fpu vfp
	MOV r0, #1
	@ If the next line raises SIGILL,  the signal handle will set r0 to 0 and skip the instruction (4 bytes)
	MRC p10, 0x7, r1, cr7, cr0, 0
	BX lr
END_ARM_FUNCTION _yepLibrary_ReadMVFR0

BEGIN_ARM_FUNCTION _yepLibrary_ReadWCID
	.internal _yepLibrary_ReadWCID
	.arch armv5t
	MOV r0, #1
	@ If the next line raises SIGILL,  the signal handle will set r0 to 0 and skip the instruction (4 bytes)
	MRC p1, 0, r1, c0, c0
	BX lr
END_ARM_FUNCTION _yepLibrary_ReadWCID

BEGIN_ARM_FUNCTION _yepLibrary_ReadPMCCNTR
	.internal _yepLibrary_ReadPMCCNTR
	.arch armv7-a
	MOV r0, #1
	@ If the next line raises SIGILL,  the signal handle will set r0 to 0 and skip the instruction (4 bytes)
	MRC p15, 0, r1, c9, c13, 0
	BX lr
END_ARM_FUNCTION _yepLibrary_ReadPMCCNTR

BEGIN_ARM_FUNCTION _yepLibrary_ReadPMCR
	.internal _yepLibrary_ReadPMCR
	.arch armv7-a
	MOV r0, #1
	@ If the next line raises SIGILL,  the signal handle will set r0 to 0 and skip the instruction (4 bytes)
	MRC p15, 0, r1, c9, c12, 0
	BX lr
END_ARM_FUNCTION _yepLibrary_ReadPMCR
