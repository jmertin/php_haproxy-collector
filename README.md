# php_haproxy-collector 1.0-6 by J. Mertin -- joerg.mertin@broadcom.com
Purpose: Collects haproxy csv stats and sends them to restfull enabled APMIA 

# Description
## Introduction

this package gives the possibility to import haproxy statistics into the APMIA. It will just be an interface between the provided statistics by HAPRoxy, and what the APMIA expects on its RESTful API interface.

![Setup](Architecture_communications_diagram.png)

The idea to write the HAProxy collector was to reduce the resource footprint so it can be run on any system (even the machine running the haproxy itself).

Note that the memory requirement will depend on the HAProxy server provided statistics - as these will be worked out in memory itself.   
For example, polling a small HAProxy servicing 2 sites will require around 14MBytes of real memory measured using smem/PSS value using the csv poller. The json poller will use 16MBytes of ram as the json payload is way larger than the csv one.

### Requirements

- The RESTful interface of the APMIA needs to be enabled
- HAProxy needs to provide the statistics
- PHP 7.2+ needs to have posix, pcntl, curl installed (extensions)

### Limitations

- The script can be used on a per haproxy server. The lockfile is based on the URL to be polled. This means that if you want to process the data from more then one HAProxy server, you have to start 2 or more instances of the php poller script.

- The php_haproxy_collector can collect statistics data from the haproxy in CSV or JSON format.  The main differences between the CSV and the JSON poller is that the json-poller provides by default all field names, while the csv-poller requires the configuration to be "adapted" for each new version.


## Configuring the APMIA Agent

