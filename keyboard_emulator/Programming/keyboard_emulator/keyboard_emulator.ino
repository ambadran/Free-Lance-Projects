#include <Keyboard.h>

const int buttonPin = 9; // Button connected to digital pin 2

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up resistor
  Keyboard.begin();
}

void loop() {
  if (digitalRead(buttonPin) == LOW) { // Button pressed
    Keyboard.print('B'); // Types 'A' 
    delay(300); // Prevent spamming
  }
}

