/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <onion/onion.hpp>
#include <onion/url.hpp>
#include <onion/log.h>
#include <string.h>

#include "server.h"
#include "utils.h"
#include "inireader.h"

using namespace Onion;
using namespace Garlic;

void usage(){
	ONION_ERROR("Usage: garlic <configdir> [--check-and-run]");
	exit(1);
}


int main(int argc, char **argv){
	if (argc==1){
		usage();
	}
	if (argc==3){
		if (strcmp(argv[2],"--check-and-run")==0){
			IniReader ini(argv[1]);
			Test test( ini );
			bool ok=test.check_and_run();
			exit(ok);
		}
		else{
			usage();
		}
	}
	
	::Onion::Onion o;
	IniReader ini(argv[1]);
	Server garlic(ini);
	ONION_INFO("Garlic at path %s, listening at %s:%s", ini.getPath().c_str(), ini.get("server.address","::").c_str(), ini.get("server.port","8000").c_str());
	
	o.setPort(ini.get("server.port","8000"));
	o.setHostname(ini.get("server.address","8000"));
	
	Url root(o);
	
	root.add("",&garlic, &Server::login);
	root.add("style.css",&garlic, &Server::style_css);
	root.add("index",&garlic, &Server::index);
	root.add("results",&garlic, &Server::results_json);
	root.add("^result/(.*)$",&garlic, &Server::result);
	
	o.listen();
}
