FCNAME=$(notdir ${FC})
override FFLAGS += -O3
override FFLAGS += -I${YEPROOT}/bindings/fortran/modules/${YEPPLATFORM}-${FCNAME}/
ifeq (${YEPPLATFORM},x64-linux-sysv-default)
    override FFLAGS += -m64
else ifeq (${YEPPLATFORM},x64-macosx-sysv-default)
    override FFLAGS += -m64
else ifeq (${YEPPLATFORM},x64-linux-k1om-default)
    ifeq (${F90},ifort)
        override FFLAGS += -mmic
    endif
else ifeq (${YEPPLATFORM},x86-linux-pic-i586)
    override FFLAGS += -m32
else ifeq (${YEPPLATFORM},x86-macosx-pic-default)
    override FFLAGS += -m32
else ifeq (${YEPPLATFORM},arm-linux-softeabi-v5t)
    
else ifeq (${YEPPLATFORM},arm-linux-hardeabi-v7a)
    
else
    $(error YEPPLATFORM variable is not defined or contains unsupported value)
endif
override LDFLAGS += -lyeppp

all : Entropy Polynomial PolynomialF

Entropy : binaries/${YEPPLATFORM}/Entropy
Polynomial : binaries/${YEPPLATFORM}/Polynomial
PolynomialF : binaries/${YEPPLATFORM}/PolynomialF

binaries/${YEPPLATFORM}:
	-mkdir -p binaries
	-mkdir -p binaries/${YEPPLATFORM}

binaries/${YEPPLATFORM}/Entropy: binaries/${YEPPLATFORM}
	${FC} ${FFLAGS} -o binaries/${YEPPLATFORM}/Entropy sources/Entropy.f90 ${LDFLAGS} -lm

binaries/${YEPPLATFORM}/Polynomial: binaries/${YEPPLATFORM}
	${FC} ${FFLAGS} -o binaries/${YEPPLATFORM}/Polynomial sources/Polynomial.f90 ${LDFLAGS}

binaries/${YEPPLATFORM}/PolynomialF: binaries/${YEPPLATFORM}
	${FC} ${FFLAGS} -o binaries/${YEPPLATFORM}/PolynomialF sources/PolynomialF.f90 ${LDFLAGS}

clean:
	-rm -rf binaries/${YEPPLATFORM}
