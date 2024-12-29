#include <Keyboard.h>

const int buttonPin0 = 21;
const int buttonPin1 = 20;
const int buttonPin2 = 2;
const int buttonPin3 = 3;

void setup() {
  pinMode(buttonPin0, INPUT_PULLUP); 
  pinMode(buttonPin1, INPUT_PULLUP); 
  pinMode(buttonPin2, INPUT_PULLUP); 
  pinMode(buttonPin3, INPUT_PULLUP); 
  Keyboard.begin();
}

void loop() {
  if (digitalRead(buttonPin0) == LOW) { 
    Keyboard.print(' ');
    delay(300); 
  }
  if (digitalRead(buttonPin1) == LOW) { 
    Keyboard.press(KEY_RETURN);
    delay(100); 
    Keyboard.releaseAll(); 
    delay(300); 
  }
  if (digitalRead(buttonPin2) == LOW) { 
    Keyboard.press(KEY_UP_ARROW); // Simulate Up Arrow
    delay(100);
    Keyboard.releaseAll();
    delay(300); 
  }
  if (digitalRead(buttonPin3) == LOW) { 
    Keyboard.press(KEY_RIGHT_ARROW); // Simulate Right Arrow
    delay(100);
    Keyboard.releaseAll();
    delay(300); 
  }
}

