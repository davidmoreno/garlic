/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <libgen.h>

#include "inireader.h"
#include "utils.h"

using namespace Garlic;

class Garlic::GarlicPrivate{
public:
	boost::property_tree::ptree ini;
	std::string path;
};

IniReader::IniReader(const std::string& inifile)
{
	d=std::make_shared<GarlicPrivate>();
	boost::property_tree::ini_parser::read_ini(inifile, d->ini);
	char *path=strdupa(realpath(inifile).c_str());
	::dirname(path);
	d->path=path;
}

IniReader::~IniReader()
{
}

std::string IniReader::get(const std::string& field) const
{
	return d->ini.get<std::string>(field);
}

std::string IniReader::get(const std::string& field, const std::string& defaultvalue) const
{
	return d->ini.get<std::string>(field, defaultvalue);
}

std::vector<std::string> IniReader::get_keys(const std::string &group) const
{
	std::vector<std::string> ret;
	auto g=d->ini.get_child(group);
	auto I=g.begin(), endI=g.end();
	for(;I!=endI;++I){
		ret.push_back((*I).first);
	}
	return ret;
}

std::string IniReader::getPath() const
{
	return d->path;
}
