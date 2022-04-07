## Introduction

this package provides the possibility to import haproxy statistics
into the APMIA.  The script can be used on a per haproxy server.
The idea to write the HAProxy collector was to reduce the resource
footprint so it can be ran on any system.   
Note that the memory requirement will depend on the HAProxy server
provided statistics - as these will be worked out in memory itself.
For example, polling a small HAProxy servicing 2 sites will require
around 14MBytes of real memory (measured using smem/PSS value).




### Requirements

- The restfull interface of the APMIA needs to be enabled
- HAProxy needs to provide the statistics
- PHP 7.4 needs to have posix, pcntl, curl installed (extensions)

## APMIA configuration

The detailed documentation for the APMIA can be found under 
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

### Configuration variables

The following variables need to be defined:

- hapver: default "1.7" - defines which haproxy version the system will poll. This defines the metrics to collect.
Only required for haproxy versions < 2.

- hapurl: Format "https://haproxy-url.domain.tld/haproxy_stats;csv" - No default.   
Note the ";csv or ;json" at the end is required. Also, do **not** add the port for the stats-UI into the URL!

- hapuser: default "admin" - The user that is used to authenticate to access the statistics

- happass: No default. The associated password to hapuser!

- happort: default "2000" - the port to access the haproxy statistics.

- self_signed: default "false" - If the certificate to access the haproxy interface is self signed, set this variable to "true".

- send_all: default "false" - By default the collector will only send metrics that have values associated. If the value fields are empty, the metric will be skipped.

- apmia_url: No default - "http://apmia-url.domain.tld/apm/metricFeed" the url used to access the APMIA restful collector.

- apmia_port: default 8080 - The port the restful collector is listening.

- poll_time: 7 - The interval between sending the stats. Note that the counter will start once all the data was sent so take into account long poll-times on larg hapreoxy deployments!

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
    - haproxy URL: https://haproxy-server.domain.tld/haproxy_stats;csv 
    - haproxy port: 2000 
    - haproxy access via user: admin 
 => APMIA configuration
    - APMIA target URL: http://apmia-server.domain.tld/apm/metricFeed 
    - APMIA Port: 8080 
    - Poll time: 7 
 => Executing! 
    - Debug flag:  
    - Statistics flag:  
 => Lockfile hacollector.php.39a80fac..LCK created.
```

## Running the docker-version

The docker version can be run as simple as the console version.

The provided image is based on alpine to keep the storage footprint
small. Also, for security reasons, the entire process runs as user
hapcoll that is created during image build.

Fill the variables accordingly in the docker-compose.yml template and
start the container with:
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

If the "DOCKER_REGISTRY" variable is holding a valid docker
repository, it will also automatically push the created image to the
docker repository. Make sure you adapt the docker-compose script so
that it polls the image from that repository!

```
jmertin@calypso:~/docker/php_haproxy_collector$ ./build_image.sh 

>>> Build php_haproxy-collector image [y/n]?: y

*** If you want to apply OS Update, don't use the cache.
>>> Use cache for build [y/n]?: y
Sending build context to Docker daemon  387.1kB
Step 1/12 : FROM php:7.4-fpm-alpine
 ---> db81efe17f88
Step 2/12 : MAINTAINER Joerg Mertin <joerg.mertin@broadcom.com>
 ---> Using cache
 ---> a79dea8a1642
Step 3/12 : RUN adduser -h /opt/hapcoll -s /sbin/nologin -g "HAProxy collector" -D hapcoll
 ---> Using cache
 ---> 416e5b574807
Step 4/12 : RUN apk upgrade --no-cache && docker-php-ext-install pcntl
 ---> Using cache
 ---> 48c0e0cef4aa
Step 5/12 : RUN mkdir /opt/hapcoll/conf
 ---> Using cache
 ---> 4393d9fa494c
Step 6/12 : COPY conf/* /opt/hapcoll/conf/
 ---> b24706db2c60
Step 7/12 : COPY hacollector.php /opt/hapcoll/hacollector.php
 ---> 697b40c70785
Step 8/12 : COPY run.sh /opt/hapcoll/run.sh
 ---> 1502dcead9c4
Step 9/12 : RUN chown hapcoll.hapcoll -R /opt/hapcoll
 ---> Running in 927ae2bbd4bf
Removing intermediate container 927ae2bbd4bf
 ---> 16ae9c6aa0c4
Step 10/12 : USER hapcoll
 ---> Running in a16876402280
Removing intermediate container a16876402280
 ---> ff074cff460f
Step 11/12 : WORKDIR /opt/hapcoll
 ---> Running in 1d3ecad5c031
Removing intermediate container 1d3ecad5c031
 ---> 024f7c8ebeea
Step 12/12 : CMD [ "php", "/opt/hapcoll/hacollector.php" ]
 ---> Running in f2fc80deac8a
Removing intermediate container f2fc80deac8a
 ---> 3a6e28fbb0f7
Successfully built 3a6e28fbb0f7
Successfully tagged bcp/php_haproxy-collector:latest
*** Tagging image to bcp/php_haproxy-collector:1.0b5
jmertin@calypso:~/docker/php_haproxy_collector$ docker images | grep "bcp/php_haproxy-collector"
bcp/php_haproxy-collector                                     1.0b5                                   3a6e28fbb0f7   17 seconds ago   67MB
bcp/php_haproxy-collector                                     latest                                  3a6e28fbb0f7   17 seconds ago   67MB
```