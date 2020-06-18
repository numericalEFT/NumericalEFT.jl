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

	#include <errno.h>
	#include <time.h>
	#include <string.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/syscall.h>
	#include <linux/unistd.h>
#endif

#if defined(YEP_X86_CPU) && defined(YEP_LINUX_OS)
	const Yep64u energyCounterMagic        = 0x14CFC5C1u;

	enum Msr {
		MsrNull                        = 0,
		MsrRaplPowerUnit               = 0x606,
		MsrRaplPackageEnergyStatus     = 0x611,
		MsrRaplPowerPlane0EnergyStatus = 0x639,
		MsrRaplPowerPlane1EnergyStatus = 0x641,
		MsrRaplPowerDRAMEnergyStatus   = 0x619
	};

	static Msr getRaplEnergyMsr(YepEnergyCounterType type) {
		switch (type) {
			case YepEnergyCounterTypeRaplPackageEnergy:
			case YepEnergyCounterTypeRaplPackagePower:
				switch (_microarchitecture) {
					case YepCpuMicroarchitectureSandyBridge:
					case YepCpuMicroarchitectureIvyBridge:
					case YepCpuMicroarchitectureHaswell:
						return MsrRaplPackageEnergyStatus;
					default:
						return MsrNull;
				}
			case YepEnergyCounterTypeRaplPowerPlane0Energy:
			case YepEnergyCounterTypeRaplPowerPlane0Power:
				switch (_microarchitecture) {
					case YepCpuMicroarchitectureSandyBridge:
					case YepCpuMicroarchitectureIvyBridge:
					case YepCpuMicroarchitectureHaswell:
						return MsrRaplPowerPlane0EnergyStatus;
					default:
						return MsrNull;
				}
			case YepEnergyCounterTypeRaplPowerPlane1Energy:
			case YepEnergyCounterTypeRaplPowerPlane1Power:
				if YEP_LIKELY(_vendor == YepCpuVendorIntel) {
					if YEP_LIKELY(_modelInfo.family == 0x06) {
						switch (_modelInfo.model) {
							case 0x2A: // Core iX (Sandy Bridge)
							case 0x3A: // Core iX (Ivy Bridge)
								return MsrRaplPowerPlane1EnergyStatus;
							default:
								return MsrNull;
						}
					}
				}
				return MsrNull;
			case YepEnergyCounterTypeRaplDRAMEnergy:
			case YepEnergyCounterTypeRaplDRAMPower:
				if YEP_LIKELY(_vendor == YepCpuVendorIntel) {
					if YEP_LIKELY(_modelInfo.family == 0x06) {
						switch (_modelInfo.model) {
							case 0x2D: // Core iX (Sandy Bridge-E), Xeon (Sandy Bridge EP/EX)
								return MsrRaplPowerDRAMEnergyStatus;
							default:
								return MsrNull;
						}
					}
				}
				return MsrNull;
			default:
				return MsrNull;
		}
	}

	static YepBoolean isPowerCounter(YepEnergyCounterType type) {
		switch (type) {
			case YepEnergyCounterTypeRaplPackagePower:
			case YepEnergyCounterTypeRaplPowerPlane0Power:
			case YepEnergyCounterTypeRaplPowerPlane1Power:
			case YepEnergyCounterTypeRaplDRAMPower:
				return YepBooleanTrue;
			default:
				return YepBooleanFalse;
		}
	}

	static YepSize numberToString(char* string, Yep32u number) {
		if (number == 0) {
			*string++ = '0';
			return 1;
		} else {
			char buffer[10];
			char *cur = &buffer[10];
			while (number != 0) {
				#if defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)
					const Yep32u newNumber = ((Yep64u)(number) * 3435973837u) >> 3;
					*(--cur) = '0' + (number - newNumber * 10u);
					number = newNumber;
				#else
					*(--cur) = '0' + (number % 10);
					number /= 10;
				#endif
			}
			const YepSize length = (&buffer[10] - cur);
			memcpy(string, cur, length);
			return length;
		}
	}
#endif

