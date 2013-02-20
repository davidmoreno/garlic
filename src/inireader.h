/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __PPEPPER__INI_H__
#define __PPEPPER__INI_H__

#include <string>
#include <vector>

namespace PPepper{
	class PPepperPrivate;
	
	/**
	* @short Wrapper around boost property_tree, to make it faster compile and easy use.
	*/
	class IniReader{
	private:
			PPepperPrivate *d;
	public:
		IniReader();
		~IniReader();

		void open(const std::string &inifile);
		
		std::string get(const std::string &field);
		std::string get(const std::string &field, const std::string &defaultvalue);
		std::vector<std::string> get_keys(const std::string &group);
	};
}

#endif


