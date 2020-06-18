#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <yepLibrary.h>

#define ARRAY_SIZE (1024*4)

int main(int argc, char **argv) {
	enum YepStatus status;
	Yep64u systemFeatures, state, cycles, minOverhead, minCycles;
	YepSize i, iteration, iterationMax = 1000;
	Yep16u array[ARRAY_SIZE];
	Yep64f CPE;

	/* Initialize the Yeppp! library */
	status = yepLibrary_Init();
	assert(status == YepStatusOk);

	/* Retrieve system capabilities info */
	status = yepLibrary_GetCpuSystemFeatures(&systemFeatures);
	assert(status == YepStatusOk);

	/* Check if the system supports performance counters */
	if (systemFeatures & YepSystemFeatureCycleCounter) {
		/* Estimate the measurement overhead */
		minOverhead = (Yep64u)(-1);
		/* Repeat many times and take the minimum to filter out noise from interrupts and caches/branch prediction/page faults */
		for (iteration = 0; iteration < iterationMax; iteration++) {
			status = yepLibrary_GetCpuCyclesAcquire(&state);
			assert(status == YepStatusOk);

			status = yepLibrary_GetCpuCyclesRelease(&state, &cycles);
			assert(status == YepStatusOk);
			if (cycles < minOverhead)
				minOverhead = cycles;
		}

		/* Now measure the cycles for computation */
		minCycles = (Yep64u)(-1);
		/* Repeat many times and take the minimum to filter out noise from interrupts and caches/branch prediction/page faults */
		for (iteration = 0; iteration < iterationMax; iteration++) {
			status = yepLibrary_GetCpuCyclesAcquire(&state);
			assert(status == YepStatusOk);

			for (i = 0; i < ARRAY_SIZE; i++) {
				array[i] = (Yep16u)rand();
			}

			status = yepLibrary_GetCpuCyclesRelease(&state, &cycles);
			assert(status == YepStatusOk);

			if (cycles < minCycles)
				minCycles = cycles;
		}
		/* Subtract the overhead and normalize by the number of elements */
		CPE = ((Yep64f)(minCycles - minOverhead)) / ((Yep64f)ARRAY_SIZE);
		printf("Cycles per element: %3.2lf\n", (double)(CPE));
	} else {
		printf("Processor cycle counter is not supported\n");
	}
	
	/* Deinitialize the Yeppp! library */
	status = yepLibrary_Release();
	assert(status == YepStatusOk);
	return 0;
}
