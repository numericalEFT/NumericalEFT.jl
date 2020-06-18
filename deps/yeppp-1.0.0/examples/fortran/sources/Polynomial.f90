PROGRAM polynomial
    USE yepLibrary, ONLY: yepLibrary_Init, yepLibrary_Release, yepLibrary_GetTimerFrequency, yepLibrary_GetTimerTicks
    USE yepMath, ONLY: yepMath_EvaluatePolynomial_V64fV64f_V64f
    USE ISO_C_BINDING, ONLY : C_SIZE_T, C_INT
    IMPLICIT NONE
    ! Size of the array of elements to compute the polynomial on
    INTEGER(C_SIZE_T), PARAMETER :: n = 1024*1024*8
    INTEGER(C_SIZE_T), PARAMETER :: coefCount = 101
    INTEGER(8) :: ticksStart, ticksEnd, frequency
    REAL(8) :: seconds, flops
    REAL(8) :: x(n), pYeppp(n) = 0.0d0, pNaive(n) = 0.0d0
    INTEGER(C_INT) :: s
    ! Polynomial Coefficients 101
    REAL(8), PARAMETER :: c0   = 1.53270461724076346
    REAL(8), PARAMETER :: c1   = 1.45339856462100293
    REAL(8), PARAMETER :: c2   = 1.21078763026010761
    REAL(8), PARAMETER :: c3   = 1.46952786401453397
    REAL(8), PARAMETER :: c4   = 1.34249847863665017
    REAL(8), PARAMETER :: c5   = 0.75093174077762164
    REAL(8), PARAMETER :: c6   = 1.90239336671587562
    REAL(8), PARAMETER :: c7   = 1.62162053962810579
    REAL(8), PARAMETER :: c8   = 0.53312230473555923
    REAL(8), PARAMETER :: c9   = 1.76588453111778762
    REAL(8), PARAMETER :: c10  = 1.31215699612484679
    REAL(8), PARAMETER :: c11  = 1.49636144227257237
    REAL(8), PARAMETER :: c12  = 1.52170011054112963
    REAL(8), PARAMETER :: c13  = 0.83637497322280110
    REAL(8), PARAMETER :: c14  = 1.12764540941736043
    REAL(8), PARAMETER :: c15  = 0.65513628703807597
    REAL(8), PARAMETER :: c16  = 1.15879020877781906
    REAL(8), PARAMETER :: c17  = 1.98262901973751791
    REAL(8), PARAMETER :: c18  = 1.09134643523639479
    REAL(8), PARAMETER :: c19  = 1.92898634047221235
    REAL(8), PARAMETER :: c20  = 1.01233347751449659
    REAL(8), PARAMETER :: c21  = 1.89462732589369078
    REAL(8), PARAMETER :: c22  = 1.28216239080886344
    REAL(8), PARAMETER :: c23  = 1.78448898277094016
    REAL(8), PARAMETER :: c24  = 1.22382217182612910
    REAL(8), PARAMETER :: c25  = 1.23434674193555734
    REAL(8), PARAMETER :: c26  = 1.13914782832335501
    REAL(8), PARAMETER :: c27  = 0.73506235075797319
    REAL(8), PARAMETER :: c28  = 0.55461432517332724
    REAL(8), PARAMETER :: c29  = 1.51704871121967963
    REAL(8), PARAMETER :: c30  = 1.22430234239661516
    REAL(8), PARAMETER :: c31  = 1.55001237689160722
    REAL(8), PARAMETER :: c32  = 0.84197209952298114
    REAL(8), PARAMETER :: c33  = 1.59396169927319749
    REAL(8), PARAMETER :: c34  = 0.97067044414760438
    REAL(8), PARAMETER :: c35  = 0.99001960195021281
    REAL(8), PARAMETER :: c36  = 1.17887814292622884
    REAL(8), PARAMETER :: c37  = 0.58955609453835851
    REAL(8), PARAMETER :: c38  = 0.58145654861350322
    REAL(8), PARAMETER :: c39  = 1.32447212043555583
    REAL(8), PARAMETER :: c40  = 1.24673632882394241
    REAL(8), PARAMETER :: c41  = 1.24571828921765111
    REAL(8), PARAMETER :: c42  = 1.21901343493503215
    REAL(8), PARAMETER :: c43  = 1.89453941213996638
    REAL(8), PARAMETER :: c44  = 1.85561626872427416
    REAL(8), PARAMETER :: c45  = 1.13302165522004133
    REAL(8), PARAMETER :: c46  = 1.79145993815510725
    REAL(8), PARAMETER :: c47  = 1.59227069037095317
    REAL(8), PARAMETER :: c48  = 1.89104468672467114
    REAL(8), PARAMETER :: c49  = 1.78733894997070918
    REAL(8), PARAMETER :: c50  = 1.32648559107345081
    REAL(8), PARAMETER :: c51  = 1.68531055586072865
    REAL(8), PARAMETER :: c52  = 1.08980909640581993
    REAL(8), PARAMETER :: c53  = 1.34308207822154847
    REAL(8), PARAMETER :: c54  = 1.81689492849547059
    REAL(8), PARAMETER :: c55  = 1.38582137073988747
    REAL(8), PARAMETER :: c56  = 1.04974901183570510
    REAL(8), PARAMETER :: c57  = 1.14348742300966456
    REAL(8), PARAMETER :: c58  = 1.87597730040483323
    REAL(8), PARAMETER :: c59  = 0.62131555899466420
    REAL(8), PARAMETER :: c60  = 0.64710935668225787
    REAL(8), PARAMETER :: c61  = 1.49846610600978751
    REAL(8), PARAMETER :: c62  = 1.07834176789680957
    REAL(8), PARAMETER :: c63  = 1.69130785175832059
    REAL(8), PARAMETER :: c64  = 1.64547687732258793
    REAL(8), PARAMETER :: c65  = 1.02441150427208083
    REAL(8), PARAMETER :: c66  = 1.86129006037146541
    REAL(8), PARAMETER :: c67  = 0.98309038830424073
    REAL(8), PARAMETER :: c68  = 1.75444578237500969
    REAL(8), PARAMETER :: c69  = 1.08698336765112349
    REAL(8), PARAMETER :: c70  = 1.89455010772036759
    REAL(8), PARAMETER :: c71  = 0.65812118412299539
    REAL(8), PARAMETER :: c72  = 0.62102711487851459
    REAL(8), PARAMETER :: c73  = 1.69991208083436747
    REAL(8), PARAMETER :: c74  = 1.65467704495635767
    REAL(8), PARAMETER :: c75  = 1.69599459626992174
    REAL(8), PARAMETER :: c76  = 0.82365682103308750
    REAL(8), PARAMETER :: c77  = 1.71353437063595036
    REAL(8), PARAMETER :: c78  = 0.54992984722831769
    REAL(8), PARAMETER :: c79  = 0.54717367088443119
    REAL(8), PARAMETER :: c80  = 0.79915543248858154
    REAL(8), PARAMETER :: c81  = 1.70160318364006257
    REAL(8), PARAMETER :: c82  = 1.34441280175456970
    REAL(8), PARAMETER :: c83  = 0.79789486341474966
    REAL(8), PARAMETER :: c84  = 0.61517383020710754
    REAL(8), PARAMETER :: c85  = 0.55177400048576055
    REAL(8), PARAMETER :: c86  = 1.43229889543908696
    REAL(8), PARAMETER :: c87  = 1.60658663666266949
    REAL(8), PARAMETER :: c88  = 1.78861146369896090
    REAL(8), PARAMETER :: c89  = 1.05843250742401821
    REAL(8), PARAMETER :: c90  = 1.58481799048208832
    REAL(8), PARAMETER :: c91  = 1.70954313374718085
    REAL(8), PARAMETER :: c92  = 0.52590070195022226
    REAL(8), PARAMETER :: c93  = 0.92705074709607885
    REAL(8), PARAMETER :: c94  = 0.71442651832362455
    REAL(8), PARAMETER :: c95  = 1.14752795948077643
    REAL(8), PARAMETER :: c96  = 0.89860175106926404
    REAL(8), PARAMETER :: c97  = 0.76771198245570573
    REAL(8), PARAMETER :: c98  = 0.67059202034800746
    REAL(8), PARAMETER :: c99  = 0.53785922275590729
    REAL(8), PARAMETER :: c100 = 0.82098327929734880
    REAL(8), DIMENSION (coefCount) :: coefs = (/ c0, &
         c1,  c2,  c3,  c4,  c5,  c6,  c7,  c8,  c9, c10, &
        c11, c12, c13, c14, c15, c16, c17, c18, c19, c20, &
        c21, c22, c23, c24, c25, c26, c27, c28, c29, c30, &
        c31, c32, c33, c34, c35, c36, c37, c38, c39, c40, &
        c41, c42, c43, c44, c45, c46, c47, c48, c49, c50, &
        c51, c52, c53, c54, c55, c56, c57, c58, c59, c60, &
        c61, c62, c63, c64, c65, c66, c67, c68, c69, c70, &
        c71, c72, c73, c74, c75, c76, c77, c78, c79, c80, &
        c81, c82, c83, c84, c85, c86, c87, c88, c89, c90, &
        c91, c92, c93, c94, c95, c96, c97, c98, c99, c100 /)

    s = yepLibrary_Init()
    s = yepLibrary_GetTimerFrequency(frequency)
    CALL RANDOM_NUMBER(x)
    pYeppp = 0.0d0
    pNaive = 0.0d0

    s = yepLibrary_GetTimerTicks(ticksStart)

    ! Evaluate polynomial using FORTRAN implementation
    pNaive = c0 + x * (c1 + x * (c2 + x * (c3 + x * (c4 + x * (c5 + x * (c6 + &
        x * (c7 + x * (c8 + x * (c9 + x * (c10 + x * (c11 + x * (c12 + &
        x * (c13 + x * (c14 + x * (c15 + x * (c16 + x * (c17 + x * (c18 + &
        x * (c19 + x * (c20 + x * (c21 + x * (c22 + x * (c23 + x * (c24 + &
        x * (c25 + x * (c26 + x * (c27 + x * (c28 + x * (c29 + x * (c30 + &
        x * (c31 + x * (c32 + x * (c33 + x * (c34 + x * (c35 + x * (c36 + &
        x * (c37 + x * (c38 + x * (c39 + x * (c40 + x * (c41 + x * (c42 + &
        x * (c43 + x * (c44 + x * (c45 + x * (c46 + x * (c47 + x * (c48 + &
        x * (c49 + x * (c50 + x * (c51 + x * (c52 + x * (c53 + x * (c54 + &
        x * (c55 + x * (c56 + x * (c57 + x * (c58 + x * (c59 + x * (c60 + &
        x * (c61 + x * (c62 + x * (c63 + x * (c64 + x * (c65 + x * (c66 + &
        x * (c67 + x * (c68 + x * (c69 + x * (c70 + x * (c71 + x * (c72 + &
        x * (c73 + x * (c74 + x * (c75 + x * (c76 + x * (c77 + x * (c78 + &
        x * (c79 + x * (c80 + x * (c81 + x * (c82 + x * (c83 + x * (c84 + &
        x * (c85 + x * (c86 + x * (c87 + x * (c88 + x * (c89 + x * (c90 + &
        x * (c91 + x * (c92 + x * (c93 + x * (c94 + x * (c95 + x * (c96 + &
        x * (c97 + x * (c98 + x * (c99 + x * c100)))))))))))))))))))))))))))) &
        )))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))

    s = yepLibrary_GetTimerTicks(ticksEnd)
    seconds = REAL(ticksEnd - ticksStart) / REAL(frequency)
    flops = REAL(n, 8) * REAL((coefCount - 1) * 2, 8) / seconds;
    PRINT '(A)', 'Naive'
    PRINT '(A20,F7.4, A)', "Time = ", seconds, " secs"
    PRINT '(A20,F7.4, A)', "Performance = ", flops * 1.0e-9, " GFLOPS"

    s = yepLibrary_GetTimerTicks(ticksStart)
    ! Evaluate polynomial using Yeppp!
    s = yepMath_EvaluatePolynomial_V64fV64f_V64f(coefs, x, pYeppp, coefCount, n)
    s = yepLibrary_GetTimerTicks(ticksEnd)
    seconds = REAL(ticksEnd - ticksStart) / REAL(frequency)
    flops = REAL(n, 8) * REAL((coefCount - 1) * 2, 8) / seconds;
    PRINT '(A)', 'Yeppp!'
    PRINT '(A20,F7.4, A)', "Time = ", seconds, " secs"
    PRINT '(A20,F7.4, A)', "Performance = ", flops * 1.0e-9, " GFLOPS"

    PRINT '(A,F7.3, A)', "Max relative error = ", MAXVAL(ABS((pNaive - pYeppp) / pNaive)) * 100, "%"

    s = yepLibrary_Release()
END
