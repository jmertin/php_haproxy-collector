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
