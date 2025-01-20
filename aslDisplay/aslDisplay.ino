#include <LiquidCrystal.h>

char letters[16] = {}; // Buffer for 16 characters
char current[16] = {}; // Buffer for 16 characters
const int debounceDelay = 50; // milliseconds for button debounce
unsigned long lastConfirmPress = 0; // Debounce timer
bool lastConfirmState = LOW;

float confirmButtonState = 0;
float resetButtonState = 0;

const int confirmButtonPin = 10;
const int resetButtonPin = 9;

const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int displayIndex = 0; // Current position in letters
char input;           // Current input from Serial

// Reset button handling
bool displayOnly = false; // Tracks the current mode: false = input mode, true = display mode
bool resetButtonPressed = false;

void setup() {
  // Initialize letters to spaces
for (int i = 0; i < 16; i++) {
    letters[i] = ' ';
    current[i] = ' ';
  }
  Serial.begin(9600);
  Serial.setTimeout(1);
  pinMode(confirmButtonPin, INPUT);
  pinMode(resetButtonPin, INPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print(letters); // Print the initial empty buffer
}

void loop() {
  // Read button states
  confirmButtonState = digitalRead(confirmButtonPin);
  resetButtonState = digitalRead(resetButtonPin);

  // Handle reset button
  if (resetButtonState == HIGH) {
    lcd.setCursor(0, 0);
    letters[displayIndex] = ' ';
    lcd.print(letters);

    for (int i = 0; i < 16; i++) {
      current[i] = ' ';
    }
    lcd.setCursor(0, 1);
    lcd.print(current);
    delay(500);
    resetButtonState = LOW;
    while (resetButtonState == LOW) {
      //lcd.setCursor(0, 0);
      //letters[0] = '!';
      //lcd.print(letters);
      resetButtonState = digitalRead(resetButtonPin);
    }
    delay(500);
    resetButtonState = LOW;
    for (int j = 0; j < 16; j++) {
      letters[j] = ' ';
    }
    for (int i = 0; i < 16; i++) {
      current[i] = ' ';
    }
    displayIndex = 0;
    lcd.clear();
    lcd.setCursor(0, 0); // Reset cursor
    //lcd.print(letters); // Print cleared buffer

  }
    
  // Handle serial input
  if (Serial.available()) {
    Serial.flush();
    input = Serial.read(); // Read one character
    if (displayIndex < 16) {
      letters[displayIndex] = input; // Store input at current position
      current[displayIndex] = '-';
    }
  }

  // Handle confirm button with debounce
  if (confirmButtonState == HIGH && !lastConfirmState) {
    if (millis() - lastConfirmPress > debounceDelay) {
      lastConfirmPress = millis(); // Update debounce timer
      lastConfirmState = HIGH;     // Register button press

      current[displayIndex] = ' ';
      displayIndex++; // Move to next character position
      if (displayIndex >= 16) {
        displayIndex = 0; // Prevent overflow
      }
    }
  } else if (confirmButtonState == LOW) {
    lastConfirmState = LOW; // Reset state when button is released
  }

  // Update LCD only when necessary
  lcd.setCursor(0, 0);
  lcd.print(letters); // Print all characters
  lcd.setCursor(0, 1);
  lcd.print(current);
}