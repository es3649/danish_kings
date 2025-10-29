#include <ArduinoBLE.h>

// #define DEBUG true

#define WHITE_LED_PIN 3
#define RED_LED_PIN 4
#define BLUE_LED_PIN 5
#define GREEN_LED_PIN 6

const char* SERVICE_ID = "05f69d94-0000-4ab4-b404-ff694e970625";
const char* WHITE_LIGHT_CHARACTERISTIC = "05f69d94-0001-4ab4-b404-ff694e970625";
const char* WHITE_DELAY_CHARACTERISTIC = "05f69d94-0002-4ab4-b404-ff694e970625";
const char* RED_LIGHT_CHARACTERISTIC = "05f69d94-0003-4ab4-b404-ff694e970625";
const char* RED_DELAY_CHARACTERISTIC = "05f69d94-0004-4ab4-b404-ff694e970625";
const char* BLUE_LIGHT_CHARACTERISTIC = "05f69d94-0005-4ab4-b404-ff694e970625";
const char* BLUE_DELAY_CHARACTERISTIC = "05f69d94-0006-4ab4-b404-ff694e970625";
const char* GREEN_LIGHT_CHARACTERISTIC = "05f69d94-0007-4ab4-b404-ff694e970625";
const char* GREEN_DELAY_CHARACTERISTIC = "05f69d94-0008-4ab4-b404-ff694e970625";

BLEService controlService(SERVICE_ID);
// on/off characteristics
BLEBooleanCharacteristic whiteLightCharacteristic(WHITE_LIGHT_CHARACTERISTIC, BLEWrite);
BLEBooleanCharacteristic redLightCharacteristic(RED_LIGHT_CHARACTERISTIC, BLEWrite);
BLEBooleanCharacteristic blueLightCharacteristic(BLUE_LIGHT_CHARACTERISTIC, BLEWrite);
BLEBooleanCharacteristic greenLightCharacteristic(GREEN_LIGHT_CHARACTERISTIC, BLEWrite);
// delay rate characteristics
BLEUnsignedShortCharacteristic whiteDelayCharacteristic(WHITE_DELAY_CHARACTERISTIC, BLEWrite);
BLEUnsignedShortCharacteristic redDelayCharacteristic(RED_DELAY_CHARACTERISTIC, BLEWrite);
BLEUnsignedShortCharacteristic blueDelayCharacteristic(BLUE_DELAY_CHARACTERISTIC, BLEWrite);
BLEUnsignedShortCharacteristic greenDelayCharacteristic(GREEN_DELAY_CHARACTERISTIC, BLEWrite);

BLEDevice central;

// global values for on/off toggle
bool white_on = true;
bool red_on = true;
bool blue_on = true;
bool green_on = true;

// global values for delay time (ms)
uint16_t white_delay = 1000;
uint16_t red_delay = 1000;
uint16_t blue_delay = 1000;
uint16_t green_delay = 1000;

void connect() {
  // if not initialized: initialize
  BLE.advertise();

  // delay the STATUS LED quickly while awaiting a connection
  do {
    central = BLE.central();
    digitalWrite(WHITE_LED_PIN, !digitalRead(WHITE_LED_PIN));
    delay(500);
  } while (!central);

  BLE.stopAdvertise();

  #ifdef DEBUG
    Serial.println("Connected to central device!");
    Serial.print("MAC Address: ");
    Serial.println(central.address());
  #endif
}

