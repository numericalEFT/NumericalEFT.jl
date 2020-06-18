class Entropy {
	
	public static void main(String[] args) {
		final int arraySize = 1024*1024*16;
		
		/* Allocate an array of probabilities */
		double[] p = new double[arraySize];

		/* Populate the array of probabilities with random probabilities */
		for (int i = 0; i < arraySize; i++) {
			/* 0 < p[i] <= 1.0 */
			p[i] = 1.0 - Math.random();
		}
		
		/* Retrieve the number of timer ticks per second */
		final long frequency = info.yeppp.Library.getTimerFrequency();
		
		/* Retrieve the number of timer ticks before calling naive entropy computation */
		final long startTimeNaive = info.yeppp.Library.getTimerTicks();
		
		final double entropyNaive = computeEntropyNaive(p);
		
		/* Retrieve the number of timer ticks after calling naive entropy computation */
		final long endTimeNaive = info.yeppp.Library.getTimerTicks();
		
		/* Retrieve the number of timer ticks before calling naive entropy computation */
		final long startTimeYeppp = info.yeppp.Library.getTimerTicks();
		
		final double entropyYeppp = computeEntropyYeppp(p);
		
		/* Retrieve the number of timer ticks after calling naive entropy computation */
		final long endTimeYeppp = info.yeppp.Library.getTimerTicks();
		
		/* Report the results */
		System.out.println("Naive implementation:");
		System.out.println(String.format("\tEntropy = %f", entropyNaive));
		System.out.println(String.format("\tTime = %f secs", ulongToDouble(endTimeNaive - startTimeNaive) / ulongToDouble(frequency)));
		System.out.println("Yeppp! implementation:");
		System.out.println(String.format("\tEntropy = %f", entropyYeppp));
		System.out.println(String.format("\tTime = %f secs", ulongToDouble(endTimeYeppp - startTimeYeppp) / ulongToDouble(frequency)));
	}
	
	/* The naive implementation of entropy computation using log function for LibM */
	private static double computeEntropyNaive(final double[] probabilities) {
		double entropy = 0.0;
		/** $\mathbb{H}\left[P\right] = - \sum_{i = 0}^{n} p_{i}\cdot\log\left(p_{i}\right) $ */
		for (int i = 0; i < probabilities.length; i++) {
			final double p = probabilities[i];
			/* Compute $p_{i}\cdot\log\left(p_{i}\right)$ and subtract it from the current entropy value */
			entropy -= p * Math.log(p);
		}
		return entropy;
	}
	
	/* The implementation of entropy computation using vector log and dot-product functions from Yeppp! library */
	/* To avoid allocating a large array for logarithms (and also to benefit from cache locality) the logarithms are computed on small blocks of the input array */
	/* The size of the block used to compute the logarithm */
	private static double computeEntropyYeppp(final double[] p) {
		double entropy = 0.0;
		final int blockSize = 1024;
		/* The small array for computed logarithms of the part of the input array */
		double[] logP = new double[blockSize];
		
		/** $\mathbb{H}\left[P\right] = - \sum_{i = 0}^{n} p_{i}\cdot\log\left(p_{i}\right) $ */
		for (int index = 0; index < p.length; index += blockSize) {
			/* Process min(BLOCK_SIZE, number of remaining elements) elements of the input array */
			int blockLength = Math.min(blockSize, p.length - index);
			
			/* Compute logarithms of probabilities in the current block of the input array */
			info.yeppp.Math.Log_V64f_V64f(p, index, logP, 0, blockLength);
			
			/* Compute the dot product of probabilities and log-probabilities of the current block */
			/* This will give minus entropy of the current block */
			final double dotProduct = info.yeppp.Core.DotProduct_V64fV64f_S64f(p, index, logP, 0, blockLength);
			
			/* Compute entropy of the current block and subtract it from the current entropy value */
			entropy -= dotProduct;
		}
		return entropy;
	}

	private static double ulongToDouble(long n) {
		return (double)(n & 0x7FFFFFFFFFFFFFFFL) - (double)(n & 0x8000000000000000L);
	}
}
