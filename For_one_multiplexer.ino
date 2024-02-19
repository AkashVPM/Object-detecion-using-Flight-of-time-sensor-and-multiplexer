#include <Wire.h>
#include <Adafruit_VL6180X.h>
#include <Adafruit_NeoPixel.h>

#define TCAADDR 0x70
#define LED_PIN 4
#define PIN1 8

Adafruit_VL6180X vl1 = Adafruit_VL6180X();
Adafruit_VL6180X vl2 = Adafruit_VL6180X();
Adafruit_VL6180X vl3 = Adafruit_VL6180X();

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, LED_PIN, NEO_GRB + NEO_KHZ800);

int scanned_data = 0;
int current_comp = 0;
int update_comp = 90;
int no_of_comp = 5;

void tcaselect(uint8_t channel) {
  if (channel > 7)
    return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void light(int scanned_data, int current_comp, int no_of_comp) {
  uint32_t color = 0;

  if (scanned_data != current_comp) {
    Serial.println("in comming");
    color = strip.Color(255, 255, 255);
    digitalWrite(PIN1, HIGH);
  } else {
    digitalWrite(PIN1, LOW);
    
    if (current_comp == 0)
      {Serial.println("no comp");
      color = strip.Color(255, 0, 0); // red
          }
    else if (no_of_comp == current_comp)
      {Serial.println("all comp");
      color = strip.Color(0, 255, 0); // green 
      }
    else if (no_of_comp != current_comp >= 2)
      {Serial.println("min comp");
        color = strip.Color(255, 255, 0); // yellow
      }

  }

  strip.fill(color, 0, strip.numPixels());
  strip.show();
}

int readExcelData() {
  int value = 0;
  while (Serial.available() > 0) {
    char digit = Serial.read(); // Read the next character
    if (isdigit(digit)) { // Check if the character is a digit
      value = value * 10 + (digit - '0'); // Construct the integer value
    } else if (digit == '\n') { // End of number
      return value;
    }
  }
  return 0; // No complete number received yet
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Wire.begin();

  tcaselect(0);
  vl1.begin();

  tcaselect(1);
  vl2.begin();

  tcaselect(2);
  vl3.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(PIN1, OUTPUT);

  strip.begin();
  strip.setBrightness(50);
  strip.show();
}

void loop() {
  scanned_data = readExcelData(); // Read integer value from serial input

  tcaselect(1);
  uint8_t range1 = vl2.readRange();
  uint8_t status1 = vl2.readRangeStatus();

  if (status1 == VL6180X_ERROR_NONE && range1 <= 85) {
    if (update_comp >= range1 && update_comp - range1 >= 5) {
      current_comp++;
      update_comp = range1;
    }
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(PIN1, HIGH);
    Serial.print("Scanned:");
    Serial.println(scanned_data);
    Serial.print("No_Of_Component: ");
    Serial.println(current_comp);
    Serial.print("range: ");
    Serial.println(range1);
    light(scanned_data, current_comp, no_of_comp);
  } else {
    current_comp = 0;
    update_comp = range1;
    Serial.print("No_Of_Component: ");
    Serial.println(current_comp);
    light(scanned_data, current_comp, no_of_comp);
  }

  delay(1000);
}