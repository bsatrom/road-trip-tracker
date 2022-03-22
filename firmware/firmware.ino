#include <Notecard.h>
#include "SparkFunCCS811.h"
#include "SparkFunBME280.h"
#include <Wire.h>

#define CCS811_ADDR 0x5B
#define PRODUCT_UID "com.blues.bsatrom:road_trip_tracker"

Notecard notecard;
CCS811 airQualitySensor(CCS811_ADDR);
BME280 tempSensor;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 360000;

bool airQualitySensorAvailable = false;
bool tempSensorAvailable = false;

void setup() {
  Serial.begin(115200);
  delay(2500);
  Serial.println("Blues Road Trip Tracker - Feather Edition");
  Serial.println("=========================================");

  notecard.setDebugOutputStream(Serial);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  tempSensor.settings.commInterface = I2C_MODE;
  tempSensor.settings.I2CAddress = 0x77;
  tempSensor.settings.runMode = 3; //Normal mode
  tempSensor.settings.tStandby = 0;
  tempSensor.settings.filter = 4;
  tempSensor.settings.tempOverSample = 5;
  tempSensor.settings.pressOverSample = 5;
  tempSensor.settings.humidOverSample = 5;

  Wire.begin();
  delay(250);
  notecard.begin();

  if (tempSensor.beginI2C() == false) {
    Serial.println("BME280 error. Please check wiring.");
  } else {
    Serial.println("BME280 Connected.");
    tempSensorAvailable = true;
  }

  if (airQualitySensor.begin() == false) {
    Serial.println("CCS811 error. Please check wiring.");
  } else {
    Serial.println("CCS811 Connected.");
    airQualitySensorAvailable = true;
  }
  
  J *req = notecard.newRequest("hub.set");
  JAddStringToObject(req, "product", PRODUCT_UID);
  JAddStringToObject(req, "sn", "feather-tracker");
  JAddStringToObject(req, "mode", "periodic");
  
  notecard.sendRequest(req);

  req = notecard.newRequest("card.location.mode");
  JAddStringToObject(req, "mode", "periodic");
  JAddNumberToObject(req, "seconds", 300);
  
  notecard.sendRequest(req);

  req = notecard.newRequest("card.location.track");
  JAddBoolToObject(req, "start", true);
  JAddBoolToObject(req, "heartbeat", true);
  JAddBoolToObject(req, "sync", true);
  JAddNumberToObject(req, "hours", 4);
  
  notecard.sendRequest(req);

  delay(1000);
  
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  currentMillis = millis();

  if (currentMillis - startMillis >= period) {

    if (tempSensorAvailable) {

      float tempC = tempSensor.readTempC();
      float humid = tempSensor.readFloatHumidity();
      float tempF = tempSensor.readTempF();
      float pressure = tempSensor.readFloatPressure();
      float alt = tempSensor.readFloatAltitudeFeet();

      airQualitySensor.setEnvironmentalData(humid, tempC);
        
      Serial.print("Humidity: ");
      Serial.print(humid, 0);
    
      Serial.print(" Pressure: ");
      Serial.print(pressure, 0);
    
      Serial.print(" Alt: ");
      Serial.print(alt, 1);
    
      Serial.print(" Temp: ");
      Serial.print(tempF, 2);
      Serial.println();
  
      J *req = notecard.newRequest("note.add");
      if (req != NULL) {
          JAddBoolToObject(req, "sync", true);
          JAddStringToObject(req, "file", "env.qo");
          J *body = JCreateObject();
          if (body != NULL) {
              JAddNumberToObject(body, "temp", tempF);
              JAddNumberToObject(body, "humidity", humid);
              JAddNumberToObject(body, "pressure", pressure);
              JAddNumberToObject(body, "altitude", alt);
              JAddItemToObject(req, "body", body);
          }
          notecard.sendRequest(req);
      }
    } 
    
    if(airQualitySensorAvailable) {
      if (airQualitySensor.dataAvailable()) {
        airQualitySensor.readAlgorithmResults();

        Serial.print("CO2[");
        //Returns calculated CO2 reading
        Serial.print(airQualitySensor.getCO2());
        Serial.print("] tVOC[");
        //Returns calculated TVOC reading
        Serial.print(airQualitySensor.getTVOC());
        Serial.print("] millis[");
        //Display the time since program start
        Serial.print(millis());
        Serial.print("]");
        Serial.println();
        
        J *req = notecard.newRequest("note.add");
        if (req != NULL) {
            JAddBoolToObject(req, "sync", true);
            JAddStringToObject(req, "file", "air.qo");
            J *body = JCreateObject();
            if (body != NULL) {
                JAddNumberToObject(body, "co2", airQualitySensor.getCO2());
                JAddNumberToObject(body, "tvoc", airQualitySensor.getTVOC());
                JAddItemToObject(req, "body", body);
            }
            notecard.sendRequest(req);
        }
      }
    }
    
    startMillis = currentMillis;
  }
}
