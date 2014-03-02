/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <functional>
#include <string>
#include <vector>
#include <thread>

namespace Garlic{
	class CronJob;
	class Cron{
		std::vector<std::shared_ptr<CronJob>> job_queue;
		std::thread job_thread;
		bool working;
	public:
		Cron();
		~Cron();
		
		/// Adds a function to call acording to that timespec
		void add(const std::string &timespec, const std::function<void()> &f);
		
		/// Starts the cron daemon, creates a new thread.
		void start();
		/// Stops the cron daemon
		void stop();
		
		/// Works until stop
		void work();
	};
};
