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

// Showing current config
print "\n";
print "*** HAProxy stats to APMIA RESTFul collector started\n";
print " => The following configuration is set (conf/hacollector.conf)\n";
print " -------------------------------------------------------------\n";

print " * HA Proxy version: $haproxy_version \n";
print " * HA Proxy URL: $csvurl \n";
print " * HA Proxy port: $haproxy_port \n";
if (strlen($username) > 0) {
  print " * HA Proxy access via user: $username \n";
} else {
  print " * Anmonymous access to HA Proxy csv stats \n";
}
print " * APMIA target URL: $apmia_url \n";
print " * APMIA Port: $apmia_port \n";
print " * Poll time: $poll_time \n";
print " > Executing - Lockfile $filename created.\n";

// Load the haproxy version file - configuration for what to poll and type definitions.
include_once("./conf/haproxy_" . $haproxy_version . ".inc"); // will load haproxy file


// Show env Variables
if ($DEBUG) {

  print "Current configuration \n";
  print "DEBUG Flag: $DEBUG \n";
  print "HAProxy version: $haproxy_version \n";
  print "HAProxy port: $haproxy_port \n";
  print "User Name: $username \n";
  print "Password : $password \n";
}

function poller() {
  global $haproxy_version, $haproxy_port, $DEBUG, $username, $password, $csvurl, $self_signed, $apmia_url, $apmia_port, $poll_time, $apmia_send, $apmia_des, $apmia_type, $send_all;
  /*first log in*/
  $ch = curl_init($csvurl);

  // If curl debugging is required, uncomment the following line.
  // curl_setopt($ch, CURLOPT_VERBOSE, $DEBUG);

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

  // We need to exit the data according to a specific data format.
  // Example:
  // { metrics : [{type : "PerIntervalCounter", name : "MyTest|RESTFul|PerIntervalCounter|Test1:Count", value : "123"}] }
  // { metrics : [{type : "LongCounter", name : "MyTest|RESTFul|LongCounter|Test2:Count", value : "456"}] }

  //$xch = curl_init( $apmia_url );
  //curl_setopt($xch, CURLOPT_PORT , $apmia_port);

  foreach ($assoc_data as &$output) {
        // Counting elements in $
        // print "Working on: \n";
        // print_r($output);
        $coma = "";
        $line = "";
        foreach ($output as $key => $value) {

          // Sanitize data somehow
          if ($send_all) {
            if ($value == "") {$value = 0; }
          }

          if (strlen($value) > 0) {
            if ($apmia_send["{$key}"]) {
              // Key will be used to extract the values out of the apmia_des array
              $line .=  $coma . "{\"type\" : \"" .
                $apmia_des["{$key}"] . "\", \"name\" : \"" .
                $output['pxname'] . "|" .
                $output['svname'] . ":" .
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
      print " > Polled: " . gmstrftime("%Y-%m-%d %T %Z") . " {$output['pxname']}|{$output['svname']} Agent answer: $result \n";
      // print "\n\n $tosend\n\n"; // Payload in case
      curl_close($xch);
  }

  //curl_close($xch);
}

while (true) {
  // Actually run the code
  poller();
    sleep($poll_time);

}



?>
