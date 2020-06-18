/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/** @brief	Contains information about @Yeppp library version. */
/** @see	Library#getVersion() */
public class Version {
	protected Version(int major, int minor, int patch, int build, String releaseName) {
		this.major = major;
		this.minor = minor;
		this.patch = patch;
		this.build = build;
		this.releaseName = releaseName;
	}

	final private int major;
	final private int minor;
	final private int patch;
	final private int build;
	final private String releaseName;

	/**
	 * @brief	Provides the major version number of @Yeppp library.
	 * @details	Library releases with the same major versions are guaranteed to be API- and ABI-compatible.
	 * @return	The major version of @Yeppp library.
	 */
	public int getMajor() {
		return this.major;
	}

	/**
	 * @brief	Provides the minor version number of @Yeppp library.
	 * @details	A change in minor versions indicates addition of new features, and major bug-fixes.
	 * @return	The minor version of @Yeppp library.
	 */
	public int getMinor() {
		return this.minor;
	}

	/**
	 * @brief	Provides the patch level of @Yeppp library.
	 * @details	A version with a higher patch level indicates minor bug-fixes.
	 * @return	The patch level of @Yeppp library.
	 */
	public int getPatch() {
		return this.patch;
	}

	/**
	 * @brief	Provides the build number of @Yeppp library.
	 * @details	The build number is unique for the fixed combination of major, minor, and patch-level versions.
	 * @return	The build number of @Yeppp library.
	 */
	public int getBuild() {
		return this.build;
	}

	/**
	 * @brief	Provides the human-readable name of this release of @Yeppp library.
	 * @details	The release name may contain non-ASCII characters.
	 * @return	The release name of the @Yeppp library.
	 */
	public String getReleaseName() {
		return this.releaseName;
	}

	/**
	 * @brief	Provides a string representation for all parts of the version.
	 * @return	The full version string in the format "major.minor.patch.build (release name)".
	 */
	@Override
	public String toString() {
		return String.format("%d.%d.%d.%d (%s)", this.major, this.minor, this.patch, this.build, this.releaseName);
	}

}
