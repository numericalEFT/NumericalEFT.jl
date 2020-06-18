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

#pragma once

// PMCR = Performance Monitors Control Register
inline int read_pmcr(void) {
	int pmcr;
	asm ("MRC p15, 0, %[pmcr], c9, c12, 0;"
		: [pmcr] "=r" (pmcr)
		:
		:
	);
	return pmcr;
}

inline void write_pmcr(int pmcr) {
	asm ("MCR p15, 0, %[pmcr], c9, c12, 0;"
		:
		: [pmcr] "r" (pmcr)
		:
	);
}

// MUSERENR = Performance Monitors User Enable Register
inline int read_pmuserenr(void) {
	int pmuserenr;
	asm ("MRC p15, 0, %[pmuserenr], c9, c14, 0;"
		: [pmuserenr] "=r" (pmuserenr)
		:
		:
	);
	return pmuserenr;
}

inline void write_pmuserenr(int pmuserenr) {
	asm ("MCR p15, 0, %[pmuserenr], c9, c14, 0;"
		:
		: [pmuserenr] "r" (pmuserenr)
		:
	);
}

// PMCCNTR = Performance Monitors Cycle Count Register
inline int read_pmccntr(void) {
	int pmccntr;
	asm ("MRC p15, 0, %[pmccntr], c9, c13, 0;"
		: [pmccntr] "=r" (pmccntr)
		:
		:
	);
	return pmccntr;
}

inline void write_pmccntr(int pmccntr) {
	asm ("MCR p15, 0, %[pmccntr], c9, c13, 0;"
		:
		: [pmccntr] "r" (pmccntr)
		:
	);
}

// PMCEID0 = Performance Monitors Common Event ID Register 0
inline int read_pmceid0(void) {
	int pmceid0;
	asm ("MRC p15, 0, %[pmceid0], c9, c12, 6;"
		: [pmceid0] "=r" (pmceid0)
		:
		:
	);
	return pmceid0;
}

// PMCEID1 = Performance Monitors Common Event ID Register 1
inline int read_pmceid1(void) {
	int pmceid1;
	asm ("MRC p15, 0, %[pmceid1], c9, c12, 7;"
		: [pmceid1] "=r" (pmceid1)
		:
		:
	);
	return pmceid1;
}

// PMCNTENCLR = Performance Monitors Count Enable Clear Register
inline int read_pmcntenclr(void) {
	int pmcntenclr;
	asm ("MRC p15, 0, %[pmcntenclr], c9, c12, 2;"
		: [pmcntenclr] "=r" (pmcntenclr)
		:
		:
	);
	return pmcntenclr;
}

inline void write_pmcntenclr(int pmcntenclr) {
	asm ("MCR p15, 0, %[pmcntenclr], c9, c12, 2;"
		: [pmcntenclr] "=r" (pmcntenclr)
		:
		:
	);
}

// PMCNTENSET = Performance Monitors Count Enable Set Register
inline int read_pmcntenset(void) {
	int pmcntenset;
	asm ("MRC p15, 0, %[pmcntenset], c9, c12, 1;"
		: [pmcntenset] "=r" (pmcntenset)
		:
		:
	);
	return pmcntenset;
}

inline void write_pmcntenset(int pmcntenset) {
	asm ("MCR p15, 0, %[pmcntenset], c9, c12, 1;"
		:
		: [pmcntenset] "r" (pmcntenset)
		:
	);
}

// PMINTENCLR = Performance Monitors Interrupt Enable Clear Register
inline int read_pmintenclr(void) {
	int pmintenclr;
	asm ("MRC p15, 0, %[pmintenclr], c9, c14, 2;"
		: [pmintenclr] "=r" (pmintenclr) 
		:
		:
	);
	return pmintenclr;
}

inline void write_pmintenclr(int pmintenclr) {
	asm ("MCR p15, 0, %[pmintenclr], c9, c14, 2;"
		: 
		: [pmintenclr] "r" (pmintenclr)
		:
	);
}

// PMINTENSET = Performance Monitors Interrupt Enable Set Register
inline int read_pmintenset(void) {
	int pmintenset;
	asm ("MRC p15, 0, %[pmintenset], c9, c14, 1;"
		: [pmintenset] "=r" (pmintenset) 
		:
		:
	);
	return pmintenset;
}

inline void write_pmintenset(int pmintenset) {
	asm ("MCR p15, 0, %[pmintenset], c9, c14, 1;"
		:
		: [pmintenset] "r" (pmintenset)
		:
	);
}
