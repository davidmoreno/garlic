/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <onion/log.h>
#include <unistd.h>
#include <assert.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "test.h"

using namespace Garlic;

Test::Test(IniReader &reader) : ini(reader)
{
	
}

bool Test::check_and_run()
{
	if (check()){
		char now[32];
		snprintf(now,sizeof(now),"%ld",time(NULL));
		run(now);
		return true;
	}
	return false;
}

bool Test::check()
{
	setup_env();
	int ok=system(ini.get("scripts.check").c_str());
	return ok!=0;
}


int Test::run(const std::string &testname)
{
	setup_env();
	char tmp[1024];
	const std::string logpath=ini.get("logpath", ini.getPath()+ "/log/");

	ONION_DEBUG("Log path is %s", logpath.c_str());
	{
		int err_mkdir=mkdir(logpath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (err_mkdir <0 && err_mkdir != -EEXIST){
			ONION_ERROR("Error creating log path %s: %s", logpath.c_str(), strerror(errno));
		}
	}
	std::string basefilename=logpath+testname;
	
	ONION_DEBUG("Debug to %s",(basefilename+".[pid,output,result]").c_str());
	ONION_DEBUG("Output to %s",(basefilename+".output").c_str());

	int fd;
	fd=open((basefilename+".pid").c_str(), O_WRONLY|O_CREAT, 0666);
	if (fd<0)
		perror("Cant open pid file");
	assert(fd>=0);
	snprintf(tmp,sizeof(tmp),"%d",getpid());
	int w=write(fd,tmp,strlen(tmp));
	assert(w==(signed)strlen(tmp));
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
	w=write(fd,tmp,strlen(tmp));
	assert(w==(signed)strlen(tmp));
	close(fd);

	{ // Do something with the results.
		std::string error_on_last_file(logpath+"/error_on_last");
		fd=close(0); // Close just in case
		fd=open((basefilename+".output").c_str(), O_RDONLY);
		assert(fd==0);
		if (ok!=0){
			ONION_DEBUG("Error! Execute: %s", ini.get("scripts.on_error").c_str());
			int ok=system(ini.get("scripts.on_error").c_str());
			if (ok!=0){
			  ONION_ERROR("Coult not execute on_error script");
			}
			fd=open(error_on_last_file.c_str(),O_WRONLY|O_CREAT, 0666);
			assert(fd>=0);
			close(fd);
		}
		else if (access(error_on_last_file.c_str(), F_OK)!=-1){
			ONION_DEBUG("Success after many errors! Execute: %s", ini.get("scripts.on_back_to_normal").c_str());
			ok=system(ini.get("scripts.on_back_to_normal").c_str());
			if (ok!=0){
			  ONION_ERROR("Error executing script on_back_to_normal");
			}
			unlink(error_on_last_file.c_str());
		}
		fd=close(0);
		assert(fd==0);
	}
	
	return ok;
}

void Test::setup_env(){
	int ok=chdir(ini.getPath().c_str());
	if (ok<0){
	  ONION_ERROR("Could not chdir to %s: %s", ini.getPath().c_str(), strerror(errno));
	}
	try{
		auto ks=ini.get_keys("env");
		auto I=ks.begin(), endI=ks.end();
		for(;I!=endI;++I){
			const auto &k=*I;
			ONION_DEBUG("Set env %s=%s",k.c_str(), ini.get("env."+k).c_str());
			setenv(k.c_str(), ini.get("env."+k).c_str(), 1);
		}
		ONION_DEBUG("Chdir to %s",ini.get("global.cwd",ini.getPath()).c_str());
	}
	catch(...){
		// pass FIXME better exception handling.
	}
}

