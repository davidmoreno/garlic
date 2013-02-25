Garlic
======

This is a Jenkins like minimal testing server. 

It has a very simple configuration file that sets up how to perform the full testing. 

As its in a very infant state, and the goals are quite restricted, if you need something 
more complex, go for Jenkins (http://jenkins-ci.org/). I use it myself for more complex projects.

Features:
 * Very minimal configuration, depends on just one small config file and your own shell based rules and tests. Up in 2 min.
 * Very minimal memory consumption. Just now about 2MB Res memory. 
 * Very fast. Its made on C++.
 * Cron runnable check for test.
 
Vs Jenkins:
 * No need for extra servers, no tomcat, no apache/nginx frontend.
 * 2MB vs 150MB (using Jetty).
 * Just one test.
 * Really easy to understand. Just 750 lines of code.
 * C++ vs Java.
 * Potentially insecure. C++ vs Java.
 * Just one user account.

Quick Start
===========

Requirements
------------

 * Onion cpp branch:

	```
	git clone https://github.com/davidmoreno/onion.git
	cd onion
	git checkout origin/cpp -b cpp
	
	mkdir build
	cd build
	cmake ..
	make
	sudo make install
	```

 * Boost

Compile
-------

	git clone https://github.com/davidmoreno/garlic.git
	cd garlic
  mkdir build
  
	cd build
	cmake ..
	make
	sudo make install

Setup
-----

1. Create directory for your tests
2. Create a config.ini file with contents similar to this, but tailored to your needs:

	```
	[global]
	username=coralbits
	password=coralbits

	name=Example test: random, sometimes ok, sometimes fail.

	#cwd= # where to chdir, normally to test dir.

	[scripts]
	on_back_to_normal=mail -s "Garlic back to normal" "dmoreno@coralbits.com"
	on_error=mail -s "Garlic error" "dmoreno@coralbits.com"

	#check=git fetch && [ $(git diff origin/master | wc -l) = 0 ]
	check=false
	test=./test.sh # must include update/pull if necessary

	[env]
	FROM=dmoreno@coralbits.com
	TO=dmoreno@coralbits.com
	```
	
3. Add to cron to check each hour (optional):
	
	crontab -e

		00 * * * * garlic PATH_TO_CONFIG --check-and-run

4. Run the webserver:

	garlic PATH_TO_CONFIG 

