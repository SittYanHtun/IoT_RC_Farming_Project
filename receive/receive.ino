#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2SzJ8gP8D"
#define BLYNK_TEMPLATE_NAME "IOTprojectRemoteCar"
#define BLYNK_AUTH_TOKEN "_WFt90Mlmm904Qvh2PdszewDAmf_i4yR"

#include <vector>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const int bufferSize = 128; // Size of the character array (adjust as needed)
char receivedData[bufferSize]; // Character array to store received data
int newDataLen = 0;

int servoPin = D7;

// Sensor pins
#define sensorPower D6
#define sensorPin A0

/* Change these values based on your calibration values */
#define soilWet 500   // Define max value we consider soil 'wet'
#define soilDry 750   // Define min value we consider soil 'dry'

int dhtPin = D5;
#define dhtType DHT11
DHT dht(dhtPin, dhtType);

char ssid[] = "GUSTO WiFi";
char pass[] = "Gusto@123";

// for data store
std::vector<double> tempVector;
std::vector<double> humiVector;
std::vector<int> soilVector;
// for data averate temp
std::vector<double> tempAvg;
std::vector<double> humiAvg;
std::vector<int> soilAvg;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(sensorPower, OUTPUT);
  digitalWrite(sensorPower, LOW); // Initially keep the sensor OFF
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V4, 0);
  Blynk.virtualWrite(V6, "Status: Let Test");
}

void loop() {
  Blynk.run();
  // Read data from serial port and store it in the receivedData array
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      receivedData[newDataLen] = '\0'; // Null-terminate the string
      newDataLen = 0; // Reset the length counter
      processReceivedData(); // Call a function to process the received data
    } else {
      receivedData[newDataLen++] = c;
      if (newDataLen >= bufferSize) {
        // Prevent buffer overflow
        newDataLen = bufferSize - 1;
      }
    }
  }
}

void processReceivedData() {
  // Check if receivedData is equal to "Hello, Receiver!"
  if (receivedData[0] == 'G') {
    CheckSensors();
  } 
  if (receivedData[0] == 'o') {
    ShowResult();
  }
  else {
    // Handle other received data here
    Serial.print("Received Data: ");
    Serial.println(receivedData);
  }
  // You can now use the 'receivedData' variable for further processing
  // For example, you can perform actions based on the received data.
  
  // Clear the received data buffer for the next message
  memset(receivedData, 0, sizeof(receivedData));
}

void ShowResult() {
  int soil;
  double humi;
  double temp;

  for (int value : soilVector) {
    soil += value;
  }

  for (int value : humiVector) {
    humi += value;
  }

  for (int value : tempVector) {
    temp += value;
  }

  soil = static_cast<int>(soil / soilVector.size());
  humi = humi / humiVector.size();
  temp = temp / tempVector.size(); 

  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, humi);
  Blynk.virtualWrite(V3, soil);
	if (soil < soilWet) {
		Serial.println("Status: Soil is too wet");
    Blynk.virtualWrite(V6, "Status: Soil is too wet");
	} else if (soil >= soilWet && soil < soilDry) {
		Serial.println("Status: Soil moisture is perfect");
    Blynk.virtualWrite(V6, "Status: Soil is perfect");
	} else {
		Serial.println("Status: Soil is too dry - time to water!");
    Blynk.virtualWrite(V6, "Status: Soil is dry");
	}

  soilVector.clear();
  humiVector.clear();
  tempVector.clear();

  Blynk.virtualWrite(V4, 0);
}

void CheckSensors() {
  SoilSensor();
  temp_hum();
  SoilSensor();
  temp_hum();
  SoilSensor();
  temp_hum();
  SoilSensor();
  temp_hum();
  SoilSensor();
  temp_hum();

  CalcuateAvg();
}

void CalcuateAvg() {
  int soil;
  double humi;
  double temp;

  soil = static_cast<int>((soilAvg[0] + soilAvg[1] + soilAvg[2] + soilAvg[3] + soilAvg[4]) / 5);
  humi = (humiAvg[0] + humiAvg[1] + humiAvg[2] + humiAvg[3] + humiAvg[4]) / 5;
  temp = (tempAvg[0] + tempAvg[1] + tempAvg[2] + tempAvg[3] + tempAvg[4]) / 5;

  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, humi);
  Blynk.virtualWrite(V3, soil);
	if (soil < soilWet) {
		Serial.println("Status: Soil is too wet");
    Blynk.virtualWrite(V6, "Status: Soil is too wet");
	} else if (soil >= soilWet && soil < soilDry) {
		Serial.println("Status: Soil moisture is perfect");
    Blynk.virtualWrite(V6, "Status: Soil is perfect");
	} else {
		Serial.println("Status: Soil is too dry - time to water!");
    Blynk.virtualWrite(V6, "Status: Soil is dry");
	}

  soilVector.push_back(soil);
  humiVector.push_back(humi);
  tempVector.push_back(temp);

  Blynk.virtualWrite(V4, tempVector.size());

  soilAvg.clear();
  humiAvg.clear();
  tempAvg.clear();
}

void temp_hum () {
  // put your main code here, to run repeatedly:
  // the riser the temperature the riser the humidity
  double h = dht.readHumidity();
  delay(10);
  double t = dht.readTemperature();
  Serial.print("\nHumidity: ");
  Serial.print(h);
  Serial.print("%");
  Serial.print("\nTemperature: ");
  Serial.print(t);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  tempAvg.push_back(t);
  humiAvg.push_back(h);
  delay(1000);
}

void SoilSensor() {
  	//get the reading from the function below and print it
	int moisture = readSensor();
	Serial.print("Analog Output: ");
	Serial.println(moisture);

  Blynk.virtualWrite(V3, moisture);

	// Determine status of our soil
	if (moisture < soilWet) {
		Serial.println("Status: Soil is too wet");
    Blynk.virtualWrite(V6, "Status: Soil is too wet");
	} else if (moisture >= soilWet && moisture < soilDry) {
		Serial.println("Status: Soil moisture is perfect");
    Blynk.virtualWrite(V6, "Status: Soil is perfect");
	} else {
		Serial.println("Status: Soil is too dry - time to water!");
    Blynk.virtualWrite(V6, "Status: Soil is dry");
	}
  
  soilAvg.push_back(moisture);
	
	delay(1000);	// Take a reading every second for testing
					// Normally you should take reading perhaps once or twice a day
	Serial.println();
}

//  This function returns the analog soil moisture measurement
int readSensor() {
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	delay(10);							// Allow power to settle
	int val = analogRead(sensorPin);	// Read the analog value form sensor
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							// Return analog moisture value
}
