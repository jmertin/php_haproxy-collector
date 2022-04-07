<?php

/* ==============================================================================================
   Nothing to be modified here. Please edit the configuration file located in the conf directory!
   ==============================================================================================
*/

// Load misc functions.
include_once('./include/misc.inc');

// Load configuration
include_once('./conf/hacollector.conf'); // will load the config.inc from the same directory.


// Implemnent file locking. We don't want this script to run more than once.
// Note - the Lockfile has a name composed of the script-name and the haproxy
// csv URL as we assume we'll be polling this HA Proxy server only once.
$filename = preg_replace('/\.\//', '', $_SERVER['PHP_SELF']) . "." . hash('adler32', "$pollurl") . "..LCK";
$fp = fopen( $filename,"w"); // open it for WRITING ("w")
if (flock($fp, LOCK_EX)) {
    file_put_contents($filename, posix_getppid());
    // do your file writes here
    flock($fp, LOCK_UN); // unlock the file
} else {
    // flock() returned false, no lock obtained
    print "Could not lock $filename!\n";
}

// Showing current config
print "\n";
print "*** HAProxy stats to APMIA RESTFul collector started\n";
print " => The following configuration is set (conf/hacollector.conf)\n";
print " =============================================================\n";

print " => HA Proxy version: $haproxy_version \n";
print "    - haproxy URL: $pollurl \n";
print "    - haproxy port: $haproxy_port \n";
if (strlen($username) > 0) {
    print "    - haproxy access via user: $username \n";
} else {
    print "    - haproxy csv stats access via anonymous user \n";
}
print " => APMIA configuration\n";
print "    - APMIA target URL: $apmia_url \n";
print "    - APMIA Port: $apmia_port \n";
print "    - Poll time: $poll_time \n";
print " => Executing! \n";
print "    - Debug flag: $DEBUG \n";
print "    - Statistics flag: $STATS \n";
print " => Lockfile $filename created.\n";

// tick use required
declare(ticks = 1);

// Actual code runs here :)
while (true) {
    // Actually run the code
    if (endsWith("$pollurl", 'csv')) { 
        // Load the haproxy version file - configuration for the version is in hacollector.conf
        include_once("./conf/haproxy_" . $haproxy_version . ".inc"); // will load haproxy file

        // Load csv poller
        include_once('./include/poller_csv.inc'); // will load the config.inc from the same directory.

        // CSV poller requires static configuration
        poller_csv();

    } elseif (endsWith("$pollurl", 'json')) {
        // Load json poller
        include_once('./conf/haproxy_json.conf'); 

        // Load json poller
        include_once('./include/poller_json.inc'); 

        // JSon poller does not require static configuration - as it
        // has the type definitions and field names included in json
        // string.
        poller_json();
        
    } else {
        print "*** Wrong target URL format! \n";
        unlink($filename);
        posix_kill(posix_getpid(), SIGTERM);
    }
    // setup signal handlers - or else we cannot interrupt.
    pcntl_signal(SIGTERM, "sig_handler");
    pcntl_signal(SIGINT,  "sig_handler");
    pcntl_signal(SIGUSR1,  "sig_handler");
    // Pause for the requested amount of time
    sleep($poll_time);
} // while loop main function



?>
