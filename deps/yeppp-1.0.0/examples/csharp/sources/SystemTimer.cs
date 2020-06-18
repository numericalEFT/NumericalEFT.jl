using System;

class SystemTimer
{

	public static void Main(string[] args)
	{
		const int arraySize = 1024*1024*16;

		/* Allocate an array of numbers */
		int[] array = new int[arraySize];

		/* Populate the array with random numbers */
		Random rng = new Random();
		for (int i = 0; i < arraySize; i++)
		{
			array[i] = rng.Next();
		}

		/* Retrieve the number of timer ticks per second */
		ulong frequency = Yeppp.Library.GetTimerFrequency();

		/* Retrieve the number of timer ticks before computations */
		ulong startTime = Yeppp.Library.GetTimerTicks();

		/* Do the computations */
		Array.Sort(array);

		/* Retrieve the number of timer ticks after computations */
		ulong endTime = Yeppp.Library.GetTimerTicks();

		/* Compute the length of computations in timer ticks */
		ulong time = endTime - startTime;
		/* To convert the number of timer ticks to seconds we divide them by frequency */
		double timeSecs = ((double)time) / ((double)frequency);
		Console.WriteLine("Executed in {0:F2} secs", timeSecs);
	}

}