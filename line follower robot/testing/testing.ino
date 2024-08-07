
// Ultrasonic Sensor Pin Defs
#define trigPin 3
#define echoPin 4
#define rightMotorPin1 10
#define rightMotorPin2 9
#define leftMotorPin1 7
#define leftMotorPin2 8
#define rightMotorEnable 5
#define leftMotorEnable 6
#define MIN_DISTANCE_CM 7

// Proximity Sensor Defs
#define rightProxSensorPin 12
#define leftProxSensorPin 11
#define readProxSensor(v) !digitalRead(v)  // Macro to read proximity sensors and invert the logic for active low

// DC Motors Mode Defs
#define MOVE_HALT 0
#define MOVE_RIGHT 1
#define MOVE_LEFT 2
#define MOVE_FORWARD 3
#define MOVE_BACKWARD 4

void setup() {
  Serial.begin(115200); 

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  // Initialize motor control pins as outputs
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorEnable, OUTPUT);
  pinMode(leftMotorEnable, OUTPUT);

  // Set initial motor mode to 'no movement' and motors always enabled
  digitalWrite(rightMotorEnable, HIGH);
  digitalWrite(leftMotorEnable, HIGH);
  setMotorMode(1);
}

void loop() {
  int distance = getDistance();

  bool rightProxActive = readProxSensor(rightProxSensorPin);
  bool leftProxActive = readProxSensor(leftProxSensorPin);

  // Print sensor states for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Right Prox: ");
  Serial.print(rightProxActive);
  Serial.print(", Left Prox: ");
  Serial.println(leftProxActive);

  // Logic for robot movement
  if (distance < MIN_DISTANCE_CM) {
    setMotorMode(MOVE_HALT); 
  } else if (!rightProxActive && !leftProxActive) {
    setMotorMode(MOVE_HALT); 
  } else if (rightProxActive && leftProxActive) {
    setMotorMode(MOVE_FORWARD); 
  } else if (rightProxActive) {
    setMotorMode(MOVE_RIGHT); 
  } else if (leftProxActive) {
    setMotorMode(MOVE_LEFT);
  }

  delay(100); // Short delay for stability!!!

}


// Function to read distance from the ultrasonic sensor
int getDistance() {
  long duration;
  int distance;
  
  // Clear the trigPin by setting it LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set the trigPin HIGH for 10 microseconds to send a pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin. The duration will be the time it takes for the pulse to return.
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance (in cm) based on the speed of sound.
  distance = duration * 0.034 / 2;

  return distance; // Return the calculated distance
}

// Function to set motor movement mode
void setMotorMode(int mode) {
  switch (mode) {
    case MOVE_HALT: 
      digitalWrite(rightMotorPin1, LOW);
      digitalWrite(rightMotorPin2, LOW);
      digitalWrite(leftMotorPin1, LOW);
      digitalWrite(leftMotorPin2, LOW);
      break;

    case MOVE_RIGHT: 
      digitalWrite(rightMotorPin1, HIGH); 
      digitalWrite(rightMotorPin2, LOW);
      digitalWrite(leftMotorPin1, LOW); 
      digitalWrite(leftMotorPin2, HIGH);
      break;

    case MOVE_LEFT: 
      digitalWrite(rightMotorPin1, LOW); 
      digitalWrite(rightMotorPin2, HIGH);
      digitalWrite(leftMotorPin1, HIGH); 
      digitalWrite(leftMotorPin2, LOW);
      break;

    case MOVE_FORWARD: 
      digitalWrite(rightMotorPin1, LOW); 
      digitalWrite(rightMotorPin2, HIGH);
      digitalWrite(leftMotorPin1, LOW); 
      digitalWrite(leftMotorPin2, HIGH);
      break;

    case MOVE_BACKWARD: 
      digitalWrite(rightMotorPin1, HIGH); 
      digitalWrite(rightMotorPin2, LOW);
      digitalWrite(leftMotorPin1, HIGH); 
      digitalWrite(leftMotorPin2, LOW);
      break;

    default:
      // Invalid mode, turn off all motors
      digitalWrite(rightMotorPin1, LOW);
      digitalWrite(rightMotorPin2, LOW);
      digitalWrite(leftMotorPin1, LOW);
      digitalWrite(leftMotorPin2, LOW);
      break;

  }
}