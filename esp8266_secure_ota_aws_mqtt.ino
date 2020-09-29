/*

    #define PIN_WIRE_SDA (4)
    #define PIN_WIRE_SCL (5)

    static const uint8_t SDA = PIN_WIRE_SDA;
    static const uint8_t SCL = PIN_WIRE_SCL;

    static const uint8_t LED_BUILTIN = 16;
    static const uint8_t BUILTIN_LED = 16;

    static const uint8_t D0   = 16;
    static const uint8_t D1   = 5;
    static const uint8_t D2   = 4;
    static const uint8_t D3   = 0;
    static const uint8_t D4   = 2;
    static const uint8_t D5   = 14;
    static const uint8_t D6   = 12;
    static const uint8_t D7   = 13;
    static const uint8_t D8   = 15;
    static const uint8_t D9   = 3;
    static const uint8_t D10 = 1;
*/


#include "config.h"
#include "FS.h"
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

#ifdef OTA
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#endif

SoftwareSerial swSer(13, -1/*, false, 128*/); //Define hardware connections

// X(n+1) = (2053 * X(n)) + 13849)
#define RAND16_2053  ((uint16_t)(2053))
#define RAND16_13849 ((uint16_t)(13849))

#ifdef OTA
WiFiClientSecure otaClient;
#endif

#ifdef AWS
WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif

PubSubClient client(espClient);


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//clientID length=52
//String clientId = "TamojitSaha" //54 61 6d 6f 6a 69 74 53 61 68 61



unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("Occupancy_AP", "achillesresolute@tamojit"); // password protected ap

  if (!res) {
    //Serial.println("Failed to connect");
    ESP.restart();
  }
  else {
    //if you get here you have connected to the WiFi
    //Serial.println("connected...yeey :)");
  }


  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  espClient.setX509Time(timeClient.getEpochTime());
#ifdef OTA
  otaClient.setX509Time(timeClient.getEpochTime());
#endif
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
  }
  //Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(publishTopic, "Tamojit-ESP8266-77f8705f-4abd-4930-8e44-38a5a6247a6a");
      // ... and resubscribe
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
#ifdef AWS
      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);
#endif
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

#ifdef SIMULATE
uint16_t rand16seed;// = RAND16_SEED;
uint16_t random16()
{
  rand16seed = (rand16seed * RAND16_2053) + RAND16_13849;
  return rand16seed;
}
void random16_add_entropy( uint16_t entropy)
{
  rand16seed += entropy;
}
#endif

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  swSer.begin(19200);
  setup_wifi();
#ifdef AWS
  client.setServer(mqtt_server, 8883);
//#else
//  client.setServer(mqtt_server, 1883);
#endif
  client.setCallback(callback);
  randomSeed(analogRead(0));



  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
#ifdef AWS
  // Load certificate file
  File cert = SPIFFS.open("/occuTerm_AmazonCert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");

  delay(1000);

  if (espClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/occuTerm_key.der", "r"); //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Failed to open private key file");
  }
  else
    Serial.println("Success to open private key file");

  delay(1000);

  if (espClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");

  // Load CA file
  File ca = SPIFFS.open("/AmazonRootCA1.der", "r"); //replace ca eith your uploaded file name
  if (!ca) {
    Serial.println("Failed to open ca ");
  }
  else
    Serial.println("Success to open ca");

  delay(1000);

  if (espClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
#endif

#ifdef OTA
  File otaCACert = SPIFFS.open("/githubRootCA.der", "r");
  if (!otaCACert)
    Serial.println(F("OTA CA failed"));

  else {
    Serial.println(F("OTA CA success"));
    delay(1000);

    if (otaClient.loadCACert(otaCACert))
      Serial.println("OTA CA loaded");
    else
      Serial.println("OTA CA load failed");


    Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
  }
#endif

  checkForUpdates();
}



String buffer = "";
char _temp[30] = {};
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
#ifndef SIMULATE
  if (swSer.available() > 0) {

    int code = -1;
    swSer.readBytesUntil(';', _temp, 30);
    buffer = String(_temp);
    Serial.print("Received:"); Serial.println(buffer);

    snprintf (msg, MSG_BUFFER_SIZE, "%s,PID:A1;", buffer.c_str());

    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publishTopic, msg);
    memset(_temp, '\0', 30);
  }
  else {
    char dmsg[30];

    snprintf(dmsg, 30, "@%d, Waiting for data", random(500, 32001));

    Serial.print(F("Debug: "));
    Serial.print(dmsg);
    Serial.print(F(", at topic:")); Serial.println(debugTopic);
    client.publish(debugTopic, dmsg);
    delay(200);
  }
#endif

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    //++value;
    //String uuidStr = ESP8266TrueRandom.uuidToString(uuidNumber);
    //ESP8266TrueRandom.uuid(uuidNumber);
#ifdef SIMULATE
    uint32_t rand = random16();
    random16_add_entropy(random(65536L));
    //String PID = "A"+(String)random(1,2);
    snprintf (msg, MSG_BUFFER_SIZE, "@%u,Response:U:%d_L:%d,PID:A4;", rand, random(0, 10), random(0, 10), random(1, 3));
#else
    snprintf (msg, MSG_BUFFER_SIZE, "%s,PID:A1;", buffer.c_str());
#endif
    Serial.print(F("Publish message: "));
    Serial.println(msg);
    client.publish(publishTopic, msg);
    memset(_temp, 0, 30);
  }
}
