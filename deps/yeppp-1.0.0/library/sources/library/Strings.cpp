/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <library/functions.h>
#include <string.h>

template <Yep64u n> struct CTZ {
	enum {
		result = CTZ<(n >> 1)>::result + 1
	};
};

template <> struct CTZ<1ull> {
	enum {
		result = 0
	};
};

#define YEP_RETURN_CONSTANT_STRING(text) \
	return ConstantString(text, YEP_COUNT_OF(text) - 1);

static ConstantString getStatusDescription(YepStatus status) {
	switch (status) {
		case YepStatusOk:
			YEP_RETURN_CONSTANT_STRING("Success");
		case YepStatusNullPointer:
			YEP_RETURN_CONSTANT_STRING("Null pointer");
		case YepStatusMisalignedPointer:
			YEP_RETURN_CONSTANT_STRING("Misaligned pointer");
		case YepStatusInvalidArgument:
			YEP_RETURN_CONSTANT_STRING("Invalid argument");
		case YepStatusInvalidData:
			YEP_RETURN_CONSTANT_STRING("Invalid data");
		case YepStatusInvalidState:
			YEP_RETURN_CONSTANT_STRING("Invalid state");
		case YepStatusUnsupportedHardware:
			YEP_RETURN_CONSTANT_STRING("Unsupported hardware");
		case YepStatusUnsupportedSoftware:
			YEP_RETURN_CONSTANT_STRING("Unsupported software");
		case YepStatusInsufficientBuffer:
			YEP_RETURN_CONSTANT_STRING("Insufficient buffer");
		case YepStatusOutOfMemory:
			YEP_RETURN_CONSTANT_STRING("Not enough memory");
		case YepStatusSystemError:
			YEP_RETURN_CONSTANT_STRING("System error");
		default:
			return ConstantString();
	}
}

static ConstantString getStatusID(YepStatus status) {
	switch (status) {
		case YepStatusOk:
			YEP_RETURN_CONSTANT_STRING("Success");
		case YepStatusNullPointer:
			YEP_RETURN_CONSTANT_STRING("NullPointer");
		case YepStatusMisalignedPointer:
			YEP_RETURN_CONSTANT_STRING("MisalignedPointer");
		case YepStatusInvalidArgument:
			YEP_RETURN_CONSTANT_STRING("InvalidArgument");
		case YepStatusInvalidData:
			YEP_RETURN_CONSTANT_STRING("InvalidData");
		case YepStatusInvalidState:
			YEP_RETURN_CONSTANT_STRING("InvalidState");
		case YepStatusUnsupportedHardware:
			YEP_RETURN_CONSTANT_STRING("UnsupportedHardware");
		case YepStatusUnsupportedSoftware:
			YEP_RETURN_CONSTANT_STRING("UnsupportedSoftware");
		case YepStatusInsufficientBuffer:
			YEP_RETURN_CONSTANT_STRING("InsufficientBuffer");
		case YepStatusOutOfMemory:
			YEP_RETURN_CONSTANT_STRING("OutOfMemory");
		case YepStatusSystemError:
			YEP_RETURN_CONSTANT_STRING("SystemError");
		default:
			return ConstantString();
	}
}

static ConstantString getCpuArchitectureDescription(YepCpuArchitecture architecture) {
	switch (architecture) {
		case YepCpuArchitectureUnknown:
			YEP_RETURN_CONSTANT_STRING("Unknown");
		case YepCpuArchitectureX86:
			YEP_RETURN_CONSTANT_STRING("x86");
		case YepCpuArchitectureARM:
			YEP_RETURN_CONSTANT_STRING("ARM");
		case YepCpuArchitectureMIPS:
			YEP_RETURN_CONSTANT_STRING("MIPS");
		case YepCpuArchitecturePowerPC:
			YEP_RETURN_CONSTANT_STRING("PowerPC");
		case YepCpuArchitectureIA64:
			YEP_RETURN_CONSTANT_STRING("IA64");
		case YepCpuArchitectureSPARC:
			YEP_RETURN_CONSTANT_STRING("SPARC");
		default:
			return ConstantString();
	}
}

static ConstantString getCpuArchitectureID(YepCpuArchitecture architecture) {
	switch (architecture) {
		case YepCpuArchitectureUnknown:
			YEP_RETURN_CONSTANT_STRING("Unknown");
		case YepCpuArchitectureX86:
			YEP_RETURN_CONSTANT_STRING("x86");
		case YepCpuArchitectureARM:
			YEP_RETURN_CONSTANT_STRING("ARM");
		case YepCpuArchitectureMIPS:
			YEP_RETURN_CONSTANT_STRING("MIPS");
		case YepCpuArchitecturePowerPC:
			YEP_RETURN_CONSTANT_STRING("PowerPC");
		case YepCpuArchitectureIA64:
			YEP_RETURN_CONSTANT_STRING("IA64");
		case YepCpuArchitectureSPARC:
			YEP_RETURN_CONSTANT_STRING("SPARC");
		default:
			return ConstantString();
	}
}

ConstantString _yepLibrary_GetCpuVendorDescription(YepCpuVendor vendor) {
	switch (vendor) {
		case YepCpuVendorUnknown:
			YEP_RETURN_CONSTANT_STRING("Unknown");
		case YepCpuVendorIntel:
			YEP_RETURN_CONSTANT_STRING("Intel");
		case YepCpuVendorAMD:
			YEP_RETURN_CONSTANT_STRING("AMD");
		case YepCpuVendorVIA:
			YEP_RETURN_CONSTANT_STRING("VIA");
		case YepCpuVendorTransmeta:
			YEP_RETURN_CONSTANT_STRING("Transmeta");
		case YepCpuVendorCyrix:
			YEP_RETURN_CONSTANT_STRING("Cyrix");
		case YepCpuVendorRise:
			YEP_RETURN_CONSTANT_STRING("Rise");
		case YepCpuVendorNSC:
			YEP_RETURN_CONSTANT_STRING("NSC");
		case YepCpuVendorSiS:
			YEP_RETURN_CONSTANT_STRING("SiS");
		case YepCpuVendorNexGen:
			YEP_RETURN_CONSTANT_STRING("NexGen");
		case YepCpuVendorUMC:
			YEP_RETURN_CONSTANT_STRING("UMC");
		case YepCpuVendorRDC:
			YEP_RETURN_CONSTANT_STRING("RDC");
		case YepCpuVendorDMP:
			YEP_RETURN_CONSTANT_STRING("DM&P");
		case YepCpuVendorARM:
			YEP_RETURN_CONSTANT_STRING("ARM");
		case YepCpuVendorMarvell:
			YEP_RETURN_CONSTANT_STRING("Marvell");
		case YepCpuVendorQualcomm:
			YEP_RETURN_CONSTANT_STRING("Qualcomm");
		case YepCpuVendorDEC:
			YEP_RETURN_CONSTANT_STRING("DEC");
		case YepCpuVendorTI:
			YEP_RETURN_CONSTANT_STRING("TI");
		case YepCpuVendorApple:
			YEP_RETURN_CONSTANT_STRING("Apple");
		case YepCpuVendorIngenic:
			YEP_RETURN_CONSTANT_STRING("Ingenic");
		case YepCpuVendorICT:
			YEP_RETURN_CONSTANT_STRING("ICT");
		case YepCpuVendorMIPS:
			YEP_RETURN_CONSTANT_STRING("MIPS");
		case YepCpuVendorIBM:
			YEP_RETURN_CONSTANT_STRING("IBM");
		case YepCpuVendorMotorola:
			YEP_RETURN_CONSTANT_STRING("Motorola");
		case YepCpuVendorPASemi:
			YEP_RETURN_CONSTANT_STRING("P.A.Semi");
		case YepCpuVendorSun:
			YEP_RETURN_CONSTANT_STRING("Sun");
		case YepCpuVendorOracle:
			YEP_RETURN_CONSTANT_STRING("Oracle");
		case YepCpuVendorFujitsu:
			YEP_RETURN_CONSTANT_STRING("Fujitsu");
		case YepCpuVendorMCST:
			YEP_RETURN_CONSTANT_STRING("MCST");
		default:
			return ConstantString();
	}
}

