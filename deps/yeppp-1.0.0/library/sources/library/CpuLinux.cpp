/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#if defined(_FORTIFY_SOURCE)
	#undef _FORTIFY_SOURCE
#endif

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <library/functions.h>
#include <yepBuiltin.h>
#include <string.h>

#if defined(YEP_LINUX_OS)
	#include <yepSyscalls.hpp>
	#include <errno.h>
	#include <sys/klog.h>
	#include <sys/mman.h>
#else
	#error "The functions in this file should only be used in and compiled for Linux"
#endif

YepStatus _yepLibrary_ParseProcCpuInfo(LineParser lineParser, void* state) {
	int file = yepSyscall_open("/proc/cpuinfo", O_RDONLY);
	if YEP_UNLIKELY(file < 0) {
		return YepStatusSystemError;
	}
	YepStatus status = YepStatusOk;
	
	const YepSize bufferSize = 1024;
	char buffer[bufferSize];
	char* bufferStart = buffer;

	ssize_t readResult;
	do {
		readResult = yepSyscall_read(file, bufferStart, bufferSize - (bufferStart - buffer));
		if YEP_UNLIKELY(readResult < 0) {
			status = YepStatusSystemError;
			goto cleanup;
		}
		const char* bufferEnd = bufferStart + readResult;
		const char* lineStart = buffer;
		const char* lineEnd;

		if YEP_UNLIKELY(readResult == 0) {
			// Process the remaining text in the buffer as a single line
			lineParser(lineStart, bufferEnd, state);
		} else {
			do {
				// Find the end of line
				for (lineEnd = lineStart; lineEnd != bufferEnd; lineEnd++) {
					if (*lineEnd == '\n')
						break;
				}

				// If we found the end-of-line
				if (lineEnd != bufferEnd) {
					lineParser(lineStart, lineEnd, state);
					lineStart = lineEnd + 1;
				}
			} while (lineEnd != bufferEnd);
			
			const YepSize lineLength = lineEnd - lineStart;
			memcpy(buffer, lineStart, lineLength);
			bufferStart = buffer + lineLength;
		}
	} while (readResult != 0);

cleanup:
	int closeResult = yepSyscall_close(file);
	if YEP_UNLIKELY(closeResult < 0) {
		return YepStatusSystemError;
	}
	return status;
}

