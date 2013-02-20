/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <onion/onion.hpp>
#include <onion/url.hpp>
#include <onion/log.h>

#include "server.h"

using namespace Onion;
using namespace PPepper;


int main(int argc, char **argv){
	if (argc==1){
		ONION_ERROR("Usage: %s <configdir>",argv[0]);
		return 1;
	}
	
	Onion::Onion o;
	Server ppepper(argv[1]);
	
	Url root(o);
	
	root.add("",&ppepper, &Server::login);
	root.add("style.css",&ppepper, &Server::style_css);
	root.add("index",&ppepper, &Server::index);
	root.add("results",&ppepper, &Server::results_json);
	root.add("^result/(.*)$",&ppepper, &Server::result);
	
	o.listen();
}
