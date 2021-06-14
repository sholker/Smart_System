
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

/********* WiFi Access Point ***********/

#define WLAN_SSID       "OMT2.4"
#define WLAN_PASS       "Covid21!"
#define DHTPin 5
#define DHTTYPE DHT22
#define ledPin 21
#define buzzerPin 4

/********* Adafruit.io Setup ***********/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL

#define AIO_USERNAME  "oriki"
#define AIO_KEY       "aio_ZrjP76q2fjYWgdwbHLCdXnuBSRto"

/**** Global State ******/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
//WiFiClient client;
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

DHT dht(DHTPin, DHTTYPE);

/********** Feeds *************/

Adafruit_MQTT_Subscribe humiditySubscribe = Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME "/feeds/humidity_sensor");
Adafruit_MQTT_Publish humidityPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity_sensor");
Adafruit_MQTT_Subscribe distanceSubscribe = Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME "/feeds/distance-sensor");

void MQTT_connect();

void setup() {
  
//Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

//Start measuring humidity and temperature
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  Serial.println();

//init
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
 
//observe changes
  mqtt.subscribe(&humiditySubscribe);
  mqtt.subscribe(&distanceSubscribe);

}

void loop() {

  MQTT_connect();

  //Read Humidity
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

//log
//  Serial.print(F("Humidity: "));
//  Serial.print(humidity);
  
//Upload to cloud
  humidityPublish.publish(humidity);

//Optimal humidity for the average plant is at least 50% humidity.
//We were unable to reduce the actual humidity in the room to less than 50%, 
//so just for the sake of showing that it works we changed the condition to 72%.
  Adafruit_MQTT_Subscribe *subscription_H;
  char* minHumidity = "72";
  while((subscription_H = mqtt.readSubscription(5000))) {
    if(subscription_H == &humiditySubscribe){
      Serial.println(F((char *)humiditySubscribe.lastread));
      if(strcmp((char *)humiditySubscribe.lastread, minHumidity) <= 0) {
        digitalWrite (ledPin, HIGH);   
      } else {
        digitalWrite (ledPin, LOW);
      }
    }
  }
  
  char* minDistance = "250";
  Adafruit_MQTT_Subscribe *subscription_D;
  while((subscription_D = mqtt.readSubscription(5000))) {
    if(subscription_D == &distanceSubscribe){
      Serial.println(F((char *)distanceSubscribe.lastread));
       if(atoi((char *)distanceSubscribe.lastread) < 150) {
        digitalWrite (buzzerPin, HIGH);   
      } else {
        digitalWrite (buzzerPin, LOW);
      }
    }
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

// Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  
  uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
