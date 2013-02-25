/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <onion/response.hpp>
#include <onion/dict.hpp>
#include <onion/request.hpp>
#include <onion/shortcuts.h>

#include <boost/algorithm/string.hpp>

#include <time.h>
#include <assert.h>
#include <dirent.h>
#include <vector>
#include <sstream>

#include "server.h"
#include "utils.h"

using namespace Onion;
using namespace Garlic;

extern "C" void login_html(onion_dict *context, onion_response *res);
extern "C" void index_html(onion_dict *context, onion_response *res);
extern "C" void style_css(onion_dict *context, onion_response *res);

Server::Server(const std::string &configdir_) : configdir(realpath(configdir_)){
	
	test.setDefaultdir(this->configdir);
	
	ini.open(this->configdir+"/config.ini");
	test.setIniReader(ini);
}

onion_connection_status Server::login(Onion::Request &req, Onion::Response &res){
	if (req.session().has("loggedin"))
		return onion_shortcut_redirect("/index",req.c_handler(), res.c_handler());
	Dict context;
	if (req.post().has("username")){
		if (req.post().get("username")==ini.get("global.username") && 
				req.post().get("password")==ini.get("global.username")){
			req.session().add("loggedin","true");
			return onion_shortcut_redirect("/index",req.c_handler(), res.c_handler());
		}
		else{
			context.add("error","Invalid username or password.");
		}
	}
	
	context.add("title", ini.get("global.name",""));
	ONION_DEBUG("Login ctx: %s",context.toJSON().c_str());
	
	login_html(context.c_handler(), res.c_handler());
	
	return OCS_PROCESSED;
}

onion_connection_status Server::style_css(Onion::Request &req, Onion::Response &res){
	::style_css(nullptr, res.c_handler());
	return OCS_PROCESSED;
}

onion_connection_status Server::logout(Onion::Request &req, Onion::Response &res){
	if (req.session().has("loggedin")){
		req.session().remove("loggedin");
	}
	return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
}

onion_connection_status Server::index(Onion::Request &req, Onion::Response &res){
	if (!req.session().has("loggedin"))
		return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
	
	Dict context;
	if (req.post().has("run")){
		std::string test_name=run_test();
		context.add("test_name",test_name);
	}
	context.add("title", ini.get("global.name",""));
	
	index_html(context.c_handler(), res.c_handler());
	return OCS_PROCESSED;
}

onion_connection_status Server::results_json(Onion::Request &req, Onion::Response &res){
	if (!req.session().has("loggedin"))
		return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
	std::vector<std::string> files;
	
	DIR *dir=opendir((configdir+"/log").c_str());

	if (dir){
		struct dirent *ent;
		while ( (ent=readdir(dir)) ){
			std::string filename=ent->d_name;
			if (boost::algorithm::ends_with(filename,".pid"))
				files.push_back(filename.substr(0,filename.length()-4));
		}
		closedir(dir);
	}
	
	std::sort(files.begin(), files.end(),[](const std::string &A, const std::string &B){ return B<A; });
	
	Dict ret;
	int n=8;
	int count=files.size();
	for (const std::string &name: files){
		if (n--==0)
			break;
			
		Dict data;
		std::stringstream ss;
		ss<<"#"<<count;
		count--;
		
		data.add("name",ss.str());
		data.add("timestamp",name);
		
		try{
			auto result=file2string(configdir+"/log/"+name+".result");
			data.add("result",result);
		}
		catch(...){ // if no result, still running.. unless something went bad.
			data.add("running","true"); // Fixme to real check if running
		}
		
		ret.add(name, data);
	}
	
	res<<ret.toJSON();
	return OCS_PROCESSED;
}

onion_connection_status Server::result(Onion::Request &req, Onion::Response &res){
	if (!req.session().has("loggedin"))
		return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
	
	std::string filename=(configdir+"/log/"+req.query()["1"]+".output").c_str();
	if (filename.find("..")!=std::string::npos){
		ONION_WARNING("Trying to read out of log dir: %s", filename.c_str());
		return OCS_INTERNAL_ERROR;
	}
	
	ONION_DEBUG("Return response from %s",filename.c_str());
	return onion_shortcut_response_file(filename.c_str(), req.c_handler(), res.c_handler());
}


std::string Server::run_test(){
	char now[32];
	snprintf(now,sizeof(now),"%ld",time(nullptr));

	if (fork()==0){ // Another process.
		int ok=test.run(now);
		
		exit(ok);
	}
	return now;
}
