# php_haproxy-collector 1.0-4 by J. Mertin -- joerg.mertin@broadcom.com
Purpose: Collects haproxy csv stats and sends them to restfull enabled APMIA 

# Description
## Introduction

this package provides the possibility to import haproxy statistics
into the APMIA.

### Requirements

- The restfull interface of the APMIA needs to be enabled


### APMIA details

The detailed documentation for the APMIA can be found under https://techdocs.broadcom.com/us/en/ca-enterprise-software/it-operations-management/application-performance-management/10-7/implementing-agents/infrastructure-agent/epagent-plug-ins/configure-epagent-network-ports/enable-the-epagent-plug-ins-restful-interface.html

Available Metric types in APMIA are

#### PerIntervalCounter
The value is a rate per interval. The metrics are aggregated over time by adding up the values. The sum of values is kept by a counter. At the end of the 15-second interval, the value of the counter is reported. After that, the counter is reset to zero.
Use this data type whenever you report on the rate at which something is occurring. For example, the rate of errors per interval.

#### IntCounter/LongCounter
 Reports the integer or long value that is sent to the metric. This attribute keeps reporting the same value over every subsequent 15-second interval, until a new value is supplied.
This data type can be useful in the context of reporting queue depth, threads available, or similar gauge-like, tally metrics.

#### IntAverage/LongAverage
This attribute calculates and reports the averages (integer or long) for the timings you supply to the metric. As the IntAverage/LongAverage receives your measurements, it keeps a running count and a running average of your results. At the end of each 15-second interval, the attribute reports information about measurements. IntAverage/LongAverage reports the count of measurements, the mean of all measurements, the fastest measurement, and the slowest measurement.
Use this data type to calculate response times, like “average latency time in milliseconds”.

#### IntRate
The value is a per-second integer rate. IntRate divides one or more values you supply by 15. This attribute assumes a per second
rate when looking at live data. For a 15-second interval, the remainder (14 or fewer) are truncated. When aggregated over multiple time periods, the weighted average is used as the aggregated value.
Use this data type in the context of queries per second
, for example.

#### StringEvent
Reports string values, like a process ID or a startup command line. This metric data type introduces a considerable overhead in terms of memory and bandwidth, so use it sparingly. Reporting log entries as string events is not recommended.

#### Timestamp
This data type generates successively increasing timestamps. The value is the number of milliseconds that have elapsed because January 1, 1970 00:00:00 UTC (UNIX Epoch Time).
Reporting timestamps is unnecessary, because the performance metrics are all automatically timestamped when the Enterprise Manager receives them.

Example:
```
{ metrics : [{type : "PerIntervalCounter", name : "MyTest|RESTFul|PerIntervalCounter|Test1:Count", value : "123"}] }
{ metrics : [{type : "LongCounter", name : "MyTest|RESTFul|LongCounter|Test2:Count", value : "456"}] }
```

#### APMia Rest API requires data to come in the following format

   {"type" : "<a supported metric type>",
   "name"  : "<a unique metric name (including path but excluding host|process|agent)>",
   "value" : "<the metric value>"}

 {"host"    : "<agent hostname>",     (Optional.  If set, it must match that in IntroscopeAgent.properties)
  "process" : "<agent process name>", (Optional.  If set, it must match that in IntroscopeAgent.properties)
  "agent"   : "<agent name>",         (Optional.  If set, it must match that in IntroscopeAgent.properties)
  "metrics" : [{"type" : "<type>", "name" : "<name>", "value" : "<value>"},
               {"type" : "<type>", "name" : "<name>", "value" : "<value>"},
               {"type" : "<type>", "name" : "<name>", "value" : "<value>"}


 {"host"    : "<agent hostname>",     (Optional.  If set, it must match that in IntroscopeAgent.properties)
  "process" : "<agent process name>", (Optional.  If set, it must match that in IntroscopeAgent.properties)
  "agent"   : "<agent name>",         (Optional.  If set, it must match that in IntroscopeAgent.properties)
  "metrics" : [{"type" : "<type>", "name" : "<name>", "value" : "<value>"},
               {"type" : "<type>", "name" : "<name>", "value" : "<value>"},
               {"type" : "<type>", "name" : "<name>", "value" : "<value>"}



### HAProxy docs are located here:
http://cbonte.github.io/haproxy-dconv/1.7/management.html#9.1

 */


# Manual Changelog
```
Fri, 25 Mar 2022 17:07:11
	- Initial release
```