static ConstantString _yepLibrary_GetCpuVendorID(YepCpuVendor vendor) {
	switch (vendor) {
		case YepCpuVendorUnknown:
			YEP_RETURN_CONSTANT_STRING("Unknown");
		case YepCpuVendorIntel:
			YEP_RETURN_CONSTANT_STRING("Intel");
		case YepCpuVendorAMD:
			YEP_RETURN_CONSTANT_STRING("AMD");
		case YepCpuVendorVIA:
			YEP_RETURN_CONSTANT_STRING("VIA");
		case YepCpuVendorTransmeta:
			YEP_RETURN_CONSTANT_STRING("Transmeta");
		case YepCpuVendorCyrix:
			YEP_RETURN_CONSTANT_STRING("Cyrix");
		case YepCpuVendorRise:
			YEP_RETURN_CONSTANT_STRING("Rise");
		case YepCpuVendorNSC:
			YEP_RETURN_CONSTANT_STRING("NSC");
		case YepCpuVendorSiS:
			YEP_RETURN_CONSTANT_STRING("SiS");
		case YepCpuVendorNexGen:
			YEP_RETURN_CONSTANT_STRING("NexGen");
		case YepCpuVendorUMC:
			YEP_RETURN_CONSTANT_STRING("UMC");
		case YepCpuVendorRDC:
			YEP_RETURN_CONSTANT_STRING("RDC");
		case YepCpuVendorDMP:
			YEP_RETURN_CONSTANT_STRING("DMP");
		case YepCpuVendorARM:
			YEP_RETURN_CONSTANT_STRING("ARM");
		case YepCpuVendorMarvell:
			YEP_RETURN_CONSTANT_STRING("Marvell");
		case YepCpuVendorQualcomm:
			YEP_RETURN_CONSTANT_STRING("Qualcomm");
		case YepCpuVendorDEC:
			YEP_RETURN_CONSTANT_STRING("DEC");
		case YepCpuVendorTI:
			YEP_RETURN_CONSTANT_STRING("TI");
		case YepCpuVendorApple:
			YEP_RETURN_CONSTANT_STRING("Apple");
		case YepCpuVendorIngenic:
			YEP_RETURN_CONSTANT_STRING("Ingenic");
		case YepCpuVendorICT:
			YEP_RETURN_CONSTANT_STRING("ICT");
		case YepCpuVendorMIPS:
			YEP_RETURN_CONSTANT_STRING("MIPS");
		case YepCpuVendorIBM:
			YEP_RETURN_CONSTANT_STRING("IBM");
		case YepCpuVendorMotorola:
			YEP_RETURN_CONSTANT_STRING("Motorola");
		case YepCpuVendorPASemi:
			YEP_RETURN_CONSTANT_STRING("PASemi");
		case YepCpuVendorSun:
			YEP_RETURN_CONSTANT_STRING("Sun");
		case YepCpuVendorOracle:
			YEP_RETURN_CONSTANT_STRING("Oracle");
		case YepCpuVendorFujitsu:
			YEP_RETURN_CONSTANT_STRING("Fujitsu");
		case YepCpuVendorMCST:
			YEP_RETURN_CONSTANT_STRING("MCST");
		default:
			return ConstantString();
	}
}

ConstantString _yepLibrary_GetCpuMicroarchitectureDescription(YepCpuMicroarchitecture microarchitecture) {
	switch (microarchitecture) {
		case YepCpuMicroarchitectureUnknown:
			YEP_RETURN_CONSTANT_STRING("Unknown");
		case YepCpuMicroarchitectureP5:
			YEP_RETURN_CONSTANT_STRING("P5");
		case YepCpuMicroarchitectureP6:
			YEP_RETURN_CONSTANT_STRING("P6");
		case YepCpuMicroarchitectureWillamette:
			YEP_RETURN_CONSTANT_STRING("Willamette");
		case YepCpuMicroarchitecturePrescott:
			YEP_RETURN_CONSTANT_STRING("Prescott");
		case YepCpuMicroarchitectureDothan:
			YEP_RETURN_CONSTANT_STRING("Dothan");
		case YepCpuMicroarchitectureYonah:
			YEP_RETURN_CONSTANT_STRING("Yonah");
		case YepCpuMicroarchitectureConroe:
			YEP_RETURN_CONSTANT_STRING("Conroe");
		case YepCpuMicroarchitecturePenryn:
			YEP_RETURN_CONSTANT_STRING("Penryn");
		case YepCpuMicroarchitectureBonnell:
			YEP_RETURN_CONSTANT_STRING("Bonnell");
		case YepCpuMicroarchitectureNehalem:
			YEP_RETURN_CONSTANT_STRING("Nehalem");
		case YepCpuMicroarchitectureSandyBridge:
			YEP_RETURN_CONSTANT_STRING("Sandy Bridge");
		case YepCpuMicroarchitectureSaltwell:
			YEP_RETURN_CONSTANT_STRING("Saltwell");
		case YepCpuMicroarchitectureIvyBridge:
			YEP_RETURN_CONSTANT_STRING("Ivy Bridge");
		case YepCpuMicroarchitectureHaswell:
			YEP_RETURN_CONSTANT_STRING("Haswell");
		case YepCpuMicroarchitectureSilvermont:
			YEP_RETURN_CONSTANT_STRING("Silvermont");
		case YepCpuMicroarchitectureKnightsFerry:
			YEP_RETURN_CONSTANT_STRING("Knights Ferry");
		case YepCpuMicroarchitectureKnightsCorner:
			YEP_RETURN_CONSTANT_STRING("Knights Corner");
		case YepCpuMicroarchitectureK5:
			YEP_RETURN_CONSTANT_STRING("K5");
		case YepCpuMicroarchitectureK6:
			YEP_RETURN_CONSTANT_STRING("K6");
		case YepCpuMicroarchitectureK7:
			YEP_RETURN_CONSTANT_STRING("K7");
		case YepCpuMicroarchitectureGeode:
			YEP_RETURN_CONSTANT_STRING("Geode");
		case YepCpuMicroarchitectureK8:
			YEP_RETURN_CONSTANT_STRING("K8");
		case YepCpuMicroarchitectureK10:
			YEP_RETURN_CONSTANT_STRING("K10");
		case YepCpuMicroarchitectureBobcat:
			YEP_RETURN_CONSTANT_STRING("Bobcat");
		case YepCpuMicroarchitectureBulldozer:
			YEP_RETURN_CONSTANT_STRING("Bulldozer");
		case YepCpuMicroarchitecturePiledriver:
			YEP_RETURN_CONSTANT_STRING("Piledriver");
		case YepCpuMicroarchitectureJaguar:
			YEP_RETURN_CONSTANT_STRING("Jaguar");
		case YepCpuMicroarchitectureSteamroller:
			YEP_RETURN_CONSTANT_STRING("Steamroller");
		case YepCpuMicroarchitectureStrongARM:
			YEP_RETURN_CONSTANT_STRING("StrongARM");
		case YepCpuMicroarchitectureXScale:
			YEP_RETURN_CONSTANT_STRING("XScale");
		case YepCpuMicroarchitectureARM7:
			YEP_RETURN_CONSTANT_STRING("ARM7");
		case YepCpuMicroarchitectureARM9:
			YEP_RETURN_CONSTANT_STRING("ARM9");
		case YepCpuMicroarchitectureARM11:
			YEP_RETURN_CONSTANT_STRING("ARM11");
		case YepCpuMicroarchitectureCortexA5:
			YEP_RETURN_CONSTANT_STRING("Cortex-A5");
		case YepCpuMicroarchitectureCortexA7:
			YEP_RETURN_CONSTANT_STRING("Cortex-A7");
		case YepCpuMicroarchitectureCortexA8:
			YEP_RETURN_CONSTANT_STRING("Cortex-A8");
		case YepCpuMicroarchitectureCortexA9:
			YEP_RETURN_CONSTANT_STRING("Cortex-A9");
		case YepCpuMicroarchitectureCortexA15:
			YEP_RETURN_CONSTANT_STRING("Cortex-A15");
		case YepCpuMicroarchitectureScorpion:
			YEP_RETURN_CONSTANT_STRING("Scorpion");
		case YepCpuMicroarchitectureKrait:
			YEP_RETURN_CONSTANT_STRING("Krait");
		case YepCpuMicroarchitecturePJ1:
			YEP_RETURN_CONSTANT_STRING("PJ1");
		case YepCpuMicroarchitecturePJ4:
			YEP_RETURN_CONSTANT_STRING("PJ4");
		case YepCpuMicroarchitectureSwift:
			YEP_RETURN_CONSTANT_STRING("Swift");
		case YepCpuMicroarchitectureItanium:
			YEP_RETURN_CONSTANT_STRING("Itanium");
		case YepCpuMicroarchitectureItanium2:
			YEP_RETURN_CONSTANT_STRING("Itanium 2");
		case YepCpuMicroarchitectureMIPS24K:
			YEP_RETURN_CONSTANT_STRING("MIPS 24K");
		case YepCpuMicroarchitectureMIPS34K:
			YEP_RETURN_CONSTANT_STRING("MIPS 34K");
		case YepCpuMicroarchitectureMIPS74K:
			YEP_RETURN_CONSTANT_STRING("MIPS 74K");
		case YepCpuMicroarchitectureXBurst:
			YEP_RETURN_CONSTANT_STRING("XBurst");
		case YepCpuMicroarchitectureXBurst2:
			YEP_RETURN_CONSTANT_STRING("XBurst 2");
		default:
			return ConstantString();
	}
}

