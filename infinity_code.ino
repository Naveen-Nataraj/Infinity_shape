#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>

MPU6050 mpu;

#define limit 0.1 // threshold for detecting infinity shaped path

float pitch[100]; // pitch values
float roll[100]; // roll values
float yaw[100]; // yaw values

int index = 0; // index of the current sample

void setup() {
  Wire.begin();
  Serial.begin(9600);
  mpu.initialize();
}

void loop() {
  // read sensor values
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // calculate pitch, roll, and yaw
  pitch[index] = atan2(ay, az);
  roll[index] = atan2(ax, ay);
  yaw[index] = atan2(ax, az);

  index = (index + 1) % 100;
  
  // check if the sensor is making an infinity shaped path
  if (isInfinity()) {
    Serial.println("Infinity shaped path detected!");
  }
  delay(100);
}

// function to check if the gyroscope is making an infinity shaped path
bool isInfinity() {
  // calculate curvature at each point in time
  // 100 samples of curvature is considered
  float curvature[100];
  
  for (int i = 0; i < 100; i++) {
    int prev = (i - 1 + 100) % 100;
    int next = (i + 1) % 100;
    curvature[i] = (yaw[next] - yaw[i]) - (yaw[i] - yaw[prev]);
  }
  // check if any point has a curvature above the threshold
  
  int flag=0;
  for (int i = 0; i < 100; i++) 
  {
    if (abs(curvature[i]) > limit) 
    {
      flag++;
    }
    if (abs(curvature[i]) < -(limit))
    {
      flag++;
    }    
    if (abs(curvature[i]) > limit) 
    {
      if(flag == 2)
      {
        return true;
      }
    }
  }
  return false;
}
