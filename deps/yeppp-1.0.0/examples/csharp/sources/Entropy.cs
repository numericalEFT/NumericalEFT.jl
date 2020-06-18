using System;

class Entropy {

	public static void Main(string[] args)
	{
		const int arraySize = 1024*1024*16;

		/* Allocate an array of probabilities */
		double[] p = new double[arraySize];

		/* Populate the array of probabilities with random probabilities */
		Random rng = new Random();
		for (int i = 0; i < arraySize; i++)
		{
			/* 0 < p[i] <= 1.0 */
			p[i] = 1.0 - rng.NextDouble();
		}

		/* Retrieve the number of timer ticks per second */
		ulong frequency = Yeppp.Library.GetTimerFrequency();

		/* Retrieve the number of timer ticks before calling naive entropy computation */
		ulong startTimeNaive = Yeppp.Library.GetTimerTicks();

		double entropyNaive = computeEntropyNaive(p);

		/* Retrieve the number of timer ticks after calling naive entropy computation */
		ulong endTimeNaive = Yeppp.Library.GetTimerTicks();

		/* Retrieve the number of timer ticks before calling safe Yeppp! entropy computation */
		ulong startTimeYepppSafe = Yeppp.Library.GetTimerTicks();

		double entropyYepppSafe = computeEntropyYepppSafe(p);

		/* Retrieve the number of timer ticks after calling safe Yeppp! computation */
		ulong endTimeYepppSafe = Yeppp.Library.GetTimerTicks();

		/* Retrieve the number of timer ticks before calling unsafe Yeppp! entropy computation */
		ulong startTimeYepppUnsafe = Yeppp.Library.GetTimerTicks();

		double entropyYepppUnsafe = computeEntropyYepppUnsafe(p);

		/* Retrieve the number of timer ticks after calling unsafe Yeppp! computation */
		ulong endTimeYepppUnsafe = Yeppp.Library.GetTimerTicks();

		/* Report the results */
		Console.WriteLine("Naive implementation:");
		Console.WriteLine("\tEntropy = {0:F}", entropyNaive);
		Console.WriteLine("\tTime = {0:F} secs", ((double)(endTimeNaive - startTimeNaive)) / ((double)(frequency)));
		Console.WriteLine("Yeppp! implementation (safe):");
		Console.WriteLine("\tEntropy = {0:F}", entropyYepppSafe);
		Console.WriteLine("\tTime = {0:F} secs", ((double)(endTimeYepppSafe - startTimeYepppSafe)) / ((double)(frequency)));
		Console.WriteLine("Yeppp! implementation (unsafe):");
		Console.WriteLine("\tEntropy = {0:F}", entropyYepppUnsafe);
		Console.WriteLine("\tTime = {0:F} secs", ((double)(endTimeYepppUnsafe - startTimeYepppUnsafe)) / ((double)(frequency)));
	}

	/* The naive implementation of entropy computation using log function for LibM */
	private static double computeEntropyNaive(double[] probabilities)
	{
		double entropy = 0.0;
		/** $\mathbb{H}\left[P\right] = - \sum_{i = 0}^{n} p_{i}\cdot\log\left(p_{i}\right) $ */
		for (int i = 0; i < probabilities.Length; i++)
		{
			double p = probabilities[i];
			/* Compute $p_{i}\cdot\log\left(p_{i}\right)$ and subtract it from the current entropy value */
			entropy -= p * Math.Log(p);
		}
		return entropy;
	}

	/* The implementation of entropy computation using vector log and dot-product functions from Yeppp! library */
	/* To avoid allocating a large array for logarithms (and also to benefit from cache locality) the logarithms are computed on small blocks of the input array */
	/* The size of the block used to compute the logarithm */
	private static double computeEntropyYepppSafe(double[] p)
	{
		double entropy = 0.0;
		const int blockSize = 1024;
		/* The small array for computed logarithms of the part of the input array */
		double[] logP = new double[blockSize];

		/** $\mathbb{H}\left[P\right] = - \sum_{i = 0}^{n} p_{i}\cdot\log\left(p_{i}\right) $ */
		for (int index = 0; index < p.Length; index += blockSize)
		{
			/* Process min(BLOCK_SIZE, number of remaining elements) elements of the input array */
			int blockLength = Math.Min(blockSize, p.Length - index);

			/* Compute logarithms of probabilities in the current block of the input array */
			Yeppp.Math.Log_V64f_V64f(p, index, logP, 0, blockLength);
			/* Compute the dot product of probabilities and log-probabilities of the current block */
			/* This will give minus entropy of the current block */
			double dotProduct = Yeppp.Core.DotProduct_V64fV64f_S64f(p, index, logP, 0, blockLength);

			/* Compute entropy of the current block and subtract it from the current entropy value */
			entropy -= dotProduct;
		}

		return entropy;
	}

	/* The implementation of entropy computation using vector log and dot-product functions from Yeppp! library */
	/* To avoid allocating a large array for logarithms (and also to benefit from cache locality) the logarithms are computed on small blocks of the input array */
	/* The size of the block used to compute the logarithm */
	private static unsafe double computeEntropyYepppUnsafe(double[] probabilities)
	{
		double entropy = 0.0;
		const int blockSize = 1024;
		/* The small array for computed logarithms of the part of the input array */
		double* logP = stackalloc double[blockSize];

		fixed (double* p = &probabilities[0])
		{
			/** $\mathbb{H}\left[P\right] = - \sum_{i = 0}^{n} p_{i}\cdot\log\left(p_{i}\right) $ */
			for (int index = 0; index < probabilities.Length; index += blockSize)
			{
				/* Process min(BLOCK_SIZE, number of remaining elements) elements of the input array */
				int blockLength = Math.Min(blockSize, probabilities.Length - index);

				/* Compute logarithms of probabilities in the current block of the input array */
				Yeppp.Math.Log_V64f_V64f(p + index, logP, blockLength);
				/* Compute the dot product of probabilities and log-probabilities of the current block */
				/* This will give minus entropy of the current block */
				double dotProduct = Yeppp.Core.DotProduct_V64fV64f_S64f(p + index, logP, blockLength);

				/* Compute entropy of the current block and subtract it from the current entropy value */
				entropy -= dotProduct;
			}
		}

		return entropy;
	}
}