static ConstantString _yepLibrary_GetCpuMicroarchitectureID(YepCpuMicroarchitecture microarchitecture) {
	switch (microarchitecture) {
		case YepCpuMicroarchitectureUnknown:
			YEP_RETURN_CONSTANT_STRING("Unknown");
		case YepCpuMicroarchitectureP5:
			YEP_RETURN_CONSTANT_STRING("P5");
		case YepCpuMicroarchitectureP6:
			YEP_RETURN_CONSTANT_STRING("P6");
		case YepCpuMicroarchitectureWillamette:
			YEP_RETURN_CONSTANT_STRING("Willamette");
		case YepCpuMicroarchitecturePrescott:
			YEP_RETURN_CONSTANT_STRING("Prescott");
		case YepCpuMicroarchitectureDothan:
			YEP_RETURN_CONSTANT_STRING("Dothan");
		case YepCpuMicroarchitectureYonah:
			YEP_RETURN_CONSTANT_STRING("Yonah");
		case YepCpuMicroarchitectureConroe:
			YEP_RETURN_CONSTANT_STRING("Conroe");
		case YepCpuMicroarchitecturePenryn:
			YEP_RETURN_CONSTANT_STRING("Penryn");
		case YepCpuMicroarchitectureBonnell:
			YEP_RETURN_CONSTANT_STRING("Bonnell");
		case YepCpuMicroarchitectureNehalem:
			YEP_RETURN_CONSTANT_STRING("Nehalem");
		case YepCpuMicroarchitectureSandyBridge:
			YEP_RETURN_CONSTANT_STRING("SandyBridge");
		case YepCpuMicroarchitectureSaltwell:
			YEP_RETURN_CONSTANT_STRING("Saltwell");
		case YepCpuMicroarchitectureIvyBridge:
			YEP_RETURN_CONSTANT_STRING("IvyBridge");
		case YepCpuMicroarchitectureHaswell:
			YEP_RETURN_CONSTANT_STRING("Haswell");
		case YepCpuMicroarchitectureSilvermont:
			YEP_RETURN_CONSTANT_STRING("Silvermont");
		case YepCpuMicroarchitectureKnightsFerry:
			YEP_RETURN_CONSTANT_STRING("KnightsFerry");
		case YepCpuMicroarchitectureKnightsCorner:
			YEP_RETURN_CONSTANT_STRING("KnightsCorner");
		case YepCpuMicroarchitectureK5:
			YEP_RETURN_CONSTANT_STRING("K5");
		case YepCpuMicroarchitectureK6:
			YEP_RETURN_CONSTANT_STRING("K6");
		case YepCpuMicroarchitectureK7:
			YEP_RETURN_CONSTANT_STRING("K7");
		case YepCpuMicroarchitectureGeode:
			YEP_RETURN_CONSTANT_STRING("Geode");
		case YepCpuMicroarchitectureK8:
			YEP_RETURN_CONSTANT_STRING("K8");
		case YepCpuMicroarchitectureK10:
			YEP_RETURN_CONSTANT_STRING("K10");
		case YepCpuMicroarchitectureBobcat:
			YEP_RETURN_CONSTANT_STRING("Bobcat");
		case YepCpuMicroarchitectureBulldozer:
			YEP_RETURN_CONSTANT_STRING("Bulldozer");
		case YepCpuMicroarchitecturePiledriver:
			YEP_RETURN_CONSTANT_STRING("Piledriver");
		case YepCpuMicroarchitectureJaguar:
			YEP_RETURN_CONSTANT_STRING("Jaguar");
		case YepCpuMicroarchitectureSteamroller:
			YEP_RETURN_CONSTANT_STRING("Steamroller");
		case YepCpuMicroarchitectureStrongARM:
			YEP_RETURN_CONSTANT_STRING("StrongARM");
		case YepCpuMicroarchitectureXScale:
			YEP_RETURN_CONSTANT_STRING("XScale");
		case YepCpuMicroarchitectureARM7:
			YEP_RETURN_CONSTANT_STRING("ARM7");
		case YepCpuMicroarchitectureARM9:
			YEP_RETURN_CONSTANT_STRING("ARM9");
		case YepCpuMicroarchitectureARM11:
			YEP_RETURN_CONSTANT_STRING("ARM11");
		case YepCpuMicroarchitectureCortexA5:
			YEP_RETURN_CONSTANT_STRING("CortexA5");
		case YepCpuMicroarchitectureCortexA7:
			YEP_RETURN_CONSTANT_STRING("CortexA7");
		case YepCpuMicroarchitectureCortexA8:
			YEP_RETURN_CONSTANT_STRING("CortexA8");
		case YepCpuMicroarchitectureCortexA9:
			YEP_RETURN_CONSTANT_STRING("CortexA9");
		case YepCpuMicroarchitectureCortexA15:
			YEP_RETURN_CONSTANT_STRING("CortexA15");
		case YepCpuMicroarchitectureScorpion:
			YEP_RETURN_CONSTANT_STRING("Scorpion");
		case YepCpuMicroarchitectureKrait:
			YEP_RETURN_CONSTANT_STRING("Krait");
		case YepCpuMicroarchitecturePJ1:
			YEP_RETURN_CONSTANT_STRING("PJ1");
		case YepCpuMicroarchitecturePJ4:
			YEP_RETURN_CONSTANT_STRING("PJ4");
		case YepCpuMicroarchitectureSwift:
			YEP_RETURN_CONSTANT_STRING("Swift");
		case YepCpuMicroarchitectureItanium:
			YEP_RETURN_CONSTANT_STRING("Itanium");
		case YepCpuMicroarchitectureItanium2:
			YEP_RETURN_CONSTANT_STRING("Itanium2");
		case YepCpuMicroarchitectureMIPS24K:
			YEP_RETURN_CONSTANT_STRING("MIPS24K");
		case YepCpuMicroarchitectureMIPS34K:
			YEP_RETURN_CONSTANT_STRING("MIPS34K");
		case YepCpuMicroarchitectureMIPS74K:
			YEP_RETURN_CONSTANT_STRING("MIPS74K");
		case YepCpuMicroarchitectureXBurst:
			YEP_RETURN_CONSTANT_STRING("XBurst");
		case YepCpuMicroarchitectureXBurst2:
			YEP_RETURN_CONSTANT_STRING("XBurst2");
		default:
			return ConstantString();
	}
}

static ConstantString getGenericIsaFeatureDescription(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		default:
			return ConstantString();
	}
};

static ConstantString getGenericIsaFeatureID(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		default:
			return ConstantString();
	}
};

static ConstantString getGenericSimdFeatureDescription(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		default:
			return ConstantString();
	}
};

static ConstantString getGenericSimdFeatureID(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		default:
			return ConstantString();
	}
};

static ConstantString getGenericSystemFeatureDescription(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		case CTZ<YepSystemFeatureCycleCounter>::result:
			YEP_RETURN_CONSTANT_STRING("CPU cycle counter");
		case CTZ<YepSystemFeatureCycleCounter64Bit>::result:
			YEP_RETURN_CONSTANT_STRING("64-bit CPU cycle counter");
		case CTZ<YepSystemFeatureAddressSpace64Bit>::result:
			YEP_RETURN_CONSTANT_STRING("64-bit address space");
		case CTZ<YepSystemFeatureGPRegisters64Bit>::result:
			YEP_RETURN_CONSTANT_STRING("64-bit general-purpose registers");
		case CTZ<YepSystemFeatureMisalignedAccess>::result:
			YEP_RETURN_CONSTANT_STRING("Misaligned memory access");
		case CTZ<YepSystemFeatureSingleThreaded>::result:
			YEP_RETURN_CONSTANT_STRING("Single hardware thread");
		default:
			return ConstantString();
	}
};

static ConstantString getGenericSystemFeatureID(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		case CTZ<YepSystemFeatureCycleCounter>::result:
			YEP_RETURN_CONSTANT_STRING("CycleCounter");
		case CTZ<YepSystemFeatureCycleCounter64Bit>::result:
			YEP_RETURN_CONSTANT_STRING("CycleCounter64Bit");
		case CTZ<YepSystemFeatureAddressSpace64Bit>::result:
			YEP_RETURN_CONSTANT_STRING("AddressSpace64Bit");
		case CTZ<YepSystemFeatureGPRegisters64Bit>::result:
			YEP_RETURN_CONSTANT_STRING("GPRegisters64Bit");
		case CTZ<YepSystemFeatureMisalignedAccess>::result:
			YEP_RETURN_CONSTANT_STRING("MisalignedAccess");
		case CTZ<YepSystemFeatureSingleThreaded>::result:
			YEP_RETURN_CONSTANT_STRING("SingleThreaded");
		default:
			return ConstantString();
	}
};

