/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#if defined(_FORTIFY_SOURCE)
	#undef _FORTIFY_SOURCE
#endif

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <library/functions.h>
#include <yepBuiltin.h>

#if defined(YEP_LINUX_OS)
	#include <yepSyscalls.hpp>

	#include <string.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/syscall.h>
	#include <linux/unistd.h>
	#if defined(__ANDROID__)
		#include <sys/ioctl.h>

		#if defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)
			#define PERF_EVENT_IOC_ENABLE  _IO ('$', 0)
			#define PERF_EVENT_IOC_DISABLE _IO ('$', 1)
			#define PERF_EVENT_IOC_REFRESH _IO ('$', 2)
			#define PERF_EVENT_IOC_RESET   _IO ('$', 3)

			enum perf_type_id {
				PERF_TYPE_HARDWARE   = 0,
				PERF_TYPE_SOFTWARE   = 1,
				PERF_TYPE_TRACEPOINT = 2,
				PERF_TYPE_HW_CACHE   = 3,
				PERF_TYPE_RAW        = 4,
				PERF_TYPE_BREAKPOINT = 5
			};

			enum perf_hw_id {
				PERF_COUNT_HW_CPU_CYCLES              = 0,
				PERF_COUNT_HW_INSTRUCTIONS            = 1,
				PERF_COUNT_HW_CACHE_REFERENCES        = 2,
				PERF_COUNT_HW_CACHE_MISSES            = 3,
				PERF_COUNT_HW_BRANCH_INSTRUCTIONS     = 4,
				PERF_COUNT_HW_BRANCH_MISSES           = 5,
				PERF_COUNT_HW_BUS_CYCLES              = 6,
				PERF_COUNT_HW_STALLED_CYCLES_FRONTEND = 7,
				PERF_COUNT_HW_STALLED_CYCLES_BACKEND  = 8
			};

			enum perf_hw_cache_id {
				PERF_COUNT_HW_CACHE_L1D  = 0,
				PERF_COUNT_HW_CACHE_L1I  = 1,
				PERF_COUNT_HW_CACHE_LL   = 2,
				PERF_COUNT_HW_CACHE_DTLB = 3,
				PERF_COUNT_HW_CACHE_ITLB = 4,
				PERF_COUNT_HW_CACHE_BPU  = 5,
				PERF_COUNT_HW_CACHE_NODE = 6
			};

			enum perf_hw_cache_op_id {
				PERF_COUNT_HW_CACHE_OP_READ     = 0,
				PERF_COUNT_HW_CACHE_OP_WRITE    = 1,
				PERF_COUNT_HW_CACHE_OP_PREFETCH = 2
			};

			enum perf_hw_cache_op_result_id {
				PERF_COUNT_HW_CACHE_RESULT_ACCESS = 0,
				PERF_COUNT_HW_CACHE_RESULT_MISS   = 1
			};

			enum perf_sw_ids {
				PERF_COUNT_SW_CPU_CLOCK        = 0,
				PERF_COUNT_SW_TASK_CLOCK       = 1,
				PERF_COUNT_SW_PAGE_FAULTS      = 2,
				PERF_COUNT_SW_CONTEXT_SWITCHES = 3,
				PERF_COUNT_SW_CPU_MIGRATIONS   = 4,
				PERF_COUNT_SW_PAGE_FAULTS_MIN  = 5,
				PERF_COUNT_SW_PAGE_FAULTS_MAJ  = 6,
				PERF_COUNT_SW_ALIGNMENT_FAULTS = 7,
				PERF_COUNT_SW_EMULATION_FAULTS = 8
			};

			struct perf_event_attr {
				Yep32u type;
				Yep32u size;
				Yep64u config;
				union {
					Yep64u sample_period;
					Yep64u sample_freq;
				};
				Yep64u sample_type;
				Yep64u read_format;
				Yep64u disabled       : 1,
				       inherit        : 1,
				       pinned         : 1,
				       exclusive      : 1,
				       exclude_user   : 1,
				       exclude_kernel : 1,
				       exclude_hv     : 1,
				       exclude_idle   : 1,
				       mmap           : 1,
				       comm           : 1,
				       freq           : 1,
				       inherit_stat   : 1,
				       enable_on_exec : 1,
				       task           : 1,
				       watermark      : 1,
				       precise_ip     : 2,
				       mmap_data      : 1,
				       sample_id_all  : 1,
				       exclude_host   : 1,
				       exclude_guest  : 1,
				       __reserved_1   : 43;

				union {
					Yep32u wakeup_events;
					Yep32u wakeup_watermark;
				};

				Yep32u bp_type;
				union {
					Yep64u bp_addr;
					Yep64u config1;
				};
				union {
					Yep64u bp_len;
					Yep64u config2;
				};
			};
		#endif
	#else
		#include <linux/perf_event.h>
	#endif
