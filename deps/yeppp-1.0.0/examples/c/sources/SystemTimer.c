#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <yepLibrary.h>

#define ARRAY_SIZE (1024*1024*16)

/* Compare function for qsort. */
int compare_function(const void* aVoidPointer, const void* bVoidPointer) {
	const Yep32u *aPointer = (const Yep32u*)aVoidPointer;
	const Yep32u *bPointer = (const Yep32u*)bVoidPointer;
	const Yep32u a = *aPointer;
	const Yep32u b = *bPointer;
	if (a > b)
		return 1;
	else if (a < b)
		return -1;
	else
		return 0;
}

int main(int argc, char **argv) {
	enum YepStatus status;
	Yep64u startTime, endTime, time, frequency;
	Yep64f timeSecs;
	Yep32u *array = NULL;
	YepSize i;

	array = (Yep32u*)malloc(ARRAY_SIZE * sizeof(Yep32u));
	assert(array != NULL);

	for (i = 0; i < ARRAY_SIZE; i++) {
		array[i] = rand();
	}

	/* Initialize Yeppp! library */
	status = yepLibrary_Init();
	assert(status == YepStatusOk);

	/* Retrieve the number of timer ticks per second */
	status = yepLibrary_GetTimerFrequency(&frequency);
	assert(status == YepStatusOk);

	/* Retrieve the number of timer ticks before computations */
	status = yepLibrary_GetTimerTicks(&startTime);
	assert(status == YepStatusOk);

	/* Do the computations */
	qsort(array, ARRAY_SIZE, sizeof(Yep32u), &compare_function);

	/* Retrieve the number of timer ticks after computations */
	status = yepLibrary_GetTimerTicks(&endTime);
	assert(status == YepStatusOk);

	/* Compute the length of computations in timer ticks */
	time = endTime - startTime;
	/* To convert the number of timer ticks to seconds we divide them by frequency */
	timeSecs = ((Yep64f)time) / ((Yep64f)frequency);
	printf("Executed in %3.2lf secs\n", (double)timeSecs);

	/* Deinitialize Yeppp! library */
	status = yepLibrary_Release();
	assert(status == YepStatusOk);

	free(array);

	return 0;
}