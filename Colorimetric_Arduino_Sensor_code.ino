
/* * * * * * * * * * * * * * *\
 *    Calorimeter program    *
 *     Arduino Software      *
 *       Jan 08 2018         *
 *       Written by          *
 *       M Alghazwi          *
 *       m.ghazwi@gmail.com  *  
 *   http://mghazwi.com      *
\* * * * * * * * * * * * * * */

//The code uses the following libraries
#include <SoftwareSerial.h>
#include <math.h>
//Nextion library for the use of Nextion screen
//Library Available at https://github.com/bborncr/nextion
#include <Nextion.h>

SoftwareSerial nextion(10, 11);// Nextion TX to pin 10 and RX to pin 11 of Arduino

// measured and equation values (change equation values with your calibrated values)
int measured = 0;
// i0 is the value of picric acid + water as baseline to be used in Lambert-Beer law https://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law
// = -log10(i1/i0)
int i0 = 333;  // change this with your baseline value eg. water
// y = a*x + b
double a = 18.9201;  // change this with your calculated a
double b = 0.1183;   // change this with your calculated b

Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps

// pin 12 and 13 used for flashing LEDs 
int absorbanceLed   = 12;
int flucoLed = 13;
// Sensor attached on pin A0
int photodiodeInput = A0;

void setup() {

  // set LEDs as output
  pinMode(absorbanceLed,   OUTPUT);
  pinMode(flucoLed, OUTPUT);
  
  // turn them off by default
  digitalWrite(absorbanceLed,   LOW);
  digitalWrite(flucoLed,   LOW);
  // set photodiodeInput as input
  pinMode(photodiodeInput, INPUT);
  
  Serial.begin(9600);
  myNextion.init();

}


void caloriMeasurement(){
  
  // turn on LED
  digitalWrite(absorbanceLed, HIGH);
  digitalWrite(flucoLed,   LOW);
  //reset stopwatch
  int m = 0;
  int n = 0;
  int g = 0;
  int fin ;
  String f;
  for (int r = 0; r <= 10; r++){
    digitalWrite(absorbanceLed, HIGH);
    delay(1000);
    n = analogRead(photodiodeInput);
    m= m + n;
    if (r == 10){
      fin = m/r;
      Serial.println(fin);
      f = String(fin);
      measured = fin; // global value stored to be calibrated
      myNextion.setComponentText("t2", f);
    }
    digitalWrite(absorbanceLed, LOW);
    delay(500);
  }
  digitalWrite(absorbanceLed, LOW);
  digitalWrite(flucoLed,   LOW);
  
}

// convert measured value to creatinine concentration based on equation values
void calibrate() {
  double l;
  double y;
  double n;
  String m;
  n = (double)measured/(double)i0;
  l = -log10(n); // applying Lambert-Beer law
  y = a*l+b; // applying equation
  Serial.println(y);
  m = String(y);
  m = m + "mg/L";
  myNextion.setComponentText("t2", m);
}

/*
Please note that message value changes so please adjust it to the
message value shown on the serial monitor 
for example in our set up:
button 1 sends message ->  "65 0 2 0 ffff ffff ffff"
button 2 sends message ->  "65 0 4 0 ffff ffff ffff"
*/
void loop() {
  String message = myNextion.listen(); //check for message
  if(message != ""){ // if a message is received...
    Serial.println(message); //...print it out
  }
  if (message == "65 0 2 0 ffff ffff ffff") {
    myNextion.setComponentText("t2", "Processing...");
    caloriMeasurement();
  }
  if (message == "65 0 4 0 ffff ffff ffff") {
    //myNextion.buttonToggle(button2State, "b0", 0, 2);
    myNextion.setComponentText("t2", "Processing...");
    calibrate();
  }

  
}
