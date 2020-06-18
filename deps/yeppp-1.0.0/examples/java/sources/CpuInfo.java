import java.util.Iterator;

class CpuInfo {
	
	public static void main(String[] args) {
		final info.yeppp.CpuArchitecture architecture = info.yeppp.Library.getCpuArchitecture();
		final info.yeppp.CpuVendor vendor = info.yeppp.Library.getCpuVendor();
		final info.yeppp.CpuMicroarchitecture microarchitecture = info.yeppp.Library.getCpuMicroarchitecture();
		System.out.println("Architecture: " + architecture.getDescription());
		System.out.println("Vendor: " + vendor.getDescription());
		System.out.println("Microarchitecture: " + microarchitecture.getDescription());
		final Iterator<info.yeppp.CpuIsaFeature> isaFeaturesIterator = architecture.iterateIsaFeatures();
		if (isaFeaturesIterator.hasNext()) {
			System.out.println("CPU ISA extensions:");
			while (isaFeaturesIterator.hasNext()) {
				final info.yeppp.CpuIsaFeature isaFeature = isaFeaturesIterator.next();
				System.out.println(String.format("\t%-60s\t%s", isaFeature.getDescription() + ":", (info.yeppp.Library.isSupported(isaFeature) ? "Yes" : "No")));
			}
		}
		final Iterator<info.yeppp.CpuSimdFeature> simdFeaturesIterator = architecture.iterateSimdFeatures();
		if (simdFeaturesIterator.hasNext()) {
			System.out.println("CPU SIMD extensions:");
			while (simdFeaturesIterator.hasNext()) {
				final info.yeppp.CpuSimdFeature simdFeature = simdFeaturesIterator.next();
				System.out.println(String.format("\t%-60s\t%s", simdFeature.getDescription() + ":", (info.yeppp.Library.isSupported(simdFeature) ? "Yes" : "No")));
			}
		}
		final Iterator<info.yeppp.CpuSystemFeature> systemFeaturesIterator = architecture.iterateSystemFeatures();
		if (systemFeaturesIterator.hasNext()) {
			System.out.println("Non-ISA CPU and system features:");
			while (systemFeaturesIterator.hasNext()) {
				final info.yeppp.CpuSystemFeature systemFeature = systemFeaturesIterator.next();
				System.out.println(String.format("\t%-60s\t%s", systemFeature.getDescription() + ":", (info.yeppp.Library.isSupported(systemFeature) ? "Yes" : "No")));
			}
		}
	}
	
}