The detailed documentation for the APMIA Restful API and usage can be found under 
[Enable the EPAgent Plug-ins RESTful Interface](https://techdocs.broadcom.com/us/en/ca-enterprise-software/it-operations-management/application-performance-management/10-7/implementing-agents/infrastructure-agent/epagent-plug-ins/configure-epagent-network-ports/enable-the-epagent-plug-ins-restful-interface.html)

#### Enable the REST interface on your Infrastructure Agent

- Navigate to the [Agent_Home]/core/config directory and open the **IntroscopeAgent.profile** in a text editor.   
Look for:
```
introscope.epagent.config.httpServerPort=8080
```
and set the port to whatever port is available. The haproxy collector will send the received metrics to this port.


### Configuring HAProxy

the current version of haproxy_collector supports collecting statistics in csv format for the old legacy versions (1.x).

For haproxy 2.x and newer, the poller can use the json statistics feed. Note however that some statistics are missing description fields. This will result in the displayed statistics to use cryptic names (as provided by haproxy). You can fix this by completing the description array (See section **Fine-tuning the json poller**)


HAproxy Docs are located here:

- [Management Guide 2.5](http://cbonte.github.io/haproxy-dconv/2.5/management.html#9.1) - json poller
- [Management Guide 1.7](http://cbonte.github.io/haproxy-dconv/1.7/management.html#9.1) - csv poller  
- [Management Guide 1.8](https://cbonte.github.io/haproxy-dconv/1.8/management.html#9.1) - csv poller
- [Management Guide 1.5](http://cbonte.github.io/haproxy-dconv/1.5/snapshot/configuration.html#9.1) - csv poller  

Note currently only haproxy 1.5, 1.7  and 1.8 are supported through the csv poller.   
Though new versions can be added easily through adapting the existing
**conf/haproxy_1.7.inc** file for example. See the **Adding a new haproxy version** section.


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
Note that haproxy provides basic authentication which sends a header with a base64 encoded login/password pair as a header variable.    
In case you use a password, for security make sure to enable https (even a self signed key will do).


### Configuration variables

The following variables need to be defined in the launcher script or docker-compose.yml file.

- **hapver**: default "1.7" - defines which haproxy version the system will poll. This defines the metrics to collect. It is required for the csv poller!  Only required for haproxy versions < 2.
- **hapurl**: Format "https://haproxy-url.domain.tld/haproxy_stats;csv" - No default.   _Note the ";csv or ;json" at the end is required. Also, do **not** add the port for the stats-UI into the URL!_
- **hapuser**: default "admin" - The user that is used to authenticate to access the statistics
- **happass**: No default. The associated password to hapuser!
- **happort**: default "2000" - the port to access the haproxy statistics.
- **self_signed**: default "false" - If the certificate to access the haproxy interface is self signed, set this variable to "true".
- **send_all**: default "false" - By default the collector will only send metrics that have values associated. If the value fields are empty,  the metric will be skipped.
- **apmia_url**: No default - "http://apmia-url.domain.tld/apm/metricFeed" the url used to access the APMIA restful collector.
- **apmia_port**: default 8080 - The port the restful collector is listening.
- **poll_time**: 7 - The interval between sending the stats. Note that the counter will start once all the data was sent so take into account long poll-times on larg hapreoxy deployments!
- **hapdebug**: default "false" - Set to true for activating debugging
- **hapstats**: default "false" - Set to true for activating stats in
  console

The **run.sh** file would look like this:
```
#!/bin/bash
#
# These entries can be used on docker-variables!
export hapver=1.7
export hapuser="admin"
export happass="xxxxxxxx"
export hapurl="https://haproxy-url.domain.tld/haproxy_stats;csv"
export happort=2000
export self_signed=true
export send_all=

export hapdebug= # Set to true for activating debugging
export hapstats= # Set to true for activating stats in console

export apmia_url="http://apmia-url.domain.tld/apm/metricFeed"
export apmia_port=8080
export poll_time=7


# Run actual script.
php ./hacollector.php
```

## Running the CLI Version only

The CLI version can be run from any location given PHP-CLI 7.2+ is installed. Just edit the run.sh script, provide the correct values, and execute it.   
Below an example using the json poller.   
```
php_haproxy_collector$ ./run.sh

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

The provided image is based on alpine to keep the storage footprint small. Also, for security reasons, the entire process runs as user **hapcoll** that is created during image build.

Fill the variables accordingly in the provided docker-compose.yml template and start the container with:   
```
php_haproxy_collector$ docker-compose up -d
Creating network "php_haproxy_collector_default" with the default driver
Creating haproxy_collector ... done
php_haproxy_collector$ docker-compose logs
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

This git-package provides a complete build-set so you can create your own docker-image. Simply execute the **./build_image.sh** script.

If the "DOCKER_REGISTRY" variable in **00_infosource.cfg** is holding a valid docker repository, it will also automatically push the created image to the docker repository. Make sure you adapt the docker-compose script so that it polls the image from that repository!   
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

The json-poller fine tuning can be done in the file **conf/haproxy_json.conf**    
Only 2 arrays are of importance.   

#### apmia_send:

This array is used to tell the poller which metrics names to send or not to send.   

They can take either "**true**" or "**false**" as value. By default, or if an entry does not exist, it assumes the "**true**" value.   
Note that the metrics **pxname** and **svname** as provided by haproxy will be integrated into the metric path, hence not sent!

#### apmia_descr:

By default, haproxy sends short cryptic names as identifiers. For example: **qcur**.
To have  **qcur** translated into something more meaningful, assign the array key a decriptive value:   

```
"qcur" => ":Current queued requests"
```
Note that in case you want to group several entries under the same metric group, add the group name to the description:   
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

This array is used to tell the poller which metrics names not to send to APMIA. **pxname** and **svname** as provided by haproxy will be integrated into the metric path, hence not sent.   
They can take either "**true**" or "**false**" as value. By default, or if an entry does not exist, it assumes the "**true**" value.

#### apmia_descr:

By default, haproxy sends short cryptic names as identifiers. For example: **qcur**.
To have  **qcur** translated into something more meaningful, assign the array key a decriptive value:   

```
"qcur" => ":Current queued requests"
```
Note that in case you want to group several entries under the same metric group, add the group name to the description:   
```
"qcur" => "|Queue:Current queued requests"
```
In this example, the data will show up under:   
```
SuperDomain|APMIAHostname|Infrastructure|Agent|haproxy|appname|appname_backend|Process|Queue:Current queued requests
```


#### Adding a new haproxy version

Adding a new HAPRoxy version to be supported through the csv poller is straight forward.

1. Find the CSV file definition. Best is to look up the "**Management Guide [Version]**" - and find the CSV section.   
For example for HAPRoxy 1.7 it would be [Management Guide version 1.7.14](http://cbonte.github.io/haproxy-dconv/1.7/management.html#9.1).
2. Copy one HAPRoxy definition file **conf/haproxy_1.5.inc** to the wanted version - in this case ->  HAPRoxy 1.7 it would be **conf/haproxy_1.7.inc**
3. There are 3 arrays that need to be adapted: apmia_send, apmia_des and apmia_type
In these 3 arrays, the missing key field names and associated values need to be completed, compared to the values found in the online Management Guide.   
   

You may want to make a diff between haproxy_1.5.inc and haproxy_1.7.inc to see what needs to be done.


## Support
This document and associated tools are made available from CA Technologies as examples and provided at no charge as a courtesy to the CA APM Community at large. This resource may require modification for use in your environment. However, please note that this resource is not supported by CA Technologies, and inclusion in this site should not be construed to be an endorsement or recommendation by CA Technologies. These utilities are not covered by the CA Technologies software license agreement and there is no explicit or implied warranty from CA Technologies. They can be used and distributed freely amongst the CA APM Community, but not sold. As such, they are unsupported software, provided as is without warranty of any kind, express or implied, including but not limited to warranties of merchantability and fitness for a particular purpose. CA Technologies does not warrant that this resource will meet your requirements or that the operation of the resource will be uninterrupted or error free or that any defects will be corrected. The use of this resource implies that you understand and agree to the terms listed herein.

Although these utilities are unsupported, please let us know if you have any problems or questions by adding a comment to the CA APM Community Site area where the resource is located, so that the Author(s) may attempt to address the issue or question.

Unless explicitly stated otherwise this extension is only supported on the same platforms as the APM core agent. See [APM Compatibility Guide](https://techdocs.broadcom.com/us/product-content/status/compatibility-matrix/application-performance-management-compatibility-guide.html).

### Support URL
Needs to be changed to public repository   
https://github.gwd.broadcom.net/jm645719/php_haproxy-collector



# Manual Changelog
```
Fri, 25 Mar 2022 17:07:11
	- Initial release
```
