/*modified by Tania Winters 18/02/2021
created extra lcd.print lines to add more text
this program is to check the temperature and display with print lines. 
giving you a coloured screen that can be changed
was set at a pink display screen, 
I modified it from colorR = 250, colorG = 0 modified to 250, colorB = 250
gave it a blue screen by cahnging the middle
*
*
modified code for Major project 27/02/2021
*/

    
#include<Bridge.h>
#include <HttpClient.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <math.h>
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A0

int reading = 0;
int relay =7; //connected to 7D

//pir sensor
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;
//end of pir sensor


  /////////////////
 // MODIFY HERE //
/////////////////

// Add your PushingBox Scenario DeviceID here:
  char devid[] = "v2C9A47CF46EC811";
  
  //////////////
 //   End    //
//////////////

char serverName[] = "api.pushingbox.com";
boolean DEBUG = true;
//end of pushing box

//lcd display colour
rgb_lcd lcd;

const int colorR = 250; //colour on display is a blue tone
const int colorG = 250;
const int colorB = 250;

void setup() 
{
    // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  Serial.begin(9600);
  while (!Serial); // wait for a serial connection
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);
    
    // Print a message to the LCD.
    lcd.print(" Welcome");
   
    delay(1500);

     lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);
    
    lcd.print(" Lamcubator ");
    delay(1500);

   //pir motion sensor code - input and out put to be changed 
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
    
}

void loop() 
{

    pinMode(relay,OUTPUT); //relay output 

   int a = analogRead(pinTempSensor);
   float R = 1023.0/a-1.0;
    R = R0*R;

  
    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15/2;
    
    lcd.clear();
     int temperature1 = reading/2 ;
    lcd.setCursor(0, 2);
    lcd.print("Temp: ");
    lcd.print(temperature, DEC);
    lcd.print((char)223);
    lcd.print("C");                                           
    lcd.begin(16,2);
  
    //lcd.setCursor(0, 0);*/
 

  
  if (temperature <= 20) //if temperature is below/equal to 20 on 
  {
    digitalWrite(7, HIGH);
     lcd.print(" ON ");
     lcd.setCursor(0,0);
    
    lcd.setCursor(0, 2);
     lcd.print("Temp: "); //displayed on LCD
     lcd.print(temperature,DEC);
    lcd.print((char)223);
    lcd.print("C");
   
     
  } 
  else if (temperature > 20) //if temperature goes above 20 switch off 
  {
    digitalWrite(7, LOW);
    lcd.print("   OFF");
    
  }
  delay(1500);
   
  
  lcd.clear();
  
  

  //pir motion sensor code -addded major project 
     if(digitalRead(pirPin) == HIGH){
       digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW)
     {       
       digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(50);
           }
       }
         // Initialize the client library
  HttpClient client;

  //Setup sensorValue to read a value from Analog Port A0
  int sensorValue = analogRead(A0);
  
  //Testing value - when sensor is not connected - comment out when sketch is shown to be working - and take value from A0 instead
  //sensorValue=1500;

  // Make a HTTP request:  
  String APIRequest;
  APIRequest = String(serverName) + "/pushingbox?devid=" + String(devid)+ "&IDtag=100&TimeStamp=50&TempC="+sensorValue;
  client.get (APIRequest);
  
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();

  }
  Serial.flush();
  String UploadMessage;
  Serial.print("\n Uploaded temp value: ");
  Serial.print(sensorValue);
  delay(5000);
       
}
