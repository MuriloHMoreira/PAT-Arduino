/**
 * @file MultiThermometer.ino
 * Read temperatures from multiple Termopar Nanoshields, with different thermocouple types.
 * 
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>

int sensorPin = A0;
int sensorRead = 0;
float V_read = 0.0;

void setup()
{
  Serial.begin(9600);
  Serial.println("------------------");
  Serial.println(" Analog Input");
  Serial.println("------------------");
  Serial.println();
  

}

float convertVoltToVolFlow(float V_read) {
  float q_0 = 0;
  float q_max = 500;
  float q;
  float V_0 = 2.69;
  float V_max = 3.63;
  q = (V_read - V_0) / (V_max - V_0) * (q_max - q_0);
  return q;
}

void loop()
{

  Serial.print("Voltage [V]: ");
  sensorRead = analogRead(sensorPin);
  float V_read = sensorRead * (5.0 / 1023.0);
  Serial.print(V_read);
  Serial.print(", Mass Flow [mL/min]:");
  Serial.print(convertVoltToVolFlow(V_read));
  Serial.println();

  // Wait for next second
  delay(300);
}
