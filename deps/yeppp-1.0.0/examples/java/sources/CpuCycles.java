import java.util.Random;

class CpuCycles {
	
	public static void main(String[] args) {
		final int arraySize = 1024*4;
		final int iterationMax = 1000;
		
		final short[] array = new short[arraySize];
		
		/* Check if the system supports performance counters */
		if (info.yeppp.Library.isSupported(info.yeppp.CpuSystemFeature.CycleCounter)) {
			/* Estimate the measurement overhead */
			long minOverhead = ulongMax();
			/* Repeat many times and take the minimum to filter out noise from interrupts and caches/branch prediction/page faults */
			for (int iteration = 0; iteration < iterationMax; iteration++) {
				final info.yeppp.CpuCycleCounterState state = info.yeppp.Library.acquireCycleCounter();

				final long cycles = info.yeppp.Library.releaseCycleCounter(state);
				minOverhead = minu(minOverhead, cycles);
			}

			/* Now measure the cycles for computation */
			long minCycles = ulongMax();
			/* Repeat many times and take the minimum to filter out noise from interrupts and caches/branch prediction/page faults */
			for (int iteration = 0; iteration < iterationMax; iteration++) {
				final info.yeppp.CpuCycleCounterState state = info.yeppp.Library.acquireCycleCounter();

				final Random rng = new Random(42l);
				for (int i = 0; i < arraySize; i++) {
					array[i] = (short)rng.nextInt();
				}

				final long cycles = info.yeppp.Library.releaseCycleCounter(state);
				minCycles = minu(minCycles, cycles);
			}
			/* Subtract the overhead and normalize by the number of elements */
			final double cpe = ulongToDouble(minCycles - minOverhead) / ((double)arraySize);
			System.out.println(String.format("Cycles per element: %3.2f", cpe));
		} else {
			System.out.println("Processor cycle counter is not supported");
		}
	}
	
	private static long ulongMax() {
		return 0xFFFFFFFFFFFFFFFFL;
	}

	private static long minu(long a, long b) {
		if ((a ^ 0x8000000000000000L) < (b ^ 0x8000000000000000L)) {
			return a;
		} else {
			return b;
		}
	}
	
	private static double ulongToDouble(long n) {
		return (double)(n & 0x7FFFFFFFFFFFFFFFL) - (double)(n & 0x8000000000000000L);
	}
}
