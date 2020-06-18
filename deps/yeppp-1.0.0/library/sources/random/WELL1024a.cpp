/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepRandom.h>
#include <yepBuiltin.h>

static const Yep32u defaultSeed[32] = {
	0xB82D98AE, 0x0E383D9B, 0xB029FC23, 0x17EF0277,
	0xAC12E5F8, 0x211DE462, 0x92F753F8, 0x1992D23E,
	0xF80886A4, 0xBAF351BF, 0x414C0DFF, 0xD3471E28,
	0x5F4D98B6, 0x9CBF6187, 0x5A1D3B47, 0xD382697E,
	0x4758A1B0, 0x9E33ECC5, 0x952103FB, 0x3573531D,
	0xA5109863, 0xD3AD17E8, 0x1C71529C, 0xC447A5C2,
	0x78A72138, 0x8FB243E8, 0x76AEEF6F, 0xA217DFF0,
	0x505E0941, 0x7E226F23, 0x9991B643, 0x6A3D0819
};

YepStatus YEPABI yepRandom_WELL1024a_Init(YepRandom_WELL1024a *YEP_RESTRICT rng) {
	return yepRandom_WELL1024a_Init_V32u(rng, &defaultSeed[0]);
}

YepStatus YEPABI yepRandom_WELL1024a_Init_V32u(YepRandom_WELL1024a *YEP_RESTRICT rng, const Yep32u seed[32]) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(seed == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(seed, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	for (YepSize i = 0; i < 32u; i++) {
		rng->state[i] = seed[i];
	}
	rng->index = 0u;
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform__V8u(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep8u *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}

	Yep32u index = rng->index;
	while (length-- != 0) {
		const Yep32u newIndex = (index - 1) % 32u;
		const Yep32u vm1 = rng->state[(index + 3) % 32u];
		const Yep32u vm2 = rng->state[(index + 24) % 32u];
		const Yep32u vm3 = rng->state[(index + 10) % 32u];
		const Yep32u z0 = rng->state[newIndex];
		const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
		const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
		const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
		rng->state[index] = z1 ^ z2;
		rng->state[newIndex] = sample;
		index = newIndex;
		*samples++ = Yep8u(sample ^ (sample >> 8) ^ (sample >> 16) ^ (sample >> 24));
	}
	rng->index = index;
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform__V16u(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep16u *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep16u)) != 0) {
		return YepStatusMisalignedPointer;
	}

	Yep32u index = rng->index;
	while (length-- != 0) {
		const Yep32u newIndex = (index - 1) % 32u;
		const Yep32u vm1 = rng->state[(index + 3) % 32u];
		const Yep32u vm2 = rng->state[(index + 24) % 32u];
		const Yep32u vm3 = rng->state[(index + 10) % 32u];
		const Yep32u z0 = rng->state[newIndex];
		const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
		const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
		const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
		rng->state[index] = z1 ^ z2;
		rng->state[newIndex] = sample;
		index = newIndex;
		*samples++ = Yep16u(sample ^ (sample >> 16));
	}
	rng->index = index;
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform__V32u(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep32u *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}

	Yep32u index = rng->index;
	while (length-- != 0) {
		const Yep32u newIndex = (index - 1) % 32u;
		const Yep32u vm1 = rng->state[(index + 3) % 32u];
		const Yep32u vm2 = rng->state[(index + 24) % 32u];
		const Yep32u vm3 = rng->state[(index + 10) % 32u];
		const Yep32u z0 = rng->state[newIndex];
		const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
		const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
		const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
		rng->state[index] = z1 ^ z2;
		rng->state[newIndex] = sample;
		index = newIndex;
		*samples++ = sample;
	}
	rng->index = index;
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform__V64u(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep64u *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep64u)) != 0) {
		return YepStatusMisalignedPointer;
	}

	Yep32u index = rng->index;
	while (length-- != 0) {
		const Yep32u newIndexLow = (index - 1) % 32u;
		const Yep32u newIndexHigh = (index - 2) % 32u;

		const Yep32u vm1High = rng->state[(index + 2) % 32u];
		const Yep32u vm1Low = rng->state[(index + 3) % 32u];
		const Yep32u vm2High = rng->state[(index + 23) % 32u];
		const Yep32u vm2Low = rng->state[(index + 24) % 32u];
		const Yep32u vm3High = rng->state[(index + 9) % 32u];
		const Yep32u vm3Low = rng->state[(index + 10) % 32u];

		const Yep32u z0High = rng->state[newIndexHigh];
		const Yep32u z0Low = rng->state[newIndexLow];

		const Yep32u z2Low = vm2Low ^ (vm2Low << 19) ^ vm3Low ^ (vm3Low << 14);
		const Yep32u z2High = vm2High ^ (vm2High << 19) ^ vm3High ^ (vm3High << 14);

		const Yep32u z1Low = rng->state[index] ^ vm1Low ^ (vm1Low >> 8);
		const Yep32u sampleLow = z0Low ^ (z0Low << 11) ^ z1Low ^ (z1Low << 7) ^ z2Low ^ (z2Low << 13);

		const Yep32u z1High = sampleLow ^ vm1High ^ (vm1High >> 8);
		const Yep32u sampleHigh = z0High ^ (z0High << 11) ^ z1High ^ (z1High << 7) ^ z2High ^ (z2High << 13);

		rng->state[index] = z1Low ^ z2Low;
		rng->state[newIndexLow] = z1High^ z2High;
		rng->state[newIndexHigh] = sampleHigh;
		index = newIndexHigh;

		const Yep64u sample = yepBuiltin_CombineParts_32u32u_64u(sampleHigh, sampleLow);
		*samples++ = sample;
	}
	rng->index = index;
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S8sS8s_V8s(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep8s supportMin, Yep8s supportMax, Yep8s *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if (supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	const Yep8u supportRange = supportMax - supportMin + 1;
	if YEP_UNLIKELY(Yep8u(supportRange & Yep8u(supportRange - 1)) == 0) {
		/* Support range is a power of 2, probably even 2^8 */
		const Yep8u mask = supportRange - 1;

		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndex = (index - 1) % 32u;
			const Yep32u vm1 = rng->state[(index + 3) % 32u];
			const Yep32u vm2 = rng->state[(index + 24) % 32u];
			const Yep32u vm3 = rng->state[(index + 10) % 32u];
			const Yep32u z0 = rng->state[newIndex];
			const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
			const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
			const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
			rng->state[index] = z1 ^ z2;
			rng->state[newIndex] = sample;
			index = newIndex;
			*samples++ = ((Yep8u(sample) ^ Yep8u(sample >> 8) ^ Yep8u(sample >> 16) ^ Yep8u(sample >> 24)) & mask) + supportMin;
		}
		rng->index = index;
	} else {
		const Yep8u repeats = Yep32u(0x100u - supportRange) / supportRange;
		const Yep8u threshold = repeats * supportRange;

		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep32u sample;
			do {
				const Yep32u newIndex = (index - 1) % 32u;
				const Yep32u vm1 = rng->state[(index + 3) % 32u];
				const Yep32u vm2 = rng->state[(index + 24) % 32u];
				const Yep32u vm3 = rng->state[(index + 10) % 32u];
				const Yep32u z0 = rng->state[newIndex];
				const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
				const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
				sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
				rng->state[index] = z1 ^ z2;
				rng->state[newIndex] = sample;
				index = newIndex;
			} while YEP_UNLIKELY(sample >= threshold);
			*samples++ = Yep8u(Yep8u(Yep8u(sample) ^ Yep8u(sample >> 8) ^ Yep8u(sample >> 16) ^ Yep8u(sample >> 24)) / repeats) + supportMin;
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S16sS16s_V16s(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep16s supportMin, Yep16s supportMax, Yep16s *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep16s)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if (supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	const Yep16u supportRange = supportMax - supportMin + 1;
	if YEP_UNLIKELY(Yep16u(supportRange & Yep16u(supportRange - 1)) == 0) {
		/* Support range is a power of 2, probably even 2^32 */
		const Yep16u mask  = supportRange - 1;

		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndex = (index - 1) % 32u;
			const Yep32u vm1 = rng->state[(index + 3) % 32u];
			const Yep32u vm2 = rng->state[(index + 24) % 32u];
			const Yep32u vm3 = rng->state[(index + 10) % 32u];
			const Yep32u z0 = rng->state[newIndex];
			const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
			const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
			const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
			rng->state[index] = z1 ^ z2;
			rng->state[newIndex] = sample;
			index = newIndex;
			*samples++ = ((Yep16u(sample) ^ Yep16u(sample >> 16)) & mask) + supportMin;
		}
		rng->index = index;
	} else {
		const Yep16u repeats = Yep32u(0x10000u - supportRange) / supportRange;
		const Yep16u threshold = repeats * supportRange;

		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep32u sample;
			do {
				const Yep32u newIndex = (index - 1) % 32u;
				const Yep32u vm1 = rng->state[(index + 3) % 32u];
				const Yep32u vm2 = rng->state[(index + 24) % 32u];
				const Yep32u vm3 = rng->state[(index + 10) % 32u];
				const Yep32u z0 = rng->state[newIndex];
				const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
				const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
				sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
				rng->state[index] = z1 ^ z2;
				rng->state[newIndex] = sample;
				index = newIndex;
			} while YEP_UNLIKELY(sample >= threshold);
			*samples++ = Yep16u(Yep16u(Yep16u(sample) ^ Yep16u(sample >> 16)) / repeats) + supportMin;
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S32sS32s_V32s(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep32s supportMin, Yep32s supportMax, Yep32s *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32s)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep32s)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if (supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	const Yep32u supportRange = supportMax - supportMin + 1;
	if YEP_UNLIKELY((supportRange & (supportRange - 1)) == 0) {
		/* Support range is a power of 2, probably even 2^32 */
		const Yep32u mask  = supportRange - 1;

		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndex = (index - 1) % 32u;
			const Yep32u vm1 = rng->state[(index + 3) % 32u];
			const Yep32u vm2 = rng->state[(index + 24) % 32u];
			const Yep32u vm3 = rng->state[(index + 10) % 32u];
			const Yep32u z0 = rng->state[newIndex];
			const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
			const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
			const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
			rng->state[index] = z1 ^ z2;
			rng->state[newIndex] = sample;
			index = newIndex;
			*samples++ = (sample & mask) + supportMin;
		}
		rng->index = index;
	} else {
		/* Same as Yep32u(0x100000000ull / supportRange) == Yep32u(0x100000000ull - supportRange) / supportRange + 1 */
		const Yep32u repeats = Yep32u(-supportRange) / supportRange + 1u;
		const Yep32u threshold = repeats * supportRange;

		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep32u sample;
			do {
				const Yep32u newIndex = (index - 1) % 32u;
				const Yep32u vm1 = rng->state[(index + 3) % 32u];
				const Yep32u vm2 = rng->state[(index + 24) % 32u];
				const Yep32u vm3 = rng->state[(index + 10) % 32u];
				const Yep32u z0 = rng->state[newIndex];
				const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
				const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
				sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
				rng->state[index] = z1 ^ z2;
				rng->state[newIndex] = sample;
				index = newIndex;
			} while YEP_UNLIKELY(sample >= threshold);
			*samples++ = (sample / repeats) + supportMin;
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S64sS64s_V64s(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep64s supportMin, Yep64s supportMax, Yep64s *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep64s)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if (supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	const Yep64u supportRange = supportMax - supportMin + 1;
	if YEP_UNLIKELY((supportRange & (supportRange - 1)) == 0) {
		/* Support range is a power of 2, probably even 2^32 */
		const Yep32u mask  = supportRange - 1;

		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndexLow = (index - 1) % 32u;
			const Yep32u newIndexHigh = (index - 2) % 32u;

			const Yep32u vm1High = rng->state[(index + 2) % 32u];
			const Yep32u vm1Low = rng->state[(index + 3) % 32u];
			const Yep32u vm2High = rng->state[(index + 23) % 32u];
			const Yep32u vm2Low = rng->state[(index + 24) % 32u];
			const Yep32u vm3High = rng->state[(index + 9) % 32u];
			const Yep32u vm3Low = rng->state[(index + 10) % 32u];

			const Yep32u z0High = rng->state[newIndexHigh];
			const Yep32u z0Low = rng->state[newIndexLow];

			const Yep32u z2Low = vm2Low ^ (vm2Low << 19) ^ vm3Low ^ (vm3Low << 14);
			const Yep32u z2High = vm2High ^ (vm2High << 19) ^ vm3High ^ (vm3High << 14);

			const Yep32u z1Low = rng->state[index] ^ vm1Low ^ (vm1Low >> 8);
			const Yep32u sampleLow = z0Low ^ (z0Low << 11) ^ z1Low ^ (z1Low << 7) ^ z2Low ^ (z2Low << 13);

			const Yep32u z1High = sampleLow ^ vm1High ^ (vm1High >> 8);
			const Yep32u sampleHigh = z0High ^ (z0High << 11) ^ z1High ^ (z1High << 7) ^ z2High ^ (z2High << 13);

			rng->state[index] = z1Low ^ z2Low;
			rng->state[newIndexLow] = z1High^ z2High;
			rng->state[newIndexHigh] = sampleHigh;
			index = newIndexHigh;

			const Yep64u sample = yepBuiltin_CombineParts_32u32u_64u(sampleHigh, sampleLow);
			*samples++ = (sample & mask) + supportMin;
		}
		rng->index = index;
	} else {
		/* Same as Yep64u(0x10000000000000000ull / supportRange) == Yep64u(0x10000000000000000ull - supportRange) / supportRange + 1 */
		const Yep64u repeats = Yep64u(-supportRange) / supportRange + 1ull;
		const Yep64u threshold = repeats * supportRange;

		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep64u sample;
			do {
				const Yep32u newIndexLow = (index - 1) % 32u;
				const Yep32u newIndexHigh = (index - 2) % 32u;

				const Yep32u vm1High = rng->state[(index + 2) % 32u];
				const Yep32u vm1Low = rng->state[(index + 3) % 32u];
				const Yep32u vm2High = rng->state[(index + 23) % 32u];
				const Yep32u vm2Low = rng->state[(index + 24) % 32u];
				const Yep32u vm3High = rng->state[(index + 9) % 32u];
				const Yep32u vm3Low = rng->state[(index + 10) % 32u];

				const Yep32u z0High = rng->state[newIndexHigh];
				const Yep32u z0Low = rng->state[newIndexLow];

				const Yep32u z2Low = vm2Low ^ (vm2Low << 19) ^ vm3Low ^ (vm3Low << 14);
				const Yep32u z2High = vm2High ^ (vm2High << 19) ^ vm3High ^ (vm3High << 14);

				const Yep32u z1Low = rng->state[index] ^ vm1Low ^ (vm1Low >> 8);
				const Yep32u sampleLow = z0Low ^ (z0Low << 11) ^ z1Low ^ (z1Low << 7) ^ z2Low ^ (z2Low << 13);

				const Yep32u z1High = sampleLow ^ vm1High ^ (vm1High >> 8);
				const Yep32u sampleHigh = z0High ^ (z0High << 11) ^ z1High ^ (z1High << 7) ^ z2High ^ (z2High << 13);

				rng->state[index] = z1Low ^ z2Low;
				rng->state[newIndexLow] = z1High^ z2High;
				rng->state[newIndexHigh] = sampleHigh;
				index = newIndexHigh;

				sample = yepBuiltin_CombineParts_32u32u_64u(sampleHigh, sampleLow);
			} while YEP_UNLIKELY(sample >= threshold);
			*samples++ = (sample / repeats) + supportMin;
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S8uS8u_V8u(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep8u supportMin, Yep8u supportMax, Yep8u *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if (supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	const Yep8u supportRange = supportMax - supportMin + 1;
	if YEP_UNLIKELY(Yep8u(supportRange & Yep8u(supportRange - 1)) == 0) {
		/* Support range is a power of 2, probably even 2^8 */
		const Yep8u mask = supportRange - 1;

		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndex = (index - 1) % 32u;
			const Yep32u vm1 = rng->state[(index + 3) % 32u];
			const Yep32u vm2 = rng->state[(index + 24) % 32u];
			const Yep32u vm3 = rng->state[(index + 10) % 32u];
			const Yep32u z0 = rng->state[newIndex];
			const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
			const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
			const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
			rng->state[index] = z1 ^ z2;
			rng->state[newIndex] = sample;
			index = newIndex;
			*samples++ = ((Yep8u(sample) ^ Yep8u(sample >> 8) ^ Yep8u(sample >> 16) ^ Yep8u(sample >> 24)) & mask) + supportMin;
		}
		rng->index = index;
	} else {
		const Yep8u repeats = Yep32u(0x100u - supportRange) / supportRange;
		const Yep8u threshold = repeats * supportRange;

		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep32u sample;
			do {
				const Yep32u newIndex = (index - 1) % 32u;
				const Yep32u vm1 = rng->state[(index + 3) % 32u];
				const Yep32u vm2 = rng->state[(index + 24) % 32u];
				const Yep32u vm3 = rng->state[(index + 10) % 32u];
				const Yep32u z0 = rng->state[newIndex];
				const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
				const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
				sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
				rng->state[index] = z1 ^ z2;
				rng->state[newIndex] = sample;
				index = newIndex;
			} while YEP_UNLIKELY(sample >= threshold);
			*samples++ = Yep8u(Yep8u(Yep8u(sample) ^ Yep8u(sample >> 8) ^ Yep8u(sample >> 16) ^ Yep8u(sample >> 24)) / repeats) + supportMin;
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S16uS16u_V16u(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep16u supportMin, Yep16u supportMax, Yep16u *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep16u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if (supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	const Yep16u supportRange = supportMax - supportMin + 1;
	if YEP_UNLIKELY(Yep16u(supportRange & Yep16u(supportRange - 1)) == 0) {
		/* Support range is a power of 2, probably even 2^32 */
		const Yep16u mask  = supportRange - 1;

		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndex = (index - 1) % 32u;
			const Yep32u vm1 = rng->state[(index + 3) % 32u];
			const Yep32u vm2 = rng->state[(index + 24) % 32u];
			const Yep32u vm3 = rng->state[(index + 10) % 32u];
			const Yep32u z0 = rng->state[newIndex];
			const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
			const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
			const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
			rng->state[index] = z1 ^ z2;
			rng->state[newIndex] = sample;
			index = newIndex;
			*samples++ = ((Yep16u(sample) ^ Yep16u(sample >> 16)) & mask) + supportMin;
		}
		rng->index = index;
	} else {
		const Yep16u repeats = Yep32u(0x10000u - supportRange) / supportRange;
		const Yep16u threshold = repeats * supportRange;

		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep32u sample;
			do {
				const Yep32u newIndex = (index - 1) % 32u;
				const Yep32u vm1 = rng->state[(index + 3) % 32u];
				const Yep32u vm2 = rng->state[(index + 24) % 32u];
				const Yep32u vm3 = rng->state[(index + 10) % 32u];
				const Yep32u z0 = rng->state[newIndex];
				const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
				const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
				sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
				rng->state[index] = z1 ^ z2;
				rng->state[newIndex] = sample;
				index = newIndex;
			} while YEP_UNLIKELY(sample >= threshold);
			*samples++ = Yep16u(Yep16u(Yep16u(sample) ^ Yep16u(sample >> 16)) / repeats) + supportMin;
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S32uS32u_V32u(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep32u supportMin, Yep32u supportMax, Yep32u *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if (supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	const Yep32u supportRange = supportMax - supportMin + 1;
	if YEP_UNLIKELY((supportRange & (supportRange - 1)) == 0) {
		/* Support range is a power of 2, probably even 2^32 */
		const Yep32u mask  = supportRange - 1;

		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndex = (index - 1) % 32u;
			const Yep32u vm1 = rng->state[(index + 3) % 32u];
			const Yep32u vm2 = rng->state[(index + 24) % 32u];
			const Yep32u vm3 = rng->state[(index + 10) % 32u];
			const Yep32u z0 = rng->state[newIndex];
			const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
			const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
			const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
			rng->state[index] = z1 ^ z2;
			rng->state[newIndex] = sample;
			index = newIndex;
			*samples++ = (sample & mask) + supportMin;
		}
		rng->index = index;
	} else {
		/* Same as Yep32u(0x100000000ull / supportRange) == Yep32u(0x100000000ull - supportRange) / supportRange + 1 */
		const Yep32u repeats = Yep32u(-supportRange) / supportRange + 1u;
		const Yep32u threshold = repeats * supportRange;

		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep32u sample;
			do {
				const Yep32u newIndex = (index - 1) % 32u;
				const Yep32u vm1 = rng->state[(index + 3) % 32u];
				const Yep32u vm2 = rng->state[(index + 24) % 32u];
				const Yep32u vm3 = rng->state[(index + 10) % 32u];
				const Yep32u z0 = rng->state[newIndex];
				const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
				const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
				sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
				rng->state[index] = z1 ^ z2;
				rng->state[newIndex] = sample;
				index = newIndex;
			} while YEP_UNLIKELY(sample >= threshold);
			*samples++ = (sample / repeats) + supportMin;
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S64uS64u_V64u(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep64u supportMin, Yep64u supportMax, Yep64u *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep64u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if (supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	const Yep64u supportRange = supportMax - supportMin + 1;
	if YEP_UNLIKELY((supportRange & (supportRange - 1)) == 0) {
		/* Support range is a power of 2, probably even 2^32 */
		const Yep32u mask  = supportRange - 1;

		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndexLow = (index - 1) % 32u;
			const Yep32u newIndexHigh = (index - 2) % 32u;

			const Yep32u vm1High = rng->state[(index + 2) % 32u];
			const Yep32u vm1Low = rng->state[(index + 3) % 32u];
			const Yep32u vm2High = rng->state[(index + 23) % 32u];
			const Yep32u vm2Low = rng->state[(index + 24) % 32u];
			const Yep32u vm3High = rng->state[(index + 9) % 32u];
			const Yep32u vm3Low = rng->state[(index + 10) % 32u];

			const Yep32u z0High = rng->state[newIndexHigh];
			const Yep32u z0Low = rng->state[newIndexLow];

			const Yep32u z2Low = vm2Low ^ (vm2Low << 19) ^ vm3Low ^ (vm3Low << 14);
			const Yep32u z2High = vm2High ^ (vm2High << 19) ^ vm3High ^ (vm3High << 14);

			const Yep32u z1Low = rng->state[index] ^ vm1Low ^ (vm1Low >> 8);
			const Yep32u sampleLow = z0Low ^ (z0Low << 11) ^ z1Low ^ (z1Low << 7) ^ z2Low ^ (z2Low << 13);

			const Yep32u z1High = sampleLow ^ vm1High ^ (vm1High >> 8);
			const Yep32u sampleHigh = z0High ^ (z0High << 11) ^ z1High ^ (z1High << 7) ^ z2High ^ (z2High << 13);

			rng->state[index] = z1Low ^ z2Low;
			rng->state[newIndexLow] = z1High^ z2High;
			rng->state[newIndexHigh] = sampleHigh;
			index = newIndexHigh;

			const Yep64u sample = yepBuiltin_CombineParts_32u32u_64u(sampleHigh, sampleLow);
			*samples++ = (sample & mask) + supportMin;
		}
		rng->index = index;
	} else {
		/* Same as Yep64u(0x10000000000000000ull / supportRange) == Yep64u(0x10000000000000000ull - supportRange) / supportRange + 1 */
		const Yep64u repeats = Yep64u(-supportRange) / supportRange + 1ull;
		const Yep64u threshold = repeats * supportRange;

		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep64u sample;
			do {
				const Yep32u newIndexLow = (index - 1) % 32u;
				const Yep32u newIndexHigh = (index - 2) % 32u;

				const Yep32u vm1High = rng->state[(index + 2) % 32u];
				const Yep32u vm1Low = rng->state[(index + 3) % 32u];
				const Yep32u vm2High = rng->state[(index + 23) % 32u];
				const Yep32u vm2Low = rng->state[(index + 24) % 32u];
				const Yep32u vm3High = rng->state[(index + 9) % 32u];
				const Yep32u vm3Low = rng->state[(index + 10) % 32u];

				const Yep32u z0High = rng->state[newIndexHigh];
				const Yep32u z0Low = rng->state[newIndexLow];

				const Yep32u z2Low = vm2Low ^ (vm2Low << 19) ^ vm3Low ^ (vm3Low << 14);
				const Yep32u z2High = vm2High ^ (vm2High << 19) ^ vm3High ^ (vm3High << 14);

				const Yep32u z1Low = rng->state[index] ^ vm1Low ^ (vm1Low >> 8);
				const Yep32u sampleLow = z0Low ^ (z0Low << 11) ^ z1Low ^ (z1Low << 7) ^ z2Low ^ (z2Low << 13);

				const Yep32u z1High = sampleLow ^ vm1High ^ (vm1High >> 8);
				const Yep32u sampleHigh = z0High ^ (z0High << 11) ^ z1High ^ (z1High << 7) ^ z2High ^ (z2High << 13);

				rng->state[index] = z1Low ^ z2Low;
				rng->state[newIndexLow] = z1High^ z2High;
				rng->state[newIndexHigh] = sampleHigh;
				index = newIndexHigh;

				sample = yepBuiltin_CombineParts_32u32u_64u(sampleHigh, sampleLow);
			} while YEP_UNLIKELY(sample >= threshold);
			*samples++ = (sample / repeats) + supportMin;
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateUniform_S32fS32f_V32f_Acc32(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep32f supportMin, Yep32f supportMax, Yep32f *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep32f)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMax < yepBuiltin_PositiveInfinity_32f())) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMin > yepBuiltin_NegativeInfinity_32f())) {
		return YepStatusInvalidArgument;
	}
	const Yep32f range = supportMax - supportMin;
	#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
		const Yep32f scale = 0x1.000000p-32f;
	#else
		const Yep32f scale = 2.3283064365386962890625e-10f;
	#endif
	const Yep32f scaledRange = range * scale;
	Yep32u index = rng->index;
	while (length-- != 0) {
		const Yep32u newIndex = (index - 1) % 32u;
		const Yep32u vm1 = rng->state[(index + 3) % 32u];
		const Yep32u vm2 = rng->state[(index + 24) % 32u];
		const Yep32u vm3 = rng->state[(index + 10) % 32u];
		const Yep32u z0 = rng->state[newIndex];
		const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
		const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
		const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
		rng->state[index] = z1 ^ z2;
		rng->state[newIndex] = sample;
		index = newIndex;
		*samples++ = yepBuiltin_Convert_32u_32f(sample) * scaledRange + supportMin;
	}
	rng->index = index;
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateFPUniform_S32fS32f_V32f(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep32f supportMin, Yep32f supportMax, Yep32f *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep32f)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMax < yepBuiltin_PositiveInfinity_32f())) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMin > yepBuiltin_NegativeInfinity_32f())) {
		return YepStatusInvalidArgument;
	}
	const Yep32u xMin = yepBuiltin_Map_32f_32u(supportMin);
	const Yep32u xMax = yepBuiltin_Map_32f_32u(supportMax);
	const Yep32u xRange = xMax - xMin + 1u;
	if YEP_UNLIKELY((xRange & (xRange - 1u)) == 0u) {
		/* xRange is a power of 2 */
		const Yep32u xMask = xRange - 1u;
		
		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndex = (index - 1) % 32u;
			const Yep32u vm1 = rng->state[(index + 3) % 32u];
			const Yep32u vm2 = rng->state[(index + 24) % 32u];
			const Yep32u vm3 = rng->state[(index + 10) % 32u];
			const Yep32u z0 = rng->state[newIndex];
			const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
			const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
			const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
			rng->state[index] = z1 ^ z2;
			rng->state[newIndex] = sample;
			index = newIndex;
			
			const Yep32u adjustedSample = (sample & xMask) + xMin;
			*samples++ = yepBuiltin_Map_32u_32f(adjustedSample);
		}
		rng->index = index;
	} else {
		/* Same as Yep32u(0x100000000ull / xRange) == Yep32u(0x100000000ull - xRange) / xRange + 1 */
		const Yep32u repeats = Yep32u(-xRange) / xRange + 1u;
		const Yep32u threshold = repeats * xRange;
		
		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep32u sample;
			do {
				const Yep32u newIndex = (index - 1) % 32u;
				const Yep32u vm1 = rng->state[(index + 3) % 32u];
				const Yep32u vm2 = rng->state[(index + 24) % 32u];
				const Yep32u vm3 = rng->state[(index + 10) % 32u];
				const Yep32u z0 = rng->state[newIndex];
				const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
				const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
				sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
				rng->state[index] = z1 ^ z2;
				rng->state[newIndex] = sample;
				index = newIndex;
			} while (sample >= threshold);
			
			const Yep32u adjustedSample = (sample / repeats) + xMin;
			*samples++ = yepBuiltin_Map_32u_32f(adjustedSample);
		}
		rng->index = index;
	}
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateUniform_S64fS64f_V64f_Acc32(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep64f supportMin, Yep64f supportMax, Yep64f *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep64f)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMax < yepBuiltin_PositiveInfinity_64f())) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMin > yepBuiltin_NegativeInfinity_64f())) {
		return YepStatusInvalidArgument;
	}
	const Yep64f range = supportMax - supportMin;
	#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
		const Yep64f scale = 0x1.0000000000000p-32;
	#else
		const Yep64f scale = 2.3283064365386962890625e-10;
	#endif
	const Yep64f scaledRange = range * scale;
	Yep32u index = rng->index;
	while (length-- != 0) {
		const Yep32u newIndex = (index - 1) % 32u;
		const Yep32u vm1 = rng->state[(index + 3) % 32u];
		const Yep32u vm2 = rng->state[(index + 24) % 32u];
		const Yep32u vm3 = rng->state[(index + 10) % 32u];
		const Yep32u z0 = rng->state[newIndex];
		const Yep32u z1 = rng->state[index] ^ vm1 ^ (vm1 >> 8);
		const Yep32u z2 = vm2 ^ (vm2 << 19) ^ vm3 ^ (vm3 << 14);
		const Yep32u sample = z0 ^ (z0 << 11) ^ z1 ^ (z1 << 7) ^ z2 ^ (z2 << 13);
		rng->state[index] = z1 ^ z2;
		rng->state[newIndex] = sample;
		index = newIndex;
		*samples++ = yepBuiltin_Convert_32u_64f(sample) * scaledRange + supportMin;
	}
	rng->index = index;
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateUniform_S64fS64f_V64f_Acc64(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep64f supportMin, Yep64f supportMax, Yep64f *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep64f)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMax < yepBuiltin_PositiveInfinity_64f())) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMin > yepBuiltin_NegativeInfinity_64f())) {
		return YepStatusInvalidArgument;
	}
	const Yep64f range = supportMax - supportMin;
	#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
		const Yep64f scale = 0x1.0000000000000p-64;
	#else
		const Yep64f scale = 5.42101086242752217003726400434970855712890625e-20;
	#endif
	const Yep64f scaledRange = range * scale;
	Yep32u index = rng->index;
	while (length-- != 0) {
		const Yep32u newIndexLow = (index - 1) % 32u;
		const Yep32u newIndexHigh = (index - 2) % 32u;

		const Yep32u vm1High = rng->state[(index + 2) % 32u];
		const Yep32u vm1Low = rng->state[(index + 3) % 32u];
		const Yep32u vm2High = rng->state[(index + 23) % 32u];
		const Yep32u vm2Low = rng->state[(index + 24) % 32u];
		const Yep32u vm3High = rng->state[(index + 9) % 32u];
		const Yep32u vm3Low = rng->state[(index + 10) % 32u];

		const Yep32u z0High = rng->state[newIndexHigh];
		const Yep32u z0Low = rng->state[newIndexLow];

		const Yep32u z2Low = vm2Low ^ (vm2Low << 19) ^ vm3Low ^ (vm3Low << 14);
		const Yep32u z2High = vm2High ^ (vm2High << 19) ^ vm3High ^ (vm3High << 14);

		const Yep32u z1Low = rng->state[index] ^ vm1Low ^ (vm1Low >> 8);
		const Yep32u sampleLow = z0Low ^ (z0Low << 11) ^ z1Low ^ (z1Low << 7) ^ z2Low ^ (z2Low << 13);

		const Yep32u z1High = sampleLow ^ vm1High ^ (vm1High >> 8);
		const Yep32u sampleHigh = z0High ^ (z0High << 11) ^ z1High ^ (z1High << 7) ^ z2High ^ (z2High << 13);

		rng->state[index] = z1Low ^ z2Low;
		rng->state[newIndexLow] = z1High^ z2High;
		rng->state[newIndexHigh] = sampleHigh;
		index = newIndexHigh;

		const Yep64u sample = yepBuiltin_CombineParts_32u32u_64u(sampleHigh, sampleLow);

		*samples++ = yepBuiltin_Convert_64u_64f(sample) * scaledRange + supportMin;
	}
	rng->index = index;
	return YepStatusOk;
}

