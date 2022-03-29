<?php

/* HAProxy version 1.5
   This file holds the arrays required to identify the statistics returned by the haproxy csv interface
   It is bound to the HAProxy version.
*/

// Define here what will be sent to the APMIA
$apmia_send = array("pxname" => false,
		               "svname" => false,
                   "qcur" => true, // current queued requests. For the backend this reports the number queued without a server assigned.
                   "qmax" => true, // max value of qcur
                   "scur" => true, // current sessions
                   "smax" => true, // max sessions
                   "slim" => true, // configured session limit
                   "stot" => true, // cumulative number of sessions
                   "bin" => true, // Bytes in
                   "bout" => true, // Bytes out
                   "dreq" => true, // requests denied because of security concerns.
                   "dresp" => true, // responses denied because of security concerns.
                   "ereq" => true, // request errors.
                   "econ" => true, // number of requests that encountered an error trying to connect to a backend server.
                   "eresp" => true, // response errors. srv_abrt will be counted here also.
                   "wretr" => true, //  number of times a connection to a server was retried.
                   "wredis" => true, // number of times a request was redispatched to another server
                   "status" => true, // status (UP/DOWN/NOLB/MAINT/MAINT(via)/MAINT(resolution)...)
                   "weight" => true, // total weight (backend), server weight (server)
                   "act" => true, // number of active servers (backend), server is active (server)
                   "bck" => true, // number of backup servers (backend), server is backup (server)
                   "chkfail" => true, // number of failed checks. (Only counts checks failed when the server is up.)
                   "chkdown" => true, // number of UP->DOWN transitions.
                   "lastchg" => true, // number of seconds since the last UP<->DOWN transition
                   "downtime" => true, // total downtime (in seconds).
                   "qlimit" => true, // configured maxqueue for the server, or nothing in the value is 0
                   "pid" => true, // process id (0 for first instance, 1 for second, ...)
                   "iid" => true, // unique proxy id
                   "sid" => true, // server id (unique inside a proxy)
                   "throttle" => true, // current throttle percentage for the server
                   "lbtot" => true, // total number of times a server was selected
                   "tracked" => true, // id of proxy/server if tracking is enabled.
                   "type" => true, // (0=frontend, 1=backend, 2=server, 3=socket/listener)
                   "rate" => true, // number of sessions per second over last elapsed second
                   "rate_lim" => true, // configured limit on new sessions per second
                   "rate_max" => true, // max number of new sessions per second
                   "check_status" => true, // status of last health check
                   "check_code" => true, // layer5-7 code, if available
                   "check_duration" => true, // time in ms took to finish last health check
                   "hrsp_1xx" => true, // http responses with 1xx code
                   "hrsp_2xx" => true, // http responses with 2xx code
                   "hrsp_3xx" => true, // http responses with 3xx code
                   "hrsp_4xx" => true, // http responses with 4xx code
                   "hrsp_5xx" => true, // http responses with 5xx code
                   "hrsp_other" => true, // http responses with other codes (protocol error)
                   "hanafail" => true, // failed health checks details
                   "req_rate" => true, // HTTP requests per second over last elapsed second
                   "req_rate_max" => true, // max number of HTTP requests per second observed
                   "req_tot" => true, // total number of HTTP requests received
                   "cli_abrt" => true, // number of data transfers aborted by the client
                   "srv_abrt" => true, // number of data transfers aborted by the server
                   "comp_in" => true, // number of HTTP response bytes fed to the compressor
                   "comp_out" => true, // number of HTTP response bytes emitted by the compressor
                   "comp_byp" => true, // number of bytes that bypassed the HTTP compressor (CPU/BW limit)
                   "comp_rsp" => true, // number of HTTP responses that were compressed
                   "lastsess" => true, // number of seconds since last session assigned to server/backend
                   "last_chk" => true, // last health check contents or textual error
                   "last_agt" => true, // last agent check contents or textual error
                   "qtime" => true, // the average queue time in ms over the 1024 last requests
                   "ctime" => true, // the average connect time in ms over the 1024 last requests
                   "rtime" => true, // the average response time in ms over the 1024 last requests
                   "ttime" => true, // the average total session time in ms over the 1024 last
);


