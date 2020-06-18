#!/bin/sh
#                      Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.

show_usage()
{
	echo "Usage: . set-vars.sh [param]"
	echo "   or source set-vars.sh [param]"
	echo "Possible options for [param]"
	echo "   x86    - set variables for x86 target"
	echo "   x86_64 - set variables for x86-64 target"
	echo "   k1om   - set variables for Xeon Phi target"
	echo "   armel  - set variables for ARM Soft-Float EABI target"
	echo "   armhf  - set variables for ARM Hard-Float EABI target"
	echo "By default the variables are set according to OS architecture"
}

error_usage()
{
	echo "Error: invalid command-line argument(s)" >&2
	show_usage
}

error_arch()
{
	echo "Error: invalid architecture/ABI name $1" >&2
	show_usage
	return 1
}

error_os()
{
	echo "Error: could not detect host OS: unknown kernel name $1" >&2
	echo "   Please refer to Yeppp! developers for a fix to this problem" >&2
	return 1
}

error_os_arch()
{
	echo "Error: could not detect host architecture: unknown architecture name $2 for OS $1" >&2
	echo "   Please refer to Yeppp! developers for a fix to this problem" >&2
	return 1
}

error_os_arch_abi()
{
	echo "Error: could not detect host ABI: unknown ABI name $3 for OS $1 on architecture $2" >&2
	echo "   Please refer to Yeppp! developers for a fix to this problem" >&2
	return 1
}

error_shell()
{
	echo "Error: unknown Unix shell." >&2
	echo "   Please use bash, dash, zsh, or ksh" >&2
	return 1
}

guess_platform()
{
	OS_KERNEL=$(uname -s)
	ARCHITECTURE=$(uname -m)
	if [ "${OS_KERNEL}" = "Linux" ]
	then
		case "${ARCHITECTURE}" in
			"i386"|"i486"|"i586"|"i686")
				echo "linux-x86"
				return 0
			;;
			"x86_64")
				echo "linux-x86_64"
				return 0
			;;
			"k1om")
				echo "linux-k1om"
				return 0
			;;
			"armv5tel"|"armv6l")
				dpkg --version >/dev/null 2>&1
				if [ "$?" -eq 0 ]
				then
					ABI=$(dpkg --print-architecture)
					if [ "${ABI}" = "armhf" ]
					then
						echo "linux-armhf"
						return 0
					elif [ "${ABI}" = "armel" ]
					then
						echo "linux-armel"
						return 0
					else
						if [ "$1" != "silent" ]
						then
							error_os_arch_abi "${OS_KERNEL}" "${ARCHITECTURE}" "${ABI}"
						fi
						echo "unknown"
						return 1
					fi
				else
					if [ "$1" != "silent" ]
					then
						echo "Warning: could not reliably detect ABI. Assume soft-float ARM EABI" >&2
					fi
					echo "linux-armel"
					return -1
				fi
			;;
			"armv7l")
				dpkg --version >/dev/null 2>&1
				if [ "$?" -eq 0 ]
				then
					ABI=$(dpkg --print-architecture)
					if [ "${ABI}" = "armhf" ]
					then
						echo "linux-armhf"
						return 0
					elif [ "${ABI}" = "armel" ]
					then
						echo "linux-armel"
						return 0
					else
						if [ "$1" != "silent" ]
						then
							error_os_arch_abi "${OS_KERNEL}" "${ARCHITECTURE}" "${ABI}"
						fi
						echo "unknown"
						return 1
					fi
				else
					if [ "$1" != "silent" ]
					then
						echo "Warning: could not reliably detect ABI. Assume hard-float ARM EABI" >&2
					fi
					echo "linux-armhf"
					return -1
				fi
			;;
			*)
				if [ "$1" != "silent" ]
				then
					error_os_arch "${OS_KERNEL}" "${ARCHITECTURE}"
				fi
				echo "unknown"
				return 1
			;;
		esac
	elif [ "${OS_KERNEL}" = "Darwin" ]
	then
		case "${ARCHITECTURE}" in
			"x86")
				echo "darwin-x86"
				return 0
			;;
			"x86_64")
				echo "darwin-x86_64"
				return 0
			;;
			*)
				if [ "$1" != "silent" ]
				then
					error_os_arch "${OS_KERNEL}" "${ARCHITECTURE}"
				fi
				echo "unknown"
				return 1
			;;
		esac
	else
		if [ "$1" != "silent" ]
		then
			error_os "${OS_KERNEL}"
		fi
		echo "unknown"
		return 1
	fi
}

