help:
	$(info "======================================================================")
	$(info "Yeppp! runtime library: minimalistic C/C++ compiler runtime")
	$(info "")
	$(info "Valid build targets:")
	$(info "")
	$(info "       x64-linux-sysv-default : build for GNU/Linux on x86-64")
	$(info "       x64-linux-k1om-default : build for GNU/Linux on Xeon Phi")
	$(info "           x86-linux-pic-i586 : build for GNU/Linux on i586")
	$(info "       arm-linux-softeabi-v5t : build for GNU/Linux on ARM (armel)")
	$(info "       arm-linux-hardeabi-v7a : build for GNU/Linux on ARM (armhf)")
	$(info "   arm-linux-softeabi-android : build for Android ARMEABI ABI")
	$(info "arm-linux-softeabi-androidv7a : build for Android ARMEABIv7A ABI")
	$(info "        x86-linux-pic-android : build for Android x86 ABI")
	$(info "       mips-linux-o32-android : build for Android MIPS ABI")
	$(info "      x64-macosx-sysv-default : build for Mac OS X on x86-64")
	$(info "       x86-macosx-pic-default : build for Mac OS X on x86")
	$(info "                        clean : delete all binaries")
	$(info "======================================================================")

arm-linux-softeabi-android: CC=arm-linux-androideabi-gcc
arm-linux-softeabi-androidv7a: CC=arm-linux-androideabi-gcc
arm-linux-softeabi-v5t: CC=arm-linux-gnueabi-gcc
arm-linux-hardeabi-v7a: CC=arm-linux-gnueabihf-gcc
mips-linux-o32-android: CC=mipsel-linux-android-gcc

arm-linux-softeabi-android: CCFLAGS=-nostdlib -marm -fPIC -ffunction-sections -fdata-sections -Wno-psabi -march=armv5te -mtune=arm1136j-s -mfloat-abi=soft -fno-unwind-tables -O3 -fomit-frame-pointer -I$(ANDROID_NDK_ROOT)/platforms/android-3/arch-arm/usr/include -Wa,--noexecstack
arm-linux-softeabi-androidv7a: CCFLAGS=-nostdlib -marm -fPIC -ffunction-sections -fdata-sections -Wno-psabi -march=armv7-a -mtune=cortex-a9 -mfloat-abi=softfp -fno-unwind-tables -O3 -fomit-frame-pointer -I$(ANDROID_NDK_ROOT)/platforms/android-3/arch-arm/usr/include -Wa,--noexecstack
arm-linux-softeabi-v5t: CCFLAGS=-nostdlib -marm -fPIC -ffunction-sections -fdata-sections -Wno-psabi -march=armv5t -mtune=arm1136j-s -mfloat-abi=soft -fno-unwind-tables -O3 -fomit-frame-pointer -Wa,--noexecstack
arm-linux-hardeabi-v7a: CCFLAGS=-nostdlib -marm -fPIC -ffunction-sections -fdata-sections -Wno-psabi -march=armv7-a -mtune=cortex-a9 -mfloat-abi=hard -fno-unwind-tables -O3 -fomit-frame-pointer -Wa,--noexecstack
mips-linux-o32-android: CCFLAGS=-nostdlib -fPIC -ffunction-sections -fdata-sections -Wno-psabi -march=mips32 -fno-unwind-tables -O3 -fomit-frame-pointer -I$(ANDROID_NDK_ROOT)/platforms/android-9/arch-mips/usr/include -Wa,--noexecstack

arm-linux-softeabi-androidv7a: AS=arm-linux-androideabi-as
arm-linux-softeabi-android: AS=arm-linux-androideabi-as
arm-linux-hardeabi-v7a: AS=arm-linux-gnueabihf-as
arm-linux-softeabi-v5t: AS=arm-linux-gnueabi-as
x86-linux-pic-android: AS=nasm
x86-linux-pic-i586: AS=nasm
x86-macosx-pic-default: AS=nasm
x64-linux-sysv-default: AS=nasm
x64-linux-k1om-default: AS=x86_64-k1om-linux-as
x64-macosx-sysv-default: AS=nasm
mips-linux-o32-android: AS=mipsel-linux-android-as

