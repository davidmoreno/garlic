/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __PPEPPER_SERVER_H__
#define __PPEPPER_SERVER_H__

#include <onion/onion.hpp>

#include "inireader.h"

namespace PPepper{
	class Server{
	private:
		std::string configdir;
		IniReader ini;
	public:
		Server(const std::string &configdir);
		onion_connection_status login(Onion::Request &req, Onion::Response &res);
		onion_connection_status style_css(Onion::Request &req, Onion::Response &res);
		onion_connection_status logout(Onion::Request &req, Onion::Response &res);
		onion_connection_status index(Onion::Request &req, Onion::Response &res);
		onion_connection_status results_json(Onion::Request &req, Onion::Response &res);
		onion_connection_status result(Onion::Request &req, Onion::Response &res);
			
	private:
		void setup_env();
		std::string run_test();
	};
}

#endif