setup_ld_library_path()
{
	if [ -z "${LD_LIBRARY_PATH}" ]
	then
		export LD_LIBRARY_PATH="${YEPBINARIES}"
	else
		export LD_LIBRARY_PATH="${YEPBINARIES}:${LD_LIBRARY_PATH}"
	fi
}

setup_mic_ld_library_path()
{
	if [ -z "${MIC_LD_LIBRARY_PATH}" ]
	then
		export MIC_LD_LIBRARY_PATH="${YEPBINARIES}"
	else
		export MIC_LD_LIBRARY_PATH="${YEPBINARIES}:${MIC_LD_LIBRARY_PATH}"
	fi
}

setup_dyld_library_path()
{
	if [ -z "${DYLD_LIBRARY_PATH}" ]
	then
		export DYLD_LIBRARY_PATH="${YEPBINARIES}"
	else
		export DYLD_LIBRARY_PATH="${YEPBINARIES}:${DYLD_LIBRARY_PATH}"
	fi
}

setup_compiler_variables()
{
	if [ -z "${INCLUDE}" ]
	then
		export INCLUDE="${YEPROOT}/library/headers"
	else
		export INCLUDE="${YEPROOT}/library/headers:${INCLUDE}"
	fi

	if [ -z "${CPATH}" ]
	then
		export CPATH="${YEPROOT}/library/headers"
	else
		export CPATH="${YEPROOT}/library/headers:${CPATH}"
	fi
}

setup_linker_variables()
{
	if [ -z "${LIBRARY_PATH}" ]
	then
		export LIBRARY_PATH="${YEPBINARIES}"
	else
		export LIBRARY_PATH="${YEPBINARIES}:${LIBRARY_PATH}"
	fi
}

setup_androidndk_variables()
{
	if [ -z "${NDK_MODULE_PATH}" ]
	then
		export NDK_MODULE_PATH="${YEPROOT}/binaries/android"
	else
		export NDK_MODULE_PATH="${YEPROOT}/binaries/android:${NDK_MODULE_PATH}"
	fi
}

setup_java_variables()
{
	if [ -z "${CLASSPATH}" ]
	then
		export CLASSPATH="${YEPROOT}/binaries/java-1.5/yeppp.jar"
	else
		export CLASSPATH="${YEPROOT}/binaries/java-1.5/yeppp.jar:${CLASSPATH}"
	fi
}

setup_native_tools()
{
	setup_compiler_variables
	setup_linker_variables
	setup_androidndk_variables
	setup_java_variables
}

setup_cross_tools()
{
	setup_compiler_variables
	setup_androidndk_variables
	setup_java_variables
}

setup_x86()
{
	YEP_HOST_PLATFORM="$1"
	YEP_ROOT="$2"
	case "${YEP_HOST_PLATFORM}" in
		"linux-x86"|"linux-x86_64")
			# Native compilation for x86 on x86/x86-64 (Linux)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/i586"
			export YEPPLATFORM="x86-linux-pic-i586"

			setup_native_tools
			setup_ld_library_path
			return 0
		;;
		"linux-k1om")
			# Cross-compilation for x86 on K1OM (Xeon Phi) is not supported
			echo "Error: cross-compilation for x86 on Xeon Phi is not supported" >&2
			return 1
		;;
		"linux-armel"|"linux-armhf")
			# Cross-compilation for x86 on ARM (armel/armhf)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/i586"
			export YEPPLATFORM="x86-linux-pic-i586"

			setup_cross_tools
			return 0
		;;
		"darwin-x86"|"darwin-x86_64")
			# Native compilation for x86 on x86/x86-64 (Mac OS X)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/macosx/i586"
			export YEPPLATFORM="x86-macosx-pic-default"

			setup_native_tools
			setup_dyld_library_path
			return 0
		;;
		*)
			guess_platform >/dev/null
			return 1
		;;
	esac
}