// Don't change anything below!
$apmia_des = array("pxname =" => "StringEvent",
                   "svname" => "StringEvent",
                   "qcur" => "IntCounter", // current queued requests. For the backend this reports the number queued without a server assigned.
                   "qmax" => "IntCounter", // max value of qcur
                   "scur" => "IntCounter", // current sessions
                   "smax" => "IntCounter", // max sessions
                   "slim" => "IntCounter", // configured session limit
                   "stot" => "IntCounter", // cumulative number of sessions
                   "bin" => "LongCounter", // Bytes in
                   "bout" => "LongCounter", // Bytes out
                   "dreq" => "IntCounter", // requests denied because of security concerns.
                   "dresp" => "IntCounter", // responses denied because of security concerns.
                   "ereq" => "IntCounter", // request errors.
                   "econ" => "IntCounter", // number of requests that encountered an error trying to connect to a backend server.
                   "eresp" => "IntCounter", // response errors. srv_abrt will be counted here also.
                   "wretr" => "IntCounter", //  number of times a connection to a server was retried.
                   "wredis" => "IntCounter", // number of times a request was redispatched to another server
                   "status" => "StringEvent", // status (UP/DOWN/NOLB/MAINT/MAINT(via)/MAINT(resolution)...)
                   "weight" => "IntCounter", // total weight (backend), server weight (server)
                   "act" => "IntCounter", // number of active servers (backend), server is active (server)
                   "bck" => "IntCounter", // number of backup servers (backend), server is backup (server)
                   "chkfail" => "IntCounter", // number of failed checks. (Only counts checks failed when the server is up.)
                   "chkdown" => "IntCounter", // number of UP->DOWN transitions.
                   "lastchg" => "IntCounter", // number of seconds since the last UP<->DOWN transition
                   "downtime" => "IntCounter", // total downtime (in seconds).
                   "qlimit" => "StringEvent", // configured maxqueue for the server, or nothing in the value is 0
                   "pid" => "StringEvent", // process id (0 for first instance, 1 for second, ...)
                   "iid" => "StringEvent", // unique proxy id
                   "sid" => "StringEvent", // server id (unique inside a proxy)
                   "throttle" => "StringEvent", // current throttle percentage for the server
                   "lbtot" => "IntCounter", // total number of times a server was selected
                   "tracked" => "StringEvent", // id of proxy/server if tracking is enabled.
                   "type" => "IntCounter", // (0=frontend, 1=backend, 2=server, 3=socket/listener)
                   "rate" => "IntRate", // number of sessions per second over last elapsed second
                   "rate_lim" => "StringEvent", // configured limit on new sessions per second
                   "rate_max" => "IntRate", // max number of new sessions per second
                   "check_status" => "StringEvent", // status of last health check
                   "check_code" => "StringEvent", // layer5-7 code, if available
                   "check_duration" => "IntCounter", // time in ms took to finish last health check
                   "hrsp_1xx" => "IntCounter", // http responses with 1xx code
                   "hrsp_2xx" => "IntCounter", // http responses with 2xx code
                   "hrsp_3xx" => "IntCounter", // http responses with 3xx code
                   "hrsp_4xx" => "IntCounter", // http responses with 4xx code
                   "hrsp_5xx" => "IntCounter", // http responses with 5xx code
                   "hrsp_other" => "IntCounter", // http responses with other codes (protocol error)
                   "hanafail" => "StringEvent", // failed health checks details
                   "req_rate" => "IntRate", // HTTP requests per second over last elapsed second
                   "req_rate_max" => "IntRate", // max number of HTTP requests per second observed
                   "req_tot" => "IntCounter", // total number of HTTP requests received
                   "cli_abrt" => "IntCounter", // number of data transfers aborted by the client
                   "srv_abrt" => "IntCounter", // number of data transfers aborted by the server
                   "comp_in" => "LongCounter", // number of HTTP response bytes fed to the compressor
                   "comp_out" => "LongCounter", // number of HTTP response bytes emitted by the compressor
                   "comp_byp" => "LongCounter", // number of bytes that bypassed the HTTP compressor (CPU/BW limit)
                   "comp_rsp" => "IntCounter", // number of HTTP responses that were compressed
                   "lastsess" => "IntCounter", // number of seconds since last session assigned to server/backend
                   "last_chk" => "StringEvent", // last health check contents or textual error
                   "last_agt" => "StringEvent", // last agent check contents or textual error
                   "qtime" => "IntCounter", // the average queue time in ms over the 1024 last requests
                   "ctime" => "IntCounter", // the average connect time in ms over the 1024 last requests
                   "rtime" => "IntCounter", // the average response time in ms over the 1024 last requests
                   "ttime" => "IntCounter", // the average total session time in ms over the 1024 last
);


