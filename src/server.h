/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __GARLIC_SERVER_H__
#define __GARLIC_SERVER_H__

#include <onion/onion.hpp>
#include <onion/dict.hpp>

#include "inireader.h"
#include "test.h"

namespace Garlic{
	class Server{
	private:
		std::string logdir;
		IniReader ini;
		Test test;
	public:
		Server(IniReader &ini);
		onion_connection_status login(Onion::Request &req, Onion::Response &res);
		onion_connection_status style_css(Onion::Request &req, Onion::Response &res);
		onion_connection_status logout(Onion::Request &req, Onion::Response &res);
		onion_connection_status index(Onion::Request &req, Onion::Response &res);
		onion_connection_status results_json(Onion::Request &req, Onion::Response &res);
		onion_connection_status result(Onion::Request &req, Onion::Response &res);
			
		
		Onion::Dict defaultContext();
	private:
		std::string run_test(int test_id, const std::map<std::string, std::string> &extra_env);
	};
}

#endif