setup_x64()
{
	YEP_HOST_PLATFORM="$1"
	YEP_ROOT="$2"
	case "${YEP_HOST_PLATFORM}" in
		"linux-x86_64")
			# Native compilation for x86-64 on x86-64 (Linux)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/x86_64"
			export YEPPLATFORM="x64-linux-sysv-default"

			setup_native_tools
			setup_ld_library_path
			return 0
		;;
		"linux-k1om")
			# Cross-compilation for x86-64 on K1OM (Xeon Phi)
			echo "Error: cross-compilation for x86-64 on Xeon Phi is not supported" >&2
			return 1
		;;
		"linux-x86"|"linux-armel"|"linux-armhf")
			# Cross-compilation for x86-64 on x86 or ARM (armel/armhf)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/x86_64"
			export YEPPLATFORM="x64-linux-sysv-default"

			setup_cross_tools
			return 0
		;;
		"darwin-x86_64")
			# Native compilation for x86-64 on x86-64 (Mac OS X)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/macosx/x86_64"
			export YEPPLATFORM="x64-macosx-sysv-default"

			setup_native_tools
			setup_dyld_library_path
			return 0
		;;
		"darwin-x86")
			# Cross-compilation for x86-64 on x86 (Mac OS X)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/macosx/x86_64"
			export YEPPLATFORM="x64-macosx-sysv-default"

			setup_cross_tools
			return 0
		;;
		*)
			guess_platform >/dev/null
			return 1
		;;
	esac
}

setup_k1om()
{
	YEP_HOST_PLATFORM="$1"
	YEP_ROOT="$2"
	case "${YEP_HOST_PLATFORM}" in
		"linux-x86"|"linux-x86_64")
			# Cross-compilation for K1OM on x86/x86-64 (Linux)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/k1om"
			export YEPPLATFORM="x64-linux-k1om-default"

			setup_cross_tools
			setup_mic_ld_library_path
			return 0
		;;
		"linux-k1om")
			# Native-compilation on K1OM (Xeon Phi)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/k1om"
			export YEPPLATFORM="x64-linux-k1om-default"

			setup_ld_library_path
			return 0
		;;
		"linux-armel"|"linux-armhf")
			# Cross-compilation for x86-64 on ARM (armel/armhf)
			echo "Error: cross-compilation for Xeon Phi on ARM is not supported" >&2
			return 1
		;;
		"darwin-x86"|"darwin-x86_64")
			# Cross-compilation for K1OM on x86/x86-64 (Mac OS X)
			echo "Error: cross-compilation for Xeon Phi on Mac OS X is not supported" >&2
			return 1
		;;
		*)
			guess_platform >/dev/null
			return 1
		;;
	esac
}

setup_armel()
{
	YEP_HOST_PLATFORM="$1"
	YEP_ROOT="$2"
	case "${YEP_HOST_PLATFORM}" in
		"linux-x86"|"linux-x86_64")
			# Cross-compilation for ARM (armel) on x86/x86-64 (Linux)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/armel"
			export YEPPLATFORM="arm-linux-softeabi-v5t"

			setup_cross_tools
			return 0
		;;
		"linux-k1om")
			# Cross-compilation for ARM on K1OM (Xeon Phi)
			echo "Error: cross-compilation for ARM on Xeon Phi is not supported" >&2
			return 1
		;;
		"linux-armel"|"linux-armhf")
			# Native compilation for ARM (armel) on ARM (armel/armhf)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/armel"
			export YEPPLATFORM="arm-linux-softeabi-v5t"

			setup_native_tools
			setup_ld_library_path
			return 0
		;;
		"darwin-x86"|"darwin-x86_64")
			# Cross-compilation for ARM on x86/x86-64 (Mac OS X)
			echo "Error: cross-compilation for ARM on Mac OS X is not supported" >&2
			return 1
		;;
		*)
			guess_platform >/dev/null
			return 1
		;;
	esac
}

setup_armhf()
{
	YEP_HOST_PLATFORM="$1"
	YEP_ROOT="$2"
	case "${YEP_HOST_PLATFORM}" in
		"linux-x86"|"linux-x86_64")
			# Cross-compilation for ARM (armel) on x86/x86-64 (Linux)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/armhf"
			export YEPPLATFORM="arm-linux-hardeabi-v7a"

			setup_cross_tools
			return 0
		;;
		"linux-k1om")
			# Cross-compilation for ARM on K1OM (Xeon Phi)
			echo "Error: cross-compilation for ARM on Xeon Phi is not supported" >&2
			return 1
		;;
		"linux-armel"|"linux-armhf")
			# Native compilation for ARM (armhf) on ARM (armel/armhf)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/armhf"
			export YEPPLATFORM="arm-linux-hardeabi-v7a"

			setup_native_tools
			setup_ld_library_path
			return 0
		;;
		"darwin-x86"|"darwin-x86_64")
			# Cross-compilation for ARM on x86/x86-64 (Mac OS X)
			echo "Error: cross-compilation for ARM on Mac OS X is not supported" >&2
			return 1
		;;
		*)
			guess_platform >/dev/null
			return 1
		;;
	esac
}