static ConstantString getX86IsaFeatureDescription(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		case CTZ<YepX86IsaFeatureFPU>::result:
			YEP_RETURN_CONSTANT_STRING("x87 FPU");
		case CTZ<YepX86IsaFeatureCpuid>::result:
			YEP_RETURN_CONSTANT_STRING("CPUID instruction");
		case CTZ<YepX86IsaFeatureRdtsc>::result:
			YEP_RETURN_CONSTANT_STRING("RDTSC instruction");
		case CTZ<YepX86IsaFeatureCMOV>::result:
			YEP_RETURN_CONSTANT_STRING("CMOV instruction");
		case CTZ<YepX86IsaFeatureSYSENTER>::result:
			YEP_RETURN_CONSTANT_STRING("SYSENTER and SYSEXIT instructions");
		case CTZ<YepX86IsaFeatureSYSCALL>::result:
			YEP_RETURN_CONSTANT_STRING("SYSCALL and SYSRET instructions");
		case CTZ<YepX86IsaFeatureMSR>::result:
			YEP_RETURN_CONSTANT_STRING("RDMSR and WRMSR instructions");
		case CTZ<YepX86IsaFeatureClflush>::result:
			YEP_RETURN_CONSTANT_STRING("CLFLUSH instruction");
		case CTZ<YepX86IsaFeatureMONITOR>::result:
			YEP_RETURN_CONSTANT_STRING("MONITOR and MWAIT instructions");
		case CTZ<YepX86IsaFeatureFXSAVE>::result:
			YEP_RETURN_CONSTANT_STRING("FXSAVE and FXRSTOR instructions");
		case CTZ<YepX86IsaFeatureXSAVE>::result:
			YEP_RETURN_CONSTANT_STRING("XSAVE, XRSTOR, XGETBV, and XSETBV instructions");
		case CTZ<YepX86IsaFeatureCmpxchg8b>::result:
			YEP_RETURN_CONSTANT_STRING("CMPXCHG8B instruction");
		case CTZ<YepX86IsaFeatureCmpxchg16b>::result:
			YEP_RETURN_CONSTANT_STRING("CMPXCHG16B instruction");
		case CTZ<YepX86IsaFeatureX64>::result:
			YEP_RETURN_CONSTANT_STRING("x86-64 mode");
		case CTZ<YepX86IsaFeatureLahfSahf64>::result:
			YEP_RETURN_CONSTANT_STRING("LAHF and SAHF instructions in x86-64 mode");
		case CTZ<YepX86IsaFeatureFsGsBase>::result:
			YEP_RETURN_CONSTANT_STRING("RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE instructions");
		case CTZ<YepX86IsaFeatureMovbe>::result:
			YEP_RETURN_CONSTANT_STRING("MOVBE instruction");
		case CTZ<YepX86IsaFeaturePopcnt>::result:
			YEP_RETURN_CONSTANT_STRING("POPCNT instruction");
		case CTZ<YepX86IsaFeatureLzcnt>::result:
			YEP_RETURN_CONSTANT_STRING("LZCNT instruction");
		case CTZ<YepX86IsaFeatureBMI>::result:
			YEP_RETURN_CONSTANT_STRING("BMI instruction set");
		case CTZ<YepX86IsaFeatureBMI2>::result:
			YEP_RETURN_CONSTANT_STRING("BMI 2 instruction set");
		case CTZ<YepX86IsaFeatureTBM>::result:
			YEP_RETURN_CONSTANT_STRING("TBM instruction set");
		case CTZ<YepX86IsaFeatureRdrand>::result:
			YEP_RETURN_CONSTANT_STRING("RDRAND instruction");
		case CTZ<YepX86IsaFeatureACE>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Advanced Cryptography Engine");
		case CTZ<YepX86IsaFeatureACE2>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Advanced Cryptography Engine 2");
		case CTZ<YepX86IsaFeatureRNG>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Random Number Generator");
		case CTZ<YepX86IsaFeaturePHE>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Hash Engine");
		case CTZ<YepX86IsaFeaturePMM>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Montgomery Multiplier");
		case CTZ<YepX86IsaFeatureAES>::result:
			YEP_RETURN_CONSTANT_STRING("AES instruction set");
		case CTZ<YepX86IsaFeaturePclmulqdq>::result:
			YEP_RETURN_CONSTANT_STRING("PCLMULQDQ instruction");
		case CTZ<YepX86IsaFeatureRdtscp>::result:
			YEP_RETURN_CONSTANT_STRING("RDTSCP instruction");
		case CTZ<YepX86IsaFeatureLWP>::result:
			YEP_RETURN_CONSTANT_STRING("Lightweight Profiling extension");
		case CTZ<YepX86IsaFeatureHLE>::result:
			YEP_RETURN_CONSTANT_STRING("Hardware Lock Elision extension");
		case CTZ<YepX86IsaFeatureRTM>::result:
			YEP_RETURN_CONSTANT_STRING("Restricted Transactional Memory extension");
		case CTZ<YepX86IsaFeatureXtest>::result:
			YEP_RETURN_CONSTANT_STRING("XTEST instruction");
		case CTZ<YepX86IsaFeatureRdseed>::result:
			YEP_RETURN_CONSTANT_STRING("RDSEED instruction");
		case CTZ<YepX86IsaFeatureADX>::result:
			YEP_RETURN_CONSTANT_STRING("ADCX and ADOX instructions");
		case CTZ<YepX86IsaFeatureSHA>::result:
			YEP_RETURN_CONSTANT_STRING("SHA instructions");
		case CTZ<YepX86IsaFeatureMPX>::result:
			YEP_RETURN_CONSTANT_STRING("Memory Protection extension");
		default:
			return getGenericIsaFeatureDescription(ctzIsaFeature);
	}
};

static ConstantString getX86IsaFeatureID(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		case CTZ<YepX86IsaFeatureFPU>::result:
			YEP_RETURN_CONSTANT_STRING("FPU");
		case CTZ<YepX86IsaFeatureCpuid>::result:
			YEP_RETURN_CONSTANT_STRING("Cpuid");
		case CTZ<YepX86IsaFeatureRdtsc>::result:
			YEP_RETURN_CONSTANT_STRING("Rdtsc");
		case CTZ<YepX86IsaFeatureCMOV>::result:
			YEP_RETURN_CONSTANT_STRING("CMOV");
		case CTZ<YepX86IsaFeatureSYSENTER>::result:
			YEP_RETURN_CONSTANT_STRING("SYSENTER");
		case CTZ<YepX86IsaFeatureSYSCALL>::result:
			YEP_RETURN_CONSTANT_STRING("SYSCALL");
		case CTZ<YepX86IsaFeatureMSR>::result:
			YEP_RETURN_CONSTANT_STRING("MSR");
		case CTZ<YepX86IsaFeatureClflush>::result:
			YEP_RETURN_CONSTANT_STRING("Clflush");
		case CTZ<YepX86IsaFeatureMONITOR>::result:
			YEP_RETURN_CONSTANT_STRING("MONITOR");
		case CTZ<YepX86IsaFeatureFXSAVE>::result:
			YEP_RETURN_CONSTANT_STRING("FXSAVE");
		case CTZ<YepX86IsaFeatureXSAVE>::result:
			YEP_RETURN_CONSTANT_STRING("XSAVE");
		case CTZ<YepX86IsaFeatureCmpxchg8b>::result:
			YEP_RETURN_CONSTANT_STRING("Cmpxchg8b");
		case CTZ<YepX86IsaFeatureCmpxchg16b>::result:
			YEP_RETURN_CONSTANT_STRING("Cmpxchg16b");
		case CTZ<YepX86IsaFeatureX64>::result:
			YEP_RETURN_CONSTANT_STRING("X64");
		case CTZ<YepX86IsaFeatureLahfSahf64>::result:
			YEP_RETURN_CONSTANT_STRING("LahfSahf64");
		case CTZ<YepX86IsaFeatureFsGsBase>::result:
			YEP_RETURN_CONSTANT_STRING("FsGsBase");
		case CTZ<YepX86IsaFeatureMovbe>::result:
			YEP_RETURN_CONSTANT_STRING("Movbe");
		case CTZ<YepX86IsaFeaturePopcnt>::result:
			YEP_RETURN_CONSTANT_STRING("Popcnt");
		case CTZ<YepX86IsaFeatureLzcnt>::result:
			YEP_RETURN_CONSTANT_STRING("Lzcnt");
		case CTZ<YepX86IsaFeatureBMI>::result:
			YEP_RETURN_CONSTANT_STRING("BMI");
		case CTZ<YepX86IsaFeatureBMI2>::result:
			YEP_RETURN_CONSTANT_STRING("BMI2");
		case CTZ<YepX86IsaFeatureTBM>::result:
			YEP_RETURN_CONSTANT_STRING("TBM");
		case CTZ<YepX86IsaFeatureRdrand>::result:
			YEP_RETURN_CONSTANT_STRING("Rdrand");
		case CTZ<YepX86IsaFeatureACE>::result:
			YEP_RETURN_CONSTANT_STRING("ACE");
		case CTZ<YepX86IsaFeatureACE2>::result:
			YEP_RETURN_CONSTANT_STRING("ACE2");
		case CTZ<YepX86IsaFeatureRNG>::result:
			YEP_RETURN_CONSTANT_STRING("RNG");
		case CTZ<YepX86IsaFeaturePHE>::result:
			YEP_RETURN_CONSTANT_STRING("PHE");
		case CTZ<YepX86IsaFeaturePMM>::result:
			YEP_RETURN_CONSTANT_STRING("PMM");
		case CTZ<YepX86IsaFeatureAES>::result:
			YEP_RETURN_CONSTANT_STRING("AES");
		case CTZ<YepX86IsaFeaturePclmulqdq>::result:
			YEP_RETURN_CONSTANT_STRING("Pclmulqdq");
		case CTZ<YepX86IsaFeatureRdtscp>::result:
			YEP_RETURN_CONSTANT_STRING("Rdtscp");
		case CTZ<YepX86IsaFeatureLWP>::result:
			YEP_RETURN_CONSTANT_STRING("LWP");
		case CTZ<YepX86IsaFeatureHLE>::result:
			YEP_RETURN_CONSTANT_STRING("HLE");
		case CTZ<YepX86IsaFeatureRTM>::result:
			YEP_RETURN_CONSTANT_STRING("RTM");
		case CTZ<YepX86IsaFeatureXtest>::result:
			YEP_RETURN_CONSTANT_STRING("Xtest");
		case CTZ<YepX86IsaFeatureRdseed>::result:
			YEP_RETURN_CONSTANT_STRING("Rdseed");
		case CTZ<YepX86IsaFeatureADX>::result:
			YEP_RETURN_CONSTANT_STRING("ADX");
		case CTZ<YepX86IsaFeatureSHA>::result:
			YEP_RETURN_CONSTANT_STRING("SHA");
		case CTZ<YepX86IsaFeatureMPX>::result:
			YEP_RETURN_CONSTANT_STRING("MPX");
		default:
			return getGenericIsaFeatureID(ctzIsaFeature);
	}
};

