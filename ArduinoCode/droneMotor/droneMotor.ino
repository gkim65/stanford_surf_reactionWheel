#include <Servo.h>//Using servo library to control ESC

Servo Motor1; //Creating a servo motor 1
int Motor1Pin = 9;

Servo Motor2; //Creating a servo motor 2
int Motor2Pin = 8;

int Min = 1000;
int Max = 2000;

int PercentDelay = 1000;
int RampDelay = 100;
int Throttle = 0;
int Throttle2 = 0;
int MotorThrottle = 0;
int MotorThrottle2 = 0;
int EndDelay = 1000;

int val; //Creating a variable val 
void setup()
{
  Motor1.attach(Motor1Pin, Min, Max); //Specify the esc signal pin,Here as D8 
  Motor1.write(180);
  Motor1.write(0);
  Motor2.attach(Motor2Pin, Min, Max); //Specify the esc signal pin,Here as D8 
  Motor2.write(0);
  Motor2.write(180);
  Serial.begin(9600);
}

void loop()
{
  if (Throttle < 180)
  {
    Throttle = Throttle + 25;
    MotorThrottle = Motor1.read();
    while(MotorThrottle != Throttle)
    {
      Motor1.write(MotorThrottle + 1);
      MotorThrottle = Motor1.read();
      delay(RampDelay);
    }
    delay(PercentDelay);
  }
  else
  {
    Throttle = 0;
    MotorThrottle = Motor1.read();
    while(MotorThrottle != Throttle)
    {
      Motor1.write(MotorThrottle - 1);
      MotorThrottle = Motor1.read();
      delay(RampDelay);
    }
    delay(EndDelay);
  }

}
