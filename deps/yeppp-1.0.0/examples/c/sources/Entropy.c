#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <yepCore.h>
#include <yepMath.h>
#include <yepLibrary.h>

/* The naive implementation of entropy computation using log function for LibM */
double compute_entropy_naive(const double *pPointer, size_t pLength) {
	double entropy = 0.0;
	for (; pLength != 0; pLength--) {
		const double p = *pPointer++;
		entropy -= p * log(p);
	}
	return entropy;
}

/* The implementation of entropy computation using vector log and dot-product functions from Yeppp! library */
/* To avoid allocating a large array for logarithms (and also to benefit from cache locality) the logarithms are computed on small blocks of the input array */
/* The size of the block used to compute the logarithm */
#define BLOCK_SIZE 1024
double compute_entropy_yeppp(const double *p, size_t length) {
	enum YepStatus status;
	Yep64f entropy = 0.0;
	/* The small array for computed logarithms of the part of the input array */
	YEP_ALIGN(64) Yep64f logP[BLOCK_SIZE];
	YepSize index, blockLength;
	
	for (index = 0; index < length; index += BLOCK_SIZE) {
		Yep64f dotProduct;
		
		/* Process min(BLOCK_SIZE, number of remaining elements) elements of the input array */
		blockLength = length - index;
		if (blockLength > BLOCK_SIZE) {
			blockLength = BLOCK_SIZE;
		}
		
		/* Compute logarithms of probabilities in the current block of the input array */
		status = yepMath_Log_V64f_V64f(p + index, logP, blockLength);
		assert(status == YepStatusOk);
		
		/* Compute the dot product of probabilities and log-probabilities of the current block */
		/* This will give minus entropy of the current block */
		status = yepCore_DotProduct_V64fV64f_S64f(p + index, logP, &dotProduct, blockLength);
		assert(status == YepStatusOk);
		
		/* Compute entropy of the current block and subtract it from the current entropy value */
		entropy -= dotProduct;
	}
	return entropy;
}

#define ARRAY_SIZE (1024*1024*16)

int main(int argc, char **argv) {
	enum YepStatus status;
	Yep64u startTimeNaive, startTimeYeppp, endTimeNaive, endTimeYeppp, frequency;
	Yep64f entropyNaive, entropyYeppp;
	YepSize i;
	
	/* Allocate an array of probabilities */
	Yep64f *p = (Yep64f*)calloc(ARRAY_SIZE, sizeof(Yep64f));

	/* Initialize the Yeppp! library */
	status = yepLibrary_Init();
	assert(status == YepStatusOk);

	/* Populate the array of probabilities with random probabilities */
	for (i = 0; i < ARRAY_SIZE; i++) {
		/* 0 < p[i] <= 1.0 */
		p[i] = ((double)(rand() + 1)) / ((double)(RAND_MAX) + 1.0);
	}

	/* Retrieve the number of timer ticks per second */
	status = yepLibrary_GetTimerFrequency(&frequency);
	assert(status == YepStatusOk);

	/* Retrieve the number of timer ticks before calling naive entropy computation */
	status = yepLibrary_GetTimerTicks(&startTimeNaive);
	assert(status == YepStatusOk);

	/* Compute entropy using naive implementation */
	entropyNaive = compute_entropy_naive(p, ARRAY_SIZE);

	/* Retrieve the number of timer ticks after calling naive entropy computation */
	status = yepLibrary_GetTimerTicks(&endTimeNaive);
	assert(status == YepStatusOk);

	/* Retrieve the number of timer ticks before calling Yeppp!-based entropy computation */
	status = yepLibrary_GetTimerTicks(&startTimeYeppp);
	assert(status == YepStatusOk);

	/* Compute entropy using Yeppp!-based implementation */
	entropyYeppp = compute_entropy_yeppp(p, ARRAY_SIZE);

	/* Retrieve the number of timer ticks after calling Yeppp!-based entropy computation */
	status = yepLibrary_GetTimerTicks(&endTimeYeppp);
	assert(status == YepStatusOk);

	/* Report the results */
	printf("Naive implementation:\n");
	printf("\tEntropy = %lf\n", ((double)entropyNaive));
	printf("\tTime = %lf\n", ((double)(endTimeNaive - startTimeNaive)) / ((double)(frequency)));
	printf("Yeppp! implementation:\n");
	printf("\tEntropy = %lf\n", ((double)entropyYeppp));
	printf("\tTime = %lf\n", ((double)(endTimeYeppp - startTimeYeppp)) / ((double)(frequency)));
	
	/* Deinitialize the Yeppp! library */
	status = yepLibrary_Release();
	assert(status == YepStatusOk);

	/* Release the memory for probabilities array */
	free(p);

	return 0;
}
