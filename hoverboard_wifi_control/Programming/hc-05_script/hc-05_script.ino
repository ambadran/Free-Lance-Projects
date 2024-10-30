#include <SoftwareSerial.h>
#define HC_05_BAUD 115200

SoftwareSerial HoverSerial(2,3);        // RX, TX
SoftwareSerial hc_05(4, 5);             // RX, TX

/******************** MOVE FUNCS *******************/
typedef enum {
  FORWARD = 0,
  BACKWARD = 1,
  RIGHT = 2,
  LEFT = 3,
  STOP = 4
} movement_state_t;
void move(movement_state_t movement_state) {
  switch(movement_state) {
    case FORWARD:
      Serial.println("moving FORWARD");
      // hoverboard send command

      break;

    case BACKWARD:
      Serial.println("moving BACKWARD");
      // hoverboard send command
      
      break;

    case RIGHT:
      Serial.println("moving RIGHT");
      // hoverboard send command
      
      break;

    case LEFT:
      Serial.println("moving LEFT");
      // hoverboard send command
      
      break;

    case STOP:
      Serial.println("moving STOP\n");
      // hoverboard send command
      
      break;
  }
}
/******************************************************/

/*************** CMD STUFF **********************/
char cmd;
bool await_cmd(char to_look_for) {
  while(!hc_05.available());
  return (to_look_for == hc_05.read());
}
/************************************************/

void setup() {
  
  hc_05.begin(HC_05_BAUD);
  Serial.begin(9600);

}

void loop() {

  if(hc_05.available()) {
    cmd = hc_05.read();

    switch(cmd) {

      case 'F':
        move(FORWARD);
        await_cmd('f');
        move(STOP);
        break;

      case 'B':
        move(BACKWARD);
        await_cmd('B');
        move(STOP);
        break;

      case 'R':
        move(RIGHT);
        await_cmd('r');
        move(STOP);
        break;

      case 'L':
        move(LEFT);
        await_cmd('l');
        move(STOP);
        break;

    }
  }
}
