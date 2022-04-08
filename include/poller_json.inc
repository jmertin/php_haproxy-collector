<?php

// Actual function, doing the HAProxy polling when downloadin json data
function poller_json() {
    global $haproxy_type, $haproxy_nature, $haproxy_port, $DEBUG, $username, $password, $pollurl, $self_signed, $apmia_url, $apmia_port, $poll_time, $apmia_send, $apmia_des, $apmia_type, $send_all, $STATS, $filename, $apmia_desc;

    $jsoncont = get_stats();
    
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
