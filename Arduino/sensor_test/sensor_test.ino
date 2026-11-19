/*
====================================================
        VANGUARD - TCS3200 SENSOR TEST
====================================================

Board: Arduino UNO

TCS3200 CONNECTIONS:

S0   -> D4
S1   -> D5
S2   -> D6
S3   -> D7
OUT  -> D8
VCC  -> 5V
GND  -> GND
LED  -> 5V

This program:
  1. Tests the TCS3200
  2. Reads RED, GREEN and BLUE
  3. Calculates yellowness
  4. Prints values to Serial Monitor

Serial Monitor:
  9600 baud
====================================================
*/

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define OUT 8


// ==================================================
// SETUP
// ==================================================

void setup() {

  Serial.begin(9600);

  // TCS3200 pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);

  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(OUT, INPUT);

  /*
    Frequency scaling:

    S0 = HIGH
    S1 = LOW

    This gives 20% output-frequency scaling.
  */

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);


  Serial.println();
  Serial.println("======================================");
  Serial.println("     VANGUARD TCS3200 SENSOR TEST");
  Serial.println("======================================");

  Serial.println();
  Serial.println("Sensor: TCS3200");
  Serial.println("Board : Arduino UNO");

  Serial.println();
  Serial.println("Connections:");
  Serial.println("S0  -> D4");
  Serial.println("S1  -> D5");
  Serial.println("S2  -> D6");
  Serial.println("S3  -> D7");
  Serial.println("OUT -> D8");

  Serial.println();
  Serial.println("Starting sensor test...");
  Serial.println();

  delay(2000);
}


// ==================================================
// MAIN LOOP
// ==================================================

void loop() {

  // Read RGB
  float red = readRed();
  float green = readGreen();
  float blue = readBlue();


  // ------------------------------------------------
  // Calculate yellowness
  //
  // Formula used in VANGUARD:
  //
  //       G - B
  // Y =   -------
  //         R
  //
  // ------------------------------------------------

  float yellowness;

  if (red > 0) {

    yellowness =
      (green - blue) / red;

  }

  else {

    yellowness = 0;
  }


  // =================================================
  // SERIAL OUTPUT
  // =================================================

  Serial.println("--------------------------------------");

  Serial.print("RED        : ");
  Serial.println(red, 3);

  Serial.print("GREEN      : ");
  Serial.println(green, 3);

  Serial.print("BLUE       : ");
  Serial.println(blue, 3);

  Serial.print("YELLOWNESS : ");
  Serial.println(yellowness, 3);

  Serial.println("--------------------------------------");


  // Simple sensor status

  if (red == 0 &&
      green == 0 &&
      blue == 0) {

    Serial.println("WARNING: No sensor signal!");

  }

  else {

    Serial.println("TCS3200: OK");
  }


  Serial.println();

  delay(1000);
}


// ==================================================
// READ RED
// ==================================================

float readRed() {

  /*
    RED FILTER

    S2 = LOW
    S3 = LOW
  */

  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  delay(50);

  unsigned long pulse =
    pulseIn(
      OUT,
      LOW,
      100000
    );


  if (pulse == 0) {

    return 0;
  }


  /*
    Convert pulse period
    to frequency.
  */

  float frequency =
    1000000.0 / pulse;


  return frequency;
}


// ==================================================
// READ GREEN
// ==================================================

float readGreen() {

  /*
    GREEN FILTER

    S2 = HIGH
    S3 = HIGH
  */

  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  delay(50);

  unsigned long pulse =
    pulseIn(
      OUT,
      LOW,
      100000
    );


  if (pulse == 0) {

    return 0;
  }


  float frequency =
    1000000.0 / pulse;


  return frequency;
}


// ==================================================
// READ BLUE
// ==================================================

float readBlue() {

  /*
    BLUE FILTER

    S2 = LOW
    S3 = HIGH
  */

  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  delay(50);

  unsigned long pulse =
    pulseIn(
      OUT,
      LOW,
      100000
    );


  if (pulse == 0) {

    return 0;
  }


  float frequency =
    1000000.0 / pulse;


  return frequency;
}
