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

#if defined(YEP_LINUX_OS)
	#include <unistd.h>
	#include <sched.h>
	#include <sys/wait.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/syscall.h>
	#include <linux/unistd.h>
	#include <signal.h>
	#ifndef SA_RESTORER
		#define SA_RESTORER 0x04000000
	#endif
	#include <linux/aio_abi.h>


	#ifndef __NR_getcpu
		#if defined(YEP_EABI_ARM_ABI)
			#define __NR_getcpu 345
		#elif defined(YEP_SYSTEMV_X64_ABI) || defined(YEP_K1OM_X64_ABI)
			#define __NR_getcpu 309
		#endif
	#endif

	#if defined(YEP_ANDROID_LINUX_OS)
		#ifndef __NR_perf_event_open
			#if defined(YEP_EABI_ARM_ABI)
				#define __NR_perf_event_open 364
			#endif
			#if defined(YEP_O32_MIPS_ABI)
				#define __NR_perf_event_open 333
			#endif
		#endif
		#if defined(YEP_EABI_ARM_ABI)
			#undef P_ALL
			#undef P_PID
			#undef P_PGID
			typedef enum {
				P_ALL,
				P_PID,
				P_PGID
			} idtype_t;
		#endif
	#endif
	
	#if defined(YEP_EABI_ARM_ABI)
		#define KERNEL_NSIG 64
		struct kernel_sigset_t {
			Yep32u sig[KERNEL_NSIG / 32];
		};

		#ifdef sa_handler
			#undef sa_handler
		#endif
		#ifdef sa_sigaction
			#undef sa_sigaction
		#endif

		struct kernel_sigaction {
			union {
				void (*sa_handler)(int);
				void (*sa_sigaction)(int, siginfo_t*, void *);
			};
			Yep32u sa_flags;
			void (*sa_restorer)(void);
			struct kernel_sigset_t sa_mask;
		};
	#endif
	#if defined(YEP_O32_MIPS_ABI)
		#define KERNEL_NSIG 128
		struct kernel_sigset_t {
			Yep32u sig[KERNEL_NSIG / 32];
		};

		#ifdef sa_handler
			#undef sa_handler
		#endif
		#ifdef sa_sigaction
			#undef sa_sigaction
		#endif

		struct kernel_sigaction {
			Yep32u sa_flags;
			union {
				void (*sa_handler)(int);
				void (*sa_sigaction) (int, siginfo_t*, void *);
			};
			struct kernel_sigset_t sa_mask;
		};		
	#endif
#endif

