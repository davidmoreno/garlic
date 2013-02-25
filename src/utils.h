/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __GARLIC__UTILS_H__
#define __GARLIC__UTILS_H__

#include <string>

namespace Garlic{
	std::string realpath(const std::string &path);
	std::string file2string(const std::string &filename);
}

#endif