$apmia_type = array("pxname =" => "String",
                   "svname" => "String",
                   "qcur" => "|Queue:Current queued requests", // current queued requests. For the backend this reports the number queued without a server assigned.
                   "qmax" => "|Queue:Max current queued requests", // max value of qcur
                   "scur" => "|Queue:Current sessions", // current sessions
                   "smax" => "|Sessions:Max sessions", // max sessions
                   "slim" => "|Sessions:Configured session limit", // configured session limit
                   "stot" => "|Sessions:Total sessions", // cumulative number of sessions
                   "bin" => "|Bytes:Bytes in", // Bytes in
                   "bout" => "|Bytes:Bytes out", // Bytes out
                   "dreq" => "|Denied:Requests denied", // requests denied because of security concerns.
                   "dresp" => "|Denied:Responses denied", // responses denied because of security concerns.
                   "ereq" => "|Errors:Request errors", // request errors.
                   "econ" => "|Errors:Errors during connection to backend", // number of requests that encountered an error trying to connect to a backend server.
                   "eresp" => "|Errors:Response errors", // response errors. srv_abrt will be counted here also.
                   "wretr" => "|Warnings:Nr connection retries", //  number of times a connection to a server was retried.
                   "wredis" => "|Warnings:Nr connections rerouted", // number of times a request was redispatched to another server
                   "status" => "|Server:Status", // status (UP/DOWN/NOLB/MAINT/MAINT(via)/MAINT(resolution)...)
                   "weight" => "|Server:Server weight", // total weight (backend), server weight (server)
                   "act" => "|Server:Active servers", // number of active servers (backend), server is active (server)
                   "bck" => "|Server:Backup servers", // number of backup servers (backend), server is backup (server)
                   "chkfail" => "|Server:Failed server check", // number of failed checks. (Only counts checks failed when the server is up.)
                   "chkdown" => "|Server:Up-Down transitions", // number of UP->DOWN transitions.
                   "lastchg" => "|Server:Seconds since last Up-Dowm transition", // number of seconds since the last UP<->DOWN transition
                   "downtime" => "|Server:Total downtime", // total downtime (in seconds).
                   "qlimit" => "|Queue:Configure queue limit", // configured maxqueue for the server, or nothing in the value is 0
                   "pid" => "|Server:Instance ID", // process id (0 for first instance, 1 for second, ...)
                   "iid" => "|Server:Unique proxy ID", // unique proxy id
                   "sid" => "|Server:Unique server ID", // server id (unique inside a proxy)
                   "throttle" => "|Server:Throttle percentage", // current throttle percentage for the server
                   "lbtot" => "|Server:Cnt server was selected", // total number of times a server was selected
                   "tracked" => "|Server:ID of proxy", // id of proxy/server if tracking is enabled.
                   "type" => "|Server:Server type", // (0=frontend, 1=backend, 2=server, 3=socket/listener)
                   "rate" => "|Session rate:Nr sessions during last second", // number of sessions per second over last elapsed second
                   "rate_lim" => "|Session rate:Conf session limit per sec.", // configured limit on new sessions per second
                   "rate_max" => "|Session rate:Max session per sec.", // max number of new sessions per second
                   "check_status" => "|Server:Status", // status of last health check
                   "check_code" => "|Server:Layer 5-7 code", // layer5-7 code, if available
                   "check_duration" => "|Server:Health check duration", // time in ms took to finish last health check
                   "hrsp_1xx" => "|Responses:http responses with 1xx code", // http responses with 1xx code
                   "hrsp_2xx" => "|Responses:http responses with 2xx code", // http responses with 2xx code
                   "hrsp_3xx" => "|Responses:http responses with 3xx cod", // http responses with 3xx code
                   "hrsp_4xx" => "|Responses:http responses with 4xx code", // http responses with 4xx code
                   "hrsp_5xx" => "|Responses:http responses with 5xx code", // http responses with 5xx code
                   "hrsp_other" => "|Errors:Protocol errors", // http responses with other codes (protocol error)
                   "hanafail" => "|Server:Failed health check", // failed health checks details
                   "req_rate" => "|Requests:HTTP req. per last second", // HTTP requests per second over last elapsed second
                   "req_rate_max" => "|Requests:Max HTTP req per sec. seen", // max number of HTTP requests per second observed
                   "req_tot" => "|Requests:Total HTTP requests", // total number of HTTP requests received
                   "cli_abrt" => "|Requests:Client aborted transfers", // number of data transfers aborted by the client
                   "srv_abrt" => "|Server:Server aborted transfers", // number of data transfers aborted by the server
                   "comp_in" => "|Bytes:Bytes to the compressor", // number of HTTP response bytes fed to the compressor
                   "comp_out" => "|Bytes:Bytes emitted by compressor", // number of HTTP response bytes emitted by the compressor
                   "comp_byp" => "|Bytes:Bytes bypassing compressor", // number of bytes that bypassed the HTTP compressor (CPU/BW limit)
                   "comp_rsp" => "|Responses:Nr compressed HTTP responses", // number of HTTP responses that were compressed
                   "lastsess" => "|Sessions:Last session assigned (s)", // number of seconds since last session assigned to server/backend
                   "last_chk" => "|Server:Last health error message", // last health check contents or textual error
                   "last_agt" => "|Server:Last agent error message", // last agent check contents or textual error
                   "qtime" => "|Queue:Average queue time (ms)", // the average queue time in ms over the 1024 last requests
                   "ctime" => "|Requests:Average connect time (ms)", // the average connect time in ms over the 1024 last requests
                   "rtime" => "|Responses:Average response time (ms)", // the average response time in ms over the 1024 last requests
                   "ttime" => "|Sessions:Average total session time (ms)", // the average total session time in ms over the 1024 last
);

?>
