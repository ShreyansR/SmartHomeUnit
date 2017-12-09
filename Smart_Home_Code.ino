#include <dht11.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

dht11 DHT11;

#define DHT11PIN 31
#define SS_PIN   53
#define RST_PIN   5
#define redPin   30
#define greenPin 29
#define bluePin  28
#define ledStrip 3      //LED STRIP OUTPUT
#define inputPin  2      //Input for motion sensor
#define rank1 150
#define rank2 200
#define rank3 250

Servo servo1;

int pos1 = 0;
int pos2 = 110;
int pirState = LOW;
int val = 0;
int buzzer2 = 11;

int  sensorPin    =    A0;     // select the input  pin for  the potentiometer 
int  sensorPin2   =    A1;
int  buzzer       =     9;   // select the pin for  the LED
int  laser        =    10;
int  laser2       =     8;
int  sensorValue  =     0;  // variable to  store  the value  coming  from  the sensor
int  sensorValue2 =     0;
int  countIn      =     0;
int  count        =     0;

const int buttonPin = 6; // the number of the pushbutton pin
const int motorIn1 = 9;
const int motorIn2 = 7;
int stat = 0; 

int buttonState; // the current reading from the input pin
int lastButtonState = LOW; // the previous reading from the input pin
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0; // the last time the output pin was toggled
long debounceDelay = 50; // the debounce time; increase if the output flickers

  
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  initializeLCD();
  initializePIR();
  
  servo1.attach(4); 

  initializeRGB();
  
  //Serial.begin(9600);   // Initiate a serial communication

  initializeRFID();
  
  initializeLaser();

  //initializeDC();
}

void loop() 
{
  thermostatToLCD();

  turnLaserOn();
  readPRValue();

  count = 1 - (laserLogic());
  
  Serial.print("The number of people in the house are: ");
  Serial.println(count);
  servo1.write(pos2);
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  val = digitalRead(inputPin);
  
  if (content.substring(1) == "B5 F9 E4 1D") //change here the UID of the card/cards that you want to give access
  {
    setColor(0, 255, 0);
    servo1.write(pos1);

    delay(3000);

    servo1.write(pos2);
    setColor(0, 0, 0);

   do{
      if(val == HIGH)
      analogWrite(ledStrip, 255);
      
      delay(500);
      val = digitalRead(inputPin);
      if(val == LOW)
      {
        analogWrite(ledStrip, 0);
      }
    }while(val == HIGH);
    
  }

  if (content.substring(1) == "B3 82 BE A9") //change here the UID of the card/cards that you want to give access
  {
    setColor(0, 255, 0);
    servo1.write(pos1);

    delay(3000);

    servo1.write(pos2);
    setColor(0, 0, 0);

   do{
      if(val == HIGH)
      analogWrite(ledStrip, 50);
      
      delay(500);
      val = digitalRead(inputPin);
      if(val == LOW)
      {
        analogWrite(ledStrip, 0);
      }
    }while(val == HIGH);
    
  }

  /*if (content.substring(1) != "B3 82 BE A9" && count < 0)
  {
    digitalWrite(buzzer2, HIGH);
    {
      tone(9, 3047, 400);
      delay(1000);
      noTone(8);
      delay(100);
    }
  }*/
 
  
 else   {
    ledStripOff();
    setColor(255,0, 0);
    delay(3000);
    setColor(0, 0, 0);
  }

  delay(500);
      val = digitalRead(inputPin);
  
  
} 

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}

void ledStripOn()
{
  analogWrite(ledStrip, 255);
}

void ledStripOff()
{
  analogWrite(ledStrip, 0);
}

void initializeLCD()
{
  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight
  lcd.backlight();
  lcd.print("Temperature:");
}

void initializeRGB()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void initializePIR()
{
  pinMode(inputPin, INPUT);    //Declare motion sensor as input
}

void initializeRFID()
{
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

void initializeLaser()
{
  pinMode(buzzer,OUTPUT);
  pinMode(laser, OUTPUT);
  pinMode(laser2, OUTPUT);
}

void thermostatToLCD()
{
  int chk = DHT11.read(DHT11PIN);

  Serial.print("Read sensor: ");
  switch (chk)
  {
    case 0: Serial.println("OK"); break;
    case -1: Serial.println("Checksum error"); break;
    case -2: Serial.println("Time out error"); break;
    default: Serial.println("Unknown error"); break;
  }
  
  lcd.print((float)DHT11.temperature, 0);
  Serial.print("Temperature (oC): ");
  Serial.println((float)DHT11.temperature, 2);
  lcd.print((char)223);
  lcd.print("C");

  
  lcd.setCursor(0, 1);

  lcd.print("Humidity:   ");
  lcd.print((float)DHT11.humidity, 0);
  lcd.print("%");
  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2); 
  lcd.print("    "); 

  delay(2000);
}

void turnLaserOn()
{
  digitalWrite(laser, HIGH);
  digitalWrite(laser2, HIGH);
}

void readPRValue()
{
  sensorValue =  analogRead(sensorPin);
  sensorValue2 = analogRead(sensorPin2);
}

int laserLogic()
{
  if (sensorValue >= 120)
  {
    //digitalWrite(buzzer, LOW);
    digitalWrite(buzzer, HIGH);
    {
      tone(9, 3047, 400);
      delay(1000);
      noTone(8);
      delay(100);
      countIn++;
      
      delay(1000);

      sensorValue2 = 500;
      delay(1000);
      
    }
  }
  
  if (sensorValue2 <= 20)
  {
    
    digitalWrite(buzzer, HIGH);
    {
      tone(9, 3047, 400);
      delay(1000);
      noTone(8);
      delay(100);
      countIn--;
      delay(1000);
    }
  }
  return countIn;
}




