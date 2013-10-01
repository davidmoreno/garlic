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
 * Allow several tests.
 * Change env variables on interface for runtime change of the behaviour of your tests.
 
Vs Jenkins:
 * No need for extra servers, no tomcat, no apache/nginx frontend.
 * 2MB vs 150MB (using Jetty).
 * Really easy to understand. Just 750 lines of code.
 * C++ vs Java.
 * Just one user account.

Quick Start
===========

Requirements
------------

 * Onion:

```
git clone https://github.com/davidmoreno/onion.git
cd onion

mkdir build
cd build

cmake ..
make

sudo make install
```

 * Boost

Compile
-------

```
git clone https://github.com/davidmoreno/garlic.git
cd garlic

mkdir build
cd build

cmake ..
make

sudo make install
```

Setup
-----

1. Create directory for your tests
2. Create a config.ini file with contents similar to this, but tailored to your needs:

```
[global]
# if no password, then free entry.
username=coralbits
password=coralbits

name=Example test: random, sometimes ok, sometimes fail.

#cwd= # where to chdir, defaults to test dir.

[scripts]
# command to run when an error happens. As stdin it receives the output of the test.
on_error=mail -s "Garlic error" "dmoreno@coralbits.com"
# command to run when after an error we get a success. As stdin it receives the output of the test.
on_back_to_normal=mail -s "Garlic back to normal" "dmoreno@coralbits.com"

# check to run for --check_and_run command line parameter.
#check=git fetch && [ $(git diff origin/master | wc -l) = 0 ]
check=false
# test to run
test=./test.sh # must include update/pull if necessary
# name of the test
test_name=Generic test

# if more than one, add _1, _2... to test and test_name.
test_1=git pull
test_name_1=Pull

[env]
# envirnomental variables added before runnign tests and on_error/on_back_to_normal
FROM=dmoreno@coralbits.com
TO=dmoreno@coralbits.com

[env-rw]
# env-rw are environmental varaibles modifiable at the gui when running the tests.

# with defaul option
PARAM1=Default value
# No default option.
PARAM2=
```
	
3. Add to cron to check each hour (optional):

	
```
crontab -e

	00 * * * * garlic PATH_TO_CONFIG --check-and-run
```

4. Run the webserver:


```
garlic PATH_TO_CONFIG_INI
```

[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/d20adecf5166eaee302d188aeb41656d "githalytics.com")](http://githalytics.com/davidmoreno/garlic)

