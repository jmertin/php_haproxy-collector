<?php

// Misc functions stored here.

// The URL is always an issue. Make sure it is currect.
function check_url($text, $return_var, $description){

    // define the answer array and empty it
    $answer = array();
    $answer['error_msg'] = '';

    // URL needs to be valid, not 0
    if (strlen($text) > 0) {
        if (preg_match("/^http/i",$text)) {
            if (filter_var($text, FILTER_VALIDATE_URL)  === FALSE) {
                $answer["$return_var"] = htmlentities($text, ENT_QUOTES, 'UTF-8');
                $answer['error_msg'] = "*** FATAL: URL format for $description \"" . $answer["$return_var"] . "\" invalid!";
            } else {
                $answer["$return_var"] = htmlentities($text, ENT_QUOTES, 'UTF-8');
            }
        }
    } else {
        // If Data was empty - it won't hurt here either
        $answer["$return_var"] = "";
        $answer['error_msg'] = "*** FATAL: Valid URL for $description required!";
    }

    // Return what we found out.
    return($answer);

} // function check_url


// signal handler function
function sig_handler($signo) {
    global $filename;

    switch ($signo) {
    case SIGTERM:
        print "Received signal $signo (SIGTERM) - cleaning up!\n";
        // handle shutdown tasks
        if (file_exists($filename)) {
            unlink($filename);
        }
        exit;
        break;
    case SIGINT:
        print "Received signal $signo (SIGINT) - cleaning up!\n";
        // handle restart tasks
        if (file_exists($filename)) {
            unlink($filename);
        }
        exit;
        break;
    case SIGUSR1:
        print "Received signal $signo (SIGUSR1)- Rereading configuration!\n";
        // Load configuration
        include('./conf/hacollector.conf'); // will load the config.inc from the same directory.
        // Load the haproxy version file - configuration for what to poll and type definitions.
        include("./conf/haproxy_" . $haproxy_version . ".inc"); // will load haproxy file
        // handle restart tasks
    default:
        // handle all other signals
        print "Received signal $signo - don't know what to do!";
    }

} // function sig_handler

// Checking string end.
function endsWith( $haystack, $needle ) {
    $length = strlen( $needle );
    if( !$length ) {
        return true;
    }
    return substr( $haystack, -$length ) === $needle;
}

// Function polls the haproxy URL
function get_stats() {
    global $filename, $DEBUG, $username, $password, $pollurl, $haproxy_port, $self_signed;
    
    // Checking the provided URL for HAPRoxy
    $url_check = check_url($pollurl, "clean", "HAProxy");
    if (strlen($url_check['error_msg']) > 1) {
        // we have an issue here. Error message should not exist.
        print "{$url_check['error_msg']} - ";
        unlink($filename);
        posix_kill(posix_getpid(), SIGTERM);
    }

    // Check port for haproxy
    if (($haproxy_port < 65536) && ($haproxy_port > 0)) {
        $port['check'] = "valid";
    } else {
        print "*** FATAL: Port $haproxy_port for APMIA invalid. - ";
        unlink($filename);
        posix_kill(posix_getpid(), SIGTERM);
    }
    
    // Init ch
    $ch = curl_init($pollurl);
    
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
    $jsoncont = curl_exec($ch);

    //Check for errors.
    if (curl_errno($ch)) {
        // If an error occured, throw an Exception.
        throw new Exception(curl_error($ch));
    } else {
        return($jsoncont);
    }

    // Close data collection from haproxy.
    curl_close($ch);
} // End function get_stats


// Function sends the metrics to the APMIA RESTful collector.
function send_metrics($tosend, $workon) {
    global $apmia_port, $apmia_url, $DEBUG, $STATS;

    // Checking the provided for APMIA
    $url_check = check_url($apmia_url, "clean", "APMIA");
    if (strlen($url_check['error_msg']) > 1) {
        // we have an issue here. Error message should not exist.
        print "{$url_check['error_msg']} - ";
        unlink($filename);
        posix_kill(posix_getpid(), SIGTERM);
    }

    // Check port for apmia
    if (($apmia_port < 65536) && ($apmia_port > 0)) {
        $port['check'] = "valid";
    } else {
        print "*** FATAL: Port $apmia_port for APMIA invalid. - ";
        unlink($filename);
        posix_kill(posix_getpid(), SIGTERM);
    }

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
    $STATS && print " > Polled: " . gmstrftime("%Y-%m-%d %T %Z") . " {$workon} Agent answer: $result \n";
    // print "\n\n $tosend\n\n"; // Payload in case
    curl_close($xch);

} // function send_metrics

?>
