PKG_CFLAGS := "-I../../library/headers"
PKG_LIBS := "-L../../library/binaries/x64-linux-sysv-default" -lyeppp
export PKG_CFLAGS
export PKG_LIBS

all: libyerrr.so

libyerrr.so: yerrr.c
	R CMD SHLIB -o libyerrr.so yerrr.c

check:
	LD_LIBRARY_PATH="../../library/binaries/x64-linux-sysv-default:${LD_LIBRARY_PATH}" Rscript --no-save yerrr.R

clean:
	rm -f yerrr.o libyerrr.so
