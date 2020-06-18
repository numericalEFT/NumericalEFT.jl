using System;

class CpuCycles {

	public static void Main(string[] args)
	{
		const int arraySize = 1024*4;
		const int iterationMax = 1000;
		
		short[] array = new short[arraySize];
		
		/* Check if the system supports performance counters */
		if (Yeppp.Library.IsSupported(Yeppp.CpuSystemFeature.CycleCounter)) {
			/* Estimate the measurement overhead */
			ulong minOverhead = UInt64.MaxValue;
			/* Repeat many times and take the minimum to filter out noise from interrupts and caches/branch prediction/page faults */
			for (int iteration = 0; iteration < iterationMax; iteration++) {
				Yeppp.CpuCycleCounterState state = Yeppp.Library.AcquireCycleCounter();

				ulong cycles = Yeppp.Library.ReleaseCycleCounter(state);
				minOverhead = Math.Min(minOverhead, cycles);
			}

			/* Now measure the cycles for computation */
			ulong minCycles = UInt64.MaxValue;
			/* Repeat many times and take the minimum to filter out noise from interrupts and caches/branch prediction/page faults */
			for (int iteration = 0; iteration < iterationMax; iteration++) {
				Yeppp.CpuCycleCounterState state = Yeppp.Library.AcquireCycleCounter();

				Random rng = new Random();
				for (int i = 0; i < arraySize; i++) {
					array[i] = (short)rng.Next();
				}

				ulong cycles = Yeppp.Library.ReleaseCycleCounter(state);
				minCycles = Math.Min(minCycles, cycles);
			}
			/* Subtract the overhead and normalize by the number of elements */
			double cpe = ((double)(minCycles - minOverhead)) / ((double)arraySize);
			Console.WriteLine("Cycles per element: {0:F2}", cpe);
		} else {
			Console.WriteLine("Processor cycle counter is not supported");
		}
	}

}