static ConstantString getX86SimdFeatureDescription(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		case CTZ<YepX86SimdFeatureMMX>::result:
			YEP_RETURN_CONSTANT_STRING("MMX instruction set");
		case CTZ<YepX86SimdFeatureMMXPlus>::result:
			YEP_RETURN_CONSTANT_STRING("MMX+ instruction set");
		case CTZ<YepX86SimdFeatureEMMX>::result:
			YEP_RETURN_CONSTANT_STRING("EMMX instruction set");
		case CTZ<YepX86SimdFeature3dnow>::result:
			YEP_RETURN_CONSTANT_STRING("3dnow! instruction set");
		case CTZ<YepX86SimdFeature3dnowPlus>::result:
			YEP_RETURN_CONSTANT_STRING("3dnow!+ instruction set");
		case CTZ<YepX86SimdFeature3dnowPrefetch>::result:
			YEP_RETURN_CONSTANT_STRING("3dnow! prefetch instructions");
		case CTZ<YepX86SimdFeature3dnowGeode>::result:
			YEP_RETURN_CONSTANT_STRING("Geode 3dnow! instructions");
		case CTZ<YepX86SimdFeatureSSE>::result:
			YEP_RETURN_CONSTANT_STRING("SSE instruction set");
		case CTZ<YepX86SimdFeatureSSE2>::result:
			YEP_RETURN_CONSTANT_STRING("SSE 2 instruction set");
		case CTZ<YepX86SimdFeatureSSE3>::result:
			YEP_RETURN_CONSTANT_STRING("SSE 3 instruction set");
		case CTZ<YepX86SimdFeatureSSSE3>::result:
			YEP_RETURN_CONSTANT_STRING("Supplemental SSE 3 instruction set");
		case CTZ<YepX86SimdFeatureSSE4_1>::result:
			YEP_RETURN_CONSTANT_STRING("SSE 4.1 instruction set");
		case CTZ<YepX86SimdFeatureSSE4_2>::result:
			YEP_RETURN_CONSTANT_STRING("SSE 4.2 instruction set");
		case CTZ<YepX86SimdFeatureSSE4A>::result:
			YEP_RETURN_CONSTANT_STRING("SSE 4A instruction set");
		case CTZ<YepX86SimdFeatureAVX>::result:
			YEP_RETURN_CONSTANT_STRING("AVX instruction set");
		case CTZ<YepX86SimdFeatureAVX2>::result:
			YEP_RETURN_CONSTANT_STRING("AVX 2 instruction set");
		case CTZ<YepX86SimdFeatureXOP>::result:
			YEP_RETURN_CONSTANT_STRING("XOP instruction set");
		case CTZ<YepX86SimdFeatureF16C>::result:
			YEP_RETURN_CONSTANT_STRING("F16C instruction set");
		case CTZ<YepX86SimdFeatureFMA3>::result:
			YEP_RETURN_CONSTANT_STRING("FMA3 instruction set");
		case CTZ<YepX86SimdFeatureFMA4>::result:
			YEP_RETURN_CONSTANT_STRING("FMA4 instruction set");
		case CTZ<YepX86SimdFeatureKNF>::result:
			YEP_RETURN_CONSTANT_STRING("KNF instruction set");
		case CTZ<YepX86SimdFeatureKNC>::result:
			YEP_RETURN_CONSTANT_STRING("KNC instruction set");
		case CTZ<YepX86SimdFeatureAVX512F>::result:
			YEP_RETURN_CONSTANT_STRING("AVX-512 Foundation instructions");
		case CTZ<YepX86SimdFeatureAVX512CD>::result:
			YEP_RETURN_CONSTANT_STRING("AVX-512 Conflict Detection instructions");
		case CTZ<YepX86SimdFeatureAVX512ER>::result:
			YEP_RETURN_CONSTANT_STRING("AVX-512 Exponential and Reciprocal instructions");
		case CTZ<YepX86SimdFeatureAVX512PF>::result:
			YEP_RETURN_CONSTANT_STRING("AVX-512 Prefetch instructions");
		default:
			return getGenericSimdFeatureDescription(ctzSimdFeature);
	}
};

static ConstantString getX86SimdFeatureID(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		case CTZ<YepX86SimdFeatureMMX>::result:
			YEP_RETURN_CONSTANT_STRING("MMX");
		case CTZ<YepX86SimdFeatureMMXPlus>::result:
			YEP_RETURN_CONSTANT_STRING("MMXPlus");
		case CTZ<YepX86SimdFeatureEMMX>::result:
			YEP_RETURN_CONSTANT_STRING("EMMX");
		case CTZ<YepX86SimdFeature3dnow>::result:
			YEP_RETURN_CONSTANT_STRING("3dnow");
		case CTZ<YepX86SimdFeature3dnowPlus>::result:
			YEP_RETURN_CONSTANT_STRING("3dnowPlus");
		case CTZ<YepX86SimdFeature3dnowPrefetch>::result:
			YEP_RETURN_CONSTANT_STRING("3dnowPrefetch");
		case CTZ<YepX86SimdFeature3dnowGeode>::result:
			YEP_RETURN_CONSTANT_STRING("3dnowGeode");
		case CTZ<YepX86SimdFeatureSSE>::result:
			YEP_RETURN_CONSTANT_STRING("SSE");
		case CTZ<YepX86SimdFeatureSSE2>::result:
			YEP_RETURN_CONSTANT_STRING("SSE2");
		case CTZ<YepX86SimdFeatureSSE3>::result:
			YEP_RETURN_CONSTANT_STRING("SSE3");
		case CTZ<YepX86SimdFeatureSSSE3>::result:
			YEP_RETURN_CONSTANT_STRING("SSSE3");
		case CTZ<YepX86SimdFeatureSSE4_1>::result:
			YEP_RETURN_CONSTANT_STRING("SSE4_1");
		case CTZ<YepX86SimdFeatureSSE4_2>::result:
			YEP_RETURN_CONSTANT_STRING("SSE4_2");
		case CTZ<YepX86SimdFeatureSSE4A>::result:
			YEP_RETURN_CONSTANT_STRING("SSE4A");
		case CTZ<YepX86SimdFeatureAVX>::result:
			YEP_RETURN_CONSTANT_STRING("AVX");
		case CTZ<YepX86SimdFeatureAVX2>::result:
			YEP_RETURN_CONSTANT_STRING("AVX2");
		case CTZ<YepX86SimdFeatureXOP>::result:
			YEP_RETURN_CONSTANT_STRING("XOP");
		case CTZ<YepX86SimdFeatureF16C>::result:
			YEP_RETURN_CONSTANT_STRING("F16C");
		case CTZ<YepX86SimdFeatureFMA3>::result:
			YEP_RETURN_CONSTANT_STRING("FMA3");
		case CTZ<YepX86SimdFeatureFMA4>::result:
			YEP_RETURN_CONSTANT_STRING("FMA4");
		case CTZ<YepX86SimdFeatureKNF>::result:
			YEP_RETURN_CONSTANT_STRING("KNF");
		case CTZ<YepX86SimdFeatureKNC>::result:
			YEP_RETURN_CONSTANT_STRING("KNC");
		case CTZ<YepX86SimdFeatureAVX512F>::result:
			YEP_RETURN_CONSTANT_STRING("AVX512F");
		case CTZ<YepX86SimdFeatureAVX512CD>::result:
			YEP_RETURN_CONSTANT_STRING("AVX512CD");
		case CTZ<YepX86SimdFeatureAVX512ER>::result:
			YEP_RETURN_CONSTANT_STRING("AVX512ER");
		case CTZ<YepX86SimdFeatureAVX512PF>::result:
			YEP_RETURN_CONSTANT_STRING("AVX512PF");
		default:
			return getGenericSimdFeatureID(ctzSimdFeature);
	}
};

static ConstantString getX86SystemFeatureDescription(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		case CTZ<YepX86SystemFeatureACE>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Advanced Cryptography Engine");
		case CTZ<YepX86SystemFeatureACE2>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Advanced Cryptography Engine 2");
		case CTZ<YepX86SystemFeatureRNG>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Random Number Generator");
		case CTZ<YepX86SystemFeaturePHE>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Hash Engine");
		case CTZ<YepX86SystemFeaturePMM>::result:
			YEP_RETURN_CONSTANT_STRING("Padlock Montgomery Multiplier");
		case CTZ<YepX86SystemFeatureMisalignedSSE>::result:
			YEP_RETURN_CONSTANT_STRING("Misaligned memory operands in SSE instructions");
		case CTZ<YepX86SystemFeatureFPU>::result:
			YEP_RETURN_CONSTANT_STRING("x87 FPU registers");
		case CTZ<YepX86SystemFeatureXMM>::result:
			YEP_RETURN_CONSTANT_STRING("XMM registers");
		case CTZ<YepX86SystemFeatureYMM>::result:
			YEP_RETURN_CONSTANT_STRING("YMM registers");
		case CTZ<YepX86SystemFeatureZMM>::result:
			YEP_RETURN_CONSTANT_STRING("ZMM registers");
		case CTZ<YepX86SystemFeatureBND>::result:
			YEP_RETURN_CONSTANT_STRING("BND registers");
		default:
			return getGenericSystemFeatureDescription(ctzSystemFeature);
	}
};

