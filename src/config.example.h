const char* hostname = "jjy-emitter";
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* ntp[] = {"0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org"};
const char* tz = "CET-1CEST,M3.5.0,M10.5.0/3";
//const char* tz = "GMT0";

// If true, will deep-sleep one hour at a time until reaches the hour it is configured to broadcast. Will also broadcast one hour after first boot/reset regardless of limit placed on hours.
#define LIMIT_HOURS true
#if LIMIT_HOURS
// Config suitable for Citizen H100 movement that performs reception from 02:00, and if that fails at 04:00. This config broadcasts time-signal between 02:00-03:00
const short hourMin = 2;
const short hourMax = 3;
#endif