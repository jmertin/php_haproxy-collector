<?php

// Actual function, doing the HAProxy polling when downloadin json data
function poller_json() {
    global $haproxy_type, $haproxy_nature, $haproxy_port, $DEBUG, $username, $password, $pollurl, $self_signed, $apmia_url, $apmia_port, $poll_time, $apmia_send, $apmia_des, $apmia_type, $send_all, $STATS, $filename, $apmia_desc;

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
    $jsoncont = curl_exec($ch);

    //Check for errors.
    if (curl_errno($ch)) {
        // If an error occured, throw an Exception.
        throw new Exception(curl_error($ch));
    }

    // Close data collection from haproxy.
    curl_close($ch);

    // We assume is is json!
    $json_array = json_decode($jsoncont, true);
    //$DEBUG && print "\n => Associative array : \n" . print_r($json_array);

    $assoc_array = array();
    
    $newline = "";
    $svname = "";
    foreach ($json_array as $entryid => $values) {
        $line = "";
        $coma = "";
        $DEBUG && print "=============================================================\n";
       
        // Loop through the rest.
        foreach ($values as $dataid => $entries) {
            $send = true;
            
            // Extract the svname and pxname to compute the metric path "prefix"
            switch ($entries['field']['name']) {
            case "svname":
                $svname = $entries['value']['value'];
                break;
            case "pxname":
                $pxname = $entries['value']['value'];
                break;
            }

            // Once we have both, we have the path
            if ((strlen($svname) > 0) && (strlen($pxname) > 0)) {
                $metricheader = "{$svname}|{$pxname}";
                $DEBUG && print " => processing: $metricheader \n";
            }
            
            $DEBUG && print " => Data ID: $dataid \n";           
            
            // Define if we want to send the data
            $type = $entries['value']['type'];
            $nature = $entries['tags']['nature'];
            $entryname = $entries['field']['name'];
            
            if (isset($apmia_send["$entryname"]) && ($apmia_send["$entryname"] == "false")) {
                $send = false;
                $DEBUG && print " ** Skipping line: \"$entryname\" as requested by configuration \n";
            }

            // We can define the "description" of the entry name in
            // the haproxy_json.conf file.
            // If a definition is there, use it. Else - make it usable.
            if (isset($apmia_desc["$entryname"])) {
                $descentryname = $apmia_desc["$entryname"];
            } else {
                $descentryname = ":" . $entryname;
            }
            $DEBUG && print " >> Entry description: $descentryname \n";

            $metricscope = $entries['tags']['scope'];
            $DEBUG && print " >> Metric scope: $metricscope \n";

            // Compute data type.
            switch ($haproxy_nature["$nature"]) {
            case 'StringEvent':
                $metrictype = "StringEvent";
                break;
            case 'String':
                $metrictype = "StringEvent";
                break;
            default:
                if ($haproxy_type["$type"] == "String") {
                    $metrictype = "StringEvent";
                } else {
                    $metrictype = $haproxy_type["$type"] . $haproxy_nature["$nature"];
                }
                break;
            }
            $DEBUG && print " >> Metric type: $metrictype \n";

            $metricvalue = $entries['value']['value'];
            $DEBUG && print " >> Metric value: $metricvalue \n";
            
            // Sanitize data somehow
            if ($send_all) {
                if ($metricvalue == "") {$metricvalue = 0; }
                $DEBUG && print " >> INFO: Metric value manually set to 0 \n";
            }

            if (!(strlen($metricvalue) > 0)) {
                $send = false;
                $DEBUG && print " >> INFO: Entry skipped due to non existing data \n";
            }
            
            if ($send) {
                $newline = "{\"type\" : \"" . $metrictype . "\", " . 
                         "\"name\" : \"haproxy|{$metricheader}|{$metricscope}{$descentryname}\", " . 
                         "\"value\" : \"$metricvalue\"}";
                $line .= "{$coma}{$newline}";
                $coma = ",";
                
            } // If send
            
        } // values foreach loop
        
        // This is the entire line that will be sent as json payload to the APMIA.
        $tosend = "{ \"metrics\" : [" . $line . "] }";
        $DEBUG && print " => $tosend \n";
        $coma = "";

        if (!$DEBUG) {
            send_metrics($tosend, $metricheader);
        }
        
    } // entries foreach loop

    
} // poller_json function

?>
