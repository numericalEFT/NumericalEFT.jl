/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	Operating System exception
 * @details	This exception is thrown when a @Yeppp function call fails inside the OS kernel.
 */
public class SystemException extends RuntimeException {
	
	/**
	 * @brief	Constructs an Operating System exception with the supplied description.
	 */
	public SystemException(String description) {
		super(description);
	}

}
