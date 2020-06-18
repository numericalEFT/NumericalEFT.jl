all: yeplibrary.mod yepcore.mod yepmath.mod

FCNAME=$(notdir ${FC})
ifeq (${FCNAME},ifort)
    # Intel Fortran Compiler
    override FFLAGS += -std03
    override FFLAGS += -syntax-only
    override FFLAGS += -module modules/${YEPPLATFORM}-${FCNAME}
else ifeq (${FCNAME},pathf95)
    # PathScale Fortran Compiler
    override FFLAGS += -module modules/${YEPPLATFORM}-${FCNAME}
else ifeq (${FCNAME},pf90)
    # PGI Fortran Compiler
    override FFLAGS += -module modules/${YEPPLATFORM}-${FCNAME}
else
    # Assume GNU Fortran Compiler (e.g. x86_64-k1om-linux-gfortran or gfortran-4.8)
    override FFLAGS += -std=f2003
    override FFLAGS += -fsyntax-only
    override FFLAGS += -Jmodules/${YEPPLATFORM}-${FCNAME}/
endif

modules/${YEPPLATFORM}-${FCNAME}:
	-mkdir -p modules
	-mkdir -p modules/${YEPPLATFORM}-${FCNAME}/

yeplibrary.mod: modules/${YEPPLATFORM}-${FCNAME}
	${FC} ${FFLAGS} sources/yepLibrary.f90

yepcore.mod: modules/${YEPPLATFORM}-${FCNAME}
	${FC} ${FFLAGS} sources/yepCore.f90

yepmath.mod: modules/${YEPPLATFORM}-${FCNAME}
	${FC} ${FFLAGS} sources/yepMath.f90

clean:
	rm -f modules
