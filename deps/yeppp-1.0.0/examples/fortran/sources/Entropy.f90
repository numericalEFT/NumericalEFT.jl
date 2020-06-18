program entropy
    USE yepLibrary, ONLY: yepLibrary_Init, yepLibrary_Release, yepLibrary_GetTimerFrequency, yepLibrary_GetTimerTicks
    USE yepCore, ONLY: yepCore_DotProduct_V64fV64f_S64f
    USE yepMath, ONLY: yepMath_Log_V64f_V64f
    USE ISO_C_BINDING, ONLY : C_SIZE_T, C_INT64_T, C_INT, C_DOUBLE
    implicit none
    INTEGER(C_SIZE_T), PARAMETER :: n = 1024*1024*16
    INTEGER(C_INT64_T) :: ticksStart, ticksEnd, frequency
    real(C_DOUBLE) :: p(n), logP(n), H
    integer(C_INT) :: s
    s = yepLibrary_Init()
    s = yepLibrary_GetTimerFrequency(frequency)
    call RANDOM_NUMBER(p)

    s = yepLibrary_GetTimerTicks(ticksStart)
    s = yepMath_Log_V64f_V64f(p, logP, n)
    s = yepCore_DotProduct_V64fV64f_S64f(p, logP, H, n)
    H = -H
    s = yepLibrary_GetTimerTicks(ticksEnd)
    print*, "Yeppp!"
    print*, "    Entropy =", H
    print*, "    Time =", REAL(ticksEnd - ticksStart) / REAL(frequency)

    s = yepLibrary_GetTimerTicks(ticksStart)
    H = -DOT_PRODUCT(p, LOG(p))
    s = yepLibrary_GetTimerTicks(ticksEnd)
    print*, "Naive"
    print*, "    Entropy =", H
    print*, "    Time =", REAL(ticksEnd - ticksStart) / REAL(frequency)
    s = yepLibrary_Release()
end
