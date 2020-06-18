import java.util.Arrays;

class Polynomial {

	public static void main(String[] args) {
		/* Size of the array of elements to compute the polynomial on */
		final int arraySize = 1024*1024*8;

		/* Allocate arrays of inputs and outputs */
		final double[] x = new double[arraySize];
		final double[] pYeppp = new double[arraySize];
		final double[] pNaive = new double[arraySize];

		/* Populate the array of inputs with random data */
		for (int i = 0; i < x.length; i++) {
			x[i] = Math.random();
		}

		/* Zero-initialize the output arrays */
		Arrays.fill(pYeppp, 0.0);
		Arrays.fill(pNaive, 0.0);

		/* Retrieve the number of timer ticks per second */
		final long frequency = info.yeppp.Library.getTimerFrequency();

		/* Retrieve the number of timer ticks before calling the C version of polynomial evaluation */
		final long startTimeNaive = info.yeppp.Library.getTimerTicks();

		/* Evaluate polynomial using Java implementation */
		evaluatePolynomialNaive(x, pNaive);

		/* Retrieve the number of timer ticks after calling the C version of polynomial evaluation */
		final long endTimeNaive = info.yeppp.Library.getTimerTicks();

		/* Retrieve the number of timer ticks before calling Yeppp! polynomial evaluation */
		final long startTimeYeppp = info.yeppp.Library.getTimerTicks();

		/* Evaluate polynomial using Yeppp! */
		info.yeppp.Math.EvaluatePolynomial_V64fV64f_V64f(coefs, 0, x, 0, pYeppp, 0, coefs.length, x.length);

		/* Retrieve the number of timer ticks after calling Yeppp! polynomial evaluation */
		final long endTimeYeppp = info.yeppp.Library.getTimerTicks();

		/* Compute time in seconds and performance in FLOPS */
		double secsNaive = ulongToDouble(endTimeNaive - startTimeNaive) / ulongToDouble(frequency);
		double secsYeppp = ulongToDouble(endTimeYeppp - startTimeYeppp) / ulongToDouble(frequency);
		double flopsNaive = (double)(arraySize * (coefs.length - 1) * 2) / secsNaive;
		double flopsYeppp = (double)(arraySize * (coefs.length - 1) * 2) / secsYeppp;

		/* Report the timing and performance results */
		System.out.println("Naive implementation:");
		System.out.println(String.format("\tTime = %.2f secs", secsNaive));
		System.out.println(String.format("\tPerformance = %.2f GFLOPS", flopsNaive * 1.0e-9));
		System.out.println("Yeppp! implementation:");
		System.out.println(String.format("\tTime = %.2f secs", secsYeppp));
		System.out.println(String.format("\tPerformance = %.2f GFLOPS", flopsYeppp * 1.0e-9));

		/* Make sure the result is correct. */
		System.out.println(String.format("Max difference: %5.2f%%", computeMaxDifference(pNaive, pYeppp) * 100.0f));
	}