#if defined(YEP_LINUX_OS)
	#if defined(YEP_X86_ABI)
		static YEP_INLINE int yepSyscall_uname(char *buffer) {
			int result;
			#if defined(YEP_PIC)
				asm volatile(
					"xchgl %%ecx, %%ebx;"
					"int $0x80;"
					"movl %%ecx, %%ebx;"
					: "=a" (result)
					: "c" (buffer), "a" (__NR_oldolduname)
					: "memory"
				);
			#else
				asm volatile(
					"int $0x80;"
					: "=a" (result)
					: "b" (buffer), "a" (__NR_oldolduname)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE int yepSyscall_open(const char *path, int flags) {
			int result;
			#if defined(YEP_PIC)
				asm volatile(
					"xchgl %%edx, %%ebx;"
					"int $0x80;"
					"movl %%edx, %%ebx;"
					: "=a" (result)
					: "d" (path), "c" (flags), "a" (__NR_open)
					: "memory"
				);
			#else
				asm volatile(
					"int $0x80;"
					: "=a" (result)
					: "b" (path), "c" (flags), "a" (__NR_open)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE ssize_t yepSyscall_read(int file, void *buffer, size_t count) {
			ssize_t result;
			#if defined(YEP_PIC)
				asm volatile (
					"xchgl %%edi, %%ebx;"
					"int $0x80;"
					"movl %%edi, %%ebx;"
					: "=a" (result)
					: "D" (file), "c" (buffer), "d" (count), "a" (__NR_read)
					: "memory"
				);
			#else
				asm volatile (
					"int $0x80;"
					: "=a" (result)
					: "b" (file), "c" (buffer), "d" (count), "a" (__NR_read)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE ssize_t yepSyscall_write(int file, const void *buffer, size_t count) {
			ssize_t result;
			#if defined(YEP_PIC)
				asm volatile (
					"xchgl %%edi, %%ebx;"
					"int $0x80;"
					"movl %%edi, %%ebx;"
					: "=a" (result)
					: "D" (file), "c" (buffer), "d" (count), "a" (__NR_write)
					: "memory"
				);
			#else
				asm volatile (
					"int $0x80;"
					: "=a" (result)
					: "b" (file), "c" (buffer), "d" (count), "a" (__NR_write)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE ssize_t yepSyscall_pread(int file, void *buffer, size_t count, off_t offset) {
			ssize_t result;
			#if defined(YEP_PIC)
				asm volatile (
					"xchgl %%edi, %%ebx;"
					"int $0x80;"
					"movl %%edi, %%ebx;"
					: "=a" (result)
					: "D" (file), "c" (buffer), "d" (count), "S" (offset), "a" (__NR_pread64)
					: "memory"
				);
			#else
				asm volatile (
					"int $0x80;"
					: "=a" (result)
					: "b" (file), "c" (buffer), "d" (count), "S" (offset), "a" (__NR_pread64)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE int yepSyscall_close(int file) {
			int result;
			#if defined(YEP_PIC)
				asm volatile (
					"xchgl %%ecx, %%ebx;"
					"int $0x80;"
					"movl %%ecx, %%ebx;"
					: "=a" (result)
					: "c" (file), "a" (__NR_close)
					: "memory"
				);
			#else
				asm volatile (
					"int $0x80;"
					: "=a" (result)
					: "b" (file), "a" (__NR_close)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE int yepSyscall_getdents(int file, struct linux_dirent *buffer, unsigned int count) {
			int result;
			#if defined(YEP_PIC)
				asm volatile (
					"xchgl %%edi, %%ebx;"
					"int $0x80;"
					"movl %%edi, %%ebx;"
					: "=a" (result)
					: "D" (file), "c" (buffer), "d" (count), "a" (__NR_getdents)
					: "memory"
				);
			#else
				asm volatile (
					"int $0x80;"
					: "=a" (result)
					: "b" (file), "c" (buffer), "d" (count), "a" (__NR_getdents)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE int yepSyscall_clock_gettime(int clockId, struct timespec *time) {
			int result;
			#if defined(YEP_PIC)
				asm volatile (
					"xchgl %%edi, %%ebx;"
					"int $0x80;"
					"movl %%edi, %%ebx;"
					: "=a" (result)
					: "D" (clockId), "c" (time), "a" (__NR_clock_gettime)
					: "memory"
				);
			#else
				asm volatile (
					"int $0x80;"
					: "=a" (result)
					: "b" (clockId), "c" (time), "a" (__NR_clock_gettime)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE int yepSyscall_clock_getres(int clockId, struct timespec *resolution) {
			int result;
			#if defined(YEP_PIC)
				asm volatile (
					"xchgl %%edi, %%ebx;"
					"int $0x80;"
					"movl %%edi, %%ebx;"
					: "=a" (result)
					: "D" (clockId), "c" (resolution), "a" (__NR_clock_getres)
					: "memory"
				);
			#else
				asm volatile (
					"int $0x80;"
					: "=a" (result)
					: "b" (clockId), "c" (resolution), "a" (__NR_clock_getres)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE void *yepSyscall_mmap(void *address, size_t length, int memoryProtection, int flags, int fileDescriptor, off_t offset) {
			void *result;
			#if defined(YEP_PIC)
				struct mmapParameters {
					void *address;
					size_t length;
					int memoryProtection;
					int flags;
					int fileDescriptor;
					off_t offset;
				};

				const mmapParameters parameters = { address, length, memoryProtection, flags, fileDescriptor, offset };
				asm volatile (
					"pushl %%ebx;"
					"pushl %%esi;"
					"pushl %%edi;"
					"pushl %%ebp;"
					"movl 0(%%eax), %%ebx;"
					"movl 4(%%eax), %%ecx;"
					"movl 8(%%eax), %%edx;"
					"movl 12(%%eax), %%esi;"
					"movl 16(%%eax), %%edi;"
					"movl 20(%%eax), %%ebp;"
					"movl %[syscallNumber], %%eax;"
					"int $0x80;"
					"popl %%ebp;"
					"popl %%edi;"
					"popl %%esi;"
					"popl %%ebx;"
					: "=a" (result)
					: "a" (&parameters),  [syscallNumber] "n" (__NR_mmap)
					: "%ecx", "%edx", "memory"
				);
			#else
				asm volatile (
					"pushl %%ebp;"
					"movl %%eax, %%ebp;"
					"movl %%eax, %[syscallNumber];"
					"int $0x80;"
					"popl %%ebp;"
					: "=a" (result)
					: "b" (address), "c" (length), "d" (memoryProtection), "S" (flags), "D" (fileDescriptor), "a" (offset), [syscallNumber] "n" (__NR_clock_getres)
					: "memory"
				);
			#endif
			return result;
		}

		static YEP_INLINE int yepSyscall_getcpu(unsigned *cpu, unsigned *nodestruct, void *unused_null) {
			int result;
			#if defined(YEP_PIC)
				asm volatile (
					"xchgl %%edi, %%ebx;"
					"int $0x80;"
					"movl %%edi, %%ebx;"
					: "=a" (result)
					: "D" (cpu), "c" (nodestruct), "d" (unused_null), "a" (__NR_getcpu)
					: "memory"
				);
			#else
				asm volatile (
					"int $0x80;"
					: "=a" (result)
					: "b" (cpu), "c" (nodestruct), "d" (unused_null), "a" (__NR_getcpu)
					: "memory"
				);
			#endif
			return result;
		}
	#elif defined(YEP_SYSTEMV_X64_ABI) || defined(YEP_K1OM_X64_ABI)
		static YEP_INLINE int yepSyscall_uname(char *buffer) {
			int result;
			asm volatile(
				"syscall;"
				: "=a" (result)
				: "D" (buffer), "a" (__NR_uname)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE int yepSyscall_syslog(int type, char *buffer, int length) {
			int result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (type), "S" (buffer), "d" (length), "a" (__NR_syslog)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE int yepSyscall_open(const char *path, int flags) {
			int result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (path), "S" (flags), "a" (__NR_open)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE ssize_t yepSyscall_read(int file, void *buffer, size_t count) {
			ssize_t result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (file), "S" (buffer), "d" (count), "a" (__NR_read)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE ssize_t yepSyscall_write(int file, const void *buffer, size_t count) {
			ssize_t result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (file), "S" (buffer), "d" (count), "a" (__NR_write)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE ssize_t yepSyscall_pread(int file, void *buffer, size_t count, off_t offset) {
			register Yep64u r10 asm ("r10") = static_cast<Yep64u>(offset);
			ssize_t result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (file), "S" (buffer), "d" (count), "r" (r10), "a" (__NR_pread64)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE int yepSyscall_close(int file) {
			int result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (file), "a" (__NR_close)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE int yepSyscall_getdents(int file, struct linux_dirent *buffer, unsigned int count) {
			int result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (file), "S" (buffer), "d" (count), "a" (__NR_getdents)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE int yepSyscall_clock_gettime(int clockId, struct timespec *time) {
			int result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (clockId), "S" (time), "a" (__NR_clock_gettime)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE int yepSyscall_clock_getres(int clockId, struct timespec *resolution) {
			int result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (clockId), "S" (resolution), "a" (__NR_clock_getres)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE void* yepSyscall_mmap(void *address, size_t length, int memoryProtection, int flags, int fileDescriptor, off_t offset) {
			register Yep64u r10 asm ("r10") = static_cast<Yep64u>(flags);
			register Yep64u r8 asm ("r8") = static_cast<Yep64u>(fileDescriptor);
			register Yep64u r9 asm ("r9") = static_cast<Yep64u>(offset);
			void* result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (address), "S" (length), "d" (memoryProtection), "r" (r10), "r" (r8), "r" (r9), "a" (__NR_mmap)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE int yepSyscall_munmap(void *address, size_t length) {
			int result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (address), "S" (length), "a" (__NR_munmap)
				: "rcx", "r11", "memory"
			);
			return result;
		}

		static YEP_INLINE int yepSyscall_getcpu(unsigned *cpu, unsigned *nodestruct, void *unused_null) {
			int result;
			asm volatile (
				"syscall;"
				: "=a" (result)
				: "D" (cpu), "S" (nodestruct), "d" (unused_null), "a" (__NR_getcpu)
				: "rcx", "r11", "memory"
			);
			return result;
		}
	#elif defined(YEP_EABI_ARM_ABI)
		static YEP_INLINE int yepSyscall_uname(char *buffer) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u r7 asm ("r7") = __NR_uname;
			asm volatile(
				"swi $0;"
				: "+r" (r0), "+r" (r7)
				:
				: "r1", "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE pid_t yepSyscall_getpid() {
			register Yep32u r0 asm ("r0");
			register Yep32u r7 asm ("r7") = __NR_uname;
			asm volatile(
				"swi $0;"
				: "=r" (r0), "+r" (r7)
				:
				: "r1", "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<pid_t>(r0);
		}

		static YEP_INLINE int yepSyscall_syslog(int type, char *buffer, int length) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(type);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(length);
			register Yep32u r7 asm ("r7") = __NR_syslog;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}
	
		static YEP_INLINE int yepSyscall_open(const char *path, int flags) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(path);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(flags);
			register Yep32u r7 asm ("r7") = __NR_open;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_ioctl(int file, int request) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(file);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(request);
			register Yep32u r7 asm ("r7") = __NR_ioctl;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE ssize_t yepSyscall_read(int file, void *buffer, size_t count) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(file);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(count);
			register Yep32u r7 asm ("r7") = __NR_read;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<ssize_t>(r0);
		}

		static YEP_INLINE ssize_t yepSyscall_write(int file, const void *buffer, size_t count) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(file);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(count);
			register Yep32u r7 asm ("r7") = __NR_write;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<ssize_t>(r0);
		}

		static YEP_INLINE int yepSyscall_close(int file) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(file);
			register Yep32u r7 asm ("r7") = __NR_close;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r7)
				:
				: "r1", "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_perf_event_open(struct perf_event_attr *hardwareEvent, pid_t processId, int cpu, int groupFileDescriptor, unsigned long flags) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(hardwareEvent);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(processId);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(cpu);
			register Yep32u r3 asm ("r3") = static_cast<Yep32u>(groupFileDescriptor);
			register Yep32u r4 asm ("r4") = static_cast<Yep32u>(flags);
			register Yep32u r7 asm ("r7") = __NR_perf_event_open;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r4), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_rt_sigaction(int signalNumber, const struct kernel_sigaction *newAction, struct kernel_sigaction *oldAction, size_t sigsetsize) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(signalNumber);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(newAction);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(oldAction);
			register Yep32u r3 asm ("r3") = static_cast<Yep32u>(sigsetsize);
			register Yep32u r7 asm ("r7") = __NR_rt_sigaction;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_rt_sigreturn(unsigned long reserved) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(reserved);
			register Yep32u r7 asm ("r7") = __NR_rt_sigreturn;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r7)
				:
				: "r1", "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_getdents(int file, struct linux_dirent *buffer, unsigned int count) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(file);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(count);
			register Yep32u r7 asm ("r7") = __NR_getdents;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_clock_gettime(clockid_t clockId, struct timespec *time) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(clockId);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(time);
			register Yep32u r7 asm ("r7") = __NR_clock_gettime;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_clock_getres(clockid_t clockId, struct timespec *resolution) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(clockId);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(resolution);
			register Yep32u r7 asm ("r7") = __NR_clock_getres;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_clock_nanosleep(clockid_t clockId, int flags, const struct timespec *rqtp, struct timespec *rmtp) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(clockId);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(flags);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(rqtp);
			register Yep32u r3 asm ("r3") = reinterpret_cast<Yep32u>(rmtp);
			register Yep32u r7 asm ("r7") = __NR_clock_nanosleep;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_nanosleep(const struct timespec *req, struct timespec *rem) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(req);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(rem);
			register Yep32u r7 asm ("r7") = __NR_nanosleep;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE void *yepSyscall_mmap2(void *address, size_t length, int memoryProtection, int flags, int fileDescriptor, off_t offset) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(address);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(length);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(memoryProtection);
			register Yep32u r3 asm ("r3") = static_cast<Yep32u>(flags);
			register Yep32u r4 asm ("r4") = static_cast<Yep32u>(fileDescriptor);
			register Yep32u r5 asm ("r5") = static_cast<Yep32u>(offset);
			register Yep32u r7 asm ("r7") = __NR_mmap2;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r4), "+r" (r5), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return reinterpret_cast<void*>(r0);
		}

		static YEP_INLINE int yepSyscall_madvise(void *addr, size_t length, int advice) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(addr);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(length);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(advice);
			register Yep32u r7 asm ("r7") = __NR_madvise;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_munmap(void *address, size_t length) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(address);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(length);
			register Yep32u r7 asm ("r7") = __NR_munmap;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE void yepSyscall_exit(int status) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(status);
			register Yep32u r7 asm ("r7") = __NR_exit;
			asm volatile (
				"swi $0;"
				:
				: "r" (r0), "r" (r7)
				:
			);
		}

		static YEP_INLINE int yepSyscall_clone(int flags, void *stackPointer) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(flags);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(stackPointer);
			register Yep32u r7 asm ("r7") = __NR_clone;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE pid_t yepSyscall_wait4(pid_t pid, int *status, int options, struct rusage *rusage) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(pid);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(status);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(options);
			register Yep32u r3 asm ("r3") = reinterpret_cast<Yep32u>(rusage);
			register Yep32u r7 asm ("r7") = __NR_wait4;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return static_cast<pid_t>(r0);
		}

		static YEP_INLINE int yepSyscall_waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(idtype);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(id);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(infop);
			register Yep32u r3 asm ("r3") = static_cast<Yep32u>(options);
			register Yep32u r7 asm ("r7") = __NR_waitid;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_semget(key_t key, int nsems, int semflg) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(key);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(nsems);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(semflg);
			register Yep32u r7 asm ("r7") = __NR_semget;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_semctl(int semid, int semnum, int cmd) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(semid);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(semnum);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(cmd);
			register Yep32u r7 asm ("r7") = __NR_semctl;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_semop(int semid, struct sembuf *sops, unsigned nsops) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(semid);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(sops);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(nsops);
			register Yep32u r7 asm ("r7") = __NR_semop;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_semtimedop(int semid, struct sembuf *sops, unsigned nsops, struct timespec *timeout) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(semid);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(sops);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(nsops);
			register Yep32u r3 asm ("r3") = reinterpret_cast<Yep32u>(timeout);
			register Yep32u r7 asm ("r7") = __NR_semtimedop;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_futex(int *uaddr, int op, int valconst, struct timespec *timeout, int *uaddr2, int val3) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(uaddr);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(op);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(valconst);
			register Yep32u r3 asm ("r3") = reinterpret_cast<Yep32u>(timeout);
			register Yep32u r4 asm ("r4") = reinterpret_cast<Yep32u>(uaddr2);
			register Yep32u r5 asm ("r5") = static_cast<Yep32u>(val3);
			register Yep32u r7 asm ("r7") = __NR_futex;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r4), "+r" (r5), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_getcpu(unsigned *cpu, unsigned *nodestruct, void *unused_null) {
			register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(cpu);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(nodestruct);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(unused_null);
			register Yep32u r7 asm ("r7") = __NR_getcpu;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_sched_yield() {
			register Yep32u r0 asm ("r0");
			register Yep32u r7 asm ("r7") = __NR_sched_yield;
			asm volatile (
				"swi $0;"
				: "=r" (r0), "+r" (r7)
				:
				: "r1", "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(pid);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(policy);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(param);
			register Yep32u r7 asm ("r7") = __NR_sched_setscheduler;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_sched_getscheduler(pid_t pid) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(pid);
			register Yep32u r7 asm ("r7") = __NR_sched_getscheduler;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r7)
				:
				: "r1", "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_sched_setparam(pid_t pid, const struct sched_param *param) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(pid);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(param);
			register Yep32u r7 asm ("r7") = __NR_sched_setparam;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_sched_getparam(pid_t pid, struct sched_param *param) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(pid);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(param);
			register Yep32u r7 asm ("r7") = __NR_sched_getparam;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_sched_setaffinity(pid_t pid, size_t cpusetsize, void *mask) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(pid);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(cpusetsize);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(mask);
			register Yep32u r7 asm ("r7") = __NR_sched_setaffinity;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_sched_getaffinity(pid_t pid, size_t cpusetsize, void *mask) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(pid);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(cpusetsize);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(mask);
			register Yep32u r7 asm ("r7") = __NR_sched_getaffinity;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_io_setup(unsigned nr_events, aio_context_t *ctxp) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(nr_events);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(ctxp);
			register Yep32u r7 asm ("r7") = __NR_io_setup;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r7)
				:
				: "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_io_destory(aio_context_t ctx) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(ctx);
			register Yep32u r7 asm ("r7") = __NR_io_destroy;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r7)
				:
				: "r1", "r2", "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_io_getevents(aio_context_t ctx_id, long min_nr, long nr, struct io_event *events, struct timespec *timeout) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(ctx_id);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(min_nr);
			register Yep32u r2 asm ("r2") = static_cast<Yep32u>(nr);
			register Yep32u r3 asm ("r3") = reinterpret_cast<Yep32u>(events);
			register Yep32u r4 asm ("r4") = reinterpret_cast<Yep32u>(timeout);
			register Yep32u r7 asm ("r7") = __NR_io_getevents;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r4), "+r" (r7)
				:
				: "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_io_submit(aio_context_t ctx_id, long nr, struct iocb **iocbpp) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(ctx_id);
			register Yep32u r1 asm ("r1") = static_cast<Yep32u>(nr);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(iocbpp);
			register Yep32u r7 asm ("r7") = __NR_io_submit;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}

		static YEP_INLINE int yepSyscall_io_cancel(aio_context_t ctx_id, struct iocb *iocb, struct io_event *result) {
			register Yep32u r0 asm ("r0") = static_cast<Yep32u>(ctx_id);
			register Yep32u r1 asm ("r1") = reinterpret_cast<Yep32u>(iocb);
			register Yep32u r2 asm ("r2") = reinterpret_cast<Yep32u>(result);
			register Yep32u r7 asm ("r7") = __NR_io_cancel;
			asm volatile (
				"swi $0;"
				: "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r7)
				:
				: "r3", "ip", "memory", "cc"
			);
			return static_cast<int>(r0);
		}
	#elif defined(YEP_O32_MIPS_ABI)
		static YEP_INLINE int yepSyscall_uname(char *buffer) {
			register Yep32u v0 asm ("v0") = __NR_uname;
			register Yep32u a0 asm ("a0") = reinterpret_cast<Yep32u>(buffer);
			asm volatile(
				"syscall;"
				: "+r" (v0), "+r" (a0)
				:
				: "v1", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_syslog(int type, char *buffer, int length) {
			register Yep32u v0 asm ("v0") = __NR_syslog;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(type);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u a2 asm ("a2") = static_cast<Yep32u>(length);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_open(const char *path, int flags) {
			register Yep32u v0 asm ("v0") = __NR_open;
			register Yep32u a0 asm ("a0") = reinterpret_cast<Yep32u>(path);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(flags);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_ioctl(int file, int request) {
			register Yep32u v0 asm ("v0") = __NR_ioctl;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(file);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(request);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE ssize_t yepSyscall_read(int file, void *buffer, size_t count) {
			register Yep32u v0 asm ("v0") = __NR_read;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(file);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u a2 asm ("a2") = static_cast<Yep32u>(count);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE ssize_t yepSyscall_write(int file, const void *buffer, size_t count) {
			register Yep32u v0 asm ("v0") = __NR_write;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(file);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u a2 asm ("a2") = static_cast<Yep32u>(count);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_close(int file) {
			register Yep32u v0 asm ("v0") = __NR_close;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(file);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0)
				:
				: "v1", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_perf_event_open(struct perf_event_attr *hardwareEvent, pid_t processId, int cpu, int groupFileDescriptor, unsigned long flags) {
			register Yep32u v0 asm ("v0") = __NR_perf_event_open;
			register Yep32u a0 asm ("a0") = reinterpret_cast<Yep32u>(hardwareEvent);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(processId);
			register Yep32u a2 asm ("a2") = static_cast<Yep32u>(cpu);
			register Yep32u a3 asm ("a3") = static_cast<Yep32u>(groupFileDescriptor);
			register Yep32u a4 = static_cast<Yep32u>(flags);
			asm volatile (
				"addiu $sp, -24;"
				"sw %[a4], 16($sp);"
				"syscall;"
				"addiu $sp, 24;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2), "+r" (a3)
				: [a4] "r" (a4)
				: "v1", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_rt_sigaction(int signalNumber, const struct kernel_sigaction *newAction, struct kernel_sigaction *oldAction, size_t sigsetsize) {
			register Yep32u v0 asm ("v0") = __NR_rt_sigaction;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(signalNumber);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(newAction);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(oldAction);
			register Yep32u a3 asm ("a3") = static_cast<Yep32u>(sigsetsize);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2), "+r" (a3)
				:
				: "v1", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_getdents(int file, struct linux_dirent *buffer, unsigned int count) {
			register Yep32u v0 asm ("v0") = __NR_getdents;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(file);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(buffer);
			register Yep32u a2 asm ("a2") = static_cast<Yep32u>(count);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_clock_gettime(clockid_t clockId, struct timespec *time) {
			register Yep32u v0 asm ("v0") = __NR_clock_gettime;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(clockId);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(time);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_clock_getres(clockid_t clockId, struct timespec *resolution) {
			register Yep32u v0 asm ("v0") = __NR_clock_getres;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(clockId);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(resolution);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_clock_nanosleep(clockid_t clockId, int flags, const struct timespec *rqtp, struct timespec *rmtp) {
			register Yep32u v0 asm ("v0") = __NR_clock_nanosleep;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(clockId);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(flags);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(rqtp);
			register Yep32u a3 asm ("a3") = reinterpret_cast<Yep32u>(rmtp);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2), "+r" (a3)
				:
				: "v1", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_nanosleep(const struct timespec *req, struct timespec *rem) {
			register Yep32u v0 asm ("v0") = __NR_nanosleep;
			register Yep32u a0 asm ("a0") = reinterpret_cast<Yep32u>(req);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(rem);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE void *yepSyscall_mmap2(void *address, size_t length, int memoryProtection, int flags, int fileDescriptor, off_t offset) {
			register Yep32u a0 asm ("a0") = reinterpret_cast<Yep32u>(address);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(length);
			register Yep32u a2 asm ("a2") = static_cast<Yep32u>(memoryProtection);
			register Yep32u a3 asm ("a3") = static_cast<Yep32u>(flags);
			register Yep32u a4 = static_cast<Yep32u>(fileDescriptor);
			register Yep32u a5 = static_cast<Yep32u>(offset);
			register Yep32u v0 asm ("v0") = __NR_mmap2;
			asm volatile (
				"addiu $sp, -24;"
				"sw %[a4], 16($sp);"
				"sw %[a5], 20($sp);"
				"syscall;"
				"addiu $sp, 24;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2), "+r" (a3)
				: [a4] "r" (a4), [a5] "r" (a5)
				: "v1", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return reinterpret_cast<void*>(v0);
		}

		static YEP_INLINE int yepSyscall_madvise(void *addr, size_t length, int advice) {
			register Yep32u v0 asm ("v0") = __NR_madvise;
			register Yep32u a0 asm ("a0") = reinterpret_cast<Yep32u>(addr);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(length);
			register Yep32u a2 asm ("a2") = static_cast<Yep32u>(advice);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_munmap(void *address, size_t length) {
			register Yep32u v0 asm ("v0") = __NR_munmap;
			register Yep32u a0 asm ("a0") = reinterpret_cast<Yep32u>(address);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(length);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE void yepSyscall_exit(int status) {
			register Yep32u v0 asm ("v0") = __NR_exit;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(status);
			asm volatile (
				"syscall;"
				:
				: "r" (v0), "r" (a0)
				:
			);
		}

		static YEP_INLINE int yepSyscall_clone(int flags, void *stackPointer) {
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(flags);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(stackPointer);
			register Yep32u v0 asm ("v0") = __NR_clone;
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE pid_t yepSyscall_wait4(pid_t pid, int *status, int options, struct rusage *rusage) {
			register Yep32u v0 asm ("v0") = __NR_wait4;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(pid);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(status);
			register Yep32u a2 asm ("a2") = static_cast<Yep32u>(options);
			register Yep32u a3 asm ("a3") = reinterpret_cast<Yep32u>(rusage);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2), "+r" (a3)
				:
				: "v1", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<pid_t>(v0);
		}

		static YEP_INLINE int yepSyscall_waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
			register Yep32u v0 asm ("v0") = __NR_waitid;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(idtype);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(id);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(infop);
			register Yep32u a3 asm ("a3") = static_cast<Yep32u>(options);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2), "+r" (a3)
				:
				: "v1", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		//~ static YEP_INLINE int yepSyscall_futex(int *uaddr, int op, int valconst, struct timespec *timeout, int *uaddr2, int val3) {
			//~ register Yep32u r0 asm ("r0") = reinterpret_cast<Yep32u>(uaddr);
			//~ register Yep32u r1 asm ("r1") = static_cast<Yep32u>(op);
			//~ register Yep32u r2 asm ("r2") = static_cast<Yep32u>(valconst);
			//~ register Yep32u r3 asm ("r3") = reinterpret_cast<Yep32u>(timeout);
			//~ register Yep32u r4 asm ("r4") = reinterpret_cast<Yep32u>(uaddr2);
			//~ register Yep32u r5 asm ("r5") = static_cast<Yep32u>(val3);
			//~ register Yep32u r7 asm ("r7") = __NR_futex;
			//~ asm volatile (
				//~ "swi $0;"
				//~ : "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r4), "+r" (r5), "+r" (r7)
				//~ :
				//~ : "memory", "cc"
			//~ );
			//~ return static_cast<int>(r0);
		//~ }

		static YEP_INLINE int yepSyscall_getcpu(unsigned *cpu, unsigned *nodestruct, void *unused_null) {
			register Yep32u v0 asm ("v0") = __NR_getcpu;
			register Yep32u a0 asm ("a0") = reinterpret_cast<Yep32u>(cpu);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(nodestruct);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(unused_null);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_sched_yield() {
			register Yep32u v0 asm ("v0") = __NR_sched_yield;
			asm volatile (
				"swi $0;"
				: "+r" (v0)
				:
				: "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
			register Yep32u v0 asm ("v0") = __NR_sched_setscheduler;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(pid);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(policy);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(param);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_sched_getscheduler(pid_t pid) {
			register Yep32u v0 asm ("v0") = __NR_sched_getscheduler;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(pid);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0)
				:
				: "v1", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_sched_setparam(pid_t pid, const struct sched_param *param) {
			register Yep32u v0 asm ("v0") = __NR_sched_setparam;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(pid);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(param);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_sched_getparam(pid_t pid, struct sched_param *param) {
			register Yep32u v0 asm ("v0") = __NR_sched_getparam;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(pid);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(param);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_sched_setaffinity(pid_t pid, size_t cpusetsize, void *mask) {
			register Yep32u v0 asm ("v0") = __NR_sched_setaffinity;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(pid);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(cpusetsize);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(mask);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_sched_getaffinity(pid_t pid, size_t cpusetsize, void *mask) {
			register Yep32u v0 asm ("v0") = __NR_sched_getaffinity;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(pid);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(cpusetsize);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(mask);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_io_setup(unsigned nr_events, aio_context_t *ctxp) {
			register Yep32u v0 asm ("v0") = __NR_io_setup;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(nr_events);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(ctxp);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1)
				:
				: "v1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_io_destory(aio_context_t ctx) {
			register Yep32u v0 asm ("v0") = __NR_io_destroy;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(ctx);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0)
				:
				: "v1", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		//~ static YEP_INLINE int yepSyscall_io_getevents(aio_context_t ctx_id, long min_nr, long nr, struct io_event *events, struct timespec *timeout) {
			//~ register Yep32u r0 asm ("r0") = static_cast<Yep32u>(ctx_id);
			//~ register Yep32u r1 asm ("r1") = static_cast<Yep32u>(min_nr);
			//~ register Yep32u r2 asm ("r2") = static_cast<Yep32u>(nr);
			//~ register Yep32u r3 asm ("r3") = reinterpret_cast<Yep32u>(events);
			//~ register Yep32u r4 asm ("r4") = reinterpret_cast<Yep32u>(timeout);
			//~ register Yep32u r7 asm ("r7") = __NR_io_getevents;
			//~ asm volatile (
				//~ "swi $0;"
				//~ : "+r" (r0), "+r" (r1), "+r" (r2), "+r" (r3), "+r" (r4), "+r" (r7)
				//~ :
				//~ : "memory", "cc"
			//~ );
			//~ return static_cast<int>(r0);
		//~ }

		static YEP_INLINE int yepSyscall_io_submit(aio_context_t ctx_id, long nr, struct iocb **iocbpp) {
			register Yep32u v0 asm ("v0") = __NR_io_submit;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(ctx_id);
			register Yep32u a1 asm ("a1") = static_cast<Yep32u>(nr);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(iocbpp);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}

		static YEP_INLINE int yepSyscall_io_cancel(aio_context_t ctx_id, struct iocb *iocb, struct io_event *result) {
			register Yep32u v0 asm ("v0") = __NR_io_cancel;
			register Yep32u a0 asm ("a0") = static_cast<Yep32u>(ctx_id);
			register Yep32u a1 asm ("a1") = reinterpret_cast<Yep32u>(iocb);
			register Yep32u a2 asm ("a2") = reinterpret_cast<Yep32u>(result);
			asm volatile (
				"syscall;"
				: "+r" (v0), "+r" (a0), "+r" (a1), "+r" (a2)
				:
				: "v1", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "memory", "cc"
			);
			return static_cast<int>(v0);
		}
	#endif
#endif
