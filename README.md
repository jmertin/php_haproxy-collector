# php_haproxy-collector 1.0-5 by J. Mertin -- joerg.mertin@broadcom.com
Purpose: Collects haproxy csv stats and sends them to restfull enabled APMIA 

# Description
## Introduction

this package provides the possibility to import haproxy statistics
into the APMIA.

### Requirements

- The restfull interface of the APMIA needs to be enabled
- HAProxy needs to provide the statistics
- PHP needs to have posix, pcntl, curl installed (extensions)

## APMIA configuration

The detailed documentation for the APMIA can be found under
https://techdocs.broadcom.com/us/en/ca-enterprise-software/it-operations-management/application-performance-management/10-7/implementing-agents/infrastructure-agent/epagent-plug-ins/configure-epagent-network-ports/enable-the-epagent-plug-ins-restful-interface.html

#### Enable the REST interface to your Infrastructure Agent.

- Navigate to the <Agent_Home>/core/config directory and open the IntroscopeAgent.profile in a text editor.   
Look for:
```
introscope.epagent.config.httpServerPort=8080
```
and set the port to whatever port is available for polling.


### HAProxy

Docs are located here:   
- http://cbonte.github.io/haproxy-dconv/1.7/management.html#9.1
- http://cbonte.github.io/haproxy-dconv/1.5/snapshot/configuration.html#9.1

Note currently only haproxy 1.5 and 1.7 are supported.
Though new versions can be added easily.

### Configuration variables

The following variables need to be defined:

- hapver: default "1.7" - defines which haproxy the system will poll/

- hapurl: Format "https://haproxy-url.domain.tld/haproxy_stats;csv" - No default.
Note the ";csv" at the end is required. Also, do _not_ add the port   
for the stats-UI into the URL!

- hapuser: default "admin" - The user that is used to authenticate to access the statistics

- happass: No default. The associated password to hapuser!

- happort: default "2000" - the port to access the haproxy statistics.

- self_signed: default "false" - If the certificate to access the haproxy interface is self signed, set this variable to "true".

- send_all: default "false" - By default the collector will only send metrics that have values associated. If the value fields are empty, the metric will be skipped.

- apmia_url: No default - "http://apmia-url.domain.tld/apm/metricFeed" the url used to access the APMIA restful collector.

- apmia_port: default 8080 - The port the restful collector is listening.

- poll_time: 7 - The interval between sending the stats. Note that the counter will start once all the data was sent.

- hapdebug: default "false" - Set to true for activating debugging

- hapstats: default "false" - Set to true for activating stats in console

## Running the CLI Version only

The CLI version can be run from any location.
Just edit the run.sh script, provide the correct values, and execute it.
```
php_haproxy_collector$ ./run.sh 

*** HAProxy stats to APMIA RESTFul collector started
 => The following configuration is set (conf/hacollector.conf)
 =============================================================
 => HA Proxy version: 1.7 
    - haproxy URL: https://pcm.solsys.org/haproxy_stats;csv 
    - haproxy port: 2000 
    - haproxy access via user: admin 
 => APMIA configuration
    - APMIA target URL: http://calypso.solsys.org/apm/metricFeed 
    - APMIA Port: 8080 
    - Poll time: 7 
 => Executing! 
    - Debug flag:  
    - Statistics flag:  
 => Lockfile hacollector.php.39a80fac..LCK created.
```

## Running the docker-version




# Manual Changelog
```
Fri, 25 Mar 2022 17:07:11
	- Initial release
```