arm-linux-softeabi-androidv7a: ASFLAGS=-Isources/arm --noexecstack -march=armv7-a
arm-linux-softeabi-android: ASFLAGS=-Isources/arm --noexecstack -march=armv5te
arm-linux-hardeabi-v7a: ASFLAGS=-Isources/arm --noexecstack -march=armv7-a
arm-linux-softeabi-v5t: ASFLAGS=-Isources/arm --noexecstack -march=armv5t
x86-linux-pic-android: ASFLAGS=-f elf32
x86-linux-pic-i586: ASFLAGS=-f elf32
x86-macosx-pic-default: ASFLAGS=-f macho32
x64-linux-sysv-default: ASFLAGS=-f elf64
x64-macosx-sysv-default: ASFLAGS=-f macho64
x64-linux-k1om-default: ASFLAGS=-Isources/x86 --march=k1om --noexecstack
mips-linux-o32-android: ASFLAGS=-Isources/mips --noexecstack

arm-linux-softeabi-androidv7a: STRIP=arm-linux-androideabi-strip
arm-linux-softeabi-android: STRIP=arm-linux-androideabi-strip
arm-linux-hardeabi-v7a: STRIP=arm-linux-gnueabihf-strip
arm-linux-softeabi-v5t: STRIP=arm-linux-gnueabi-strip
x86-linux-pic-android: STRIP=i686-linux-android-strip
x86-linux-pic-i586: STRIP=strip
x86-macosx-pic-default: STRIP=strip
x64-linux-sysv-default: STRIP=strip
x64-linux-k1om-default: STRIP=x86_64-k1om-linux-strip
x64-macosx-sysv-default: STRIP=strip
mips-linux-o32-android: STRIP=mipsel-linux-android-strip

arm-linux-softeabi-androidv7a: AR=arm-linux-androideabi-ar
arm-linux-softeabi-android: AR=arm-linux-androideabi-ar
arm-linux-hardeabi-v7a: AR=arm-linux-gnueabihf-ar
arm-linux-softeabi-v5t: AR=arm-linux-gnueabi-ar
x86-linux-pic-android: AR=i686-linux-android-ar
x86-linux-pic-i586: AR=ar
x86-macosx-pic-default: AR=ar
x64-linux-sysv-default: AR=ar
x64-linux-k1om-default: AR=x86_64-k1om-linux-ar
x64-macosx-sysv-default: AR=ar
mips-linux-o32-android: AR=mipsel-linux-android-ar

