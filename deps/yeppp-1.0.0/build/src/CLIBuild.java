/*
 *                      Yeppp! library build framework
 *
 * This file is part of Yeppp! library infrastructure and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

import info.yeppp.ebuilda.*;
import info.yeppp.ebuilda.filesystem.*;
import info.yeppp.ebuilda.AndroidNDK;
import info.yeppp.ebuilda.AndroidToolchain;
import info.yeppp.ebuilda.generic.Assembler;
import info.yeppp.ebuilda.generic.CCompiler;
import info.yeppp.ebuilda.generic.CppCompiler;
import info.yeppp.ebuilda.generic.Linker;

import java.io.IOException;
import java.util.*;
import java.util.regex.Pattern;

public class CLIBuild {

	public static void main(String[] args) throws Exception {
		final AbsoluteDirectoryPath yepppRoot = Machine.getLocal().getWorkingDirectory();

		for (final String abiName : args) {
			final ABI abi = ABI.parse(abiName);
			final Toolchain toolchain = getToolchain(abi);
			setup(toolchain, yepppRoot);
			build(toolchain, yepppRoot);
		}
	}

	static class Toolchain {
		public Toolchain(CppCompiler libraryCompiler, CCompiler jniCompiler, Assembler assembler, Linker libraryLinker, JavaSDK javaSDK) {
			this.libraryCompiler = libraryCompiler;
			this.unitTestCompiler = libraryCompiler.copy();
			this.jniCompiler = jniCompiler;
			this.assembler = assembler;
			this.libraryLinker = libraryLinker;
			this.unitTestLinker = libraryLinker.copy();
			this.javaSDK = javaSDK;
			this.microsoftResourceCompiler = null;
			this.gnuStrip = null;
			this.gnuObjCopy = null;
			this.appleStrip = null;
			this.appleDSymUtil = null;
		}

		public Toolchain(CppCompiler libraryCompiler, CCompiler jniCompiler, Assembler assembler, Linker libraryLinker, JavaSDK javaSDK, MicrosoftResourceCompiler microsoftResourceCompiler) {
			this.libraryCompiler = libraryCompiler;
			this.unitTestCompiler = libraryCompiler.copy();
			this.jniCompiler = jniCompiler;
			this.assembler = assembler;
			this.libraryLinker = libraryLinker;
			this.unitTestLinker = libraryLinker.copy();
			this.javaSDK = javaSDK;
			this.microsoftResourceCompiler = microsoftResourceCompiler;
			this.gnuStrip = null;
			this.gnuObjCopy = null;
			this.appleStrip = null;
			this.appleDSymUtil = null;
		}

		public Toolchain(CppCompiler libraryCompiler, CCompiler jniCompiler, Assembler assembler, Linker libraryLinker, JavaSDK javaSDK, GnuStrip gnuStrip, GnuObjCopy gnuObjCopy) {
			this.libraryCompiler = libraryCompiler;
			this.unitTestCompiler = libraryCompiler.copy();
			this.jniCompiler = jniCompiler;
			this.assembler = assembler;
			this.libraryLinker = libraryLinker;
			this.unitTestLinker = libraryLinker.copy();
			this.javaSDK = javaSDK;
			this.microsoftResourceCompiler = null;
			this.gnuStrip = gnuStrip;
			this.gnuObjCopy = gnuObjCopy;
			this.appleStrip = null;
			this.appleDSymUtil = null;
		}

		public Toolchain(CppCompiler libraryCompiler, CCompiler jniCompiler, Assembler assembler, Linker libraryLinker, JavaSDK javaSDK, AppleStrip appleStrip, AppleDSymUtil appleDSymUtil) {
			this.libraryCompiler = libraryCompiler;
			this.unitTestCompiler = libraryCompiler.copy();
			this.jniCompiler = jniCompiler;
			this.assembler = assembler;
			this.libraryLinker = libraryLinker;
			this.unitTestLinker = libraryLinker.copy();
			this.javaSDK = javaSDK;
			this.microsoftResourceCompiler = null;
			this.gnuStrip = null;
			this.gnuObjCopy = null;
			this.appleStrip = appleStrip;
			this.appleDSymUtil = appleDSymUtil;
		}

		final CppCompiler libraryCompiler;
		final CppCompiler unitTestCompiler;
		final CCompiler jniCompiler;
		final Assembler assembler;
		final Linker libraryLinker;
		final Linker unitTestLinker;
		final MicrosoftResourceCompiler microsoftResourceCompiler;
		final GnuStrip gnuStrip;
		final GnuObjCopy gnuObjCopy;
		final AppleStrip appleStrip;
		final AppleDSymUtil appleDSymUtil;
		final JavaSDK javaSDK;
	}

	public static void setup(Toolchain toolchain, AbsoluteDirectoryPath yepppRoot) {
		final ABI abi = toolchain.libraryCompiler.getABI();

		final AbsoluteDirectoryPath librarySourceDirectory = new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("library/sources"));
		final AbsoluteDirectoryPath libraryHeaderDirectory = new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("library/headers"));
		final AbsoluteDirectoryPath libraryObjectDirectory = new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("library/binaries/" + abi.toString()));
		final AbsoluteDirectoryPath unitTestSourceDirectory = new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("unit-tests/sources"));
		final AbsoluteDirectoryPath unitTestObjectDirectory = new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("unit-tests/binaries/" + abi.toString()));
		final AbsoluteDirectoryPath jniSourceDirectory = new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("bindings/java/sources-jni"));
		final AbsoluteDirectoryPath jniObjectDirectory = new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("bindings/java/binaries/" + abi.toString()));
		final AbsoluteDirectoryPath runtimeBinariesDirectory = new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("runtime/binaries/" + abi.toString()));

		toolchain.libraryCompiler.setSourceDirectory(librarySourceDirectory);
		toolchain.libraryCompiler.setObjectDirectory(libraryObjectDirectory);
		toolchain.libraryCompiler.addDefaultGlobalIncludeDirectories();
		toolchain.libraryCompiler.setVerboseBuild(true);
		toolchain.libraryCompiler.addMacro("YEP_BUILD_LIBRARY");
		toolchain.libraryCompiler.setPositionIndependentCodeGeneration(PositionIndependentCodeGeneration.UnlimitedLibraryPIC);
		toolchain.libraryCompiler.setRttiEnabled(false);
		toolchain.libraryCompiler.setExceptionsSupport(CppCompiler.Exceptions.NoExceptions);
		if (abi.getOperatingSystem().equals(OperatingSystem.MacOSX)) {
			toolchain.libraryCompiler.setRuntimeLibrary(CCompiler.RuntimeLibrary.DynamicRuntimeLibrary);
		} else {
			toolchain.libraryCompiler.setRuntimeLibrary(CCompiler.RuntimeLibrary.NoRuntimeLibrary);
		}
		toolchain.libraryCompiler.setOptimization(CCompiler.Optimization.MaxSpeedOptimization);
		toolchain.libraryCompiler.addIncludeDirectory(librarySourceDirectory);
		toolchain.libraryCompiler.addIncludeDirectory(libraryHeaderDirectory);

		toolchain.unitTestCompiler.setSourceDirectory(unitTestSourceDirectory);
		toolchain.unitTestCompiler.setObjectDirectory(unitTestObjectDirectory);
		toolchain.unitTestCompiler.addDefaultGlobalIncludeDirectories();
		toolchain.unitTestCompiler.setVerboseBuild(true);
		toolchain.unitTestCompiler.addMacro("YEP_BUILD_LIBRARY");
		toolchain.unitTestCompiler.setRttiEnabled(false);
		toolchain.unitTestCompiler.setExceptionsSupport(CppCompiler.Exceptions.NoExceptions);
		toolchain.unitTestCompiler.setRuntimeLibrary(CCompiler.RuntimeLibrary.DynamicRuntimeLibrary);
		toolchain.unitTestCompiler.setOptimization(CCompiler.Optimization.MaxSpeedOptimization);
		toolchain.unitTestCompiler.addIncludeDirectory(librarySourceDirectory);
		toolchain.unitTestCompiler.addIncludeDirectory(libraryHeaderDirectory);

		toolchain.jniCompiler.setSourceDirectory(jniSourceDirectory);
		toolchain.jniCompiler.setObjectDirectory(jniObjectDirectory);
		toolchain.jniCompiler.addDefaultGlobalIncludeDirectories();
		toolchain.jniCompiler.setVerboseBuild(true);
		toolchain.jniCompiler.addMacro("YEP_BUILD_LIBRARY");
		toolchain.jniCompiler.setPositionIndependentCodeGeneration(PositionIndependentCodeGeneration.UnlimitedLibraryPIC);
		if (abi.getOperatingSystem().equals(OperatingSystem.MacOSX)) {
			toolchain.jniCompiler.setRuntimeLibrary(CCompiler.RuntimeLibrary.DynamicRuntimeLibrary);
		} else {
			toolchain.jniCompiler.setRuntimeLibrary(CCompiler.RuntimeLibrary.NoRuntimeLibrary);
		}
		toolchain.jniCompiler.setOptimization(CCompiler.Optimization.MinSizeOptimization);
		toolchain.jniCompiler.addIncludeDirectory(jniSourceDirectory);
		toolchain.jniCompiler.addIncludeDirectory(libraryHeaderDirectory);
		toolchain.jniCompiler.addIncludeDirectory(librarySourceDirectory);
		if (toolchain.javaSDK != null) {
			toolchain.jniCompiler.addGlobalIncludeDirectories(toolchain.javaSDK.getIncludeDirectories());
		}

		if (toolchain.microsoftResourceCompiler != null) {
			toolchain.microsoftResourceCompiler.setSourceDirectory(librarySourceDirectory);
			toolchain.microsoftResourceCompiler.setObjectDirectory(libraryObjectDirectory);
			toolchain.microsoftResourceCompiler.addDefaultGlobalIncludeDirectories();
			toolchain.microsoftResourceCompiler.setVerboseBuild(true);
			toolchain.microsoftResourceCompiler.addIncludeDirectory(librarySourceDirectory);
			toolchain.microsoftResourceCompiler.addIncludeDirectory(new AbsoluteDirectoryPath(yepppRoot, new RelativeDirectoryPath("library/headers")));
		}

		if (toolchain.assembler != null) {
			toolchain.assembler.setSourceDirectory(librarySourceDirectory);
			toolchain.assembler.setObjectDirectory(libraryObjectDirectory);
			toolchain.assembler.setVerboseBuild(true);
			if (toolchain.assembler instanceof NASM) {
				final NASM nasm = (NASM)toolchain.assembler;
				nasm.setOptimization(NASM.Optimization.Multipass);
			}
		}

		if (toolchain.gnuObjCopy != null) {
			toolchain.gnuObjCopy.setVerboseBuild(true);
		}

		if (toolchain.gnuStrip != null) {
			toolchain.gnuStrip.setVerboseBuild(true);
		}

		if (toolchain.appleDSymUtil != null) {
			toolchain.appleDSymUtil.setVerboseBuild(true);
		}

		if (toolchain.appleStrip != null) {
			toolchain.appleStrip.setVerboseBuild(true);
		}

		toolchain.libraryLinker.setObjectDirectory(libraryObjectDirectory);
		toolchain.libraryLinker.setBinariesDirectory(libraryObjectDirectory);
		toolchain.libraryLinker.addDefaultGlobalLibraryDirectories();
		if (!abi.getOperatingSystem().equals(OperatingSystem.Windows)) {
			toolchain.libraryLinker.setPIC(PositionIndependentCodeGeneration.UnlimitedLibraryPIC);
		}
		toolchain.libraryLinker.setVerboseBuild(true);
		toolchain.libraryLinker.setRuntimeLibraryUse(false);
		if (!abi.getOperatingSystem().equals(OperatingSystem.MacOSX)) {
			toolchain.libraryLinker.addLibraryDirectory(runtimeBinariesDirectory);
		}
		if (abi.getOperatingSystem().equals(OperatingSystem.MacOSX)) {
			toolchain.libraryLinker.addDynamicLibraryDependence("c");
		} else {
			toolchain.libraryLinker.addStaticLibraryDependence("yeprt");
		}
		if (abi.getOperatingSystem().equals(OperatingSystem.Windows)) {
			toolchain.libraryLinker.addDynamicLibraryDependence("kernel32");
		}

		toolchain.unitTestLinker.setObjectDirectory(unitTestObjectDirectory);
		toolchain.unitTestLinker.setBinariesDirectory(unitTestObjectDirectory);
		toolchain.unitTestLinker.addDefaultGlobalLibraryDirectories();
		toolchain.unitTestLinker.setRuntimeLibraryUse(true);
		toolchain.unitTestLinker.setVerboseBuild(true);
	}

	public static Pattern getAssemblyPattern(ABI abi) {
		switch (abi) {
			case X86_Linux_Pic_Android:
			case X86_Linux_Pic_i586:
			case X86_MacOSX_Pic_Default:
				return Pattern.compile(".+\\.x86\\-pic\\.asm");
			case X86_Windows_Default_i586:
				return Pattern.compile(".+\\.x86\\-nonpic\\.asm");
			case X64_Windows_Microsoft_Default:
				return Pattern.compile(".+\\.x64\\-ms\\.asm");
			case X64_Linux_SystemV_Default:
			case X64_MacOSX_SystemV_Default:
				return Pattern.compile(".+\\.x64\\-sysv\\.asm");
			case X64_Linux_KNC_Default:
				return Pattern.compile(".+\\.x64\\-k1om\\.asm");
			case ARM_Linux_SoftEABI_V5T:
			case ARM_Linux_SoftEABI_Android:
			case ARM_Linux_SoftEABI_AndroidV7A:
				return Pattern.compile(".+\\.arm(?:\\-softeabi)?\\.asm");
			case ARM_Linux_HardEABI_V7A:
				return Pattern.compile(".+\\.arm(?:\\-hardeabi)?\\.asm");
			case MIPS_Linux_O32_Android:
				return Pattern.compile(".+\\.mips\\.asm");
			default:
				throw new Error(String.format("Unknown ABI %s", abi.toString()));
		}
	}

	public static void build(Toolchain toolchain, AbsoluteDirectoryPath yepppRoot) throws IOException {
		final ABI abi = toolchain.libraryCompiler.getABI();
		final Architecture architecture = abi.getArchitecture();
		final OperatingSystem operatingSystem = abi.getOperatingSystem();
		final AbsoluteFilePath libraryBinaryPath = new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), new RelativeFilePath("yeppp"));
		final BuildMessages buildMessages = new BuildMessages();
		final List<AbsoluteFilePath> libraryCppSources = toolchain.libraryCompiler.getSourceDirectory().getFiles(Pattern.compile(".+\\.cpp"), true);
		final List<AbsoluteFilePath> jniCSources = toolchain.jniCompiler.getSourceDirectory().getFiles(Pattern.compile(".+\\.c"), true);
		final List<AbsoluteFilePath> libraryRcSources = toolchain.libraryCompiler.getSourceDirectory().getFiles(Pattern.compile(".+\\.rc"), true);
		final List<AbsoluteFilePath> libraryAsmSources = toolchain.assembler.getSourceDirectory().getFiles(getAssemblyPattern(abi), true);
		final List<AbsoluteFilePath> libraryObjects = new ArrayList<AbsoluteFilePath>(libraryCppSources.size());
		final List<AbsoluteFilePath> unitTestCppSources = toolchain.unitTestCompiler.getSourceDirectory().getFiles(Pattern.compile(".+\\.cpp"), true);
		final List<AbsoluteFilePath> commonUnitTestObjects = new ArrayList<AbsoluteFilePath>(libraryCppSources.size());
		final Map<RelativeFilePath, List<AbsoluteFilePath>> specificUnitTestObjects = new HashMap<RelativeFilePath, List<AbsoluteFilePath>>();
		for (final AbsoluteFilePath source : libraryCppSources) {
			final RelativeFilePath sourcePath = source.getRelativePath(toolchain.libraryCompiler.getSourceDirectory());
			if (sourcePath.toString().equals("library/CpuX86.cpp") && !(architecture.equals(Architecture.X86) || architecture.equals(Architecture.X64))) {
				continue;
			}
			if (sourcePath.toString().equals("library/CpuArm.cpp") && !architecture.equals(Architecture.ARM)) {
				continue;
			}
			if (sourcePath.toString().equals("library/CpuMips.cpp") && !architecture.equals(Architecture.MIPS)) {
				continue;
			}
			if (sourcePath.toString().equals("library/CpuWindows.cpp") && !operatingSystem.equals(OperatingSystem.Windows)) {
				continue;
			}
			if (sourcePath.toString().equals("library/CpuLinux.cpp") && !operatingSystem.equals(OperatingSystem.Linux)) {
				continue;
			}
			if (sourcePath.toString().equals("library/CpuMacOSX.cpp") && !operatingSystem.equals(OperatingSystem.MacOSX)) {
				continue;

			}
			if (sourcePath.toString().equals("library/Unsafe.cpp") && !operatingSystem.equals(OperatingSystem.Linux)) {
				continue;
			}
			buildMessages.add(toolchain.libraryCompiler.compile(source));
			libraryObjects.add(toolchain.libraryCompiler.getObjectPath(source));
			if (sourcePath.toString().startsWith("library/") || sourcePath.toString().startsWith("random/")) {
				if (!sourcePath.toString().equals("library/Init.cpp")) {
					commonUnitTestObjects.add(toolchain.libraryCompiler.getObjectPath(source));
				}
			} else {
				final RelativeFilePath sourceKey = new RelativeFilePath(sourcePath.getDirectory(), sourcePath.getFileName().removeExtension(true));
				if (!specificUnitTestObjects.containsKey(sourceKey)) {
					specificUnitTestObjects.put(sourceKey, new ArrayList<AbsoluteFilePath>());
				}
				specificUnitTestObjects.get(sourceKey).add(toolchain.libraryCompiler.getObjectPath(source));
			}
		}
		if (abi.getOperatingSystem().equals(OperatingSystem.Windows)) {
			for (final AbsoluteFilePath source : libraryRcSources) {
				buildMessages.add(toolchain.microsoftResourceCompiler.compile(source));
				libraryObjects.add(toolchain.microsoftResourceCompiler.getObjectPath(source));
			}
		}
		for (final AbsoluteFilePath source : libraryAsmSources) {
			final RelativeFilePath sourcePath = source.getRelativePath(toolchain.libraryCompiler.getSourceDirectory());
			buildMessages.add(toolchain.assembler.assemble(source));
			libraryObjects.add(toolchain.assembler.getObjectPath(source));
			if (sourcePath.toString().startsWith("library/")) {
				commonUnitTestObjects.add(toolchain.libraryCompiler.getObjectPath(source));
			} else {
				final RelativeFilePath sourceKey = new RelativeFilePath(sourcePath.getDirectory(), sourcePath.getFileName().removeExtension(true));
				if (!specificUnitTestObjects.containsKey(sourceKey)) {
					specificUnitTestObjects.put(sourceKey, new ArrayList<AbsoluteFilePath>());
				}
				specificUnitTestObjects.get(sourceKey).add(toolchain.libraryCompiler.getObjectPath(source));
			}
		}
		if (!abi.equals(ABI.X64_Linux_KNC_Default)) {
			for (final AbsoluteFilePath source : jniCSources) {
				buildMessages.add(toolchain.jniCompiler.compile(source));
				libraryObjects.add(toolchain.jniCompiler.getObjectPath(source));
			}
		}
		buildMessages.add(toolchain.libraryLinker.linkDynamicLibrary(libraryBinaryPath, libraryObjects));
		if (abi.getOperatingSystem().equals(OperatingSystem.Linux)) {
			final RelativeFilePath libraryBinary = new RelativeFilePath("libyeppp.so");
			final RelativeFilePath debugBinary = new RelativeFilePath("libyeppp.dbg");
			try {
				getBinariesDirectory(yepppRoot, abi).create();
				buildMessages.add(toolchain.gnuStrip.extractDebugInformation(
						new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), libraryBinary),
						new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), debugBinary)));
				buildMessages.add(toolchain.gnuStrip.strip(new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), libraryBinary)));
				buildMessages.add(toolchain.gnuObjCopy.addGnuDebugLink(
						new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), libraryBinary),
						new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), debugBinary)));
				FileSystem.copyFile(new AbsoluteFilePath(getBinariesDirectory(yepppRoot, abi), libraryBinary), new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), libraryBinary));
				FileSystem.copyFile(new AbsoluteFilePath(getBinariesDirectory(yepppRoot, abi), debugBinary), new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), debugBinary));
			} catch (IOException e) {
			}
		} else if (abi.getOperatingSystem().equals(OperatingSystem.Windows)) {
			final RelativeFilePath libraryBinary = new RelativeFilePath("yeppp.dll");
			final RelativeFilePath importBinary = new RelativeFilePath("yeppp.lib");
			final RelativeFilePath debugBinary = new RelativeFilePath("yeppp.pdb");
			try {
				getBinariesDirectory(yepppRoot, abi).create();
				FileSystem.copyFile(new AbsoluteFilePath(getBinariesDirectory(yepppRoot, abi), libraryBinary), new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), libraryBinary));
				FileSystem.copyFile(new AbsoluteFilePath(getBinariesDirectory(yepppRoot, abi), importBinary), new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), importBinary));
				FileSystem.copyFile(new AbsoluteFilePath(getBinariesDirectory(yepppRoot, abi), debugBinary), new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), debugBinary));
			} catch (IOException e) {
			}
		} else if (abi.getOperatingSystem().equals(OperatingSystem.MacOSX)) {
			final RelativeFilePath libraryBinary = new RelativeFilePath("libyeppp.dylib");
			final RelativeFilePath debugBinary = new RelativeFilePath("libyeppp.dylib.dSYM");
			try {
				getBinariesDirectory(yepppRoot, abi).create();
				buildMessages.add(toolchain.appleDSymUtil.extractDebugInformation(
						new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), libraryBinary),
						new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), debugBinary)));
				buildMessages.add(toolchain.appleStrip.stripLocalSymbols(new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), libraryBinary)));
				FileSystem.copyFile(new AbsoluteFilePath(getBinariesDirectory(yepppRoot, abi), libraryBinary), new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), libraryBinary));
				FileSystem.copyFile(new AbsoluteFilePath(getBinariesDirectory(yepppRoot, abi), debugBinary), new AbsoluteFilePath(toolchain.libraryLinker.getBinariesDirectory(), debugBinary));
			} catch (IOException e) {
			}
		}
		for (BuildMessage buildMessage : buildMessages.iterable()) {
			System.out.println(buildMessage.toString());
		}
		for (final AbsoluteFilePath unitTestSource : unitTestCppSources) {
			final RelativeFilePath sourcePath = unitTestSource.getRelativePath(toolchain.unitTestCompiler.getSourceDirectory());
			final RelativeFilePath sourceKey = new RelativeFilePath(sourcePath.getDirectory(), sourcePath.getFileName().removeExtension(true));
			final List<AbsoluteFilePath> unitTestObjects = new ArrayList<AbsoluteFilePath>();
			buildMessages.add(toolchain.unitTestCompiler.compile(unitTestSource));
			unitTestObjects.add(toolchain.unitTestCompiler.getObjectPath(unitTestSource));
			unitTestObjects.addAll(commonUnitTestObjects);
			unitTestObjects.addAll(specificUnitTestObjects.get(sourceKey));
			final AbsoluteFilePath unitTest = new AbsoluteFilePath(toolchain.unitTestLinker.getBinariesDirectory(), sourceKey);
			buildMessages.add(toolchain.unitTestLinker.linkExecutable(unitTest, unitTestObjects));
		}
	}

	public static AbsoluteDirectoryPath getBinariesDirectory(AbsoluteDirectoryPath rootDirectory, ABI abi) {
		switch (abi) {
			case X86_Linux_Pic_i586:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/linux/i586"));
			case X64_Linux_SystemV_Default:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/linux/x86_64"));
			case X64_Linux_KNC_Default:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/linux/k1om"));
			case ARM_Linux_OABI_V4T:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/linux/arm"));
			case ARM_Linux_SoftEABI_V5T:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/linux/armel"));
			case ARM_Linux_HardEABI_V7A:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/linux/armhf"));
			case X86_Windows_Default_i586:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/windows/x86"));
			case X64_Windows_Microsoft_Default:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/windows/amd64"));
			case IA64_Windows_Microsoft_Default:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/windows/ia64"));
			case ARM_Linux_SoftEABI_Android:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/android/armeabi"));
			case ARM_Linux_SoftEABI_AndroidV7A:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/android/armeabi-v7a"));
			case X86_Linux_Pic_Android:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/android/x86"));
			case MIPS_Linux_O32_Android:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/android/mips"));
			case X86_MacOSX_Pic_Default:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/macosx/x86"));
			case X64_MacOSX_SystemV_Default:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/macosx/x86_64"));
			case X86_NaCl_NaCl_i686:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/nacl/x86"));
			case X64_NaCl_NaCl_Default:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/nacl/x86_64"));
			case ARM_NaCl_NaCl_Default:
				return new AbsoluteDirectoryPath(rootDirectory, new RelativeDirectoryPath("binaries/nacl/arm"));
			default:
				throw new Error(String.format("Unknown ABI %s", abi));
		}
	}

	public static Toolchain getToolchain(ABI abi) {
		switch (abi) {
			case X64_Windows_Microsoft_Default:
			case X86_Windows_Default_i586:
			{
				final VisualStudio visualStudio = VisualStudio.enumerate(Machine.getLocal(), abi).getNewest();
				final NASM nasm = NASM.enumerate(Machine.getLocal(), abi).getNewest();
				final MicrosoftResourceCompiler resourceCompiler = visualStudio.getWindowsSDK().getResourceCompiler();
				final JavaSDK javaSDK = JavaSDK.enumerate(Machine.getLocal()).getNewest();
				return new Toolchain(visualStudio.getCppCompiler(), visualStudio.getCCompiler(), nasm, visualStudio.getLinker(), javaSDK, resourceCompiler);
			}
			case ARM_Linux_SoftEABI_Android:
			case ARM_Linux_SoftEABI_AndroidV7A:
			case X86_Linux_Pic_Android:
			case MIPS_Linux_O32_Android:
			{
				final AndroidNDK androidNDK = AndroidNDK.enumerate(Machine.getLocal()).getNewest();
				final AndroidToolchain androidToolchain = androidNDK.enumerateToolchains(abi, AndroidToolchain.Type.GNU).getNewest();
				return new Toolchain(androidToolchain.getCppCompiler(), androidToolchain.getCCompiler(), androidToolchain.getAssembler(), androidToolchain.getLinker(), null, androidToolchain.getStrip(), androidToolchain.getObjCopy());
			}
			case X64_Linux_SystemV_Default:
			case X86_Linux_Pic_i586:
			{
				final GccToolchain gccToolchain = GccToolchain.enumerate(Machine.getLocal(), abi).getNewest();
				final GnuBinutils gnuBinutils = GnuBinutils.enumerate(Machine.getLocal(), abi).getNewest();
				final NASM nasm = NASM.enumerate(Machine.getLocal(), abi).getNewest();
				final JavaSDK javaSDK = JavaSDK.enumerate(Machine.getLocal()).getNewest();
				return new Toolchain(gccToolchain.getCppCompiler(), gccToolchain.getCCompiler(), nasm,
						gccToolchain.getCppCompiler().asLinker(new LinkedList<AbsoluteDirectoryPath>()), javaSDK,
						gnuBinutils.getStrip(), gnuBinutils.getObjCopy());
			}
			case ARM_Linux_HardEABI_V7A:
			case ARM_Linux_SoftEABI_V5T:
			{
				final GccToolchain gccToolchain = GccToolchain.enumerate(Machine.getLocal(), abi).getNewest();
				final GnuBinutils gnuBinutils = GnuBinutils.enumerate(Machine.getLocal(), abi).getNewest();
				final JavaSDK javaSDK = JavaSDK.enumerate(Machine.getLocal()).getNewest();
				return new Toolchain(gccToolchain.getCppCompiler(), gccToolchain.getCCompiler(), gnuBinutils.getAssembler(),
						gccToolchain.getCppCompiler().asLinker(new LinkedList<AbsoluteDirectoryPath>()), javaSDK,
						gnuBinutils.getStrip(), gnuBinutils.getObjCopy());
			}
			case X64_Linux_KNC_Default:
			{
				final GnuBinutils gnuBinutils = GnuBinutils.enumerate(Machine.getLocal(), abi).getNewest();
				final IntelCppToolchain intelCppToolchain = IntelCppToolchain.enumerate(Machine.getLocal(), abi).getNewest();
				return new Toolchain(intelCppToolchain.getCppCompiler(), intelCppToolchain.getCCompiler(), gnuBinutils.getAssembler(),
						intelCppToolchain.getCppCompiler().asLinker(new LinkedList<AbsoluteDirectoryPath>()), null,
						gnuBinutils.getStrip(), gnuBinutils.getObjCopy());
			}
			case X64_MacOSX_SystemV_Default:
			case X86_MacOSX_Pic_Default:
			{
				final ClangToolchain clangToolchain = ClangToolchain.enumerate(Machine.getLocal(), abi).getNewest();
				final AppleStrip appleStrip = AppleStrip.enumerate(Machine.getLocal(), abi).getNewest();
				final AppleDSymUtil appleDSymUtil = AppleDSymUtil.enumerate(Machine.getLocal(), abi).getNewest();
				final NASM nasm = NASM.enumerate(Machine.getLocal(), abi).getNewest();
				final JavaSDK javaSDK = JavaSDK.enumerate(Machine.getLocal()).getNewest();
				return new Toolchain(clangToolchain.getCppCompiler(), clangToolchain.getCCompiler(), nasm,
						clangToolchain.getCppCompiler().asLinker(new LinkedList<AbsoluteDirectoryPath>()), javaSDK,
						appleStrip, appleDSymUtil);
			}
			default:
				return null;
		}
	}

}
