/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __GARLIC__TEST_H__
#define __GARLIC__TEST_H__

#include <map>
#include <string>

#include "inireader.h"

namespace Garlic{
	class Test{
	private:
		IniReader ini;
		std::string defaultdir;
	public:
		Test(IniReader &reader);
		
		bool check_and_run();
		bool check();
		int run(int test_id, const std::map<std::string, std::string> &extra_env, const std::string &outfilename);
	private:

		std::map<std::string, std::string> default_rw_env(); /// Loads default env-rw
		void setup_env(const std::map<std::string, std::string> &extra_env);
	};
}

#endif
