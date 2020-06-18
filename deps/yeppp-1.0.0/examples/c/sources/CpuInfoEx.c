#include <stdio.h>
#include <assert.h>
#include <yepLibrary.h>

/* The size of buffer for strings from the Yeppp! library */
#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
	enum YepStatus status;
	Yep32u coresCount, L1I, L1D, L2, L3;
	YepSize bufferLength;
	char buffer[BUFFER_SIZE];

	status = yepLibrary_Init();
	assert(status == YepStatusOk);

	bufferLength = BUFFER_SIZE - 1; /* Reserve one symbol for terminating null */
	status = yepLibrary_GetString(YepEnumerationCpuFullName, 0, YepStringTypeDescription, buffer, &bufferLength);
	assert(status == YepStatusOk);
	buffer[bufferLength] = '\0'; /* Append terminating null */
	printf("Processor: %s\n", buffer);

	status = yepLibrary_GetLogicalCoresCount(&coresCount);
	assert(status == YepStatusOk);
	printf("Logical cores: %u\n", (unsigned)(coresCount));
	
	status = yepLibrary_GetCpuInstructionCacheSize(1, &L1I);
	assert(status == YepStatusOk);
	printf("L1I: %u\n", (unsigned)(L1I));
	
	status = yepLibrary_GetCpuDataCacheSize(1, &L1D);
	assert(status == YepStatusOk);
	printf("L1D: %u\n", (unsigned)(L1D));
	
	status = yepLibrary_GetCpuDataCacheSize(2, &L2);
	assert(status == YepStatusOk);
	printf("L2: %u\n", (unsigned)(L2));
	
	status = yepLibrary_GetCpuDataCacheSize(3, &L3);
	assert(status == YepStatusOk);
	printf("L3: %u\n", (unsigned)(L3));
	
	status = yepLibrary_Release();
	assert(status == YepStatusOk);
	return 0;
}
