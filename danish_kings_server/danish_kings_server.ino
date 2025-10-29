#include <ArduinoBLE.h>

// #define DEBUG true

#define WHITE_DELAY_PIN A0
#define RED_DELAY_PIN A1
#define BLUE_DELAY_PIN A2
#define GREEN_DELAY_PIN A3

#define WHITE_POWER_PIN 3
#define RED_POWER_PIN 4
#define BLUE_POWER_PIN 7
#define GREEN_POWER_PIN 8
#define STATUS_PIN 13

const char* SERVICE_ID = "05f69d94-0000-4ab4-b404-ff694e970625";
const char* WHITE_LIGHT_CHARACTERISTIC = "05f69d94-0001-4ab4-b404-ff694e970625";
const char* WHITE_DELAY_CHARACTERISTIC = "05f69d94-0002-4ab4-b404-ff694e970625";
const char* RED_LIGHT_CHARACTERISTIC = "05f69d94-0003-4ab4-b404-ff694e970625";
const char* RED_DELAY_CHARACTERISTIC = "05f69d94-0004-4ab4-b404-ff694e970625";
const char* BLUE_LIGHT_CHARACTERISTIC = "05f69d94-0005-4ab4-b404-ff694e970625";
const char* BLUE_DELAY_CHARACTERISTIC = "05f69d94-0006-4ab4-b404-ff694e970625";
const char* GREEN_LIGHT_CHARACTERISTIC = "05f69d94-0007-4ab4-b404-ff694e970625";
const char* GREEN_DELAY_CHARACTERISTIC = "05f69d94-0008-4ab4-b404-ff694e970625";

void setup() {
  // set the pin modes, all inputs
  pinMode(WHITE_DELAY_PIN, INPUT);
  pinMode(RED_DELAY_PIN, INPUT);
  pinMode(BLUE_DELAY_PIN, INPUT);
  pinMode(GREEN_DELAY_PIN, INPUT);
  pinMode(WHITE_POWER_PIN, INPUT);
  pinMode(RED_POWER_PIN, INPUT);
  pinMode(BLUE_POWER_PIN, INPUT);
  pinMode(GREEN_POWER_PIN, INPUT);

  digitalWrite(WHITE_DELAY_PIN, INPUT_PULLDOWN);
  digitalWrite(RED_DELAY_PIN, INPUT_PULLDOWN);
  digitalWrite(BLUE_DELAY_PIN, INPUT_PULLDOWN);
  digitalWrite(GREEN_DELAY_PIN, INPUT_PULLDOWN);
  digitalWrite(WHITE_POWER_PIN, INPUT_PULLDOWN);
  digitalWrite(RED_POWER_PIN, INPUT_PULLDOWN);
  digitalWrite(BLUE_POWER_PIN, INPUT_PULLDOWN);
  digitalWrite(GREEN_POWER_PIN, INPUT_PULLDOWN);

  pinMode(STATUS_PIN, OUTPUT);

  #ifdef DEBUG
    Serial.begin(9600);
    while(!Serial) {}
    Serial.println("Initializing bluetooth...");
  #endif

  // begin bluetooth connection
  if (!BLE.begin()) {
    while (1) {
      digitalWrite(STATUS_PIN, LOW);
      delay(500);
      digitalWrite(STATUS_PIN, HIGH);
      delay(500);
    }
  }

  #ifdef DEBUG
    Serial.println("Bluetooth initialized");
  #endif

  BLE.setLocalName("Nano 33 BLE (Central)");
  BLE.advertise();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(STATUS_PIN, HIGH);
  #ifdef DEBUG
  Serial.println("Ready for pairing");
  #endif
  connectToPeripheral();
}

void connectToPeripheral() {
  BLEDevice peripheral;

  do {
    BLE.scanForUuid(SERVICE_ID);
    peripheral = BLE.available();
  } while (!peripheral);

  if (peripheral) {
    BLE.stopScan();
    controlPeripheral(peripheral);
  }
}

