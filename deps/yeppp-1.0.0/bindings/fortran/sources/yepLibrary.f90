!
!                      Yeppp! library implementation
!
! This file is part of Yeppp! library and licensed under 2-clause BSD license.
! See LICENSE.txt for details.

!> @defgroup yepLibrary yepLibrary: library initialization, information, and support functions.
MODULE yepLibrary
    INTERFACE
        !> @ingroup yepLibrary
        !! @defgroup yepLibrary_Init	Library initialization, deinitialization, and version information

        !> @ingroup yepLibrary_Init
        !! @brief	Initialized the @Yeppp library.
        !! @retval	0	The library is successfully initialized.
        !! @retval	10	An uncoverable error inside the OS kernel occurred during library initialization.
        !! @see	yepLibrary_Release
        INTEGER(C_INT) FUNCTION yepLibrary_Init() &
            BIND(C, NAME='yepLibrary_Init')

            USE ISO_C_BINDING, ONLY: C_INT
            IMPLICIT NONE
        END FUNCTION yepLibrary_Init
        !> @ingroup yepLibrary_Init
        !! @brief	Deinitialized the @Yeppp library and releases the consumed system resources.
        !! @retval	0	The library is successfully deinitialized.
        !! @retval	10	The library failed to release some of the resources due to a failed call to the OS kernel.
        !! @see	yepLibrary_Init
        INTEGER(C_INT) FUNCTION yepLibrary_Release() &
            BIND(C, NAME='yepLibrary_Release')

            USE ISO_C_BINDING, ONLY: C_INT
            IMPLICIT NONE
        END FUNCTION yepLibrary_Release


        !> @ingroup yepLibrary
        !! @defgroup yepLibrary_Timer	System timer access

        !> @ingroup yepLibrary_Timer
        !! @brief	Returns the number of ticks of the high-resolution system timer.
        !! @param[out]	ticks	A variable where the number of timer ticks will be stored.
        !!            	     	If the function fails, the value of this variable is not changed.
        !! @retval	0	The number of timer ticks is successfully stored to the @a ticks variable.
        !! @retval	10	An attempt to read the high-resolution timer failed inside the OS kernel.
        INTEGER(C_INT) FUNCTION yepLibrary_GetTimerTicks(t) &
            BIND(C, NAME='yepLibrary_GetTimerTicks')

            USE ISO_C_BINDING, ONLY: C_INT, C_INT64_T
            IMPLICIT NONE
            INTEGER(C_INT64_T), INTENT(OUT) :: t
        END FUNCTION yepLibrary_GetTimerTicks
        !> @ingroup yepLibrary_Timer
        !! @brief	Returns the number of ticks of the system timer per second.
        !! @param[out]	frequency	A variable where the number of timer ticks per second will be stored.
        !! @retval	0	The number of timer ticks is successfully stored to the @a frequency variable.
        !! @retval	10	An attempt to query the high-resolution timer parameters failed inside the OS kernel.
        INTEGER(C_INT) FUNCTION yepLibrary_GetTimerFrequency(f) &
            BIND(C, NAME='yepLibrary_GetTimerFrequency')

            USE ISO_C_BINDING, ONLY: C_INT, C_INT64_T
            IMPLICIT NONE
            INTEGER(C_INT64_T), INTENT(OUT) :: f
        END FUNCTION yepLibrary_GetTimerFrequency
        !> @ingroup yepLibrary_Timer
        !! @brief	Returns the minimum time difference in nanoseconds which can be measured by the high-resolution system timer.
        !! @param[out]	accuracy	A variable where the timer accuracy will be stored.
        !! @retval	0	The accuracy of the timer is successfully stored to the @a accuracy variable.
        !! @retval	10	An attempt to measure the accuracy of high-resolution timer failed inside the OS kernel.
        INTEGER(C_INT) FUNCTION yepLibrary_GetTimerAccuracy(a) &
            BIND(C, NAME='yepLibrary_GetTimerAccuracy')

            USE ISO_C_BINDING, ONLY: C_INT, C_INT64_T
            IMPLICIT NONE
            INTEGER(C_INT64_T), INTENT(OUT) :: a
        END FUNCTION yepLibrary_GetTimerAccuracy
    END INTERFACE
END MODULE yepLibrary
