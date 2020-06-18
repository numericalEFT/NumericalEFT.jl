/*
 * Copyright (c) 2012-2013 Georgia Institute of Technology
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies, 
 * either expressed or implied, of the Yeppp! Project.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "yep_arm_pmu.h"

MODULE_AUTHOR("Marat Dukhan (www.cc.gatech.edu/grads/m/mdukhan3/)");
MODULE_DESCRIPTION("Enables user-mode access to ARMv7 performance counters");
MODULE_LICENSE("Dual BSD/GPL");

static void cpucounter_enable(void* reserved) {
	int pmuserenr, pmcr;

	// PMUSERENR[bit 31] = PMCCNTR overflow interrupt request disable bit (write: disable cycle count overflow interrupts)
	// TODO: Disable interrupts for other events.
	write_pmintenclr(0x80000000u);

	// PMCNTENCLR[bit 31] = PMCCNTR disable bit (write: disable cycle counter)
	write_pmcntenclr(0x80000000u);

	// PMCR[bit 0] = E (Enable all counters, including PMCNTR)
	// PMCR[bit 1] = P (Reset all counters, expect PMCNTR)
	// PMCR[bit 2] = C (Reset PMCNTR)
	// PMCR[bit 3] = D (Cycle counter clock divider, switches to counting every 64 cycles)
	// PMCR[bit 4] = X (Export of events to an external debug device)
	// PMCR[bit 5] = DP (Disable PMCCNTR when event counting is prohibited)
	// PMCR[bits 11:15] = N (Number of event counters)
	pmcr = read_pmcr();
	pmcr &= ~0x0000003Fu;
	pmcr |= 0x00000015u;
	write_pmcr(pmcr);

	// PMCNTENSET[bit 31] = PMCCNTR enable bit (write: enable cycle counter)
	write_pmcntenset(0x80000000u);

	pmuserenr = read_pmuserenr();
	// PMUSERENR[bit 0] = EN (user-mode access to performance counters)
	pmuserenr |= 0x00000001u;
	write_pmuserenr(pmuserenr);
}

static void cpucounter_disable(void* reserved) {
	int pmuserenr, pmcr;
	
	// PMCNTENCLR[bit 31] = PMCCNTR disable bit (write: disable cycle counter)
	// TODO: Disable all enabled event counters.
	write_pmcntenclr(0x80000000u);

	// PMCR[bit 0] = E (Enable all counters, including PMCNTR)
	// PMCR[bit 1] = P (Reset all counters, expect PMCNTR)
	// PMCR[bit 2] = C (Reset PMCNTR)
	// PMCR[bit 3] = D (Cycle counter clock divider, switches to counting every 64 cycles)
	// PMCR[bit 4] = X (Export of events to an external debug device)
	// PMCR[bit 5] = DP (Disable PMCCNTR when event counting is prohibited)
	// PMCR[bits 11:15] = N (Number of event counters)
	pmcr = read_pmcr();
	pmcr &= ~0x0000003Fu;
	write_pmcr(pmcr);

	pmuserenr = read_pmuserenr();
	// PMUSERENR[bit 0] = EN (user-mode access to performance counters)
	pmuserenr &= ~0x00000001u;
	write_pmuserenr(pmuserenr);
}

static int cpucounter_init(void) {
	printk(KERN_INFO "Enabling user-mode access to performance counter\n");
	cpucounter_enable(NULL);
	// Execute cpucounter_enable on all other cores, block until finished
	smp_call_function(cpucounter_enable, NULL, 1);
	return 0;
}

static void cpucounter_exit(void) {
	printk(KERN_INFO "Disabling user-mode access to performance counter\n");
	cpucounter_disable(NULL);
	// Execute cpucounter_disable on all other cores, block until finished
	smp_call_function(cpucounter_disable, NULL, 1);
}

module_init(cpucounter_init);
module_exit(cpucounter_exit);