	/* Java implementation with hard-coded coefficients. */
	private static void evaluatePolynomialNaive(double[] xArray, double[] yArray) {
		assert xArray.length == yArray.length;
		for (int index = 0; index < xArray.length; index++) {
			double x = xArray[index];
			double y = c0 + x * (c1 + x * (c2 + x * (c3 + x * (c4 + x * (c5 + x * (c6 + x * (c7 + x * (c8 + x * (c9 + x * (c10 + x * (c11 + 
				x * (c12 + x * (c13 + x * (c14 + x * (c15 + x * (c16 + x * (c17 + x * (c18 + x * (c19 + x * (c20 + x * (c21 + 
				x * (c22 + x * (c23 + x * (c24 + x * (c25 + x * (c26 + x * (c27 + x * (c28 + x * (c29 + x * (c30 + x * (c31 +
				x * (c32 + x * (c33 + x * (c34 + x * (c35 + x * (c36 + x * (c37 + x * (c38 + x * (c39 + x * (c40 + x * (c41 +
				x * (c42 + x * (c43 + x * (c44 + x * (c45 + x * (c46 + x * (c47 + x * (c48 + x * (c49 + x * (c50 + x * (c51 +
				x * (c52 + x * (c53 + x * (c54 + x * (c55 + x * (c56 + x * (c57 + x * (c58 + x * (c59 + x * (c60 + x * (c61 +
				x * (c62 + x * (c63 + x * (c64 + x * (c65 + x * (c66 + x * (c67 + x * (c68 + x * (c69 + x * (c70 + x * (c71 +
				x * (c72 + x * (c73 + x * (c74 + x * (c75 + x * (c76 + x * (c77 + x * (c78 + x * (c79 + x * (c80 + x * (c81 +
				x * (c82 + x * (c83 + x * (c84 + x * (c85 + x * (c86 + x * (c87 + x * (c88 + x * (c89 + x * (c90 + x * (c91 +
				x * (c92 + x * (c93 + x * (c94 + x * (c95 + x * (c96 + x * (c97 + x * (c98 + x * (c99 + x * c100)
				))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
			yArray[index] = y;
		}
	}

	/* This function computes the maximum relative error between two vectors. */
	private static double computeMaxDifference(double[] xArray, double[] yArray) {
		assert xArray.length == yArray.length;
		double maxDiff = 0.0;
		for (int index = 0; index < xArray.length; index++) {
			if (xArray[index] == 0.0f)
				continue;
			final double diff = Math.abs(xArray[index] - yArray[index]) / Math.abs(xArray[index]);
			maxDiff = Math.max(maxDiff, diff);
		}
		return maxDiff;
	}

	/* Polynomial Coefficients 101 */
	private final static double c0   = 1.53270461724076346;
	private final static double c1   = 1.45339856462100293;
	private final static double c2   = 1.21078763026010761;
	private final static double c3   = 1.46952786401453397;
	private final static double c4   = 1.34249847863665017;
	private final static double c5   = 0.75093174077762164;
	private final static double c6   = 1.90239336671587562;
	private final static double c7   = 1.62162053962810579;
	private final static double c8   = 0.53312230473555923;
	private final static double c9   = 1.76588453111778762;
	private final static double c10  = 1.31215699612484679;
	private final static double c11  = 1.49636144227257237;
	private final static double c12  = 1.52170011054112963;
	private final static double c13  = 0.83637497322280110;
	private final static double c14  = 1.12764540941736043;
	private final static double c15  = 0.65513628703807597;
	private final static double c16  = 1.15879020877781906;
	private final static double c17  = 1.98262901973751791;
	private final static double c18  = 1.09134643523639479;
	private final static double c19  = 1.92898634047221235;
	private final static double c20  = 1.01233347751449659;
	private final static double c21  = 1.89462732589369078;
	private final static double c22  = 1.28216239080886344;
	private final static double c23  = 1.78448898277094016;
	private final static double c24  = 1.22382217182612910;
	private final static double c25  = 1.23434674193555734;
	private final static double c26  = 1.13914782832335501;
	private final static double c27  = 0.73506235075797319;
	private final static double c28  = 0.55461432517332724;
	private final static double c29  = 1.51704871121967963;
	private final static double c30  = 1.22430234239661516;
	private final static double c31  = 1.55001237689160722;
	private final static double c32  = 0.84197209952298114;
	private final static double c33  = 1.59396169927319749;
	private final static double c34  = 0.97067044414760438;
	private final static double c35  = 0.99001960195021281;
	private final static double c36  = 1.17887814292622884;
	private final static double c37  = 0.58955609453835851;
	private final static double c38  = 0.58145654861350322;
	private final static double c39  = 1.32447212043555583;
	private final static double c40  = 1.24673632882394241;
	private final static double c41  = 1.24571828921765111;
	private final static double c42  = 1.21901343493503215;
	private final static double c43  = 1.89453941213996638;
	private final static double c44  = 1.85561626872427416;
	private final static double c45  = 1.13302165522004133;
	private final static double c46  = 1.79145993815510725;
	private final static double c47  = 1.59227069037095317;
	private final static double c48  = 1.89104468672467114;
	private final static double c49  = 1.78733894997070918;
	private final static double c50  = 1.32648559107345081;
	private final static double c51  = 1.68531055586072865;
	private final static double c52  = 1.08980909640581993;
	private final static double c53  = 1.34308207822154847;
	private final static double c54  = 1.81689492849547059;
	private final static double c55  = 1.38582137073988747;
	private final static double c56  = 1.04974901183570510;
	private final static double c57  = 1.14348742300966456;
	private final static double c58  = 1.87597730040483323;
	private final static double c59  = 0.62131555899466420;
	private final static double c60  = 0.64710935668225787;
	private final static double c61  = 1.49846610600978751;
	private final static double c62  = 1.07834176789680957;
	private final static double c63  = 1.69130785175832059;
	private final static double c64  = 1.64547687732258793;
	private final static double c65  = 1.02441150427208083;
	private final static double c66  = 1.86129006037146541;
	private final static double c67  = 0.98309038830424073;
	private final static double c68  = 1.75444578237500969;
	private final static double c69  = 1.08698336765112349;
	private final static double c70  = 1.89455010772036759;
	private final static double c71  = 0.65812118412299539;
	private final static double c72  = 0.62102711487851459;
	private final static double c73  = 1.69991208083436747;
	private final static double c74  = 1.65467704495635767;
	private final static double c75  = 1.69599459626992174;
	private final static double c76  = 0.82365682103308750;
	private final static double c77  = 1.71353437063595036;
	private final static double c78  = 0.54992984722831769;
	private final static double c79  = 0.54717367088443119;
	private final static double c80  = 0.79915543248858154;
	private final static double c81  = 1.70160318364006257;
	private final static double c82  = 1.34441280175456970;
	private final static double c83  = 0.79789486341474966;
	private final static double c84  = 0.61517383020710754;
	private final static double c85  = 0.55177400048576055;
	private final static double c86  = 1.43229889543908696;
	private final static double c87  = 1.60658663666266949;
	private final static double c88  = 1.78861146369896090;
	private final static double c89  = 1.05843250742401821;
	private final static double c90  = 1.58481799048208832;
	private final static double c91  = 1.70954313374718085;
	private final static double c92  = 0.52590070195022226;
	private final static double c93  = 0.92705074709607885;
	private final static double c94  = 0.71442651832362455;
	private final static double c95  = 1.14752795948077643;
	private final static double c96  = 0.89860175106926404;
	private final static double c97  = 0.76771198245570573;
	private final static double c98  = 0.67059202034800746;
	private final static double c99  = 0.53785922275590729;
	private final static double c100 = 0.82098327929734880;


	/* The same coefficients as an array. This array is used for a Yeppp! function call. */
	private final static double[] coefs = {
		 c0,  c1,  c2,  c3,  c4,  c5,  c6,  c7,  c8,  c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19,
		c20, c21, c22, c23, c24, c25, c26, c27, c28, c29, c30, c31, c32, c33, c34, c35, c36, c37, c38, c39,
		c40, c41, c42, c43, c44, c45, c46, c47, c48, c49, c50, c51, c52, c53, c54, c55, c56, c57, c58, c59,
		c60, c61, c62, c63, c64, c65, c66, c67, c68, c69, c70, c71, c72, c73, c74, c75, c76, c77, c78, c79,
		c80, c81, c82, c83, c84, c85, c86, c87, c88, c89, c90, c91, c92, c93, c94, c95, c96, c97, c98, c99,
		c100
	};

	private static double ulongToDouble(long n) {
		return (double)(n & 0x7FFFFFFFFFFFFFFFL) - (double)(n & 0x8000000000000000L);
	}
}
