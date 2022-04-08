# php_haproxy-collector 1.0-6 by J. Mertin -- joerg.mertin@broadcom.com
Purpose: Collects haproxy csv stats and sends them to restfull enabled APMIA 

# Description
## Introduction

this package provides the possibility to import haproxy statistics
into the APMIA.  The script can be used on a per haproxy server.
The idea to write the HAProxy collector was to reduce the resource
footprint so it can be ran on any system.

Note that the memory requirement will depend on the HAProxy server
provided statistics - as these will be worked out in memory itself.
For example, polling a small HAProxy servicing 2 sites will require
around 14MBytes of real memory measured using smem/PSS value using the
csv poller. The json poller will use 16MBytes of ram as the json
payload is way larger than the csv one.

The main differences between the CSV and the JSON poller is that the
json-poller provides by default all field names, while the csv-poller
requires the configuration to be "adapted" for each new version.


### Requirements

- The restfull interface of the APMIA needs to be enabled
- HAProxy needs to provide the statistics
- PHP 7.4 needs to have posix, pcntl, curl installed (extensions)

## APMIA configuration

The detailed documentation for the APMIA Restful API and usage can be found under 
[Enable the EPAgent Plug-ins RESTful Interface](https://techdocs.broadcom.com/us/en/ca-enterprise-software/it-operations-management/application-performance-management/10-7/implementing-agents/infrastructure-agent/epagent-plug-ins/configure-epagent-network-ports/enable-the-epagent-plug-ins-restful-interface.html)

#### Enable the REST interface on your Infrastructure Agent

- Navigate to the [Agent_Home]/core/config directory and open the IntroscopeAgent.profile in a text editor.   
Look for:
```
introscope.epagent.config.httpServerPort=8080
```
and set the port to whatever port is available. The collector will send the received metrics to this port.


### HAProxy

the current version of haproxy_collector supports haproxy collecting
statistics in csv format for the old legacy versions.

For haproxy 2.x and newer, the poller can use the haproxy json
statistics feed. Note however that some statistics are missing
description fields. This will result in the displayed statistics to
use cryptic names (as provided by haproxy.


Docs are located here:

- [Management Guide 2.5](http://cbonte.github.io/haproxy-dconv/2.5/management.html#9.1) - json poller
- [Management Guide 1.7](http://cbonte.github.io/haproxy-dconv/1.7/management.html#9.1) - csv poller  
- [Management Guide 1.5](http://cbonte.github.io/haproxy-dconv/1.5/snapshot/configuration.html#9.1) - csv poller  


Note currently only haproxy 1.5 and 1.7 are supported through the csv poller.   
Though new versions can be added easily through adapting the existing
conf/haproxy_1.7.inc file for example.

Configure the haproxy to provide server statistics.
An example configuration could look like below:
```
# Statistics
listen stats
    bind *:2000 ssl crt /etc/ssl/haproxy.pem
    mode http
    stats enable
    stats hide-version
    stats realm HAproxy-Statistics
    stats uri /haproxy_stats
    stats auth admin:XXXXXX
```
Note that haproxy preovides basic authentication which sends a header
with base64 encoded login/password pair as a header variable. So in
case you use a password, make sure to enable https (even self signed
key will do).


### Configuration variables

The following variables need to be defined:

- hapver: default "1.7" - defines which haproxy version the system
will poll. This defines the metrics to collect. It is required for the
csv poller!  Only required for haproxy versions < 2.

- hapurl: Format "https://haproxy-url.domain.tld/haproxy_stats;csv" -
No default.   _Note the ";csv or ;json" at the end is required. Also,
do **not** add the port for the stats-UI into the URL!_

- hapuser: default "admin" - The user that is used to authenticate to
  access the statistics

- happass: No default. The associated password to hapuser!

- happort: default "2000" - the port to access the haproxy statistics.

- self_signed: default "false" - If the certificate to access the
  haproxy interface is self signed, set this variable to "true".

- send_all: default "false" - By default the collector will only send
  metrics that have values associated. If the value fields are empty,
  the metric will be skipped.

- apmia_url: No default - "http://apmia-url.domain.tld/apm/metricFeed"
  the url used to access the APMIA restful collector.

- apmia_port: default 8080 - The port the restful collector is
  listening.

- poll_time: 7 - The interval between sending the stats. Note that the
  counter will start once all the data was sent so take into account
  long poll-times on larg hapreoxy deployments!

- hapdebug: default "false" - Set to true for activating debugging

- hapstats: default "false" - Set to true for activating stats in
  console

## Running the CLI Version only

The CLI version can be run from any location.
Just edit the run.sh script, provide the correct values, and execute it.
Below an example using the json poller.
```
php_haproxy_collector$ ./run-dev.sh

*** HAProxy stats to APMIA RESTFul collector started
 => The following configuration is set (conf/hacollector.conf)
 =============================================================
 => HA Proxy version: Ignored (json poller used) 
    - haproxy URL: https://pcm.solsys.org/haproxy_stats;json 
    - haproxy port: 2000 
    - haproxy access via user: admin 
 => APMIA configuration
    - APMIA target URL: http://calypso.solsys.org/apm/metricFeed 
    - APMIA Port: 8080 
    - Poll time: 7 
 => Executing! 
    - Debug flag:  
    - Statistics flag: 1 
 => Lockfile hacollector.php.49d0101a..LCK created.
 > Polled: 2022-04-08 08:45:25 GMT FRONTEND|http-in Agent answer: {"validCount":57} 
 > Polled: 2022-04-08 08:45:25 GMT FRONTEND|pcm_frontend Agent answer: {"validCount":57} 
 > Polled: 2022-04-08 08:45:25 GMT pcm|pcm_backend Agent answer: {"validCount":66} 
 > Polled: 2022-04-08 08:45:25 GMT BACKEND|pcm_backend Agent answer: {"validCount":75} 
 > Polled: 2022-04-08 08:45:25 GMT FRONTEND|phpmyadmin_frontend Agent answer: {"validCount":57} 
 > Polled: 2022-04-08 08:45:25 GMT phpmyadmin|phpmyadmin_backend Agent answer: {"validCount":66} 
 > Polled: 2022-04-08 08:45:25 GMT BACKEND|phpmyadmin_backend Agent answer: {"validCount":75} 
 > Polled: 2022-04-08 08:45:25 GMT FRONTEND|stats Agent answer: {"validCount":57} 
 > Polled: 2022-04-08 08:45:25 GMT BACKEND|stats Agent answer: {"validCount":74}
```

## Running the docker-version

The docker version can be run as simple as the console version.

The provided image is based on alpine to keep the storage footprint
small. Also, for security reasons, the entire process runs as user
hapcoll that is created during image build.

Fill the variables accordingly in the provided docker-compose.yml
template and start the container with:
```
jmertin@calypso:~/docker/php_haproxy_collector$ docker-compose up -d
Creating network "php_haproxy_collector_default" with the default driver
Creating haproxy_collector ... done
jmertin@calypso:~/docker/php_haproxy_collector$ docker-compose logs
Attaching to haproxy_collector
haproxy_collector | 
haproxy_collector | *** HAProxy stats to APMIA RESTFul collector started
haproxy_collector |  => The following configuration is set (conf/hacollector.conf)
haproxy_collector |  =============================================================
haproxy_collector |  => HA Proxy version: 1.7 
haproxy_collector |     - haproxy URL: https://haproxy-server.domain.tld/haproxy_stats;csv 
haproxy_collector |     - haproxy port: 2000 
haproxy_collector |     - haproxy access via user: admin 
haproxy_collector |  => APMIA configuration
haproxy_collector |     - APMIA target URL: http://apmia-server.domain.tld/apm/metricFeed 
haproxy_collector |     - APMIA Port: 8080 
haproxy_collector |     - Poll time: 7 
haproxy_collector |  => Executing! 
haproxy_collector |     - Debug flag:  
haproxy_collector |     - Statistics flag: 1 
haproxy_collector |  => Lockfile /opt/hapcoll/hacollector.php.39a80fac..LCK created.
 > Polled: 2022-03-29 09:13:17 GMT http-in|FRONTEND Agent answer: {"validCount":37} 
 > Polled: 2022-03-29 09:13:17 GMT pcm_frontend|FRONTEND Agent answer: {"validCount":37} 
 > Polled: 2022-03-29 09:13:17 GMT pcm_backend|pcm Agent answer: {"validCount":48} 
 > Polled: 2022-03-29 09:13:17 GMT pcm_backend|BACKEND Agent answer: {"validCount":47} 
 > Polled: 2022-03-29 09:13:17 GMT phpmyadmin_frontend|FRONTEND Agent answer: {"validCount":37} 
 > Polled: 2022-03-29 09:13:17 GMT phpmyadmin_backend|phpmyadmin Agent answer: {"validCount":48} 
 > Polled: 2022-03-29 09:13:17 GMT phpmyadmin_backend|BACKEND Agent answer: {"validCount":47} 
 > Polled: 2022-03-29 09:13:17 GMT stats|FRONTEND Agent answer: {"validCount":37} 
 > Polled: 2022-03-29 09:13:17 GMT stats|BACKEND Agent answer: {"validCount":46} 
```

### Building the docker image

This git-package provides a complete build-set so you can create your
own docker-image. Simply execute the ./build_image.sh script.

If the "DOCKER_REGISTRY" variable in 00_infosource.cfg is holding a
valid docker repository, it will also automatically push the created
image to the docker repository. Make sure you adapt the docker-compose
script so that it polls the image from that repository!
```
php_haproxy_collector$ ./build_image.sh 

>>> Build php_haproxy-collector image [y/n]?: y

*** If you want to apply OS Update, don't use the cache.
>>> Use cache for build [y/n]?: y
Sending build context to Docker daemon  1.366MB
Step 1/13 : FROM php:7.4-fpm-alpine
 ---> db81efe17f88
Step 2/13 : MAINTAINER Joerg Mertin <joerg.mertin@broadcom.com>
 ---> Using cache
 ---> 9f32be12b556
Step 3/13 : RUN adduser -h /opt/hapcoll -s /sbin/nologin -g "HAProxy collector" -D hapcoll
 ---> Using cache
 ---> 2c712dda907b
Step 4/13 : RUN apk upgrade --no-cache  && docker-php-ext-install pcntl && rm -rf /usr/src/* && rm -rf /var/cache/apk/*
 ---> Using cache
 ---> 7439384c05e8
Step 5/13 : RUN mkdir /opt/hapcoll/conf /opt/hapcoll/include
 ---> Using cache
 ---> 5b979491ba79
Step 6/13 : COPY conf/* /opt/hapcoll/conf/
 ---> Using cache
 ---> 4071f8bb25fd
Step 7/13 : COPY include/* /opt/hapcoll/include/
 ---> Using cache
 ---> b42f0352e07f
Step 8/13 : COPY hacollector.php /opt/hapcoll/hacollector.php
 ---> Using cache
 ---> 94f1a937dac8
Step 9/13 : COPY run.sh /opt/hapcoll/run.sh
 ---> Using cache
 ---> 9b2238455483
Step 10/13 : RUN chown hapcoll.hapcoll -R /opt/hapcoll
 ---> Using cache
 ---> 8dc4783cd7ff
Step 11/13 : USER hapcoll
 ---> Using cache
 ---> f9267ef4f6e6
Step 12/13 : WORKDIR /opt/hapcoll
 ---> Using cache
 ---> 0988ce34c8ff
Step 13/13 : CMD [ "php", "/opt/hapcoll/hacollector.php" ]
 ---> Using cache
 ---> d5e4d11c6613
Successfully built d5e4d11c6613
Successfully tagged bcp/php_haproxy-collector:latest
*** Tagging image to bcp/php_haproxy-collector:1.0b6
php_haproxy_collector$ docker images | grep bcp/php_haproxy-collector
bcp/php_haproxy-collector                                     1.0b6                                   d5e4d11c6613   2 minutes ago   68MB
bcp/php_haproxy-collector                                     latest                                  d5e4d11c6613   2 minutes ago   68MB
```

### Fine tuning the json poller

The json-poller fine tuning can be done in the file _conf/haproxy_json.conf_    
Only 2 arrays are of importance.   

#### apmia_send:

This array is used to tell the poller which metrics
  names not to send to APMIA. pxname and svname as provided by haproxy
  will be integrated into the metric path, hence not sent.
  They can take either "true" or "false" as value. By default, or if
  an entry does not exist, it assumes the "true" value.

#### apmia_descr:

By default, haproxy sends short cryptic names as
  identifiers. For example: **qcur => current queued requests**.
  To have just qcur translated, assign the array value:   
```
"qcur" => ":Current queued requests"
```   
Note that in case you want to group several entries under the same
metric group, add the group name to the description:   
```
"qcur" => "|Queue:Current queued requests"
```   
In this example, the data will show up under:   
```
SuperDomain|APMIAHostname|Infrastructure|Agent|haproxy|appname|appname_backend|Process|Queue:Current queued requests
```

### Fine tuning the csv poller, and adding new HAProxy releases

The fine tuning part is done through 2 array definitions.

#### apmia_send:

This array is used to tell the poller which metrics
  names not to send to APMIA. pxname and svname as provided by haproxy
  will be integrated into the metric path, hence not sent.
  They can take either "true" or "false" as value. By default, or if
  an entry does not exist, it assumes the "true" value.

#### apmia_descr:

By default, haproxy sends short cryptic names as
  identifiers. For example: **qcur => current queued requests**.
  To have just qcur translated, assign the array value:   
```
"qcur" => ":Current queued requests"
```   
Note that in case you want to group several entries under the same
metric group, add the group name to the description:   
```
"qcur" => "|Queue:Current queued requests"
```   
In this example, the data will show up under:   
```
SuperDomain|APMIAHostname|Infrastructure|Agent|haproxy|appname|appname_backend|Process|Queue:Current queued requests
```


#### Adding a new haproxy version

Adding a new HAPRoxy version to be supported through the csv poller, is straight forward.

1. Find the CSV file definition. Best is to look up the "Management Guide [Version]" - and find the CSV section.   
For example for HAPRoxy 1.7 it would be [Management Guide version 1.7.14](http://cbonte.github.io/haproxy-dconv/1.7/management.html#9.1).
2. Copy one HAPRoxy definition file **conf/haproxy_1.5.inc** to the wanted version - in this case ->  HAPRoxy 1.7 it would be **conf/haproxy_1.7.inc**
3. There are 3 arrays that need to be adapted: apmia_send, apmia_des and apmia_type
In these 3 arrays, the missing field names and associated values need to be completed compared to the values found in the online Management Guide.   
   
You may want to make a diff between haproxy_1.5.inc and haproxy_1.7.inc to see what needs to be done.


# Manual Changelog
```
Fri, 25 Mar 2022 17:07:11
	- Initial release
```
