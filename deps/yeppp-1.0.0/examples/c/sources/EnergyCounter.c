#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <yepLibrary.h>

#define ARRAY_SIZE (1024*1024)

int main(int argc, char **argv) {
	enum YepStatus status;
	Yep64f joules, kcal;
	YepSize i;
	struct YepEnergyCounter energyCounter;
	Yep16u array[ARRAY_SIZE];

	/* Initialize the Yeppp! library */
	status = yepLibrary_Init();
	assert(status == YepStatusOk);

	/* Try to initialize energy counter and start energy measurements */
	status = yepLibrary_GetEnergyCounterAcquire(YepEnergyCounterTypeRaplPackageEnergy, &energyCounter);
	if (status == YepStatusOk) {
		for (i = 0; i < ARRAY_SIZE; i++) {
			array[i] = (Yep16u)rand();
		}

		/* Stop energy measurements and release all resources */
		status = yepLibrary_GetEnergyCounterRelease(&energyCounter, &joules);
		assert(status == YepStatusOk);

		/* The output of Yeppp! energy measurements is in Joules (or Watts, if power is measured). Convert them to kcalories */
		kcal = joules / 4.184;
		printf("Just burned %3.2lf kcal\n", (double)(kcal));
	} else {
		/* Something went wrong */
		printf("Could not access RAPL per-package energy counter\n");
	}

	/* Deinitialize the Yeppp! library */
	status = yepLibrary_Release();
	assert(status == YepStatusOk);
	return 0;
}