setup_guess()
{
	YEP_HOST_PLATFORM="$1"
	YEP_ROOT="$2"
	case "${YEP_HOST_PLATFORM}" in
		"linux-x86")
			# Native compilation for x86 on x86
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/i586"
			export YEPPLATFORM="x86-linux-pic-i586"

			setup_native_tools
			setup_ld_library_path
			return 0
		;;
		"linux-x86_64")
			# Native compilation for x86-64 on x86-64
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/x86_64"
			export YEPPLATFORM="x64-linux-sysv-default"

			setup_native_tools
			setup_ld_library_path
			return 0
		;;
		"linux-k1om")
			# Native compilation for K1OM on K1OM (Xeon Phi)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/k1om"
			export YEPPLATFORM="x64-linux-k1om-default"

			setup_ld_library_path
			return 0
		;;
		"linux-armel")
			# Native compilation for ARM (armel) on ARM (armel)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/armel"
			export YEPPLATFORM="arm-linux-softeabi-v5t"

			setup_native_tools
			setup_ld_library_path

			# Report warnings in ABI detection (if any)
			guess_platform >/dev/null
			return 0
		;;
		"linux-armhf")
			# Native compilation for ARM (armhf) on ARM (armhf)
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/linux/armhf"
			export YEPPLATFORM="arm-linux-hardeabi-v7a"

			setup_native_tools
			setup_ld_library_path

			# Report warnings in ABI detection (if any)
			guess_platform >/dev/null
			return 0
		;;
		"darwin-x86")
			# Native compilation for x86 on x86
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/macosx/x86"
			export YEPPLATFORM="x86-macosx-pic-default"

			setup_native_tools
			setup_dyld_library_path
			return 0
		;;
		"darwin-x86_64")
			# Native compilation for x86-64 on x86-64
			export YEPROOT="${YEP_ROOT}"
			export YEPBINARIES="${YEPROOT}/binaries/macosx/x86_64"
			export YEPPLATFORM="x64-macosx-sysv-default"

			setup_native_tools
			setup_dyld_library_path
			return 0
		;;
		*)
			guess_platform >/dev/null
			return 1
		;;
	esac
}

if [ -n "${BASH_SOURCE}" ]
then
	YEP_ROOT=$( cd $(dirname "${BASH_SOURCE}") ; pwd )
elif [ -n "${ZSH_VERSION}" ]
then
	YEP_ROOT=$( cd $(dirname "$0") ; pwd )
elif [ -n "${KSH_VERSION}" ]
then
	YEP_ROOT=$( cd $(dirname ${.sh.file}) ; pwd )
elif [ "$0" = "dash" ]
then
	echo "Warning: dash is only partially supported: architecture is always auto-detected" >&2
	YEP_SCRIPT_FD=`ls /proc/$$/fd/ | sort -nr | head -n 1`
	YEP_ROOT=$(dirname $(readlink "/proc/$$/fd/${YEP_SCRIPT_FD}"))
	unset YEP_SCRIPT_FD
else
	error_shell
	return 1
fi

YEP_HOST_PLATFORM=$(guess_platform "silent")

if [ "$#" -eq 0 ]
then
	setup_guess "${YEP_HOST_PLATFORM}" "${YEP_ROOT}"
	return "$?"
elif [ "$#" -ne 1 ]
then
	error_usage
	unset YEP_ROOT
	unset YEP_HOST_PLATFORM
	return 1
fi

case "$1" in
	"x86")
		setup_x86 "${YEP_HOST_PLATFORM}" "${YEP_ROOT}"
		unset YEP_ROOT
		unset YEP_HOST_PLATFORM
	;;
	"x86_64")
		setup_x64 "${YEP_HOST_PLATFORM}" "${YEP_ROOT}"
		unset YEP_ROOT
		unset YEP_HOST_PLATFORM
	;;
	"k1om")
		setup_k1om "${YEP_HOST_PLATFORM}" "${YEP_ROOT}"
		unset YEP_ROOT
		unset YEP_HOST_PLATFORM
	;;
	"armel")
		setup_armel "${YEP_HOST_PLATFORM}" "${YEP_ROOT}"
		unset YEP_ROOT
		unset YEP_HOST_PLATFORM
	;;
	"armhf")
		setup_armhf "${YEP_HOST_PLATFORM}" "${YEP_ROOT}"
		unset YEP_ROOT
		unset YEP_HOST_PLATFORM
	;;
	*)
		error_arch "$1"
		unset YEP_ROOT
		unset YEP_HOST_PLATFORM
	;;
esac
return "$?"