#if defined(YEP_ARM_CPU)
	YepStatus _yepLibrary_ParseKernelLog(LineParser lineParser, void* state) {
		/* Trying to open or close syslog fails, but reading it actually works. */
		const int syslogReadAll = 3;
		const int syslogGetSize = 10;

		YepStatus status = YepStatusOk;
		YepSize syslogBufferSize;
		void *syslogBuffer = YEP_NULL_POINTER;
		void *mmapResult;
		const char *bufferStart, *bufferEnd, *lineStart, *lineEnd;
		int syslogGetSizeResult, syslogReadAllResult, unmmapResult;
		

		syslogGetSizeResult = yepSyscall_syslog(syslogGetSize, YEP_NULL_POINTER, 0);
		if YEP_UNLIKELY(syslogGetSizeResult <= 0) {
			status = YepStatusSystemError;
			goto cleanup;
		}

		syslogBufferSize = YepSize((unsigned int)((syslogGetSizeResult + 4095) & -4096));
		mmapResult = yepSyscall_mmap2(0, syslogBufferSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if YEP_UNLIKELY(YepSize(mmapResult) > YepSize(-YepSize(4096))) {
			if YEP_LIKELY(YepSize(mmapResult) == YepSize(-YepSize(ENOMEM))) {
				status = YepStatusOutOfMemory;
			} else {
				status = YepStatusSystemError;
			}
			goto cleanup;
		} else {
			syslogBuffer = mmapResult;
		}

		syslogReadAllResult = yepSyscall_syslog(syslogReadAll, static_cast<char*>(syslogBuffer), syslogGetSizeResult);
		if YEP_UNLIKELY(syslogReadAllResult < 0) {
			status = YepStatusSystemError;
			goto cleanup;
		}
		
		bufferStart = static_cast<const char*>(syslogBuffer);
		if (syslogReadAllResult != 0) {
			bufferEnd = bufferStart + syslogReadAllResult;
			lineStart = bufferStart;
			lineEnd;

			do {
				// Find the end of line
				for (lineEnd = lineStart; lineEnd != bufferEnd; lineEnd++) {
					if (*lineEnd == '\n')
						break;
				}

				lineParser(lineStart, lineEnd, state);
				lineStart = lineEnd + 1;
			} while (lineEnd != bufferEnd);
		}

	cleanup:
		if YEP_LIKELY(syslogBuffer != YEP_NULL_POINTER) {
			unmmapResult = yepSyscall_munmap(syslogBuffer, syslogBufferSize);
			if YEP_UNLIKELY(unmmapResult < 0) {
				status = YepStatusSystemError;
			}
		}
		return status;
	}
#endif

#pragma pack(push, 1)
/* This is an incomplete linux_direct structure definition, but it is enough for our purpose */
struct linux_dirent {
	unsigned long d_ino;
	unsigned long d_off;
	unsigned short d_reclen;
	char d_name[];
};
#pragma pack(pop)

YepStatus _yepLibrary_InitLinuxLogicalCoresCount(Yep32u& logicalCoresCount, Yep64u& systemFeatures) {
	YepStatus status = YepStatusOk;
	int rootCpuDirectory = yepSyscall_open("/sys/devices/system/cpu", O_RDONLY | O_NONBLOCK | O_DIRECTORY);
	if YEP_UNLIKELY(rootCpuDirectory < 0) {
		return YepStatusSystemError;
	}

	const YepSize bufferSize = 1024;
	Yep8u buffer[bufferSize];
readDirectoryEntries:
	int syscallResult = yepSyscall_getdents(rootCpuDirectory, reinterpret_cast<linux_dirent*>(buffer), bufferSize);
	if YEP_LIKELY(syscallResult > 0) {
		int entriesLength = syscallResult;
		
		const Yep8u* entryPointer = buffer;
		while (entriesLength > 0) {
			const char* filename = (reinterpret_cast<const linux_dirent*>(entryPointer))->d_name;
			unsigned short recordLength = reinterpret_cast<const linux_dirent*>(entryPointer)->d_reclen;
			// Check that the filename has /cpu[0-9]+/ pattern
			if (YEP_UNLIKELY(filename[0] == 'c') && YEP_LIKELY(filename[1] == 'p') && YEP_LIKELY(filename[2] == 'u')) {
				// Filename starts with "cpu"
				if (YEP_LIKELY(filename[3] >= '0') && YEP_LIKELY(filename[3] <= '9')) {
					// Filename start with /cpu[0-9]/
					for (filename = &filename[4]; *filename != 0; filename++) {
						if (YEP_UNLIKELY(*filename < '0') || YEP_UNLIKELY(*filename > '9')) {
							goto nextEntry;
						}
					}
					// Filename matches with /cpu[0-9]+/: increment the number of cores
					logicalCoresCount += 1;
				}
			}
nextEntry:
			entriesLength -= recordLength;
			entryPointer += recordLength;
		}
		goto readDirectoryEntries;
	} else if YEP_UNLIKELY(syscallResult < 0) {
		status = YepStatusSystemError;
	}

	if (logicalCoresCount == 1) {
		// Set this flag only if we know for sure.
		systemFeatures |= YepSystemFeatureSingleThreaded;
	} else {
		if (logicalCoresCount == 0) {
			logicalCoresCount = 1;
		}
	}

	int closeResult = yepSyscall_close(rootCpuDirectory);
	if (closeResult < 0) {
		status = YepStatusSystemError;
	}
	return status;
}
