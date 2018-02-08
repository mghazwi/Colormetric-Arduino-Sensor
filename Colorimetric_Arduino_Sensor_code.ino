/*
Ricardo Mena C
ricardo@crcibernetica.com
http://crcibernetica.com
  This example code is in public domain
*/

#include <SoftwareSerial.h>
#include <Nextion.h>
#include <math.h> 

SoftwareSerial nextion(10, 11);// Nextion TX to pin 2 and RX to pin 3 of Arduino

// measured and equation values (change equation values with your calibrated values)
int measured = 0;
// i0 is the value of picric acid + water as baseline to be used in Lambert-Beer law
// = -log10(i1/i0)
int i0 = 333; 
// y = a*x + b
double a = 18.9201;
double b = 0.1183;

Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps
boolean button1State;
boolean button2State;

int absorbanceLed   = 12;
int flucoLed = 13;

int photodiodeInput = A0;

// Calorimeter threshold before the timer is stopped.
int threshold = 100;

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
  
  // announce
  //Serial.println("Performing a absorbance measurement, press 5 to stop the time manual");
  //Serial.println("Measurement will take around 10-15 secconds, press 5 to stop the time manual");
  
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
  Serial.println(n);
  l = -log10(n); // applying Lambert-Beer law
  Serial.println(l);
  y = a*l+b; // applying equation
  Serial.println(y);
  m = String(y);
  m = m + "mg/L";
  myNextion.setComponentText("t2", m);
}

void loop() {
  String message = myNextion.listen(); //check for message
  if(message != ""){ // if a message is received...
    Serial.println(message); //...print it out
  }
  if (message == "65 0 2 0 ffff ffff ffff") {
   // myNextion.buttonToggle(button1State, "b0", 0, 1);
    myNextion.setComponentText("t2", "Processing...");
    caloriMeasurement();
    //myNextion.setComponentText("t2", "1 pressed");
  }
  if (message == "65 0 4 0 ffff ffff ffff") {
    //myNextion.buttonToggle(button2State, "b0", 0, 2);
    myNextion.setComponentText("t2", "Processing...");
    calibrate();
    //myNextion.setComponentText("t2", "2 pressed");
  }

  
}
