This driver enables user-mode access to performance counters and initializes
the processor cycles counter. After that user-mode programs, in particular
the Yeppp! library can read the CPU cycle counter.

The driver is intended to work on ARM v7 achitecture processors (ARM Cortex-A5,
Cortex-A7, Cortex-A8, Cortex-A9, Cortex-A15, Qualcomm Scorpion, Qualcomm Krait,
and Marvell Armada 500 Series), but probably it will also work on older ARM v6
architecture processors (ARM 1136, 1156, 1176, and 11MPCore).

Modern Linux kernels provide "perf events" subsystem, which also provides
access to CPU performance counters (and is supported by Yeppp! library).
Using "perf events" subsystem is better than relying on this driver, and
is recommended whenever possible. The "perf events" subsystem is supported
since Linux kernel version 2.6.31, but sometimes kernel is compiled without
support for it. If your Linux kernel supports "perf events" subsystem,
it will create the file /proc/sys/kernel/perf_event_paranoid. This file
sets restrictions on the use of "perf events" subsystem: in many Linux
distributions by default the access to this subsystem is too restricted,
(/proc/sys/kernel/perf_event_paranoid contains 1 or 2) and Yeppp! library
can not use it. To tighten restriction and allow Yeppp! to use this subsystem
write 0 or -1 to /proc/sys/kernel/perf_event_paranoid (you need root access to
do it).

"perf events" subsystem is superior to the functionality provided by this
driver, and should be used whenever possible. Only in cases when kernel is
compiled without "perf events" subsystem, or this subsystem is buggy and not
usable (e.g. Cortex-A9 on Ubuntu 12.04 LTS), we recommend to use this driver.
To compile the driver download kernel headers for your particular kernel version
(on Ubuntu you can just install "linux-headers" package with apt-get), and type
"make" to build the driver. After that load the driver with the command
	sudo insmod yep_arm_pmu.ko
If you will want to unload the driver, use the command
	sudo rmmod yep_arm_pmu
