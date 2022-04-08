<?php

// Actual function, doing the HAProxy polling etc.
function poller_csv() {
    global $haproxy_version, $haproxy_port, $DEBUG, $username, $password, $pollurl, $self_signed, $apmia_url, $apmia_port, $poll_time, $apmia_send, $apmia_des, $apmia_type, $send_all, $STATS, $filename;

    // Checking the provided URL for HAPRoxy
    $url_check = check_url($pollurl, "clean", "HAProxy");
    if (strlen($url_check['error_msg']) > 1) {
        // we have an issue here. Error message should not exist.
        print "{$url_check['error_msg']} - ";
        unlink($filename);
        posix_kill(posix_getpid(), SIGTERM);
    }

    // Checking the provided for APMIA
    $url_check = check_url($apmia_url, "clean", "APMIA");
    if (strlen($url_check['error_msg']) > 1) {
        // we have an issue here. Error message should not exist.
        print "{$url_check['error_msg']} - ";
        unlink($filename);
        posix_kill(posix_getpid(), SIGTERM);
    }

    // Check port for apmia
    if (($haproxy_port < 65536) && ($haproxy_port > 0)) {
            $port['check'] = "valid";
        } else {
            print "*** FATAL: Port $haproxy_port for APMIA invalid. - ";
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
    $csvcont = curl_exec($ch);

    //Check for errors.
    if (curl_errno($ch)) {
        // If an error occured, throw an Exception.
        throw new Exception(curl_error($ch));
    }

    // Close data collection from haproxy.
    curl_close($ch);
    // $DEBUG && print "\n 2 => Returned RAW Data : \n" . print_r($csvcont);


    // Initialize what we need
    $header = NULL; // header array to be used
    $assoc_data = array(); // associative array containing all results

    // the row always has a "# " at the beginning. strip it out.
    $csvcont = preg_replace('/^#\ /', '', $csvcont);

    // $DEBUG && print  " 1 => Return CSV Content : \n $csvcont \n\n";

    // it gets tricky here. The web-server returns only a string, we need to separate each line
    $data = str_getcsv($csvcont, "\n");

    // $DEBUG && print " 2 => Return Data array : \n" . print_r($data);

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

    // $DEBUG && print " 3 => Return Associative array : \n" . print_r($assoc_data);
     
    // #1 Cycle through associative array
    foreach ($assoc_data as &$output) {
        $coma = "";
        $line = "";
        // #2 Extract key/values out of master array
        foreach ($output as $key => $value) {

            // Sanitize data somehow
            if ($send_all) {
                if ($value == "") {$value = 0; }
            }

            if (strlen($value) > 0) {
                if ($apmia_send["{$key}"]) {
                    // Key will be used to extract the values out of the apmia_des array
                    $newline =  $coma . "{\"type\" : \"" .
                          $apmia_des["{$key}"] . "\", \"name\" : \"haproxy|" .
                          $output['pxname'] . "|" .
                          $output['svname'] .
                          $apmia_type["{$key}"] . "\", \"value\" : \"" .
                          $value . "\"}";
                    $DEBUG && print " > Newline => $newline \n";
                    $line .= $newline;
                    $coma=",";
                }
            }
        } // Foeach loop.

        // This is the entire line that will be sent as json payload to the APMIA.
        $tosend = "{ \"metrics\" : [" . $line . "] }";

        //$DEBUG && print " 5 => Returne Data JSon : \n" . $tosend . "\n\n";
        
        if (!$DEBUG) {
            send_metrics($tosend, "{$output['pxname']}|{$output['svname']}");
        }

    } // foreach loop #2
} // poller_csv function

?>