YepStatus YEPABI yepRandom_WELL1024a_GenerateFPUniform_S64fS64f_V64f(YepRandom_WELL1024a *YEP_RESTRICT rng, Yep64f supportMin, Yep64f supportMax, Yep64f *YEP_RESTRICT samples, YepSize length) {
	if YEP_UNLIKELY(rng == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(samples == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(rng, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(samples, sizeof(Yep64f)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(supportMin >= supportMax) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMax < yepBuiltin_PositiveInfinity_64f())) {
		return YepStatusInvalidArgument;
	}
	if YEP_UNLIKELY(!(supportMin > yepBuiltin_NegativeInfinity_64f())) {
		return YepStatusInvalidArgument;
	}
	const Yep64u xMin = yepBuiltin_Map_64f_64u(supportMin);
	const Yep64u xMax = yepBuiltin_Map_64f_64u(supportMax);
	const Yep64u xRange = xMax - xMin + 1ull;
	if YEP_UNLIKELY((xRange & (xRange - 1ull)) == 0ull) {
		/* xRange is a power of 2 */
		const Yep64u xMask = xRange - 1ull;
		
		Yep32u index = rng->index;
		while (length-- != 0) {
			const Yep32u newIndexLow = (index - 1) % 32u;
			const Yep32u newIndexHigh = (index - 2) % 32u;

			const Yep32u vm1High = rng->state[(index + 2) % 32u];
			const Yep32u vm1Low = rng->state[(index + 3) % 32u];
			const Yep32u vm2High = rng->state[(index + 23) % 32u];
			const Yep32u vm2Low = rng->state[(index + 24) % 32u];
			const Yep32u vm3High = rng->state[(index + 9) % 32u];
			const Yep32u vm3Low = rng->state[(index + 10) % 32u];

			const Yep32u z0High = rng->state[newIndexHigh];
			const Yep32u z0Low = rng->state[newIndexLow];

			const Yep32u z2Low = vm2Low ^ (vm2Low << 19) ^ vm3Low ^ (vm3Low << 14);
			const Yep32u z2High = vm2High ^ (vm2High << 19) ^ vm3High ^ (vm3High << 14);

			const Yep32u z1Low = rng->state[index] ^ vm1Low ^ (vm1Low >> 8);
			const Yep32u sampleLow = z0Low ^ (z0Low << 11) ^ z1Low ^ (z1Low << 7) ^ z2Low ^ (z2Low << 13);

			const Yep32u z1High = sampleLow ^ vm1High ^ (vm1High >> 8);
			const Yep32u sampleHigh = z0High ^ (z0High << 11) ^ z1High ^ (z1High << 7) ^ z2High ^ (z2High << 13);

			rng->state[index] = z1Low ^ z2Low;
			rng->state[newIndexLow] = z1High^ z2High;
			rng->state[newIndexHigh] = sampleHigh;
			index = newIndexHigh;
			
			const Yep64u sample = yepBuiltin_CombineParts_32u32u_64u(sampleHigh, sampleLow);
			
			const Yep64u adjustedSample = (sample & xMask) + xMin;
			*samples++ = yepBuiltin_Map_64u_64f(adjustedSample);
		}
		rng->index = index;
	} else {
		/* Same as Yep64u(0x10000000000000000ulll / xRange) == Yep64u(0x10000000000000000ulll - xRange) / xRange + 1 */
		const Yep64u repeats = Yep64u(-xRange) / xRange + 1ull;
		const Yep64u threshold = repeats * xRange;
		
		Yep32u index = rng->index;
		while (length-- != 0) {
			Yep64u sample;
			do {
				const Yep32u newIndexLow = (index - 1) % 32u;
				const Yep32u newIndexHigh = (index - 2) % 32u;

				const Yep32u vm1High = rng->state[(index + 2) % 32u];
				const Yep32u vm1Low = rng->state[(index + 3) % 32u];
				const Yep32u vm2High = rng->state[(index + 23) % 32u];
				const Yep32u vm2Low = rng->state[(index + 24) % 32u];
				const Yep32u vm3High = rng->state[(index + 9) % 32u];
				const Yep32u vm3Low = rng->state[(index + 10) % 32u];

				const Yep32u z0High = rng->state[newIndexHigh];
				const Yep32u z0Low = rng->state[newIndexLow];

				const Yep32u z2Low = vm2Low ^ (vm2Low << 19) ^ vm3Low ^ (vm3Low << 14);
				const Yep32u z2High = vm2High ^ (vm2High << 19) ^ vm3High ^ (vm3High << 14);

				const Yep32u z1Low = rng->state[index] ^ vm1Low ^ (vm1Low >> 8);
				const Yep32u sampleLow = z0Low ^ (z0Low << 11) ^ z1Low ^ (z1Low << 7) ^ z2Low ^ (z2Low << 13);

				const Yep32u z1High = sampleLow ^ vm1High ^ (vm1High >> 8);
				const Yep32u sampleHigh = z0High ^ (z0High << 11) ^ z1High ^ (z1High << 7) ^ z2High ^ (z2High << 13);

				rng->state[index] = z1Low ^ z2Low;
				rng->state[newIndexLow] = z1High^ z2High;
				rng->state[newIndexHigh] = sampleHigh;
				index = newIndexHigh;
				
				sample = yepBuiltin_CombineParts_32u32u_64u(sampleHigh, sampleLow);
			} while (sample >= threshold);
			
			const Yep64u adjustedSample = (sample / repeats) + xMin;
			*samples++ = yepBuiltin_Map_64u_64f(adjustedSample);
		}
		rng->index = index;
	}
	return YepStatusOk;
}
