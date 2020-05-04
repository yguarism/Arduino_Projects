#include <LiquidCrystal.h>

#include <dht.h>

#include <QueueList.h>

#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

// Defines a Queue Object for storing Temp/Humidity Information
QueueList<double> Q;
//Arduino Pin Definitions for each Device
const int IRpin = 4; // IR
const int DHTpin = 3; // Temp Sensor
const int Fpin1 = 33; // Fan pin 1
const int Fpin2 = 31; // Fan pin 2 
const int rs = 12, en = 11, d4 = 5, d5 = 6, d6 = 7, d7 = 8; // LCD Display
const int SIZE = 3; 
IRrecv irrecv(IRpin);
decode_results res;
dht DHT;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variable Definitions to track measurement time
unsigned long startTime;
unsigned long endTime;
unsigned long duration;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(IRpin, INPUT);
  pinMode(Fpin1, OUTPUT);
  pinMode(Fpin2, OUTPUT);
  lcd.begin(16,2); // Setup LCD
}

int num = -2;

void loop() {
  lcd.setCursor(0,0);
  lcd.print("Temp & Humidity");
  if (irrecv.decode(&res)) {                
    num = DecodeIR(res.value); 
    irrecv.resume(); // Receive the next value
  }

  if(num >= 0) { // Check Valid IR Input
    lcd.clear();
    if(num == 1){ // Gives a new temp/hum measurement
      int chk = DHT.read11(DHTpin);      
      double temp = DHT.temperature;
      double hum = DHT.humidity;
      Q.push(temp, hum);
      

      lcd.setCursor(0,0);
      lcd.print("Temp: ");
      lcd.setCursor(0,1);
      lcd.print(temp);
      lcd.setCursor(7,0);
      lcd.print("Humid: ");
      lcd.setCursor(7,1);
      lcd.print(hum);
      
      startTime = millis();

    }
   if(num == 2){ // Gives you last 3 temperature measurements
    int cur = 0;
      int i = 1;
      lcd.clear();
     while(i <= SIZE) {
        int temp = Q.peek (i,1);   
        
        lcd.setCursor(cur,0);
        lcd.print("Tmp");
        lcd.print(i);
        lcd.setCursor(cur,1);
        if(temp == 0) lcd.print("N/A");
        else lcd.print(temp);  
        cur = cur + 6; 
        i++;   
        
     }  
    }

    if(num == 3){ // Gives you last three humidity measurements
    int cur = 0;
      int i = 1;
      lcd.clear();
     while(i <= SIZE) {
        int hum = Q.peek (i,2);   
    
        lcd.setCursor(cur,0);
        lcd.print("Hum");
        lcd.print(i);
        lcd.setCursor(cur,1);
        if(hum == 0) lcd.print("N/A");
        else lcd.print(hum);
        
        cur = cur + 6; 
        i++;     
     }
    }   

    if(num == 4) { // Tells you how long since last measurement
        endTime = millis();
        duration = endTime - startTime;
        lcd.setCursor(0,0);
        lcd.print ("Last Meas: ");
        lcd.setCursor(0,1);
        lcd.print(duration/60000);
        lcd.print(" min");
     }

    if(num == 5) {
      digitalWrite(Fpin1, HIGH);
      digitalWrite(Fpin2, LOW);
      lcd.setCursor(0,0);
      lcd.print ("Fan on 2 sec.");
      delay(2000);
      digitalWrite(Fpin1, LOW);
      digitalWrite(Fpin2, LOW);
    }
  }
  
  Q.print();
  delay(5000);
  lcd.clear();
  num = -2;
}

// Decodes IR Hex Reading to Button Pressed (Not Complete - Only Numbers)
int DecodeIR (int n) {
  int num = -2;
  switch(n){
    case 0xFF6897:  num = 0; break; // Button 1
    case 0xFF30CF:  num = 1; break; // Button 2
    case 0xFF18E7:  num = 2; break; // Button 3
    case 0xFF7A85:  num = 3; break; // Button 4
    case 0xFF10EF:  num = 4; break; // Button 5
    case 0xFF38C7:  num = 5; break; // Button 6
    case 0xFF5AA5:  num = 6; break; // Button 7
    case 0xFF42BD:  num = 7; break; // Button 8
    case 0xFF4AB5:  num = 8; break; // Button 9
    case 0xFF52AD:  num = 9; break; // Button 0 
  }

  return num;
}
