#include <Arduino.h>
#include <Servo.h>
#ifdef SOLOMOTOR
Servo servoMotor;
#endif

#ifdef SOLOMOTOR
void setup() {
  // put your setup code here, to run once:
  servoMotor.attach(9);
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  servoMotor.write(90);
  digitalWrite(13, HIGH);
  // Esperamos 2 segundos
  delay(2000);

  // Desplazamos a la posición 180º
  servoMotor.write(-90);
  digitalWrite(13, LOW);
	// Wait 1 second before continuing
	delay(1000);
}
#endif
