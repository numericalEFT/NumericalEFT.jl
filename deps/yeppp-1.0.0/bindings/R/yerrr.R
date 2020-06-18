#
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under 2-clause BSD license.
# See LICENSE.txt for details.
#
#

library(matlab, warn.conflicts=FALSE, quietly=TRUE);

dyn.load('libyerrr.so')

L <- 1024 * 1024 * 64;
cat("Generating input data..");
A <- rnorm(L);
Am <- as.matrix(A);
Aexp <- exp(A);
B <- rnorm(L);
cat(".finished\n");

cat("\nDot product:\n")
tic();
	invisible(I <- .Call("yerCore_DotProduct_V64fV64f_S64f", A, B));
cat("\tYeppp!\t", toc(echo=FALSE), " secs\n");
tic();
	invisible(J <- crossprod(A, B));
cat("\tJ <- crossprod(A, B)", toc(echo=FALSE), " secs\n");

cat("\nAdd two vectors:\n")
tic();
	invisible(X <- .Call("yerCore_Add_V64fV64f_V64f", A, B));
cat("\tYeppp!\t", toc(echo=FALSE), " secs\n");
tic();
	invisible(Y <- A + B);
cat("\tY <- A + B\t", toc(echo=FALSE), " secs\n");

cat("\nVector exp:\n")
tic();
	invisible(X <- .Call("yerMath_Exp_V64f_V64f", A));
cat("\tYeppp!\t", toc(echo=FALSE), " secs\n");
tic();
	invisible(X <- exp(A));
cat("\tX <- exp(A)\t", toc(echo=FALSE), " secs\n");

cat("\nVector log:\n")
tic();
	invisible(X <- .Call("yerMath_Log_V64f_V64f", Aexp));
cat("\tYeppp!\t", toc(echo=FALSE), " secs\n");
tic();
	invisible(X <- log(Aexp));
cat("\tX <- log(A)\t", toc(echo=FALSE), " secs\n");

cat("\nSum-of-squares:\n")
tic();
	invisible(X <- .Call("yerCore_SumSquares_V64f_S64f", A));
cat("\tYeppp!\t", toc(echo=FALSE), " secs\n");
tic();
	invisible(X <- crossprod(A, A));
cat("\tX <- crossprod(A, A)\t", toc(echo=FALSE), " secs\n");
tic();
	invisible(X <- norm(Am, type = "F"));
cat("\tX <- norm(Am, type = 'F')\t", toc(echo=FALSE), " secs\n");

dyn.unload('libyerrr.so')
