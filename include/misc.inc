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

function send_metrics($tosend, $workon) {
    global $apmia_port, $apmia_url, $DEBUG, $STATS;
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
