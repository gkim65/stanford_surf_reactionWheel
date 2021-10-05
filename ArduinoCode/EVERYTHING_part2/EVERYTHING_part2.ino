// Full orientation sensing using NXP/Madgwick/Mahony and a range of 9-DoF
// sensor sets.

#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>

Adafruit_Sensor *accelerometer, *gyroscope, *magnetometer;

#include "LSM6DS_LIS3MDL.h"  // can adjust to LSM6DS33, LSM6DS3U, LSM6DSOX...

// pick your filter! slower == better quality output
//Adafruit_NXPSensorFusion filter; // slowest
//Adafruit_Madgwick filter;  // faster than NXP
Adafruit_Mahony filter;  // fastest/smalleset

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM)
  Adafruit_Sensor_Calibration_EEPROM cal;
#else
  Adafruit_Sensor_Calibration_SDFat cal;
#endif

#define FILTER_UPDATE_RATE_HZ 100
#define PRINT_EVERY_N_UPDATES 10
//#define AHRS_DEBUG_OUTPUT

uint32_t timestamp;


// Input text in the form "123 456\n"
char instring[20]; // Room for 19 characters plus terminating null
int spos = 0; // Location in the array
int val1 = 0;
int val2 = 0;

#include <Servo.h>

byte servoPin1 = 9; // signal pin for the ESC.
byte servoPin2 = 10; // signal pin for the ESC.
Servo servo1;
Servo servo2;
int whichMotor = 0;
int motorSpeed = 0;



void setup() {
  Serial.begin(115200);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo1.write(40); // send "stop" signal to ESC. Also necessary to arm the ESC.
  servo2.write(40); // send "stop" signal to ESC. Also necessary to arm the ESC.
  delay(7000); // delay to allow the ESC to recognize the stopped signal.
  
  while (!Serial) yield();

  if (!cal.begin()) {
    Serial.println("Failed to initialize calibration helper");
  } else if (! cal.loadCalibration()) {
    Serial.println("No calibration loaded/found");
  }

  if (!init_sensors()) {
    Serial.println("Failed to find sensors");
    while (1) delay(10);
  }
  
  accelerometer->printSensorDetails();
  gyroscope->printSensorDetails();
  magnetometer->printSensorDetails();

  setup_sensors();
  filter.begin(FILTER_UPDATE_RATE_HZ);
  timestamp = millis();

  Wire.setClock(400000); // 400KHz

}

void serial(){
  if(Serial.available()>0)
  {
    int val=Serial.read();
    
    if (val == 2){
      
      Serial.println(val);
      Serial.print("MOTOR SPEED_1");
      int pwmVal = map(val,0, 50, 50, 130); // maps potentiometer values to PWM value.
      Serial.print("Output");
      Serial.println(pwmVal);
      servo1.write(45); // Send signal to ESC.
      delay(500);
      servo1.write(1);
      Serial.println(val);
      //int pwmVal2 = map(motorSpeed,0, 500, 1300, 1500); // maps potentiometer values to PWM value.
      Serial.print("MOTOR SPEED_2");
      int pwmVal2 = map(40,0, 100, 50, 130); // maps potentiometer values to PWM value.
      Serial.print("Output");
      Serial.println(pwmVal2);
      servo2.write(45); // Send signal to ESC.
      delay(500);
      servo2.write(1);
    }
    
  
  }
}

void serial_motor(){
  if(Serial1.available())
  {
      int val=Serial1.available();
      if ((val == 201) || (val == 200)) {
  
        Serial.print("YAS MOTORS");
        Serial.println(val, DEC);

      }
  }
}

void loop() {
  serial();
  serial();
  
  float roll, pitch, heading;
  float gx, gy, gz;
  static uint8_t counter = 0;

  if ((millis() - timestamp) < (1000 / FILTER_UPDATE_RATE_HZ)) {
    return;
  }
  timestamp = millis();
  // Read the motion sensors
  sensors_event_t accel, gyro, mag;
  accelerometer->getEvent(&accel);
  gyroscope->getEvent(&gyro);
  magnetometer->getEvent(&mag);

  cal.calibrate(mag);
  cal.calibrate(accel);
  cal.calibrate(gyro);
  // Gyroscope needs to be converted from Rad/s to Degree/s
  // the rest are not unit-important
  gx = gyro.gyro.x * SENSORS_RADS_TO_DPS;
  gy = gyro.gyro.y * SENSORS_RADS_TO_DPS;
  gz = gyro.gyro.z * SENSORS_RADS_TO_DPS;

  // Update the SensorFusion filter
  filter.update(gx, gy, gz, 
                accel.acceleration.x, accel.acceleration.y, accel.acceleration.z, 
                mag.magnetic.x, mag.magnetic.y, mag.magnetic.z);
  // only print the calculated output once in a while
  if (counter++ <= PRINT_EVERY_N_UPDATES) {
    return;
  }
  // reset the counter
  counter = 0;
  
  // print the heading, pitch and roll
  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();
  Serial.print("GyroZ:");
  Serial.print(gx);
  Serial.print(":OrientationZ:");
  Serial.println(roll);

  

}
