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

using namespace Onion;
using namespace PPepper;

void usage(){
	ONION_ERROR("Usage: ppepper <configdir> [--check-and-run]");
	exit(1);
}


int main(int argc, char **argv){
	if (argc==1){
		usage();
	}
	if (argc==3){
		if (strcmp(argv[2],"--check-and-run")==0){
			Test test;
			std::string path(realpath(argv[1]));
			test.setDefaultdir(path);
			test.setIniFile(path+"/config.ini");
			
			int ok=test.check_and_run();
			exit(ok);
		}
		else{
			usage();
		}
	}
	
	::Onion::Onion o;
	Server ppepper(argv[1]);
	
	Url root(o);
	
	root.add("",&ppepper, &Server::login);
	root.add("style.css",&ppepper, &Server::style_css);
	root.add("index",&ppepper, &Server::index);
	root.add("results",&ppepper, &Server::results_json);
	root.add("^result/(.*)$",&ppepper, &Server::result);
	
	o.listen();
}
