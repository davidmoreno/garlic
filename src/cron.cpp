/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cron.hpp"
#include <iostream>
#include <stdio.h>
#include <limits>
#include <exception>
#include <sstream>
#include <memory>
#include <string.h>

#include <underscore/underscore.hpp>
#include <underscore/strings.hpp>

using namespace Garlic;
using namespace underscore;


class ParseCron{
	static const int max_year = 2040;
	
	class unsatisfiable : public std::exception{ /// Cant satisfy the cron expression request.
	public:
	};
	
	struct candidate_t{
		int data[5]; // year-month-day hour:minute
		
		int &operator[](int p){
			return data[p];
		}
		int operator[](int p) const{
			return data[p];
		}
		
		time_t to_unix_timestamp() const{
			struct tm tm;
			memset(&tm,0,sizeof(tm));
			tm.tm_year=data[0]-1900;
			tm.tm_mon=data[1]-1;
			tm.tm_mday=data[2];
			tm.tm_hour=data[3];
			tm.tm_min=data[4];
			tm.tm_isdst=-1;

			return mktime(&tm);
		}
		
		std::string to_string() const{
// 			return string("{}-{}-{} {}:{}").args(data);
			std::stringstream ss;
			ss<<data[0]<<"-"<<data[1]<<"-"<<data[2]<<" "<<data[3]<<":"<<data[4];
			return ss.str();
		}
	};

	class Check {
	protected:
		std::shared_ptr<Check> overflow_part; // For minutes is hours, so at minute overflow, increment hour.
	public:
		Check(){};
		
		virtual bool valid(const candidate_t &candidate) = 0;
		virtual bool incr(candidate_t &candidate) = 0; // After incr, is candidate still valid, or should I look from the begining again.
		virtual std::string to_string() = 0;
		void set_overflow_part(std::shared_ptr<Check> prev){
			overflow_part=prev;
		}
	};

	class InRange : public Check{
		int min, max;
		int each;
		int partn;
	public:
		InRange(const std::string &rule, int _min, int _max, int partn) : min(_min), max(_max), each(1), partn(partn){
			std::cout<<"Create rule "<<partn<<" "<<rule<<std::endl;
			if (rule=="*"){
				std::cout<<"All"<<std::endl;
				return;
			}
			min=max=std::stoi(rule); // single number
			std::cout<<"Range "<<min<<"-"<<max<<std::endl;
		};
		
		std::string to_string(){
			return std::to_string(partn)+" in range "+std::to_string(min)+" - "+std::to_string(max);
		}
		
		bool valid(const candidate_t &candidate){
// 			std::cout<<"check "<<to_string()<<" "<<candidate.to_string()<<" "<<(candidate[partn]>=min && candidate[partn]<=max)<<std::endl;
			return candidate[partn]>=min && candidate[partn]<=max; // and each
		}
		
		bool incr(candidate_t &candidate){
			if (candidate[partn]<min){
				candidate[partn]=min;
				return true;
			}
			candidate[partn]++;
			if (candidate[partn]>=max){
				candidate[partn]=min;
				if (overflow_part){
					overflow_part->incr(candidate);
				}
			}
			return true;
		}
	};
	
	class WeekDay : public Check{
	public:
		WeekDay(const std::string &expr) {}
		virtual bool incr(candidate_t &candidate) override { 
			if (overflow_part){
				overflow_part->incr(candidate);
			}
			return false;
		}
		virtual bool valid(const candidate_t &candidate) { return true; }
		virtual std::string to_string() override { return "Week of day"; }
	};

	class ValidDate : public Check{
	public:
		virtual bool valid(const candidate_t &candidate) { 
			if (candidate[1]==2){
				if (candidate[2]==29){
					// Check if leap year, and if its leap, 29th feb is possible.
					if ((candidate[0]%4)==0)
						return true;
					else if ((candidate[0]%400)==0)
						return true;
					else if ((candidate[0]%100)==0)
						return false;
					return false;
				}
				if (candidate[2]>=30)
					return false;
			}
			if (candidate[2] == 31){
				if ( (candidate[1] + ((candidate[1]/8)%2) ) % 2) 
					return false;
			}
			return true;
		}
		virtual bool incr(candidate_t &candidate) override { 
			if (overflow_part){
				overflow_part->incr(candidate);
			}
			return false;
		}
		virtual std::string to_string() override { return "Valid date (leap years, 30 or 31 months...)"; }
	};
	
public:
	static time_t next(const std::string &timespec){
		std::cout<<timespec<<std::endl;
		auto parts=string(timespec).split(' ');
		if (parts.size()!=6){
			throw(std::exception());
		}
		std::vector<std::shared_ptr<Check>> rules{
			std::make_shared<InRange>(parts[0], 0,59, 4), // minutes
			std::make_shared<InRange>(parts[1], 0,24, 3), // hours
			std::make_shared<InRange>(parts[2], 0,31, 2), // day of month
			std::make_shared<InRange>(parts[3], 0,12, 1), // month
			std::make_shared<InRange>(parts[5], 1970,max_year+1, 0), // year
			std::make_shared<WeekDay>( parts[4] ),
			std::make_shared<ValidDate>( )
		};
		for(int i=0;i<4;i++)
			rules[i]->set_overflow_part(rules[i+1]);
		rules[5]->set_overflow_part(rules[2]);
		rules[6]->set_overflow_part(rules[2]);
		
		struct tm *tm;
		time_t rawtime;

		time (&rawtime);
		tm = localtime (&rawtime);

		candidate_t candidate{tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min};
		rules[0]->incr(candidate);
		
		bool valid=true;
		do{
			valid=true;
			std::cout<<std::endl;
			for(auto &r: rules){
				std::cout<<candidate.to_string()<<" Check rule "<<r->to_string()<<std::endl;
				while(!r->valid(candidate)){
					std::cout<<"incr by <"<<r->to_string()<<"> incr "<<candidate.to_string()<<" to ";
					valid&=r->incr(candidate);
					std::cout<<candidate.to_string()<<std::endl;
					if (!valid) // Start over again, this is normally not good day of week, or 30 feb style dates.
						break; 
				}
			}
			if (candidate[0]>=max_year)
				throw(unsatisfiable());
		}while(!valid);
		std::cout<<"Final "<<candidate.to_string()<<std::endl;
		
		return candidate.to_unix_timestamp();
	}
};

Cron::Cron()
{

}

Cron::~Cron()
{
	stop();
}

void Cron::add(const std::string& timespec, const std::function< void () >& f)
{
	int next_t=ParseCron::next(timespec);
	std::cout<<"Next occurence is "<<next_t<<", which means in "<<(next_t-time(NULL))<<std::endl;
// 	queue.push({next_t, timespec, f});
	// FIXME check if its next, and if so, cancel the timer and restart.
}

void Cron::start(){
}

void Cron::stop()
{

}
