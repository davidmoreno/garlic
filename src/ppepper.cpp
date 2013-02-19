#include <onion/onion.hpp>
#include <onion/url.hpp>
#include <onion/response.hpp>
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

#define USERNAME "dmoreno"
#define PASSWORD "dmoreno"

using namespace Onion;

extern "C" void login_html(onion_dict *context, onion_response *res);
extern "C" void index_html(onion_dict *context, onion_response *res);
extern "C" void style_css(onion_dict *context, onion_response *res);

class PPepper{
private:
	std::string configdir;
public:
	PPepper(const std::string &configdir){
		char *basefilename_c;
		basefilename_c=realpath(configdir.c_str(),NULL);
		if (basefilename_c==NULL){
			ONION_ERROR("Error getting realpath %s: %s",configdir.c_str(), strerror(errno));
			throw(std::exception());
		}
		this->configdir=basefilename_c;
		free(basefilename_c);
	}
	
	onion_connection_status login(Onion::Request &req, Onion::Response &res){
		if (req.session().has("loggedin"))
			return onion_shortcut_redirect("/index",req.c_handler(), res.c_handler());
		Dict context;
		if (req.post().has("username")){
			if (req.post().get("username")==USERNAME && req.post().get("password")==PASSWORD){
				req.session().add("loggedin","true");
				return onion_shortcut_redirect("/index",req.c_handler(), res.c_handler());
			}
			else{
				context.add("error","Invalid username or password.");
			}
		}
		
		login_html(context.c_handler(), res.c_handler());
		
		return OCS_PROCESSED;
	}

	onion_connection_status style_css(Onion::Request &req, Onion::Response &res){
		::style_css(nullptr, res.c_handler());
		return OCS_PROCESSED;
	}

	onion_connection_status logout(Onion::Request &req, Onion::Response &res){
		if (req.session().has("loggedin")){
			req.session().remove("loggedin");
			return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
		}
	}

	onion_connection_status index(Onion::Request &req, Onion::Response &res){
		if (!req.session().has("loggedin"))
			return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
		
		Dict context;
		if (req.post().has("run")){
			std::string test_name=run_test();
			context.add("test_name",test_name);
		}
		
		index_html(context.c_handler(), res.c_handler());
		return OCS_PROCESSED;
	}
	
	onion_connection_status results_json(Onion::Request &req, Onion::Response &res){
		if (!req.session().has("loggedin"))
			return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
		
		Dict ret;
		DIR *dir=opendir((configdir+"/log").c_str());
		struct dirent *ent;
		while ( (ent=readdir(dir)) ){
			std::string filename=ent->d_name;
			if (boost::algorithm::ends_with(filename,".pid")){
				Dict data;
				std::string name=filename.substr(0,filename.length()-4);
				data.add("name",name);
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
		}
		
		closedir(dir);
		
		res<<ret.toJSON();
		return OCS_PROCESSED;
	}
	
	onion_connection_status result(Onion::Request &req, Onion::Response &res){
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
	
	static std::string file2string(const std::string &filename){
		std::ifstream ifs(filename);
		if (!ifs.is_open())
			throw(std::exception());
		return std::string ( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
	}
	
	std::string run_test(){
		char now[1024];
		char tmp[1024];
		snprintf(now,sizeof(now),"%ld",time(nullptr));

		std::string basefilename=configdir+"/log/"+now;
		
		ONION_DEBUG("Debug to %s",(basefilename+".[pid,output,result]").c_str());

		if (fork()==0){ // Another process.
			int fd;
			fd=open((basefilename+".pid").c_str(), O_WRONLY|O_CREAT, 0666);
			if (fd<0)
				perror("Cant open pid file");
			assert(fd>=0);
			snprintf(tmp,sizeof(tmp),"%d",getpid());
			write(fd,tmp,strlen(tmp));
			close(fd);
			
			chdir(configdir.c_str());
			close(0);
			close(1);
			close(2);

			fd=open("/dev/null", O_RDONLY);
			assert(fd==0);
			fd=open( (basefilename+".output").c_str(), O_WRONLY|O_CREAT, 0666);
			assert(fd==1);
			fd=dup2(1,2);
			assert(fd==2);
			
			int ok=system("./test.sh");
			fd=close(0);
			assert(fd==0);
			fd=close(1);
			assert(fd==0);
			fd=close(2);
			assert(fd==0);
			
			assert(fd>=0);
			snprintf(tmp,16,"%d",ok);
			
			fd=open((basefilename+".result").c_str(), O_WRONLY|O_CREAT, 0666);
			write(fd,tmp,strlen(tmp));
			close(fd);
			exit(ok);
		}
		return now;
	}
};


int main(int argc, char **argv){
	if (argc==1){
		ONION_ERROR("Usage: %s <configdir>",argv[0]);
		return 1;
	}
	
	Onion::Onion o;
	PPepper ppepper(argv[1]);
	
	Onion::Url root(o);
	
	root.add("",&ppepper, &PPepper::login);
	root.add("style.css",&ppepper, &PPepper::style_css);
	root.add("index",&ppepper, &PPepper::index);
	root.add("results",&ppepper, &PPepper::results_json);
	root.add("^result/(.*)$",&ppepper, &PPepper::result);
	
	o.listen();
}
