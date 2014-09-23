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
const int PINS = 5;

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet/WiFi shield)

class analogSensor {
  public:
    analogSensor(char* name, int pin, int type);
    char* name();
    int read();
    int type();
  private:
    char* _name;
    int _pin;
    int _type;  // type of sensor (1-resistance, 2-...)
};

analogSensor::analogSensor(char* name, int pin, int type) {
  pinMode(pin, INPUT);
  _name = name;
  _pin = pin;
  _type = type;
}

char* analogSensor::name() {
  return _name;
}

int analogSensor::read() {
  int value = analogRead(_pin);
  delay(1);
  if(analogSensor::type() == 1){
    // R2 = R1/((Vin/Vout)-1)
    // Vout = (analog*5)/1024
    int value = 1000/(5/(((value*5)/1024)-1));
  }
  return value;
}

int analogSensor::type() {
  return _type;
}

// Analog sensor pins and relative display name
analogSensor sensors[] = {
  analogSensor("temp", A1, 1),
  analogSensor("blue", A2, 1),
  analogSensor("green", A3, 1),
  analogSensor("orange", A4, 1),
  analogSensor("light", A5, 1)
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
char tempId[] = "temp";
char blueId[] = "blue";
char greenId[] = "green";
char orangeId[] = "orange";
char lightId[] = "light";

// Number of datastreams
byte datastreamsNumber = 5;

// Define the strings for our datastream IDs
XivelyDatastream datastreams[] = {
  XivelyDatastream(tempId, strlen(tempId), DATASTREAM_FLOAT),
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
  // for(k=0; k<PINS; k++){
  //   pinMode(readPins[k], INPUT);
  // }

  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(8, OUTPUT);  // transistor on-off
  pinMode(9, OUTPUT);  // transistor on-off
  pinMode(6, OUTPUT);  // transistor on-off

  
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
  
  digitalWrite(8, HIGH);   // sets the Transistor on
  digitalWrite(9, HIGH);   // sets the Transistor on
  digitalWrite(6, HIGH);   // sets the Transistor on

  for(k=0; k<PINS; k++){
    //read sensor values
    int value = sensors[k].read();
    delay(1);
    datastreams[k].setFloat(value);
    //print the sensor name and relative value
    Serial.print("Read ");
    Serial.print(sensors[k].name());
    Serial.print(" sensor value ");
    Serial.println(datastreams[k].getFloat());
  }

  digitalWrite(8, LOW);    // sets the Transistor off
  digitalWrite(9, LOW);    // sets the Transistor off
  digitalWrite(6, LOW);    // sets the Transistor off

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