static ConstantString getX86SystemFeatureID(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		case CTZ<YepX86SystemFeatureACE>::result:
			YEP_RETURN_CONSTANT_STRING("ACE");
		case CTZ<YepX86SystemFeatureACE2>::result:
			YEP_RETURN_CONSTANT_STRING("ACE2");
		case CTZ<YepX86SystemFeatureRNG>::result:
			YEP_RETURN_CONSTANT_STRING("RNG");
		case CTZ<YepX86SystemFeaturePHE>::result:
			YEP_RETURN_CONSTANT_STRING("PHE");
		case CTZ<YepX86SystemFeaturePMM>::result:
			YEP_RETURN_CONSTANT_STRING("PMM");
		case CTZ<YepX86SystemFeatureMisalignedSSE>::result:
			YEP_RETURN_CONSTANT_STRING("MisalignedSSE");
		case CTZ<YepX86SystemFeatureFPU>::result:
			YEP_RETURN_CONSTANT_STRING("FPU");
		case CTZ<YepX86SystemFeatureXMM>::result:
			YEP_RETURN_CONSTANT_STRING("XMM");
		case CTZ<YepX86SystemFeatureYMM>::result:
			YEP_RETURN_CONSTANT_STRING("YMM");
		case CTZ<YepX86SystemFeatureZMM>::result:
			YEP_RETURN_CONSTANT_STRING("ZMM");
		case CTZ<YepX86SystemFeatureBND>::result:
			YEP_RETURN_CONSTANT_STRING("BND");
		default:
			return getGenericSystemFeatureID(ctzSystemFeature);
	}
};

static ConstantString getARMIsaFeatureDescription(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		case CTZ<YepARMIsaFeatureV4>::result:
			YEP_RETURN_CONSTANT_STRING("ARMv4 instruction set");
		case CTZ<YepARMIsaFeatureV5>::result:
			YEP_RETURN_CONSTANT_STRING("ARMv5 instruction set");
		case CTZ<YepARMIsaFeatureV5E>::result:
			YEP_RETURN_CONSTANT_STRING("ARMv5 DSP instructions");
		case CTZ<YepARMIsaFeatureV6>::result:
			YEP_RETURN_CONSTANT_STRING("ARMv6 instruction set");
		case CTZ<YepARMIsaFeatureV6K>::result:
			YEP_RETURN_CONSTANT_STRING("ARMv6 Multiprocessing extensions");
		case CTZ<YepARMIsaFeatureV7>::result:
			YEP_RETURN_CONSTANT_STRING("ARMv7 instruction set");
		case CTZ<YepARMIsaFeatureV7MP>::result:
			YEP_RETURN_CONSTANT_STRING("ARMv7 Multiprocessing extensions");
		case CTZ<YepARMIsaFeatureThumb>::result:
			YEP_RETURN_CONSTANT_STRING("Thumb mode");
		case CTZ<YepARMIsaFeatureThumb2>::result:
			YEP_RETURN_CONSTANT_STRING("Thumb-2 mode");
		case CTZ<YepARMIsaFeatureThumbEE>::result:
			YEP_RETURN_CONSTANT_STRING("Thumb EE mode");
		case CTZ<YepARMIsaFeatureJazelle>::result:
			YEP_RETURN_CONSTANT_STRING("Jazelle extension");
		case CTZ<YepARMIsaFeatureFPA>::result:
			YEP_RETURN_CONSTANT_STRING("FPA instruction set");
		case CTZ<YepARMIsaFeatureVFP>::result:
			YEP_RETURN_CONSTANT_STRING("VFP instruction set");
		case CTZ<YepARMIsaFeatureVFP2>::result:
			YEP_RETURN_CONSTANT_STRING("VFPv2 instruction set");
		case CTZ<YepARMIsaFeatureVFP3>::result:
			YEP_RETURN_CONSTANT_STRING("VFPv3 instruction set");
		case CTZ<YepARMIsaFeatureVFPd32>::result:
			YEP_RETURN_CONSTANT_STRING("VFP with 32 DP registers");
		case CTZ<YepARMIsaFeatureVFP3HP>::result:
			YEP_RETURN_CONSTANT_STRING("VFPv3 half-precision extension");
		case CTZ<YepARMIsaFeatureVFP4>::result:
			YEP_RETURN_CONSTANT_STRING("VFPv4 instruction set");
		case CTZ<YepARMIsaFeatureDiv>::result:
			YEP_RETURN_CONSTANT_STRING("SDIV and UDIV instructions");
		case CTZ<YepARMIsaFeatureArmada>::result:
			YEP_RETURN_CONSTANT_STRING("Marvell Armada instruction extensions");
		default:
			return getGenericIsaFeatureDescription(ctzIsaFeature);
	}
};

static ConstantString getARMIsaFeatureID(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		case CTZ<YepARMIsaFeatureV4>::result:
			YEP_RETURN_CONSTANT_STRING("V4");
		case CTZ<YepARMIsaFeatureV5>::result:
			YEP_RETURN_CONSTANT_STRING("V5");
		case CTZ<YepARMIsaFeatureV5E>::result:
			YEP_RETURN_CONSTANT_STRING("V5E");
		case CTZ<YepARMIsaFeatureV6>::result:
			YEP_RETURN_CONSTANT_STRING("V6");
		case CTZ<YepARMIsaFeatureV6K>::result:
			YEP_RETURN_CONSTANT_STRING("V6K");
		case CTZ<YepARMIsaFeatureV7>::result:
			YEP_RETURN_CONSTANT_STRING("V7");
		case CTZ<YepARMIsaFeatureV7MP>::result:
			YEP_RETURN_CONSTANT_STRING("V7MP");
		case CTZ<YepARMIsaFeatureThumb>::result:
			YEP_RETURN_CONSTANT_STRING("Thumb");
		case CTZ<YepARMIsaFeatureThumb2>::result:
			YEP_RETURN_CONSTANT_STRING("Thumb2");
		case CTZ<YepARMIsaFeatureThumbEE>::result:
			YEP_RETURN_CONSTANT_STRING("ThumbEE");
		case CTZ<YepARMIsaFeatureJazelle>::result:
			YEP_RETURN_CONSTANT_STRING("Jazelle");
		case CTZ<YepARMIsaFeatureFPA>::result:
			YEP_RETURN_CONSTANT_STRING("FPA");
		case CTZ<YepARMIsaFeatureVFP>::result:
			YEP_RETURN_CONSTANT_STRING("VFP");
		case CTZ<YepARMIsaFeatureVFP2>::result:
			YEP_RETURN_CONSTANT_STRING("VFP2");
		case CTZ<YepARMIsaFeatureVFP3>::result:
			YEP_RETURN_CONSTANT_STRING("VFP3");
		case CTZ<YepARMIsaFeatureVFPd32>::result:
			YEP_RETURN_CONSTANT_STRING("VFPd32");
		case CTZ<YepARMIsaFeatureVFP3HP>::result:
			YEP_RETURN_CONSTANT_STRING("VFP3HP");
		case CTZ<YepARMIsaFeatureVFP4>::result:
			YEP_RETURN_CONSTANT_STRING("VFP4");
		case CTZ<YepARMIsaFeatureDiv>::result:
			YEP_RETURN_CONSTANT_STRING("Div");
		case CTZ<YepARMIsaFeatureArmada>::result:
			YEP_RETURN_CONSTANT_STRING("Armada");
		default:
			return getGenericIsaFeatureID(ctzIsaFeature);
	}
};

static ConstantString getARMSimdFeatureDescription(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		case CTZ<YepARMSimdFeatureXScale>::result:
			YEP_RETURN_CONSTANT_STRING("XScale instructions");
		case CTZ<YepARMSimdFeatureWMMX>::result:
			YEP_RETURN_CONSTANT_STRING("Wireless MMX instruction set");
		case CTZ<YepARMSimdFeatureWMMX2>::result:
			YEP_RETURN_CONSTANT_STRING("Wireless MMX 2 instruction set");
		case CTZ<YepARMSimdFeatureNEON>::result:
			YEP_RETURN_CONSTANT_STRING("NEON (Advanced SIMD) instruction set");
		case CTZ<YepARMSimdFeatureNEONHP>::result:
			YEP_RETURN_CONSTANT_STRING("NEON (Advanced SIMD) half-precision extension");
		case CTZ<YepARMSimdFeatureNEON2>::result:
			YEP_RETURN_CONSTANT_STRING("NEON (Advanced SIMD) v2 instruction set");
		default:
			return getGenericSimdFeatureDescription(ctzSimdFeature);
	}
};

static ConstantString getARMSimdFeatureID(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		case CTZ<YepARMSimdFeatureXScale>::result:
			YEP_RETURN_CONSTANT_STRING("XScale");
		case CTZ<YepARMSimdFeatureWMMX>::result:
			YEP_RETURN_CONSTANT_STRING("WMMX");
		case CTZ<YepARMSimdFeatureWMMX2>::result:
			YEP_RETURN_CONSTANT_STRING("WMMX2");
		case CTZ<YepARMSimdFeatureNEON>::result:
			YEP_RETURN_CONSTANT_STRING("NEON");
		case CTZ<YepARMSimdFeatureNEONHP>::result:
			YEP_RETURN_CONSTANT_STRING("NEONHP");
		case CTZ<YepARMSimdFeatureNEON2>::result:
			YEP_RETURN_CONSTANT_STRING("NEON2");
		default:
			return getGenericSimdFeatureID(ctzSimdFeature);
	}
};

