#!/bin/bash
#
#
export hapver=1.7
export hapuser="admin"
export happass="PA55w0rd"
export hapurl="https://pcm.solsys.org/haproxy_stats;csv"
export happort=2000
export self_signed=true
export send_all=

export hapdebug= # Set to true for activating debugging

export apmia_url="http://calypso.solsys.org/apm/metricFeed"
export apmia_port=8080
export poll_time=7



php ./hacollector.php