void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
    Serial.begin(9600);
    while (!Serial) {}
    Serial.println("Started...");
  #endif

  // set pin modes
  pinMode(WHITE_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // write initial values
  digitalWrite(WHITE_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  #ifdef DEBUG
    Serial.println("Starting Bluetooth® Low Energy");
  #endif

  if(!BLE.begin()) {
  #ifdef DEBUG
      Serial.println("Failed to start Bluetooth®!");
  #endif
    while (1) {
      digitalWrite(WHITE_LED_PIN, HIGH);
      delay(500);
      digitalWrite(WHITE_LED_PIN, LOW);
      delay(500);
    }
  }

  BLE.setLocalName("WizardTower");
  BLE.setAdvertisedService(controlService);
  controlService.addCharacteristic(whiteLightCharacteristic);
  controlService.addCharacteristic(whiteDelayCharacteristic);
  controlService.addCharacteristic(redLightCharacteristic);
  controlService.addCharacteristic(redDelayCharacteristic);
  controlService.addCharacteristic(blueLightCharacteristic);
  controlService.addCharacteristic(blueDelayCharacteristic);
  controlService.addCharacteristic(greenLightCharacteristic);
  controlService.addCharacteristic(greenDelayCharacteristic);
  BLE.addService(controlService);

  connect();
}

void updateWhite() {
  static int last_white_swap = 0;
  int now = millis();
  PinStatus led_status = digitalRead(WHITE_LED_PIN);
  int diff = now - last_white_swap;
  if (diff > white_delay) {
    digitalWrite(WHITE_LED_PIN, !led_status);
    last_white_swap = now;
  }
}

void updateRed() {
  static int last_red_swap = 0;
  int now = millis();
  PinStatus led_status = digitalRead(RED_LED_PIN);
  int diff = now - last_red_swap;
  if (diff > red_delay) {
    digitalWrite(RED_LED_PIN, !led_status);
    last_red_swap = now;
  }
}

void updateBlue() {
  static int last_blue_swap = 0;
  int now = millis();
  PinStatus led_status = digitalRead(BLUE_LED_PIN);
  int diff = now - last_blue_swap;
  if (diff > blue_delay) {
    digitalWrite(BLUE_LED_PIN, !led_status);
    last_blue_swap = now;
  }
}

void updateGreen() {
  static int last_green_swap = 0;
  int now = millis();
  PinStatus led_status = digitalRead(GREEN_LED_PIN);
  int diff = now - last_green_swap;
  if (diff > green_delay) {
    digitalWrite(GREEN_LED_PIN, !led_status);
    last_green_swap = now;
  }
}

void updateBLEValues() {
  white_on = whiteLightCharacteristic.value();
  red_on = redLightCharacteristic.value();
  blue_on = blueLightCharacteristic.value();
  green_on = greenLightCharacteristic.value();

  #ifdef DEBUG
    Serial.print("Reading white power value ");
    Serial.println(white_on);
    Serial.print("Reading red power value ");
    Serial.println(red_on);
    Serial.print("Reading blue power value ");
    Serial.println(blue_on);
    Serial.print("Reading green power value ");
    Serial.println(green_on);
  #endif

  if (white_on) {
    white_delay = whiteDelayCharacteristic.value();
    #ifdef DEBUG
      Serial.print("Reading white delay value ");
      Serial.println(white_delay);
    #endif
    updateWhite();
  } else {
    digitalWrite(WHITE_LED_PIN, LOW);
  }

  if (red_on) {
    red_delay = redDelayCharacteristic.value();
    #ifdef DEBUG
      Serial.print("Reading red delay value ");
      Serial.println(red_delay);
    #endif
    updateRed();
  } else {
    digitalWrite(RED_LED_PIN, LOW);
  }

  if (blue_on) {
    blue_delay = blueDelayCharacteristic.value();
    #ifdef DEBUG
      Serial.print("Reading blue delay value ");
      Serial.println(blue_delay);
    #endif
    updateBlue();
  } else {
    digitalWrite(BLUE_LED_PIN, LOW);
  }

  if (green_on) {
    green_delay = greenDelayCharacteristic.value();
    #ifdef DEBUG
      Serial.print("Reading green delay value ");
      Serial.println(green_delay);
    #endif
    updateGreen();
  } else {
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!central.connected()) {
    connect();
  } else {
    updateBLEValues();
  }
}
