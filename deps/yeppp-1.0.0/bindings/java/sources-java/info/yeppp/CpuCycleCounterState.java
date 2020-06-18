/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	The state of the processor cycle counter.
 * @details	This class is intended to use only through Library#acquireCycleCounter and Library#releaseCycleCounter methods.
 * @see	Library#acquireCycleCounter(), Library#releaseCycleCounter(CpuCycleCounterState)
 */
public final class CpuCycleCounterState {

	protected CpuCycleCounterState(long state) {
		this.state = state;
	}

	protected long state;

};
