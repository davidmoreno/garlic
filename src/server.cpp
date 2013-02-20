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

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include "server.h"

using namespace Onion;
using namespace PPepper;

extern "C" void login_html(onion_dict *context, onion_response *res);
extern "C" void index_html(onion_dict *context, onion_response *res);
extern "C" void style_css(onion_dict *context, onion_response *res);

static std::string file2string(const std::string &filename){
	std::ifstream ifs(filename);
	if (!ifs.is_open())
		throw(std::exception());
	return std::string ( (std::istreambuf_iterator<char>(ifs) ),
											(std::istreambuf_iterator<char>()    ) );
}

Server::Server(const std::string &configdir){
	char *basefilename_c;
	basefilename_c=realpath(configdir.c_str(),NULL);
	if (basefilename_c==NULL){
		ONION_ERROR("Error getting realpath %s: %s",configdir.c_str(), strerror(errno));
		throw(std::exception());
	}
	this->configdir=basefilename_c;
	free(basefilename_c);
	
	ini.open(this->configdir+"/config.ini");
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
	
	context.add("title", ini.get("globals.name",""));
	
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
	context.add("title", ini.get("globals.name",""));
	
	index_html(context.c_handler(), res.c_handler());
	return OCS_PROCESSED;
}

onion_connection_status Server::results_json(Onion::Request &req, Onion::Response &res){
	if (!req.session().has("loggedin"))
		return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
	
	DIR *dir=opendir((configdir+"/log").c_str());
	struct dirent *ent;

	std::vector<std::string> files;
	while ( (ent=readdir(dir)) ){
		std::string filename=ent->d_name;
		if (boost::algorithm::ends_with(filename,".pid"))
			files.push_back(filename.substr(0,filename.length()-4));
	}
	closedir(dir);
	
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
	char now[1024];
	char tmp[1024];
	snprintf(now,sizeof(now),"%ld",time(nullptr));

	mkdir((configdir+"/log/").c_str(), 0666);
	std::string basefilename=configdir+"/log/"+now;
	
	ONION_DEBUG("Debug to %s",(basefilename+".[pid,output,result]").c_str());

	if (fork()==0){ // Another process.
		setup_env();
		ONION_DEBUG("Chdir to %s",ini.get("global.cwd",configdir).c_str());
		chdir(ini.get("global.cwd",configdir).c_str());
		
		int fd;
		fd=open((basefilename+".pid").c_str(), O_WRONLY|O_CREAT, 0666);
		if (fd<0)
			perror("Cant open pid file");
		assert(fd>=0);
		snprintf(tmp,sizeof(tmp),"%d",getpid());
		write(fd,tmp,strlen(tmp));
		close(fd);
		
		int fd_stderr=dup(2);
		stderr=fdopen(fd_stderr,"w");
		int ok;
		
		{ // Run of the test command, close all fds, open output to result file, close all at the end.
			close(0);
			close(1);
			close(2);

			fd=open("/dev/null", O_RDONLY);
			assert(fd==0);
			fd=open( (basefilename+".output").c_str(), O_WRONLY|O_CREAT, 0666);
			assert(fd==1);
			fd=dup2(1,2);
			assert(fd==2);
			
			ok=system(ini.get("scripts.test","./test.sh").c_str());
			
			fd=close(0);
			assert(fd==0);
			fd=close(1);
			assert(fd==0);
			fd=close(2);
			assert(fd==0);
		}

		fd=dup2(fd_stderr,2);
		assert(fd==2);
		fclose(stderr);
		stderr=fdopen(2,"w");
		
		ONION_DEBUG("Test finished. Result %d", ok);
		
		snprintf(tmp,16,"%d",ok);
		fd=open((basefilename+".result").c_str(), O_WRONLY|O_CREAT, 0666);
		write(fd,tmp,strlen(tmp));
		close(fd);

		{ // Do something with the results.
			std::string error_on_last_file(configdir+"/log/error_on_last");
			fd=close(0); // Close just in case
			fd=open((basefilename+".output").c_str(), O_RDONLY);
			assert(fd==0);
			if (ok!=0){
				ONION_DEBUG("Error! Execute: %s", ini.get("scripts.on_error").c_str());
				system(ini.get("scripts.on_error").c_str());
				fd=open(error_on_last_file.c_str(),O_WRONLY|O_CREAT, 0666);
				assert(fd>=0);
				close(fd);
			}
			else if (access(error_on_last_file.c_str(), F_OK)!=-1){
				ONION_DEBUG("Success after many errors! Execute: %s", ini.get("scripts.on_back_to_normal").c_str());
				system(ini.get("scripts.on_back_to_normal").c_str());
				unlink(error_on_last_file.c_str());
			}
			fd=close(0);
			assert(fd==0);
		}

		
		exit(ok);
	}
	return now;
}

void Server::setup_env(){
	for(const std::string &k: ini.get_keys("env")){
		ONION_DEBUG("Set env %s=%s",k.c_str(), ini.get("env."+k).c_str());
		setenv(k.c_str(), ini.get("env."+k).c_str(), 1);
	}
}
