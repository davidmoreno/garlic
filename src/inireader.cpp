/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "inireader.h"

using namespace PPepper;

class PPepper::PPepperPrivate{
public:
	boost::property_tree::ptree ini;
};

IniReader::IniReader()
{
	d=std::make_shared<PPepperPrivate>();
}

IniReader::~IniReader()
{
}

void IniReader::open(const std::string& inifile)
{
	boost::property_tree::ini_parser::read_ini(inifile, d->ini);
}

std::string IniReader::get(const std::string& field)
{
	return d->ini.get<std::string>(field);
}

std::string IniReader::get(const std::string& field, const std::string& defaultvalue)
{
	return d->ini.get<std::string>(field, defaultvalue);
}

std::vector<std::string> IniReader::get_keys(const std::string &group)
{
	std::vector<std::string> ret;
	auto g=d->ini.get_child(group);
	auto I=g.begin(), endI=g.end();
	for(;I!=endI;++I){
		ret.push_back((*I).first);
	}
	return ret;
}
