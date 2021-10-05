#include <Servo.h>

byte servoPin1 = 9; // signal pin for the ESC.
byte servoPin2 = 10; // signal pin for the ESC.
Servo servo1;
Servo servo2;

//Fritzing File: Serial to Serial Connection.fzz
int rec;
int Ack_tx = 2; // Acknowledgement tx.

const int ledPin =  LED_BUILTIN;// the number of the LED pin

int motorSpeed = 0;
int whichMotor = 200;

void setup() {
  Serial.begin(115200);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo1.writeMicroseconds(1500); // send "stop" signal to ESC. Also necessary to arm the ESC.
  servo2.writeMicroseconds(1500); // send "stop" signal to ESC. Also necessary to arm the ESC.
  delay(7000); // delay to allow the ESC to recognize the stopped signal.
  Serial.println("--------------------------NEW SESSION------------------------");
}

void serial(){

  
  while(Serial.available()<2)
  {
  }
  
  int val=Serial.read();
  int val2=Serial.read();
  digitalWrite(ledPin, HIGH);
  if ((val == 200) || (val == 201)){
    whichMotor = val;
    motorSpeed = val2;
    Serial.write(whichMotor);
    Serial.write(motorSpeed);
    
  }
  else{
    whichMotor = val2;
    motorSpeed = val;
    Serial.write(whichMotor);
    Serial.println(motorSpeed);
  }
  if (whichMotor == 200){
      int pwmVal = map(motorSpeed,0, 200, 1100, 1500); // maps potentiometer values to PWM value.
      Serial.print("MOTOR SPEED");
      Serial.println(pwmVal);
      servo1.writeMicroseconds(pwmVal); // Send signal to ESC.
      delay(1000);
    }
    if (whichMotor == 201){
      int pwmVal2 = map(motorSpeed,0, 200, 1300, 1350); // maps potentiometer values to PWM value.
      Serial.print("MOTOR SPEED");
      Serial.println(pwmVal2);
      servo2.writeMicroseconds(pwmVal2); // Send signal to ESC.
  
      delay(1000); 
    }
}

void serial2(){
  if(Serial.available()>0)
  {
      int val=Serial.read();
      if ((val != 0) && (val != 240)) {
        Serial.println(val);
      }
  }
}
void motors() {
  if (whichMotor == 200){
    int pwmVal = map(motorSpeed,0, 200, 1100, 1500); // maps potentiometer values to PWM value.
    Serial.print("MOTOR SPEED");
    Serial.println(pwmVal);
    servo1.writeMicroseconds(pwmVal); // Send signal to ESC.
    delay(1000);
  }
  if (whichMotor == 201){
    int pwmVal2 = map(motorSpeed,0, 200, 1300, 1350); // maps potentiometer values to PWM value.
    Serial.print("MOTOR SPEED");
    Serial.println(pwmVal2);
    servo2.writeMicroseconds(pwmVal2); // Send signal to ESC.

    delay(1000); 
  }
}

void loop() {
  serial();
}
