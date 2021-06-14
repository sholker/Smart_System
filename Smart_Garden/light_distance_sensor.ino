/***************************************************

SHENKAR - SMART SYSTEMS
By: Ori Shinsholker & Michal Tamir & Osnat Blau & Toval Barak & Deviad Bokobza
DATE: Jun-2021

 ****************************************************/
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"



/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "OMT2.4"
#define WLAN_PASS       "Covid21!"

/************************* WiFi Access Point *********************************/

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "oriki"
#define AIO_KEY       "aio_ZrjP76q2fjYWgdwbHLCdXnuBSRto"

/************************* Adafruit.io Setup *********************************/


/***************************** Global Val **********************************/

#define LIGHT_PIN 33 //light sensor
#define LED_RED 15 //led
#define LED_BLUE 12 //led
#define LED_WHITE 13 //led
//distance sensor
#define trigPin 14    // Trigger
#define echoPin 27  // Echo
long duration, cm, inches;

int sum_light_level=0;
int i=0;
int count=0;//counter 4 time dilay
/***************************** Global Val **********************************/



/************ Global State ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/************ Global State ******************/

/****************************** Feeds ***************************************/

// Setup a feed called 'light-sensor' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>

Adafruit_MQTT_Publish light_level = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME "/feeds/light-sensor");
Adafruit_MQTT_Publish distance_feed = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME "/feeds/distance-sensor");


// Setup a feed called 'light-sensor' for subscribing to changes.
Adafruit_MQTT_Subscribe light_level_read = Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME "/feeds/light-sensor");

/****************************** Feeds ***************************************/


/*************************** Sketch Code ************************************/

int val = 0;

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();



void setup() {
    Serial.begin(115200);

    //LEDs
    pinMode(LED_RED, OUTPUT); // RED
    pinMode(LED_BLUE, OUTPUT); // BLUE
    pinMode(LED_WHITE, OUTPUT); // WHITE

    

    //Distance Sensor
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    
    
    delay(1000);
    Serial.println(F("Starting..."));
    delay(1000);
    Serial.println(F("\n\n##################################"));
    Serial.println(F("Adafruit MQTT demo - SHENKAR"));
    Serial.println(F("--> YOU GUYS ARE AWESOME! -- "));
    Serial.println(F("##################################"));

    // Connect to WiFi access point.
    Serial.println(); Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);

    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.println("WiFi connected");
    Serial.println("IP address: "); Serial.println(WiFi.localIP());
    Serial.println();
// SETUP THE SUBSRCIPTIONS 
    mqtt.subscribe(&light_level_read);
}




void loop() {
// We must keep this for now
    MQTT_connect();
    count++;
    ///******************************** GET Distance Value ******************************
      // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
     
      // Read the signal from the sensor: a HIGH pulse whose
      // duration is the time (in microseconds) from the sending
      // of the ping to the reception of its echo off of an object.
      pinMode(echoPin, INPUT);
      duration = pulseIn(echoPin, HIGH);
     
      // Convert the time into a distance
      cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
      
      Serial.print(cm);
      Serial.print("cm");
      Serial.println();

      ///******************************** GET Distance Value ******************************



    //********************************* Send parameter to adafruit **************************


    // Now we can publish stuff!
    Serial.print(F("\nSending Distance value =>"));
    Serial.print(cm);
    Serial.print("...");
  if (cm>=0 && cm<=1000)
    if (!distance_feed.publish((int)cm)) {
        Serial.println(F("Failed"));
    } else {
        Serial.println(F("OK!"));
    }

    //********************************* Send parameter to adafruit **************************


//if (count %5==0){
    //********************************* Send parameter to adafruit **************************

    val = analogRead(LIGHT_PIN); //read form photoresistor

    // Now we can publish stuff!
    Serial.print(F("\nSending Light value =>"));
    Serial.print(val);
    Serial.print("...");

    if (! light_level.publish(val)) {
        Serial.println(F("Failed"));
    } else {
        Serial.println(F("OK!"));
    }

    //********************************* Send parameter to adafruit **************************

   
   //********************************* Read parameter to adafruit **************************


    Serial.print("i= ");
    Serial.println(i);

    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(5000))) {
        // Check if its the onoff button feed
        if (subscription == &light_level_read) {
            Serial.print(F("Light Level: "));
            Serial.println((char *)light_level_read.lastread);

            uint16_t light_level_value = atoi((char *)light_level_read.lastread);  // convert to a number
            sum_light_level+=light_level_value;
            i++;
        }
    }

   //********************************* Read parameter to adafruit **************************


    // if read 6 values from photoresistor
    if (i==5){
        Serial.print("The summary of last hour is: ");
        Serial.println(sum_light_level);

        // check if the light level in the last hour is above 4000
        if (sum_light_level >4000){
            Serial.println("Blue");
            digitalWrite(LED_RED, LOW);   // turn off the red LED on (HIGH is the voltage level)
            digitalWrite(LED_BLUE, HIGH);
            digitalWrite(LED_WHITE,LOW);
        }
        else{
            Serial.println("RED");
            digitalWrite(LED_RED, HIGH);   // turn the red LED on (HIGH is the voltage level)
            digitalWrite(LED_BLUE,LOW);
              digitalWrite(LED_WHITE,HIGH);
        }

       //reset the value for new lastest values
        sum_light_level=0; 
        i=0;
    }
//}
   //********************************* Read parameter to adafruit **************************


    // ping the server to keep the mqtt connection alive
    // NOT required if you are publishing once every KEEPALIVE seconds

    if(! mqtt.ping()) {
        mqtt.disconnect();
    }
//        delay(60000);   //delay 60 seconds
        delay(400);   //delay 2 seconds for testing

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
