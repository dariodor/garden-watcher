/**
 * Garden watcher - main file
 *
 * @author    Dario Dorfelli
 */

#include <SPI.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <Xively.h>

// Pins
const byte outPin1 = 6;
const byte outPin2 = 8;
const byte outPin3 = 9;
const int inPin1 = A1;
const int inPin2 = A2;
const int inPin3 = A3;
const int inPin4 = A4;
const int inPin5 = A5;

// Number of analog pins connect to sensors
const int sensorPinsToDatastreams = 5;

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
  int vIn = 4.8;
  int value = analogRead(_pin);
  delay(1);
  if(analogSensor::type() == 1){
    vIn = vIn - 0.7;  // transistor Vce
    // R2 = R1/((Vin/Vout)-1)
    // Vout = (analogValue*vIn)/1024
    // R2[Ω] = R1/((((Vin*analogValue)/1024)/Vin)-1)
    value = 1000/((((vIn*value)/1024)/vIn)-1);
  }
  else if (analogSensor::type() == 2){
    // R2 = R1/((Vin/Vout)-1)
    // Vout = (analogValue*vIn)/1024
    // R2[Ω] = R1/((((Vin*analogValue)/1024)/Vin)-1)
    value = 1000/((((vIn*value)/1024)/vIn)-1);
  }
  else if (analogSensor::type() == 3){
    // Temp[°C] = ((Vin*analogValue)/1024)*100
    value = ((vIn*value)/1024)*100;
  }
  return value;
}

int analogSensor::type() {
  return _type;
}

// Analog sensor pins and relative display name
analogSensor sensors[] = {
  analogSensor("temp", inPin1, 3),
  analogSensor("blue", inPin2, 1),
  analogSensor("green", inPin3, 1),
  analogSensor("orange", inPin4, 1),
  analogSensor("light", inPin5, 2)
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
  // for(k=0; k<sensorPinsToDatastreams; k++){
  //   pinMode(readPins[k], INPUT);
  // }

  pinMode(inPin1, INPUT);
  pinMode(inPin2, INPUT);
  pinMode(inPin3, INPUT);
  pinMode(inPin4, INPUT);
  pinMode(inPin5, INPUT);
  pinMode(outPin1, OUTPUT);  // transistor on-off
  pinMode(outPin2, OUTPUT);  // transistor on-off
  pinMode(outPin3, OUTPUT);  // transistor on-off

  
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
  
  digitalWrite(outPin1, HIGH);   // sets the Transistor on
  digitalWrite(outPin2, HIGH);   // sets the Transistor on
  digitalWrite(outPin3, HIGH);   // sets the Transistor on

  for(k=0; k<sensorPinsToDatastreams; k++){
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

  digitalWrite(outPin1, LOW);    // sets the Transistor off
  digitalWrite(outPin2, LOW);    // sets the Transistor off
  digitalWrite(outPin3, LOW);    // sets the Transistor off

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
