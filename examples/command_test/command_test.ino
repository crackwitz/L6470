#include <SPI.h>
#include <L6470.h>

L6470 driver(10, 7, 6);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println(sizeof(struct abs_pos));
}

void loop() {
  // put your main code here, to run repeatedly:
 
}
