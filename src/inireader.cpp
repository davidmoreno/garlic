/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <onion/log.h>
#include <underscore/underscore.hpp>
#include <underscore/string.hpp>
#include <underscore/file.hpp>
#include <string>
#include <libgen.h>

#include "inireader.hpp"
#include "utils.hpp"

using namespace Garlic;

class Garlic::GarlicPrivate{
public:
	typedef std::map<std::string, std::map<std::string, std::string>> data_t;
	
	std::string path;
	data_t data;
};

IniReader::IniReader(const std::string& inifile)
{
	d=std::make_shared<GarlicPrivate>();
	
	std::string group;
	for(const ::underscore::string &line: underscore::file(inifile)){
		auto l=line.split('#',true)[0];
		if (l.empty())
			continue;
		if (l.startswith("[") && l.endswith("]"))
			group=l.slice(1,-2);
		else{
			auto p=l.split('=',true);
			d->data[group][p[0].strip()]=l.slice(p[0].length()+1,-1).strip();
		}
	}
	
	try{
		std::string p=get("global.path");
		if (p[0]=='/'){
			d->path=p;
		}
		else{
			d->path=realpath(d->path+"/"+p);
		}
	}
	catch(const std::exception &e){
		std::string tmp=inifile;
		d->path=realpath(dirname( (char*)inifile.c_str()) );
	}
}

IniReader::~IniReader()
{
}

std::string IniReader::get(const std::string& key) const
{
	auto p=underscore::string(key).split('.');
	auto group = d->data.find(p[0]);
	if (group!=d->data.end()){
		auto v=group->second.find(p[1]);
		if (v!=group->second.end()){
			return v->second;
		}
	}
	throw value_not_found(key);
}

bool IniReader::has(const std::string& key) const
{
	auto p=underscore::string(key).split('.');
	auto group = d->data.find(p[0]);
	if (group!=d->data.end()){
		if (p.size()==1)
			return true;
		auto v=group->second.find(p[1]);
		if (v!=group->second.end())
			return true;
	}
	return false;
}

std::string IniReader::get(const std::string& field, const std::string& defaultvalue) const
{
	try{
		return get(field);
	}
	catch(std::exception &e){
		return defaultvalue;
	}
}

std::vector<std::string> IniReader::get_keys(const std::string &group) const
{
	std::vector<std::string> ret;
 	for(const auto &v: d->data[group])
 		ret.push_back(v.first);
	return ret;
}

std::string IniReader::getPath() const
{
	return d->path;
}
