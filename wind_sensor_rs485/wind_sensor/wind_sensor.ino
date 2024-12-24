// Wind Speed Sensor PR-3000-FSJT-N01

// --------------------- Define Pins ------------------------

//   [Sensor Pin]   --->   [RS485 to TTL Pin]
//    Brown         --->    10-30V DC+ (Power Supply)
//    Black         --->    GND DC-    (Power Supply)
//    Green         --->    A+
//    Blue          --->    B-
//
//   [ESP32 Pin]    --->   [RS485 to TTL Pin]
//    VIN           --->    Vcc
//    TX2           --->    TXD
//    RX2           --->    RXD
//    GND           --->    GND

//------------------------------------------------------------

#define RXD2 16   // ESP32 Pin [RX2]
#define TXD2 17   // ESP32 Pin [TX2]
byte ByteArray[250];
int ByteData[20];

void setup() {
  Serial.begin(9600);
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  readWindSpeesSensor();
}

void readWindSpeesSensor() {
  delay(200);
  byte msgfs[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
  int i;
  int len = 8;
  for (i = 0; i < len; i++) {
    Serial2.write(msgfs[i]);
  }
  len = 0;
  int a = 0;
  while (Serial2.available()) {
    ByteArray[a] = Serial2.read();
    a++;
  }
  int b = 0;
  String registros;
  for (b = 0; b < a; b++) {
    registros = String(ByteArray[b], HEX);
  }
  ByteData[0] = ByteArray[3] * 256 + ByteArray[4];
  float winds;
  winds = ByteData[0] * 0.1;

  Serial.print("Wind Speed = ");
  Serial.print(winds);
  Serial.println(" m/s");
  //delay(50);//200
}

