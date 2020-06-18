/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under 2-clause BSD license.
 * See LICENSE.txt for details.
 *
 */

pragma(lib, "yeppp-d.lib");
pragma(lib, "yeppp.lib");

import yeppp.types;
import yeppp.library;

static import std.stdio;
static import std.conv;

int main(string[] args) {
	Status status;
	ulong state;
	ulong cycles;
	status = yepLibrary_Init();
	assert(status == Status.Ok);
	CpuArchitecture architecture;
	CpuVendor vendor;
	CpuMicroarchitecture microarchitecture;
	auto releaseName = std.conv.to!string(yepLibrary_GetVersion().releaseName);
	std.stdio.writeln(releaseName);
	
	status = yepLibrary_GetCpuArchitecture(architecture);
	assert(status == Status.Ok);
	std.stdio.writeln(std.conv.text(architecture));
	status = yepLibrary_GetCpuVendor(vendor);
	assert(status == Status.Ok);
	std.stdio.writeln(std.conv.text(vendor));
	status = yepLibrary_GetCpuMicroarchitecture(microarchitecture);
	assert(status == Status.Ok);
	std.stdio.writeln(std.conv.text(microarchitecture));
	status = yepLibrary_Release();
	assert(status == Status.Ok);

	yepLibrary_GetCpuCyclesAcquire(state);
	yepLibrary_GetCpuCyclesRelease(state, cycles);
	std.stdio.writeln(std.conv.text(cycles));
	
	ulong startTicks, endTicks, frequency, accuracy;
	yepLibrary_GetTimerTicks(startTicks);

	yepLibrary_GetTimerTicks(endTicks);
	yepLibrary_GetTimerFrequency(frequency);
	yepLibrary_GetTimerAccuracy(accuracy);
	double ticks = cast (double) (endTicks - startTicks);
	std.stdio.writeln(std.conv.text(cast (ulong) (ticks/(cast (double) frequency) * 1.0e+9), "+-", accuracy));
	
	return 0;
}
