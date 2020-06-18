#include <stdio.h>
#include <assert.h>
#include <yepLibrary.h>

/* The size of buffer for strings from the Yeppp! library */
#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
	enum YepStatus status;
	enum YepCpuArchitecture architecture;
	enum YepCpuMicroarchitecture microarchitecture;
	enum YepCpuVendor vendor;
	Yep64u isaFeatures, simdFeatures, systemFeatures, testFeature;
	Yep32u enumerationValue;
	YepSize bufferLength;
	char buffer[BUFFER_SIZE];

	/* Initialize the Yeppp! library */
	status = yepLibrary_Init();
	assert(status == YepStatusOk);

	printf("Basic CPU information:\n");

	/* Retrieve information about processor architecture */
	status = yepLibrary_GetCpuArchitecture(&architecture);
	assert(status == YepStatusOk);
	/* Convert processor architecture into string */
	bufferLength = BUFFER_SIZE - 1; /* Reserve one symbol for terminating null */
	status = yepLibrary_GetString(YepEnumerationCpuArchitecture, architecture, YepStringTypeDescription, buffer, &bufferLength);
	assert(status == YepStatusOk);
	buffer[bufferLength] = '\0'; /* Append terminating null */
	printf("\tArchitecture: %s\n", buffer);

	/* Retrieve information about processor vendor */
	status = yepLibrary_GetCpuVendor(&vendor);
	assert(status == YepStatusOk);
	/* Convert processor vendor into string */
	bufferLength = BUFFER_SIZE - 1; /* Reserve one symbol for terminating null */
	status = yepLibrary_GetString(YepEnumerationCpuVendor, vendor, YepStringTypeDescription, buffer, &bufferLength);
	assert(status == YepStatusOk);
	buffer[bufferLength] = '\0'; /* Append terminating null */
	printf("\tVendor: %s\n", buffer);

	/* Retrieve information about processor microarchitecture */
	status = yepLibrary_GetCpuMicroarchitecture(&microarchitecture);
	assert(status == YepStatusOk);
	/* Convert processor vendor into string */
	bufferLength = BUFFER_SIZE - 1; /* Reserve one symbol for terminating null */
	status = yepLibrary_GetString(YepEnumerationCpuMicroarchitecture, microarchitecture, YepStringTypeDescription, buffer, &bufferLength);
	assert(status == YepStatusOk);
	buffer[bufferLength] = '\0'; /* Append terminating null */
	printf("\tMicroarchitecture: %s\n", buffer);

	printf("CPU ISA extensions:\n");
	/* Retrieve information about ISA features */
	status = yepLibrary_GetCpuIsaFeatures(&isaFeatures);
	assert(status == YepStatusOk);
	/* Iterate through bits in ISA features mask */
	for (enumerationValue = 0; enumerationValue < 64; enumerationValue++) {
		bufferLength = BUFFER_SIZE - 2; /* Reserve one symbol for semicolon, and one symbol for terminating null */
		status = yepLibrary_GetString(YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(architecture), enumerationValue, YepStringTypeDescription, buffer, &bufferLength);
		/* YepStatusInvalidArgument indicates that either enumerationType or enumerationValue are incorrect
		 * Since we know that enumerationType is valid, the only possibility is specifying
		 * enumerationValue corresponding to a bit which was not assigned any ISA extension.
		 * In this case we simply skip this feature bit. */
		if (status != YepStatusInvalidArgument) {
			assert(status == YepStatusOk);
			buffer[bufferLength] = ':'; /* Append semicolon */
			buffer[bufferLength + 1] = '\0'; /* Append terminating null */
			/* The bitmask to test for one ISA extension corresponding to enumerationValue */
			testFeature = ((Yep64u)(1ull)) << enumerationValue;
			/* Print ISA extension name and whether is it supported on the current processor */
			printf("\t%-60s\t%s\n", buffer, ((isaFeatures & testFeature) != 0 ? "Yes" : "No"));
		}
	}

	printf("CPU SIMD extensions:\n");
	/* Retrieve information about SIMD features */
	status = yepLibrary_GetCpuSimdFeatures(&simdFeatures);
	assert(status == YepStatusOk);
	/* Iterate through bits in SIMD features mask */
	for (enumerationValue = 0; enumerationValue < 64; enumerationValue++) {
		bufferLength = BUFFER_SIZE - 2; /* Reserve one symbol for semicolon, and one symbol for terminating null */
		status = yepLibrary_GetString(YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(architecture), enumerationValue, YepStringTypeDescription, buffer, &bufferLength);
		/* YepStatusInvalidArgument indicates that either enumerationType or enumerationValue are incorrect
		 * Since we know that enumerationType is valid, the only possibility is specifying
		 * enumerationValue corresponding to a bit which was not assigned any SIMD extension.
		 * In this case we simply skip this feature bit. */
		if (status != YepStatusInvalidArgument) {
			assert(status == YepStatusOk);
			buffer[bufferLength] = ':'; /* Append semicolon */
			buffer[bufferLength + 1] = '\0'; /* Append terminating null */
			/* The bitmask to test for one SIMD extension corresponding to enumerationValue */
			testFeature = ((Yep64u)(1ull)) << enumerationValue;
			/* Print SIMD extension name and whether is it supported on the current processor */
			printf("\t%-60s\t%s\n", buffer, ((simdFeatures & testFeature) != 0 ? "Yes" : "No"));
		}
	}

	printf("Non-ISA CPU and system features:\n");
	/* Retrieve information about non-ISA CPU and system features */
	status = yepLibrary_GetCpuSystemFeatures(&systemFeatures);
	assert(status == YepStatusOk);
	/* Iterate through bits in non-ISA CPU and system features mask */
	for (enumerationValue = 0; enumerationValue < 64; enumerationValue++) {
		bufferLength = BUFFER_SIZE - 2;
		status = yepLibrary_GetString(YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(architecture), enumerationValue, YepStringTypeDescription, buffer, &bufferLength);
		/* YepStatusInvalidArgument indicates that either enumerationType or enumerationValue are incorrect
		 * Since we know that enumerationType is valid, the only possibility is specifying
		 * enumerationValue corresponding to a bit which was not assigned any non-ISA CPU or system extension.
		 * In this case we simply skip this feature bit. */
		if (status != YepStatusInvalidArgument) {
			assert(status == YepStatusOk);
			buffer[bufferLength] = ':'; /* Append semicolon */
			buffer[bufferLength + 1] = '\0'; /* Append terminating null */
			/* The bitmask to test for one non-ISA CPU or system extension corresponding to enumerationValue */
			testFeature = ((Yep64u)(1ull)) << enumerationValue;
			/* Print extension name and whether is it supported on the current system and processor */
			printf("\t%-60s\t%s\n", buffer, ((systemFeatures & testFeature) != 0 ? "Yes" : "No"));
		}
	}

	/* Deinitialize the Yeppp! library */
	status = yepLibrary_Release();
	assert(status == YepStatusOk);
	return 0;
}
