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

Test::Test()
{
}

void Test::setIniReader(IniReader &reader){
	ini=reader;
}

void Test::setIniFile(const std::string& inifile)
{
	ini.open(inifile);
}

void Test::setDefaultdir(const std::string& defaultdir)
{
	this->defaultdir=defaultdir;
}


bool Test::check_and_run()
{
	if (check()){
		char now[32];
		snprintf(now,sizeof(now),"%ld",time(nullptr));
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

	mkdir((defaultdir+"/log/").c_str(), 0666);
	std::string basefilename=defaultdir+"/log/"+testname;
	
	ONION_DEBUG("Debug to %s",(basefilename+".[pid,output,result]").c_str());
	ONION_DEBUG("Output to %s",(basefilename+".output").c_str());

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
		std::string error_on_last_file(defaultdir+"/log/error_on_last");
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
	
	return ok;
}

void Test::setup_env(){
	for(const std::string &k: ini.get_keys("env")){
		ONION_DEBUG("Set env %s=%s",k.c_str(), ini.get("env."+k).c_str());
		setenv(k.c_str(), ini.get("env."+k).c_str(), 1);
	}
	ONION_DEBUG("Chdir to %s",ini.get("global.cwd",defaultdir).c_str());
	chdir(ini.get("global.cwd",defaultdir).c_str());
}

