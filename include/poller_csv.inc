<?php

// Actual function, doing the HAProxy polling etc.
function poller_csv() {
    global $haproxy_version, $haproxy_port, $DEBUG, $username, $password, $pollurl, $self_signed, $apmia_url, $apmia_port, $poll_time, $apmia_send, $apmia_des, $apmia_type, $send_all, $STATS, $filename;

    // Poll haproxy for stats
    $csvcont = get_stats();

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
                if ($value == "") {
                    $value = 0;
                }
                $DEBUG && print " >> INFO: Metric value manually set to 0 \n";
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
