#include <SimpleTimer.h>
#include <SoftwareSerial.h>
SimpleTimer timer;
int xbeeTx = 10;
int xbeeRx = 11;
SoftwareSerial xbee(xbeeTx,xbeeRx);
//////////////////////////////////////////////////////////////////////////////
#include <DHT.h>
#define DHTPIN 8 // digital Pin sensor is connected to
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
float t;
float h;
///////////////////////////////////////////////////////////////////////////////
int LED1 = 3; //the digital pin we connect the LED for AC
int LED2 = 4; //the digital pin we connect the LED for Heater
int stateLED1 = LOW;
int stateLED2 = LOW;
int light = 5; //digital pin for lights 
int medicineNoti = 6;
int lightvalue = 1;
const int buzzer = 7; //buzzer to arduino pin 9
////////////////////////////////////////////////////////////////////////////////
int motiontimer = 0;
int pir = 13;
int flag=0;
void lightsoff()
{
  int val = digitalRead(pir);
//  Serial.print(val);
  if(val == 0)
    motiontimer += 25;
  else
    motiontimer = 0;

  if(motiontimer >= 10000)
  {
    digitalWrite(light, 0);//setting lights off
    lightvalue = 0;
  }
  if(lightvalue== 0 && val == 1){
    digitalWrite(light,1);
    lightvalue = 1;
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
  if(t < 29.0)
  {
    Serial.println("Turning On Heater");
    xbee.write("Turning On Heater");
    stateLED2 = HIGH ;
    if(stateLED1 == HIGH)
    {
      stateLED1 = LOW;
    }
    digitalWrite(LED1, stateLED1); 
    digitalWrite(LED2, stateLED2);
    Serial.println("Heater Turned On");
    xbee.write("Heater Turned On");
  }
  if(t >= 29.0 && t <= 30.0)
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
///////////////////////////////////////////////////////////////////////////////////////
void Noti()
{
  digitalWrite(medicineNoti, 1);
  tone(buzzer, 1000); // Send 1KHz sound signal... 
  delay(2000);  
  digitalWrite(medicineNoti, 0);
  noTone(buzzer);     // Stop sound...
}
////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  xbee.begin(9600);
  delay(100);
//////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(light, OUTPUT);
  pinMode(medicineNoti, OUTPUT);
  digitalWrite(light, 1);
//////////////////////////////////////////////////////////////////////////////////////////////
  dht.begin();
//////////////////////////////////////////////////////////////////////////////////////////////
  timer.setInterval(10, temphumi);
  timer.setInterval(7000, printTempHumi);
  timer.setInterval(25, lightsoff);
  timer.setInterval(12000, Noti);
}
void loop() 
{
  timer.run();
  if(xbee.available())
  {
      Serial.write(xbee.read());
  }
}

 
