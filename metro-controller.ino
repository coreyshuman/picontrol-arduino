/* metro-controller.ino
 * 
 * 
 * Corey Shuman
 * 1/19/16
 */
#include "InputHandler.h"
#include <Adafruit_NeoPixel.h>
#include "PixelLED.h"


const long interval = 500;           // interval at which to blink (milliseconds)
const int statusLed =  13;      // the number of the LED pin
const int pixelPin = 2;         // the pin number for neopixel led

 
int firstSensor = 0;    // first analog sensor
int secondSensor = 0;   // second analog sensor
int thirdSensor = 0;    // digital sensor
int inByte = 0;         // incoming serial byte
int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated

int analogChannels[6];
int chan = 0;

InputHandler USBCom(10, 500);
PixelLED pixel;

void getChannel(String* arg, int argc)
{
  if(argc == 1) 
  {
    int chan = arg[0].toInt();
    if(chan >= 1 && chan <= 4)
    {
       Serial.println(analogChannels[chan-1]);   
    }
    else
    {
      Serial.println("Err: getChannel invalid chan");
    }
  }
  else
  {
    Serial.println("Err: getChannel invalid arg cnt");
  }
}

void getButton(String* arg, int argc)
{
  if(argc == 1) 
  {
    int chan = arg[0].toInt();
    if(chan >= 1 && chan <= 10)
    {
       Serial.println(digitalRead(chan+3));   
    }
    else
    {
      Serial.println("Err: getButton invalid chan");
    }
  }
  else
  {
    Serial.println("Err: getButton invalid arg cnt");
  }
}

void getAll(String* arg, int argc)
{
  byte buttons0_7 = 0;
  byte buttons8_15 = 0;
  //String output = "ga ";
  byte data[20];
  data[0] = 0x7E; // header
  data[1] = 0x00; // length
  data[2] = 0x0F; // length
  data[3] = 0x10; // frame type
  for(int i=0; i<6; i++) {
    //output += String(analogChannels[i]) + ",";
    data[4+i*2] = (analogChannels[i] >> 8) & 0xFF;
    data[5+i*2] = analogChannels[i] & 0xFF;
  }
  // 16 bytes
  
  buttons0_7 = digitalRead(3)&0x1 | (digitalRead(4)&0x1)<2 
              | (digitalRead(5)&0x1)<<3 | (digitalRead(6)&0x1)<<4
              | (digitalRead(7)&0x1)<<5 | (digitalRead(8)&0x1)<<5
              | (digitalRead(9)&0x1)<<6 | (digitalRead(10)&0x1)<<7;
  buttons8_15 = (digitalRead(11)&0x1) | (digitalRead(12)&0x1)<<1 ;

  //output += String(buttons0_7) + "," + String(buttons8_15);
  data[16] = buttons0_7;
  data[17] = buttons8_15;
  // checksum
  data[18] = CalcChecksum(data+3, 17-3);

  //Serial.println(output);
  Serial.write(data, 19);
}

void setPixel(String* arg, int argc)
{
  if(argc == 3)
  {
    int r = arg[0].toInt();
    int g = arg[1].toInt();
    int b = arg[2].toInt();
    pixel.SetPixel(r,g,b);
    Serial.println("sp ok");
  }
  else
  {
    Serial.println("Err: Requires 3 params.");
  }
}


void setup() {

  Serial.begin(115200);   // initialize serial com to master at 115200 bps:
  USBCom.Init(&Serial);   // init command parser
  // init pins
  pinMode(statusLed, OUTPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
 

  Serial.print("-Add Handler\n");
  // setup input handlers
  USBCom.AddHandler("gc", 1, getChannel);
  USBCom.AddHandler("gb", 1, getButton);
  USBCom.AddHandler("ga", 0, getAll);
  USBCom.AddHandler("sp", 3, setPixel);
  Serial.print("-Done Init\n");
  pixel.Init(pixelPin, 1);
}


void loop() {
  unsigned long currentMillis = millis();
  // keep track of time passed
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(statusLed, ledState);
    //Serial.print("abcdefg\n");
  }

  USBCom.Proc();
  pixel.Proc();

  switch(chan)
  {
     case 0:
      analogChannels[0] = analogRead(A0);
      break;
    case 1:
      analogChannels[1] = analogRead(A1);
      break;
    case 2:
      analogChannels[2] = analogRead(A2);
      break;
    case 3:
      analogChannels[3] = analogRead(A3);
      break;
  }

  if(++chan >= 4)
    chan = 0;
  
// if we get a valid byte, read analog ins:
/*
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    digitalWrite(9, LOW);
    // read first analog input:
    firstSensor = analogRead(A0);
    // read second analog input:
    secondSensor = analogRead(A1);
    // read  switch, map it to 0 or 255L
    thirdSensor = map(digitalRead(2), 0, 1, 0, 255);
    // send sensor values:
    Serial.print(firstSensor);
    Serial.print(",");
    Serial.print(secondSensor);
    Serial.print(",");
    Serial.println(thirdSensor);
    Serial.print("\n");
  }
  */
}

byte CalcChecksum(byte data[], int len) {
  byte cs = 0;
  int i;
  for(i=0; i<len; i++) {
    cs += data[i];
  }
  return 0xFF - cs;
}



