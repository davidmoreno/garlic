/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __GARLIC__TEST_H__
#define __GARLIC__TEST_H__

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
		int run(const std::string &name);
	private:
		void setup_env();
	};
}

#endif
