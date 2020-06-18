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

YEP_PRIVATE_SYMBOL const YepCpuMicroarchitecture *_yepLibrary_GetMicroarchitectureDispatchList(YepCpuMicroarchitecture microarchitecture) {
#if defined(YEP_X86_ABI)
	static const YepCpuMicroarchitecture dispatchUnknown[] = { YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchP5[] = { YepCpuMicroarchitectureP5, YepCpuMicroarchitectureK5, YepCpuMicroarchitectureKnightsFerry, YepCpuMicroarchitectureKnightsCorner, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchP6[] = { YepCpuMicroarchitectureP6, YepCpuMicroarchitectureDothan, YepCpuMicroarchitectureK7, YepCpuMicroarchitectureK6, YepCpuMicroarchitectureYonah, YepCpuMicroarchitectureConroe, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchWillamette[] = { YepCpuMicroarchitectureWillamette, YepCpuMicroarchitecturePrescott, YepCpuMicroarchitectureYonah, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchPrescott[] = { YepCpuMicroarchitecturePrescott, YepCpuMicroarchitectureWillamette, YepCpuMicroarchitectureYonah, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchDothan[] = { YepCpuMicroarchitectureDothan, YepCpuMicroarchitectureP6, YepCpuMicroarchitectureYonah, YepCpuMicroarchitectureConroe, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchYonah[] = { YepCpuMicroarchitectureYonah, YepCpuMicroarchitectureConroe, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureDothan, YepCpuMicroarchitectureP6, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchConroe[] = { YepCpuMicroarchitectureConroe, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchPenryn[] = { YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchBonnell[] = { YepCpuMicroarchitectureBonnell, YepCpuMicroarchitectureSaltwell, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchNehalem[] = { YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSandyBridge[] = { YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSaltwell[] = { YepCpuMicroarchitectureSaltwell, YepCpuMicroarchitectureBonnell, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchIvyBridge[] = { YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchHaswell[] = { YepCpuMicroarchitectureHaswell, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSilvermont[] = { YepCpuMicroarchitectureSilvermont, YepCpuMicroarchitectureBobcat, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchKnightsFerry[] = { YepCpuMicroarchitectureKnightsFerry, YepCpuMicroarchitectureKnightsCorner, YepCpuMicroarchitectureP5, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchKnightsCorner[] = { YepCpuMicroarchitectureKnightsCorner, YepCpuMicroarchitectureKnightsFerry, YepCpuMicroarchitectureP5, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchK5[] = { YepCpuMicroarchitectureK5, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchK6[] = { YepCpuMicroarchitectureK6, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchGeode[] = { YepCpuMicroarchitectureGeode, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchK7[] = { YepCpuMicroarchitectureK7, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchK8[] = { YepCpuMicroarchitectureK8, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchK10[] = { YepCpuMicroarchitectureK10, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchBobcat[] = { YepCpuMicroarchitectureBobcat, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchBulldozer[] = { YepCpuMicroarchitectureBulldozer, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchPiledriver[] = { YepCpuMicroarchitecturePiledriver, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchJaguar[] = { YepCpuMicroarchitectureJaguar, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSteamroller[] = { YepCpuMicroarchitectureSteamroller, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	
	switch (microarchitecture) {
		case YepCpuMicroarchitectureP5:
			return dispatchP5;
		case YepCpuMicroarchitectureP6:
			return dispatchP6;
		case YepCpuMicroarchitectureWillamette:
			return dispatchWillamette;
		case YepCpuMicroarchitecturePrescott:
			return dispatchPrescott;
		case YepCpuMicroarchitectureDothan:
			return dispatchDothan;
		case YepCpuMicroarchitectureYonah:
			return dispatchYonah;
		case YepCpuMicroarchitectureConroe:
			return dispatchConroe;
		case YepCpuMicroarchitecturePenryn:
			return dispatchPenryn;
		case YepCpuMicroarchitectureBonnell:
			return dispatchBonnell;
		case YepCpuMicroarchitectureNehalem:
			return dispatchNehalem;
		case YepCpuMicroarchitectureSandyBridge:
			return dispatchSandyBridge;
		case YepCpuMicroarchitectureSaltwell:
			return dispatchSaltwell;
		case YepCpuMicroarchitectureIvyBridge:
			return dispatchIvyBridge;
		case YepCpuMicroarchitectureHaswell:
			return dispatchHaswell;
		case YepCpuMicroarchitectureSilvermont:
			return dispatchSilvermont;
		case YepCpuMicroarchitectureKnightsFerry:
			return dispatchKnightsFerry;
		case YepCpuMicroarchitectureKnightsCorner:
			return dispatchKnightsCorner;
		case YepCpuMicroarchitectureK5:
			return dispatchK5;
		case YepCpuMicroarchitectureK6:
			return dispatchK6;
		case YepCpuMicroarchitectureGeode:
			return dispatchGeode;
		case YepCpuMicroarchitectureK7:
			return dispatchK7;
		case YepCpuMicroarchitectureK8:
			return dispatchK8;
		case YepCpuMicroarchitectureK10:
			return dispatchK10;
		case YepCpuMicroarchitectureBobcat:
			return dispatchBobcat;
		case YepCpuMicroarchitectureBulldozer:
			return dispatchBulldozer;
		case YepCpuMicroarchitecturePiledriver:
			return dispatchPiledriver;
		case YepCpuMicroarchitectureJaguar:
			return dispatchJaguar;
		case YepCpuMicroarchitectureSteamroller:
			return dispatchSteamroller;
		default:
			return dispatchUnknown;
	}
#elif defined(YEP_X64_ABI)
	static const YepCpuMicroarchitecture dispatchUnknown[] = { YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchPrescott[] = { YepCpuMicroarchitecturePrescott, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchConroe[] = { YepCpuMicroarchitectureConroe, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchPenryn[] = { YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchBonnell[] = { YepCpuMicroarchitectureBonnell, YepCpuMicroarchitectureSaltwell, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchNehalem[] = { YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSandyBridge[] = { YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureHaswell, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSaltwell[] = { YepCpuMicroarchitectureSaltwell, YepCpuMicroarchitectureBonnell, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchIvyBridge[] = { YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureSteamroller, YepCpuMicroarchitectureHaswell, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchHaswell[] = { YepCpuMicroarchitectureHaswell, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitecturePiledriver, YepCpuMicroarchitectureBulldozer, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSilvermont[] = { YepCpuMicroarchitectureSilvermont, YepCpuMicroarchitectureBobcat, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchKnightsFerry[] = { YepCpuMicroarchitectureKnightsFerry, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchKnightsCorner[] = { YepCpuMicroarchitectureKnightsCorner,YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchK8[] = { YepCpuMicroarchitectureK8, YepCpuMicroarchitectureBobcat, YepCpuMicroarchitectureBonnell, YepCpuMicroarchitectureK10, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitecturePrescott, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchK10[] = { YepCpuMicroarchitectureK10, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureBonnell, YepCpuMicroarchitectureSilvermont, YepCpuMicroarchitectureJaguar, YepCpuMicroarchitectureK8, YepCpuMicroarchitectureBobcat, YepCpuMicroarchitecturePrescott, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchBobcat[] = { YepCpuMicroarchitectureBobcat, YepCpuMicroarchitectureJaguar, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchBulldozer[] = { YepCpuMicroarchitectureBulldozer, YepCpuMicroarchitecturePiledriver, YepCpuMicroarchitectureSteamroller, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureHaswell, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureK10, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchPiledriver[] = { YepCpuMicroarchitecturePiledriver, YepCpuMicroarchitectureSteamroller, YepCpuMicroarchitectureBulldozer, YepCpuMicroarchitectureHaswell, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitecturePenryn, YepCpuMicroarchitectureConroe, YepCpuMicroarchitectureK10, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchJaguar[] = { YepCpuMicroarchitectureJaguar, YepCpuMicroarchitectureBobcat, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSteamroller[] = { YepCpuMicroarchitectureSteamroller, YepCpuMicroarchitectureHaswell, YepCpuMicroarchitecturePiledriver, YepCpuMicroarchitectureBulldozer, YepCpuMicroarchitectureIvyBridge, YepCpuMicroarchitectureSandyBridge, YepCpuMicroarchitectureNehalem, YepCpuMicroarchitectureUnknown };

	switch (microarchitecture) {
		case YepCpuMicroarchitecturePrescott:
			return dispatchPrescott;
		case YepCpuMicroarchitectureConroe:
			return dispatchConroe;
		case YepCpuMicroarchitecturePenryn:
			return dispatchPenryn;
		case YepCpuMicroarchitectureBonnell:
			return dispatchBonnell;
		case YepCpuMicroarchitectureNehalem:
			return dispatchNehalem;
		case YepCpuMicroarchitectureSandyBridge:
			return dispatchSandyBridge;
		case YepCpuMicroarchitectureSaltwell:
			return dispatchSaltwell;
		case YepCpuMicroarchitectureIvyBridge:
			return dispatchIvyBridge;
		case YepCpuMicroarchitectureHaswell:
			return dispatchHaswell;
		case YepCpuMicroarchitectureSilvermont:
			return dispatchSilvermont;
		case YepCpuMicroarchitectureKnightsFerry:
			return dispatchKnightsFerry;
		case YepCpuMicroarchitectureKnightsCorner:
			return dispatchKnightsCorner;
		case YepCpuMicroarchitectureK8:
			return dispatchK8;
		case YepCpuMicroarchitectureK10:
			return dispatchK10;
		case YepCpuMicroarchitectureBobcat:
			return dispatchBobcat;
		case YepCpuMicroarchitectureBulldozer:
			return dispatchBulldozer;
		case YepCpuMicroarchitecturePiledriver:
			return dispatchPiledriver;
		case YepCpuMicroarchitectureJaguar:
			return dispatchJaguar;
		case YepCpuMicroarchitectureSteamroller: 
			return dispatchSteamroller;
		default:
			return dispatchUnknown;
	}
#elif defined(YEP_ARM_ABI)
	static const YepCpuMicroarchitecture dispatchUnknown[] = { YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchStrongARM[] = { YepCpuMicroarchitectureStrongARM, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchXScale[] = { YepCpuMicroarchitectureXScale, YepCpuMicroarchitectureARM9, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchARM7[] = { YepCpuMicroarchitectureARM7, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchARM9[] = { YepCpuMicroarchitectureARM9, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchARM11[] = { YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureARM9, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchCortexA5[] = { YepCpuMicroarchitectureCortexA5, YepCpuMicroarchitectureCortexA7, YepCpuMicroarchitectureCortexA8, YepCpuMicroarchitectureScorpion, YepCpuMicroarchitectureCortexA9, YepCpuMicroarchitectureKrait, YepCpuMicroarchitectureCortexA15, YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchCortexA7[] = { YepCpuMicroarchitectureCortexA7, YepCpuMicroarchitectureCortexA9, YepCpuMicroarchitectureCortexA8, YepCpuMicroarchitectureScorpion, YepCpuMicroarchitectureKrait, YepCpuMicroarchitectureCortexA15, YepCpuMicroarchitectureCortexA5, YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchCortexA8[] = { YepCpuMicroarchitectureCortexA8, YepCpuMicroarchitectureCortexA5, YepCpuMicroarchitectureCortexA7, YepCpuMicroarchitectureCortexA9, YepCpuMicroarchitectureScorpion, YepCpuMicroarchitectureKrait, YepCpuMicroarchitectureCortexA15, YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchCortexA9[] = { YepCpuMicroarchitectureCortexA9, YepCpuMicroarchitectureCortexA7, YepCpuMicroarchitectureCortexA8, YepCpuMicroarchitectureScorpion, YepCpuMicroarchitectureKrait, YepCpuMicroarchitectureCortexA15, YepCpuMicroarchitectureCortexA5, YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchCortexA15[] = { YepCpuMicroarchitectureCortexA15, YepCpuMicroarchitectureKrait, YepCpuMicroarchitectureScorpion, YepCpuMicroarchitectureCortexA7, YepCpuMicroarchitectureCortexA9, YepCpuMicroarchitectureCortexA8, YepCpuMicroarchitectureCortexA5, YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchScorpion[] = { YepCpuMicroarchitectureScorpion, YepCpuMicroarchitectureKrait, YepCpuMicroarchitectureCortexA15, YepCpuMicroarchitectureCortexA9, YepCpuMicroarchitectureCortexA8, YepCpuMicroarchitectureCortexA7, YepCpuMicroarchitectureCortexA5, YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchKrait[] = { YepCpuMicroarchitectureKrait, YepCpuMicroarchitectureCortexA15, YepCpuMicroarchitectureScorpion, YepCpuMicroarchitectureCortexA9, YepCpuMicroarchitectureCortexA7, YepCpuMicroarchitectureCortexA8, YepCpuMicroarchitectureCortexA5, YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchPJ1[] = { YepCpuMicroarchitecturePJ1, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchPJ4[] = { YepCpuMicroarchitecturePJ4, YepCpuMicroarchitectureCortexA9, YepCpuMicroarchitectureXScale, YepCpuMicroarchitectureARM11, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchSwift[] = { YepCpuMicroarchitectureSwift, YepCpuMicroarchitectureUnknown };
	
	switch (microarchitecture) {
		case YepCpuMicroarchitectureStrongARM:
			return dispatchStrongARM;
		case YepCpuMicroarchitectureXScale:
			return dispatchXScale;
		case YepCpuMicroarchitectureARM7:
			return dispatchARM7;
		case YepCpuMicroarchitectureARM9:
			return dispatchARM9;
		case YepCpuMicroarchitectureARM11:
			return dispatchARM11;
		case YepCpuMicroarchitectureCortexA5:
			return dispatchCortexA5;
		case YepCpuMicroarchitectureCortexA7:
			return dispatchCortexA7;
		case YepCpuMicroarchitectureCortexA8:
			return dispatchCortexA8;
		case YepCpuMicroarchitectureCortexA9:
			return dispatchCortexA9;
		case YepCpuMicroarchitectureCortexA15:
			return dispatchCortexA15;
		case YepCpuMicroarchitectureScorpion:
			return dispatchScorpion;
		case YepCpuMicroarchitectureKrait:
			return dispatchKrait;
		case YepCpuMicroarchitecturePJ1:
			return dispatchPJ1;
		case YepCpuMicroarchitecturePJ4:
			return dispatchPJ4;
		case YepCpuMicroarchitectureSwift:
			return dispatchSwift;
		default:
			return dispatchUnknown;
	}
#elif defined(YEP_MIPS32_ABI)
	static const YepCpuMicroarchitecture dispatchUnknown[] = { YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchMIPS24K[] = { YepCpuMicroarchitectureMIPS24K, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchMIPS34K[] = { YepCpuMicroarchitectureMIPS34K, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchMIPS74K[] = { YepCpuMicroarchitectureMIPS74K, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchXBurst[] = { YepCpuMicroarchitectureXBurst, YepCpuMicroarchitectureUnknown };
	static const YepCpuMicroarchitecture dispatchXBurst2[] = { YepCpuMicroarchitectureXBurst2, YepCpuMicroarchitectureUnknown };
	
	switch (microarchitecture) {
		case YepCpuMicroarchitectureMIPS24K:
			return dispatchMIPS24K;
		case YepCpuMicroarchitectureMIPS34K:
			return dispatchMIPS34K;
		case YepCpuMicroarchitectureMIPS74K:
			return dispatchMIPS74K;
		case YepCpuMicroarchitectureXBurst:
			return dispatchXBurst;
		case YepCpuMicroarchitectureXBurst2:
			return dispatchXBurst2;
		default:
			return dispatchUnknown;
	}
#else
	static const YepCpuMicroarchitecture dispatchUnknown[] = { YepCpuMicroarchitectureUnknown };
	
	return dispatchUnknown;
#endif
}
