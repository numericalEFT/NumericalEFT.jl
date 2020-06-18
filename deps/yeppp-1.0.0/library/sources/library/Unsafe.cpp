/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <library/functions.h>
#include <yepBuiltin.h>

#if defined(YEP_LINUX_OS)
	#include <yepSyscalls.hpp>

	#include <string.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/syscall.h>
	#include <linux/unistd.h>
	#if defined(__ANDROID__)
		#include <signal.h>
		#include <sys/ioctl.h>
		#if defined(YEP_ARM_CPU)
			// The last parts of the context structures are not needed and leaved out
			struct sigcontext {
				unsigned long trap_no;
				unsigned long error_code;
				unsigned long oldmask;
				unsigned long arm_r0;
				unsigned long arm_r1;
				unsigned long arm_r2;
				unsigned long arm_r3;
				unsigned long arm_r4;
				unsigned long arm_r5;
				unsigned long arm_r6;
				unsigned long arm_r7;
				unsigned long arm_r8;
				unsigned long arm_r9;
				unsigned long arm_r10;
				unsigned long arm_fp;
				unsigned long arm_ip;
				unsigned long arm_sp;
				unsigned long arm_lr;
				unsigned long arm_pc;
				unsigned long arm_cpsr;
				unsigned long fault_address;
			};
		#endif
		struct ucontext {
			unsigned long	  uc_flags;
			struct ucontext  *uc_link;
			stack_t		  uc_stack;
			struct sigcontext uc_mcontext;
		};

		typedef struct ucontext ucontext_t;
	#else
		#include <ucontext.h>
	#endif
#else
	#error "The functions in this file should only be used in and compiled for Linux"
#endif

#if (defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)) && defined(YEP_LINUX_OS)
	#if defined(YEP_ARM_CPU)
		static void _yepLibrary_ProbeSignalHandler(int, siginfo_t *, void* ptr) {
			ucontext_t* ctx = (ucontext_t*)ptr;
			ctx->uc_mcontext.arm_pc += 4; // All probed instructions are four bytes long both in ARM and Thumb-2 mode
			ctx->uc_mcontext.arm_r0 = 1; // To indicate that the signal handler was called
		}

		static void _yepLibrary_ReadCoprocessorSignalHandler(int, siginfo_t *, void* ptr) {
			ucontext_t* ctx = (ucontext_t*)ptr;
			ctx->uc_mcontext.arm_pc += 4; // All read coprocessor instructions are four bytes long both in ARM and Thumb-2 mode
			ctx->uc_mcontext.arm_r0 = 0; // To indicate that the signal handler was called
		}
	#elif defined(YEP_MIPS_CPU)
		static void _yepLibrary_ProbeSignalHandler(int, siginfo_t *, void* ptr) {
			ucontext_t* ctx = (ucontext_t*)ptr;
			ctx->uc_mcontext.sc_pc += 4; // Skip unsupported instruction
			ctx->uc_mcontext.sc_regs[2] = 1; // Change $2 = $v0 to 1 to indicate that the signal handler was called
		}
	#endif

	Yep32u _yepLibrary_ProbeInstruction(Yep32u (*ProbeFunction)()) {
		struct kernel_sigaction oldSigillAction;
		struct kernel_sigaction probeSigillAction;
		memset(&probeSigillAction, 0, sizeof(probeSigillAction));
		probeSigillAction.sa_sigaction = &_yepLibrary_ProbeSignalHandler;
		// Needs Linux >= 2.2
		probeSigillAction.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
		int sigactionResult = yepSyscall_rt_sigaction(SIGILL, &probeSigillAction, &oldSigillAction, KERNEL_NSIG / 8);
		if (sigactionResult == 0) {
			const Yep32u probeResult = ProbeFunction();
			yepSyscall_rt_sigaction(SIGILL, &oldSigillAction, NULL, KERNEL_NSIG / 8);
			return probeResult;
		} else {
			return 2;
		}
	}

	#if defined(YEP_ARM_CPU)
		Yep64u _yepLibrary_ReadCoprocessor(Yep64u (*ReadFunction)()) {
			struct kernel_sigaction oldSigillAction;
			struct kernel_sigaction readSigillAction;
			memset(&readSigillAction, 0, sizeof(readSigillAction));
			readSigillAction.sa_sigaction = &_yepLibrary_ReadCoprocessorSignalHandler;
			// Needs Linux >= 2.2
			readSigillAction.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
			int sigactionResult = yepSyscall_rt_sigaction(SIGILL, &readSigillAction, &oldSigillAction, KERNEL_NSIG / 8);
			if (sigactionResult == 0) {
				const Yep64u probeResult = ReadFunction();
				yepSyscall_rt_sigaction(SIGILL, &oldSigillAction, NULL, KERNEL_NSIG / 8);
				return probeResult;
			} else {
				return 0ull;
			}
		}
	#endif
#endif
