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
	* @short Reads s ini file and creates a in mem map of groups and keys to values.
	*/
	class IniReader{
	private:
		std::shared_ptr<GarlicPrivate> d;
	public:
		class value_not_found : public std::exception{
			std::string msg;
		public:
			value_not_found(const std::string &str) : msg(std::string("Key not found ")+str) {}
			const char *what() const throw() { return msg.c_str(); }
		};
		
		IniReader(const std::string &inifile);
		~IniReader();
		
		bool has(const std::string &field) const;
		std::string get(const std::string &field) const;
		std::string get(const std::string &field, const std::string &defaultvalue) const;
		std::vector<std::string> get_keys(const std::string &group) const;
		
		std::string getPath() const;
	};
}

#endif


