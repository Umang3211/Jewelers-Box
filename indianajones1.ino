//Made by Umang Sharma
#include <Keypad.h>
#include <Wire.h>
#include "HX711.h"

// Define keypad pins
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; // Connect to the column pinouts of the keypad

// Initialize keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define passcode
const String passcode = "1234#"; //You can change this to whatever you like

// Define HX711 pins
//const int LOADCELL_DOUT_PIN = A1;
//const int LOADCELL_SCK_PIN = A0;
const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 22;

// Define buzzer and LED pins
const int BUZZER_PIN = 10;
const int LED_PIN = 13;

// Define weight threshold
const int WEIGHT_THRESHOLD = 150; // Change this value according to your specific setup

// Initialize HX711 library
HX711 scale;
float scaleFactor = 0.00963211521;

// Variable to keep track of entered passcode
String enteredPasscode = "";
bool systemArmed = false;

void setup() {
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() 
{
  char key = keypad.getKey();
  
  if (key != NO_KEY) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    
    if (systemArmed) {
      if (key == passcode[enteredPasscode.length()]) {
        enteredPasscode += key;
      } else {
        enteredPasscode = "";
      }

      if (enteredPasscode == passcode) {
        // Passcode entered correctly, disarm the system
        digitalWrite(LED_PIN, LOW); // Turn off LED
        noTone(BUZZER_PIN); // Stop the tone
        enteredPasscode = ""; // Reset entered passcode
        systemArmed = false;
      }
    } else {
      if (key == '#') {
        // Arm the system
        digitalWrite(LED_PIN, HIGH); // Turn on LED
        tone(BUZZER_PIN, 1000, 1000); // Play a tone
        systemArmed = true;
      }
    }
  }

  if (systemArmed) {
    // Check weight sensor
    float weight = scale.get_units() * scaleFactor;
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" g");
    if (weight < WEIGHT_THRESHOLD) {
      // Trigger alarm
      digitalWrite(LED_PIN, LOW); // Turn off LED
      tone(BUZZER_PIN, 2000); // Play a different tone continuously
    } else {
      noTone(BUZZER_PIN); // Stop the tone
    }
  } else {
    noTone(BUZZER_PIN); // Stop the tone
}
}