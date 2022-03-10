#include <Notecard.h>
#include <Wire.h>

#define LOUDNESS_SENSOR_ADDRESS 0x38
#define COMMAND_GET_VALUE 0x05

#define PRODUCT_UID "com.blues.bsatrom:road_trip_tracker"
Notecard notecard;

void setup() {
  Serial.begin(115200);
  delay(2500);
  Serial.println("Blues Road Trip Tracker - Feather Edition");
  Serial.println("=========================================");
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Wire.begin();
  notecard.begin();

  J *req = notecard.newRequest("hub.set");
  JAddStringToObject(req, "product", PRODUCT_UID);
  JAddStringToObject(req, "sn", "feather-tracker");
  JAddStringToObject(req, "mode", "perodic");
  
  notecard.sendRequest(req);

  testForConnectivity();
  delay(1000);
  
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  uint16_t noise = get_noise_level();
  
  Serial.print("Noise Level:  ");
  Serial.println(noise,DEC);
  
  delay(1000);
}

uint16_t get_noise_level() {
  uint16_t ADC_VALUE = 0;

  Wire.beginTransmission(LOUDNESS_SENSOR_ADDRESS);
  Wire.write(COMMAND_GET_VALUE); // command for status
  Wire.endTransmission();

  Wire.requestFrom(LOUDNESS_SENSOR_ADDRESS, 2);

  while (Wire.available()) {
    uint8_t ADC_VALUE_L = Wire.read(); 
    uint8_t ADC_VALUE_H = Wire.read();
    ADC_VALUE=ADC_VALUE_H;
    ADC_VALUE<<=8;
    ADC_VALUE|=ADC_VALUE_L;
  }
  
  uint16_t x = Wire.read(); 

  // < 400 seems to be quiet
  // 400-700 some noise
  // 700+ is high noise
  return ADC_VALUE;
}

void testForConnectivity() {
  Wire.beginTransmission(LOUDNESS_SENSOR_ADDRESS);
  if (Wire.endTransmission() != 0) {
    Serial.println("Check connections. No Loudness sensor attached.");
    while (1);
  }

  Serial.println("Loudness sensor connected");
}
