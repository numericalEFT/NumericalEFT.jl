using System;

class CpuInfo {

	public static void Main(string[] args)
	{
		/* Retrieve information about processor architecture */
		Yeppp.CpuArchitecture architecture = Yeppp.Library.GetCpuArchitecture();
		Console.WriteLine("Architecture: {0}", architecture.Description);
		/* Retrieve information about processor vendor */
		Yeppp.CpuVendor vendor = Yeppp.Library.GetCpuVendor();
		Console.WriteLine("Vendor: {0}", vendor.Description);
		/* Retrieve information about processor microarchitecture */
		Yeppp.CpuMicroarchitecture microarchitecture = Yeppp.Library.GetCpuMicroarchitecture();
		Console.WriteLine("Microarchitecture: {0}", microarchitecture.Description);
		/* Check if Yeppp! is aware of any ISA features on this architecture */
		if (architecture.CpuIsaFeatures.GetEnumerator().MoveNext()) {
			Console.WriteLine("CPU ISA extensions:");
			/* Iterate through ISA features */
			foreach (Yeppp.CpuIsaFeature isaFeature in architecture.CpuIsaFeatures)
			{
				Console.WriteLine(String.Format("\t{0, -60}\t{1}", isaFeature.Description + ":", (Yeppp.Library.IsSupported(isaFeature) ? "Yes" : "No")));
			}
		}
		/* Check if Yeppp! is aware of any SIMD features on this architecture */
		if (architecture.CpuSimdFeatures.GetEnumerator().MoveNext()) {
			Console.WriteLine("CPU SIMD extensions:");
			/* Iterate through SIMD features */
			foreach (Yeppp.CpuSimdFeature simdFeature in architecture.CpuSimdFeatures)
			{
				Console.WriteLine(String.Format("\t{0, -60}\t{1}", simdFeature.Description + ":", (Yeppp.Library.IsSupported(simdFeature) ? "Yes" : "No")));
			}
		}
		/* Check if Yeppp! is aware of any non-ISA CPU and system features on this architecture */
		if (architecture.CpuSystemFeatures.GetEnumerator().MoveNext()) {
			Console.WriteLine("Non-ISA CPU and system features:");
			/* Iterate through non-ISA CPU and system features */
			foreach (Yeppp.CpuSystemFeature systemFeature in architecture.CpuSystemFeatures)
			{
				Console.WriteLine(String.Format("\t{0, -60}\t{1}", systemFeature.Description + ":", (Yeppp.Library.IsSupported(systemFeature) ? "Yes" : "No")));
			}
		}
	}

}