static ConstantString getARMSystemFeatureDescription(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		case CTZ<YepARMSystemFeatureVFPVectorMode>::result:
			YEP_RETURN_CONSTANT_STRING("Hardware VFP vector mode");
		case CTZ<YepARMSystemFeatureFPA>::result:
			YEP_RETURN_CONSTANT_STRING("FPA registers");
		case CTZ<YepARMSystemFeatureWMMX>::result:
			YEP_RETURN_CONSTANT_STRING("WMMX registers");
		case CTZ<YepARMSystemFeatureS32>::result:
			YEP_RETURN_CONSTANT_STRING("32 VFP S registers");
		case CTZ<YepARMSystemFeatureD32>::result:
			YEP_RETURN_CONSTANT_STRING("32 VFP D registers");
		default:
			return getGenericSystemFeatureDescription(ctzSystemFeature);
	}
};

static ConstantString getARMSystemFeatureID(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		case CTZ<YepARMSystemFeatureVFPVectorMode>::result:
			YEP_RETURN_CONSTANT_STRING("VFPVectorMode");
		case CTZ<YepARMSystemFeatureFPA>::result:
			YEP_RETURN_CONSTANT_STRING("FPA");
		case CTZ<YepARMSystemFeatureWMMX>::result:
			YEP_RETURN_CONSTANT_STRING("WMMX");
		case CTZ<YepARMSystemFeatureS32>::result:
			YEP_RETURN_CONSTANT_STRING("S32");
		case CTZ<YepARMSystemFeatureD32>::result:
			YEP_RETURN_CONSTANT_STRING("D32");
		default:
			return getGenericSystemFeatureID(ctzSystemFeature);
	}
};

static ConstantString getMIPSIsaFeatureDescription(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		case CTZ<YepMIPSIsaFeatureMIPS_I>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS I instructions");
		case CTZ<YepMIPSIsaFeatureMIPS_II>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS II instructions");
		case CTZ<YepMIPSIsaFeatureMIPS_III>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS III instructions");
		case CTZ<YepMIPSIsaFeatureMIPS_IV>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS IV instructions");
		case CTZ<YepMIPSIsaFeatureMIPS_V>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS V instructions");
		case CTZ<YepMIPSIsaFeatureR1>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS32/MIPS64 Release 1 instructions");
		case CTZ<YepMIPSIsaFeatureR2>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS32/MIPS64 Release 2 instructions");
		case CTZ<YepMIPSIsaFeatureFPU>::result:
			YEP_RETURN_CONSTANT_STRING("FPU with S, D, and W formats");
		case CTZ<YepMIPSIsaFeatureMIPS16>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS16 extension");
		case CTZ<YepMIPSIsaFeatureSmartMIPS>::result:
			YEP_RETURN_CONSTANT_STRING("SmartMIPS extension");
		case CTZ<YepMIPSIsaFeatureMT>::result:
			YEP_RETURN_CONSTANT_STRING("Multi-threading extension");
		case CTZ<YepMIPSIsaFeatureMicroMIPS>::result:
			YEP_RETURN_CONSTANT_STRING("MicroMIPS extension");
		case CTZ<YepMIPSIsaFeatureVZ>::result:
			YEP_RETURN_CONSTANT_STRING("Virtualization extension");
		default:
			return getGenericIsaFeatureDescription(ctzIsaFeature);
	}
};

static ConstantString getMIPSIsaFeatureID(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		case CTZ<YepMIPSIsaFeatureMIPS_I>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS_I");
		case CTZ<YepMIPSIsaFeatureMIPS_II>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS_II");
		case CTZ<YepMIPSIsaFeatureMIPS_III>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS_III");
		case CTZ<YepMIPSIsaFeatureMIPS_IV>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS_IV");
		case CTZ<YepMIPSIsaFeatureMIPS_V>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS_V");
		case CTZ<YepMIPSIsaFeatureR1>::result:
			YEP_RETURN_CONSTANT_STRING("R1");
		case CTZ<YepMIPSIsaFeatureR2>::result:
			YEP_RETURN_CONSTANT_STRING("R2");
		case CTZ<YepMIPSIsaFeatureFPU>::result:
			YEP_RETURN_CONSTANT_STRING("FPU");
		case CTZ<YepMIPSIsaFeatureMIPS16>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS16");
		case CTZ<YepMIPSIsaFeatureSmartMIPS>::result:
			YEP_RETURN_CONSTANT_STRING("SmartMIPS");
		case CTZ<YepMIPSIsaFeatureMT>::result:
			YEP_RETURN_CONSTANT_STRING("MT");
		case CTZ<YepMIPSIsaFeatureMicroMIPS>::result:
			YEP_RETURN_CONSTANT_STRING("MicroMIPS");
		case CTZ<YepMIPSIsaFeatureVZ>::result:
			YEP_RETURN_CONSTANT_STRING("VZ");
		default:
			return getGenericIsaFeatureID(ctzIsaFeature);
	}
};

static ConstantString getMIPSSimdFeatureDescription(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		case CTZ<YepMIPSSimdFeatureMDMX>::result:
			YEP_RETURN_CONSTANT_STRING("MDMX instruction set");
		case CTZ<YepMIPSSimdFeaturePairedSingle>::result:
			YEP_RETURN_CONSTANT_STRING("Paired-single instructions");
		case CTZ<YepMIPSSimdFeatureMIPS3D>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS3D instruction set");
		case CTZ<YepMIPSSimdFeatureDSP>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS DSP extension");
		case CTZ<YepMIPSSimdFeatureDSP2>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS DSP Release 2 extension");
		case CTZ<YepMIPSSimdFeatureGodsonMMX>::result:
			YEP_RETURN_CONSTANT_STRING("Loongson (Godson) MMX instruction set");
		case CTZ<YepMIPSSimdFeatureMXU>::result:
			YEP_RETURN_CONSTANT_STRING("Ingenic Media Extension");
		case CTZ<YepMIPSSimdFeatureMXU2>::result:
			YEP_RETURN_CONSTANT_STRING("Ingenic Media Extension 2");
		default:
			return getGenericSimdFeatureDescription(ctzSimdFeature);
	}
};

static ConstantString getMIPSSimdFeatureID(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		case CTZ<YepMIPSSimdFeatureMDMX>::result:
			YEP_RETURN_CONSTANT_STRING("MDMX");
		case CTZ<YepMIPSSimdFeaturePairedSingle>::result:
			YEP_RETURN_CONSTANT_STRING("PairedSingle");
		case CTZ<YepMIPSSimdFeatureMIPS3D>::result:
			YEP_RETURN_CONSTANT_STRING("MIPS3D");
		case CTZ<YepMIPSSimdFeatureDSP>::result:
			YEP_RETURN_CONSTANT_STRING("DSP");
		case CTZ<YepMIPSSimdFeatureDSP2>::result:
			YEP_RETURN_CONSTANT_STRING("DSP2");
		case CTZ<YepMIPSSimdFeatureGodsonMMX>::result:
			YEP_RETURN_CONSTANT_STRING("GodsonMMX");
		case CTZ<YepMIPSSimdFeatureMXU>::result:
			YEP_RETURN_CONSTANT_STRING("MXU");
		case CTZ<YepMIPSSimdFeatureMXU2>::result:
			YEP_RETURN_CONSTANT_STRING("MXU2");
		default:
			return getGenericSimdFeatureID(ctzSimdFeature);
	}
};

static ConstantString getMIPSSystemFeatureDescription(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		default:
			return getGenericSystemFeatureDescription(ctzSystemFeature);
	}
};

static ConstantString getMIPSSystemFeatureID(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		default:
			return getGenericSystemFeatureID(ctzSystemFeature);
	}
};

static ConstantString getPowerPCIsaFeatureDescription(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		default:
			return getGenericIsaFeatureDescription(ctzIsaFeature);
	}
};

static ConstantString getPowerPCIsaFeatureID(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		default:
			return getGenericIsaFeatureID(ctzIsaFeature);
	}
};

static ConstantString getPowerPCSimdFeatureDescription(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		default:
			return getGenericSimdFeatureDescription(ctzSimdFeature);
	}
};

static ConstantString getPowerPCSimdFeatureID(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		default:
			return getGenericSimdFeatureID(ctzSimdFeature);
	}
};

static ConstantString getPowerPCSystemFeatureDescription(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		default:
			return getGenericSystemFeatureDescription(ctzSystemFeature);
	}
};

static ConstantString getPowerPCSystemFeatureID(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		default:
			return getGenericSystemFeatureID(ctzSystemFeature);
	}
};

static ConstantString getIA64IsaFeatureDescription(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		default:
			return getGenericIsaFeatureDescription(ctzIsaFeature);
	}
};

static ConstantString getIA64IsaFeatureID(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		default:
			return getGenericIsaFeatureID(ctzIsaFeature);
	}
};

static ConstantString getIA64SimdFeatureDescription(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		default:
			return getGenericSimdFeatureDescription(ctzSimdFeature);
	}
};

