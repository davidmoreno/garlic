/**
 * This Source Code Form is subject to the terms 
 * of the Mozilla Public License, v. 2.0. If a 
 * copy of the MPL was not distributed with this 
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <functional>
#include <string>
#include <queue>

namespace Garlic{
	class Cron{
// 		typedef std::tuple<int, const std::string &timespec, std::function<void ()> job;
		
		std::priority_queue<int> task_queue;
	public:
		Cron();
		~Cron();
		
		/// Adds a function to call acording to that timespec
		void add(const std::string &timespec, const std::function<void()> &f);
		
		/// Starts the cron daemon, creates a new thread.
		void start();
		/// Stops the cron daemon
		void stop();
	};
};
