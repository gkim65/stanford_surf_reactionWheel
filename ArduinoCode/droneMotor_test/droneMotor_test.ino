#include <Servo.h>

byte servoPin1 = 9; // signal pin for the ESC.
byte servoPin2 = 10; // signal pin for the ESC.
Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo1.writeMicroseconds(1500); // send "stop" signal to ESC. Also necessary to arm the ESC.
  servo2.writeMicroseconds(1700); // send "stop" signal to ESC. Also necessary to arm the ESC.
  delay(7000); // delay to allow the ESC to recognize the stopped signal.
  Serial.println("--------------------------NEW SESSION------------------------");
}

void loop() {
  if(Serial.available()>0)
  {
    int val=Serial.read();
    Serial.println(val);
          
    if (val == 1){
      Serial.print("MOTOR SPEED_1");
      int pwmVal = map(val,0, 2, 1100, 1300); // maps potentiometer values to PWM value.
      servo1.writeMicroseconds(pwmVal); // Send signal to ESC.
      delay(2000);
    }      
    if (val == 2){
      //int pwmVal2 = map(motorSpeed,0, 500, 1300, 1500); // maps potentiometer values to PWM value.
      Serial.print("MOTOR SPEED_2");
      int pwmVal2 = map(40,0, 100, 1300, 1500); // maps potentiometer values to PWM value.
      Serial.print("Output");
      Serial.println(pwmVal2);
      servo2.writeMicroseconds(pwmVal2); // Send signal to ESC.
      delay(2000); 
      val = 0;
    }
  
  }
}
