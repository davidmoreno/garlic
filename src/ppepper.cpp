#include <onion/onion.hpp>
#include <onion/url.hpp>
#include <onion/response.hpp>
#include <onion/request.hpp>
#include <onion/shortcuts.h>

#define USERNAME "dmoreno"
#define PASSWORD "dmoreno"

using namespace Onion;

extern "C" void login_html(onion_dict *context, onion_response *res);
extern "C" void index_html(onion_dict *context, onion_response *res);

class PPepper{
public:
	onion_connection_status login(Onion::Request &req, Onion::Response &res){
		ONION_DEBUG("Login?");
		if (req.session().has("loggedin"))
			return onion_shortcut_redirect("/index",req.c_handler(), res.c_handler());
		ONION_DEBUG("Not logged in");
		Dict context;
		if (req.post().has("username")){
			ONION_DEBUG("Check passwd");
			if (req.post().get("username")==USERNAME && req.post().get("password")==PASSWORD){
				req.session().add("loggedin","true");
				ONION_DEBUG("Login!");
				return onion_shortcut_redirect("/index",req.c_handler(), res.c_handler());
			}
			else{
				context.add("error","Invalid username or password.");
			}
		}
		
		ONION_DEBUG("Return hello");
		login_html(context.c_handler(), res.c_handler());
		
		return OCS_PROCESSED;
	}

	onion_connection_status logout(Onion::Request &req, Onion::Response &res){
		if (req.session().has("loggedin")){
			req.session().remove("loggedin");
			return onion_shortcut_redirect("/",req.c_handler(), res.c_handler());
		}
	}

	onion_connection_status index(Onion::Request &req, Onion::Response &res){
		Dict context;
		if (req.post().has("run")){
			int ok=system("cd /home/dmoreno/src/onion/build/tests/; ./auto.sh");
			context.add("result",ok==0 ? "ok" : "error");
		}
		
		index_html(context.c_handler(), res.c_handler());
		return OCS_PROCESSED;
	}
};


int main(void){
	Onion::Onion o;
	PPepper ppepper;
	
	Onion::Url root(o);
	
	root.add("",&ppepper, &PPepper::login);
	root.add("index",&ppepper, &PPepper::index);
	
	o.listen();
}
