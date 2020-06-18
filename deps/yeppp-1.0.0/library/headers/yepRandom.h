/*
 *                          Yeppp! library header
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 *
 * Copyright (C) 2010-2012 Marat Dukhan
 * Copyright (C) 2012-2013 Georgia Institute of Technology
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Georgia Institute of Technology nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <yepPredefines.h>
#include <yepTypes.h>

/** @defgroup yepRandom yepRandom.h: random number generators. */

/**
 * @ingroup yepRandom
 * @defgroup yepRandom_WELL1024a	WELL 1024a random number generator
 */

#pragma pack(push, 1)

struct YepRandom_WELL1024a {
	Yep32u state[32];
	Yep32u index;
};

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Initializes the state of WELL1024a pseudo-random number generator with the default seed.
	 * @details	Default seed is fixed, and never changes during execution of a program.
	 * @param[out]	state	Pointer to a WELL1024a random number generator state to be initialized.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	The @a state pointer is null.
	 * @retval	#YepStatusMisalignedPointer	The @a state pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_Init(struct YepRandom_WELL1024a *YEP_RESTRICT state);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Initializes the state of WELL1024a pseudo-random number generator with the specified seed.
	 * @param[out]	state	Pointer to a WELL1024a random number generator state to be initialized.
	 * @param[in]	seed	The 1024-bit initial seed for the random number generator.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a seed pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a seed pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_Init_V32u(struct YepRandom_WELL1024a *YEP_RESTRICT state, const Yep32u seed[32]);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 8-bit samples with WELL1024a pseudo-random number generator.
	 * @details	Each 8-bit number is generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	The @a state pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform__V8u(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep8u *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 16-bit samples with WELL1024a pseudo-random number generator.
	 * @details	Each 16-bit number is generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform__V16u(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep16u *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 32-bit samples with WELL1024a pseudo-random number generator.
	 * @details	Each 32-bit number is generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform__V32u(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep32u *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 64-bit samples with WELL1024a pseudo-random number generator.
	 * @details	Each 64-bit number is generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform__V64u(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep64u *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 8-bit signed integer samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S8sS8s_V8s(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep8s supportMin, Yep8s supportMax, Yep8s *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 16-bit signed integer samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S16sS16s_V16s(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep16s supportMin, Yep16s supportMax, Yep16s *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 32-bit signed integer samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S32sS32s_V32s(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep32s supportMin, Yep32s supportMax, Yep32s *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 64-bit signed integer samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S64sS64s_V64s(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep64s supportMin, Yep64s supportMax, Yep64s *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 8-bit unsigned integer samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S8uS8u_V8u(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep8u supportMin, Yep8u supportMax, Yep8u *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 16-bit unsigned integer samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S16uS16u_V16u(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep16u supportMin, Yep16u supportMax, Yep16u *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 32-bit unsigned integer samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S32uS32u_V32u(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep32u supportMin, Yep32u supportMax, Yep32u *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random 64-bit unsigned integer samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateDiscreteUniform_S64uS64u_V64u(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep64u supportMin, Yep64u supportMax, Yep64u *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random single precision floating-point samples in the specified range with WELL1024a pseudo-random number generator and 32-bit accuracy.
	 * @details	All real numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax, or any of the bounds is not finite.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateUniform_S32fS32f_V32f_Acc32(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep32f supportMin, Yep32f supportMax, Yep32f *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random single precision floating-point samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All floating-point numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax, or any of the bounds is not finite.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateFPUniform_S32fS32f_V32f(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep32f supportMin, Yep32f supportMax, Yep32f *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random double precision floating-point samples in the specified range with WELL1024a pseudo-random number generator and 32-bit accuracy.
	 * @details	All real numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax, or any of the bounds is not finite.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateUniform_S64fS64f_V64f_Acc32(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep64f supportMin, Yep64f supportMax, Yep64f *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random double precision floating-point samples in the specified range with WELL1024a pseudo-random number generator and 64-bit accuracy.
	 * @details	All real numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax, or any of the bounds is not finite.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateUniform_S64fS64f_V64f_Acc64(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep64f supportMin, Yep64f supportMax, Yep64f *YEP_RESTRICT samples, YepSize length);
	/**
	 * @ingroup yepRandom_WELL1024a
	 * @brief	Generates random double precision floating-point samples in the specified range with WELL1024a pseudo-random number generator.
	 * @details	All floating-point numbers between @a supportMin (inclusive) and @a supportMax (inclusive) are generated with the same probability.
	 * @param[in,out]	state	Pointer to the WELL1024a random number generator state.
	 * @param[in]	supportMin	The lower bound (inclusive) of the range of the generated samples.
	 * @param[in]	supportMax	The upper bound (inclusive) of the range of the generated samples.
	 * @param[out]	samples	Pointer to the output array for generated numbers.
	 * @param[in]	length	Length of the output array.
	 * @retval	#YepStatusOk	The random number generator is successfully initialized.
	 * @retval	#YepStatusNullPointer	Either @a state or @a samples pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a state or @a samples pointer is not naturally aligned.
	 * @retval	#YepStatusInvalidArgument	If @a supportMin is not less than @a supportMax, or any of the bounds is not finite.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepRandom_WELL1024a_GenerateFPUniform_S64fS64f_V64f(struct YepRandom_WELL1024a *YEP_RESTRICT state, Yep64f supportMin, Yep64f supportMax, Yep64f *YEP_RESTRICT samples, YepSize length);

#ifdef __cplusplus
}
#endif