YepStatus YEPABI yepLibrary_GetEnergyCounterAcquire(YepEnergyCounterType type, YepEnergyCounter *energyCounter) {
	if YEP_UNLIKELY(energyCounter == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
#if defined(YEP_X86_CPU)
	#if defined(YEP_LINUX_OS)
		Msr raplMsr = getRaplEnergyMsr(type);
		if YEP_LIKELY(raplMsr != MsrNull) {
			/* Inspired by the code from http://web.eece.maine.edu/~vweaver/projects/rapl/ */
			/** @todo	Check if the module from https://github.com/razvanlupusoru/Intel-RAPL-via-Sysfs is usable */
			unsigned logicalCpu;
			const int getcpuResult = yepSyscall_getcpu(&logicalCpu, NULL, NULL);
			if YEP_LIKELY(getcpuResult == 0) {
				YepStatus status = YepStatusOk;
				YepEnergyCounter localEnergyCounter;
				localEnergyCounter.state[0] = yepBuiltin_CombineParts_32u32u_64u(type, energyCounterMagic);

				/**
				 * "/dev/<logicalCpu>/msr":
				 * - 9 chars for "/dev/cpu/"
				 * - up to 10 chars for <logicalCpu> (32-bit unsigned int)
				 * - 4 chars for /msr"
				 * - 1 char for terminating '\0'
				 */
				char msrDevicePath[24];
				memcpy(msrDevicePath, "/dev/cpu/", 9);
				char *stringEnd = msrDevicePath + 9;
				stringEnd += numberToString(stringEnd, static_cast<Yep32u>(logicalCpu));
				memcpy(stringEnd, "/msr", 5); /* Adds terminating null */
				const int msrDeviceFile = yepSyscall_open(msrDevicePath, O_RDONLY);
				if YEP_LIKELY(msrDeviceFile >= 0) {
					localEnergyCounter.state[1] = yepBuiltin_CombineParts_32u32u_64u(static_cast<Yep32u>(msrDeviceFile), static_cast<Yep32u>(raplMsr));

					Yep64u energyStatus;
					const int preadResult = yepSyscall_pread(msrDeviceFile, &energyStatus, sizeof(Yep64u), raplMsr);
					if YEP_LIKELY(preadResult == sizeof(Yep64u)) {
						const Yep32u totalEnergyConsumed = yepBuiltin_GetLowPart_64u_32u(energyStatus);
						localEnergyCounter.state[2] = yepBuiltin_CombineParts_32u32u_64u(static_cast<Yep32u>(logicalCpu), totalEnergyConsumed);
						if YEP_UNLIKELY(isPowerCounter(type)) {
							struct timespec ts;
							const int clockGetTimeResult = yepSyscall_clock_gettime(CLOCK_MONOTONIC, &ts);
							switch (clockGetTimeResult) {
								case 0: /* Success */
									localEnergyCounter.state[3] = static_cast<Yep64u>(ts.tv_sec);
									localEnergyCounter.state[4] = static_cast<Yep64u>(ts.tv_nsec);
									break;
								case -EPERM:
									status = YepStatusAccessDenied; break;
								default: /* Other errors */
									status = YepStatusSystemError; break;
							}
						} else {
							localEnergyCounter.state[3] = 0ull;
							localEnergyCounter.state[4] = 0ull;
						}
					} else {
						status = YepStatusSystemError;
					}
				} else {
					switch (msrDeviceFile) {
						case -EIO:
							status = YepStatusUnsupportedHardware; break;
						case -EACCES:
							status = YepStatusAccessDenied; break;
						default:
							status = YepStatusSystemError; break;
					}
				}
				if YEP_UNLIKELY(status != YepStatusOk) {
					yepSyscall_close(msrDeviceFile);
				} else {
					memcpy(energyCounter, &localEnergyCounter, sizeof(YepEnergyCounter));
				}
				return status;
			} else {
				return YepStatusSystemError;
			}
		} else {
			return YepStatusUnsupportedHardware;
		}
	#else
		return YepStatusUnsupportedSoftware;
	#endif
#else
	return YepStatusUnsupportedHardware;
#endif	
}

YepStatus YEPABI yepLibrary_GetEnergyCounterRelease(YepEnergyCounter *energyCounter, Yep64f* measurement) {
	if YEP_UNLIKELY(energyCounter == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(measurement == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(measurement, sizeof(Yep64f)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_X86_CPU)
	#if defined(YEP_LINUX_OS)
		if YEP_LIKELY(yepBuiltin_GetLowPart_64u_32u(energyCounter->state[0]) == energyCounterMagic) {
			YepStatus status = YepStatusOk;
			Yep64f localMeasurement;

			const YepEnergyCounterType type = static_cast<YepEnergyCounterType>(yepBuiltin_GetHighPart_64u_32u(energyCounter->state[0]));
			const Msr msr = static_cast<Msr>(yepBuiltin_GetLowPart_64u_32u(energyCounter->state[1]));
			const int msrDeviceFile = static_cast<int>(yepBuiltin_GetHighPart_64u_32u(energyCounter->state[1]));

			Yep64u energyStatus;
			const int preadEnergyStatusResult = yepSyscall_pread(msrDeviceFile, &energyStatus, sizeof(Yep64u), msr);
			if YEP_LIKELY(preadEnergyStatusResult == sizeof(Yep64u)) {
				const Yep32u energyConsumedEnd = yepBuiltin_GetLowPart_64u_32u(energyStatus);
				const Yep32u energyConsumedStart = yepBuiltin_GetLowPart_64u_32u(energyCounter->state[2]);
				localMeasurement = yepBuiltin_Convert_32u_64f(energyConsumedEnd - energyConsumedStart);
				if YEP_UNLIKELY(isPowerCounter(type)) {
					struct timespec ts;
					const int clockGetTimeResult = yepSyscall_clock_gettime(CLOCK_MONOTONIC, &ts);
					switch (clockGetTimeResult) {
						case 0: /* Success */
						{
							const Yep64f startSec = yepBuiltin_Convert_64u_64f(energyCounter->state[3]);
							const Yep64f startNSec = yepBuiltin_Convert_64s_64f(energyCounter->state[4]);
							const Yep64f endSec = yepBuiltin_Convert_64u_64f(ts.tv_sec);
							const Yep64f endNSec = yepBuiltin_Convert_64s_64f(ts.tv_nsec);
							const Yep64f startTime = startSec + startNSec * 1.0e-9;
							const Yep64f endTime = endSec + endNSec * 1.0e-9;
							const Yep64f time = endTime - startTime;
							localMeasurement /= time;
							break;
						}
						case -EPERM:
							status = YepStatusAccessDenied; break;
						default: /* Other errors */
							status = YepStatusSystemError; break;
					}
				}
			} else {
				status = YepStatusSystemError;
			}
			if YEP_LIKELY(status == YepStatusOk) {
				Yep64u raplUnits;
				const int preadPowerUnitResult = yepSyscall_pread(msrDeviceFile, &raplUnits, sizeof(Yep64u), MsrRaplPowerUnit);
				if YEP_LIKELY(preadPowerUnitResult == sizeof(Yep64u)) {
					/* MSR_RAPL_POWER_UNIT[bits 8-12] = energy status units */
					const Yep32u energyStatusUnits = (Yep32u(raplUnits) >> 8) & 0x1Fu;
					const Yep32u exponent = 0x3FFu - energyStatusUnits;
					const Yep64f joulesPerUnit = yepBuiltin_Cast_64u_64f(yepBuiltin_CombineParts_32u32u_64u(exponent << 20, 0x00000000u));
					localMeasurement *= joulesPerUnit;
				} else {
					status = YepStatusSystemError;
				}
			}
			const int closeResult = yepSyscall_close(msrDeviceFile);
			if YEP_UNLIKELY(closeResult < 0) {
				if YEP_LIKELY(status == YepStatusOk) {
					status = YepStatusSystemError;
				}
			}
			if YEP_LIKELY(status == YepStatusOk) {
				*measurement = localMeasurement;
			}
			memset(energyCounter, 0, sizeof(YepEnergyCounter));
			return status;
		} else {
			memset(energyCounter, 0, sizeof(YepEnergyCounter));
			return YepStatusInvalidState;
		}
	#else
		return YepStatusUnsupportedSoftware;
	#endif
#else
	return YepStatusUnsupportedHardware;
#endif	
}
