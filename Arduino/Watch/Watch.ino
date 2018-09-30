#include <SimpleTimer.h>
SimpleTimer timer;
//////////////////////////////////////////////////////////////////////////////
#include <DHT.h>
#define DHTPIN 8 // digital Pin sensor is connected to
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
float t;
float h;
///////////////////////////////////////////////////////////////////////////////
#define Threshold 600
int goingup = 0;
int goingdown = 0;
int pulse = 0;
int oldpulse = 0;
int Pulsepin = 0;         //analog pin
int heartbeat = 0;
///////////////////////////////////////////////////////////////////////////////
int LED1 = 4; //the digital pin we connect the LED for AC
int LED2 = 3; //the digital pin we connect the LED for Heater
int LED3 = 6; // For emergency fall detection
int timerIDforFall;
int stateLED1 = LOW;
int stateLED2 = LOW;
int light = 5; //digital pin for lights 
int pinButton = 7;
int stateButton;
int previous = LOW;
long time = 0;
long debounce = 200;
////////////////////////////////////////////////////////////////////////////////
int motiontimer = 0;
int pir = 13;
void lightsoff()
{
  int val = digitalRead(pir);
  if(val == 0)
    motiontimer += 25;
  else
    motiontimer = 0;

  if(motiontimer >= 1000)
    digitalWrite(light, 0);//setting lights off
}
////////////////////////////////////////////////////////////////////////////////
const int trigPin = 9; 
const int echoPin = 10;
long duration;
int distance;
void ultrasonic() 
{
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

Serial.print("Distance: ");
Serial.println(distance);
}
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
    digitalWrite(LED3, HIGH);
    Serial.println("Emergency!! your grandparent fell");
  }
  else
  {
    //Serial.println("Emergency!! your grandparent is enjoying life");
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
///////////////////////////////////////////////////////////////////////////////////
void printTempHumi()
{
  if(t > 30.0)
  {
    Serial.println("Turning On AC");
    stateLED1 = HIGH ;
    if(stateLED2 == HIGH)
    {
      stateLED2 = LOW;
    }
    digitalWrite(LED2, stateLED2); 
    digitalWrite(LED1, stateLED1);
    Serial.println("AC Turned On");
  }
  if(t < 28.0)
  {
    Serial.println("Turning On Heater");
    stateLED2 = HIGH ;
    if(stateLED1 == HIGH)
    {
      stateLED1 = LOW;
    }
    digitalWrite(LED1, stateLED1); 
    digitalWrite(LED2, stateLED2);
    Serial.println("Heater Turned On");
  }
  if(t >= 28.0 && t <= 30.0)
  {
    if(stateLED1 == HIGH)
    {
      Serial.println("Turning Off AC");
      stateLED1 = LOW;
      digitalWrite(LED1, stateLED1);
      Serial.println("AC Turned Off");
    }
    if(stateLED2 == HIGH)
    {
      Serial.println("Turning Off Heater");
      stateLED2 = LOW;
      digitalWrite(LED2, stateLED2);
      Serial.println("Heater Turned Off");
    }
  }
  Serial.print("temperature = "); 
  Serial.print(t);
  Serial.print("C  ");
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.println("%  ");
}
void temphumi()
{
  t = dht.readTemperature();
  h = dht.readHumidity();
  if (isnan(h) || isnan(t)) 
  {
    Serial1.println("Failed to read from DHT sensor!");
    return;
  }
}
////////////////////////////////////////////////////////////////////////////////////////
void turnoffHomeappliancesOKStatus()
{
  stateButton = digitalRead(pinButton);
  if(stateButton == HIGH && previous == LOW && millis() - time > debounce) 
  {
    if(stateLED1 == HIGH || stateLED2 == HIGH)
    {
      stateLED1 = LOW;
      stateLED2 = LOW; 
    } 
    time = millis();
  }
  digitalWrite(LED2, stateLED2);
  digitalWrite(LED1, stateLED1);
  digitalWrite(LED3, HIGH);
  previous == stateButton;
}
//////////////////////////////////////////////////////////////////////////////////////////
void printheartbeat()
{
  Serial.print("No. of Heart Beats are: ");
  Serial.println(heartbeat);
  if(heartbeat < 20)
  {
    Serial.println("Emergency!! Possible Heart Failure");
  }
  if(heartbeat > 100)
  {
    Serial.println("Emergency!! Possible Heart Attack");
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
  if(goingup == 1 && goingdown == 1){
    heartbeat++;
    goingup = 0;
    goingdown = 0;
  }
  oldpulse = val;

}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(pinButton, INPUT);
  pinMode(light, OUTPUT);
  digitalWrite(light, 1);
//////////////////////////////////////////////////////////////////////////////////////////////
  dht.begin();
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
  timer.setInterval(10, temphumi);
  timer.setInterval(7000, printTempHumi);
  timer.setInterval(100, falldetect);
  timer.setInterval(500, ultrasonic);
  timer.setInterval(25, lightsoff);
  timer.setInterval(25, lightsoff);
}
void loop() 
{
  timer.run();
  turnoffHomeappliancesOKStatus();
  turnoffHomeappliancesOKStatus();
}

 
