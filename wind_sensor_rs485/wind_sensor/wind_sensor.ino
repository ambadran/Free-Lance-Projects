#include <DHT11.h>
#include <Wire.h>
#include <BH1750.h>

#define TXD2 23   
#define RXD2 19
#define DHT11PIN 32

byte ByteArray[250];
int ByteData[20];
int humidity = 0;
int temperature = 0;
float lux;

DHT11 dht11(DHT11PIN);
BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2);

  Wire.begin();
  lightMeter.begin();
}

void loop() {
  readWindSpeesSensor();
  read_DHT11_sensor();
  read_light_meter();

  delay(500);

}

void read_light_meter(void) {

  lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

}

void read_DHT11_sensor(void) {
    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
}


void readWindSpeesSensor() {
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
}

