/**
 * Garden watcher - main file
 *
 * @author    Dario Dorfelli
 */

#include <SPI.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <Xively.h>

// Number of analog pins connect to sensors
#define PINS 4

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet/WiFi shield)
// Pins and relative display name
const char readPins[2][PINS] = { 
  {'A2','A3','A4','A5'},
  {'blueMoisture','greenMoisture','orangeMoisture','light'}
};

// for variable
byte k = 0;

/* Network */

char ssid[] = "";    // network SSID
char pass[] = "";    // network password (use for WPA, or use as key for WEP)
int keyIndex = 0;    // network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

/* Xively */

// Your Xively key to let you upload data
char xivelyKey[] = "";

// Your Xively feed ID
#define xivelyFeed 1743234869

// Datastreams
char blueId[] = "blue";
char greenId[] = "green";
char orangeId[] = "orange";
char lightId[] = "light";

// Number of datastreams
byte datastreamsNumber = 4;

// Define the strings for our datastream IDs
XivelyDatastream datastreams[] = {
  XivelyDatastream(blueId, strlen(blueId), DATASTREAM_FLOAT),
  XivelyDatastream(greenId, strlen(greenId), DATASTREAM_FLOAT),
  XivelyDatastream(orangeId, strlen(orangeId), DATASTREAM_FLOAT),
  XivelyDatastream(lightId, strlen(lightId), DATASTREAM_FLOAT)
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(xivelyFeed, datastreams, datastreamsNumber /* number of datastreams */);

WiFiClient client;
XivelyClient xivelyclient(client);

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm \n");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //pins setup
  for(k=0; k<PINS; k++){
    pinMode(readPins[0][k], INPUT);
  }
  
  Serial.println("Starting single datastream upload to Xively...");
  Serial.println();

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
  printWifiStatus();
}

void loop() {
  
  for(k=0; k<PINS; k++){
    //read sensor values
    int value = analogRead(readPins[0][k]);
    delay(1);
    datastreams[k].setFloat(value);
    //print the sensor name and relative value
    Serial.print("Read ");
    Serial.print(readPins[1][k]);
    Serial.print(" sensor value ");
    Serial.println(datastreams[k].getFloat());
  }

  //send value to xively
  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  //return message
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
 
  Serial.println();
  //delay between calls
  delay(30000);
}