static ConstantString getIA64SimdFeatureID(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		default:
			return getGenericSimdFeatureID(ctzSimdFeature);
	}
};

static ConstantString getIA64SystemFeatureDescription(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		default:
			return getGenericSystemFeatureDescription(ctzSystemFeature);
	}
};

static ConstantString getIA64SystemFeatureID(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		default:
			return getGenericSystemFeatureID(ctzSystemFeature);
	}
};

static ConstantString getSPARCIsaFeatureDescription(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		default:
			return getGenericIsaFeatureDescription(ctzIsaFeature);
	}
};

static ConstantString getSPARCIsaFeatureID(Yep32u ctzIsaFeature) {
	switch (ctzIsaFeature) {
		default:
			return getGenericIsaFeatureID(ctzIsaFeature);
	}
};

static ConstantString getSPARCSimdFeatureDescription(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		default:
			return getGenericSimdFeatureDescription(ctzSimdFeature);
	}
};

static ConstantString getSPARCSimdFeatureID(Yep32u ctzSimdFeature) {
	switch (ctzSimdFeature) {
		default:
			return getGenericSimdFeatureID(ctzSimdFeature);
	}
};

static ConstantString getSPARCSystemFeatureDescription(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		default:
			return getGenericSystemFeatureDescription(ctzSystemFeature);
	}
};

static ConstantString getSPARCSystemFeatureID(Yep32u ctzSystemFeature) {
	switch (ctzSystemFeature) {
		default:
			return getGenericSystemFeatureID(ctzSystemFeature);
	}
};

YepStatus YEPABI yepLibrary_GetString(YepEnumeration enumerationType, Yep32u enumerationValue, YepStringType stringType, void *buffer, YepSize *lengthPointer) {
	if YEP_UNLIKELY(lengthPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	const YepSize length = *lengthPointer;
	ConstantString constantString;
	switch (stringType) {
		case YepStringTypeDescription:
			switch (enumerationType) {
				case YepEnumerationStatus:
					constantString = getStatusDescription(static_cast<YepStatus>(enumerationValue)); break;
				case YepEnumerationCpuArchitecture:
					constantString = getCpuArchitectureDescription(static_cast<YepCpuArchitecture>(enumerationValue)); break;
				case YepEnumerationCpuVendor:
					constantString = _yepLibrary_GetCpuVendorDescription(static_cast<YepCpuVendor>(enumerationValue)); break;
				case YepEnumerationCpuMicroarchitecture:
					constantString = _yepLibrary_GetCpuMicroarchitectureDescription(static_cast<YepCpuMicroarchitecture>(enumerationValue)); break;
				case YepEnumerationCpuBriefName:
					constantString = _briefCpuName; break;
				case YepEnumerationCpuFullName:
					constantString = _fullCpuName; break;
				case YepEnumerationGenericIsaFeature:
					constantString = getGenericIsaFeatureDescription(enumerationValue); break;
				case YepEnumerationGenericSimdFeature:
					constantString = getGenericSimdFeatureDescription(enumerationValue); break;
				case YepEnumerationGenericSystemFeature:
					constantString = getGenericSystemFeatureDescription(enumerationValue); break;
				case YepEnumerationX86IsaFeature:
					constantString = getX86IsaFeatureDescription(enumerationValue); break;
				case YepEnumerationX86SimdFeature:
					constantString = getX86SimdFeatureDescription(enumerationValue); break;
				case YepEnumerationX86SystemFeature:
					constantString = getX86SystemFeatureDescription(enumerationValue); break;
				case YepEnumerationARMIsaFeature:
					constantString = getARMIsaFeatureDescription(enumerationValue); break;
				case YepEnumerationARMSimdFeature:
					constantString = getARMSimdFeatureDescription(enumerationValue); break;
				case YepEnumerationARMSystemFeature:
					constantString = getARMSystemFeatureDescription(enumerationValue); break;
				case YepEnumerationMIPSIsaFeature:
					constantString = getMIPSIsaFeatureDescription(enumerationValue); break;
				case YepEnumerationMIPSSimdFeature:
					constantString = getMIPSSimdFeatureDescription(enumerationValue); break;
				case YepEnumerationMIPSSystemFeature:
					constantString = getMIPSSystemFeatureDescription(enumerationValue); break;
				case YepEnumerationPowerPCIsaFeature:
					constantString = getPowerPCIsaFeatureDescription(enumerationValue); break;
				case YepEnumerationPowerPCSimdFeature:
					constantString = getPowerPCSimdFeatureDescription(enumerationValue); break;
				case YepEnumerationPowerPCSystemFeature:
					constantString = getPowerPCSystemFeatureDescription(enumerationValue); break;
				case YepEnumerationIA64IsaFeature:
					constantString = getIA64IsaFeatureDescription(enumerationValue); break;
				case YepEnumerationIA64SimdFeature:
					constantString = getIA64SimdFeatureDescription(enumerationValue); break;
				case YepEnumerationIA64SystemFeature:
					constantString = getIA64SystemFeatureDescription(enumerationValue); break;
				case YepEnumerationSPARCIsaFeature:
					constantString = getSPARCIsaFeatureDescription(enumerationValue); break;
				case YepEnumerationSPARCSimdFeature:
					constantString = getSPARCSimdFeatureDescription(enumerationValue); break;
				case YepEnumerationSPARCSystemFeature:
					constantString = getSPARCSystemFeatureDescription(enumerationValue); break;
			}
			break;
		case YepStringTypeID:
			switch (enumerationType) {
				case YepEnumerationStatus:
					constantString = getStatusID(static_cast<YepStatus>(enumerationValue)); break;
				case YepEnumerationCpuArchitecture:
					constantString = getCpuArchitectureID(static_cast<YepCpuArchitecture>(enumerationValue)); break;
				case YepEnumerationCpuVendor:
					constantString = _yepLibrary_GetCpuVendorID(static_cast<YepCpuVendor>(enumerationValue)); break;
				case YepEnumerationCpuMicroarchitecture:
					constantString = _yepLibrary_GetCpuMicroarchitectureID(static_cast<YepCpuMicroarchitecture>(enumerationValue)); break;
				case YepEnumerationCpuBriefName:
					break;
				case YepEnumerationCpuFullName:
					break;
				case YepEnumerationGenericIsaFeature:
					constantString = getGenericIsaFeatureID(enumerationValue); break;
				case YepEnumerationGenericSimdFeature:
					constantString = getGenericSimdFeatureID(enumerationValue); break;
				case YepEnumerationGenericSystemFeature:
					constantString = getGenericSystemFeatureID(enumerationValue); break;
				case YepEnumerationX86IsaFeature:
					constantString = getX86IsaFeatureID(enumerationValue); break;
				case YepEnumerationX86SimdFeature:
					constantString = getX86SimdFeatureID(enumerationValue); break;
				case YepEnumerationX86SystemFeature:
					constantString = getX86SystemFeatureID(enumerationValue); break;
				case YepEnumerationARMIsaFeature:
					constantString = getARMIsaFeatureID(enumerationValue); break;
				case YepEnumerationARMSimdFeature:
					constantString = getARMSimdFeatureID(enumerationValue); break;
				case YepEnumerationARMSystemFeature:
					constantString = getARMSystemFeatureID(enumerationValue); break;
				case YepEnumerationMIPSIsaFeature:
					constantString = getMIPSIsaFeatureID(enumerationValue); break;
				case YepEnumerationMIPSSimdFeature:
					constantString = getMIPSSimdFeatureID(enumerationValue); break;
				case YepEnumerationMIPSSystemFeature:
					constantString = getMIPSSystemFeatureID(enumerationValue); break;
				case YepEnumerationPowerPCIsaFeature:
					constantString = getPowerPCIsaFeatureID(enumerationValue); break;
				case YepEnumerationPowerPCSimdFeature:
					constantString = getPowerPCSimdFeatureID(enumerationValue); break;
				case YepEnumerationPowerPCSystemFeature:
					constantString = getPowerPCSystemFeatureID(enumerationValue); break;
				case YepEnumerationIA64IsaFeature:
					constantString = getIA64IsaFeatureID(enumerationValue); break;
				case YepEnumerationIA64SimdFeature:
					constantString = getIA64SimdFeatureID(enumerationValue); break;
				case YepEnumerationIA64SystemFeature:
					constantString = getIA64SystemFeatureID(enumerationValue); break;
				case YepEnumerationSPARCIsaFeature:
					constantString = getSPARCIsaFeatureID(enumerationValue); break;
				case YepEnumerationSPARCSimdFeature:
					constantString = getSPARCSimdFeatureID(enumerationValue); break;
				case YepEnumerationSPARCSystemFeature:
					constantString = getSPARCSystemFeatureID(enumerationValue); break;
			}
			break;
	}
	if YEP_UNLIKELY(constantString.isEmpty()) {
		return YepStatusInvalidArgument;
	}
	if (YEP_LIKELY(buffer == YEP_NULL_POINTER) || YEP_UNLIKELY(constantString.length > length)) {
		*lengthPointer = constantString.length;
		return YepStatusInsufficientBuffer;
	} else {
		memcpy(buffer, static_cast<const void*>(constantString.pointer), constantString.length);
		*lengthPointer = constantString.length;
		return YepStatusOk;
	}
}