#endif

const Yep32u LinuxPerfEventCpuCounterStateMagic = 0xCAB06128u;
const Yep32u ArmCP15CpuCounterStateMagic = 0xF2C5E274u;

YepStatus YEPABI yepLibrary_GetCpuCyclesAcquire(Yep64u *statePointer) {
	if YEP_UNLIKELY(statePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
#if defined(YEP_X86_CPU)
	#if defined(YEP_X86_ABI)
		if YEP_UNLIKELY((_systemFeatures & YepSystemFeatureCycleCounter) == 0) {
			return YepStatusUnsupportedHardware;
		}
	#endif
	#if defined(YEP_MICROSOFT_COMPILER)
		Yep64u state;
		do {
			int registers[4];
			__cpuid(registers, 0);
			_ReadWriteBarrier(); // Prevent compiler from reordering CPUID and RDTSC
			state = __rdtsc();
		} while (state == 0);
		*statePointer = state;
		return YepStatusOk;
	#else
		Yep64u state;
		do {
			Yep32u lo, hi;
			#if defined(YEP_X64_ABI)
				asm volatile (
					"xorl %%eax,%%eax;"
					"cpuid;"
					"rdtsc;"
					: "=a" (lo), "=d" (hi)
					:
					: "%rbx", "%rcx"
				);
			#else
				#if defined(YEP_PIC)
					asm volatile (
						"xorl %%eax,%%eax;"
						"movl %%ebx, %%esi;"
						"cpuid;"
						"rdtsc;"
						"movl %%esi, %%ebx;"
						: "=a" (lo), "=d" (hi)
						:
						: "%esi", "%ecx"
					);
				#else
					asm volatile (
						"xorl %%eax,%%eax;"
						"cpuid;"
						"rdtsc;"
						: "=a" (lo), "=d" (hi)
						:
						: "%ebx", "%ecx"
					);
				#endif
			#endif
			state = yepBuiltin_CombineParts_32u32u_64u(hi, lo);
		} while (state == 0);
		*statePointer = state;
		return YepStatusOk;
	#endif
#elif defined(YEP_IA64_CPU)
	#if defined(YEP_MICROSOFT_COMPILER)
		Yep64u state;
		do {
			state = __getReg(__REG_IA64_ApITC);
		} while (state == 0);
		*statePointer = state;
		return YepStatusOk;
	#else
		#error "Not yet implemented for this combination of compiler and architecture (IA64)"
	#endif
#elif defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)
	#if defined(YEP_LINUX_OS)
		const Yep64u cycleCounterMask = YepSystemFeatureCycleCounter | YepSystemFeatureCycleCounter64Bit;
		const Yep64u cycleCounterPerfEvent = YepSystemFeatureCycleCounter | YepSystemFeatureCycleCounter64Bit;
		const Yep64u cycleCounterCP15 = YepSystemFeatureCycleCounter;
		if YEP_LIKELY((_systemFeatures & cycleCounterMask) == cycleCounterPerfEvent) {
			struct perf_event_attr perf_event_attributes;
			memset(&perf_event_attributes, 0, sizeof(perf_event_attributes));
			perf_event_attributes.size = sizeof(perf_event_attributes);
			perf_event_attributes.type = PERF_TYPE_HARDWARE;
			perf_event_attributes.config = PERF_COUNT_HW_CPU_CYCLES;
			perf_event_attributes.disabled = 1;

			const pid_t processId = 0; // Current process
			const int cpu = -1; // Any CPU
			const int groupFileDescriptor = -1; // New group
			const int flags = 0; // Default
			const int perfCounter = yepSyscall_perf_event_open(&perf_event_attributes, processId, cpu, groupFileDescriptor, flags);

			if YEP_LIKELY(perfCounter >= 0) {
				const int ioctlResetSyscallResult = yepSyscall_ioctl(perfCounter, PERF_EVENT_IOC_RESET);
				if YEP_LIKELY(ioctlResetSyscallResult == 0) {
					const int ioctlEnableSyscallResult = yepSyscall_ioctl(perfCounter, PERF_EVENT_IOC_ENABLE);
					if YEP_LIKELY(ioctlEnableSyscallResult == 0) {
						const Yep64u state = yepBuiltin_CombineParts_32u32u_64u(LinuxPerfEventCpuCounterStateMagic, Yep32u(perfCounter));
						*statePointer = state;
						return YepStatusOk;
					}
				}

				yepSyscall_close(perfCounter);
				return YepStatusSystemError;
			} else {
				return YepStatusSystemError;
			}
	#if defined(YEP_ARM_CPU)
		} else if YEP_UNLIKELY((_systemFeatures & cycleCounterMask) == cycleCounterCP15) {
			const Yep64u pmccntrReadResult = _yepLibrary_ReadCoprocessor(&_yepLibrary_ReadPMCCNTR);
			const Yep32u pmccntrReadSuccess = yepBuiltin_GetLowPart_64u_32u(pmccntrReadResult);
			if YEP_LIKELY(pmccntrReadSuccess) {
				const Yep32u pmccntr = yepBuiltin_GetHighPart_64u_32u(pmccntrReadResult);
				const Yep64u state = yepBuiltin_CombineParts_32u32u_64u(ArmCP15CpuCounterStateMagic, pmccntr);
				*statePointer = state;
				return YepStatusOk;
			} else {
				return YepStatusSystemError;
			}
	#endif
		} else {
			return YepStatusUnsupportedSoftware;
		}
	#elif defined(YEP_WINDOWS_OS)
		return YepStatusUnsupportedSoftware;
	#else
		#error "Not yet supported combination of OS and CPU"
	#endif
#else
	#error "Not yet implemented for this combination of compiler and architecture"
#endif
}

YepStatus YEPABI yepLibrary_GetCpuCyclesRelease(Yep64u* statePointer, Yep64u* ticksPointer) {
	if YEP_UNLIKELY(statePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(ticksPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
#if defined(YEP_X86_CPU)
	#if defined(YEP_MICROSOFT_COMPILER)
		if YEP_LIKELY(_isaFeatures & YepX86IsaFeatureRdtscp) {
			unsigned int aux;
			const Yep64u endTicks = __rdtscp(&aux);
			_ReadWriteBarrier(); // Prevent compiler from reordering RDTSCP and reading the state
			const Yep64u startTicks = *statePointer;
			*statePointer = 0;
			if YEP_LIKELY(startTicks != 0) {
				const Yep64u elapsedTicks = endTicks - startTicks;
				*ticksPointer = elapsedTicks;
				return YepStatusOk;
			} else {
				return YepStatusInvalidState;
			}
		} else {
			#if defined(YEP_X86_ABI)
			if YEP_LIKELY(_systemFeatures & YepSystemFeatureCycleCounter) {
			#endif
				int registers[4];
				__cpuid(registers, 0);
				_ReadWriteBarrier(); // Prevent compiler from reordering CPUID and RDTSC
				const Yep64u endTicks = __rdtsc();
				_ReadWriteBarrier(); // Prevent compiler from reordering RDTSC and reading the state
				const Yep64u startTicks = *statePointer;
				*statePointer = 0;
				if YEP_LIKELY(startTicks != 0) {
					const Yep64u elapsedTicks = endTicks - startTicks;
					*ticksPointer = elapsedTicks;
					return YepStatusOk;
				} else {
					return YepStatusInvalidState;
				}
			#if defined(YEP_X86_ABI)
			} else {
				return YepStatusUnsupportedHardware;
			}
			#endif
		}
	#else
		Yep32u lo, hi;
		#if !defined(YEP_K1OM_X64_ABI)
			if YEP_LIKELY(_isaFeatures & YepX86IsaFeatureRdtscp) {
				#if defined(YEP_X64_ABI)
					asm volatile (
						"rdtscp;"
						: "=a" (lo), "=d" (hi)
						:
						: "%rcx"
					);
				#else
					asm volatile (
						"rdtscp;"
						: "=a" (lo), "=d" (hi)
						:
						: "%ecx"
					);
				#endif
			} else {
		#else
			{
		#endif
			#if defined(YEP_X64_ABI)
				asm volatile (
					"xorl %%eax,%%eax;"
					"cpuid;"
					"rdtsc;"
					: "=a" (lo), "=d" (hi)
					:
					: "%rbx", "%rcx"
				);
			#else
				if YEP_LIKELY(_systemFeatures & YepSystemFeatureCycleCounter) {
					#if defined(YEP_PIC)
						asm volatile (
							"xorl %%eax,%%eax;"
							"movl %%ebx, %%esi;"
							"cpuid;"
							"rdtsc;"
							"movl %%esi, %%ebx;"
							: "=a" (lo), "=d" (hi)
							:
							: "%esi", "%ecx"
						);
					#else
						asm volatile (
							"xorl %%eax,%%eax;"
							"cpuid;"
							"rdtsc;"
							: "=a" (lo), "=d" (hi)
							:
							: "%ebx", "%ecx"
						);
					#endif
				} else {
					return YepStatusUnsupportedHardware;
				}
			#endif
		}
		const Yep64u endTicks = yepBuiltin_CombineParts_32u32u_64u(hi, lo);
		const Yep64u startTicks = *statePointer;
		*statePointer = 0;
		if YEP_LIKELY(startTicks != 0) {
			const Yep64u elapsedTicks = endTicks - startTicks;
			*ticksPointer = elapsedTicks;
			return YepStatusOk;
		} else {
			return YepStatusInvalidState;
		}
	#endif
#elif defined(YEP_IA64_CPU)
	#if defined(YEP_MICROSOFT_COMPILER)
		const Yep64u endTicks = __getReg(__REG_IA64_ApITC);
		const Yep64u startTicks = *statePointer;
		*statePointer = 0;
		if YEP_LIKELY(startTicks != 0) {
			const Yep64u elapsedTicks = endTicks - startTicks;
			*ticksPointer = elapsedTicks;
			return YepStatusOk;
		} else {
			return YepStatusInvalidState;
		}
	#else
		#error "Not yet implemented for this combination of compiler and architecture (IA64)"
	#endif
#elif defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)
	#if defined(YEP_LINUX_OS)
		const Yep64u cycleCounterMask = YepSystemFeatureCycleCounter | YepSystemFeatureCycleCounter64Bit;
		const Yep64u cycleCounterPerfEvent = YepSystemFeatureCycleCounter | YepSystemFeatureCycleCounter64Bit;
		const Yep64u cycleCounterCP15 = YepSystemFeatureCycleCounter;
		if YEP_LIKELY((_systemFeatures & cycleCounterMask) == cycleCounterPerfEvent) {
			const Yep64u state = *statePointer;
			*statePointer = 0;
			if YEP_LIKELY(yepBuiltin_GetHighPart_64u_32u(state) == LinuxPerfEventCpuCounterStateMagic) {
				const int perfCounter = int(yepBuiltin_GetLowPart_64u_32u(state));

				const int syscallIoctlDisableResult = yepSyscall_ioctl(perfCounter, PERF_EVENT_IOC_DISABLE);
				if YEP_LIKELY(syscallIoctlDisableResult == 0) {
					Yep64u elapsedTicks;
					const ssize_t bytesRead = yepSyscall_read(perfCounter, &elapsedTicks, sizeof(elapsedTicks));
					if YEP_LIKELY(bytesRead == sizeof(elapsedTicks)) {
						const int syscallCloseResult = yepSyscall_close(perfCounter);
						if YEP_LIKELY(syscallCloseResult == 0) {
							*ticksPointer = elapsedTicks;
							return YepStatusOk;
						}
					}
				}

				yepSyscall_close(perfCounter);
				return YepStatusSystemError;
			} else {
				return YepStatusInvalidState;
			}
	#if defined(YEP_ARM_CPU)
		} else if YEP_UNLIKELY((_systemFeatures & cycleCounterMask) == cycleCounterCP15) {
			const Yep64u state = *statePointer;
			*statePointer = 0;
			if YEP_LIKELY(yepBuiltin_GetHighPart_64u_32u(state) == ArmCP15CpuCounterStateMagic) {
				const Yep32u pmccntrStart = yepBuiltin_GetLowPart_64u_32u(state);

				const Yep64u pmccntrReadResult = _yepLibrary_ReadCoprocessor(&_yepLibrary_ReadPMCCNTR);
				const Yep32u pmccntrReadSuccess = yepBuiltin_GetLowPart_64u_32u(pmccntrReadResult);
				if YEP_LIKELY(pmccntrReadSuccess) {
					const Yep32u pmccntrEnd = yepBuiltin_GetHighPart_64u_32u(pmccntrReadResult);
					const Yep32u ticks = pmccntrEnd - pmccntrStart;
					
					const Yep64u pmcrReadResult = _yepLibrary_ReadCoprocessor(&_yepLibrary_ReadPMCR);
					const Yep32u pmcrReadSuccess = yepBuiltin_GetLowPart_64u_32u(pmcrReadResult);
					if YEP_LIKELY(pmcrReadSuccess) {
						const Yep32u pmcr = yepBuiltin_GetHighPart_64u_32u(pmcrReadResult);
						// PMCR[bit 3] = divider (if set then every 64th cycle is counted)
						const Yep32u cycleCounterClockDivider = pmcr & 0x00000008u;
						if YEP_UNLIKELY(cycleCounterClockDivider != 0) {
							*ticksPointer = yepBuiltin_CombineParts_32u32u_64u(ticks >> (32 - 6), ticks << 6);
						} else {
							*ticksPointer = Yep64u(ticks);
						}
						return YepStatusOk;
					}
				}
				return YepStatusSystemError;
			} else {
				return YepStatusInvalidState;
			}
	#endif
		} else {
			*statePointer = 0;
			return YepStatusUnsupportedSoftware;
		}
	#elif defined(YEP_WINDOWS_OS)
		return YepStatusUnsupportedSoftware;
	#else
		#error "Not yet supported combination of OS and CPU"
	#endif
#else
	#error "Not yet implemented for this combination of compiler and architecture"
#endif
}

#if defined(YEP_LINUX_OS)
	#if defined(YEP_ARM_CPU)
		YepStatus _yepLibrary_DetectLinuxARMCycleCounterAccess(Yep64u &systemFeatures) {
			const Yep64u pmccntrReadResult = _yepLibrary_ReadCoprocessor(&_yepLibrary_ReadPMCCNTR);
			const Yep32u pmccntrReadSuccess = yepBuiltin_GetLowPart_64u_32u(pmccntrReadResult);
			if YEP_UNLIKELY(pmccntrReadSuccess) {
				const Yep32u pmccntrStart = yepBuiltin_GetHighPart_64u_32u(pmccntrReadResult);
				const Yep64u pmccntrReReadResult = _yepLibrary_ReadCoprocessor(&_yepLibrary_ReadPMCCNTR);
				const Yep32u pmccntrReReadSuccess = yepBuiltin_GetLowPart_64u_32u(pmccntrReReadResult);
				if YEP_LIKELY(pmccntrReReadSuccess) {
					const Yep32u pmccntrEnd = yepBuiltin_GetHighPart_64u_32u(pmccntrReReadResult);
					if YEP_LIKELY(pmccntrEnd != pmccntrStart) {
						systemFeatures |= YepSystemFeatureCycleCounter;
						return YepStatusOk;
					}
				}
			}
			return YepStatusUnsupportedSoftware;
		}
	#endif

	#if defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)
		YepStatus _yepLibrary_DetectLinuxPerfEventSupport(Yep64u &systemFeatures) {
			struct perf_event_attr perfEventAttributes;
			memset(&perfEventAttributes, 0, sizeof(perfEventAttributes));
			perfEventAttributes.size = sizeof(perfEventAttributes);
			perfEventAttributes.type = PERF_TYPE_HARDWARE;
			perfEventAttributes.config = PERF_COUNT_HW_CPU_CYCLES;
			perfEventAttributes.disabled = 1;

			const pid_t processId = 0; // Current process
			const int cpu = -1; // Any CPU
			const int groupFileDescriptor = -1; // New group
			const int flags = 0; // Default
			const int perfCounter = yepSyscall_perf_event_open(&perfEventAttributes, processId, cpu, groupFileDescriptor, flags);
			if YEP_LIKELY(perfCounter >= 0) {
				const int syscallIoctlResetResult = yepSyscall_ioctl(perfCounter, PERF_EVENT_IOC_RESET);
				if YEP_LIKELY(syscallIoctlResetResult == 0) {
					const int syscallIoctlEnableResult = yepSyscall_ioctl(perfCounter, PERF_EVENT_IOC_ENABLE);
					if YEP_LIKELY(syscallIoctlEnableResult == 0) {
						const int syscallIoctlDisableResult = yepSyscall_ioctl(perfCounter, PERF_EVENT_IOC_DISABLE);
						if YEP_LIKELY(syscallIoctlDisableResult == 0) {
							Yep64u cpuCycles = 0;
							const ssize_t bytesRead = yepSyscall_read(perfCounter, &cpuCycles, sizeof(cpuCycles));
							if YEP_LIKELY(bytesRead == sizeof(cpuCycles)) {
								// Sanity check
								if YEP_LIKELY(cpuCycles != 0) {
									const int syscallCloseResult = yepSyscall_close(perfCounter);
									if YEP_LIKELY(syscallCloseResult == 0) {
										systemFeatures |= YepSystemFeatureCycleCounter | YepSystemFeatureCycleCounter64Bit;
										return YepStatusOk;
									} else {
										// Avoid calling the close twice
										return YepStatusSystemError;
									}
								}
							}
						}
					}
				}
				yepSyscall_close(perfCounter);
			}
			return YepStatusSystemError;
		}
	#endif
#endif