uint16_t transform(int reading) {
  double floated = static_cast<double>(reading);
  floated = floated * 4750 / 1024 + 250;
  uint16_t result = static_cast<uint16_t>(floated);
  // #ifdef DEBUG
  //   Serial.println(reading);
  //   Serial.println(result);
  // #endif
  return result;
}

void controlPeripheral(BLEDevice peripheral) {
  digitalWrite(STATUS_PIN, LOW);
  if (!peripheral.connect()) {
    return;
  }

  if (!peripheral.discoverAttributes()) {
    peripheral.disconnect();
    return;
  }

  #ifdef DEBUG
  Serial.println("Device connected");
  #endif

  BLECharacteristic white_light = peripheral.characteristic(WHITE_LIGHT_CHARACTERISTIC);
  BLECharacteristic white_delay = peripheral.characteristic(WHITE_DELAY_CHARACTERISTIC);
  BLECharacteristic red_light = peripheral.characteristic(RED_LIGHT_CHARACTERISTIC);
  BLECharacteristic red_delay = peripheral.characteristic(RED_DELAY_CHARACTERISTIC);
  BLECharacteristic blue_light = peripheral.characteristic(BLUE_LIGHT_CHARACTERISTIC);
  BLECharacteristic blue_delay = peripheral.characteristic(BLUE_DELAY_CHARACTERISTIC);
  BLECharacteristic green_light = peripheral.characteristic(GREEN_LIGHT_CHARACTERISTIC);
  BLECharacteristic green_delay = peripheral.characteristic(GREEN_DELAY_CHARACTERISTIC);

  if (!white_light || !white_delay 
      || !red_light || !red_delay
      || !blue_light || !blue_delay
      || !green_light || !green_delay) {
    #ifdef DEBUG
    Serial.println("Missing expected characteristic. Disconnecting...");
    #endif
    peripheral.disconnect();
  } else if (!white_light.canWrite()
          || !white_delay.canWrite()
          || !red_light.canWrite()
          || !red_delay.canWrite()
          || !blue_light.canWrite()
          || !blue_delay.canWrite()
          || !green_light.canWrite()
          || !green_delay.canWrite()) {
    Serial.println("Some characteristic is not writable. Disconnecting...");
    peripheral.disconnect();
  }

  while (peripheral.connected()) {
    bool white_value = HIGH == digitalRead(WHITE_POWER_PIN);
    white_light.writeValue(static_cast<int8_t>(white_value));
    bool red_value = HIGH == digitalRead(RED_POWER_PIN);
    red_light.writeValue(static_cast<int8_t>(red_value));
    bool blue_value = HIGH == digitalRead(BLUE_POWER_PIN);
    blue_light.writeValue(static_cast<int8_t>(blue_value));
    bool green_value = HIGH == digitalRead(GREEN_POWER_PIN);
    green_light.writeValue(static_cast<int8_t>(green_value));
    #ifdef DEBUG
    Serial.print("Writing white power value ");
    Serial.println(white_value);
    Serial.print("Writing red power value ");
    Serial.println(red_value);
    Serial.print("Writing blue power value ");
    Serial.println(blue_value);
    Serial.print("Writing green power value ");
    Serial.println(green_value);
    #endif

    uint16_t white_delay_value = transform(analogRead(WHITE_DELAY_PIN));
    white_delay.writeValue(white_delay_value);
    uint16_t red_delay_value = transform(analogRead(RED_DELAY_PIN));
    red_delay.writeValue(red_delay_value);
    uint16_t blue_delay_value = transform(analogRead(BLUE_DELAY_PIN));
    blue_delay.writeValue(blue_delay_value);
    uint16_t green_delay_value = transform(analogRead(GREEN_DELAY_PIN));
    green_delay.writeValue(green_delay_value);
    #ifdef DEBUG
    Serial.print("Writing white delay value ");
    Serial.println(white_delay_value);
    Serial.print("Writing red delay value ");
    Serial.println(red_delay_value);
    Serial.print("Writing blue delay value ");
    Serial.println(blue_delay_value);
    Serial.print("Writing green delay value ");
    Serial.println(green_delay_value);
    #endif
    delay(100);
  }
}