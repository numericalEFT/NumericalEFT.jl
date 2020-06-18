import java.util.Random;
import java.util.Arrays;

class SystemTimer {
		
	public static void main(String[] args) {
		final int arraySize = 1024*1024*16;
		
		final int[] array = new int[arraySize];
		final Random rng = new Random();
		for (int i = 0; i < array.length; i++) {
			array[i] = rng.nextInt();
		}

		/* Retrieve the number of timer ticks per second */
		final long frequency = info.yeppp.Library.getTimerFrequency();
		
		/* Retrieve the number of timer ticks before computations */
		final long startTime = info.yeppp.Library.getTimerTicks();

		/* Do the computations */
		Arrays.sort(array);

		/* Retrieve the number of timer ticks after computations */
		final long endTime = info.yeppp.Library.getTimerTicks();
		
		/* Compute the length of computations in timer ticks */
		final long time = endTime - startTime;
		/* To convert the number of timer ticks to seconds we divide them by frequency */
		final double secs = ulongToDouble(time) / ulongToDouble(frequency);
		System.out.println(String.format("Executed in %3.2f secs\n", secs));
	}
	
	private static double ulongToDouble(long n) {
		return (double)(n & 0x7FFFFFFFFFFFFFFFL) - (double)(n & 0x8000000000000000L);
	}

}
