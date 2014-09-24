/**
 * Garden watcher - main file
 *
 * @author    Dario Dorfelli
 */

#include <SPI.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <Xively.h>

// Pins (pins 13,12,11,10,7,4 are used by the Ethernet/WiFi shield)
const byte outPin1 = 6;
const byte outPin2 = 9;
const byte outPin3 = 8;
const int inPin1 = A1;
const int inPin2 = A2;
const int inPin3 = A3;
const int inPin4 = A4;
const int inPin5 = A5;

// Number of analog pins connect to sensors and number of datastreams
const int sensorPinsToDatastreams = 5;

/* Classes */

// analogSensor Class
class analogSensor {
  public:
    analogSensor(char* name, int pin, int controlPin, int type);
    char* name();
    float read();
    void setup();
    int type();
  private:
    char* _name;
    int _pin;
    int _controlPin;
    int _type;  // type of sensor (1-resistance, 2-...)
};

analogSensor::analogSensor(char* name, int pin, int controlPin, int type) {
  _name = name;
  _pin = pin;
  _controlPin = controlPin;
  _type = type;
}

char* analogSensor::name() {
  return _name;
}

float analogSensor::read() {
  int vIn = 4.8;
  float value = 0;
  if(_type == 1){
    vIn = vIn - 0.7;  // transistor Vce
    digitalWrite(_controlPin, HIGH);   // sets the Transistor on
    value = analogRead(_pin);
    delay(1);
    digitalWrite(_controlPin, LOW);   // sets the Transistor on
    // R2 = R1/((Vin/Vout)-1)
    // Vout = (analogValue*vIn)/1024
    // R2[Ω] = R1/((Vin/((Vin*analogValue)/1024))-1)
    if(value != 0){
      value = 1000/((vIn/((vIn*value)/1024))-1);
    }
    
  }
  else if (_type == 2){
    value = analogRead(_pin);
    delay(1);
    // R1[Ω] = (((Vin*R2)/Vout)-R2)
    // Vout = ((analogValue*vIn)/1024)
    // R1[Ω] = (((vIn*R2)/((analogValue*vIn)/1024))-R2)
    if(value != 0){
      value = ((vIn*680)/((value*vIn)/1024))-680;
    }
  }
  else if (_type == 3){
    value = analogRead(_pin);
    delay(1);
    // Temp[°C] = ((Vin*analogValue)/1024)*100
    value = ((vIn*value)/1024)*100;
  }
  return value;
}

void analogSensor::setup() {
  // pins setup
  pinMode(_pin, INPUT);
  if(_type == 1){
    pinMode(_controlPin, OUTPUT);  // transistor on-off
  }
}

int analogSensor::type() {
  return _type;
}

/* Sensors */

// Analog sensors "name of sensor", input pin, control pin, type of sensor
analogSensor sensors[] = {
  analogSensor("temp", inPin1, 0, 3),
  analogSensor("blue", inPin2, outPin1, 1),
  analogSensor("green", inPin3, outPin2, 1),
  analogSensor("orange", inPin4, outPin3, 1),
  analogSensor("light", inPin5, 0, 2)
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

  for(k=0; k<sensorPinsToDatastreams; k++){
    sensors[k].setup();
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
  for(k=0; k<sensorPinsToDatastreams; k++){
    //read sensor values
    datastreams[k].setFloat(sensors[k].read());
    //print the sensor name and relative value
    Serial.print("Read ");
    Serial.print(sensors[k].name());
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
