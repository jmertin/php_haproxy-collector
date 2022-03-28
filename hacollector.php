<?php

/* ==============================================================================================
   Nothing to be modified here. Please edit the configuration file located in the conf directory!
   ==============================================================================================
*/

// Load configuration
include_once('./conf/hacollector.conf'); // will load the config.inc from the same directory.

// Implemnent file locking. We don't want this script to run more than once.
// Note - the Lockfile has a name composed of the script-name and the haproxy
// csv URL as we assume we'll be polling this HA Proxy server only once.
$filename = preg_replace('/\.\//', '', $_SERVER['PHP_SELF']) . "." . hash('adler32', "$csvurl") . "..LCK";
$fp = fopen( $filename,"w"); // open it for WRITING ("w")
if (flock($fp, LOCK_EX)) {
    file_put_contents($filename, posix_getppid());
    // do your file writes here
    flock($fp, LOCK_UN); // unlock the file
} else {
    // flock() returned false, no lock obtained
    print "Could not lock $filename!\n";
}

// Load the haproxy version file - configuration for what to poll and type definitions.
include_once("./conf/haproxy_" . $haproxy_version . ".inc"); // will load haproxy file

// Showing current config
print "\n";
print "*** HAProxy stats to APMIA RESTFul collector started\n";
print " => The following configuration is set (conf/hacollector.conf)\n";
print " =============================================================\n";

print " => HA Proxy version: $haproxy_version \n";
print "    - haproxy URL: $csvurl \n";
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

// signal handler function
function sig_handler($signo) {
    global $filename;

    switch ($signo) {
    case SIGTERM:
        print "Received $signo - cleaning up!\n";
        // handle shutdown tasks
        if (file_exists($filename)) {
            unlink($filename);
        }
        exit;
        break;
    case SIGINT:
        print "Received $signo - cleaning up!\n";
        // handle restart tasks
        if (file_exists($filename)) {
            unlink($filename);
        }
        exit;
        break;
    default:
        // handle all other signals
        print "Received $signo - don't know what to do!";
    }

} // function sig_handler

// Actual function, doing the HAProxy polling etc.
function poller() {
    global $haproxy_version, $haproxy_port, $DEBUG, $username, $password, $csvurl, $self_signed, $apmia_url, $apmia_port, $poll_time, $apmia_send, $apmia_des, $apmia_type, $send_all, $STATS;
   
    // Init ch
    $ch = curl_init($csvurl);

    // If curl debugging is required, uncomment the following line.
    curl_setopt($ch, CURLOPT_VERBOSE, $DEBUG);

    // In case HAProxy runs on its own port, set the port here.
    curl_setopt($ch, CURLOPT_PORT , $haproxy_port);

    if ($self_signed) {
        // In case SSL key is self signed, activate both the lines below
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);
    }

    if (strlen($username) > 0) {
        // Specify the username and password using the CURLOPT_USERPWD option.
        // This will cause curl to generate a basic authentication header
        curl_setopt($ch, CURLOPT_USERPWD, $username . ":" . $password);
    }
    // Tell cURL to return the output as a string instead of dumping it to the browser
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

    // Execute the request
    $csvcont = curl_exec($ch);

    //Check for errors.
    if (curl_errno($ch)) {
        // If an error occured, throw an Exception.
        throw new Exception(curl_error($ch));
    }

    // Close data collection from haproxy.
    curl_close($ch);

    // Initialize what we need
    $header = NULL; // header array to be used
    $assoc_data = array(); // associative array containing all results

    // the row always has a "# " at the beginning. strip it out.
    $csvcont = preg_replace('/^#\ /', '', $csvcont);

    $DEBUG && print  " 1 => Return CSV Content : \n $csvcont \n\n";

    // it gets tricky here. The web-server returns only a string, we need to separate each line
    $data = str_getcsv($csvcont, "\n");

    $DEBUG && print " 2 => Return Data array : \n" . print_r($data);

    // Cycle through all entries in the previous found data and create an
    // associative array for the data source.
    foreach ($data as &$row) {
        // Dump array content into new array
        $row_arr = str_getcsv($row, ",");

        // If we have no header line, 1st line is header!
        if(!$header) {
            $header = $row_arr;
        } else {
            // All other lines are regular entries
            $assoc_data[] = array_combine($header, $row_arr);
        }
    }

    $DEBUG && print " 3 => Return Associative array : \n" . print_r($assoc_data);

    // Cycle through associative array
    foreach ($assoc_data as &$output) {
        $coma = "";
        $line = "";
        // Extract key/values out of master array
        foreach ($output as $key => $value) {

            // Sanitize data somehow
            if ($send_all) {
                if ($value == "") {$value = 0; }
            }

            if (strlen($value) > 0) {
                if ($apmia_send["{$key}"]) {
                    // Key will be used to extract the values out of the apmia_des array
                    $line .=  $coma . "{\"type\" : \"" .
                          $apmia_des["{$key}"] . "\", \"name\" : \"haproxy|" .
                          $output['pxname'] . "|" .
                          $output['svname'] .
                          $apmia_type["{$key}"] . "\", \"value\" : \"" .
                          $value . "\"}";
                    $coma=",";
                }
            }
        } // Foeach loop.

        // This is the entire line that will be sent as json payload to the APMIA.
        $tosend = "{ \"metrics\" : [" . $line . "] }";

        $xch = curl_init( $apmia_url );
        curl_setopt($xch, CURLOPT_PORT , $apmia_port);
        // prep data to send to APMIA
        curl_setopt( $xch, CURLOPT_POSTFIELDS, $tosend );
        curl_setopt( $xch, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));
        curl_setopt( $xch, CURLOPT_RETURNTRANSFER, true );
        $result = curl_exec($xch);

        //Check for errors.
        if (curl_errno($xch)) {
            // If an error occured, throw an Exception.
            throw new Exception(curl_error($xch));
        }
        $DEBUG && print "$result \n";
        $STATS && print " > Polled: " . gmstrftime("%Y-%m-%d %T %Z") . " {$output['pxname']}|{$output['svname']} Agent answer: $result \n";
        // print "\n\n $tosend\n\n"; // Payload in case
        curl_close($xch);
    }

    //curl_close($xch);
}

// Actual code runs here :)
while (true) {
    // Actually run the code
    poller();
    // setup signal handlers - or else we cannot interrupt.
    pcntl_signal(SIGTERM, "sig_handler");
    pcntl_signal(SIGINT,  "sig_handler");
    // Pause for the requested amount of time
    sleep($poll_time);
} // while loop main function



?>
