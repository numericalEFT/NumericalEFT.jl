/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under 2-clause BSD license.
 * See LICENSE.txt for details.
 *
 */

pragma(lib, "yeppp-d.lib");
pragma(lib, "yeppp.lib");

import yeppp.types;
import yeppp.library;
import yeppp.core;
import yeppp.math;

static import std.stdio;
static import std.conv;
static import std.random;
static import std.algorithm;
static import std.math;

/* The naive implementation of entropy computation using log function for LibM */
double computeEntropyNaive(const double[] probabilities) {
	double entropy = 0.0;
	/** $\mathbb{H}\left[P\right] = - \sum_{i = 0}^{n} p_{i}\cdot\log\left(p_{i}\right) $ */
	foreach (double p; probabilities) {
		/* Compute $p_{i}\cdot\log\left(p_{i}\right)$ and subtract it from the current entropy value */
		entropy -= p * std.math.log(p);
	}
	return entropy;
}

/* The implementation of entropy computation using vector log and dot-product functions from Yeppp! library */
/* To avoid allocating a large array for logarithms (and also to benefit from cache locality) the logarithms are computed on small blocks of the input array */
/* The size of the block used to compute the logarithm */
double computeEntropyYeppp(const double[] p) {
	double entropy = 0.0L;
	const size_t blockSize = 1024;
	std.stdio.write(&p[0]);
	/* The small array for computed logarithms of the part of the input array */
	double[blockSize] logP;

	/** $\mathbb{H}\left[P\right] = - \sum_{i = 0}^{n} p_{i}\cdot\log\left(p_{i}\right) $ */
	for (size_t index = 0; index < p.length; index += blockSize) {
		/* Process min(BLOCK_SIZE, number of remaining elements) elements of the input array */
		size_t blockLength = std.algorithm.min(blockSize, p.length - index);

		/* Compute logarithms of probabilities in the current block of the input array */
		Status status = yepMath_Log_V64f_V64f(&p[index], &logP[0], blockLength);
		std.stdio.writef("status = %u", status);
		assert(status == Status.Ok);

		/* Compute the dot product of probabilities and log-probabilities of the current block */
		/* This will give minus entropy of the current block */
		double dotProduct = 1.0L;
		status = yepCore_DotProduct_V64fV64f_S64f(&p[index], &logP[0], dotProduct, 1024);
		assert(status == Status.Ok);

		/* Compute entropy of the current block and subtract it from the current entropy value */
		entropy -= dotProduct;
	}
	return entropy;
}

int main(string[] args) {
	Status status;
	ulong state;
	ulong cycles;
	status = yepLibrary_Init();
	assert(status == Status.Ok);

	const size_t arraySize = 1024*1024;

	/* Allocate an array of probabilities */
	double[] p = new double[arraySize];

	/* Populate the array of probabilities with random probabilities */
	for (size_t i = 0; i < arraySize; i++) {
		/* 0 < p[i] <= 1.0 */
		p[i] = 1.0 - std.random.uniform(0.0L, 1.0L);
	}

	/* Retrieve the number of timer ticks per second */
	ulong frequency;
	yepLibrary_GetTimerFrequency(frequency);

	/* Retrieve the number of timer ticks before calling naive entropy computation */
	ulong startTimeNaive;
	yepLibrary_GetTimerTicks(startTimeNaive);
	
	const double entropyNaive = computeEntropyNaive(p);
	
	/* Retrieve the number of timer ticks after calling naive entropy computation */
	ulong endTimeNaive;
	yepLibrary_GetTimerTicks(endTimeNaive);
	
	/* Retrieve the number of timer ticks before calling naive entropy computation */
	ulong startTimeYeppp;
	yepLibrary_GetTimerTicks(startTimeYeppp);
	
	const double entropyYeppp = computeEntropyYeppp(p);
	
	/* Retrieve the number of timer ticks after calling naive entropy computation */
	ulong endTimeYeppp;
	yepLibrary_GetTimerTicks(endTimeYeppp);

	status = yepLibrary_Release();
	assert(status == Status.Ok);

	/* Report the results */
	std.stdio.writeln("Naive implementation:");
	std.stdio.writefln("\tEntropy = %f", entropyNaive);
	std.stdio.writefln("\tTime = %.3f secs", (cast (double) (endTimeNaive - startTimeNaive)) / (cast (double) frequency));
	std.stdio.writeln("Yeppp! implementation:");
	std.stdio.writefln("\tEntropy = %f", entropyYeppp);
	std.stdio.writefln("\tTime = %.3f secs", (cast (double) (endTimeYeppp - startTimeYeppp)) / (cast (double) frequency));

	return 0;
}