arm-linux-softeabi-androidv7a:
	-rm -rf binaries/arm-linux-softeabi-androidv7a
	-mkdir -p binaries
	-mkdir -p binaries/arm-linux-softeabi-androidv7a
	
	$(AS) $(ASFLAGS) sources/arm/aeabi_llsl.asm -o binaries/arm-linux-softeabi-androidv7a/aeabi_llsl.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_lmul.asm -o binaries/arm-linux-softeabi-androidv7a/aeabi_lmul.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ul2d.asm -o binaries/arm-linux-softeabi-androidv7a/aeabi_ul2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uidiv.asm -o binaries/arm-linux-softeabi-androidv7a/aeabi_uidiv.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uidivmod.asm -o binaries/arm-linux-softeabi-androidv7a/aeabi_uidivmod.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uldivmod.asm -o binaries/arm-linux-softeabi-androidv7a/aeabi_uldivmod.o

	$(CC) $(CCFLAGS) -c sources/arm/udivmoddi4.c -o binaries/arm-linux-softeabi-androidv7a/udivmoddi4.o

	$(CC) $(CCFLAGS) -c sources/arm/div_tables.c -o binaries/arm-linux-softeabi-androidv7a/div_tables.o

	$(AS) $(ASFLAGS) sources/arm/memcpy.asm -o binaries/arm-linux-softeabi-androidv7a/memcpy.o

	$(AS) $(ASFLAGS) sources/arm/memcmp.asm -o binaries/arm-linux-softeabi-androidv7a/memcmp.o

	$(AS) $(ASFLAGS) sources/arm/memset.asm -o binaries/arm-linux-softeabi-androidv7a/memset.o

	$(AR) rcs binaries/arm-linux-softeabi-androidv7a/libyeprt.a binaries/arm-linux-softeabi-androidv7a/*.o

arm-linux-softeabi-android:
	-rm -rf binaries/arm-linux-softeabi-android
	-mkdir -p binaries
	-mkdir -p binaries/arm-linux-softeabi-android
	
	$(AS) $(ASFLAGS) sources/arm/gnu_thumb1_case.asm -o binaries/arm-linux-softeabi-android/gnu_thumb1_case.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_llsl.asm -o binaries/arm-linux-softeabi-android/aeabi_llsl.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_lmul.asm -o binaries/arm-linux-softeabi-android/aeabi_lmul.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ul2d.asm -o binaries/arm-linux-softeabi-android/aeabi_ul2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uidiv.asm -o binaries/arm-linux-softeabi-android/aeabi_uidiv.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uidivmod.asm -o binaries/arm-linux-softeabi-android/aeabi_uidivmod.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uldivmod.asm -o binaries/arm-linux-softeabi-android/aeabi_uldivmod.o

	$(CC) $(CCFLAGS) -c sources/arm/udivmoddi4.c -o binaries/arm-linux-softeabi-android/udivmoddi4.o

	$(CC) $(CCFLAGS) -c sources/arm/div_tables.c -o binaries/arm-linux-softeabi-android/div_tables.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fmul.asm -o binaries/arm-linux-softeabi-android/aeabi_fmul.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_dmul.c -o binaries/arm-linux-softeabi-android/aeabi_dmul.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_ddiv.c -o binaries/arm-linux-softeabi-android/aeabi_ddiv.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_fadd.c -o binaries/arm-linux-softeabi-android/aeabi_fadd.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_dadd.c -o binaries/arm-linux-softeabi-android/aeabi_dadd.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_fsub.c -o binaries/arm-linux-softeabi-android/aeabi_fsub.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_dsub.c -o binaries/arm-linux-softeabi-android/aeabi_dsub.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmpeq.asm -o binaries/arm-linux-softeabi-android/aeabi_dcmpeq.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmpge.asm -o binaries/arm-linux-softeabi-android/aeabi_dcmpge.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmpgt.asm -o binaries/arm-linux-softeabi-android/aeabi_dcmpgt.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmple.asm -o binaries/arm-linux-softeabi-android/aeabi_dcmple.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmplt.asm -o binaries/arm-linux-softeabi-android/aeabi_dcmplt.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmpge.asm -o binaries/arm-linux-softeabi-android/aeabi_fcmpge.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmpgt.asm -o binaries/arm-linux-softeabi-android/aeabi_fcmpgt.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmple.asm -o binaries/arm-linux-softeabi-android/aeabi_fcmple.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmplt.asm -o binaries/arm-linux-softeabi-android/aeabi_fcmplt.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmpeq.asm -o binaries/arm-linux-softeabi-android/aeabi_fcmpeq.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmpne.asm -o binaries/arm-linux-softeabi-android/aeabi_fcmpne.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ul2d.asm -o binaries/arm-linux-softeabi-android/aeabi_ul2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_i2d.asm -o binaries/arm-linux-softeabi-android/aeabi_i2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ui2d.asm -o binaries/arm-linux-softeabi-android/aeabi_ui2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ui2f.asm -o binaries/arm-linux-softeabi-android/aeabi_ui2f.o

	$(AS) $(ASFLAGS) sources/arm/memcpy.asm -o binaries/arm-linux-softeabi-android/memcpy.o

	$(AS) $(ASFLAGS) sources/arm/memcmp.asm -o binaries/arm-linux-softeabi-android/memcmp.o

	$(AS) $(ASFLAGS) sources/arm/memset.asm -o binaries/arm-linux-softeabi-android/memset.o

	$(AR) rcs binaries/arm-linux-softeabi-android/libyeprt.a binaries/arm-linux-softeabi-android/*.o

arm-linux-hardeabi-v7a:
	-rm -rf binaries/arm-linux-hardeabi-v7a
	-mkdir -p binaries
	-mkdir -p binaries/arm-linux-hardeabi-v7a
	
	$(AS) $(ASFLAGS) sources/arm/aeabi_llsl.asm -o binaries/arm-linux-hardeabi-v7a/aeabi_llsl.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_lmul.asm -o binaries/arm-linux-hardeabi-v7a/aeabi_lmul.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ul2d.asm -o binaries/arm-linux-hardeabi-v7a/aeabi_ul2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uidiv.asm -o binaries/arm-linux-hardeabi-v7a/aeabi_uidiv.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uidivmod.asm -o binaries/arm-linux-hardeabi-v7a/aeabi_uidivmod.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uldivmod.asm -o binaries/arm-linux-hardeabi-v7a/aeabi_uldivmod.o

	$(CC) $(CCFLAGS) -c sources/arm/udivmoddi4.c -o binaries/arm-linux-hardeabi-v7a/udivmoddi4.o

	$(CC) $(CCFLAGS) -c sources/arm/div_tables.c -o binaries/arm-linux-hardeabi-v7a/div_tables.o

	$(AS) $(ASFLAGS) sources/arm/memcpy.asm -o binaries/arm-linux-hardeabi-v7a/memcpy.o

	$(AS) $(ASFLAGS) sources/arm/memcmp.asm -o binaries/arm-linux-hardeabi-v7a/memcmp.o

	$(AS) $(ASFLAGS) sources/arm/memset.asm -o binaries/arm-linux-hardeabi-v7a/memset.o

	$(AR) rcs binaries/arm-linux-hardeabi-v7a/libyeprt.a binaries/arm-linux-hardeabi-v7a/*.o

arm-linux-softeabi-v5t:
	-rm -rf binaries/arm-linux-softeabi-v5t
	-mkdir -p binaries
	-mkdir -p binaries/arm-linux-softeabi-v5t

	$(AS) $(ASFLAGS) sources/arm/gnu_thumb1_case.asm -o binaries/arm-linux-softeabi-v5t/gnu_thumb1_case.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ulcmp.asm -o binaries/arm-linux-softeabi-v5t/aeabi_ulcmp.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_llsl.asm -o binaries/arm-linux-softeabi-v5t/aeabi_llsl.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_lmul.asm -o binaries/arm-linux-softeabi-v5t/aeabi_lmul.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ul2d.asm -o binaries/arm-linux-softeabi-v5t/aeabi_ul2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uidiv.asm -o binaries/arm-linux-softeabi-v5t/aeabi_uidiv.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uidivmod.asm -o binaries/arm-linux-softeabi-v5t/aeabi_uidivmod.o

	$(CC) $(CCFLAGS) -c sources/arm/div_tables.c -o binaries/arm-linux-softeabi-v5t/div_tables.o

	$(CC) $(CCFLAGS) -c sources/arm/udivmoddi4.c -o binaries/arm-linux-softeabi-v5t/udivmoddi4.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_uldivmod.asm -o binaries/arm-linux-softeabi-v5t/aeabi_uldivmod.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fmul.asm -o binaries/arm-linux-softeabi-v5t/aeabi_fmul.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_dmul.c -o binaries/arm-linux-softeabi-v5t/aeabi_dmul.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_ddiv.c -o binaries/arm-linux-softeabi-v5t/aeabi_ddiv.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_fadd.c -o binaries/arm-linux-softeabi-v5t/aeabi_fadd.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_dadd.c -o binaries/arm-linux-softeabi-v5t/aeabi_dadd.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_fsub.c -o binaries/arm-linux-softeabi-v5t/aeabi_fsub.o

	$(CC) $(CCFLAGS) -c sources/arm/aeabi_dsub.c -o binaries/arm-linux-softeabi-v5t/aeabi_dsub.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmpeq.asm -o binaries/arm-linux-softeabi-v5t/aeabi_dcmpeq.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmpge.asm -o binaries/arm-linux-softeabi-v5t/aeabi_dcmpge.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmpgt.asm -o binaries/arm-linux-softeabi-v5t/aeabi_dcmpgt.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmple.asm -o binaries/arm-linux-softeabi-v5t/aeabi_dcmple.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_dcmplt.asm -o binaries/arm-linux-softeabi-v5t/aeabi_dcmplt.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmpge.asm -o binaries/arm-linux-softeabi-v5t/aeabi_fcmpge.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmpgt.asm -o binaries/arm-linux-softeabi-v5t/aeabi_fcmpgt.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmple.asm -o binaries/arm-linux-softeabi-v5t/aeabi_fcmple.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmplt.asm -o binaries/arm-linux-softeabi-v5t/aeabi_fcmplt.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmpeq.asm -o binaries/arm-linux-softeabi-v5t/aeabi_fcmpeq.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_fcmpne.asm -o binaries/arm-linux-softeabi-v5t/aeabi_fcmpne.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_i2d.asm -o binaries/arm-linux-softeabi-v5t/aeabi_i2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ui2d.asm -o binaries/arm-linux-softeabi-v5t/aeabi_ui2d.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_ui2f.asm -o binaries/arm-linux-softeabi-v5t/aeabi_ui2f.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_llsl.asm -o binaries/arm-linux-softeabi-v5t/aeabi_llsl.o

	$(AS) $(ASFLAGS) sources/arm/aeabi_lmul.asm -o binaries/arm-linux-softeabi-v5t/aeabi_lmul.o

	$(AS) $(ASFLAGS) sources/arm/memcpy.asm -o binaries/arm-linux-softeabi-v5t/memcpy.o

	$(AS) $(ASFLAGS) sources/arm/memcmp.asm -o binaries/arm-linux-softeabi-v5t/memcmp.o

	$(AS) $(ASFLAGS) sources/arm/memset.asm -o binaries/arm-linux-softeabi-v5t/memset.o

	$(AR) rcs binaries/arm-linux-softeabi-v5t/libyeprt.a binaries/arm-linux-softeabi-v5t/*.o

x86-linux-pic-android:
	-rm -rf binaries/x86-linux-pic-android
	-mkdir -p binaries
	-mkdir -p binaries/x86-linux-pic-android
	
	$(AS) $(ASFLAGS) sources/x86/memcpy.x86.asm -o binaries/x86-linux-pic-android/memcpy.o

	$(AS) $(ASFLAGS) sources/x86/memcmp.x86.asm -o binaries/x86-linux-pic-android/memcmp.x86.o

	$(AS) $(ASFLAGS) sources/x86/memset.x86.asm -o binaries/x86-linux-pic-android/memset.x86.o

	$(AS) $(ASFLAGS) sources/x86/udivdi3.x86.asm -o binaries/x86-linux-pic-android/udivdi3.x86.o

	$(AR) rcs binaries/x86-linux-pic-android/libyeprt.a binaries/x86-linux-pic-android/*.o

x86-linux-pic-i586:
	-rm -rf binaries/x86-linux-pic-i586
	-mkdir -p binaries
	-mkdir -p binaries/x86-linux-pic-i586
	
	$(AS) $(ASFLAGS) -o binaries/x86-linux-pic-i586/memcpy.o sources/x86/memcpy.x86.asm 

	$(AS) $(ASFLAGS) -o binaries/x86-linux-pic-i586/memcmp.x86.o sources/x86/memcmp.x86.asm

	$(AS) $(ASFLAGS) -o binaries/x86-linux-pic-i586/memset.x86.o sources/x86/memset.x86.asm

	$(AS) $(ASFLAGS) -o binaries/x86-linux-pic-i586/udivdi3.x86.o sources/x86/udivdi3.x86.asm

	$(AR) rcs binaries/x86-linux-pic-i586/libyeprt.a binaries/x86-linux-pic-i586/*.o

x86-macosx-pic-default:
	-rm -rf binaries/x86-macosx-pic-default
	-mkdir -p binaries
	-mkdir -p binaries/x86-macosx-pic-default
	
	$(AS) $(ASFLAGS) -o binaries/x86-macosx-pic-default/memcpy.o sources/x86/memcpy.x86.asm 

	$(AS) $(ASFLAGS) -o binaries/x86-macosx-pic-default/memcmp.x86.o sources/x86/memcmp.x86.asm

	$(AS) $(ASFLAGS) -o binaries/x86-macosx-pic-default/memset.x86.o sources/x86/memset.x86.asm

	$(AS) $(ASFLAGS) -o binaries/x86-macosx-pic-default/udivdi3.x86.o sources/x86/udivdi3.x86.asm

	$(AR) rcs binaries/x86-macosx-pic-default/libyeprt.a binaries/x86-macosx-pic-default/*.o

x64-linux-k1om-default:
	-rm -rf binaries/x64-linux-k1om-default
	-mkdir -p binaries
	-mkdir -p binaries/x64-linux-k1om-default
	
	$(AS) $(ASFLAGS) sources/x86/memcmp.x64-k1om.asm -o binaries/x64-linux-k1om-default/memcmp.x64-k1om.o

	$(AS) $(ASFLAGS) sources/x86/memcpy.x64-k1om.asm -o binaries/x64-linux-k1om-default/memcpy.x64-k1om.o

	$(AS) $(ASFLAGS) sources/x86/memset.x64-k1om.asm -o binaries/x64-linux-k1om-default/memset.x64-k1om.o

	$(AR) rcs binaries/x64-linux-k1om-default/libyeprt.a binaries/x64-linux-k1om-default/*.x64-k1om.o

x64-linux-sysv-default:
	-rm -rf binaries/x64-linux-sysv-default
	-mkdir -p binaries
	-mkdir -p binaries/x64-linux-sysv-default

	$(AS) $(ASFLAGS) sources/x86/memcmp.x64-sysv.asm -o binaries/x64-linux-sysv-default/memcmp.x64-sysv.o

	$(AS) $(ASFLAGS) sources/x86/memcpy.x64-sysv.asm -o binaries/x64-linux-sysv-default/memcpy.x64-sysv.o

	$(AS) $(ASFLAGS) sources/x86/memset.x64-sysv.asm -o binaries/x64-linux-sysv-default/memset.x64-sysv.o

	$(AR) rcs binaries/x64-linux-sysv-default/libyeprt.a binaries/x64-linux-sysv-default/*.x64-sysv.o

mips-linux-o32-android:
	-rm -rf binaries/mips-linux-o32-android
	-mkdir -p binaries
	-mkdir -p binaries/mips-linux-o32-android

	$(AS) $(ASFLAGS) sources/mips/ashldi3.asm -o binaries/mips-linux-o32-android/ashldi3.o

	$(CC) $(CCFLAGS) -c sources/mips/udivdi3.c -o binaries/mips-linux-o32-android/udivdi3.o

	$(CC) $(CCFLAGS) -c sources/mips/floatdidf.c -o binaries/mips-linux-o32-android/floatdidf.o

	$(AS) $(ASFLAGS) sources/mips/memcpy.asm -o binaries/mips-linux-o32-android/memcpy.o

	$(AS) $(ASFLAGS) sources/mips/memcmp.asm -o binaries/mips-linux-o32-android/memcmp.o

	$(AS) $(ASFLAGS) sources/mips/memset.asm -o binaries/mips-linux-o32-android/memset.o

	$(AR) rcs binaries/mips-linux-o32-android/libyeprt.a binaries/mips-linux-o32-android/*.o

x64-macosx-sysv-default:
	-rm -rf binaries/x64-macosx-sysv-default
	-mkdir -p binaries
	-mkdir -p binaries/x64-macosx-sysv-default

	$(AS) $(ASFLAGS) sources/x86/memcmp.x64-sysv.asm -o binaries/x64-macosx-sysv-default/memcmp.x64-sysv.o

	$(AS) $(ASFLAGS) sources/x86/memcpy.x64-sysv.asm -o binaries/x64-macosx-sysv-default/memcpy.x64-sysv.o

	$(AS) $(ASFLAGS) sources/x86/memset.x64-sysv.asm -o binaries/x64-macosx-sysv-default/memset.x64-sysv.o

	$(AR) rcs binaries/x64-macosx-sysv-default/libyeprt.a binaries/x64-macosx-sysv-default/*.x64-sysv.o

clean:
	-rm -rf binaries
