/*
=========================================================
        VANGUARD
        Neonatal Jaundice Screening Prototype
=========================================================

BOARD:
  Arduino UNO

TCS3200:
  S0   -> D4
  S1   -> D5
  S2   -> D6
  S3   -> D7
  OUT  -> D8
  VCC  -> 5V
  GND  -> GND
  LED  -> 5V

OLED 0.96" SSD1306 128x64:
  SDA  -> A4
  SCL  -> A5
  VCC  -> 5V
  GND  -> GND
  I2C address = 0x3C

TRAFFIC LIGHT MODULE:
  GREEN  -> D9
  YELLOW -> D10
  RED    -> D11

BUTTON:
  One terminal -> D12
  Other terminal -> GND

BUZZER:
  Buzzer + -> D13
  Buzzer - -> GND

=========================================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ======================================================
// OLED CONFIGURATION
// ======================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ======================================================
// TCS3200 PINS
// ======================================================

#define TCS_S0 4
#define TCS_S1 5
#define TCS_S2 6
#define TCS_S3 7
#define TCS_OUT 8

// ======================================================
// TRAFFIC LIGHT
// ======================================================

#define GREEN_LED  9
#define YELLOW_LED 10
#define RED_LED    11

// ======================================================
// BUTTON
// ======================================================

#define BUTTON_PIN 12

// ======================================================
// BUZZER
// ======================================================

#define BUZZER_PIN 13

// ======================================================
// CALIBRATION / THRESHOLDS
// ======================================================

// Replace these with your calibrated VANGUARD values.

const float THRESH_LOW  = 0.80;
const float THRESH_HIGH = 1.20;

// ======================================================
// BASELINE
// ======================================================

// Baseline reference values.
// These can be replaced with the values obtained
// from your calibration process.

float baselineRed   = 1.0;
float baselineGreen = 1.0;
float baselineBlue  = 1.0;

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(9600);

  // ----------------------------------------------------
  // TCS3200
  // ----------------------------------------------------

  pinMode(TCS_S0, OUTPUT);
  pinMode(TCS_S1, OUTPUT);
  pinMode(TCS_S2, OUTPUT);
  pinMode(TCS_S3, OUTPUT);
  pinMode(TCS_OUT, INPUT);

  // TCS3200 frequency scaling = 20%
  digitalWrite(TCS_S0, HIGH);
  digitalWrite(TCS_S1, LOW);

  // ----------------------------------------------------
  // Traffic light
  // ----------------------------------------------------

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // ----------------------------------------------------
  // Button
  // ----------------------------------------------------

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // ----------------------------------------------------
  // Buzzer
  // ----------------------------------------------------

  pinMode(BUZZER_PIN, OUTPUT);

  // ----------------------------------------------------
  // Turn everything off
  // ----------------------------------------------------

  allLightsOff();
  digitalWrite(BUZZER_PIN, LOW);

  // ----------------------------------------------------
  // OLED
  // ----------------------------------------------------

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
      )) {

    Serial.println("OLED FAILED");

    while (true) {
      // Stop here if OLED is not detected
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  showStartup();

  delay(2000);

  showReady();

  // ----------------------------------------------------
  // Serial information
  // ----------------------------------------------------

  Serial.println();
  Serial.println("================================");
  Serial.println("       VANGUARD SYSTEM");
  Serial.println("================================");
  Serial.println("Arduino UNO : OK");
  Serial.println("TCS3200     : READY");
  Serial.println("OLED        : READY");
  Serial.println("Button      : READY");
  Serial.println("Buzzer      : READY");
  Serial.println("================================");
}

// ======================================================
// MAIN LOOP
// ======================================================

void loop() {

  // Button uses INPUT_PULLUP:
  // LOW = pressed

  if (digitalRead(BUTTON_PIN) == LOW) {

    delay(50);

    if (digitalRead(BUTTON_PIN) == LOW) {

      performMeasurement();

      // Wait until button is released
      while (digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }

      delay(500);
    }
  }
}

// ======================================================
// STARTUP SCREEN
// ======================================================

void showStartup() {

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(18, 5);
  display.println("VANGUARD");

  display.setTextSize(1);

  display.setCursor(22, 30);
  display.println("Jaundice");

  display.setCursor(30, 42);
  display.println("Screener");

  display.setCursor(25, 55);
  display.println("Initializing");

  display.display();
}

// ======================================================
// READY SCREEN
// ======================================================

void showReady() {

  allLightsOff();

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(20, 4);
  display.println("VANGUARD");

  display.setTextSize(1);

  display.setCursor(24, 27);
  display.println("Place probe");

  display.setCursor(27, 39);
  display.println("on skin");

  display.setCursor(18, 53);
  display.println("Press BUTTON");

  display.display();

  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("READY FOR MEASUREMENT");
  Serial.println("--------------------------------");
}

// ======================================================
// MEASUREMENT
// ======================================================

void performMeasurement() {

  allLightsOff();

  // ----------------------------------------------------
  // Measurement screen
  // ----------------------------------------------------

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(13, 5);
  display.println("MEASURE");

  display.setTextSize(1);

  display.setCursor(20, 31);
  display.println("Hold probe steady");

  display.setCursor(25, 44);
  display.println("Please wait...");

  display.display();

  Serial.println();
  Serial.println("================================");
  Serial.println("     STARTING MEASUREMENT");
  Serial.println("================================");

  delay(1000);

  // ----------------------------------------------------
  // Take multiple readings
  // ----------------------------------------------------

  float red   = getAverageRed();
  float green = getAverageGreen();
  float blue  = getAverageBlue();

  // ----------------------------------------------------
  // Prevent divide-by-zero
  // ----------------------------------------------------

  if (red <= 0.01) {
    red = 0.01;
  }

  // ----------------------------------------------------
  // Calculate yellowness
  //
  // Basic formula:
  //
  //     (G - B) / R
  //
  // ----------------------------------------------------

  float yellowness =
    (green - blue) / red;

  // ----------------------------------------------------
  // Baseline ratio
  // ----------------------------------------------------

  float baselineAdjusted =
    yellowness /
    (
      (baselineGreen - baselineBlue)
      /
      baselineRed
    );

  // Safety against invalid baseline
  if (isnan(baselineAdjusted) ||
      isinf(baselineAdjusted)) {

    baselineAdjusted = yellowness;
  }

  // ----------------------------------------------------
  // Serial output
  // ----------------------------------------------------

  Serial.println();
  Serial.println("RGB VALUES");
  Serial.println("------------------------------");

  Serial.print("RED   : ");
  Serial.println(red, 3);

  Serial.print("GREEN : ");
  Serial.println(green, 3);

  Serial.print("BLUE  : ");
  Serial.println(blue, 3);

  Serial.println("------------------------------");

  Serial.print("YELLOWNESS       : ");
  Serial.println(yellowness, 3);

  Serial.print("BASELINE ADJUSTED: ");
  Serial.println(baselineAdjusted, 3);

  Serial.println("------------------------------");

  // ----------------------------------------------------
  // Classification
  // ----------------------------------------------------

  if (baselineAdjusted < THRESH_LOW) {

    showGreenResult(
      baselineAdjusted
    );

  }

  else if (baselineAdjusted < THRESH_HIGH) {

    showYellowResult(
      baselineAdjusted
    );

  }

  else {

    showRedResult(
      baselineAdjusted
    );
  }
}

// ======================================================
// AVERAGE RED
// ======================================================

float getAverageRed() {

  float total = 0;

  const int samples = 5;

  for (int i = 0; i < samples; i++) {

    total += readRed();

    delay(50);
  }

  return total / samples;
}

// ======================================================
// AVERAGE GREEN
// ======================================================

float getAverageGreen() {

  float total = 0;

  const int samples = 5;

  for (int i = 0; i < samples; i++) {

    total += readGreen();

    delay(50);
  }

  return total / samples;
}

// ======================================================
// AVERAGE BLUE
// ======================================================

float getAverageBlue() {

  float total = 0;

  const int samples = 5;

  for (int i = 0; i < samples; i++) {

    total += readBlue();

    delay(50);
  }

  return total / samples;
}

// ======================================================
// READ RED
// ======================================================

float readRed() {

  // Red filter
  digitalWrite(TCS_S2, LOW);
  digitalWrite(TCS_S3, LOW);

  delay(50);

  unsigned long pulse =
    pulseIn(
      TCS_OUT,
      LOW,
      100000
    );

  if (pulse == 0) {
    return 0;
  }

  return 1000000.0 / pulse;
}

// ======================================================
// READ BLUE
// ======================================================

float readBlue() {

  // Blue filter
  digitalWrite(TCS_S2, LOW);
  digitalWrite(TCS_S3, HIGH);

  delay(50);

  unsigned long pulse =
    pulseIn(
      TCS_OUT,
      LOW,
      100000
    );

  if (pulse == 0) {
    return 0;
  }

  return 1000000.0 / pulse;
}

// ======================================================
// READ GREEN
// ======================================================

float readGreen() {

  // Green filter
  digitalWrite(TCS_S2, HIGH);
  digitalWrite(TCS_S3, HIGH);

  delay(50);

  unsigned long pulse =
    pulseIn(
      TCS_OUT,
      LOW,
      100000
    );

  if (pulse == 0) {
    return 0;
  }

  return 1000000.0 / pulse;
}

// ======================================================
// GREEN RESULT
// ======================================================

void showGreenResult(float value) {

  allLightsOff();

  digitalWrite(GREEN_LED, HIGH);

  // Green = no buzzer
  digitalWrite(BUZZER_PIN, LOW);

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(31, 4);
  display.println("GREEN");

  display.setTextSize(1);

  display.setCursor(20, 28);
  display.println("Low indication");

  display.setCursor(24, 40);

  display.print("Value: ");
  display.println(value, 2);

  display.setCursor(17, 54);
  display.println("Screening result");

  display.display();

  Serial.println();
  Serial.println("RESULT = GREEN");
  Serial.println("LEVEL  = LOW");

  delay(3000);

  showReady();
}

// ======================================================
// YELLOW RESULT
// ======================================================

void showYellowResult(float value) {

  allLightsOff();

  digitalWrite(YELLOW_LED, HIGH);

  // ----------------------------------------------------
  // One buzzer beep for YELLOW
  // ----------------------------------------------------

  tone(BUZZER_PIN, 2000);

  delay(200);

  noTone(BUZZER_PIN);

  // ----------------------------------------------------
  // OLED
  // ----------------------------------------------------

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(20, 4);
  display.println("YELLOW");

  display.setTextSize(1);

  display.setCursor(30, 28);
  display.println("Moderate");

  display.setCursor(24, 40);

  display.print("Value: ");
  display.println(value, 2);

  display.setCursor(10, 54);
  display.println("Check recommended");

  display.display();

  Serial.println();
  Serial.println("RESULT = YELLOW");
  Serial.println("LEVEL  = MODERATE");

  delay(3000);

  showReady();
}

// ======================================================
// RED RESULT
// ======================================================

void showRedResult(float value) {

  allLightsOff();

  digitalWrite(RED_LED, HIGH);

  // ----------------------------------------------------
  // RED = warning buzzer
  // ----------------------------------------------------

  tone(BUZZER_PIN, 2500);

  delay(500);

  noTone(BUZZER_PIN);

  // ----------------------------------------------------
  // OLED
  // ----------------------------------------------------

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(40, 4);
  display.println("RED");

  display.setTextSize(1);

  display.setCursor(25, 28);
  display.println("High indication");

  display.setCursor(24, 40);

  display.print("Value: ");
  display.println(value, 2);

  display.setCursor(10, 54);
  display.println("Check recommended");

  display.display();

  Serial.println();
  Serial.println("RESULT = RED");
  Serial.println("LEVEL  = HIGH");

  delay(3000);

  showReady();
}

// ======================================================
// TURN OFF ALL LIGHTS
// ======================================================

void allLightsOff() {

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
}

// ======================================================
// END OF VANGUARD
// ======================================================
