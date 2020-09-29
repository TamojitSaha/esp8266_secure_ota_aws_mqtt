#define SIMULATE
#define AWS
#define OTA




const int FW_VERSION = 1245;
const char* fwBaseUrl = "https://raw.githubusercontent.com/TamojitSaha/esp8266_secure_ota_aws_mqtt/master/fota/";

#ifdef AWS
const char* mqtt_server = "axcqaksnw49b6-ats.iot.us-east-2.amazonaws.com"; //MQTT broker ip
//#else
//const char* mqtt_server = "broker.hivemq.com";//iot.eclipse.org,broker.hivemq.com
#endif

#ifdef SIMULATE
const char* publishTopic = "tamojitsaha/mqtt/occupancy/38a5a6247a6a/simulate";      //topic for esp8266 to publish data
#else
const char* publishTopic = "tamojitsaha/mqtt/occupancy/38a5a6247a6a/count";      //topic for esp8266 to publish data
const char* debugTopic = "tamojitsaha/mqtt/occupancy/38a5a6247a6a/debug";
#endif
const char* subscribeTopic = "tamojitsaha/mqtt/occupancy/38a5a6247a6a/in/led";   //topic for esp8266 to which it will subscribe
String clientId = "Tamojit-ESP8266-77f8705f-4abd-4930-8e44-38a5a6247a6a";
 
