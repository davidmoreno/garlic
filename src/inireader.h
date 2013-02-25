/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __GARLIC__INI_H__
#define __GARLIC__INI_H__

#include <string>
#include <vector>
#include <memory>

namespace Garlic{
	class GarlicPrivate;
	
	/**
	* @short Wrapper around boost property_tree, to make it faster compile and easy use.
	*/
	class IniReader{
	private:
		std::shared_ptr<GarlicPrivate> d;
	public:
		IniReader(const std::string &inifile);
		~IniReader();
		
		std::string get(const std::string &field) const;
		std::string get(const std::string &field, const std::string &defaultvalue) const;
		std::vector<std::string> get_keys(const std::string &group) const;
		
		std::string getPath() const;
	};
}

#endif


