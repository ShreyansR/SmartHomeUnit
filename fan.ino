const int buttonPin = 6; // the number of the pushbutton pin
const int motorIn1 = 9;
const int motorIn2 = 7;
int stat = 0; 

#define rank1 150
#define rank2 200
#define rank3 250
// Variables will change:
int buttonState; // the current reading from the input pin
int lastButtonState = LOW; // the previous reading from the input pin
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0; // the last time the output pin was toggled
long debounceDelay = 50; // the debounce time; increase if the output flickers

void setup() 
{
pinMode(motorIn1,OUTPUT);
pinMode(motorIn2,OUTPUT);
Serial.begin(9600);
}

  void loop() {
      // read the state of the switch into a local variable:
      int reading = digitalRead(buttonPin);
      // check to see if you just pressed the button 
      // (i.e. the input went from LOW to HIGH), and you've waited 
      // long enough since the last press to ignore any noise:
      // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
      // reset the debouncing timer
      lastDebounceTime = millis();
    }
if ((millis() - lastDebounceTime) > debounceDelay) {
// whatever the reading is at, it's been there for longer
// than the debounce delay, so take it as the actual current state:
// if the button state has changed:
if (reading != buttonState) {
buttonState = reading;

if (buttonState == HIGH) {

stat = stat + 1;
if(stat >= 4)
{
stat = 0;
}
}
}
}

switch(stat)
{
case 1:
clockwise(rank1);
break;
case 2:
clockwise(rank2);
break;
case 3:
clockwise(rank3);
break;
default:
clockwise(0);
}
lastButtonState = reading;
}

void clockwise(int Speed)//
{
analogWrite(motorIn1,0);
analogWrite(motorIn2,Speed);
}


