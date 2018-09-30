#include <SimpleTimer.h>
#include <SoftwareSerial.h>
SimpleTimer timer;
int xbeeTx = 10;
int xbeeRx = 11;
SoftwareSerial xbee(xbeeTx,xbeeRx);
///////////////////////////////////////////////////////////////////////////////
#define Threshold 600
int goingup = 0;
int goingdown = 0;
int pulse = 0;
int oldpulse = 0;
int Pulsepin = 0;         //analog pin
int heartbeat = 0;
int stateButton;
int pinButton=7;
////////////////////////////////////////////////////////////////////////////////
#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;
boolean ledState = false;
boolean freefallDetected = false;
int freefallBlinkCount = 0;
/////////////////////////////////////////////////////////////////////////////////
void doInt()
{
  freefallBlinkCount = 0;
  freefallDetected = true;  
}
void checkSettings()
{
  Serial.println();
  Serial.print(" * Sleep Mode:                ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  Serial.print(" * Motion Interrupt:     ");
  Serial.println(mpu.getIntMotionEnabled() ? "Enabled" : "Disabled");
  Serial.print(" * Zero Motion Interrupt:     ");
  Serial.println(mpu.getIntZeroMotionEnabled() ? "Enabled" : "Disabled");
  Serial.print(" * Free Fall Interrupt:       ");
  Serial.println(mpu.getIntFreeFallEnabled() ? "Enabled" : "Disabled");
  Serial.print(" * Free Fal Threshold:          ");
  Serial.println(mpu.getFreeFallDetectionThreshold());
  Serial.print(" * Free FallDuration:           ");
  Serial.println(mpu.getFreeFallDetectionDuration()); 
  Serial.print(" * Clock Source:              ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  } 
  Serial.print(" * Accelerometer:             ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  
  Serial.print(" * Accelerometer offsets:     ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());
  Serial.print(" * Accelerometer power delay: ");
  switch(mpu.getAccelPowerOnDelay())
  {
    case MPU6050_DELAY_3MS:            Serial.println("3ms"); break;
    case MPU6050_DELAY_2MS:            Serial.println("2ms"); break;
    case MPU6050_DELAY_1MS:            Serial.println("1ms"); break;
    case MPU6050_NO_DELAY:             Serial.println("0ms"); break;
  }   
  Serial.println();
}
void falldetect()
{
  Vector rawAccel = mpu.readRawAccel();
  Activites act = mpu.readActivites();
  if(act.isFreeFall)
  {
    Serial.println("Emergency!! your grandparent fell");
    xbee.write("Emergency!! your grandparent fell\n");
  }
  else
  {
    //Serial.println("Emergency!! your grandparent is enjoying life");
//    xbee.write("Emergency!! your grandparent is enjoying life\n");
  }
  if (freefallDetected)
  {
    ledState = !ledState;

    digitalWrite(4, ledState);

    freefallBlinkCount++;

    if (freefallBlinkCount == 20)
    {
      freefallDetected = false;
      ledState = false;
      digitalWrite(4, ledState);
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void printheartbeat()
{
  Serial.print("No. of Heart Beats are: ");
  Serial.println(heartbeat);
  xbee.write("No. of Heart Beats are: ");
  //xbee.write(heartbeat);
  int hb=heartbeat;
  int i=3;
  char c[3];
  for(i=3; i>0; i--){
    int a  = hb%10;
    hb = hb/10;
    c[i-1] = a+'0';
    
  }

  for(i=0; i<3; i++)
    xbee.write(c[i]);
  xbee.write("\n");
  if(heartbeat < 20)
  {
    Serial.println("Emergency!! Possible Heart Failure");
    xbee.write("Emergency!! Possible Heart Failure\n");
  }
  if(heartbeat > 100)
  {
    Serial.println("Emergency!! Possible Heart Attack");
    xbee.write("Emergency!! Possible Heart Attack\n");
  }
  heartbeat = 0;
}
void Cal_heartbeat()
{
  int val = analogRead(Pulsepin);
//  Serial.println(val);
  pulse = val;
  if(oldpulse < Threshold && pulse >= Threshold)
    goingup = 1;
  if(oldpulse >= Threshold && pulse < Threshold)
    goingdown = 1;
  if(goingup == 1 && goingdown == 1)
  {
    heartbeat++;
    goingup = 0;
    goingdown = 0;
  }
  oldpulse = val;
}
void emergency()
{
  stateButton = digitalRead(pinButton);
  if(stateButton == HIGH)
  {
    Serial.println("EMERGENCY!! YOUR GRANDPARENT IS NOT GOOD");
    xbee.write("EMERGENCY!! YOUR GRANDPARENT IS NOT GOOD");
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinButton, INPUT);
  xbee.begin(9600);
  delay(100);
  
//////////////////////////////////////////////////////////////////////////////////////////////
  Serial.println("Initialize MPU6050");
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);
  mpu.setIntFreeFallEnabled(true);
  mpu.setIntZeroMotionEnabled(false);
  mpu.setIntMotionEnabled(false);
  mpu.setDHPFMode(MPU6050_DHPF_5HZ);
  mpu.setFreeFallDetectionThreshold(17);
  mpu.setFreeFallDetectionDuration(2);  
  checkSettings();
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  attachInterrupt(0, doInt, RISING);
///////////////////////////////////////////////////////////////////////////////////////////////  
  timer.setInterval(3, Cal_heartbeat);
  timer.setInterval(60000, printheartbeat);
  timer.setInterval(100, falldetect);
//  timer.setInterval(100, emergency);
}
void loop() 
{
  timer.run();
//  if(xbee.available())
//  {
//    Serial.write(xbee.read());
//  }
//  if(Serial.available())
//  {
//    xbee.write(Serial.read());
//  }
}

 
